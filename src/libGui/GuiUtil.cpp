/*
*-------------------------------------------------------------------
* GuiUtil.cpp
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
#include "GuiUtil.h"

using namespace std;
using namespace cv;
using namespace autocut;

QString autocut::encodeProgressMsg(const ProgressMsg &msg)
{
char buf[1024];
snprintf(buf, 1024, "fileId=%d;fileName=%s;taskId=%d;percent=%d;dt=%d", msg.fileIdx, msg.fileName.c_str(), msg.taskId, msg.percent, msg.usedTimeSec);
QString s = QString::fromLocal8Bit(buf);
return s;
}

ProgressMsg autocut::decodeProgressMsg(const QString &q)
{
ProgressMsg msg;
std::string s = q.toStdString();
std::vector<std::string> v;
uint32	n = string_to_vector2(v, s, ";", false);

s = vstr_find_value(v, "fileId");
msg.fileIdx = std::stoi(s);
msg.fileName = vstr_find_value(v, "fileName");

s = vstr_find_value(v, "taskId");
msg.taskId = std::stoi(s);

s = vstr_find_value(v, "percent");
msg.percent = std::stoi(s);

s = vstr_find_value(v, "dt");
msg.usedTimeSec = std::stoi(s);

return msg;
}

QImage  autocut::cvMatToQImage(const cv::Mat &inMat)
{
switch (inMat.type())
{
// 8-bit, 4 channel
case CV_8UC4:
{
QImage image(inMat.data,
inMat.cols, inMat.rows,
static_cast<int>(inMat.step),
QImage::Format_ARGB32);

return image;
}

// 8-bit, 3 channel
case CV_8UC3:
{
QImage image(inMat.data, inMat.cols, inMat.rows, static_cast<int>(inMat.step), QImage::Format_RGB888);
return image.rgbSwapped();
//image.rgbSwapped(): Returns a QImage in which the values of the red and blue components of all pixels have been swapped, 
// effectively converting an RGB image to an BGR image
}

// 8-bit, 1 channel
case CV_8UC1:
{
QImage image(inMat.cols, inMat.rows, QImage::Format_Indexed8);
// Set the color table (used to translate colour indexes to qRgb values)  
image.setColorCount(256);
for (int i = 0; i < 256; i++)
{
image.setColor(i, qRgb(i, i, i));
}
// Copy input Mat  
uchar *pSrc = inMat.data;
for (int row = 0; row < inMat.rows; row++)
{
uchar *pDest = image.scanLine(row);
memcpy(pDest, pSrc, inMat.cols);
pSrc += inMat.step;
}

return image;
}

default:
qWarning() << "ASM::cvMatToQImage() - cv::Mat image type not handled in switch:" << inMat.type();
break;
}

return QImage();
}


QPixmap autocut::cvMatToQPixmap(const cv::Mat &inMat, bool isBGR)
{
if (isBGR) {
return QPixmap::fromImage(cvMatToQImage(inMat));
}
else {
//cv::Mat rgb;
//cv::cvtColor( inMat, rgb, cv::COLOR_BGR2RGB);
return QPixmap::fromImage( QImage(inMat.data, inMat.cols, inMat.rows, static_cast<int>(inMat.step), QImage::Format_RGB888) );
}
}


cv::Mat autocut::QImage2cvMat(const QImage &image)
{
cv::Mat mat;
qDebug() << image.format();
switch (image.format())
{
case QImage::Format_ARGB32:
case QImage::Format_RGB32:
case QImage::Format_ARGB32_Premultiplied:
mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());
break;
case QImage::Format_RGB888:
mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());
cv::cvtColor(mat, mat, CV_BGR2RGB);
break;
case QImage::Format_Indexed8:
mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());
break;
}
return mat;
}

bool autocut::isVideoFile(const std::string &filePath)
{
std::string  head, ext;

splitExt(filePath, head, ext);

if (0 == ext.compare("AVI")) {
return true;
}
if (0 == ext.compare("MP4")) {
return true;
}

return false;
}

bool autocut::isImgeFile(const std::string &filePath)
{
std::string  head, ext;

splitExt(filePath, head, ext);

if (0 == ext.compare("BMP")) {
return true;
}
if (0 == ext.compare("JPG")) {
return true;
}
if (0 == ext.compare("PNG")) {
return true;
}
if (0 == ext.compare("GIF")) {
return true;
}
if (0 == ext.compare("JPEG")) {
return true;
}
if (0 == ext.compare("PBM")) {
return true;
}
if (0 == ext.compare("PGM")) {
return true;
}
if (0 == ext.compare("PPM")) {
return true;
}
if (0 == ext.compare("XBM")) {
return true;
}
if (0 == ext.compare("XPM")) {
return true;
}

return false;
}
