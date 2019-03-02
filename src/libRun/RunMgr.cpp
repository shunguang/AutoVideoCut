/*
*-------------------------------------------------------------------
* RunMgr.cpp
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
#include "RunMgr.h"
using namespace std;
using namespace cv;
using namespace autocut;

RunMgr::RunMgr()
: RunThread("RunMgr")
, m_nWorkers(0)
, m_taskId(PRO_MSG_DEMUX_AUDIO)
, m_currWorkerId(-1)
, m_currWorker(0)
, m_videoWriter(new WrtOutputVideo())
, m_projProfile( new ProjProfile() )
{
}

RunMgr::~RunMgr()
{
}

void RunMgr::setCfg(CfgAcPtr  &cfg)
{
RunThread::setCfg(cfg);
//add more if need ...
}

void RunMgr::respReset()
{
m_videoWriter->respReset();
m_projProfile->respReset(); 
forceAllWorkersQuit();
}


bool RunMgr::resetWorkers()
{
//step 1: check seetings
std::string errMsg;
bool pass = checkCfg(errMsg);
if (!pass) {
emit sigHasErr(QString::fromStdString(errMsg));
return false;
}

createWorkers();
pauseJob();
return true;
}

void RunMgr::setTaskId(const ProgressMsgTaskId_t t)
{
boost::mutex::scoped_lock lock(m_mutexMgr);
m_taskId = t;
}


int RunMgr::getCurrWorkerId() 
{
return m_currWorkerId;
}

void RunMgr::setCurrWorkerId(const int id)
{
//make sure <m_currWorker> stop dexing
if (m_currWorker) {
if (m_currWorker->getTask() == W_TASK_DEX_ON_DEMOND) {
if (!m_currWorker->isPauseJob()) {
m_currWorker->pauseJob();
}
}
while (1) {
boost::this_thread::sleep(boost::posix_time::milliseconds(5));
if (m_currWorker->isPauseJob()) {
break;
}
}
}

//swicth <m_currWorkerId>
if (id >= 0 && id < m_nWorkers) {
m_currWorkerId = id;
m_currWorker = m_vWorkers[m_currWorkerId].get();
}
}

void RunMgr::procNextTask(bool isEmitSignal)
{
if (m_taskId == PRO_MSG_DEMUX_AUDIO) {
dexInParallel(isEmitSignal);
pauseJob(); 
}
else if (m_taskId == PRO_MSG_ANA_AUDIO) {
anaInParallel(isEmitSignal);
pauseJob();
}
else if (m_taskId == PRO_MSG_WRT_VIDEO) {
//todo:
writeOuputVideo();
pauseJob();
}
else {
pauseJob();
}
}

void RunMgr::dexInParallel( bool isEmitSignal )
{
std::vector<int> vDexDoneFlag(m_nWorkers);
for (int i = 0; i < m_nWorkers; i++) {
printf("RunMgr::dexInParallel(): AAA--- worker[%d], isPauseJob=%d\n", i, m_vWorkers[i]->isPauseJob());
m_vWorkers[i]->m_dexAlg->setDecodedProfileFileNames(m_projProfile->m_vProfFiles[i]);
m_vWorkers[i]->setTask( W_TASK_DEX_WHOLE);
m_vWorkers[i]->startJob();
vDexDoneFlag[i] = 0;
}
boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

boost::posix_time::ptime tBeg = POSIX_LOCAL_TIME;
ProgressMsg proMsg;
proMsg.taskId = PRO_MSG_DEMUX_AUDIO;
proMsg.percent = 100 / m_nWorkers;

while (1) {
for (int i = 0; i < m_nWorkers; i++) {
if (vDexDoneFlag[i] == 1) {
continue;
}
WorkerPtr &woker = m_vWorkers[i];
bool flag = woker->isPauseJob();
if ( flag ) {
vDexDoneFlag[i] = 1;
proMsg.fileIdx = woker->m_videoFileId;
proMsg.fileName = woker->m_videoFilePath;
proMsg.usedTimeSec = timeIntervalMillisec(tBeg) / 1000;
emitProgressSignal( proMsg );
}
}
//chgekc if all done
int cnt = std::accumulate(vDexDoneFlag.begin(), vDexDoneFlag.end(), 0);
if (cnt == m_nWorkers) {
break;
}
boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
}
emit sigAllDexed( QString("All files are Dexed!") );
xLog("RunMgr::dexInParallel(): All files are Dexed!");

for (int i = 0; i < m_nWorkers; i++) {
printf("RunMgr::dexInParallel(): BBBB--worker[%d], isPauseJob=%d\n", i, m_vWorkers[i]->isPauseJob());
}
}


void RunMgr::anaInParallel(bool isEmitSignal)
{
boost::posix_time::ptime tBeg = POSIX_LOCAL_TIME;

g_fftCreatePlan(m_cfg->m_tfAna.nFFT);
//step 1: check setings
std::vector<int> vAnaDoneFlag(m_nWorkers);
for (int i = 0; i < m_nWorkers; i++) {
printf("RunMgr::anaInParallel(): worker[%d], isPauseJob=%d\n", i, m_vWorkers[i]->isPauseJob());
m_vWorkers[i]->setTask(W_TASK_TIME_ANA);
m_vWorkers[i]->startJob();
vAnaDoneFlag[i] = 0;
}
//makesure all threads started
boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

ProgressMsg proMsg;
proMsg.taskId = PRO_MSG_ANA_AUDIO;
proMsg.percent = 100 / m_nWorkers;

while (1) {
for (int i = 0; i < m_nWorkers; i++) {
if (vAnaDoneFlag[i] == 1) {
continue;
}
WorkerPtr &woker = m_vWorkers[i];
bool flag = woker->isPauseJob();
if (flag) {
vAnaDoneFlag[i] = 1;
proMsg.fileIdx = woker->m_videoFileId;
proMsg.fileName = woker->m_videoFilePath;
proMsg.usedTimeSec = timeIntervalMillisec(tBeg) / 1000;
emitProgressSignal(proMsg);
}
}
//chgekc if all done
int cnt = std::accumulate(vAnaDoneFlag.begin(), vAnaDoneFlag.end(), 0);
if (cnt == m_nWorkers) {
break;
}
boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
}
g_fftDestroyPlan();

emit sigAllAnalyzed(QString("All files are analyzed!"));
xLog("RunMgr::anaInParallel(): All files are analyzed!");
}


void RunMgr::emitProgressSignal(const ProgressMsg &msg )
{
QString qs = encodeProgressMsg(msg);
emit sigProgress(qs);
//xLog(qs.toStdString());
}

//set <isEmitSignal> as false when doing single thread non GUI test
bool RunMgr::procInit( bool isEmitSignal )
{
return true;
}

void RunMgr::endJob()
{
pauseJob();
//need to reinitializing cature once weaked up
setInitProcess( false );
}

bool RunMgr::checkCfg(std::string &errMsg)
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

bool RunMgr::createWorkers()
{
const CfgGame &game = m_cfg->getGame(0);
xLog("game: %s", game.name.c_str());
m_nWorkers = game.vInputVideoFileNames.size();
if (m_nWorkers < 1) {
return false;
}
m_currWorker = 0;
m_vWorkers.clear();
for (int i = 0; i < m_nWorkers; ++i) {
std::string f = game.inputVideoFolder + "/" + game.vInputVideoFileNames[i];
WorkerPtr w( new Worker( f, i/*fileId*/) );
w->setCfg(m_cfg);
w->setDC(m_dc);
m_vWorkers.push_back(w);
}
setCurrWorkerId(0);   //default idx=0

for (int i = 0; i < m_nWorkers; ++i) {
m_vWorkers[i]->start();
}
boost::this_thread::sleep(boost::posix_time::milliseconds(10));

for (int i = 0; i < m_nWorkers; ++i) {
assert( m_vWorkers[i]->isPauseJob() );
}

return true;
}


bool RunMgr::isAllDexThreadsInPause() 
{
int cnt = 0;
for (int i = 0; i < m_nWorkers; i++) {
cnt += (int)m_vWorkers[i]->isPauseJob();
}

return (cnt == m_nWorkers);
}

void RunMgr::forceAllWorkersQuit()
{
//check if all the dex threads in pause (no job)
while (1) {
if ( isAllDexThreadsInPause() ) {
break;
}
else {
printf("RunMgr::forceAllWorkersQuit(): waiting for all workers pause their job!");
boost::this_thread::sleep(boost::posix_time::milliseconds(500));
}
}

//quit all the dex threads
for (int i = 0; i < m_nWorkers; i++) {
m_vWorkers[i]->forceThreadQuit();
}
}


uint64  RunMgr::getCurrVideoKeyFrms() const
{
if (m_currWorkerId < 0) {
return -1;
}
uint64 n = m_currWorker->m_dexAlg->getTotKeyFrms();
return n;
}

float64 RunMgr::getCurrVideoTimeSecFromKeyFrmIdx(const uint64 videoKeyFrmIdx, uint64 &keyFrmNum) const
{
if (m_currWorkerId < 0) {
return -1;
}
keyFrmNum = m_currWorker->m_dexAlg->getVideoFrmIdxFromKeyFrmIdx(videoKeyFrmIdx);
float64 sec = m_currWorker->m_dexAlg->getVideoTimeInSec(keyFrmNum);
return sec;
}

float64 RunMgr::getCurrVideoTimeSec(const uint64 videoFrmNum) const
{
if (m_currWorkerId < 0) {
return -1;
}

float64 sec = m_currWorker->m_dexAlg->getVideoTimeInSec(videoFrmNum);
return sec;
}

float64 RunMgr::getCurrVideoDurationSec() const
{
if (m_currWorkerId < 0) {
return -1;
}
float64 d = m_currWorker->m_dexAlg->getVideoDurationSec();
return d;
}

bool RunMgr::getCurrSelectedKeyFrmRanges(std::vector<RngWorld> &vKeyFrmRngs) const
{
if (m_currWorkerId < 0) {
return false;
}
m_currWorker->getSelectedKeyFrmRanges(vKeyFrmRngs);
return (vKeyFrmRngs.size()>0);
}

bool   RunMgr::getAlgSelectedKeyFrmRanges(int workerId, std::vector<RngWorld> &vKeyFrmRngs) const
{
if (workerId < 0 || workerId>= m_nWorkers ) {
return false;
}

m_vWorkers[workerId]->getSelectedKeyFrmRanges(vKeyFrmRngs);
return (vKeyFrmRngs.size()>0);
}

bool  RunMgr::getThumbnailFrm(const uint64 keyFrmIdx, RawFrmPtr &frm)
{
bool flag = m_currWorker->m_dexAlg->getThumbnailKeyFrm(keyFrmIdx, frm);
return flag;
}

bool   RunMgr::getFirstFrm2Disp(const int fileId, DispFrm &disp)
{
const RawFrmPtr &originalFrm  = m_vWorkers[fileId]->m_dexAlg->m_firstVideoFrm;
return DataContainer::rawFrm2DispFrm(originalFrm, disp);
}

//random access video
void RunMgr::startDecoding(const uint64 keyFrmIdx)
{
m_currWorker->setCopyDexFrmsIntoQ(true);
m_currWorker->startDecodeFrms(keyFrmIdx);
}

void RunMgr::stopDecoding()
{
if (m_currWorker) {
m_currWorker->stopDecodeFrms();
}
}

bool RunMgr::getNextDispFrm(cv::Mat &bgr)
{
cv::Size desSz = cv::Size( bgr.cols /*w*/, bgr.rows /*h*/);
RawFrm dispFrm(desSz, 0);
RawFrmPtr originalFrm(0);

bool hasData = m_currWorker->getNextFrm(originalFrm);
if (!hasData) {
if (m_currWorker->m_lastVideoFrmDecoded) {
originalFrm = m_currWorker->m_dexAlg->m_latestDecodedVideoFrm;
if (originalFrm == 0) { //todo: avoid this condition
putText(bgr, "last frm reached-no update!", cvPoint(30, 30), FONT_HERSHEY_COMPLEX_SMALL, 0.8, cvScalar(200, 200, 250), 1, CV_AA);
return true;
}
}
else {
return false;
}
}

//dumpFrmImg( *originalFrm.get(), "c:/temp", "ori");
if ( dispFrm.w_ > originalFrm->w_ || dispFrm.h_ > originalFrm->h_) {
cv::Mat tmp;
originalFrm->toBGR(tmp);
cv::resize(tmp, bgr, desSz);
}
else {
resizeYuvVideoFrm(dispFrm, originalFrm->buf_, originalFrm->w_, originalFrm->h_);
dispFrm.toBGR(bgr);
}
//cv::imwrite("c:/temp/disp-frm0.png",bgr);
return true;
}

void RunMgr::startDexWhole() {
setTaskId(PRO_MSG_DEMUX_AUDIO);
assert(true == isPauseJob());
startJob();
}

void RunMgr::startAnaAudio() {
xLog("RunMgr::startAnaAudio()--AA, isPauseJob=%d", isPauseJob());
setTaskId(PRO_MSG_ANA_AUDIO);
assert(true == isPauseJob());
startJob();
xLog("RunMgr::startAnaAudio()--BB, isPauseJob=%d", isPauseJob()); ;
}

void RunMgr::startWriteOutputVideo(std::vector<std::vector<RngWorld>> &vSelectedKeyFrmRngs)
{
m_vSelectedKeyFrmIdxRngs = vSelectedKeyFrmRngs;
setTaskId(PRO_MSG_WRT_VIDEO);
assert( true == isPauseJob() );
startJob();
}

void RunMgr::writeOuputVideo()
{
ProgressMsg proMsg;
proMsg.taskId = PRO_MSG_WRT_VIDEO;
proMsg.fileIdx = 0;
proMsg.fileName ="dummy";
proMsg.percent = 5;

//set <outAudioInfo>
MyAudioStreamInfo outAudioInfo = m_currWorker->m_inpAudioStreamInfo;
AVCodecContext aCodecCtx = m_currWorker->m_dexAlg->getAudioCodecContext();
outAudioInfo.channels = 2;
outAudioInfo.sample_fmt = AV_SAMPLE_FMT_FLTP;
outAudioInfo.nb_samples_perfrm = 1024;
if (outAudioInfo.bit_rate < aCodecCtx.bit_rate) {
outAudioInfo.bit_rate = aCodecCtx.bit_rate;
}
xLog(outAudioInfo.toString("outAudioInfo="));

//set <outVideoInfo>
MyVideoStreamInfo outVideoInfo = m_currWorker->m_inpVideoStreamInfo;
AVCodecContext vCodecCtx = m_currWorker->m_dexAlg->getVideoCodecContext();
if (outVideoInfo.bit_rate < vCodecCtx.bit_rate) {
outVideoInfo.bit_rate = vCodecCtx.bit_rate;
}
xLog(outVideoInfo.toString("outVideoInfo="));


std::string fmt = m_cfg->getOutputVideoFmt();
cv::Size sz = m_cfg->getOutputVideoSize( cv::Size(m_currWorker->m_inpVideoStreamInfo.w, m_currWorker->m_inpVideoStreamInfo.h));
float fps = m_cfg->getOutputVideoFrameRate(m_currWorker->m_inpVideoStreamInfo.getVideoFrmRate());

outVideoInfo.w = sz.width;
outVideoInfo.h = sz.height;
//outVideoInfo.frame_rate =
//outVideoInfo.pix_fmt = ;

//set <m_videoWriter> parameters
m_videoWriter->setCfg(m_cfg);
m_videoWriter->setOutputAudioInfo(outAudioInfo);
m_videoWriter->setOutputVideoInfo(outVideoInfo);

int dt_ms;  //ms
if ( !loadMp3FromProjProfile(dt_ms) ) {
dt_ms = m_videoWriter->decodeMp3(m_cfg->getMp3File(0));
//save for speed up future using
const std::vector<RawFrmPtr>& vMp3 = m_videoWriter->getMp3Frms();
if (vMp3.size() > 1) {
m_projProfile->saveMp3Frms(vMp3);
}
}
dt_ms += m_videoWriter->initProcess();
proMsg.usedTimeSec = dt_ms/1000;
emitProgressSignal( proMsg);

proMsg.percent = 5;
proMsg.usedTimeSec = m_videoWriter->wrtHeaderTeamGameInfo()/1000;
emitProgressSignal( proMsg);

int percent = (100 - 10) / m_nWorkers;
int nFiles = m_vSelectedKeyFrmIdxRngs.size();
float timeOffsetSec = 0;
for (int i = 0; i < m_nWorkers; ++i) {
const std::vector<RngWorld> &vKeyFrmIdxRngs = m_vSelectedKeyFrmIdxRngs[i];
int nRngs = vKeyFrmIdxRngs.size();
m_currWorkerId=i;
m_currWorker = m_vWorkers[i].get();
m_currWorker->resetCfg4WrtVideo(sz.width,sz.height);

proMsg.fileIdx = m_currWorker->m_videoFileId;
proMsg.fileName = m_currWorker->m_videoFilePath;
proMsg.percent = percent / (nRngs+1); //avoid overflow when nRngs==0
BOOST_FOREACH(const RngWorld &keyFrmIdxRng, vKeyFrmIdxRngs) {
proMsg.usedTimeSec = writeOneSelctedRng(keyFrmIdxRng, timeOffsetSec)/1000;
emitProgressSignal(proMsg);
}

timeOffsetSec += m_currWorker->m_dexAlg->getVideoDurationSec();
}
m_videoWriter->endProcess();
emit sigWrtOutputDone( QString::fromStdString(m_videoWriter->getOutputVideoFilePath()) );
xLog("void RunMgr::writeOuputVideo(): well done!");
}

//todo: change to read and write in parallel
//<m_currWorker> decoding video, then write into output file
uint32 RunMgr::writeOneSelctedRng(const RngWorld &keyFrmIdxRng, const float timeOffsetSec)
{
boost::posix_time::ptime tBeg = POSIX_LOCAL_TIME;
uint64 nVideoFrms = m_currWorker->getVideoFrmsInRng(keyFrmIdxRng);
const uint64 batchSz = (nVideoFrms > 60) ? 60 : nVideoFrms;
uint64 nBatches = ceil(nVideoFrms / (float)batchSz);

m_currWorker->setBatchDecodeFrmSize(batchSz);

m_currWorker->setCopyDexFrmsIntoQ(false);
float t0Sec = m_currWorker->startDecodeFrms(keyFrmIdxRng.lower);  //<m_currWorker> start decoding fmrs

//-----------------------------
// write timestampe clip at the selcted moment
//-----------------------------
float t = timeOffsetSec + t0Sec;
m_videoWriter->wrtEventTimeFrms(t);
//-----------------------------
// write selected video
//-----------------------------
for ( uint64 i = 0; i < nBatches; ++i) {
while( !m_currWorker->isPauseJob() ){
boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
}
assert( m_currWorker->isPauseJob() ); //means cuurent decome batch is done

//read one clip from input video
const std::vector<RawFrmPtr> &vVideoFrms = m_currWorker->m_dexAlg->m_vVideoFrms;
const std::vector<RawFrmPtr> &vAudioFrms_1ch_FLT = m_currWorker->m_dexAlg->m_vAudioFrms;   //only one channel FLT data
if (vAudioFrms_1ch_FLT.size() >= 1 && vVideoFrms.size() >= 1) {
m_videoWriter->wrtFrms(vVideoFrms, vAudioFrms_1ch_FLT);
}

//start reading next batch
if (nBatches > 1) {
if (i == nBatches - 2) {
uint64 m = nVideoFrms - (nBatches - 1) * batchSz;
m_currWorker->setBatchDecodeFrmSize(m);
}
}

if (i < nBatches - 1) {
m_currWorker->startJob();
}
}

uint32 dt_ms = timeIntervalMillisec(tBeg);
return dt_ms;
}

void RunMgr::setProjProfileInfo()
{
string 	p0 = m_cfg->getProjProfilerFolder();
m_projProfile->setRootFolder(p0);
m_projProfile->setCfg(m_cfg->m_vGames[0]);
m_projProfile->genFileNames();
}

bool RunMgr::loadMp3FromProjProfile(int &dt_ms)
{
boost::posix_time::ptime tBeg = POSIX_LOCAL_TIME;

std::vector<RawFrmPtr> vMp3Frms;
bool suc = m_projProfile->loadMp3Frms(vMp3Frms);
if (suc) {
m_videoWriter->setMp3Frms(vMp3Frms);
}

dt_ms = timeIntervalMillisec(tBeg);
return suc;
}

void RunMgr::saveProjProfile(const int fileId, const int jobId)
{
WorkerPtr &worker = m_vWorkers[fileId];
if (jobId == 0) {
const std::vector<RawFrmPtr> &v = worker->m_dexAlg->m_vAudioFrms;
m_projProfile->saveAudioFrms(fileId, v);
m_projProfile->saveAudioStreamInfo(fileId, worker->m_dexAlg->m_audioStreamInfo);
}
else if (jobId == 1) {
const std::vector<RawFrmPtr> &w = worker->m_dexAlg->m_vThumbnailKeyFrms;
m_projProfile->saveVideoKeyFrms(fileId, w);
m_projProfile->saveVideoStreamInfo(fileId, worker->m_dexAlg->m_videoStreamInfo);
}
else if (jobId == 2) {
const std::vector<FrmIdx> &w = worker->m_dexAlg->m_vFrmIdxQ;
m_projProfile->saveFrmIdxQ(fileId, w);
}
}

void RunMgr::wakeupWorkerToDex()
{
if (m_currWorker->isPauseJob()) {
m_currWorker->startJob();
}
}
void RunMgr::printBrief()
{
cout << "m_currWorkerId=" << m_currWorkerId << endl;
cout << "m_currWorker: WorkerTask=" << m_currWorker->getTask() <<"," << m_currWorker->m_dexAlg->m_src_filename <<endl;
}

