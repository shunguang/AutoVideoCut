/*
*-------------------------------------------------------------------
* LocateHighlightFrms.h
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

#ifndef __LOCATE_HIGHLIGHT_FRMS_H__
#define __LOCATE_HIGHLIGHT_FRMS_H__

#include "libyuv/libyuv.h"
#include "libUtil/RawFrm.h"
#include "libUtil/DataTypes.h"
#include "libUtil/UtilFuncs.h"
#include "libUtil/VisLog.h"
#include "libUtil/CfgAc.h"

#include "avUtils.h"
#include "TimeFreqAna.h"
#include "VideoAnaResult.h"
namespace autocut {
class LocateHighlightFrms {
public:
LocateHighlightFrms(const int threadId );
~LocateHighlightFrms();

void setCfg(CfgAcPtr &cfg) {
m_cfg = cfg;
}

void setAudioStreamInfo(const MyAudioStreamInfo &inpAudioStreamInfo) {
m_inpAudioStreamInfo = inpAudioStreamInfo;
}

//process <vAudioFrms>, results are in <m_vSelectedFrmRngs>
int process(const std::vector<RawFrmPtr> &vAudioFrms, const std::string &originalVideoFileName /*used for cute dump*/);

private:
int	preppareRawAudio4Stfft(const std::vector<RawFrmPtr> &v);
int doTimeFreqAna( const std::string &dumpFilenameTag );

public:
std::vector<FrmRange> m_vSelectedFrmRngs;

private:
CfgAcPtr			m_cfg;
MyAudioStreamInfo	m_inpAudioStreamInfo;
RawFrm              m_audio4Fft;				//the whole audio raw data  from the input videofile
const int			m_threadId;
};

typedef std::shared_ptr<LocateHighlightFrms>		LocateHighlightFrmsPtr;
}
#endif
