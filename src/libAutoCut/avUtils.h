/*
*-------------------------------------------------------------------
* avUtils.h
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

#ifndef __AV_UTILS_H__
#define __AV_UTILS_H__

#include "libYuv/libyuv.h"

#include "libUtil/DataTypes.h"
#include "libUtil/VisLog.h"
#include "libUtil/UtilFuncs.h"
#include "libUtil/RawFrm.h"

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

namespace autocut {
typedef struct {
int width;
int height;
uint8_t *data;  //[RGB ...RGB]
size_t size;
} ppm_image;

struct DecodedProfileFiles {
std::string	m_audioFrmFile;
std::string	m_videoKeyFrmFile;
std::string	m_audioStreamInfoFile;
std::string	m_videoStreamInfoFile;
std::string	m_frmIdxFile;
};

struct MyAudioStreamInfo {
AVRational time_base;
uint64     start_time;
uint64     duration;
uint64     bit_rate;

//for audio only
int channels;
int sample_rate;
enum AVSampleFormat sample_fmt;
int nb_samples_perfrm;  //# of smaple in one frm
int ch_layout;

uint64  totFrms;
uint64	frmSzInBytes;
int		smpingFreq;   //41000., etc

//---------------------------
uint64 getAudioRawFrmSizeInBytes() const {
return (uint64)nb_samples_perfrm * (uint64)channels * (uint64)av_get_bytes_per_sample(sample_fmt);
}
uint64 get_total_audio_frms() {
double t = ((double)duration / time_base.den) * time_base.num;
double fr = (double)sample_rate / (double)nb_samples_perfrm;
uint64 x = ceil(t*fr);
return x;
}

float64 getAudioTimeSec(const uint64 frmIdx) {
float64 t = (float64)(frmIdx * nb_samples_perfrm) / (float64)sample_rate;
return t;
}
float64 getAudioTimeSec2(const uint64 rawSmpIdx) {
float64 t = (float64)(rawSmpIdx) / (float64)sample_rate;
return t;
}

std::string toString(const std::string &msg) const {
char buf[1024];
sprintf_s(buf, 1024, "time_base[num=%d, den=%d],start_time=%llu, duration=%llu, channels=%d,sample_rate=%d, sample_fmt=%d, nb_samples_perfrm=%d, ch_layout=%d",
time_base.num, time_base.den, start_time, duration, channels, sample_rate, sample_fmt, nb_samples_perfrm, ch_layout);
return msg + std::string(buf);
}
};

struct MyVideoStreamInfo {
AVRational time_base;
uint64     start_time;
uint64     duration;
uint64     bit_rate;

//for video only
AVRational  frame_rate;
int w;
int h;
AVPixelFormat	pix_fmt;
uint64			frmSzInBytes;
uint64			totFrms;
//todo: condering different pix_fmt
uint32 getVideoRawFrmSizeInBytes() const {
return 3 * (w*h / 2);
}
float64 getVideoDurationSec() const {
float64 dt = time_base.num * ((float64)duration / (float64)time_base.den);
return dt;
}
//return frms/sec
float64 getVideoFrmRate() const {
return (float64)frame_rate.num / (float64)frame_rate.den;
}

uint64 get_total_video_frms() {
float64 fr = (float64)frame_rate.num / (float64)frame_rate.den;
float64 t = (float64)duration / time_base.den * time_base.num;
uint64 x = ceil(t*fr);
return x;
}

float64 getVideoTimeInSec(const uint64 frmNum) {
float64 t = frmNum * ((float64)frame_rate.den / (float64)frame_rate.num);
return t;
}

uint64 getVideoFrmNumFromTime(const float64 t) {
uint64 fn = (t * frame_rate.num) / frame_rate.den;
return fn;
}


std::string toString(const std::string &msg) const {
char buf[1024];
sprintf_s(buf, 1024, "time_base[num=%d, den=%d],start_time=%llu, duration=%llu, frame_rate[num=%d,den=%d],w=%d,h=%d,pix_fmt=%d",
time_base.num, time_base.den, start_time, duration, frame_rate.num, frame_rate.den, w, h, pix_fmt);
return msg + std::string(buf);
}
};


std::string my_av_err2str(int ret);
std::string my_av_ts2str(const int64_t x);
std::string my_av_ts2timestr(const int64_t ts, const AVRational *tb);
void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt);
void dumpRaw(const char *f, const uint8 *buf, const uint64 sz);
void loadRaw(const char *f, std::vector<char> &buf );
void raw2wav(const std::string &rawPath, const bool isDeletedRaw=false);

uint32_t ppm_save(ppm_image *img, FILE *outfile);
uint32_t yuv420p_save(const uint8 *yuv420pRaw, const int sz, const int w, const int h, const char *fname);
void dumpFrmImg(const RawFrm &x, const std::string &folder, const std::string &tag = "");

//resiez YUV420 raw video frms
void resizeYuvVideoFrm(RawFrm &dst, const uint8 *srcBuf, const int srcW, const int srcH, const enum libyuv::FilterMode flag=libyuv::kFilterNone);
void resizeYuvVideoFrm(RawFrm &des, const RawFrm &src, const enum libyuv::FilterMode flag=libyuv::kFilterNone);
void resizeYuvVideoFrms(const int srcW, const int srcH, const std::vector<RawFrm> &vSrc, const int desW, const int desH, std::vector<RawFrm> &vDes);
void resizeYuvVideoFrms(const int srcW, const int srcH, const std::vector<RawFrmPtr> &vSrc, const int desW, const int desH, std::vector<RawFrmPtr> &vDes);

bool saveAudioStreamInfo(const std::string &filePath, const MyAudioStreamInfo &info);
bool loadAudioStreamInfo(const std::string &filePath, MyAudioStreamInfo &info);

bool saveVideoStreamInfo(const std::string &filePath, const MyVideoStreamInfo &info);
bool loadVideoStreamInfo(const std::string &filePath, MyVideoStreamInfo &info);
}
#endif
