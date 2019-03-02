/*
*-------------------------------------------------------------------
* ImgLabel.h
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
#ifndef _IMG_LABEL_H_
#define _IMG_LABEL_H_

#include <QtCore/QtCore>
#include <QtGui/QtGui>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>
namespace autocut {
class ImgLabel : public QLabel{
Q_OBJECT

public:
ImgLabel(QWidget *parent = 0);


//bool saveImage(const QString &fileName, const char *fileFormat);
void resetNewFram();
void setDrawEnabled(const bool isEnabled);
void setPenColor(const QColor &newColor);
void setPenWidth(int newWidth);

QColor penColor() const			{ return myPenColor; }
int penWidth() const			{ return myPenWidth; }
const QRect &getSelectedRect() const { return lastRect; }

signals:
void sigRectSelected(const QRect &roi);
void sigLeftButtonPressed(const QPoint &pt);

protected:
void mousePressEvent(QMouseEvent *event) override;
void mouseMoveEvent(QMouseEvent *event) override;
void mouseReleaseEvent(QMouseEvent *event) override;
//void paintEvent(QPaintEvent *event) override;

private:
void	drawRect();

bool    drawDisabled;  //do not change to <drawEnabled>
bool	isStarted;
bool	isMoved;
int		myPenWidth;
QColor	myPenColor;
QPoint	begPoint;
QPoint  endPoint;
QRect   lastRect;
};
}
#endif
