/*
*-------------------------------------------------------------------
* test_audioDeEn_coding.cpp
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
#include "libyuv/libyuv.h"
#include "libutil/UtilFuncs.h"

#include "libAutoCut/AudioDecoding.h"
#include "libAutoCut/AudioEncoding.h"

using namespace std;
using namespace autocut;

int test_audio1();
int test_audio2();
int test_audio3();
int test_audioDeEn_coding(int argc, char **argv)
{
//test_audio1();
//test_audio2();
test_audio3();

return 0;
}

//mp3 to mp2 
int test_audio1()
{
AudioDecoding x;

const std::string inputFile = "C:/Projects/AutoCut/data/Violin-Ringtone-12-beautiful.mp3";
std::vector<uint8> raw;
AudioDecCfg params;
x.decode_audio( inputFile, raw, params);  //params.smpFmtOutput == AV_SAMPLE_FMT_S16

AudioEncCfg cfg;
cfg.bitRate = 64000; cfg.channels = params.nChannels; cfg.codecId = AV_CODEC_ID_MP2; cfg.smpFmt = params.smpFmtOutput;
AudioEncoding y( &cfg );
int ret = y.encode_audio(raw, "C:/Projects/AutoCut/data/a.mp2");

return 0;
}

//mp3 to mp3
int test_audio2()
{
//decoding the original mp3 file
const std::string inputFile = "C:/Projects/AutoCut/data/Violin-Ringtone-12-beautiful.mp3";
std::vector<uint8> raw;
AudioDecCfg params;
AudioDecoding x;
x.decode_audio(inputFile, raw, params);  //params.smpFmtOutput == AV_SAMPLE_FMT_S16

//since the mp3 encode only support AV_SAMPLE_FMT_S32P, AV_SAMPLE_FMT_FLTP fmt, we need to convert <raw> to  AV_SAMPLE_FMT_FLT 1st
std::vector<uint8> rawFlt;

enum AVSampleFormat desFmt1 = AV_SAMPLE_FMT_FLT, desFmt2 = AV_SAMPLE_FMT_FLTP;
//enum AVSampleFormat desFmt1 = AV_SAMPLE_FMT_S32, desFmt2 = AV_SAMPLE_FMT_S32P;
int ret = audio_convert_nonPlanar2nonPlanar(rawFlt, desFmt1, params.nChannels, params.smpRate, raw, params.smpFmtOutput, params.nChannels, params.smpRate);

//then set <cfg.smpFmt> as AV_SAMPLE_FMT_FLTP to write a mp3 file w/ codeId as AV_CODEC_ID_MP3
//mp3 Bit rates range from 96 to 320(Kbps). Using a bit rate of 128 Kbps usually results in a sound quality equivalent to what you'd hear on the radio.
//Many music sites and blogs urge people to use a bit rate of 160 Kbps or higher if they want the MP3 file to have the same sound quality as a CD
AudioEncCfg cfg;
cfg.bitRate = 128000; cfg.channels = params.nChannels; cfg.codecId = AV_CODEC_ID_MP3; cfg.smpFmt = desFmt2;
AudioEncoding y(&cfg);
ret = y.encode_audio(rawFlt, "C:/Projects/AutoCut/data/b.mp3");

return 0;
}


int test_audio3()
{
//decoding the original mp3 file
const std::string inputFile = "C:/Projects/AutoCut/data/Violin-Ringtone-12-beautiful.mp3";
AudioDecoding x;
AudioDecCfg mp3Params;
for (int i = 0; i < 1; ++i) {
enum AVSampleFormat fmt;
string outFile;
if (i == 0) {
fmt = AV_SAMPLE_FMT_FLTP;
outFile = "C:/Projects/AutoCut/data/fltp.mp3";
}
else {
fmt = AV_SAMPLE_FMT_FLT;
outFile = "C:/Projects/AutoCut/data/flt.mp3";
}

//encoding params...
AudioEncCfg cfg; cfg.bitRate = 128000; cfg.channels = 2; cfg.codecId = AV_CODEC_ID_MP3; cfg.smpFmt = fmt;
AudioEncoding y;
if (!y.resetCodec(cfg)) {
cout << "cannot set encoding codec" << endl;
break;
}
int nFrmSmps = y.getNumOfFrmSmps(); //we need to fingure out <nFrmSmps> first for setting decoding params


//decoding params
std::vector<RawFrmPtr> vDecFrms;
MyAudioStreamInfo outAudioStreamInfo;
outAudioStreamInfo.channels = 2;
outAudioStreamInfo.sample_rate = 44100;
outAudioStreamInfo.sample_fmt = fmt;
outAudioStreamInfo.nb_samples_perfrm = nFrmSmps;         

int ret = x.decode_audio(inputFile, outAudioStreamInfo, vDecFrms, mp3Params);

if ( fmt == AV_SAMPLE_FMT_FLTP ) {
ret = y.encode_audio_planarFrms(vDecFrms, 2, outFile);
}
else {
ret = y.encode_audio_nonPlanarFrms(vDecFrms, 2, outFile);
}
}
return 0;
}
