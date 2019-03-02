/*
*-------------------------------------------------------------------
* ImgSample.cpp
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
#include "ImgSample.h"

using namespace std;
using namespace autocut;
using namespace cv;

#if APP_USE_GPU_GEN
using namespace cv::gpu;
#endif
ImgSample :: ImgSample()
: m_w0			( 640 )
, m_h0			( 480 )
, m_depth		( 8 )
, m_nChannels	( 3 )
{
createImgs();
}

ImgSample :: ImgSample( const uint32 w0, const uint32 h0, const uint32 depth, const int nChannels )
: m_w0			( w0 )
, m_h0			( h0 )
, m_depth		( depth )
, m_nChannels	( nChannels )
{
createImgs();
}

ImgSample :: ~ImgSample()
{
}

void ImgSample :: createImgs()
{
if ( m_depth == 8 ){
#if APP_USE_GPU_GEN
//int tmp = gpu::getCudaEnabledDeviceCount();
//gpu::DeviceInfo info;
createContinuous( m_h0,m_w0,CV_8UC3, m_bgrImg );
createContinuous( m_h0,m_w0,CV_8UC1, m_grayImg );
#else
m_bgrImg.create(m_h0,m_w0,CV_8UC3);
m_grayImg.create(m_h0,m_w0,CV_8UC1);
#endif
}
else{
xLog( "ImgSample :: ImgSample(): depth cannot be handled" );
assert(0);
}
}

bool ImgSample::updateSize(const int w, const int h)
{
m_w0 = w;
m_h0 = h;
createImgs();
return true;
}

//deep copy rgb into $m_bgrImg$
void ImgSample :: setBgrImg(const cv::Mat &rawFrm, const uint32 fn )
{
m_fn = fn;
#if APP_USE_GPU_GEN
m_bgrImg.upload( x );
#else
rawFrm.copyTo( m_bgrImg );
#endif

}

void ImgSample :: cvtBgr2GrayImg()
{
cvtColor( m_bgrImg, m_grayImg, CV_BGR2GRAY);
}

void ImgSample :: dump(const std::string &path, const uint32 fn ) const
{
#if APP_USE_GPU_GEN
Mat bgrImg, grayImg;
m_bgrImg.download( bgrImg );
m_grayImg.download( grayImg );
#else
const Mat &bgrImg = m_bgrImg;
const Mat &grayImg = m_grayImg;
#endif

//myImWrite( grayImg, path, "frmImgGray", fn );
//myImgWrite( bgrImg, path, "frmImgBgr", fn );
}
