/*
*-------------------------------------------------------------------
* DispSample.h
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
#ifndef _DISP_SAMPLE_H_
#define _DISP_SAMPLE_H_
#include "DcDefs.h"
#include "ImgSample.h"
namespace autocut {
class DC_EXPORT DispSample{
public:
DispSample();
~DispSample();
static  cv::Scalar getPen(const int trkId);

void makeSample(const ImgSample &x, const Params4MakeDispSmp &p);
void makeSample(const cv::Mat &img, const Params4MakeDispSmp &p);
void dump(const std::string &folderPath, const uint32 fn=0 ) const;
void hdCopyTo(DispSample &des );
private:
void drawProperty(const Params4MakeDispSmp &p );

public:
//the final image displayed in main window
uint32		m_fn;
cv::Mat		m_dispImg;  //m x n x k (k=1,3) 8-bit image, CV_8UC1 or CV_8UC3 type

public:
static cv::Scalar	penWhite;
static cv::Scalar	penRed;
static cv::Scalar	penBlue;
static cv::Scalar	penGreen;
static cv::Scalar	penYellow;
static cv::Scalar	penCyan;
static cv::Scalar	penMagenta;
static cv::Scalar	penSpring;
static cv::Scalar	penBlack;
};
typedef std::shared_ptr<DispSample>		DispSamplePtr;
}  

#endif		
