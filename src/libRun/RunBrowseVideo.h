/*
*-------------------------------------------------------------------
* RunBrowseVideo.h
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
#ifndef _RUN_BROWSE_VIDEO_H_
#define _RUN_BROWSE_VIDEO_H_

#include "RunThread.h"

namespace autocut {
enum BrowseMode {
BROWSE_FIRST_FRM_ONLY = 0,  //only display the first frm in m_dc->m_videoFrmQ
BROWSE_UTIL_PAUSE
};

class LIBRUN_EXPORT RunBrowseVideo : public RunThread {
Q_OBJECT
public:
RunBrowseVideo();
~RunBrowseVideo();
virtual void endJob();

void startDisp(const BrowseMode m) {
setBrowseMode(m);
startJob();
}
void setFrmRate(float r);
void setLastFrmReached(bool flag);
bool isLastFrmReached();
void clearDispQ() {
m_dc->clearDispQ();
}
void respReset();
signals:
void sigHasErr( const QString &errMsg );
void sigDispImg();
void sigDexFrms();
protected:
//set <isEmitSignal> as false when doing single thread none GUI test
virtual void procNextTask( bool isEmitSignal = true );
virtual bool procInit(bool isEmitSignal = true );

void procNextFrm_first_only();
void procNextFrm_until_pause();
void setBrowseMode(const BrowseMode m);
BrowseMode getBrowseMode();

void pushToDispFrmQ(const RawFrmPtr &raw);
protected:
float		m_frmRate;
BrowseMode	m_browseMode;
cv::Size m_dispImgSz;
cv::Mat  m_oriImg;
bool     m_isLastFrmReached;
boost::mutex				m_mutex4BV;
};
typedef std::shared_ptr<RunBrowseVideo>		RunBrowseVideoPtr;
}
#endif
