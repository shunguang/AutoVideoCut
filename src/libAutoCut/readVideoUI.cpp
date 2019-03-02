/*
*-------------------------------------------------------------------
* readVideoUI.cpp
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
#include "ReadVideoUI.h"

using namespace std;
using namespace autocut;

//using  av_seek_frame(), then read video/audio codec ctx info, then rewind the file and beging
void autocut::getVideoInfo(const char *filePath, MyVideoStreamInfo &vInfo, MyAudioStreamInfo	&aInfo)
{
bool isRefCount = false;
CfgDemuxingDecodingPtr  cfg( new CfgDemuxingDecoding() );
cfg->demuxingVideo = 1;
cfg->dumpRawVideo = false;
cfg->storeRawVideoInMemo = false;

cfg->demuxingAudio = 1;
cfg->dumpRawAudio = false;
cfg->storeRawAudioInMemo = false;

DemuxingDecoding2 x(cfg);
x.openVideoFileToRead(filePath, isRefCount);

x.decodeSome(true,1,true, 1);

x.update_video_streamInfo();
x.update_audio_streamInfo();

vInfo = x.m_videoStreamInfo;
aInfo = x.m_audioStreamInfo;

x.closeOpenedFiles();
}


void autocut::readVideo(const char *filePath, const bool readVideo, const bool readAudio, int &vdieoFrmW, int &vdieoFrmH, std::vector<RawFrmPtr> &vFrm, std::vector<RawFrmPtr> &aFrm)
{
bool isRefCount = false;
CfgDemuxingDecodingPtr  cfg (new CfgDemuxingDecoding() );
cfg->demuxingVideo = 1;
cfg->dumpRawVideo = 0;
cfg->storeRawVideoInMemo = readVideo;

cfg->demuxingAudio = 1;
cfg->dumpRawAudio = 0;
cfg->storeRawAudioInMemo = readAudio;

DemuxingDecoding2 x(cfg);
x.openVideoFileToRead( filePath, isRefCount);
x.decodeAll( NULL, NULL);


AVPixelFormat  fmt = x.getRawVideoFrameFormat();
AVCodecContext ctx = x.getVideoCodecContext();

vdieoFrmW = ctx.width;
vdieoFrmH = ctx.height;

vFrm = x.m_vVideoFrms;
aFrm = x.m_vAudioFrms;

x.closeOpenedFiles();
}



