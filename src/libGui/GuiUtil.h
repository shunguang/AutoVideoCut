/*
*-------------------------------------------------------------------
* GuiUtil.h
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
#ifndef __GUI_UTIL_H__
#define __GUI_UTIL_H__

#include "libUtil/DataTypes.h"
#include "libUtil/UtilFuncs.h"
#include "QtHeaders.h"

#include "GuiDefs.h"

namespace autocut {
enum ProgressMsgTaskId_t {
PRO_MSG_DEMUX_AUDIO = 0,
PRO_MSG_ANA_AUDIO,
PRO_MSG_DEMUX_ON_DEMOND,    //use interaction in semi-auto mode
PRO_MSG_WRT_VIDEO,
PRO_MSG_UNKN
};

struct ProgressMsg {
ProgressMsg():fileIdx(0), fileName(""), taskId(PRO_MSG_UNKN), percent(0), usedTimeSec(0) {}

int fileIdx;
std::string fileName;
int taskId;           //0-demux audio, 1-anaAudio, 2-PRO_MSG_DEMUX_ON_DEMOND, 3-write output video
int percent;
int usedTimeSec;
};
QString GUI_EXPORT encodeProgressMsg(const ProgressMsg &msg);
ProgressMsg GUI_EXPORT decodeProgressMsg(const QString &s);

QImage GUI_EXPORT cvMatToQImage(const cv::Mat &inMat);
QPixmap GUI_EXPORT cvMatToQPixmap(const cv::Mat &inMat, bool isBGR=true);
cv::Mat GUI_EXPORT QImage2cvMat(const QImage &image);

bool GUI_EXPORT isVideoFile(const std::string &filePath);
bool GUI_EXPORT isImgeFile(const std::string &filePath);

}

#endif 

