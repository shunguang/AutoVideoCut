/*
*-------------------------------------------------------------------
* Worker.cpp
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
#include "Worker.h"
using namespace std;
using namespace cv;
using namespace autocut;

Worker::Worker(const std::string &videoFilePath, const int fileId)
: RunThread(string("Worker") + std::to_string(fileId))
, m_task(W_TASK_DEX_WHOLE)
, m_dexCfg(new CfgDemuxingDecoding())
, m_dexAlg(0)
, m_anaAlg(0)
, m_videoFilePath(videoFilePath)
, m_videoFileId(fileId)
, m_lastVideoFrmDecoded(false)
, m_nMinVideoFrmsInBatchDecode(1)
, m_copyDexFrmsIntoQ(true)
{
}

Worker::~Worker()
{
}

void Worker::setCfg(CfgAcPtr  &cfg) 
{
RunThread::setCfg(cfg);
bool isRefCount = false;

//set <m_dexCfg>
resetCfg4DecodeAudioAndKeyFrms( cfg->m_gui.thumbnailWidth, cfg->m_gui.thumbnailHeight, cfg->m_gui.dispImgWidth, cfg->m_gui.dispImgHeight);

m_dexAlg.reset(new DemuxingDecoding2(m_dexCfg));
m_dexAlg->openVideoFileToRead(m_videoFilePath.c_str(), isRefCount);
//m_dexAlg->readFirstVideoFrmThenRewindToBegining();

m_anaAlg.reset( new LocateHighlightFrms(m_videoFileId) );
m_anaAlg->setCfg(cfg);
}

void Worker::procNextTask(bool isEmitSignal)
{
const WorkerTask task = getTask();
if (task == W_TASK_DEX_WHOLE) {
//in stage 0, we decode all audios and keyfrm imges which are stored in <m_dexAlg->m_vThumbnailKeyFrms> 
dexWholeAudioAndKeyfrms();
emit sigProgress( encoding_progress_msg(W_TASK_DEX_WHOLE) );
pauseJob();
}
else if (task == W_TASK_TIME_ANA) {
findHighlightFrms();
emit sigProgress(encoding_progress_msg(W_TASK_TIME_ANA));
//after this we will not read audios anymore, and the cfg settings will no change anymore
resetCfg4BrowseVideo(m_cfg->m_gui.dispImgWidth, m_cfg->m_gui.dispImgHeight);
pauseJob();
setTask(W_TASK_DEX_ON_DEMOND );
}
else if(task == W_TASK_DEX_ON_DEMOND) {
//corresponding to user browsing & play video from key frm slider
dexVideoFrmsFromPreSetLocation();
}
else {
pauseJob();
}
}

void Worker::dexWholeAudioAndKeyfrms()
{
bool suc = m_dexAlg->loadDecodedAudioVideoFrms();
if (!suc) {
m_dexAlg->decodeAll(NULL, NULL);
m_dexAlg->update_audio_streamInfo();
m_dexAlg->update_video_streamInfo();

//automatic save it for furte using
m_dexAlg->saveDecodedAudioVideoFrms();
}
m_dexAlg->printProperties();

//m_videoFrmSzInBytes = m_demux->getRawVideoFrameSize();
//m_audioFrmSzInBytes = m_demux->getRawAudioFrameSize();
//m_audioSmpingFreq = m_demux->getAudioSamplingFreq();
//m_audioFrmSmples = m_demux->getAudioFrmSamples();

m_inpVideoStreamInfo = m_dexAlg->m_videoStreamInfo;
m_inpAudioStreamInfo = m_dexAlg->m_audioStreamInfo;

}

void Worker::findHighlightFrms()
{
AutoCutMethod m = m_cfg->get_cutMethod();
if (m == AUTO_CUT_MANMUAL) {
m_dexAlg->m_vAudioFrms.clear();
boost::this_thread::sleep(boost::posix_time::milliseconds(50));
}
else {
m_anaAlg->setAudioStreamInfo(m_inpAudioStreamInfo);
//process <vAudioFrms>, results are in <m_vSelectedFrmRngs>
const std::vector<RawFrmPtr> &v = m_dexAlg->m_vAudioFrms;
m_anaAlg->process(v, m_videoFilePath /*used for cute dump*/);
}

}

void Worker::dexVideoFrmsFromPreSetLocation()
{
if (m_lastVideoFrmDecoded) {
pauseJob();
return;
}

int readFrmRet = m_dexAlg->decodeSomeVideo(m_nMinVideoFrmsInBatchDecode, true ); //decode at least one video frme
//printf("dexVideoFrms(): %s, readFrmRet=%d, videoFile=%s\n", m_threadName.c_str(), readFrmRet, m_dexAlg->m_src_filename.c_str());
m_lastVideoFrmDecoded = (readFrmRet < 0);
bool isCopyToQ = isCopyDexFrmsIntoQ();
if (!isCopyToQ) {
pauseJob();
return;
}

BOOST_FOREACH( const RawFrmPtr &p, m_dexAlg->m_vVideoFrms) {
//cout << "Worker::dexVideoFrmsFromPreSetLocation(): keyFrmIdx=" << p->keyFrmId_ << ", frmNum" << p->fn_ << endl;
//dumpFrmImg( *p.get(), "c:/temp", m_threadName);
m_dc->pushVideoFrm(p);
}
BOOST_FOREACH(const RawFrmPtr &p, m_dexAlg->m_vAudioFrms) {
m_dc->pushAudioFrm(p);
}

if ( m_dc->isVideoQueFull()  || m_lastVideoFrmDecoded ) {
pauseJob();
}

if ( m_lastVideoFrmDecoded ) {
emit sigLastFrmDexed();
}
}

//set <isEmitSignal> as false when doing single thread non GUI test
bool Worker::procInit( bool isEmitSignal )
{
return true;
}

void Worker::endJob()
{
m_dexAlg->closeOpenedFiles();

pauseJob();
//need to reinitializing cature once weaked up
setInitProcess( false );
emit sigJobDone();
}

float64 Worker::startDecodeFrms( const uint64 keyFrmIdx)
{
m_lastVideoFrmDecoded = false;
pauseJob();
while (!isPauseJob()) {
boost::this_thread::sleep(boost::posix_time::milliseconds(5));
}

float64 t_sec = m_dexAlg->seekVideoFrm(keyFrmIdx);
m_dc->clearVideoQ();
m_dc->clearAudioQ();
//printf("Worker::startDecodeFrms():%s, newTimeSec=%f, qSize=%d\n", m_threadName.c_str(), newTimeSec, m_videoFrmQ.size());
assert(isPauseJob());
m_task == W_TASK_DEX_ON_DEMOND;
startJob();
return t_sec;
}

void Worker::stopDecodeFrms()
{
pauseJob();
}

uint64 Worker::getVideoFrmsInRng(const RngWorld &keyFrmIdxRng)
{
uint64 keyFrmNum1 = m_dexAlg->getVideoFrmIdxFromKeyFrmIdx(keyFrmIdxRng.lower);
uint64 keyFrmNum2 = m_dexAlg->getVideoFrmIdxFromKeyFrmIdx(keyFrmIdxRng.upper);
return keyFrmNum2 - keyFrmNum1 + 1;
}
bool Worker::getNextFrm(RawFrmPtr &frm)
{
if (m_lastVideoFrmDecoded) {
return false;
}
if ( m_dc->isVideoQueEmpty() ){
if (isPauseJob()) {
startJob();
boost::this_thread::sleep(boost::posix_time::milliseconds(50));
}
}
return m_dc->tryPopVideoFrm(frm);
}



void Worker::resetCfg4BrowseVideo(int dispImgW, int dispImgH)
{
m_dexCfg->demuxingVideo = true;
m_dexCfg->dumpRawVideo = false;
m_dexCfg->storeRawVideoInMemo = true;
m_dexCfg->storeThumbnailKeyFrms = false;
m_dexCfg->outputImgWidth  = dispImgW;
m_dexCfg->outputImgHeight = dispImgH;

//audio params
m_dexCfg->demuxingAudio = false;
m_dexCfg->dumpRawAudio = false;
m_dexCfg->storeRawAudioInMemo = false;
}

void Worker::resetCfg4WrtVideo(int outputImgW, int ouputImgH)
{
m_dexCfg->demuxingVideo = true;
m_dexCfg->dumpRawVideo = false;
m_dexCfg->storeRawVideoInMemo = true;
m_dexCfg->storeThumbnailKeyFrms = false;
m_dexCfg->outputImgWidth = outputImgW;
m_dexCfg->outputImgHeight = ouputImgH;


//audio params
m_dexCfg->demuxingAudio = true;
m_dexCfg->dumpRawAudio = false;
m_dexCfg->storeRawAudioInMemo = true;
}

//video params
void Worker::resetCfg4DecodeAudioAndKeyFrms(int thumbsnailImgW, int thumbsnailImgH, int dispImgW, int dispImgH)
{
m_dexCfg->demuxingVideo = true;
m_dexCfg->dumpRawVideo = false;
m_dexCfg->storeRawVideoInMemo = false;
m_dexCfg->storeThumbnailKeyFrms = true;
m_dexCfg->thumbnailWidth = thumbsnailImgW;
m_dexCfg->thumbnailHeight = thumbsnailImgH;
m_dexCfg->outputImgWidth = dispImgW;
m_dexCfg->outputImgHeight = dispImgH;

//audio params
m_dexCfg->demuxingAudio = true;
m_dexCfg->dumpRawAudio = false;
m_dexCfg->storeRawAudioInMemo = true;
}

void Worker::getSelectedKeyFrmRanges(std::vector<RngWorld> &vKeyFrmRngs) const
{
float64 t1, t2;		//time in sec
uint64 vfn1, vfn2;  //video frm num
uint64 kfId1, kfId2; //key frm index
vKeyFrmRngs.clear();
BOOST_FOREACH(const FrmRange &r, m_anaAlg->m_vSelectedFrmRngs) {
cout << r.toStr() << endl;
if (r.aid1 == 0 && r.aid2 == 0) {
continue;
}

t1 = m_dexAlg->getAudioTimeSecFromFrmIdx(r.aid1);
t2 = m_dexAlg->getAudioTimeSecFromFrmIdx(r.aid2);
vfn1 = m_dexAlg->getVideoFrmNumFromTime(t1);
vfn2 = m_dexAlg->getVideoFrmNumFromTime(t2);

kfId1 = m_dexAlg->getVideoKeyFrmIdxFromFrmIdx(vfn1);
kfId2 = m_dexAlg->getVideoKeyFrmIdxFromFrmIdx(vfn2);

if (kfId1 == UINT64_NAN || kfId2 == UINT64_NAN) {
continue;
}
vKeyFrmRngs.push_back( RngWorld(kfId1, kfId2) );
}
}
