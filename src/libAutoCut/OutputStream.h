/*
*-------------------------------------------------------------------
* OutputStream.h
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

#ifndef __OUTPUT_STREAM_H__
#define __OUTPUT_STREAM_H__

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

#define MY_STREAM_DURATION    10.0
#define MY_STREAM_FRAME_RATE  25					/* 25 images/s */
#define MY_STREAM_PIX_FMT     AV_PIX_FMT_YUV420P	/* default pix_fmt */
#define MY_AUDIO_SMP_FMT	  AV_SAMPLE_FMT_FLT     /*float ineterlaced */
#define MY_AUDIO_SMP_RATE	  44100                  //44100,22050
#define SCALE_FLAGS SWS_BICUBIC

#include "libUtil/DataTypes.h"
//#include "libUtil/RawFrm.h"
//#include "libUtil/VisLog.h"
//#include "avUtils.h"

namespace autocut {
// a wrapper around a single output AVStream
struct OutputStream {
OutputStream() : st(NULL), enc(NULL), next_pts(0), samples_count(0), frame(0), tmp_frame(0), t(0), tincr(0), tincr2(0), sws_ctx(0), swr_ctx(0) {}

AVStream *st;
AVCodecContext *enc;

/* pts of the next frame that will be generated */
int64_t next_pts;
int samples_count;

AVFrame *frame;
AVFrame *tmp_frame;

float t, tincr, tincr2;

struct SwsContext *sws_ctx;
struct SwrContext *swr_ctx;
};
typedef std::shared_ptr<OutputStream>		OutputStreamPtr;
}

#endif
