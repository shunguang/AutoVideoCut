/*
*-------------------------------------------------------------------
* ImgSample.h
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

#ifndef _IMG_SAMPLE_H_
#define _IMG_SAMPLE_H_

#include "DcDefs.h"
namespace autocut {
class DC_EXPORT ImgSample{
public:
ImgSample();
ImgSample( const uint32 w0, const uint32 h0, const uint32 depth=8, const int nchannels=3 );
~ImgSample();

void setBgrImg( const cv::Mat &rawFrm, const uint32 fn=0 );
void cvtBgr2GrayImg();

bool updateSize(const int w, const int h);

void dump(const std::string &path, const uint32 fn=0 ) const;

private:
void createImgs();

public:
uint32		m_fn;
uint32		m_depth;			//depth captured
uint32		m_nChannels;		//original input image channel
uint32		m_w0, m_h0;			//level 0 input image size

cv::Mat		m_bgrImg;			//level 0 BGR img
cv::Mat		m_grayImg;			//level 0 gray image

};
typedef std::shared_ptr<ImgSample>		ImgSamplePtr;
}   //end namespace util

#endif		
