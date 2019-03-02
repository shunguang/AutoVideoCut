/*
*-------------------------------------------------------------------
* AudioEncoding.h
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
* AudioEncoding.h
*
* shunguang wu  1/14/2018
* based on the above ope source code
*/

#ifndef __AUDIO_ENCODING_H__
#define __AUDIO_ENCODING_H__

extern "C" {
#define __STDC_CONSTANT_MACROS
#include <libavutil/imgutils.h>
#include <libavutil/samplefmt.h>
#include <libavutil/timestamp.h>
#include "libavcodec/avcodec.h"
#include "libavutil/mathematics.h"
#include <libavformat/avformat.h>
}
#include "libUtil/DataTypes.h"
#include "libUtil/RawFrm.h"
#include "libUtil/VisLog.h"
#include "libUtil/CfgAc.h"

#include "avUtils.h"

namespace autocut {
struct AudioEncCfg {

AudioEncCfg():codecId(AV_CODEC_ID_MP2), smpFmt(AV_SAMPLE_FMT_S16), smpRate(44100), bitRate(64000), channels(2){}

enum AVCodecID			codecId;
enum AVSampleFormat		smpFmt;
int						smpRate;        //Hz
int64_t					bitRate;
int						channels;
};

class AudioEncoding {
public:
AudioEncoding( const AudioEncCfg *p=NULL );
~AudioEncoding();

bool resetCodec(const AudioEncCfg &cfg);
void freeCodec();
int  getNumOfFrmSmps() const { return  m_frame->nb_samples; }

uint64  getFrmSize() const {
uint64 frmSzInBytes = (uint64)m_frame->nb_samples * (uint64)av_get_bytes_per_sample((AVSampleFormat)m_frame->format) * m_frame->channels;
return frmSzInBytes;
}

//write the data <raw> with <cfg> format into <outFile>
int encode_audio( const std::vector<uint8> &nonPlanarRaw, const std::string &outFile );
int encode_audio_planarFrms(const std::vector<RawFrmPtr> &vPlanarFrms, const int nChs, const std::string &outFile);
int encode_audio_nonPlanarFrms(const std::vector<RawFrmPtr> &vNonPlanarFrms, const int nChs, const std::string &outFile);
private:
int encode(AVCodecContext *ctx, AVFrame *frame, AVPacket *pkt, FILE *output);
uint64 select_channel_layout(const AVCodec *codec);
int select_sample_rate(const AVCodec *codec);
int check_sample_fmt(const AVCodec *codec, enum AVSampleFormat sample_fmt);

private:
AVCodecContext	*m_ctx;
bool			m_isInit;
AVFrame			*m_frame;
AVPacket		*m_pkt;

};
typedef std::shared_ptr<AudioEncoding>		AudioEncodingPtr;
}
#endif
