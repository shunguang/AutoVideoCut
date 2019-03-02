/*
*-------------------------------------------------------------------
* AudioEncoding.cpp
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
#include "AudioEncoding.h"
#include "AudioDecoding.h"  //for audio_convert_nonPlanar2planar()

using namespace std;
using namespace autocut;

AudioEncoding::AudioEncoding(const AudioEncCfg *p)
: m_ctx (NULL)
, m_frame(NULL)
, m_pkt(NULL)
, m_isInit(false)
{
if (p) {
resetCodec(*p);
}
}

AudioEncoding::~AudioEncoding()
{
freeCodec();
}

void AudioEncoding::freeCodec()
{
avcodec_free_context(&m_ctx);
av_frame_free(&m_frame);
av_packet_free(&m_pkt);

m_ctx = NULL;
m_frame = NULL;
m_pkt = NULL;
m_isInit = false;
}

bool AudioEncoding::resetCodec(const AudioEncCfg &cfg)
{
const AVCodec *codec;

if (m_ctx) {
freeCodec();
}

m_isInit = false;
/* register all the codecs */
avcodec_register_all();

/* find the MP2 encoder */
codec = avcodec_find_encoder(cfg.codecId);
if (!codec) {
xLog("AudioEncoding::resetCodec(): Codec not found");
return false;
}

m_ctx = avcodec_alloc_context3(codec);
if (!m_ctx) {
xLog("AudioEncoding::resetCodec(): Could not allocate audio codec context");
return false;
}

/* put sample parameters */
m_ctx->bit_rate = cfg.bitRate;

/* check that the encoder supports s16 pcm input */
m_ctx->sample_fmt = cfg.smpFmt;
if (!check_sample_fmt(codec, m_ctx->sample_fmt)) {
xLog("AudioEncoding::resetCodec(): Encoder does not support sample format %s", av_get_sample_fmt_name(m_ctx->sample_fmt));
return false;
}

/* select other audio parameters supported by the encoder */
m_ctx->sample_rate = select_sample_rate(codec);
m_ctx->channel_layout = select_channel_layout(codec);
m_ctx->channels = av_get_channel_layout_nb_channels(m_ctx->channel_layout);

/* open it */
//AVDictionary *opt = NULL;
//av_dict_set(&opt, "frame_size", "1024", 0);
if (avcodec_open2(m_ctx, codec, NULL /*&opt*/) < 0) {
xLog("AudioEncoding::resetCodec(): Could not open codec");
return false;
}

/* packet for holding encoded output */
m_pkt = av_packet_alloc();
if (!m_pkt) {
xLog("AudioEncoding::resetCodec(): could not allocate the packet");
return false;
}

/* frame containing input raw audio */
m_frame = av_frame_alloc();
if (!m_frame) {
xLog("AudioEncoding::resetCodec(): Could not allocate audio frame");
return -1;
}

m_frame->nb_samples = m_ctx->frame_size;
m_frame->format = m_ctx->sample_fmt;
m_frame->channel_layout = m_ctx->channel_layout;

/* allocate the data buffers */
int ret = av_frame_get_buffer(m_frame, 0);
if (ret < 0) {
xLog("AudioEncoding::encode_audio(): Could not allocate audio data buffers");
return -1;
}

m_isInit = true;
return true;
}

//todo: current <raw> is not used
int AudioEncoding::encode_audio(const std::vector<uint8> &nonPlanarRaw,  const std::string &outFile)
{
if (!m_isInit) {
xLog("AudioEncoding::encode_audio(): Codec was not init!");
return -1;
}

int ret;
FILE *f;
f = fopen(outFile.c_str(), "wb");
if (!f) {
xLog( "AudioEncoding::encode_audio(): Could not open %s", outFile.c_str());
return -1;
}

int			nChs = m_ctx->channels;
int			nBytesPerSmp = av_get_bytes_per_sample( m_ctx->sample_fmt );
int			nSmpsPerFrm = m_ctx->frame_size;
uint32_t	nBytesPerFrm = nSmpsPerFrm * nChs * nBytesPerSmp;
int64_t		nFrms = nonPlanarRaw.size() / nBytesPerFrm;

if ( av_sample_fmt_is_planar(m_ctx->sample_fmt) ) {
//for plannar codec
std::vector<std::vector<uint8>> vv;
ret = audio_convert_nonPlanar2planar( vv, nonPlanarRaw, nChs, nBytesPerSmp);
uint8_t *p0[16];
for (int j = 0; j < nChs; ++j) {
p0[j] = vv[j].data();
}
nBytesPerFrm /= nChs;
for (int64_t i = 0; i < nFrms; i++) {
ret = av_frame_make_writable(m_frame);
if (ret < 0) {
xLog("AudioEncoding::encode_audio(): Could not run av_frame_make_writable()!");
return -1;
}
for (int j = 0; j < nChs; ++j) {
uint8_t *pSamples = m_frame->data[j];
memcpy_s(pSamples, nBytesPerFrm, p0[j], nBytesPerFrm);
p0[j] += nBytesPerFrm;
}
encode(m_ctx, m_frame, m_pkt, f);
}
}
else {
//for non-plannar codec
const uint8_t *p0 = nonPlanarRaw.data();
for (int64_t i = 0; i < nFrms; i++) {
/* make sure the frame is writable -- makes a copy if the encoder
* kept a reference internally */
ret = av_frame_make_writable(m_frame);
if (ret < 0) {
xLog("AudioEncoding::encode_audio(): Could not run av_frame_make_writable()!");
return -1;
}
uint8_t *samples = m_frame->data[0];
memcpy_s(samples, nBytesPerFrm, p0, nBytesPerFrm);
encode(m_ctx, m_frame, m_pkt, f);
p0 += nBytesPerFrm;
}
}
/* flush the encoder */
encode(m_ctx, NULL, m_pkt, f);

fclose(f);

return 0;
}

int AudioEncoding::encode_audio_planarFrms(const std::vector<RawFrmPtr> &vPlanarFrms, const int nChs, const std::string &outFile)
{
assert( av_sample_fmt_is_planar(m_ctx->sample_fmt) );
int64_t nFrms = vPlanarFrms.size();
if (nFrms <= 0) {
xLog("AudioEncoding::encode_audio_planarFrms(): <vPlanarFrms> is empty!");
return -1;
}
uint64 frmSzInBytes = getFrmSize();
uint64 bufSz = vPlanarFrms[0]->sz_;
if (frmSzInBytes != bufSz) {
xLog("AudioEncoding::encode_audio_planarFrms(): frm size does not match, frmSzInBytes=%d, bufSz=%d", frmSzInBytes, bufSz);
return -1;
}

int nBytesPerInFrmChannel = bufSz/nChs;
FILE *f;
f = fopen(outFile.c_str(), "wb");
if (!f) {
xLog("AudioEncoding::encode_audio_planarFrms(): Could not open %s", outFile.c_str());
return -1;
}

for (int64_t i = 0; i < nFrms; i++) {
int ret = av_frame_make_writable( m_frame );
if (ret < 0) {
xLog("AudioEncoding::encode_audio_planarFrms(): Could not run av_frame_make_writable()!");
return -1;
}
const uint8 *p0 = vPlanarFrms[i]->buf_;
for (int j = 0; j < nChs; ++j) {
memcpy_s(m_frame->data[j], nBytesPerInFrmChannel, p0, nBytesPerInFrmChannel);
p0 += nBytesPerInFrmChannel;
}
encode(m_ctx, m_frame, m_pkt, f);
}

/* flush the encoder */
encode(m_ctx, NULL, m_pkt, f);
fclose(f);
return 0;
}


int AudioEncoding::encode_audio_nonPlanarFrms(const std::vector<RawFrmPtr> &vNonPlanarFrms, const int nChs, const std::string &outFile)
{
assert( !av_sample_fmt_is_planar(m_ctx->sample_fmt) );

FILE *f;
f = fopen(outFile.c_str(), "wb");
if (!f) {
xLog("AudioEncoding::encode_audio_nonPlanarFrms(): Could not open %s", outFile.c_str());
return -1;
}

int64_t nFrms = vNonPlanarFrms.size();
for (int64_t i = 0; i < nFrms; i++) {
int ret = av_frame_make_writable(m_frame);
if (ret < 0) {
xLog("AudioEncoding::encode_audio_nonPlanarFrms(): Could not run av_frame_make_writable()!");
return -1;
}
const RawFrmPtr &curFrm = vNonPlanarFrms[i];

memcpy_s(m_frame->data[0], curFrm->sz_, curFrm->buf_, curFrm->sz_);

encode(m_ctx, m_frame, m_pkt, f);
}

/* flush the encoder */
encode(m_ctx, NULL, m_pkt, f);
fclose(f);

return 0;
}


/* check that a given sample format is supported by the encoder */
int AudioEncoding::check_sample_fmt(const AVCodec *codec, enum AVSampleFormat sample_fmt)
{
const enum AVSampleFormat *p = codec->sample_fmts;

while (*p != AV_SAMPLE_FMT_NONE) {
xLog("AudioEncoding::check_sample_fmt(): AVSampleFormat=%d (%s)", *p, av_get_sample_fmt_name(*p) );
if (*p == sample_fmt)
return 1;
p++;
}
return 0;
}


/* just pick the highest supported samplerate */
int AudioEncoding::select_sample_rate(const AVCodec *codec)
{
const int *p;
int best_samplerate = 0;

if (!codec->supported_samplerates)
return 44100;

p = codec->supported_samplerates;
while (*p) {
if (!best_samplerate || abs(44100 - *p) < abs(44100 - best_samplerate))
best_samplerate = *p;
p++;
}
return best_samplerate;
}

/* select layout with the highest channel count */
uint64 AudioEncoding::select_channel_layout(const AVCodec *codec)
{
const uint64_t *p;
uint64_t best_ch_layout = 0;
int best_nb_channels = 0;

if (!codec->channel_layouts)
return AV_CH_LAYOUT_STEREO;

p = codec->channel_layouts;
while (*p) {
int nb_channels = av_get_channel_layout_nb_channels(*p);

if (nb_channels > best_nb_channels) {
best_ch_layout = *p;
best_nb_channels = nb_channels;
}
p++;
}
return best_ch_layout;
}

int AudioEncoding::encode(AVCodecContext *ctx, AVFrame *frame, AVPacket *pkt,FILE *output)
{
int ret;

/* send the frame for encoding */
ret = avcodec_send_frame(ctx, frame);
if (ret < 0) {
xLog( "Error sending the frame to the encoder");
return -1;
}

/* read all the available output packets (in general there may be any
* number of them */
while (ret >= 0) {
ret = avcodec_receive_packet(ctx, pkt);
if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
return -1;
else if (ret < 0) {
xLog( "Error encoding audio frame");
return -1;
}

fwrite(pkt->data, 1, pkt->size, output);
av_packet_unref(pkt);
}
return 0;
}

