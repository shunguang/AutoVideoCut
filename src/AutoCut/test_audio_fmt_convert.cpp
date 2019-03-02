/*
*-------------------------------------------------------------------
* test_audio_fmt_convert.cpp
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
#include "libAutoCut/DemuxingDecoding2.h"
#include "libAutoCut/MuxingEncoding2.h"
#include "libAutoCut/ReadVideoUI.h"
#include "libUtil/VisLog.h"
#include "libUtil/UtilFuncs.h"

using namespace std;
using namespace autocut;

int test_audio_fmt_convert(int argc, char **argv)
{
CfgDemuxingDecodingPtr  cfg (new CfgDemuxingDecoding() );
cfg->demuxingVideo = 1;
cfg->dumpRawVideo = 1;
cfg->storeRawVideoInMemo = false;

cfg->demuxingAudio = 1;
cfg->dumpRawAudio = 1;
cfg->storeRawAudioInMemo = false;

char *f0 = "C:/Projects/AutoCut/data/PreAcademySoccer/Disney_December2017/Game1_vsSouthCarolinaUnitedFCElite/SACBaltimoreArmourPreAcademyvsSouthCarolinaUnitedFCElite_Part1.MTS";
//char *f0 = "C:/Projects/AutoCut/data/big_buck_bunny.mp4";
bool isRefCount = false;
uint64 vFrmSzInBytes, aFrmSzInBytes;

int readRet = 0;
int nVideoFrms = 10;
DemuxingDecoding2 x( cfg );
x.openVideoFileToRead(f0, isRefCount);

char *fout = "C:/temp/out2.mp4";
MyVideoStreamInfo vOutInfo = x.m_videoStreamInfo;
MyAudioStreamInfo aOutInfo = x.m_audioStreamInfo;
AVCodecContext aCodecCtx = x.getAudioCodecContext();

aOutInfo.channels = 2;
aOutInfo.sample_fmt = AV_SAMPLE_FMT_FLTP;
aOutInfo.nb_samples_perfrm = 1024;  //todo: if we set this number as other values, we will have problems
if (aOutInfo.bit_rate < aCodecCtx.bit_rate) {
aOutInfo.bit_rate = aCodecCtx.bit_rate;
}

MuxingEncoding2 y;
y.openOutputFile(fout, vOutInfo, aOutInfo);

while (readRet>=0) {
readRet = x.decodeSomeVideo(nVideoFrms, true);
//x.m_vAudioFrms: each frm has 1536 frms
//but aOutInfo.nb_samples_perfrm=1024 todo:
//todo:
y.wrtSomeFrms(x.m_vVideoFrms, x.m_vAudioFrms, 1);
}
x.closeOpenedFiles();
y.closeOpenedFile();
return 0;
}

