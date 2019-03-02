/*
*-------------------------------------------------------------------
* ImgLabel.cpp
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
#include "ImgLabel.h"

using namespace std;
using namespace autocut;

ImgLabel::ImgLabel( QWidget *parent)
: QLabel( parent )
, drawDisabled( false )
, isStarted( false )
, isMoved( false )
, myPenWidth (1)
, myPenColor (Qt::blue)
, begPoint(0, 0)
, endPoint(0, 0)
, lastRect(0,0,0,0)
{
}


void ImgLabel::setDrawEnabled( const bool isEnabled )
{
drawDisabled = !isEnabled;
}

void ImgLabel::resetNewFram()
{
lastRect.setSize(QSize(0, 0));
}

void ImgLabel::setPenColor(const QColor &newColor)
{

}

void ImgLabel::setPenWidth(int newWidth)
{

}

void ImgLabel::mousePressEvent(QMouseEvent *event)
{
if (drawDisabled) {
return;
}

//	qDebug() << "mouse pressed:"<< event->pos().x() << "," << event->pos().y();
if ( event->button() == Qt::LeftButton) {
begPoint = event->pos();
isStarted = true;
isMoved = false;
emit sigLeftButtonPressed( begPoint );
}
}

void ImgLabel::mouseMoveEvent(QMouseEvent *event)
{
//qDebug() << event->pos().x() << "," << event->pos().y();
if (drawDisabled) {
return;
}

if ((event->buttons() & Qt::LeftButton) && isStarted) {
endPoint = event->pos();
isMoved = true;
drawRect();
}
}

void ImgLabel::mouseReleaseEvent(QMouseEvent *event)
{
//qDebug() << "mouse released:" << event->pos().x() << "," << event->pos().y();
if (drawDisabled) {
return;
}

if (event->button() == Qt::LeftButton && isStarted) {
endPoint = event->pos();
drawRect();
if ( isMoved ) {
emit sigRectSelected(lastRect);
}
isStarted = false;
}
}


void ImgLabel::drawRect()
{
//QPainter painter(&image);
int w = abs(begPoint.x() - endPoint.x());
int h = abs(begPoint.y() - endPoint.y());
if (w == 0 || h == 0) {
return;
}
int xo = min<int>(begPoint.x(), endPoint.x());  //-left
int yo = min<int>(begPoint.y(), endPoint.y());  //-top

QPixmap *p = (QPixmap *)this->pixmap();
if (p) {
QPainter painter(p);
//painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
painter.setRenderHint(QPainter::Antialiasing);
painter.setPen(QColor(0xff, 0xff, 0xff));

//erase the old one
if (lastRect.width() > 0) {
painter.drawRect(lastRect);  
}
//update <lastRect>
lastRect.setRect(xo, yo, w, h);

//draw it
painter.drawRect(lastRect);

update();
}
}

//void ImgLabel::paintEvent(QPaintEvent *event)
//{
//QPainter painter(this);
//QRect dirtyRect = event->rect();
//painter.drawImage(dirtyRect, image, dirtyRect);
//}

