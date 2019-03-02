/*
*-------------------------------------------------------------------
* RunBrowseVideo.cpp
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
#include "libGui/GuiUtil.h"
#include "RunBrowseVideo.h"
using namespace std;
using namespace cv;
using namespace autocut;

RunBrowseVideo::RunBrowseVideo()
: RunThread("RunBrowseVideo")
, m_isLastFrmReached(false)
, m_frmRate(30)
{
}

RunBrowseVideo::~RunBrowseVideo()
{
}

void RunBrowseVideo::respReset()
{
pauseJob();
int cnt = 0;
while (1) {
boost::this_thread::sleep(boost::posix_time::milliseconds(10));
if (isPauseJob()) {
break;
}
if ( (cnt > 100) &&  (cnt % 100) == 0 ) {
xLog("RunBrowseVideo::respReset(): strugling ...");
}
++cnt;
}
m_isLastFrmReached = false;
}


void RunBrowseVideo::setBrowseMode(const BrowseMode m) 
{
boost::mutex::scoped_lock lock(m_mutex4BV);
m_browseMode = m;
}

BrowseMode RunBrowseVideo::getBrowseMode() 
{
BrowseMode m;
{
boost::mutex::scoped_lock lock(m_mutex4BV);
m = m_browseMode;
}
return m;
}

void RunBrowseVideo::setLastFrmReached(bool flag)
{
boost::mutex::scoped_lock lock(m_mutex4BV);
m_isLastFrmReached = flag;
}

void RunBrowseVideo::setFrmRate(float r)
{
boost::mutex::scoped_lock lock(m_mutex4BV);
m_frmRate = r;
}

bool RunBrowseVideo::isLastFrmReached()
{
bool f;
{
boost::mutex::scoped_lock lock(m_mutex4BV);
f = m_isLastFrmReached;
}
return f;
}

void RunBrowseVideo::procNextTask(bool isEmitSignal)
{
BrowseMode m = getBrowseMode();
if (m == BROWSE_FIRST_FRM_ONLY) {
procNextFrm_first_only();
}
else if (m == BROWSE_UTIL_PAUSE){
procNextFrm_until_pause();
}
else{
xLog(" RunBrowseVideo::procNextTask(): sth wrong!");
}
}

void RunBrowseVideo::procNextFrm_first_only()
{
RawFrmPtr frm;
m_dc->waitPopVideoFrm(frm);
pushToDispFrmQ( frm );
pauseJob();
}

void RunBrowseVideo::procNextFrm_until_pause()
{
int T0 = (int)(1000/m_frmRate); //ms
boost::posix_time::ptime tBeg = POSIX_LOCAL_TIME;

//------------------------------------------------
//4. disp results
//------------------------------------------------
RawFrmPtr frm;
bool hasData = m_dc->tryPopVideoFrm( frm );
if ( hasData ) {
pushToDispFrmQ(frm);
}
else if ( !isLastFrmReached() ) {
emit sigDexFrms();
}
//prevent too fast (it may mess up GUI)
int dt = timeIntervalMillisec( tBeg );
if ( T0 > dt ) {
boost::this_thread::sleep(boost::posix_time::milliseconds(T0-dt));
}
}

void RunBrowseVideo::pushToDispFrmQ(const RawFrmPtr &raw)
{
//dumpFrmImg( *raw.get(), "c:/temp", "ori");
//cout << "RunBrowseVideo::pushToDispFrmQ(): keyFrmIdx=" << raw->keyFrmId_ << ", frmNum=" << raw->fn_ << endl;
DispFrmPtr newFrm( new DispFrm(m_dispImgSz.width, m_dispImgSz.height) );
if (raw->fn_ == LAST_FRM_NUM) {
setLastFrmReached(true);
}
newFrm->m_fn = raw->fn_;
newFrm->m_keyFrmIdx = raw->keyFrmId_;
newFrm->m_timeStamp_ms = 0;

raw->toBGR(m_oriImg);
if (m_dispImgSz.width != raw->w_ || m_dispImgSz.height != raw->h_) {
cv::resize(m_oriImg, newFrm->m_img, m_dispImgSz);
}
else {
m_oriImg.copyTo(newFrm->m_img);
}

m_dc->pushDispFrm(newFrm);
emit sigDispImg();

//cv::imwrite("c:/temp/disp-frm0.png",newFrm->m_img);
}

//
bool RunBrowseVideo::procInit( bool isEmitSignal )
{
int w, h;
m_cfg->getDispImgSize(w, h);
m_dispImgSz = cv::Size(w, h);
setLastFrmReached (false);
return true;
}

void RunBrowseVideo::endJob()
{
pauseJob();
//need to reinitializing cature once weaked up
setInitProcess( false );
}
