/*
*-------------------------------------------------------------------
* DispSample.cpp
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

#include "DispSample.h"

using namespace std;
using namespace cv;
using namespace autocut;

cv::Scalar	DispSample::penWhite = CV_RGB(255, 255, 255);
cv::Scalar	DispSample::penRed = CV_RGB(255, 0, 0);
cv::Scalar	DispSample::penBlue = CV_RGB(0, 0, 255);
cv::Scalar	DispSample::penGreen = CV_RGB(0, 255, 0);
cv::Scalar	DispSample::penYellow = CV_RGB(255, 255, 0);
cv::Scalar	DispSample::penCyan = CV_RGB(0, 255, 255);
cv::Scalar	DispSample::penMagenta = CV_RGB(255, 0, 255);
cv::Scalar	DispSample::penSpring = CV_RGB(102, 255, 51);
cv::Scalar	DispSample::penBlack = CV_RGB(0, 0, 0);

cv::Scalar DispSample::getPen(const int trkId)
{
int id = trkId % 9;
if (id == 0) {
return DispSample::penWhite;
}
else if (id == 1) {
return DispSample::penRed;
}
else if (id == 2) {
return DispSample::penBlue;
}
else if (id == 3) {
return DispSample::penGreen;
}
else if (id == 4) {
return DispSample::penYellow;
}
else if (id == 5) {
return DispSample::penCyan;
}
else if (id == 6) {
return DispSample::penMagenta;
}
else if (id == 7) {
return DispSample::penSpring;
}
else if (id == 8) {
return DispSample::penBlack;
}
else {
return  CV_RGB(125, 125, 125);
}
}

DispSample::DispSample()
:m_fn(0)
{

}
DispSample::~DispSample()
{

}

void DispSample::hdCopyTo(DispSample &des)
{
des.m_fn = m_fn;
m_dispImg.copyTo(des.m_dispImg);
}

void DispSample::makeSample(const cv::Mat &img, const Params4MakeDispSmp &p)
{
img.copyTo(m_dispImg);
drawProperty(p);
}


void DispSample::makeSample(const ImgSample &x, const Params4MakeDispSmp &p)
{
m_fn = x.m_fn;
x.m_bgrImg.copyTo( m_dispImg );
drawProperty( p );
}

void DispSample::dump(const std::string &folderPath, const uint32 fn) const
{
myImgWrite(m_dispImg, folderPath, "dispImg", fn, 0);
}

void DispSample::drawProperty(const Params4MakeDispSmp &p)
{
int thickness = 1;
int lineType = 8;
int shift = 0;
#if 0
for (auto it = p.vMaskRois.begin(); it != p.vMaskRois.end(); ++it) {
if ( !it->invalid()) {
cv::Rect rect = it->toCvRect();  //at level 0
if (p.pyrL > 0) {
rectPyrLevelUpDown( rect, -p.pyrL, rect);
}

rectangle(m_dispImg, rect, CV_RGB(102, 255, 51), thickness, lineType, shift);
}
}
#endif
string s = num_to_string(m_fn, "fn=") + ", " + num_to_string(p.pyrL, "L=") + ", " + p.txt;
Point pt(10, 10);
cv::putText(m_dispImg, s, pt, FONT_HERSHEY_SIMPLEX | CV_FONT_ITALIC, 0.4, CV_RGB(255, 255, 255), 1, 8);
}
