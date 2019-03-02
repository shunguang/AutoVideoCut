/*
*-------------------------------------------------------------------
* RunMuxEnc.h
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
#ifndef _RUN_AUTO_CUT_CORE_H_
#define _RUN_AUTO_CUT_CORE_H_

#include "RunThread.h"
#include "libGui/GuiUtil.h"
#include "libAutoCut/AutoCut.h"
namespace autocut {
class LIBRUN_EXPORT RunAutoCutCore : public RunThread {
#if  _WITHQT
Q_OBJECT
#endif
public:
RunAutoCutCore();
~RunAutoCutCore();
virtual void endJob();

#if  _WITHQT
signals:
void sigHasErr(const QString &errMsg);
void sigProgress(const QString &msg);
void sigJobDone();
#endif
protected:
//set <isEmitSignal> as false when doing single thread none GUI test
virtual void procNextFrm( bool isEmitSignal = true );
virtual bool procInit(bool isEmitSignal = true );
bool checkCfg(std::string &errMsg);
protected:
AutoCutPtr					m_autoCut;
};
typedef std::shared_ptr<RunAutoCutCore>		RunAutoCutCorePtr;
}
#endif
