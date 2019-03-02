/*
*-------------------------------------------------------------------
* AudioDecoding.cpp
* Copyright(c) 2019, The RgbVideoEditor Author (Shunguang Wu).
* All Rights Reserved.
* You may not use this file except in compliance with the License:
*      GNU GENERAL PUBLIC LICENSE (Version 3), 29 June 2007
*
* You may obtain a copy of the License at
*
*     https://www.gnu.org/licenses/gpl-3.0.en.html
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*-------------------------------------------------------------------
*/
#include "AudioDecoding.h"

using namespace std;
using namespace autocut;

AudioDecoding::AudioDecoding()
{
}

AudioDecoding::~AudioDecoding()
{
}

int AudioDecoding::decode_audio(const std::string &inputFile, const std::string &outputFile, AudioDecCfg &params)
{
std::vector<uint8> raw;
int ret = decode_audio(inputFile, raw, params);
if (ret < 0) {
xLog("AudioDecoding::decode_audio(): cannot ope file : %s", outputFile.c_str() );
return -1;
}

FILE *fp = fopen(outputFile.c_str(), "wb");
if (!fp) {
}
fwrite(raw.data(), raw.size(), 1, fp);
fclose(fp);

return 0;
}

int AudioDecoding::decode_audio(const std::string &inputFile, std::vector<uint8> &vNonPlanarRaw, AudioDecCfg &params)
{
const char *infilename;
const AVCodec *codec;
AVCodecContext *c = NULL;
AVCodecParserContext *parser = NULL;
int len, ret;
FILE *fp;
uint8_t inbuf[AUDIO_INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
uint8_t *data;
size_t   data_size;
AVPacket *pkt;
AVFrame *decoded_frame = NULL;

infilename = inputFile.c_str();

/* register all the codecs */
avcodec_register_all();

pkt = av_packet_alloc();

/* find the MPEG audio decoder */
codec = avcodec_find_decoder(AV_CODEC_ID_MP3);
if (!codec) {
xLog( "Codec not found");
return -1;
}

parser = av_parser_init(codec->id);
if (!parser) {
xLog( "Parser not found");
return -1;
}

c = avcodec_alloc_context3(codec);
if (!c) {
xLog( "Could not allocate audio codec context");
return -1;
}

/* open it */
if (avcodec_open2(c, codec, NULL) < 0) {
xLog( "Could not open codec");
return -1;
}

fp = fopen(infilename, "rb");
if (!fp) {
xLog( "Could not open %s\n", infilename);
return -1;
}

/* decode until eof */
data = inbuf;
data_size = fread(inbuf, 1, AUDIO_INBUF_SIZE, fp);

vNonPlanarRaw.clear();
while (data_size > 0) {
if (!decoded_frame) {
if (!(decoded_frame = av_frame_alloc())) {
xLog( "Could not allocate audio frame");
return -1;
}
}

ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size, data, data_size, AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0);
if (ret < 0) {
xLog( "Error while parsing");
return -1;
}
data += ret;
data_size -= ret;

if (pkt->size) {
decode(c, pkt, decoded_frame, vNonPlanarRaw);
}

if (data_size < AUDIO_REFILL_THRESH) {
memmove(inbuf, data, data_size);
data = inbuf;
len = fread(data + data_size, 1, AUDIO_INBUF_SIZE - data_size, fp);
if (len > 0)
data_size += len;
}
}

/* flush the decoder */
pkt->data = NULL;
pkt->size = 0;
decode(c, pkt, decoded_frame, vNonPlanarRaw);
fclose(fp);

params.codecId = c->codec_id;
params.smpFmtOriginal = c->sample_fmt;
params.smpRate = c->sample_rate;
params.nBytesPerSmp = av_get_bytes_per_sample(c->sample_fmt);
params.nChannels = c->channels;
params.nSmps = vNonPlanarRaw.size() /( params.nChannels * params.nBytesPerSmp );

//<outFmt> is decide by decode()
switch (params.smpFmtOriginal) {
case AV_SAMPLE_FMT_U8:
case AV_SAMPLE_FMT_U8P:
params.smpFmtOutput = AV_SAMPLE_FMT_U8;
break;
case AV_SAMPLE_FMT_S16:
case AV_SAMPLE_FMT_S16P:
params.smpFmtOutput = AV_SAMPLE_FMT_S16;
break;
case AV_SAMPLE_FMT_S32:
case AV_SAMPLE_FMT_S32P:
params.smpFmtOutput = AV_SAMPLE_FMT_S32;
break;
case AV_SAMPLE_FMT_FLT:
case AV_SAMPLE_FMT_FLTP:
params.smpFmtOutput = AV_SAMPLE_FMT_FLT;
break;
case AV_SAMPLE_FMT_DBL:
case AV_SAMPLE_FMT_DBLP:
params.smpFmtOutput = AV_SAMPLE_FMT_DBL;
break;
case AV_SAMPLE_FMT_S64:
case AV_SAMPLE_FMT_S64P:
params.smpFmtOutput = AV_SAMPLE_FMT_S64;
break;
}
avcodec_free_context(&c);
av_parser_close(parser);
av_frame_free(&decoded_frame);
av_packet_free(&pkt);
return 0;
}

//m_vMp3Frms
//read mp3 file into <raw> and transfer it into <m_vMp3Frms> based on <m_outpAudioStreamInfo>
int AudioDecoding::decode_audio(const std::string &audioFile, const MyAudioStreamInfo outAudioStreamInfo, std::vector<RawFrmPtr> &outFrms, AudioDecCfg &paramsInAudioFile)
{
outFrms.clear();
assert(outAudioStreamInfo.channels == 2);
assert(outAudioStreamInfo.sample_fmt == AV_SAMPLE_FMT_FLT || outAudioStreamInfo.sample_fmt == AV_SAMPLE_FMT_FLTP);

std::vector<uint8>	raw;
//raw is non-plannar
//raw order = [LLRRLLRRLLRR....] for nSmpBytes=2, m_mp3Channels=2;
//raw order = [LLMMRRLLMMRRLLMMRR....] for nSmpBytes=2, m_mp3Channels=3;
//raw order = [LLLLRRRRLLLLRRRRLLLLRRRR....] for nSmpBytes=4, m_mp3Channels=2;
//raw order = [LLLLMMMMRRRRLLLLMMMMRRRRLLLLMMMMRRRR....] for nSmpBytes=4, m_mp3Channels=2;
// ...

//1. doceoding to memory
AudioDecoding x;
int ret = decode_audio(audioFile.c_str(), raw, paramsInAudioFile);
bool mp3Exist = (raw.size() > 0);
#if 0
FILE *fp = fopen("C:/temp/mp3.raw", "wb");
fwrite(raw.data(), 1, raw.size(), fp);
fclose(fp);
#endif
if (!mp3Exist) {
return -1;
}

//2 convert to doceoding to <AV_SAMPLE_FMT_FLT>
assert(paramsInAudioFile.nChannels == 2);
std::vector<uint8> desRaw;
enum AVSampleFormat desFmt = AV_SAMPLE_FMT_FLT;
int desChannels = outAudioStreamInfo.channels;
ret = audio_convert_nonPlanar2nonPlanar(desRaw, desFmt, desChannels, outAudioStreamInfo.sample_rate,
raw, paramsInAudioFile.smpFmtOutput, paramsInAudioFile.nChannels, paramsInAudioFile.smpRate);

//3. cut into  <outFrms> 
uint32 nSmpsPerChannel = desRaw.size() / (desChannels* av_get_bytes_per_sample(desFmt));
uint32 nSmpsPerFrm = outAudioStreamInfo.nb_samples_perfrm;
uint32 nFrms = nSmpsPerChannel / nSmpsPerFrm;
uint32 nBytesPerFrm = nSmpsPerFrm * outAudioStreamInfo.channels * av_get_bytes_per_sample(outAudioStreamInfo.sample_fmt);

if (outAudioStreamInfo.sample_fmt == AV_SAMPLE_FMT_FLT) {
//from AV_SAMPLE_FMT_FLT to AV_SAMPLE_FMT_FLT
const uint8 *p0 = desRaw.data();
for (uint32 i = 0; i < nFrms; ++i) {
RawFrmPtr frm( new RawFrm(FrmType_A_FRM, 0, nBytesPerFrm) );
frm->copyFrom(p0, nBytesPerFrm, i);
outFrms.push_back(frm);
p0 += nBytesPerFrm;
}
}
else if (outAudioStreamInfo.sample_fmt == AV_SAMPLE_FMT_FLTP) {
//from AV_SAMPLE_FMT_FLT to AV_SAMPLE_FMT_FLTP
vector<float32> v(desChannels * nSmpsPerFrm);
const float32 *s0 = (const float32*)desRaw.data();   //ch0
const float32 *s1 = s0 + 1;                          //ch1
for (uint32 i = 0; i < nFrms; ++i) {
float32 *d0 = v.data();
float32 *d1 = d0 + nSmpsPerFrm;
for (uint32 j = 0; j < nSmpsPerFrm; ++j) {
*d0++ = *s0;
*d1++ = *s1;

s0 += desChannels;
s1 += desChannels;
}
RawFrmPtr frm(new RawFrm(FrmType_A_FRM, 0, nBytesPerFrm));
frm->copyFrom((uint8*)v.data(), nBytesPerFrm, i);
outFrms.push_back(frm);
}
}

return 0;
}

int AudioDecoding::decode(AVCodecContext *dec_ctx, AVPacket *pkt, AVFrame *frame, FILE *outfile)
{
int i, ch;
int ret, data_size;

/* send the packet with the compressed data to the decoder */
ret = avcodec_send_packet(dec_ctx, pkt);

if (ret < 0) {
xLog( "Error submitting the packet to the decoder");
return -1;
}
/* read all the output frames (in general there may be any number of them */
while (ret >= 0) {
ret = avcodec_receive_frame(dec_ctx, frame);
if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
return -1;
else if (ret < 0) {
xLog( "Error during decoding");
return -1;
}
data_size = av_get_bytes_per_sample(dec_ctx->sample_fmt);   //data_size=2 for AV_SAMPLE_FMT_S16P
if (data_size < 0) {
/* This should not occur, checking just for paranoia */
xLog( "Failed to calculate data size");
return -1;
}
for (i = 0; i < frame->nb_samples; i++)
for (ch = 0; ch < dec_ctx->channels; ch++)
fwrite(frame->data[ch] + data_size * i, 1, data_size, outfile);
}
return 0;
}

int AudioDecoding::decode(AVCodecContext *dec_ctx, AVPacket *pkt, AVFrame *frame,  std::vector<uint8> &out )
{
int i, j, ch;
int ret, data_size;

/* send the packet with the compressed data to the decoder */
ret = avcodec_send_packet(dec_ctx, pkt);
if (ret < 0) {
xLog( "Error submitting the packet to the decoder");
return -1;
}
/* read all the output frames (in general there may be any number of them */
while (ret >= 0) {
ret = avcodec_receive_frame(dec_ctx, frame);
if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
return -1;
else if (ret < 0) {
xLog( "Error during decoding");
return -1;
}
data_size = av_get_bytes_per_sample(dec_ctx->sample_fmt);
if (data_size < 0) {
/* This should not occur, checking just for paranoia */
xLog( "Failed to calculate data size");
return -1;
}

//store to none-planar format [L_smp,R_smp,L_smp,R_smp...], each smp has <data_size> of bytes
for (i = 0; i < frame->nb_samples; i++) {
for (ch = 0; ch < dec_ctx->channels; ch++) {
uint8 *p0 = frame->data[ch] + data_size * i;
for (j = 0; j < data_size; ++j) {
out.push_back( *p0++ );
}
}
}
}
return 0;
}

int autocut::audio_convert_nonPlanar2nonPlanar(std::vector<uint8> &desRaw, const enum AVSampleFormat desFmt, const int desChannels, const int desSmpRate,
const std::vector<uint8> &srcRaw, const enum AVSampleFormat srcFmt, const int srcChannels, const int srcSmpRate )
{
//todo: change <out_samples> accordingly to handel different <desFmt>
assert( 0 == av_sample_fmt_is_planar(desFmt));
assert( 0 == av_sample_fmt_is_planar(srcFmt) );
//assert( desChannels == 2 );
//assert( srcChannels == 2 );

SwrContext      *swr = swr_alloc();
av_opt_set_int(swr, "in_channel_layout", AV_CH_LAYOUT_STEREO, 0);
av_opt_set_int(swr, "out_channel_layout", AV_CH_LAYOUT_STEREO, 0);
av_opt_set_int(swr, "in_sample_rate", srcSmpRate, 0);
av_opt_set_int(swr, "out_sample_rate", desSmpRate, 0);
av_opt_set_sample_fmt(swr, "in_sample_fmt", srcFmt, 0);
av_opt_set_sample_fmt(swr, "out_sample_fmt", desFmt, 0);
swr_init(swr);

int smpSize = av_get_bytes_per_sample(srcFmt);
const uint8_t *in_samples = srcRaw.data();
int		in_num_samples = srcRaw.size() / (srcChannels*smpSize);
int     out_num_samples = in_num_samples;

uint8_t *out_samples;
int outSmpSz =  av_samples_alloc(&out_samples, NULL, desChannels, out_num_samples, desFmt, 0);
out_num_samples = swr_convert(swr, &out_samples, out_num_samples, &in_samples, in_num_samples);

desRaw.clear();
if (out_num_samples < 0) {
xLog("audio_convert(): fail!");
}
else {
smpSize  = av_get_bytes_per_sample(desFmt);
uint32_t outBytes = out_num_samples * desChannels * smpSize;
desRaw.resize(outBytes);
memcpy_s(desRaw.data(), outBytes, out_samples, outSmpSz);
}
av_freep(out_samples);
swr_free( &swr );
return 0;
}

int autocut::audio_convert_nonPlanar2planar(std::vector<std::vector<uint8>> &des, const std::vector<uint8> &src, const int nChannels, const int nBytes4Smp)
{
uint64_t nSmps4EachCh = src.size() / (nBytes4Smp*nChannels);
std::vector<uint8> v(nSmps4EachCh*nBytes4Smp);
for (int i = 0; i < nChannels; ++i) {
des.push_back(v);
}

for (int i = 0; i < nChannels; ++i) {
const uint8 *p0 = src.data() + i*nBytes4Smp;
uint8 *p1 = des[i].data();
for (uint64_t j = 0; j < nSmps4EachCh; ++j) {
for (int k = 0; k < nBytes4Smp; ++k) {
*p1++ = *p0++;
}
p0 += nBytes4Smp;
}
}

return 0;
}
