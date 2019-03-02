/*
*-------------------------------------------------------------------
* readVideoUI.h
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

#ifndef __READVIDEO_UI_H__
#define __READVIDEO_UI_H__

#include "libUtil/DataTypes.h"
#include "avUtils.h"
#include "libUtil/RawFrm.h"
#include "libUtil/CfgAc.h"

#include "libyuv/libyuv.h"
#include "DemuxingDecoding2.h"

namespace autocut {
//using  av_seek_frame(), then read video/audio codec ctx info, then rewind the file and beging
void getVideoInfo(const char *filePath, MyVideoStreamInfo &vInfo, MyAudioStreamInfo	&aInfo);
void readVideo ( const char *filePath, const bool readVideo, const bool readAudio, int &vdieoFrmW, int &vdieoFrmH, std::vector<RawFrmPtr> &vFrm, std::vector<RawFrmPtr> &aFrm);
}
#endif
