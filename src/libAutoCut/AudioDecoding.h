/*
*-------------------------------------------------------------------
* AudioDecoding.h
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
* AudioDecoding.h
*
* shunguang wu  1/14/2018
* based on the above ope source code
*/

#ifndef __AUDIO_DECODING_H__
#define __AUDIO_DECODING_H__

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

#define AUDIO_INBUF_SIZE 20480
#define AUDIO_REFILL_THRESH 4096
namespace autocut {
struct AudioDecCfg {
enum AVCodecID			codecId;
enum AVSampleFormat		smpFmtOriginal;
enum AVSampleFormat		smpFmtOutput; 
int						smpRate;        //Hz
int64_t					bitRate;
int						nBytesPerSmp;
int						nChannels;
int64_t					nSmps;
};

class AudioDecoding {
public:
AudioDecoding();
~AudioDecoding();

//decoding the an audio file into non-planar fmt stream either in a file or in memeory
int decode_audio(const std::string &inputFile, const std::string &outputFile, AudioDecCfg &params);
int decode_audio(const std::string &inputFile, std::vector<uint8> &vNonPlanarRaw, AudioDecCfg &params);
int decode_audio(const std::string &audioFile, const MyAudioStreamInfo outAudioStreamInfo, std::vector<RawFrmPtr> &outFrms, AudioDecCfg &paramsInAudioFile);

private:
int decode(AVCodecContext *dec_ctx, AVPacket *pkt, AVFrame *frame, FILE *outfile);
int decode(AVCodecContext *dec_ctx, AVPacket *pkt, AVFrame *frame, std::vector<uint8> &out);
};
typedef std::shared_ptr<AudioDecoding>		AudioDecodingPtr;


int audio_convert_nonPlanar2nonPlanar( std::vector<uint8> &desRaw, const enum AVSampleFormat desFmt, const int desChannels, const int desSmpRate,
const std::vector<uint8> &srcRaw, const enum AVSampleFormat srcFmt, const int srcChannels, const int srcSmpRate );

int audio_convert_nonPlanar2planar(std::vector<std::vector<uint8>> &des, const std::vector<uint8> &src, const int nChannels, const int nBytes4Smp );
}
#endif
