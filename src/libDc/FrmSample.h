/*
*-------------------------------------------------------------------
* FrmSample.h
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

#ifndef _FRM_SAMPLE_H_
#define _FRM_SAMPLE_H_

#include "DcDefs.h"
#include "ImgSample.h"
#include "DispSample.h"
namespace autocut {
class DC_EXPORT FrmSample{
public:
FrmSample( CfgAc *cfg );
void updateFrmSize(const int newProW_L0, const int newProH_L0);

void makeImgSample(const cv::Mat &rawFrm, const uint32 fn = 0);
std::string  getFrmNumStr() const;

void close4FutherUsing();
public:
CfgAc			*m_cfg;
bool			m_isImgSmp;             //if <m_imgSample> is ready to use
ImgSample		m_imgSample;            //fixed size for all frms, so we pre-allocate memory

bool			m_isDispSmp;             //if <m_imgSample> is ready to use
DispSample		m_dispSmp;
uint32			m_fn;
uint32			m_users;				//#todo: of threads so far for this frm sample, if user==0, it will be auto deleted
};
typedef std::shared_ptr<FrmSample>		FrmSamplePtr;
}   //end namespace util

#endif		
