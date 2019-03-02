/*
*-------------------------------------------------------------------
* RunMuxEnc.cpp
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
#include "RunMuxEnc.h"
using namespace std;
using namespace cv;
using namespace autocut;

RunAutoCutCore::RunAutoCutCore()
: RunThread("RunAutoCutCore")
, m_autoCut( new AutoCut() )
{
}

RunAutoCutCore::~RunAutoCutCore()
{
}


void RunAutoCutCore::procNextFrm( bool isEmitSignal )
{
//step 1: check seetings
std::string errMsg;
bool pass = checkCfg( errMsg );
if (!pass) {
emit sigHasErr(QString::fromStdString(errMsg));
return;
}

//step 2: load head and instered/space video clips
uint32 dt;
const CfgGame &game = m_cfg->getGame(0);
xLog("game: %s", game.name.c_str());

m_autoCut->resetAnaResults();

ProgressMsg proMsg;
proMsg.fileIdx = 0;
BOOST_FOREACH(const string &videoFileName, game.vInputVideoFileNames) {
proMsg.fileName = videoFileName;

string videoFilePath = game.inputVideoFolder + "/" + videoFileName;
xLog("processing %s", videoFilePath.c_str());

dt = m_autoCut->demuxAudio(videoFilePath)/1000;  //ms-->sec
proMsg.taskId = PRO_MSG_DEMUX_AUDIO;
proMsg.usedTimeSec = dt;
QString qs = encodeProgressMsg(proMsg);
emit sigProgress( qs );
xLog( qs.toStdString() );

//step 2: prepare and do FFT analysis
dt = m_autoCut->analyzeAudio(videoFilePath)/1000;
m_autoCut->addAnaResult(videoFilePath);

proMsg.taskId = PRO_MSG_ANA_AUDIO;
proMsg.usedTimeSec = dt;
qs = encodeProgressMsg(proMsg);
emit sigProgress(qs);
xLog(qs.toStdString());
}

//step 2: load head and instered/space video clips
//prepareVideoClip(m_headClip, m_cfg->m_wrtVideo.headerVidoeFilePath);
//const uint32 nMaxFrms = m_inpVideoStreamInfo.getVideoFrmRate() / 2;  //1seconde "space" frames
//prepareVideoClip(m_insertingClip, m_cfg->m_wrtVideo.insertingVidoeFilePath, nMaxFrms);
//vT[2] = timeIntervalMillisec(t0); t0 = POSIX_LOCAL_TIME;

//step 4: write output video
m_autoCut->loadMp3(game.mp3File);
dt = m_autoCut->writeOutputVideo(game);

proMsg.fileIdx = -1;
proMsg.taskId = PRO_MSG_WRT_VIDEO;
proMsg.usedTimeSec = dt;
QString qs = encodeProgressMsg(proMsg);
emit sigProgress(qs);
xLog(qs.toStdString());

endJob();
}

//set <isEmitSignal> as false when doing single thread non GUI test
bool RunAutoCutCore::procInit( bool isEmitSignal )
{
m_autoCut->setCfg(m_cfg);
return true;
}

void RunAutoCutCore::endJob()
{
pauseJob();

//need to reinitializing cature once weaked up
setInitProcess( false );

setLastFrmProcessed(true);
emit sigJobDone();
}

bool RunAutoCutCore::checkCfg(std::string &errMsg)
{
string cfgStr = m_cfg->toStr();
cout << cfgStr << endl;

errMsg = "";
//check input files
std::vector<std::string> vPaths = m_cfg->getInputVideoFilePaths();
BOOST_FOREACH( std::string &p, vPaths) {
if ( !boost::filesystem::exists(p) ){
errMsg += (string("Video file <") + p + string("> does not exist!"));
}
}

//check mp3 files
std::string p = m_cfg->getMp3FilePath();
if (!p.empty()) {
if (!boost::filesystem::exists(p)) {
errMsg += (string("Mp3 file<") + p + string("> does not exist!"));
}
}

bool ret = errMsg.empty();
return ret;
}
