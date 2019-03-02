/*
*-------------------------------------------------------------------
* RunThread.cpp
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
#include "RunThread.h"
using namespace std;
using namespace cv;
using namespace autocut;

RunThread::RunThread(const std::string &name )
: m_pauseWorking( true )
, m_quitProcess ( false)
, m_initProcess ( false )
, m_runLoopExit	( false )
, m_threadName(name)
#if _WITHQT==0
, m_runThread(0)
#endif
{
}


RunThread::~RunThread()
{
}

void RunThread::setCfg(CfgAcPtr &cfg)
{
m_cfg = cfg;
}

void RunThread::setDC(DataContainerPtr &dc )
{
m_dc = dc;
}

void RunThread::setInitProcess(const bool f)
{
boost::mutex::scoped_lock lock(m_mutex4Working);
m_initProcess = f;
}


#if _WITHQT==0
void RunThread::startRunThread()
{
m_runThread.reset(new boost::thread(boost::bind(&RunThread::run, this)));
}
#endif

void RunThread::run()
{
bool quitProcess = false;
while (!quitProcess) {
bool pauseJob = isPauseJob();
if (pauseJob) {
xLog("RunThread::run(): %s -- in sleep mode", m_threadName.c_str());
boost::mutex::scoped_lock lock(m_mutex4Working);
m_condition4Working.wait(lock);
xLog("RunThread::run(): %s -- waked up", m_threadName.c_str());
}

quitProcess = isQuitProcess();
if (quitProcess) {
break;
}

if (!m_initProcess) {
m_initProcess = procInit();
}
if (m_initProcess) {
procNextTask();
}
}

xLog("RunThread::run(): exit loop--%s", m_threadName.c_str());

m_mutex4Working.lock();
m_runLoopExit = true;
m_mutex4Working.unlock();
}

void RunThread::forceThreadQuit()
{
m_mutex4Working.lock();
m_quitProcess = true;
m_mutex4Working.unlock();

if ( isPauseJob() ) {
m_mutex4Working.lock();
m_condition4Working.notify_one();
m_mutex4Working.unlock();
}

while (!isRunLoopExit()) {
boost::this_thread::sleep(boost::posix_time::milliseconds(10));
}
}

bool RunThread::isRunLoopExit()
{
bool f;
m_mutex4Working.lock();
f = m_runLoopExit;
m_mutex4Working.unlock();

return f;
}

bool RunThread::isQuitProcess()
{
bool f;
m_mutex4Working.lock();
f = m_quitProcess;
m_mutex4Working.unlock();

return f;

}

bool RunThread::isPauseJob()
{
bool f;
m_mutex4Working.lock();
f = m_pauseWorking;
m_mutex4Working.unlock();

return f;
}


void RunThread::startJob()
{
boost::mutex::scoped_lock lock(m_mutex4Working);
m_condition4Working.notify_one();
m_pauseWorking = false;
}

void RunThread::pauseJob()
{
boost::mutex::scoped_lock lock(m_mutex4Working);
m_pauseWorking = true;
}


