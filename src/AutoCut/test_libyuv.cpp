/*
*-------------------------------------------------------------------
* test_libyuv.cpp
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
#include "libyuv/libyuv.h"
#include "libAutoCut/avUtils.h"
#include "libAutoCut/ImgRender.h"

using namespace std;
using namespace autocut;
using namespace libyuv;

int test_libyuv1(int argc, char **argv);
int test_libyuv2(int argc, char **argv);

int test_libyuv(int argc, char **argv)
{
test_libyuv1(argc, argv);
//test_libyuv2(argc, argv);

return 0;
}

int test_libyuv1(int argc, char **argv)
{
CfgRenderImg redernParam;
ImgRender x(1080, 768, redernParam);
cv::Mat I = x.getImg( "test str");
cv::imwrite("c:/temp/I1.jpg", I);


string cfgFile = "C:/Projects/AutoCut/src/AutoCut/res/cfgAutoCut.xml";
CfgAcPtr cfg(new CfgAc());
cfg->readFromFile(cfgFile.c_str());

std::vector<cv::Mat> vImgs;
x.getTeamImgs( vImgs, cfg->m_team, true );
int i = 0;
BOOST_FOREACH(const cv::Mat &I, vImgs) {
string ff = string("c:/temp/I") + std::to_string(i++) + ".jpg";
cv::imwrite(ff, I);
}

return 0;
}

int test_libyuv2(int argc, char **argv)
{
char *f0 = "C:/Projects/2018/AutoCut/data/yuv420_640x360.raw";
char *f1 = "C:/Projects/2018/AutoCut/data/yuv420_640x360.ppm";
vector<char> rawV;
loadRaw(f0, rawV);


int w = 640, h = 360;
int sz = ( w*h * 3) / 2;
assert(sz == rawV.size());

//dumpRaw(  "C:/Projects/2018/AutoCut/data/tmp_yuv420_640x360.raw", (uint8*)rawV.data(),  sz);

yuv420p_save((uint8*)rawV.data(), sz, w, h, f1 );

//----------- test I420Scale() ----------------
const uint8* src_y = (uint8*)rawV.data();
int src_stride_y = w;
const uint8* src_u = src_y + w*h;
int src_stride_u = w / 2;
const uint8* src_v = src_u + (w*h) / 4;
int src_stride_v = w / 2;
int src_width = 640;
int src_height = 360;

int w1 = 480; int h1 = 280;
int sz2 = (w1*h1 * 3) / 2;
uint8 *buf = new uint8[ sz2 ];

uint8* dst_y = buf;
int dst_stride_y = w1;

uint8* dst_u = dst_y + w1*h1;
int dst_stride_u = w1 / 2;

uint8* dst_v = dst_u + (w1*h1) / 4;
int dst_stride_v = w1 / 2;

int dst_width = w1;
int dst_height = h1;

int tmp = libyuv::I420Scale( src_y,src_stride_y, src_u, src_stride_u, src_v, src_stride_v, src_width, src_height,
dst_y, dst_stride_y, dst_u, dst_stride_u, dst_v, dst_stride_v, dst_width, dst_height, libyuv::kFilterBilinear);


char *f2 = "C:/Projects/2018/AutoCut/data/I480x280.ppm";
yuv420p_save( buf, sz2, w1, h1, f2);

delete[] buf;
return 0;
}
