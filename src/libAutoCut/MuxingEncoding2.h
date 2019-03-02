/*
*-------------------------------------------------------------------
* MuxingEncoding2.h
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
* libavformat API example.
*
* Output a media file in any supported libavformat format. The default
* codecs are used.
* @example muxing.c
*/

#ifndef __MUXING_ENCODING2_H__
#define __MUXING_ENCODING2_H__
extern "C" {
#define __STDC_CONSTANT_MACROS
#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}
#include "libUtil/DataTypes.h"
#include "libUtil/RawFrm.h"
#include "libUtil/VisLog.h"
#include "avUtils.h"
#include "OutputStream.h"
namespace autocut {
class MuxingEncoding2 {
public:
MuxingEncoding2();
~MuxingEncoding2();

int openOutputFile(const char *outFile, const MyVideoStreamInfo  &vInfo, const MyAudioStreamInfo &aInfo);

int wrtSomeFrms(const uint8 *rawVideo, const uint64 vSz0, const uint64 vFrmSz, const uint8 *rawAudio, const uint64 aSz0, const uint64 aFrmSz);
int wrtSomeFrms(const std::vector<RawFrmPtr> &vVideoFrms, const std::vector<RawFrmPtr> &vAudioFrms, const int nChs);
int wrtSomeFrms(const std::vector<RawFrmPtr> &vVideoFrms, const uint8 *rawAudio, const uint64 rawAudioSz, const uint32 aFrmSzInByte);

int testByWrtSynFrms();
int closeOpenedFile();
bool isOpened() { 
return m_isOpen;
}
private:

int write_frame(AVFormatContext *fmt_ctx, const AVRational *time_base, AVStream *st, AVPacket *pkt);

/* Add an output stream. */
void add_stream(OutputStream *ost, AVFormatContext *oc, AVCodec **codec, enum AVCodecID codec_id);


AVFrame *alloc_audio_frame(enum AVSampleFormat sample_fmt, uint64_t channel_layout, int sample_rate, int nb_samples);
void open_audio(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg);
/* Prepare a 16 bit dummy audio frame of 'frame_size' samples and 'nb_channels' channels. */
AVFrame *get_audio_frame(OutputStream *ost);
AVFrame *get_audio_frame2(OutputStream *ost, const uint8 *buf, const uint32 bufSz, const int nChs = 1);  //read audio data from  <m_rawAudio>
/*
* encode one audio frame and send it to the muxer
* return 1 when encoding is finished, 0 otherwise
*/
int write_audio_frame(AVFormatContext *oc, OutputStream *ost, const uint8 *buf, const uint32 bufSz, const int nChs=1 );

//-- for video -------------
AVFrame *alloc_picture(enum AVPixelFormat pix_fmt, int width, int height);
void open_video(AVFormatContext *oc, AVCodec *codec, OutputStream *ost, AVDictionary *opt_arg);
/* Prepare a dummy image. */
void fill_yuv_image(AVFrame *pict, int frame_index, int width, int height);
AVFrame *get_video_frame(OutputStream *ost);   //synthetic dummy image
AVFrame *get_video_frame2(OutputStream *ost, const uint8 *buf, const uint32 sz);  

/*
* encode one video frame and send it to the muxer
* return 1 when encoding is finished, 0 otherwise
*/
int write_video_frame(AVFormatContext *oc, OutputStream *ost, const uint8 *buf, const uint32 bufSz );
void close_stream(AVFormatContext *oc, OutputStream *ost);

public:
std::string  m_outputVideoFilePath;

private:
bool         m_isOpen;
OutputStream m_video_st;
OutputStream m_audio_st;
AVOutputFormat *m_fmt;
AVFormatContext *m_oc;
int m_have_video;
int m_have_audio;
int m_encode_video;
int m_encode_audio;

bool m_useSynData;
MyVideoStreamInfo   m_vInfo;
MyAudioStreamInfo   m_aInfo;
};
typedef std::shared_ptr<MuxingEncoding2>		MuxingEncoding2Ptr;
}
#endif
