/*
*-------------------------------------------------------------------
* MuxingEncoding2.cpp
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
#include "MuxingEncoding2.h"

using namespace std;
using namespace autocut;

MuxingEncoding2::MuxingEncoding2()
: m_isOpen( false )
, m_outputVideoFilePath("")
, m_video_st()
, m_audio_st()
, m_fmt(0)
, m_oc(0)
, m_have_video(0)
, m_have_audio(0)
, m_encode_video(0)
, m_encode_audio(0)
, m_useSynData(1 )
{
}

MuxingEncoding2::~MuxingEncoding2()
{
if (m_isOpen) {
closeOpenedFile();
}
}

int MuxingEncoding2::openOutputFile(const char *outFile, const MyVideoStreamInfo  &vInfo, const MyAudioStreamInfo &aInfo)
{
int ret;
AVCodec *audio_codec, *video_codec;
AVDictionary *opt = NULL;

if (m_isOpen) {
closeOpenedFile();
}

m_outputVideoFilePath = string(outFile);
m_vInfo  = vInfo;
m_aInfo =  aInfo;


const char* filename = m_outputVideoFilePath.c_str();
/* Initialize libavcodec, and register all codecs and formats. */
av_register_all();

//todo:
//set dicttiinary if need
//int av_dict_set(AVDictionary **pm, const char *key, const char *value, int flags)
//av_dict_set(&opt, key1, value1, 0);
//av_dict_set(&opt, key2, value2, 0);

/* allocate the output media context */
avformat_alloc_output_context2(&m_oc, NULL, NULL, filename );
if (!m_oc) {
xLog("MuxingEncoding2::openOutputFile(): Could not deduce output format from file extension: using MPEG.");
avformat_alloc_output_context2(&m_oc, NULL, "mp4", filename );  //or "mp4", "mpeg"
}
if (!m_oc) {
return 1;
}

m_fmt = m_oc->oformat;

/* Add the audio and video streams using the default format codecs
* and initialize the codecs. */
if (m_fmt->video_codec != AV_CODEC_ID_NONE) {
add_stream(&m_video_st, m_oc, &video_codec, m_fmt->video_codec);
m_have_video = 1;
m_encode_video = 1;
}
if (m_fmt->audio_codec != AV_CODEC_ID_NONE) {
add_stream(&m_audio_st, m_oc, &audio_codec, m_fmt->audio_codec);
m_have_audio = 1;
m_encode_audio = 1;
}

/* Now that all the parameters are set, we can open the audio and
* video codecs and allocate the necessary encode buffers. */
if (m_have_video) {
open_video(m_oc, video_codec, &m_video_st, opt);
}

if (m_have_audio) {
open_audio(m_oc, audio_codec, &m_audio_st, opt);
}

av_dump_format(m_oc, 0, filename, 1);

/* open the output file, if needed */
if ( !(m_fmt->flags & AVFMT_NOFILE) ) {
ret = avio_open(&m_oc->pb, filename, AVIO_FLAG_WRITE);
if (ret < 0) {
xLog("Could not open '%s': %s\n", filename, my_av_err2str(ret).c_str());
return 1;
}
}

/* Write the stream header, if any. */
ret = avformat_write_header(m_oc, &opt);
if (ret < 0) {
xLog("Error occurred when opening output file: %s\n", my_av_err2str(ret).c_str());
return 1;
}

m_encode_video = 1;
m_encode_audio = 1;
m_video_st.next_pts = 0;
m_audio_st.next_pts = 0;

if (m_audio_st.frame->nb_samples == 0) {
m_audio_st.frame->nb_samples = 1536; //hack for  MTS file;
}
if (m_audio_st.tmp_frame->nb_samples == 0) {
m_audio_st.tmp_frame->nb_samples = 1536; //hack for  MTS file;
}
m_isOpen = true;
return 0;
}

int MuxingEncoding2::closeOpenedFile()
{
/* Write the trailer, if any. The trailer must be written before you
* close the CodecContexts open when you wrote the header; otherwise
* av_write_trailer() may try to use memory that was freed on
* av_codec_close(). */
av_write_trailer(m_oc);

/* Close each codec. */
if (m_have_video)
close_stream(m_oc, &m_video_st);
if (m_have_audio)
close_stream(m_oc, &m_audio_st);

if (!(m_fmt->flags & AVFMT_NOFILE)) {
avio_closep(&m_oc->pb);  /* Close the output file. */
}

/* free the stream */
avformat_free_context(m_oc);

m_isOpen = false;
return 0;
}

int MuxingEncoding2::write_frame(AVFormatContext *fmt_ctx, const AVRational *time_base, AVStream *st, AVPacket *pkt)
{
/* rescale output packet timestamp values from codec to stream timebase */
av_packet_rescale_ts(pkt, *time_base, st->time_base);
pkt->stream_index = st->index;

/* Write the compressed frame to the media file. */
//log_packet(fmt_ctx, pkt);
return av_interleaved_write_frame(fmt_ctx, pkt);
}

/* Add an output stream. */
void MuxingEncoding2::add_stream(OutputStream *ost, AVFormatContext *oc, AVCodec **codec, enum AVCodecID codec_id)
{
AVCodecContext *c;

/* find the encoder */
*codec = avcodec_find_encoder(codec_id);
if (!(*codec)) {
xLogExit( "Could not find encoder for %s",	avcodec_get_name(codec_id) );
}

ost->st = avformat_new_stream(oc, NULL);
if (!ost->st) {
xLogExit( "Could not allocate stream");
}
ost->st->id = oc->nb_streams - 1;
c = avcodec_alloc_context3(*codec);
if (!c) {
xLogExit("Could not alloc an encoding context\n");
}
ost->enc = c;

switch ((*codec)->type) {
case AVMEDIA_TYPE_AUDIO:
c->sample_fmt = (AVSampleFormat)m_aInfo.sample_fmt;
c->bit_rate = m_aInfo.bit_rate;
c->sample_rate = m_aInfo.sample_rate;
#if 0
if ((*codec)->supported_samplerates) {
c->sample_rate = (*codec)->supported_samplerates[0];
for (i = 0; (*codec)->supported_samplerates[i]; i++) {
if ((*codec)->supported_samplerates[i] == MY_AUDIO_SMP_RATE)
c->sample_rate = MY_AUDIO_SMP_RATE;
}
}
#endif

c->channels = m_aInfo.channels;
c->channel_layout = m_aInfo.ch_layout;
#if 0
if ((*codec)->channel_layouts) {
c->channel_layout = (*codec)->channel_layouts[0];
for (i = 0; (*codec)->channel_layouts[i]; i++) {
if ((*codec)->channel_layouts[i] == AV_CH_LAYOUT_STEREO)
c->channel_layout = AV_CH_LAYOUT_STEREO;
}
}
c->channels = av_get_channel_layout_nb_channels(c->channel_layout);
#endif
ost->st->time_base = m_aInfo.time_base;
//ost->tmp_frame->nb_samples = m_aCodecCtx_desired.frame_size;
//ost->frame->nb_samples = m_aInfo.nb_samples_perfrm;
break;

case AVMEDIA_TYPE_VIDEO:
c->codec_id = codec_id;

c->bit_rate = m_vInfo.bit_rate; // 400000;
c->width = m_vInfo.w;               //resolution must be a multiple of two (1280x720),(1900x1080),(720x480)
c->height = m_vInfo.h;
c->time_base.num = m_vInfo.frame_rate.den;
c->time_base.den = m_vInfo.frame_rate.num;

//m_vCodecCtx_desired.time_base;
//c->framerate = m_vCodecCtx_desired.framerate;

ost->st->time_base = c->time_base;

c->gop_size = 12; /* emit one intra frame every twelve frames at most */
c->pix_fmt = m_vInfo.pix_fmt;
if (c->codec_id == AV_CODEC_ID_MPEG2VIDEO) {
/* just for testing, we also add B-frames */
c->max_b_frames = 2;
}
if (c->codec_id == AV_CODEC_ID_MPEG1VIDEO) {
/* Needed to avoid using macroblocks in which some coeffs overflow.
* This does not happen with normal video, it just happens here as
* the motion of the chroma plane does not match the luma plane. */
c->mb_decision = 2;
}
break;

default:
break;
}

/* Some formats want stream headers to be separate. */
if (oc->oformat->flags & AVFMT_GLOBALHEADER)
c->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
}

/**************************************************************/
/* audio output */

AVFrame* MuxingEncoding2::alloc_audio_frame(enum AVSampleFormat sample_fmt,
uint64_t channel_layout,
int sample_rate, int nb_samples)
{
AVFrame *frame = av_frame_alloc();
int ret;

if (!frame) {
xLogExit("Error allocating an audio frame");
}

frame->format = sample_fmt;
frame->channel_layout = channel_layout;
frame->sample_rate = sample_rate;
frame->nb_samples = nb_samples;

if (nb_samples) {
ret = av_frame_get_buffer(frame, 0);
if (ret < 0) {
xLogExit("Error allocating an audio buffer");
}
}

return frame;
}

void MuxingEncoding2::open_audio(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg)
{
AVCodecContext *c;
int nb_samples;
int ret;
AVDictionary *opt = NULL;

c = ost->enc;

/* open it */
av_dict_copy(&opt, opt_arg, 0);
ret = avcodec_open2(c, codec, &opt);
av_dict_free(&opt);
if (ret < 0) {
xLogExit("Could not open audio codec: %s", my_av_err2str(ret).c_str());
}

/* init signal generator */
ost->t = 0;
ost->tincr = 2 * M_PI * 110.0 / c->sample_rate;
/* increment frequency by 110 Hz per second */
ost->tincr2 = 2 * M_PI * 110.0 / c->sample_rate / c->sample_rate;

nb_samples = m_aInfo.nb_samples_perfrm;
ost->frame = alloc_audio_frame(c->sample_fmt, c->channel_layout, c->sample_rate, nb_samples);
ost->tmp_frame = alloc_audio_frame( MY_AUDIO_SMP_FMT, c->channel_layout, c->sample_rate, nb_samples);

/* copy the stream parameters to the muxer */
ret = avcodec_parameters_from_context(ost->st->codecpar, c);
if (ret < 0) {
xLogExit("Could not copy the stream parameters");
}

/* create resampler context */
ost->swr_ctx = swr_alloc();
if (!ost->swr_ctx) {
xLogExit("Could not allocate resampler context");
}

/* set options */
av_opt_set_int(ost->swr_ctx, "in_channel_count", c->channels, 0);
av_opt_set_int(ost->swr_ctx, "in_sample_rate", c->sample_rate, 0);
av_opt_set_sample_fmt(ost->swr_ctx, "in_sample_fmt", AV_SAMPLE_FMT_FLT, 0);
av_opt_set_int(ost->swr_ctx, "out_channel_count", c->channels, 0);
av_opt_set_int(ost->swr_ctx, "out_sample_rate", c->sample_rate, 0);
av_opt_set_sample_fmt(ost->swr_ctx, "out_sample_fmt", c->sample_fmt, 0);

/* initialize the resampling context */
if ((ret = swr_init(ost->swr_ctx)) < 0) {
xLogExit("Failed to initialize the resampling context");
}
}

/* Prepare a 16 bit dummy audio frame of 'frame_size' samples and
* 'nb_channels' channels. */
AVFrame* MuxingEncoding2::get_audio_frame(OutputStream *ost)
{
int j, i, v;
AVFrame *frame = ost->tmp_frame;
int16_t *q = (int16_t*)frame->data[0];

/* check if we want to generate more frames */
AVRational tmp;  tmp.num = 1;  tmp.den = 1;
if (av_compare_ts(ost->next_pts, ost->enc->time_base, MY_STREAM_DURATION, tmp) >= 0) {
return NULL;
}

for (j = 0; j < frame->nb_samples; j++) {
v = (int)(sin(ost->t) * 10000);
for (i = 0; i < ost->enc->channels; i++) {
*q++ = v;
}
ost->t += ost->tincr;
ost->tincr += ost->tincr2;
}

frame->pts = ost->next_pts;
ost->next_pts += frame->nb_samples;

return frame;
}

//read data from <m_rawAudio>
//ref: https://stackoverflow.com/questions/18888986/what-is-the-difference-between-av-sample-fmt-s16p-and-av-sample-fmt-s16
//AV_SAMPLE_FMT_S16P is planar signed 16 bit audio, i.e. 2 bytes for each sample which is same for AV_SAMPLE_FMT_S16.
//The only difference is in AV_SAMPLE_FMT_S16 samples of each channel are interleaved i.e. if you have two channel audio then the samples buffer will look like
//c1 c1 c2 c2 c1 c1 c2 c2...
//where c1 is a sample for channel1 and c2 is sample for channel2.

//while for one frame of planar audio you will have something like
//c1 c1 c1 c1 ....c2 c2 c2 c2 ..

//now how is it stored in AVFrame :
//for planar audio :
//data[i] will contain the data of channel i(assuming channel 0 is first channel).
//however if you have more channels than 8, then data for rest of the channels can be found in extended_data attribute of AVFrame.
//for non - planar audio
//data[0] will contain the data for all channels in an interleaved manner.

//a assign one channel audio data into multiple channles
//<buf>,  the audio data from one channel
//<sz>, the sz of <buf>
AVFrame* MuxingEncoding2::get_audio_frame2(OutputStream *ost, const uint8 *buf, const uint32 bufSz, const int nChs)
{
int j, i, k;
AVFrame *frame = ost->tmp_frame;
int bytes_ps = av_get_bytes_per_sample((AVSampleFormat)frame->format);
size_t unpadded_linesize = frame->nb_samples * bytes_ps;
int sz = bufSz / nChs;
if ( unpadded_linesize != sz ) {
xLogExit("MuxingEncoding2::get_audio_frame2(): unpadded_linesize(%d) != sz(%d)", unpadded_linesize, sz);
}

enum AVSampleFormat fmt = (AVSampleFormat)frame->format; 
if ( fmt  <= AV_SAMPLE_FMT_DBL) {  //interleave
//todo: this only works for nChs==1, if nChs>1 the first channel is copied to every channel
const uint8 *p0 = buf;
uint8* q = frame->data[0];
for (j = 0; j < frame->nb_samples; j++) {
for (i = 0; i < ost->enc->channels; i++) {
const uint8 *p = p0;
for ( k = 0; k < bytes_ps; ++k) {
*q++ = *p++;
}
}
p0 += bytes_ps;
}
}
else if ( fmt <= AV_SAMPLE_FMT_DBLP){ //planar
if ( nChs>1 && nChs == ost->enc->channels ) {
const uint8 *buf0 = buf;
for (i = 0; i < ost->enc->channels; i++) {
memcpy_s(frame->data[i], sz, buf0, sz);
buf0 += sz;
}
}
else {
for (i = 0; i < ost->enc->channels; i++) {
memcpy_s(frame->data[i], sz, buf, sz);
}
}
}
else {
xLogExit("MuxingEncoding2::get_audio_frame2() cannot handle the audio format!");
}



frame->pts = ost->next_pts;
ost->next_pts += frame->nb_samples;

return frame;
}

/*
* encode one audio frame and send it to the muxer
* return 1 when encoding is finished, 0 otherwise
*/
int MuxingEncoding2::write_audio_frame(AVFormatContext *oc, OutputStream *ost, const uint8 *buf, const uint32 bufSz, const int nChs)
{
AVCodecContext *c;
AVPacket pkt = { 0 }; // data and size must be 0;
AVFrame *frame;

int ret;
int got_packet;
int dst_nb_samples;

av_init_packet(&pkt);
c = ost->enc;

if (m_useSynData) {
frame = get_audio_frame(ost);
}
else {
frame = get_audio_frame2(ost, buf,  bufSz, nChs);
}

if (frame) {
/* convert samples from native format to destination codec format, using the resampler */
/* compute destination number of samples */
dst_nb_samples = av_rescale_rnd(swr_get_delay(ost->swr_ctx, c->sample_rate) + frame->nb_samples, c->sample_rate, c->sample_rate, AV_ROUND_UP);
av_assert0(dst_nb_samples == frame->nb_samples);

/* when we pass a frame to the encoder, it may keep a reference to it
* internally;
* make sure we do not overwrite it here
*/
ret = av_frame_make_writable(ost->frame);
if (ret < 0) {
xLogExit("write_audio_frame(): from av_frame_make_writable()!");
}

/* convert to destination format */
ret = swr_convert(ost->swr_ctx,
ost->frame->data, dst_nb_samples,
(const uint8_t **)frame->data, frame->nb_samples);
if (ret < 0) {
xLogExit("Error while converting");
}
frame = ost->frame;

AVRational tmp;  tmp.num = 1;  tmp.den = c->sample_rate;
frame->pts = av_rescale_q(ost->samples_count, tmp, c->time_base);
ost->samples_count += dst_nb_samples;
}

ret = avcodec_encode_audio2(c, &pkt, frame, &got_packet);
if (ret < 0) {
xLogExit("Error encoding audio frame: %s", my_av_err2str(ret).c_str());
}

if (got_packet) {
ret = write_frame(oc, &c->time_base, ost->st, &pkt);
if (ret < 0) {
xLogExit("Error while writing audio frame: %s", my_av_err2str(ret).c_str());
}
}

return (frame || got_packet) ? 0 : 1;
}

/**************************************************************/
/* video output */

AVFrame* MuxingEncoding2::alloc_picture(enum AVPixelFormat pix_fmt, int width, int height)
{
AVFrame *picture;
int ret;

picture = av_frame_alloc();
if (!picture)
return NULL;

picture->format = pix_fmt;
picture->width = width;
picture->height = height;

/* allocate the buffers for the frame data */
ret = av_frame_get_buffer(picture, 32);
if (ret < 0) {
xLogExit("Could not allocate frame data.");
}

return picture;
}

void MuxingEncoding2::open_video(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg)
{
int ret;
AVCodecContext *c = ost->enc;
AVDictionary *opt = NULL;

av_dict_copy(&opt, opt_arg, 0);

/* open the codec */
ret = avcodec_open2(c, codec, &opt);
av_dict_free(&opt);
if (ret < 0) {
xLogExit("Could not open video codec: %s", my_av_err2str(ret).c_str());
}

/* allocate and init a re-usable frame */
ost->frame = alloc_picture(c->pix_fmt, c->width, c->height);
if (!ost->frame) {
xLogExit("Could not allocate video frame");
}

/* If the output format is not YUV420P, then a temporary YUV420P
* picture is needed too. It is then converted to the required
* output format. */
ost->tmp_frame = NULL;
if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
ost->tmp_frame = alloc_picture(AV_PIX_FMT_YUV420P, c->width, c->height);
if (!ost->tmp_frame) {
xLogExit("Could not allocate temporary picture");
}
}

/* copy the stream parameters to the muxer */
ret = avcodec_parameters_from_context(ost->st->codecpar, c);
if (ret < 0) {
xLogExit("Could not copy the stream parameters");
}

}

/* Prepare a dummy image. */
void MuxingEncoding2::fill_yuv_image(AVFrame *pict, int frame_index, int width, int height)
{
int x, y, i;

i = frame_index;

/* Y */
for (y = 0; y < height; y++)
for (x = 0; x < width; x++)
pict->data[0][y * pict->linesize[0] + x] = x + y + i * 3;

/* Cb and Cr */
for (y = 0; y < height / 2; y++) {
for (x = 0; x < width / 2; x++) {
pict->data[1][y * pict->linesize[1] + x] = 128 + y + i * 2;
pict->data[2][y * pict->linesize[2] + x] = 64 + x + i * 5;
}
}
}

AVFrame* MuxingEncoding2::get_video_frame(OutputStream *ost)
{
AVCodecContext *c = ost->enc;

/* check if we want to generate more frames */
AVRational tmp; tmp.num = 1; tmp.den = 1;
if (av_compare_ts(ost->next_pts, c->time_base, MY_STREAM_DURATION, tmp) >= 0) {
return NULL;
}

/* when we pass a frame to the encoder, it may keep a reference to it
* internally; make sure we do not overwrite it here */
if (av_frame_make_writable(ost->frame) < 0) {
xLogExit("MuxingEncoding2::get_video_frame): av_frame_make_writable() fail!");
}

if (c->pix_fmt != AV_PIX_FMT_YUV420P) {
/* as we only generate a YUV420P picture, we must convert it
* to the codec pixel format if needed */
if (!ost->sws_ctx) {
ost->sws_ctx = sws_getContext(c->width, c->height,
AV_PIX_FMT_YUV420P,
c->width, c->height,
c->pix_fmt,
SCALE_FLAGS, NULL, NULL, NULL);
if (!ost->sws_ctx) {
xLogExit("Could not initialize the conversion context");
}
}
fill_yuv_image(ost->tmp_frame, ost->next_pts, c->width, c->height);
sws_scale(ost->sws_ctx,
(const uint8_t * const *)ost->tmp_frame->data, ost->tmp_frame->linesize,
0, c->height, ost->frame->data, ost->frame->linesize);
}
else {
fill_yuv_image(ost->frame, ost->next_pts, c->width, c->height);
}

ost->frame->pts = ost->next_pts++;

return ost->frame;
}


//read data from <m_rawAudio>
AVFrame* MuxingEncoding2::get_video_frame2(OutputStream *ost, const uint8 *buf, const uint32 sz)
{
AVCodecContext *c = ost->enc;
AVFrame* frm = ost->frame;

/* when we pass a frame to the encoder, it may keep a reference to it
* internally; make sure we do not overwrite it here */
if (av_frame_make_writable(ost->frame) < 0) {
xLogExit("MuxingEncoding2::get_video_frame2(): av_frame_make_writable() fail!");
}
int w0 = c->width, h0 = c->height;
int w1 = w0/2, h1 = h0/2;

assert( c->pix_fmt == AV_PIX_FMT_YUV420P );
assert( sz == (w0*h0 * 3) / 2);

int x, y;
//int L0 = frm->linesize[0];
//int L1 = frm->linesize[1];
//int L2 = frm->linesize[2];
uint8_t  *p0 = frm->data[0];
/* Y */
for (y = 0; y < h0; y++){
for (x = 0; x < w0; x++) {
//p0[y * L0 + x] = *buf++;  //todo using p0++
*p0++ = *buf++;
}
}

/* Cb and Cr */
uint8_t  *p1 = frm->data[1];
for (y = 0; y < h1; y++) {
for (x = 0; x < w1; x++) {
//p1[y * L1 + x] = *buf++;   //todo: using p1++
*p1++ = *buf++;
}
}

uint8_t  *p2 = frm->data[2];
for (y = 0; y < h1; y++) {
for (x = 0; x < w1; x++) {
//p2[y * L2 + x] = *buf++;   //todo: using p2++
*p2++ = *buf++;
}
}

ost->frame->pts = ost->next_pts++;

return ost->frame;
}

/*
* encode one video frame and send it to the muxer
* return 1 when encoding is finished, 0 otherwise
*/
int MuxingEncoding2::write_video_frame(AVFormatContext *oc, OutputStream *ost, const uint8 *buf, const uint32 sz)
{
int ret;
AVCodecContext *c;
AVFrame *frame;
int got_packet = 0;
AVPacket pkt = { 0 };

c = ost->enc;
if (m_useSynData) {
frame = get_video_frame(ost);
}
else {
frame = get_video_frame2(ost, buf, sz);
}

av_init_packet(&pkt);

/* encode the image */
ret = avcodec_encode_video2(c, &pkt, frame, &got_packet);
if (ret < 0) {
xLogExit("Error encoding video frame: %s", my_av_err2str(ret).c_str());
}

if (got_packet) {
ret = write_frame(oc, &c->time_base, ost->st, &pkt);
}
else {
ret = 0;
}

if (ret < 0) {
xLogExit("Error while writing video frame: %s", my_av_err2str(ret).c_str());
}

return (frame || got_packet) ? 0 : 1;
}

void MuxingEncoding2::close_stream(AVFormatContext *oc, OutputStream *ost)
{
avcodec_free_context(&ost->enc);
av_frame_free(&ost->frame);
av_frame_free(&ost->tmp_frame);
sws_freeContext(ost->sws_ctx);
swr_free(&ost->swr_ctx);
}

/**************************************************************/
/* media file output */

int MuxingEncoding2:: testByWrtSynFrms()
{
int a;
m_useSynData = true;
while (m_encode_video || m_encode_audio) {
/* select the stream to encode */
a = av_compare_ts(m_video_st.next_pts, m_video_st.enc->time_base, m_audio_st.next_pts, m_audio_st.enc->time_base);
if ( m_encode_video && (!m_encode_audio ||  a <= 0) ) {
m_encode_video = !write_video_frame(m_oc, &m_video_st, NULL, 0);
}
else {
m_encode_audio = !write_audio_frame(m_oc, &m_audio_st, NULL, 0);
}
}

return 0;
}


int MuxingEncoding2::wrtSomeFrms(const uint8 *rawVideo, const uint64 vSz0, const uint64 vFrmSz, const uint8 *rawAudio, const uint64 aSz0, const uint64 aFrmSz)
{
uint64 vWrtSz = 0;
uint64 aWrtSz = 0;

const uint8 *pV = rawVideo;
const uint8 *pA = rawAudio;

#if _DEBUG
assert( vFrmSz == m_vInfo.getVideoRawFrmSizeInBytes() );  //yuv420p format frm sz
#endif

while (m_encode_video || m_encode_audio) {
/* select the stream to encode */
int a = av_compare_ts(m_video_st.next_pts, m_video_st.enc->time_base, m_audio_st.next_pts, m_audio_st.enc->time_base);
if (m_encode_video && (!m_encode_audio || a <= 0)) {
m_encode_video = !write_video_frame(m_oc, &m_video_st, pV, vFrmSz);
pV += vFrmSz;
vWrtSz += vFrmSz;
}
else {
m_encode_audio = !write_audio_frame(m_oc, &m_audio_st, pA, aFrmSz);
pA += aFrmSz;
aWrtSz += aFrmSz;
}

if (vWrtSz >= vSz0 || aWrtSz >= aSz0 ) {
break;
}
}

return 0;
}


int MuxingEncoding2::wrtSomeFrms(const std::vector<RawFrmPtr> &vVideoFrms, const uint8 *rawAudio, const uint64 rawAudioSz, const uint32 aFrmSzInByte)
{

uint32  nTotAudioFrms = ceil(rawAudioSz/ aFrmSzInByte);
uint32  nTotVideoFrms = vVideoFrms.size();

uint32  vFrmIdx = 0;
uint32  aFrmIdx = 0;

const uint8 *pA = rawAudio;

xLog("MuxingEncoding2::wrtSomeFrms(): nTotAudioFrms=%ld, nTotVideoFrms=%ld", nTotAudioFrms, nTotVideoFrms);
if (nTotVideoFrms<1 || nTotAudioFrms<1) {
xLog(" MuxingEncoding2::wrtSomeFrms(): Abnormal -- <nTotAudioFrms> or <nTotVideoFrms> is zeros, quit w/o write any frms!");
return -1;
}

uint64 vFrmSzInByte = vVideoFrms[0]->sz_;

#if _DEBUG
assert(vFrmSzInByte == m_vInfo.getVideoRawFrmSizeInBytes());  //yuv420p format frm sz
assert(aFrmSzInByte == 4096);
#endif

while (m_encode_video || m_encode_audio) {
/* select the stream to encode */
int a = av_compare_ts(m_video_st.next_pts, m_video_st.enc->time_base, m_audio_st.next_pts, m_audio_st.enc->time_base);
if (m_encode_video && (!m_encode_audio || a <= 0)) {
m_encode_video = !write_video_frame(m_oc, &m_video_st, vVideoFrms[vFrmIdx]->buf_, vFrmSzInByte);
++vFrmIdx;
}
else {
m_encode_audio = !write_audio_frame(m_oc, &m_audio_st, pA, aFrmSzInByte);
pA += aFrmSzInByte;
++aFrmIdx;
}

if (aFrmIdx >= nTotAudioFrms || vFrmIdx >= nTotVideoFrms) {
break;
}
}

return 0;

}

int MuxingEncoding2::wrtSomeFrms(const std::vector<RawFrmPtr> &vVideoFrms, const std::vector<RawFrmPtr> &vAudioFrms, const int nChs) 
{
uint32 nVideoFrms = vVideoFrms.size();
uint32 nAudioFrms = vAudioFrms.size();

if (nVideoFrms == 0 || nAudioFrms == 0) {
return 0;
}


m_useSynData = false;

uint32 vIdx = 0, aIdx = 0;
uint32 vFrmSz = vVideoFrms[0]->sz_;
uint32 aFrmSz = vAudioFrms[0]->sz_;

#if _DEBUG
assert(vFrmSz == m_vInfo.getVideoRawFrmSizeInBytes() );  //yuv420p format frm sz
#endif

while (m_encode_video || m_encode_audio) {
/* select the stream to encode */
int a = av_compare_ts(m_video_st.next_pts, m_video_st.enc->time_base, m_audio_st.next_pts, m_audio_st.enc->time_base);
if (m_encode_video && (!m_encode_audio || a <= 0)) {
const uint8 *pV = vVideoFrms[ vIdx ]->buf_;
m_encode_video = !write_video_frame(m_oc, &m_video_st, pV, vFrmSz);
++vIdx;
}
else {
const uint8 *pA = vAudioFrms[aIdx]->buf_;
m_encode_audio = !write_audio_frame(m_oc, &m_audio_st, pA, aFrmSz, nChs);
++aIdx;
}

//printf("vIdx=%d, aIdx=%d\n", vIdx, aIdx);

if (vIdx >= nVideoFrms || aIdx >= nAudioFrms ) {
break;
}
}

return 0;
}
