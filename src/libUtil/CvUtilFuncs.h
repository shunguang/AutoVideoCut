/*
*-------------------------------------------------------------------
* CvUtilFuncs.h
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

#ifndef _CV_UTIL_FUNCS_H_
#define _CV_UTIL_FUNCS_H_

#include "DataTypes.h"
#include "Roi.h"
#include "UtilDefs.h"

namespace autocut {
template<typename _Tp>
_Tp UTIL_EXPORT sumVal(const cv::Mat &x);
cv::Mat UTIL_EXPORT sumCols(const cv::Mat &x);
cv::Mat UTIL_EXPORT sumRows(const cv::Mat &x);

//for any type of given matrix $in$, convert it into a CV_8U type $out$ matrix
void UTIL_EXPORT convert_to_CV_8U(const cv::Mat &in, cv::Mat &out);
bool UTIL_EXPORT myCvInv(const cv::Mat &X, cv::Mat &invX, double &det);

bool UTIL_EXPORT getCvImgRowStep(const IplImage *img, int &rowStep, int &row0);
//put ROI image of $src$ into $des$ at $(desXo, desYo)$
void UTIL_EXPORT setCvImg(IplImage *des, const int &desXo, const int &desYo, const IplImage *src, const int &srcRoiXo, const int &srcRoiYo, const int &srcRoiW, const int &srcRoiH);


//void UTILS_EXPORT combineTwoImg_LR( const cv::Mat &imgLeft, const cv::Mat &imgRight, cv::Mat imgOut );

float UTIL_EXPORT maskNonZeroPercent(const cv::Mat &mask, const int &xo, const int &yo, const int &xb, const int &yb, size_t &n);
//void  UTILS_EXPORT cvMat2SwfMat( const cv::Mat &x, swfMat &y );
//void  UTILS_EXPORT swfMat2cvMat( const swfMat &x, cv::Mat &y  );

//dL>0: increase the size of $in$ by 2^dL 
//dL<0: decrease the size of $in$ by 2^dL 
void UTIL_EXPORT rectPyrLevelUpDown(const cv::Rect &in, const int dL, cv::Rect &out);

void UTIL_EXPORT myImgWrite(const cv::Mat &x, const std::string &dir, const std::string &fname, const size_t fn, const size_t L = 0);
void UTIL_EXPORT myImgWrite(const IplImage* cvTrkImg, const std::string &myPath, const std::string &fileNameWoExt, const size_t &fn);
void UTIL_EXPORT dumpCvImgToTextFile(const IplImage *J, const std::string &fileName);

void UTIL_EXPORT drawPlus(cv::Mat &img, const cv::Point2f &pt, const uint32 len, const cv::Scalar& color, int thickness, int lineType, int shift);
void UTIL_EXPORT drawPlus(cv::Mat &img, const cv::Point &pt, const uint32 len, const cv::Scalar& color, int thickness, int lineType, int shift);
void UTIL_EXPORT wrtieStrings(cv::Mat& img, const std::vector<std::string>& vStrs, cv::Point &org, int fontFace, double fontScale, cv::Scalar color, int thickness, int lineType);

}

#endif
