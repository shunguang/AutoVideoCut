/*
*-------------------------------------------------------------------
* WrtOutputVideo.h
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

#ifndef __WRT_OUTPUT_VIDEO_H__
#define __WRT_OUTPUT_VIDEO_H__

#include "libyuv/libyuv.h"
#include "libUtil/RawFrm.h"
#include "libUtil/DataTypes.h"
#include "libUtil/UtilFuncs.h"
#include "libUtil/VisLog.h"
#include "libUtil/CfgAc.h"

#include "avUtils.h"
#include "MuxingEncoding2.h"
#include "ImgRender.h"
#include "AudioDecoding.h"
#include "VideoAnaResult.h"
namespace autocut {
class WrtOutputVideo {
public:
WrtOutputVideo();
~WrtOutputVideo();

void respReset();
void setCfg(CfgAcPtr &cfg);
void setOutputAudioInfo( const MyAudioStreamInfo &info);
void setOutputVideoInfo( const MyVideoStreamInfo &info);

uint32	decodeMp3(const std::string &mp3FilePath);
uint32	setMp3Frms(const std::vector<RawFrmPtr> &vMp3Frms );

uint32  initProcess();
uint32  wrtHeaderTeamGameInfo();
uint32  wrtEventTimeFrms(const float timeOfEventInSec);
uint32  wrtFrms(const std::vector<RawFrmPtr> &vVideoFrms, const std::vector<RawFrmPtr> &vAudioFrms_1ch_FLT);
void    endProcess();
const std::string& getOutputVideoFilePath() const {
return m_mux->m_outputVideoFilePath;
}
const std::vector<RawFrmPtr>& getMp3Frms() const {
return m_vMp3Frms;
}
private:
std::string  createOutputVideoFilePath(const CfgGame &game );
bool renderClipFromImage(const cv::Mat &I, RawClip2Ptr &clip);
void mixAudioWithMp3(const std::vector<RawFrmPtr> &vAudioFromVideoFile_1ch_FLT, std::vector<RawFrmPtr> &vMixedAudioFrms_2ch_FLTP);

const RawFrmPtr& getNextMp3Frm();

void wrtHeader();
void wrtGameInfo(const CfgGame &game);
void wrtTeamInfo();
void readVideoFromFile(const char *filePath, const bool readVideo, const bool readAudio, int &vdieoFrmW, int &vdieoFrmH, 
std::vector<RawFrmPtr> &vFrm, std::vector<RawFrmPtr> &aFrm);
int prepareVideoClip(RawClip2Ptr &clip, const std::string &videoFile, const uint32 nMaxFrms = UINT32_MAX);
private:
CfgAcPtr			m_cfg;
MyAudioStreamInfo	m_outpAudioStreamInfo;
MyVideoStreamInfo	m_outpVideoStreamInfo;

int					m_outputVideoW;
int					m_outputVideoH;

uint32				m_audioFrmSzInBytes;         //the size of one audio Frame
int					m_audioSmpingFreq;
int					m_audioFrmSmples;

uint32				m_videoFrmSzInBytes;         //the size of one vidoe frame

uint32              m_totAudioFrms;          
uint32              m_totvideoFrms;

RawFrm              m_audio4Fft;				//the whole audio raw data  from the input videofile

std::vector<FrmRange> m_vSelectedFrmRngs;

std::vector<VideoAnaResult> m_vAnaResult;


RawClip2Ptr			m_clip;
MuxingEncoding2Ptr	m_mux;

ImgRenderPtr		m_imgRender; 

//add music: original parameters insize the mp3 file
AudioDecCfg			m_mp3Params;
std::vector<RawFrmPtr> m_vMp3Frms; //the sample fmd in <m_mp3Frms> is the same as that in original video
int32				m_mp3FrmId;
};

typedef std::shared_ptr<WrtOutputVideo>		WrtOutputVideoPtr;
}
#endif
