/*
*-------------------------------------------------------------------
* MsgBox.cpp
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
#include "MsgBox.h"

using namespace std;
using namespace autocut;

MsgBox::MsgBox(const std::string &winTitle, const std::string &msg, const int timeout_ms, QRect *rect)
: QMessageBox()
, m_timer(NULL)
, m_isClosed(false)
{
m_timer = new QTimer(this);
this->setModal(true);
this->setWindowTitle(QString::fromStdString(winTitle));
this->setIcon(QMessageBox::Information);
this->setStandardButtons(QMessageBox::Ok);
this->setAttribute(Qt::WA_DeleteOnClose, true);
this->setWindowFlags(Qt::WindowStaysOnTopHint);
if (rect) {
this->setGeometry(*rect);
}
this->setText(QString::fromStdString(msg));

QObject::connect(m_timer, SIGNAL(timeout()), this, SLOT(autoClose()));

m_timer->start(timeout_ms);
}

MsgBox::~MsgBox()
{
if (m_timer) {
delete m_timer;
}
}

//always on top, non-auto close, block other application on GUI until <OK> is pressed
//always on top, non-auto close, block other application on GUI until <OK> is pressed
void autocut::popupMsg(QWidget *parent, const std::string &winTitle, const std::string &msg, const QMessageBox::Icon myIcon)
{
//todo: why the window no boder and title??

QMessageBox *msgBox = new QMessageBox(myIcon,
QString::fromStdString(winTitle),
QString::fromStdString(msg),
QMessageBox::Ok,
parent, Qt::Dialog);

msgBox->setWindowModality(Qt::ApplicationModal);
msgBox->setModal(true);
msgBox->setWindowFlags(Qt::WindowStaysOnTopHint);
msgBox->setAttribute(Qt::WA_DeleteOnClose, true);
//if (parent) {
//	msgBox->exec(); //block other bapplication on GUI
//}
//else {
msgBox->show(); //cannot block other bapplication on GUI
//}
QApplication::processEvents();
}

void autocut::popupInfoMsg(const std::string &winTitle, const std::string &msg, const uint32 timeout_ms, QRect *placeRect)
{
QRect rect(100, 100, 300, 100);
if (placeRect ) {
rect = *placeRect;
}
MsgBox *x = new MsgBox(winTitle, msg, timeout_ms, &rect);
x->show();
QApplication::processEvents();

//wus1 note: 
//Do not worry about the delete of <x>, because we have setAttribute(Qt::WA_DeleteOnClose, true) in <MsgBox2>
//google "QMessageBox delete on close" or see
//http://stackoverflow.com/questions/26925630/qmessagebox-delete-on-close
}

