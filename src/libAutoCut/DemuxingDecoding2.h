/*
*-------------------------------------------------------------------
* DemuxingDecoding2.h
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

/**
* @file
* DemuxingDecoding2.h
*
* shunguang wu  1/14/2018
* based on the above ope source code
*/

#ifndef __DEMUXING_DECODING2_H__
#define __DEMUXING_DECODING2_H__


extern "C" {
#define __STDC_CONSTANT_MACROS
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
}
#define WRT_RAW_VIDEO 0

#include "libUtil/DataTypes.h"
#include "libUtil/RawFrm.h"
#include "libUtil/VisLog.h"
#include "libUtil/CfgAc.h"

#include "avUtils.h"


namespace autocut {

class DemuxingDecoding2 {
public:
DemuxingDecoding2( CfgDemuxingDecodingPtr  cfg );
~DemuxingDecoding2();

void readFirstVideoFrmThenRewindToBegining();
void rewindToBegining();

void setDecodedProfileFileNames(const DecodedProfileFiles &prof) {
m_decodedProfileFiles = prof;
}
bool saveDecodedAudioVideoFrms();
bool loadDecodedAudioVideoFrms();

int openVideoFileToRead(const char *srcVideoFile, const bool isRefCount = false);

//decode all video and store the raw video and audioe into file
int decodeAll(const char* dstVideoFile, const char *dstAudioFile);

uint64 seekVideoFrm(const float64 timeSec);  // return keyFrmNum at <newTimeSec>
float64 seekVideoFrm(const uint64 keyFrmIdx);  //return time at <keyFrmIdx>
bool seekByOffset(const int64_t offset, const uint64 keyFrmNum, const uint64 keyFrmIdx);

//decode at least one frm of video from current location
//results are satored in <m_vVideoFrms>
int decodeSomeVideo(const int nMinVideoFrms, const bool isDecodeAudio);
int decodeSome(const bool isDecodeVideo, const int nMinVideoFrms, const bool isDecodeAudio, const int nMinAudioFrms);
int closeOpenedFiles();
void printProperties();

uint64_t getRawAudioFrameSize() {
return m_audio_dst_bufsize;
}
/*
*note video frm size typically is not equal w*h*3, which is for RBG24 format
*YUV444    3 bytes per pixel     (12 bytes per 4 pixels)
*YUV422    4 bytes per 2 pixels  ( 8 bytes per 4 pixels)
*YUV411    6 bytes per 4 pixels
*YUV420p   6 bytes per 4 pixels, reordered
*/
uint32_t getRawVideoFrameSize() const {
return m_video_dst_bufsize;
}

AVFormatContext getAvFtmCtx() const {
AVFormatContext ret = *m_fmt_ctx;
return ret;
}

AVCodecContext getVideoCodecContext() const {
AVCodecContext ret = *m_video_dec_ctx;
return ret;
}
AVCodecContext getAudioCodecContext() const {
AVCodecContext ret = *m_audio_dec_ctx;
return ret;
}

int64_t getDuration() {
return m_fmt_ctx->duration;
}

uint64 getTotKeyFrms() {
return m_vThumbnailKeyFrms.size();
}

float getCtxDurationSec() {
return m_fmt_ctx->duration / (float)AV_TIME_BASE;
}

float32 getVideoDurationSec() {
float32 dt = m_video_stream->duration*m_video_stream->time_base.num / (float)m_video_stream->time_base.den;
return dt;
}

int getAudioSamplingFreq() const {
return m_audio_dec_ctx->sample_rate;
}

uint64 getAudioFrmSamples() const {
uint64 n = 0;
std::vector<FrmIdx>::const_iterator it = m_vFrmIdxQ.begin();
for (; it != m_vFrmIdxQ.end(); ++it) {
if (it->type == FrmType_A_FRM) {
n = it->nsamples;
break;
}
}
return n;
}

float64 getAudioTimeSecFromFrmIdx(const uint64 frmIdx) {
return m_audioStreamInfo.getAudioTimeSec(frmIdx);
}

bool  getFirstFrmImg(cv::Mat &img);
bool  getThumbnailKeyFrm(const uint64 keyFrmIdx, RawFrmPtr &frm);
uint64 getVideoFrmIdxFromKeyFrmIdx(const uint64 keyFrmIdx);

uint64 getVideoKeyFrmIdxFromFrmIdx(const uint64 frmIdx);

float64 getVideoTimeInSec(const uint64 frmNum) {
float64 t = frmNum * ((float32)m_video_stream->avg_frame_rate.den/ (float32)m_video_stream->avg_frame_rate.num);
return t;
}

uint64 getVideoFrmNumFromTime(const float64 t) {
uint64 fn =  (t * m_video_stream->avg_frame_rate.num) / m_video_stream->avg_frame_rate.den;
return fn;
}

//we need the the format info to convert to RGB24 format
AVPixelFormat getRawVideoFrameFormat() const {
return m_pix_fmt;
}

//need to scan the while file first
uint32 getTotalAudioFrms() const { 
return  m_audio_frame_count_g; 
}

//need to scan the while file first
uint32 getTotalVideoFrms() const {
return  m_video_frame_count_g;
}

//make sure video cliips is inside audio range
bool findVideoFrmIds(const uint32 aId1, const uint32 aId2, uint32 &vId1, uint32 &vId2 );  
void dumpFrmIndexQ(const std::string &fname);

void setCfg( CfgDemuxingDecodingPtr  &cfg) {
m_cfg = cfg;
resizeThumnailKeyFrm();
}

void update_video_streamInfo();
void update_audio_streamInfo();

private:
/* Enable or disable frame reference counting. You are not supposed to support
* both paths in your application but pick the one most appropriate to your
* needs. Look for the use of m_refcount in this example to see what are the
* differences of API usage between them. */
bool seekVideo(const float64 newTimeSec, const uint64 keyFrmNum, const uint64 keyFrmIdx);
int decode_packet(int *got_frame, int cached);
int decode_packet(int &nFrmsV, int &nFrmsA, int *got_frame, int cached, const bool isDecodeVideo, const bool isDecodeAudio);
int open_codec_context(int *stream_idx, AVCodecContext **dec_ctx, AVFormatContext *fmt_ctx, enum AVMediaType type);
int get_format_from_sample_fmt(const char **fmt, enum AVSampleFormat sample_fmt);
void resizeThumnailKeyFrm();

public:
CfgDemuxingDecodingPtr  m_cfg;
std::string				m_src_filename;

MyAudioStreamInfo		m_audioStreamInfo;
MyVideoStreamInfo		m_videoStreamInfo;

//for disp thumnail images
RawFrmPtr				 m_firstVideoFrm;
RawFrmPtr				 m_latestDecodedVideoFrm;

cv::Size                 m_keyFrmSz; 
std::vector<RawFrmPtr>   m_vThumbnailKeyFrms;  //all thumnail imgs of the video file are stored in memeory when reading the audio info

std::vector<RawFrmPtr> m_vAudioFrms;       //raw audio/video frms read in each decode_packet() call if it cleared before calling decode_packet(),
std::vector<RawFrmPtr> m_vVideoFrms;       //otherwise it holds the whole video raw if cfg.storeRawAudioInMemo is set true

std::vector<FrmIdx> m_vFrmIdxQ;		     //entire video file  both video and audio FrmIdx table: this can be used to find corrspondences between video and audio frames
int m_width, m_height;                   //roiginal img size 

private:
enum AVPixelFormat	m_pix_fmt;
int					m_refcount;

AVStream *m_video_stream;
AVStream *m_audio_stream;
AVFormatContext *m_fmt_ctx;
AVCodecContext *m_video_dec_ctx;
AVCodecContext *m_audio_dec_ctx;

uint8_t *m_video_dst_data[4];
int      m_video_dst_linesize[4];
int		 m_video_dst_bufsize;

int		 m_audio_dst_bufsize;
int		 m_video_stream_idx;
int		 m_audio_stream_idx;

AVFrame *m_frame;
AVPacket m_pkt;
uint32	 m_video_frame_count_g;   //gloabl count for whole file after scaning all video frmaes
uint32	 m_audio_frame_count_g;   //gloabl count for whole file after scaning all audio frmaes
uint32   m_video_frame_count_l;   //local video frame count,  initialized by seekVideoFrm()
uint32   m_video_keyframe_count_g;   //key frm count
FILE *m_video_dst_file;
FILE *m_audio_dst_file;

//if the video file is opened
bool					m_isResizeRawImg;
bool					m_isOpened;
uint64_t				m_videoTimeBase;
DecodedProfileFiles	m_decodedProfileFiles;
int64_t					m_byteposition;
};
typedef std::shared_ptr<DemuxingDecoding2>		DemuxingDecoding2Ptr;
}
#endif
