/*
*-------------------------------------------------------------------
* DemuxingDecoding2.cpp
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
#include "DemuxingDecoding2.h"

using namespace std;
using namespace autocut;

DemuxingDecoding2::DemuxingDecoding2(CfgDemuxingDecodingPtr  cfg)
: m_cfg(cfg)
, m_video_stream(NULL)
, m_audio_stream(NULL)
, m_video_dst_file(NULL)
, m_audio_dst_file(NULL)
, m_fmt_ctx(NULL)
, m_video_dec_ctx(NULL)
, m_audio_dec_ctx(NULL)
, m_width(0)
, m_height(0)
, m_src_filename("")
, m_refcount(0)

, m_video_stream_idx(-1)
, m_audio_stream_idx(-1)
, m_frame(NULL)
, m_video_frame_count_g(0)
, m_audio_frame_count_g(0)
, m_isOpened( false )
, m_firstVideoFrm(0)
, m_latestDecodedVideoFrm(0)
{
for (int i = 0; i < 4; ++i) {
m_video_dst_data[i] = NULL;
}
memset( (uint8*)&m_videoStreamInfo, 0, sizeof(MyVideoStreamInfo) );
memset( (uint8*)&m_audioStreamInfo, 0, sizeof(MyAudioStreamInfo) );
resizeThumnailKeyFrm();
}

DemuxingDecoding2::~DemuxingDecoding2()
{
if (m_isOpened) {
closeOpenedFiles();
}
}

void DemuxingDecoding2::resizeThumnailKeyFrm()
{
m_keyFrmSz.width = m_cfg->thumbnailWidth;
m_keyFrmSz.height = m_cfg->thumbnailHeight;
}


//make sure video cliips is inside audio range
bool DemuxingDecoding2::findVideoFrmIds(const uint32 aId1, const uint32 aId2, uint32 &vId1, uint32 &vId2)
{
std::vector<FrmIdx>::const_iterator it = m_vFrmIdxQ.begin();
std::vector<FrmIdx>::const_iterator it1 = it;
std::vector<FrmIdx>::const_iterator it2 = m_vFrmIdxQ.end();
--it2;
for (; it != m_vFrmIdxQ.end(); ++it) {
if (it->type == FrmType_A_FRM) {
if (it->idx >= aId1) {
it1 = it;
break;
}
}
}

for (; it != m_vFrmIdxQ.end(); ++it) {
if (it->type == FrmType_A_FRM) {
if (it->idx >= aId2) {
it2 = it;
break;
}
}
}

//read <vId1>
for ( it=it1; it != m_vFrmIdxQ.end(); ++it) {
if (it->type == FrmType_V_FRM) {
vId1 = it->idx;
break;
}
}

//read <vId2>
vId2 = 0;
for (it = it2; it != m_vFrmIdxQ.end(); ++it) {
if (it->type == FrmType_V_FRM) {
vId2 = it->idx;
break;
}
}

if (vId2 == 0) {
it2 = m_vFrmIdxQ.end();
--it2;
for (it = it2; it != m_vFrmIdxQ.begin(); --it) {
if (it->type == FrmType_V_FRM) {
vId2 = it->idx;
break;
}
}
}

return true;
}

//called for decode Whole Video to get:
// 1. <m_vFrmIdxQ> corresonding frame indices table between video and audio
// 2 dump/store the enter video and audio raw frames into disk/memeory
int DemuxingDecoding2::decode_packet(int *got_frame, int cached )
{
int ret = 0;
int decoded = m_pkt.size;


*got_frame = 0;

if ( m_cfg->demuxingVideo && m_pkt.stream_index == m_video_stream_idx /*&&  (m_pkt.flags & AV_PKT_FLAG_KEY) == 1*/ ) {

/* decode video frame */
ret = avcodec_decode_video2(m_video_dec_ctx, m_frame, got_frame, &m_pkt);
if (ret < 0) {
xLog("DemuxingDecoding2::decode_packet(): Error decoding video frame (%s) in file: %s", my_av_err2str(ret).c_str(), m_src_filename.c_str() );
return ret;
}

if (*got_frame) {
if (m_frame->width != m_width || m_frame->height != m_height || m_frame->format != m_pix_fmt) {
/* To handle this change, one could call av_image_alloc again and
* decode the following frames into another rawvideo file. */
xLog("Error: Width, height and pixel format have to be "
"constant in a rawvideo file, but the width, height or "
"pixel format of the input video changed:\n"
"old: width = %d, height = %d, format = %s\n"
"new: width = %d, height = %d, format = %s\n",
m_width, m_height, av_get_pix_fmt_name(m_pix_fmt),
m_frame->width, m_frame->height,
av_get_pix_fmt_name((AVPixelFormat)m_frame->format));
return -1;
}
#if 0
int t;
t = m_video_dec_ctx->framerate.num;
t = m_video_dec_ctx->framerate.den;
t = m_video_dec_ctx->frame_number;
t = m_video_dec_ctx->frame_size;
t = m_video_dec_ctx->time_base.num;
t = m_video_dec_ctx->time_base.den;
#endif
FrmIdx frmId(FrmType_V_FRM, m_video_frame_count_g, 0, 0);
m_vFrmIdxQ.push_back(frmId);
#if 0
{
char buf[32];
av_ts_make_time_string(buf, m_frame->pts, &m_video_dec_ctx->time_base);
printf("video_frame%s, m_video_frame_count_g=:%d, coded_picture_number=:%d, pts:%s\n", cached ? "(cached)" : "", m_video_frame_count_g,
m_frame->coded_picture_number, buf);
}
#endif


/* copy decoded frame to destination buffer:
* this is required since rawvideo expects non aligned data */
av_image_copy(m_video_dst_data, m_video_dst_linesize, (const uint8_t **)(m_frame->data), m_frame->linesize,	m_pix_fmt, m_width, m_height);

/* write to rawvideo file */
if (m_cfg->dumpRawVideo) {
fwrite(m_video_dst_data[0], 1, m_video_dst_bufsize, m_video_dst_file);
}

if (m_cfg->storeRawVideoInMemo ) {
#if _DEBUG
assert( m_video_dst_bufsize == (m_width*m_height * 3) / 2 );
#endif
RawFrmPtr newFrm( new RawFrm(cv::Size(m_cfg->outputImgWidth, m_cfg->outputImgHeight), m_video_frame_count_g) );
if (m_isResizeRawImg) {
resizeYuvVideoFrm(*newFrm.get(), m_video_dst_data[0], m_width, m_height, libyuv::kFilterBilinear);
}
else {
newFrm->copyFrom(m_video_dst_data[0], m_video_dst_bufsize, m_video_frame_count_g);
}
newFrm->keyFrmId_ = m_video_keyframe_count_g;
newFrm->offset_ = m_byteposition;
m_vVideoFrms.push_back(newFrm);
}

if (m_cfg->storeThumbnailKeyFrms && m_frame->pict_type == AV_PICTURE_TYPE_I) {
RawFrmPtr keyFrm(new RawFrm(m_keyFrmSz, m_video_frame_count_g));
keyFrm->keyFrmId_ = m_video_keyframe_count_g++;
keyFrm->offset_ = m_byteposition;
resizeYuvVideoFrm( *keyFrm.get(), m_video_dst_data[0], m_width, m_height, libyuv::kFilterBilinear);
#if 0
//RawFrm rawVideoFrm(FrmType_V_FRM, m_video_frame_count_g, m_video_dst_bufsize);
//rawVideoFrm.copyFrom(m_video_dst_data[0], m_video_dst_bufsize, m_video_frame_count_g);
//dumpFrmImg(rawVideoFrm, "c:/temp", "raw");
string tag = num_to_string(keyFrm->fn_, "keyFrm");
dumpFrmImg(*keyFrm.get(), "c:/temp", tag);
#endif
m_vThumbnailKeyFrms.push_back(keyFrm);
}

//--------------- increase global count ------------------------
++m_video_frame_count_g;

}
}
else if (m_cfg->demuxingAudio && m_pkt.stream_index == m_audio_stream_idx) {
/* decode audio frame */
ret = avcodec_decode_audio4(m_audio_dec_ctx, m_frame, got_frame, &m_pkt);
if (ret < 0) {
xLog("Error decoding audio frame (%s)", my_av_err2str(ret).c_str());
return ret;
}
/* Some audio decoders decode only part of the packet, and have to be
* called again with the remainder of the packet data.
* Sample: fate-suite/lossless-audio/luckynight-partial.shn
* Also, some decoders might over-read the packet. */
decoded = FFMIN(ret, m_pkt.size);

if (*got_frame) {
m_audio_dst_bufsize = m_frame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)m_frame->format);
FrmIdx frmId(FrmType_A_FRM, m_audio_frame_count_g, m_frame->nb_samples, m_audio_dst_bufsize);
m_vFrmIdxQ.push_back(frmId);

#if 0
char buf[256];
av_ts_make_string(buf, m_frame->pts /*, &m_audio_dec_ctx->time_base*/);
printf("audio_frame%s n:%d nb_samples:%d pts:%ld, wrtBufSz:%d\n",
cached ? "(cached)" : "",
m_audio_frame_count_g, m_frame->nb_samples, m_frame->pts, m_audio_dst_bufsize);
#endif

/* Write the raw audio data samples of the first plane. This works
* fine for packed formats (e.g. AV_SAMPLE_FMT_S16). However,
* most audio decoders output planar audio, which uses a separate
* plane of audio samples for each channel (e.g. AV_SAMPLE_FMT_S16P).
* In other words, this code will write only the first audio channel
* in these cases.
* You should use libswresample or libavfilter to convert the frame
* to packed data. */
if (m_cfg->dumpRawAudio) {
//assert(m_frame->nb_samples * 4 == m_audio_dst_bufsize);  //float is 4 bytes
fwrite(m_frame->extended_data[0], 1, m_audio_dst_bufsize, m_audio_dst_file);
}

if (m_cfg->storeRawAudioInMemo ) {
RawFrmPtr aFrm(new  RawFrm(FrmType_A_FRM, m_audio_frame_count_g, m_audio_dst_bufsize));
aFrm->copyFrom(m_frame->extended_data[0], m_audio_dst_bufsize, m_audio_frame_count_g);
m_vAudioFrms.push_back(aFrm);
}

//--------------- increase global count ------------------------
++m_audio_frame_count_g;
}
}

/* If we use frame reference counting, we own the data and need
* to de-reference it when we don't use it anymore */
if (*got_frame && m_refcount) {
av_frame_unref(m_frame);
}

return decoded;
}


//called for read raw video/audio frames based on <frmRng>
// 1. <m_vFrmIdxQ> stores frame indices table between video and audio in  <frmRng>
// 2 dump/store the in frmae range video and audio raw frames into disk/memeory

int DemuxingDecoding2::decode_packet(int &nFrmsV, int &nFrmsA, int *got_frame, int cached, const bool isDecodeVideo, const bool isDecodeAudio)
{
int ret = 0;
int decoded = m_pkt.size;
bool isStoreCurFrm;

nFrmsV = 0;  //# of video frms decoded in this function call
nFrmsA = 0;  //# of audio frms decoded in this function call

*got_frame = 0;

if (isDecodeVideo && m_pkt.stream_index == m_video_stream_idx) {
/* decode video frame */
ret = avcodec_decode_video2(m_video_dec_ctx, m_frame, got_frame, &m_pkt);
if (ret < 0) {
xLog("DemuxingDecoding2::decode_packet(): Error decoding video frame (%s) in file:<%s>", my_av_err2str(ret).c_str(), m_src_filename.c_str());
return ret;
}

if (*got_frame) {
if (m_frame->width != m_width || m_frame->height != m_height || m_frame->format != m_pix_fmt) {
/* To handle this change, one could call av_image_alloc again and
* decode the following frames into another rawvideo file. */
xLog("Error: Width, height and pixel format have to be "
"constant in a rawvideo file, but the width, height or "
"pixel format of the input video changed:\n"
"old: width = %d, height = %d, format = %s\n"
"new: width = %d, height = %d, format = %s\n",
m_width, m_height, av_get_pix_fmt_name(m_pix_fmt),
m_frame->width, m_frame->height,
av_get_pix_fmt_name( (AVPixelFormat )m_frame->format));

return -1;
}


#if 0
{
char buf[32];
av_ts_make_time_string(buf, m_frame->pts, &m_video_dec_ctx->time_base);
printf("video_frame%s, m_video_frame_count_l=:%d, coded_picture_number=%d, pts=%s\n", 
cached ? "(cached)" : "", m_video_frame_count_l, m_frame->coded_picture_number, buf);
}
#endif
#if _DEBUG
assert(m_video_dst_bufsize == (m_width*m_height * 3) / 2);
#endif
//dispNum doese not work for *.MTS format
//cout << "dispNum=" << m_frame->display_picture_number << ", picNum=" << m_frame->coded_picture_number << ", myFrmNum=" << m_video_frame_count_l << endl;

/* copy decoded frame to destination buffer:
* this is required since rawvideo expects non aligned data */
av_image_copy(m_video_dst_data, m_video_dst_linesize, (const uint8_t **)(m_frame->data), m_frame->linesize, m_pix_fmt, m_width, m_height);

//create a newFrm, and store it
RawFrmPtr newFrm(new RawFrm(cv::Size(m_cfg->outputImgWidth, m_cfg->outputImgHeight), m_video_frame_count_g));
if (m_isResizeRawImg) {
resizeYuvVideoFrm(*newFrm.get(), m_video_dst_data[0], m_width, m_height, libyuv::kFilterBilinear);
}
else {
newFrm->copyFrom(m_video_dst_data[0], m_video_dst_bufsize, m_video_frame_count_g);
}
newFrm->keyFrmId_ = m_video_keyframe_count_g;
newFrm->offset_ = m_byteposition;
//dumpFrmImg( *newFrm.get(), "c:/temp", "frm");

m_vVideoFrms.push_back(newFrm);

if (m_frame->pict_type == AV_PICTURE_TYPE_I) {
m_video_keyframe_count_g++;
}

nFrmsV++;
m_video_frame_count_l++;
}
} 
else if (isDecodeAudio && m_pkt.stream_index == m_audio_stream_idx) {
/* decode audio frame */
ret = avcodec_decode_audio4(m_audio_dec_ctx, m_frame, got_frame, &m_pkt);
if (ret < 0) {
xLog("Error decoding audio frame (%s)", my_av_err2str(ret).c_str());
return ret;
}
/* Some audio decoders decode only part of the packet, and have to be
* called again with the remainder of the packet data.
* Sample: fate-suite/lossless-audio/luckynight-partial.shn
* Also, some decoders might over-read the packet. */
decoded = FFMIN(ret, m_pkt.size);

if (*got_frame) {
m_audio_dst_bufsize = m_frame->nb_samples * av_get_bytes_per_sample((AVSampleFormat)m_frame->format);
#if 0
char buf[256];
av_ts_make_string(buf, m_frame->pts /*, &m_audio_dec_ctx->time_base*/);
printf("audio_frame%s n:%d nb_samples:%d pts:%ld, wrtBufSz:%d\n",
cached ? "(cached)" : "",
m_audio_frame_count_g, m_frame->nb_samples, m_frame->pts, m_audio_dst_bufsize);
#endif

/* Write the raw audio data samples of the first plane. This works
* fine for packed formats (e.g. AV_SAMPLE_FMT_S16). However,
* most audio decoders output planar audio, which uses a separate
* plane of audio samples for each channel (e.g. AV_SAMPLE_FMT_S16P).
* In other words, this code will write only the first audio channel
* in these cases.
* You should use libswresample or libavfilter to convert the frame
* to packed data. */

RawFrmPtr aFrm(new  RawFrm(FrmType_A_FRM, m_audio_frame_count_g, m_audio_dst_bufsize));
aFrm->copyFrom(m_frame->extended_data[0], m_audio_dst_bufsize, m_audio_frame_count_g);
m_vAudioFrms.push_back(aFrm);
nFrmsA++;
}
}

/* If we use frame reference counting, we own the data and need
* to de-reference it when we don't use it anymore */
if (*got_frame && m_refcount) {
av_frame_unref(m_frame);
}

return decoded;
}

int DemuxingDecoding2:: open_codec_context(int *stream_idx, AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type)
{
int ret, stream_index;
AVStream *st;
AVCodec *dec = NULL;
AVDictionary *opts = NULL;

ret = av_find_best_stream(fmt_ctx, type, -1, -1, NULL, 0);
if (ret < 0) {
xLog("Could not find %s stream in input file %s", av_get_media_type_string(type), m_src_filename.c_str());
return ret;
} else {
stream_index = ret;
st = fmt_ctx->streams[stream_index];

/* find decoder for the stream */
dec = avcodec_find_decoder(st->codecpar->codec_id);
if (!dec) {
xLog("Failed to find %s codec", av_get_media_type_string(type));
return AVERROR(EINVAL);
}

/* Allocate a codec context for the decoder */
*dec_ctx = avcodec_alloc_context3(dec);
if (!*dec_ctx) {
xLog("Failed to allocate the %s codec context", av_get_media_type_string(type));
return AVERROR(ENOMEM);
}

/* Copy codec parameters from input stream to output codec context */
if ((ret = avcodec_parameters_to_context(*dec_ctx, st->codecpar)) < 0) {
xLog("Failed to copy %s codec parameters to decoder context", av_get_media_type_string(type));
return ret;
}

/* Init the decoders, with or without reference counting */
av_dict_set(&opts, "refcounted_frames", m_refcount ? "1" : "0", 0);
if ((ret = avcodec_open2(*dec_ctx, dec, &opts)) < 0) {
xLog("Failed to open %s codec", av_get_media_type_string(type));
return ret;
}
*stream_idx = stream_index;
}

return 0;
}

int DemuxingDecoding2:: get_format_from_sample_fmt(const char **fmt, enum AVSampleFormat sample_fmt)
{
int i;
struct sample_fmt_entry {
enum AVSampleFormat sample_fmt; const char *fmt_be, *fmt_le;
} sample_fmt_entries[] = {
{ AV_SAMPLE_FMT_U8,  "u8",    "u8"    },
{ AV_SAMPLE_FMT_S16, "s16be", "s16le" },
{ AV_SAMPLE_FMT_S32, "s32be", "s32le" },
{ AV_SAMPLE_FMT_FLT, "f32be", "f32le" },
{ AV_SAMPLE_FMT_DBL, "f64be", "f64le" },
};
*fmt = NULL;

for (i = 0; i < FF_ARRAY_ELEMS(sample_fmt_entries); i++) {
struct sample_fmt_entry *entry = &sample_fmt_entries[i];
if (sample_fmt == entry->sample_fmt) {
*fmt = AV_NE(entry->fmt_be, entry->fmt_le);
return 0;
}
}

xLog("sample format %s is not supported as output format", av_get_sample_fmt_name(sample_fmt));
return -1;
}


//output: m_vAudioFrms, m_vVideoFrms
int DemuxingDecoding2::decodeAll(const char* dstVideoFile, const char *dstAudioFile)
{
int ret = 0, got_frame;

if (m_cfg->dumpRawVideo) {
m_video_dst_file = fopen(dstVideoFile, "wb");
if (!m_video_dst_file) {
xLog("DemuxingDecoding2::decodeAll(): Could not open destination file %s", dstVideoFile);
return -1;
}
}
if (m_cfg->dumpRawAudio) {
m_audio_dst_file = fopen(dstAudioFile, "wb");
if (!m_audio_dst_file) {
xLog("DemuxingDecoding2::decodeAll(): Could not open destination file %s", dstAudioFile);
return -1;
}
}

if (m_video_stream) {
xLog("DemuxingDecoding2::decodeAll(): Demuxing video from file %s into %s", m_src_filename.c_str(), dstVideoFile);
}
if (m_audio_stream) {
xLog("DemuxingDecoding2::decodeAll(): Demuxing audio from file %s into %s", m_src_filename.c_str(), dstAudioFile);
}

//in decodeAll(), we want to store whole video raw  data in <m_vAudioFrms> / <m_vVideoFrms> if  m_cfg->storeRawAudioInMemo / m_cfg->storeRawVideoInMemo is set true
m_video_frame_count_g = 0;
m_video_keyframe_count_g = 0;
m_audio_frame_count_g = 0;
m_vAudioFrms.clear();
m_vVideoFrms.clear();
m_vThumbnailKeyFrms.clear();
/* read frames from the file */
int readFrmRet = 0;

//m_cfg->outputImgHeight=-1;m_cfg->outputImgWidth=-1
m_isResizeRawImg = false;
if (m_cfg->outputImgHeight > 0 && m_cfg->outputImgWidth > 0) {
if (m_cfg->outputImgHeight != m_width || m_cfg->outputImgHeight != m_height) {
m_isResizeRawImg = true;
}
}
while ( readFrmRet >= 0 ) {
m_byteposition = m_fmt_ctx->pb->pos;
readFrmRet = av_read_frame(m_fmt_ctx, &m_pkt);
AVPacket orig_pkt = m_pkt;
do {
ret = decode_packet(&got_frame, 0);
if (ret < 0) {
break;
}
m_pkt.data += ret;
m_pkt.size -= ret;
} while (m_pkt.size > 0);
av_packet_unref(&orig_pkt);
}

/* flush cached frames */
m_pkt.data = NULL;
m_pkt.size = 0;
do {
decode_packet(&got_frame, 1);
} while (got_frame);

xLog("DemuxingDecoding2::decodeAll(): Demuxing succeeded.");

if (m_video_stream) {
xLog("DemuxingDecoding2::decodeAll(): Play the output video file with the command:\n"
"ffplay -f rawvideo -pix_fmt %s -video_size %dx%d %s",
av_get_pix_fmt_name(m_pix_fmt), m_width, m_height, dstVideoFile);
}

if (m_audio_stream) {
enum AVSampleFormat sfmt = m_audio_dec_ctx->sample_fmt;
int n_channels = m_audio_dec_ctx->channels;
const char *fmt;

if (av_sample_fmt_is_planar(sfmt)) {
const char *packed = av_get_sample_fmt_name(sfmt);
xLog("DemuxingDecoding2::decodeAll(): Warning: the sample format the decoder produced is planar "
"(%s). This example will output the first channel only.",
packed ? packed : "?");
sfmt = av_get_packed_sample_fmt(sfmt);
n_channels = 1;
}

if ((ret = get_format_from_sample_fmt(&fmt, sfmt)) < 0) {
return -1;
}

xLog("DemuxingDecoding2::decodeAll(): Play the output audio file with the command:\n"
"ffplay -f %s -ac %d -ar %d %s",
fmt, n_channels, m_audio_dec_ctx->sample_rate, dstAudioFile);
}

return 0;
}


//-------------------------------------------------
//decode at least one frm of video from current location
//outptut: m_vVideoFrms
//return: <0 no more data to decode (end of file), otherwise >=0
//-------------------------------------------------
//results are satored in <m_vVideoFrms>
int DemuxingDecoding2::decodeSomeVideo(const int nMinVideoFrms, const bool isDecodeAudio)
{
return decodeSome(true, nMinVideoFrms, isDecodeAudio, 0);
}

int DemuxingDecoding2::decodeSome(const bool isDecodeVideo, const int nMinVideoFrms, const bool isDecodeAudio, const int nMinAudioFrms )
{
int ret = 0;
int readFrmRet = 0;
int got_frame = 0;

int aFrmCount=0, aTotFrms = 0, vFrmCount = 0, vTotFrms = 0;

m_video_frame_count_g = 0;
m_audio_frame_count_g = 0;
m_vVideoFrms.clear();
m_vAudioFrms.clear();
while (readFrmRet >= 0) {
//int64_t byteposition = m_fmt_ctx->pb->pos;
//cout << "byteposition=" << byteposition << endl;

readFrmRet = av_read_frame(m_fmt_ctx, &m_pkt);
AVPacket orig_pkt = m_pkt;
do {
//do not change
//note: add these two flags, it is much efficent then clean() and push_back() data into the vetcors: <m_vAudioFrms> and <m_vVideoFrms> 
ret = decode_packet(vFrmCount, aFrmCount, &got_frame, 0, isDecodeVideo, isDecodeAudio);
if (ret < 0) {
break;
}
m_pkt.data += ret;
m_pkt.size -= ret;

vTotFrms += vFrmCount;
aTotFrms += aFrmCount;
} while (m_pkt.size > 0);
av_packet_unref(&orig_pkt);

//note: m_audio_frame_count_g = 0 was set in openVideoFileToRead()
if (vTotFrms >= nMinVideoFrms && aTotFrms >= nMinAudioFrms) {
break;
}
}

size_t n = m_vVideoFrms.size();
if (n>0) {
//todo: this is costy, need a way to improve
m_latestDecodedVideoFrm = m_vVideoFrms[n - 1];
}

if (readFrmRet<0) {
cout << m_src_filename << endl;
m_latestDecodedVideoFrm->fn_ = LAST_FRM_NUM;            //add extra frame at the end, and mark its <fn_>
RawFrmPtr tmp(new RawFrm(*m_latestDecodedVideoFrm.get()));
m_vVideoFrms.push_back(tmp);
}
return readFrmRet;
}


int DemuxingDecoding2::openVideoFileToRead(const char *srcVideoFile, const bool isRefCount )
{
int ret = 0;
if (m_isOpened) {
closeOpenedFiles();
}


m_refcount = 0;
if (isRefCount) {
//note: this is not tesed 
xLog( "DemuxingDecoding2::openVideoFileToRead(): cannot handle isRefCount=%d, exit(1) called!", isRefCount);
m_refcount = 1;
return - 1;
}

m_src_filename = std::string(srcVideoFile);

/* register all formats and codecs */
av_register_all();

/* open input file, and allocate format context */
if (avformat_open_input(&m_fmt_ctx, srcVideoFile, NULL, NULL) < 0) {
xLog("Could not open source file %s", srcVideoFile);
return -1;
}

/* retrieve stream information */
if (avformat_find_stream_info(m_fmt_ctx, NULL) < 0) {
xLog("Could not find stream information");
return -1;
}

if (open_codec_context(&m_video_stream_idx, &m_video_dec_ctx, m_fmt_ctx, AVMEDIA_TYPE_VIDEO) >= 0) {
m_video_stream = m_fmt_ctx->streams[m_video_stream_idx];

/* allocate image where the decoded image will be put */
m_width = m_video_dec_ctx->width;
m_height = m_video_dec_ctx->height;
m_pix_fmt = m_video_dec_ctx->pix_fmt;

//uint64 t1;
//t1 = m_video_dec_ctx->time_base.num;
//t1 = m_video_dec_ctx->time_base.den;
//t1 = m_video_dec_ctx->framerate.num;  
//t1 = m_video_dec_ctx->framerate.den;
//t1 = m_fmt_ctx->duration;         //t = duration/AV_TIME_BASE (sec)
//t1 = m_fmt_ctx->start_time; 

//m_video_stream->avg_frame_rate;
//m_video_stream->r_frame_rate;


ret = av_image_alloc(m_video_dst_data, m_video_dst_linesize, m_width, m_height, m_pix_fmt, 1);
if (ret < 0) {
xLog("Could not allocate raw video buffer");
return -1;
}
m_video_dst_bufsize = ret;  //raw videod data size
assert(m_video_dst_bufsize = (3*m_width *m_height)/2 );
}

if (open_codec_context(&m_audio_stream_idx, &m_audio_dec_ctx, m_fmt_ctx, AVMEDIA_TYPE_AUDIO) >= 0) {
//importtant audio properties
//m_audio_dec_ctx->sample_fmt;
//m_audio_dec_ctx->sample_rate;
//m_audio_dec_ctx->bit_rate;
//m_audio_dec_ctx->channels;
//m_audio_dec_ctx->channels=2,, here we only need one channel
//todo: how to get audio frm size (in bytes) at here?
m_audio_dst_bufsize =  4096;
m_audio_stream = m_fmt_ctx->streams[m_audio_stream_idx];
}

/* dump input information to stderr */
av_dump_format(m_fmt_ctx, 0, srcVideoFile, 0);

if (!m_audio_stream && !m_video_stream) {
xLog("Could not find audio or video stream in the input, aborting");
return -1;
}

m_frame = av_frame_alloc();
if (!m_frame) {
xLog("Could not allocate frame");
ret = AVERROR(ENOMEM);
return -1;
}

/* initialize packet, set data to NULL, let the demuxer fill it */
av_init_packet(&m_pkt);
m_pkt.data = NULL;
m_pkt.size = 0;

if (m_video_stream || m_audio_stream) {
xLog("Demuxing video from file %s", srcVideoFile);
}


update_video_streamInfo();
update_audio_streamInfo();

m_isOpened = true;
m_video_frame_count_g = 0;
m_audio_frame_count_g = 0;
m_vFrmIdxQ.clear();
m_videoTimeBase = (int64_t) (m_video_stream->time_base.num * AV_TIME_BASE / m_video_stream->time_base.den);
}

int DemuxingDecoding2::closeOpenedFiles()
{
avcodec_free_context(&m_video_dec_ctx);
avcodec_free_context(&m_audio_dec_ctx);
avformat_close_input(&m_fmt_ctx);

if (m_video_dst_file) {
fclose(m_video_dst_file);
m_video_dst_file = NULL;
}

if (m_audio_dst_file) {
fclose(m_audio_dst_file);
m_audio_dst_file = NULL;
}

av_frame_free(&m_frame);
av_free(m_video_dst_data[0]);
m_isOpened = false;

return 0;
}

void DemuxingDecoding2::update_video_streamInfo()
{
if (m_video_stream) {
m_videoStreamInfo.time_base = m_video_stream->time_base;
m_videoStreamInfo.start_time = m_video_stream->start_time;  //m_fmt_ctx->start_time
m_videoStreamInfo.duration = m_video_stream->duration;    //m_fmt_ctx->duration;
m_videoStreamInfo.frame_rate = m_video_stream->avg_frame_rate;
m_videoStreamInfo.frmSzInBytes = m_video_dst_bufsize;
}
if(m_video_dec_ctx){
m_videoStreamInfo.w = m_video_dec_ctx->width;
m_videoStreamInfo.h = m_video_dec_ctx->height;
m_videoStreamInfo.pix_fmt = m_video_dec_ctx->pix_fmt;

uint64_t b = (m_fmt_ctx->bit_rate > 4000000) ? m_fmt_ctx->bit_rate : 4000000;
if (b < m_video_dec_ctx->bit_rate) {
b = m_video_dec_ctx->bit_rate;
}
m_videoStreamInfo.bit_rate = b;
}
m_videoStreamInfo.totFrms = m_videoStreamInfo.get_total_video_frms();
}

void DemuxingDecoding2::update_audio_streamInfo()
{
if (m_audio_stream) {
m_audioStreamInfo.time_base = m_audio_stream->time_base;
m_audioStreamInfo.start_time = m_audio_stream->start_time;  //m_fmt_ctx->start_time
m_audioStreamInfo.duration = m_audio_stream->duration;    //m_fmt_ctx->duration;
m_audioStreamInfo.frmSzInBytes = m_audio_dst_bufsize;
m_audioStreamInfo.smpingFreq = m_audio_dec_ctx->sample_rate;
m_audioStreamInfo.nb_samples_perfrm = getAudioFrmSamples();

m_audioStreamInfo.channels = m_audio_stream->codec->channels;
m_audioStreamInfo.sample_rate = m_audio_stream->codec->sample_rate;
m_audioStreamInfo.sample_fmt = m_audio_stream->codec->sample_fmt;
m_audioStreamInfo.ch_layout = m_audio_stream->codec->channel_layout;
m_audioStreamInfo.bit_rate = 64000; //m_audio_dec_ctx->bit_rate

m_audioStreamInfo.totFrms = m_audioStreamInfo.get_total_audio_frms();
}
//for video only
}

void DemuxingDecoding2::dumpFrmIndexQ(const std::string &fname)
{
FILE *fp = fopen(fname.c_str(), "w");
BOOST_FOREACH(const FrmIdx &x, m_vFrmIdxQ) {
fprintf( fp, "%c,%d,%d,%d\n", (x.type == FrmType_V_FRM) ? 'V' : 'A', x.idx, x.nsamples, x.frmSzInByte);
}
fclose(fp);
}

void DemuxingDecoding2::printProperties()
{
uint64_t aFrmSzInBytes = getRawAudioFrameSize();
uint32_t vFrmSzInBytes = getRawVideoFrameSize();
int64_t nDuration = getDuration();
uint32 nTotVideoFrms = getTotalVideoFrms();
uint32 nTotAudioFrms = getTotalAudioFrms();

xLog( "m_video_dec_ctx: time base=(%d / %d), frame rate=(%d / %d)\n",  
m_video_dec_ctx->time_base.num,	m_video_dec_ctx->time_base.den, 
m_video_dec_ctx->framerate.num, m_video_dec_ctx->framerate.den
);

float32 dt = getVideoDurationSec();
uint32  n = ceil(dt * m_video_stream->avg_frame_rate.num/ m_video_stream->avg_frame_rate.den);
xLog("m_video_stream: time base=(%d / %d), frame rate=(%d / %d),\n duration=(%lld,%f(sec)), start_time=%lld, nb_frames=%d, nTotFrms=%d\n",
m_video_stream->time_base.num, m_video_stream->time_base.den,
m_video_stream->avg_frame_rate.num, m_video_stream->avg_frame_rate.den,
m_video_stream->duration, dt, m_video_stream->start_time, 
m_video_stream->nb_frames, n
);

dt = m_audio_stream->duration * m_audio_stream->time_base.num / (float)m_audio_stream->time_base.den;
n =  ceil(dt * m_audio_stream->avg_frame_rate.num / m_audio_stream->avg_frame_rate.den + 1e-7);
xLog("m_audio_stream: time base=(%d / %d), frame rate=(%d / %d),\n duration=(%lld,%f(sec)), start_time=%lld, nb_frames=%d, nTotFrms=%d\n",
m_audio_stream->time_base.num, m_audio_stream->time_base.den,
m_audio_stream->avg_frame_rate.num, m_audio_stream->avg_frame_rate.den,
m_audio_stream->duration, dt, m_audio_stream->start_time,
m_audio_stream->nb_frames, n
);

xLog("m_fmt_ctx: nTotVideoFrms=%d,nTotAudioFrms=%d\n",nTotVideoFrms, nTotAudioFrms);
xLog("aFrmSzInBytes=%d, vFrmSzInBytes=%d, nDuration=%d, %f\n", aFrmSzInBytes, vFrmSzInBytes, m_fmt_ctx->duration, getCtxDurationSec());
}

uint64 DemuxingDecoding2::seekVideoFrm(const float64 timeSec)
{
uint64 keyFrmNum = 0, keyFrmIdx = 0;
//todo: find <keyFrmNum> 
seekVideo(timeSec, keyFrmNum, keyFrmIdx);
return keyFrmNum;
}

float64 DemuxingDecoding2::seekVideoFrm(const uint64 keyFrmIdx)
{
uint64 keyFrmNum = getVideoFrmIdxFromKeyFrmIdx( keyFrmIdx );
float64 t_sec = getVideoTimeInSec(keyFrmNum);
seekVideo( t_sec, keyFrmNum, keyFrmIdx);
return t_sec;
}

bool DemuxingDecoding2::seekVideo(const float64 newTimeSec, const uint64 keyFrmNum, const uint64 keyFrmIdx)
{

#if _DEBUG
boost::posix_time::ptime t1 = POSIX_LOCAL_TIME;
#endif

int64_t fn1 = av_rescale((int64_t)round(newTimeSec * 1000), m_video_stream->time_base.den, m_video_stream->time_base.num);
fn1 /= 1000;
//int64_t fn2 = newTimeSec / av_q2d(m_video_stream->time_base);
#if 1
//int ret = avformat_seek_file(m_fmt_ctx, m_video_stream_idx, INT64_MIN, fn1, INT64_MAX, 0);
int ret = avformat_seek_file(m_fmt_ctx, m_video_stream_idx, fn1, fn1, fn1, 0);
if ( ret< 0) {
xLog("DemuxingDecoding2::seekVideoFrm(): ERROR av_seek_frame: %u\n", fn1);
}
else {
//xLog("DemuxingDecoding2::seekVideoFrm(): SUCCEEDED av_seek_frame: %u newPos:%d\n", fn1, m_fmt_ctx->pb->pos);
avcodec_flush_buffers(m_video_dec_ctx);
}
#else
//inccorectm gaven up
uint64_t fn = getVideoFrmNumFromTime(newTimeSec);
int64_t timeBase = (int64_t(m_video_stream->time_base.num) * AV_TIME_BASE) / int64_t(m_video_stream->time_base.den);
int64_t seekTarget1 = fn * timeBase;
int64_t seekTarget2 = fn * m_videoTimeBase;
if (av_seek_frame(m_fmt_ctx, m_video_stream_idx, seekTarget1, AVSEEK_FLAG_FRAME) < 0) {
xLog("DemuxingDecoding2::seekVideoFrm(): av_seek_frame() failed for %s", m_src_filename.c_str());
return false;
}
#endif
m_video_frame_count_l = keyFrmNum;
m_video_keyframe_count_g = keyFrmIdx;

#if _DEBUG
uint32  dt = timeIntervalMillisec(t1);
int64_t bytePosition = m_fmt_ctx->pb->pos;

xLog("DemuxingDecoding2::seekVideoFrm(): time used: %d (ms), bytePosition=%lld, newTimeSec=%f, keyFrmNum=%d,keyFrmIdx=%d, file=%s",
dt, bytePosition, newTimeSec, keyFrmNum, keyFrmIdx, m_src_filename.c_str());
#endif
return true;
}


bool DemuxingDecoding2::seekByOffset(const int64_t offset, const uint64 keyFrmNum, const uint64 keyFrmIdx) {
int64_t newP = avio_seek(m_fmt_ctx->pb, offset, SEEK_SET);
if (newP < 0) {
char errbuf[1024];
av_make_error_string(errbuf, 1024, -newP);
xLog( "DemuxingDecoding2::seekByOffset(): %s", errbuf );
return false;
}
avcodec_flush_buffers(m_video_dec_ctx);
m_video_frame_count_l = keyFrmNum;
m_video_keyframe_count_g = keyFrmIdx;
return true;
}


bool  DemuxingDecoding2::getFirstFrmImg(cv::Mat &img)
{
if (m_firstVideoFrm == 0) {
return false;
}
m_firstVideoFrm->toBGR(img);
}

bool  DemuxingDecoding2::getThumbnailKeyFrm(const uint64 keyFrmIdx, RawFrmPtr &frm)
{
if (keyFrmIdx >= m_vThumbnailKeyFrms.size()) {
return false;
}
frm = m_vThumbnailKeyFrms[keyFrmIdx];
return true;
}

uint64 DemuxingDecoding2::getVideoFrmIdxFromKeyFrmIdx(const uint64 keyFrmIdx) {
if (keyFrmIdx >= 0 && keyFrmIdx < m_vThumbnailKeyFrms.size()) {
return m_vThumbnailKeyFrms[keyFrmIdx]->fn_;
}
else {
return UINT64_NAN;
}
}
uint64 DemuxingDecoding2::getVideoKeyFrmIdxFromFrmIdx(const uint64 frmIdx) 
{
if (m_vThumbnailKeyFrms.empty()) {
return UINT64_NAN;
}

if (frmIdx == 0) {
return 0;
}

uint32 k, k1 = 0, k2 = m_vThumbnailKeyFrms.size() - 1;
if (frmIdx < m_vThumbnailKeyFrms[k1]->fn_ || frmIdx > m_vThumbnailKeyFrms[k2]->fn_) {
return UINT64_NAN;
}
while (1) {
k = (k1 + k2) / 2;
if ((k2 - k1) == 1) {
break;
}

if (m_vThumbnailKeyFrms[k]->fn_ < frmIdx) {
k1 = k;
}
else if (m_vThumbnailKeyFrms[k]->fn_ > frmIdx) {
k2 = k;
}
else {
break;
}
}
return k;
}

#include "libutil/RawFrmFileIo.h"
bool DemuxingDecoding2::loadDecodedAudioVideoFrms()
{
bool suc[5];
RawFrmFileIoPtr x(new RawFrmFileIo());

suc[0] = loadAudioStreamInfo(m_decodedProfileFiles.m_audioStreamInfoFile, m_audioStreamInfo);
m_vAudioFrms.clear();
suc[1] = x->loadRawFrms(m_decodedProfileFiles.m_audioFrmFile, m_vAudioFrms);

suc[2] = loadVideoStreamInfo(m_decodedProfileFiles.m_videoStreamInfoFile, m_videoStreamInfo);
m_vThumbnailKeyFrms.clear();
suc[3] = x->loadRawFrms( m_decodedProfileFiles.m_videoKeyFrmFile, m_vThumbnailKeyFrms );

m_vFrmIdxQ.clear();
suc[4] = x->loadFrmIdxQ(m_decodedProfileFiles.m_frmIdxFile, m_vFrmIdxQ);

m_width	= m_videoStreamInfo.w;
m_height = m_videoStreamInfo.h;
return (suc[0] && suc[1] && suc[2] && suc[3] && suc[4]);
}

bool DemuxingDecoding2::saveDecodedAudioVideoFrms()
{
RawFrmFileIoPtr x(new RawFrmFileIo());
saveAudioStreamInfo(m_decodedProfileFiles.m_audioStreamInfoFile, m_audioStreamInfo);
x->saveRawFrms(m_decodedProfileFiles.m_audioFrmFile, m_vAudioFrms);

saveVideoStreamInfo(m_decodedProfileFiles.m_videoStreamInfoFile, m_videoStreamInfo);
x->saveRawFrms(m_decodedProfileFiles.m_videoKeyFrmFile, m_vThumbnailKeyFrms);

x->saveFrmIdxQ(m_decodedProfileFiles.m_frmIdxFile, m_vFrmIdxQ);
return true;
}

void DemuxingDecoding2::rewindToBegining()
{
bool flag = avformat_seek_file(m_fmt_ctx, m_video_stream_idx, INT64_MIN, 0, INT64_MAX, 0);
if ( flag < 0) {
xLog("DemuxingDecoding2::rewindToBegining(): ERROR av_seek_frame: 0\n");
return;
}
//xLog("DemuxingDecoding2::rewindToBegining(): SUCCEEDED av_seek_frame: %u newPos:%d\n", 0, m_fmt_ctx->pb->pos);
avcodec_flush_buffers(m_video_dec_ctx);
avcodec_flush_buffers(m_audio_dec_ctx);
}


void DemuxingDecoding2::readFirstVideoFrmThenRewindToBegining()
{
int ret = decodeSome(true,1, true,1);
update_video_streamInfo();
update_audio_streamInfo();

assert(m_vVideoFrms.size() >= 1);
m_firstVideoFrm = m_vVideoFrms[0];

//todo: doest not work, ignor the 1st frm in analysis
//rewindToBegining();
}
