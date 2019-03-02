/*
*-------------------------------------------------------------------
* Worker.h
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
#ifndef _WOKER_H_
#define _WOKER_H_
#include "libutil/util.h"
#include "libutil/FifoQue.h"
#include "RunThread.h"
#include "libGui/GuiUtil.h"
#include "libGui/MultiRngSliders.h"
#include "libAutoCut/DemuxingDecoding2.h"
#include "libAutoCut/LocateHighlightFrms.h"
namespace autocut {
enum WorkerTask{
W_TASK_DEX_WHOLE = 0,  //demuxing decoding audio and key frms
W_TASK_TIME_ANA,       //time-freq analysis 
W_TASK_DEX_ON_DEMOND,  //decoding basked on demond
W_TASK_UNKN
};

struct WorkProgressInfo {
WorkProgressInfo() : workId(-1), taskId(-1) {}
int	workId;
int	taskId;
};

class Worker : public RunThread {
#if  _WITHQT
Q_OBJECT
#endif
public:
Worker(const std::string &videoFilePath, const int fileId);
~Worker();
virtual void endJob();
virtual void setCfg(CfgAcPtr  &cfg);
void setBatchDecodeFrmSize(const int n) {
boost::mutex::scoped_lock lock(m_worker_mutex);
m_nMinVideoFrmsInBatchDecode = n;
}

void setCopyDexFrmsIntoQ(const bool f) {
boost::mutex::scoped_lock lock(m_worker_mutex);
m_copyDexFrmsIntoQ = f;
}

bool isCopyDexFrmsIntoQ() {
bool f;
{
boost::mutex::scoped_lock lock(m_worker_mutex);
f = m_copyDexFrmsIntoQ;
}
return f;
}

void setTask(const WorkerTask f) {
boost::mutex::scoped_lock lock(m_worker_mutex);
m_task = f;
}

WorkerTask getTask()  {
WorkerTask f;
{
boost::mutex::scoped_lock lock(m_worker_mutex);
f = m_task;
}
return f;
}

//--- for random access dispFrm ----------
bool getNextFrm( RawFrmPtr &frm);
float64 startDecodeFrms(const uint64 keyFrmIdx );
void stopDecodeFrms();

uint64 getVideoFrmsInRng(const RngWorld &keyFrmIdxRng );

void getSelectedKeyFrmRanges(std::vector<RngWorld> &vKeyFrmRngs) const;

QString encoding_progress_msg(const WorkerTask task) {
return QString("wokerId_%1;taskId%2").arg(m_videoFileId).arg((int)task);
}

WorkProgressInfo encoding_progress_msg(const QString &msg) {
WorkProgressInfo x;
std::string s = msg.toStdString();
sscanf_s(s.c_str(), "wokerId_%d;taskId%d", &x.workId, &x.taskId);
return x;
}

void resetCfg4DecodeAudioAndKeyFrms(int thumbsnailImgW, int thumbsnailImgH, int dispImgW, int dispImgH);
void resetCfg4BrowseVideo(int dispImgW, int dispImgH);
void resetCfg4WrtVideo(int outputImgW, int ouputImgH);

#if  _WITHQT
signals :
void sigHasErr(const QString &errMsg);
void sigProgress(const QString &msg);
void sigLastFrmDexed();
void sigJobDone();
#endif
protected:
//set <isEmitSignal> as false when doing single thread none GUI test
virtual void procNextTask(bool isEmitSignal = true);  //Note: the only one frm job is decoing a file
virtual bool procInit(bool isEmitSignal = true);
void dexWholeAudioAndKeyfrms();
void findHighlightFrms();
void dexVideoFrmsFromPreSetLocation();

public:
CfgDemuxingDecodingPtr  m_dexCfg;

MyVideoStreamInfo		m_inpVideoStreamInfo;
MyAudioStreamInfo       m_inpAudioStreamInfo;

DemuxingDecoding2Ptr	m_dexAlg;
LocateHighlightFrmsPtr  m_anaAlg;

std::string				m_videoFilePath;
int						m_videoFileId;

bool							m_decDispFrm;
boost::mutex					m_worker_mutex;

bool							m_lastVideoFrmDecoded;
int								m_nMinVideoFrmsInBatchDecode;
bool							m_copyDexFrmsIntoQ;
protected:
WorkerTask						m_task;
};
typedef std::shared_ptr<Worker>		WorkerPtr;
}
#endif
