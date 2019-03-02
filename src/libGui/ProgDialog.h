/*
*-------------------------------------------------------------------
* ProgDialog.h
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
#ifndef PROG_DIALOG_H__
#define PROG_DIALOG_H__

#include <chrono>
#include <thread>

#include <QtCore/QString>
#include <QtWidgets/QProgressDialog>
#include <QtWidgets/QApplication>
namespace autocut {
class ProgDialog : public QProgressDialog
{
Q_OBJECT
public:
ProgDialog(QWidget *parent, const char *winTitle, int x0 = 0, int y0 = 0, int w = 0, int h = 0)
: QProgressDialog(parent)
{
this->setWindowTitle(tr(winTitle));
this->setCancelButton(0);
this->setWindowModality(Qt::WindowModal);
this->setMaximum(100);
this->setMinimum(0);
this->setAutoClose(true);
this->setAutoReset(true);
this->setWindowFlags(Qt::WindowStaysOnTopHint);

if (w > 0 && h > 0) {
this->setGeometry(x0, y0, w, h);
this->setMinimumWidth(w);
this->setMinimumHeight(h);
}
}


void resetWinTitle(const char *winTitle)
{
this->setWindowTitle(tr(winTitle));
}

void resetGeometry(int xc, int yc, int w, int h)
{
int x0 = xc - (int)(w / 2);
int y0 = yc - (int)(h / 2);

if (x0 < 0) x0 = 1;
if (y0 < 0) y0 = 1;

this->setGeometry(x0, y0, w, h);
}

void setProgress(const int val, const std::string &msg = "")
{
if (this->wasCanceled())
return;

if (!msg.empty()) {
this->setLabelText(QString::fromStdString(msg));
}
this->setValue(val);
QApplication::processEvents();

if (val == this->maximum()) {
std::this_thread::sleep_for(std::chrono::milliseconds(200));
this->cancel();
}
}

void forceCancel()
{
if (!this->wasCanceled()) {
this->cancel();
}
}
};
}
#endif
