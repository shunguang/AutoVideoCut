/*
*-------------------------------------------------------------------
* FrmSample.cpp
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
#include "FrmSample.h"

using namespace std;
using namespace autocut;
using namespace cv;

FrmSample :: FrmSample( CfgAc *cfg)
: m_cfg				(cfg)
, m_fn				( 0 )
, m_users			( 0 )
, m_isImgSmp		( false )
, m_isDispSmp		( false )
, m_imgSample(1080, 720, 8,3)
{
}


std::string  FrmSample::getFrmNumStr() const
{
return num_to_string(m_fn, "fn=");
}

void FrmSample::updateFrmSize(const int newW_L0, const int newH_L0)
{
//change sizes of samples
m_imgSample.updateSize(newW_L0, newH_L0);

}

void FrmSample :: makeImgSample( const cv::Mat &rawFrm, const uint32 fn )
{
m_imgSample.setBgrImg(rawFrm, fn );
m_imgSample.cvtBgr2GrayImg();
m_isImgSmp = true;

m_imgSample.m_fn = fn;
m_fn = fn;
}


void FrmSample::close4FutherUsing()
{
m_isImgSmp = false;             //if <m_imgSample> is ready to use
m_isDispSmp = false;
}
