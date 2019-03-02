/*
*-------------------------------------------------------------------
* MsgBox.h
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
#ifndef __MSG_BOX_H__
#define __MSG_BOX_H__

#include "libUtil/DataTypes.h"
#include "QtHeaders.h"
#include "GuiDefs.h"

namespace autocut {
//auto close message box
class MsgBox : public QMessageBox
{
Q_OBJECT
public:
MsgBox(const std::string &winTitle, const std::string &msg, const int timeout_ms, QRect *rect = NULL);
~MsgBox();

bool isClosed() {
return m_isClosed;
}

public slots:
void autoClose()
{
this->done(0);
m_isClosed = true;
}

//-------------------------------
private:
QTimer* m_timer;
bool m_isClosed;
};

//always on top, non-auto close, block other application on GUI until <OK> is pressed
void GUI_EXPORT popupMsg(QWidget *parent,  const std::string &winTitle, const std::string &msg, const QMessageBox::Icon myIcon = QMessageBox::Critical);

//popup msg automaticlly closing after <timeout_ms>
void GUI_EXPORT popupInfoMsg(const std::string &winTitle, const std::string &msg, const uint32 timeout_ms = 0, QRect *placeRect = NULL);

}
#endif
