/*
*-------------------------------------------------------------------
* RunThread.h
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
#ifndef _RUN_THREAD_H_
#define _RUN_THREAD_H_

//note <_WITHQT> defined by preprocessor

#include "RunDefs.h"

#if _WITHQT
#include <QtCore/qthread.h>
#endif

#include "libUtil/util.h"
#include "libUtil/CfgAc.h"
#include "libUtil/DataContainer.h"

namespace autocut {

#define IS_CHECK_TIME_PROFILE 1

#if  _WITHQT
class LIBRUN_EXPORT RunThread : public QThread {
Q_OBJECT
#else
class LIBRUN_EXPORT RunThread {
#endif
public:
RunThread( const std::string &name="unkn" );
~RunThread();

void setInitProcess(const bool f);

void forceThreadQuit();
void startJob();
void pauseJob();

bool isRunLoopExit();
bool isQuitProcess();
bool isPauseJob();

virtual void endJob() = 0;
virtual void setCfg( CfgAcPtr  &cfg);
void setDC(DataContainerPtr &dc);

#if _WITHQT==0
void startRunThread();
#endif

protected:
//set <isEmitSignal> as false when doing single thread none GUI test
virtual void procNextTask( bool isEmitSignal = true ) = 0;
virtual bool procInit( bool isEmitSignal = true ) = 0;

#if  _WITHQT
void run() Q_DECL_OVERRIDE;
#else
void run();
#endif

protected:
CfgAcPtr			m_cfg;
DataContainerPtr	m_dc;
std::string			m_threadName; //for debugging purpose
//bool				m_isDumpDispImg;
//std::string			m_dumpPath;

bool    m_runLoopExit;
bool	m_pauseWorking;
bool    m_quitProcess;
bool	m_initProcess;

#if _WITHQT==0
std::shared_ptr<boost::thread>	 m_runThread;
#endif
boost::mutex				m_mutex4Working;
boost::condition_variable	m_condition4Working;
};
typedef std::shared_ptr<RunThread>		RunThreadPtr;
}
#endif
