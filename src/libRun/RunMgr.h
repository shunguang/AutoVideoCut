/*
*-------------------------------------------------------------------
* RunMgr.h
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
#ifndef _RUN_MGR_H_
#define _RUN_MGR_H_

#include "RunThread.h"
#include "libGui/GuiUtil.h"
#include "libGui/MultiRngSliders.h"
#include "libAutoCut/WrtOutputVideo.h" 
#include "libAutoCut/ProjProfile.h"

#include "Worker.h"
namespace autocut {
class LIBRUN_EXPORT RunMgr : public RunThread {
#if  _WITHQT
Q_OBJECT
#endif
public:
RunMgr();
~RunMgr();
virtual void endJob();
virtual void setCfg(CfgAcPtr  &cfg);

bool resetWorkers();
void forceAllWorkersQuit();
void setTaskId(const ProgressMsgTaskId_t t);
void respReset();

int	 getNumWorkers() const {
return m_nWorkers;
}
uint64  getCurrVideoKeyFrms() const;
float64 getCurrVideoTimeSec(const uint64 videoFrmNum) const;
float64 getCurrVideoTimeSecFromKeyFrmIdx(const uint64 videoKeyFrmIdx, uint64 &keyFrmNum) const;
float64 getCurrVideoDurationSec() const;
bool    getCurrSelectedKeyFrmRanges(std::vector<RngWorld> &vKeyFrmRngs) const;
bool    getAlgSelectedKeyFrmRanges(int workerId, std::vector<RngWorld> &vKeyFrmRngs) const;

bool    getThumbnailFrm(const uint64 keyFrmIdx, RawFrmPtr &frm);
bool    getFirstFrm2Disp(const int fileId, DispFrm &disp);

//random access dispImgs only for one worker at each time set by <m_currWorkerId>
int  getCurrWorkerId();
void setCurrWorkerId(const int id);

void startDecoding( const uint64 keyFrmIdx );

void stopDecoding();
bool getNextDispFrm(cv::Mat &bgr);
void emitProgressSignal(const ProgressMsg &msg);

void startDexWhole();
void startAnaAudio();
void startWriteOutputVideo(std::vector<std::vector<RngWorld>> &vSelectedKeyFrmRngs);
void writeOuputVideo();

void setProjProfileInfo();
void saveProjProfile(const int fileId, const int jobId);
bool loadMp3FromProjProfile(int &dt_ms);
void printBrief();
signals:
void sigHasErr(const QString &errMsg);
void sigProgress(const QString &msg);
void sigAllDexed(const QString &msg);
void sigAllAnalyzed(const QString &msg);
void sigWrtOutputDone(const QString &msg);

public slots:
void wakeupWorkerToDex();


protected:
//set <isEmitSignal> as false when doing single thread none GUI test
virtual void procNextTask( bool isEmitSignal = true );
virtual bool procInit(bool isEmitSignal = true );
bool checkCfg(std::string &errMsg);
bool createWorkers();
void dexInParallel(bool isEmitSignal);
void anaInParallel(bool isEmitSignal);
bool isAllDexThreadsInPause();
uint32 writeOneSelctedRng( const RngWorld &keyFrmIdxRng, const float timeOffsetSec);

public:
Worker*                 m_currWorker;
protected:
ProgressMsgTaskId_t		m_taskId;
int						m_nWorkers;

int						m_currWorkerId;
std::vector<WorkerPtr>  m_vWorkers;         //each worker is a thread
boost::mutex			m_mutexMgr;

WrtOutputVideoPtr		m_videoWriter;
std::vector<std::vector<RngWorld>> m_vSelectedKeyFrmIdxRngs;

ProjProfilePtr			m_projProfile;
};
typedef std::shared_ptr<RunMgr>		RunMgrPtr;
}
#endif

