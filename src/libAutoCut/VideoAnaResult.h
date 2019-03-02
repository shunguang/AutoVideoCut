/*
*-------------------------------------------------------------------
* VideoAnaResult.h
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

#ifndef __VIDEO_ANA_RESULT_H__
#define __VIDEO_ANA_RESULT_H__
#include "libUtil/RawFrm.h"
namespace autocut {
struct VideoAnaResult {
VideoAnaResult():durationSec(0), videoPath(""), vSelectedFrmRgns() {}

VideoAnaResult(const VideoAnaResult &x) : durationSec(x.durationSec), videoPath(x.videoPath), vSelectedFrmRgns(x.vSelectedFrmRgns) {}

~VideoAnaResult() {
vSelectedFrmRgns.clear();
}

VideoAnaResult& operator =(const VideoAnaResult &x) {
if (this != &x) {
durationSec = x.durationSec;
videoPath = x.videoPath;
vSelectedFrmRgns = x.vSelectedFrmRgns;
}
return *this;
}

float32					durationSec;
std::string				videoPath;
std::vector<FrmRange>	vSelectedFrmRgns;
};
}
#endif
