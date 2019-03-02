/*
*-------------------------------------------------------------------
* avUtils.cpp
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
#include "avUtils.h"

using namespace std;

std::string autocut::my_av_err2str(int ret)
{
char buf[256];
av_strerror(ret, buf, sizeof(buf));
std::string s(buf);
return s;
}

std::string autocut::my_av_ts2str(const int64_t x)
{
char buf[64];
snprintf(buf, 64, "%lld", x);
std::string s(buf);
return s;
}

std::string autocut::my_av_ts2timestr(const int64_t ts, const AVRational *tb)
{
char buf[256];

if (ts == AV_NOPTS_VALUE) snprintf(buf, 256, "NOPTS");
else                      snprintf(buf, 256, "%.6g", av_q2d(*tb) * ts);

return std::string(buf);
}

void autocut::log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt)
{
AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;

printf("pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
my_av_ts2str(pkt->pts).c_str(), my_av_ts2timestr(pkt->pts, time_base).c_str(),
my_av_ts2str(pkt->dts).c_str(), my_av_ts2timestr(pkt->dts, time_base).c_str(),
my_av_ts2str(pkt->duration).c_str(), my_av_ts2timestr(pkt->duration, time_base).c_str(),
pkt->stream_index);
}

void autocut::dumpRaw(const char *f, const uint8 *buf, const uint64 sz)
{
FILE *fid = fopen( f, "wb");
if (!fid) {
xLog("Could not open destination file %s", f);
return;
}

fwrite(buf, sz, 1, fid);
fclose(fid);
xLog("Note: Wrote a file:%s", f);
}

void autocut::raw2wav(const std::string &rawPath, const bool isDeletedRaw)
{
std::string out = rawPath + ".wav";
std::string cmd = "C:/pkg/ffmpeg/ffmpeg-3.2.4-win64-shared/bin/ffmpeg.exe -f f32le -ar 44.1k -ac 1 -i " + rawPath + " -y " + out;
system(cmd.c_str());
if (isDeletedRaw) {
std::string cmd = "del " + rawPath;
string_find_n_replace(cmd, '/', '\\');
system(cmd.c_str());
}
xLog("Note: create a file: %s", out.c_str());
}

void autocut::loadRaw(const char *f, std::vector<char> &buf)
{
std::ifstream ifs(f, std::ios::binary);
std::filebuf* pbuf = ifs.rdbuf();

// get file size using buffer's members
uint32_t sz = pbuf->pubseekoff(0, ifs.end, ifs.in);
pbuf->pubseekpos(0, ifs.in);

buf.resize(sz);
pbuf->sgetn( &buf[0], sz);

ifs.close();
}


uint32_t autocut::ppm_save(ppm_image *img, FILE *outfile) {
size_t n = 0;
n += fprintf(outfile, "P6\n# THIS IS A COMMENT\n%d %d\n%d\n",
img->width, img->height, 0xFF);
n += fwrite(img->data, 1, img->width * img->height * 3, outfile);
return n;
}

uint32_t autocut::yuv420p_save(const uint8 *yuv420pRaw, const int sz, const int w, const int h, const char *fname)
{

uint8* dst_rgb24 = new uint8[w*h * 3];

const uint8* src_y = yuv420pRaw;
const uint8* src_u = yuv420pRaw + (w*h);
const uint8* src_v = src_u + (w*h) / 4;

int sz1 = libyuv::I420ToRGB24(src_y, w, src_u, w/2, src_v, w/2, dst_rgb24, 3*w, w, h);

autocut::ppm_image x;
x.width = w;
x.height = h;
x.size = w * h * 3;
x.data = dst_rgb24;

FILE *fp = fopen(fname, "w");
sz1 = autocut::ppm_save(&x, fp);
fclose(fp);

delete[] dst_rgb24;

return sz1;
}

void autocut::dumpFrmImg( const RawFrm &x, const std::string &folder, const std::string &tag)
{
std::string f = folder + "/" + tag + "_vfn" + std::to_string( x.fn_ ) + ".png";
//yuv420p_save(x.buf_, x.sz_, x.w_, x.h_, f.c_str());

cv::Mat picYV12 = cv::Mat(x.h_ * 3 / 2, x.w_, CV_8UC1, x.buf_);
cv::Mat picBGR;
cv::cvtColor(picYV12, picBGR, CV_YUV420p2RGB);
cv::imwrite(f, picBGR);  //only for test
}

void autocut::resizeYuvVideoFrm(RawFrm &dst, const uint8 *srcBuf, const int srcW, const int srcH,  const enum libyuv::FilterMode flag)
{
const int dstW = dst.w_;
const int dstH = dst.h_;
#if _DEBUG
assert(dstW % 2 == 0);
assert(dstH % 2 == 0);
#endif

uint8* dst_y = dst.buf_;
uint8* dst_u = dst_y + dstW*dstH;
uint8* dst_v = dst_u + (dstW*dstH) / 4;
int dst_stride_y = dstW;
int dst_stride_u = dstW / 2;
int dst_stride_v = dstW / 2;

const uint8* src_y = srcBuf;
const uint8* src_u = src_y + srcW*srcH;
const uint8* src_v = src_u + (srcW*srcH) / 4;
int src_stride_y = srcW;
int src_stride_v = srcW / 2;
int src_stride_u = srcW / 2;

int tmp = libyuv::I420Scale(src_y, src_stride_y, src_u, src_stride_u, src_v, src_stride_v, srcW, srcH,
dst_y, dst_stride_y, dst_u, dst_stride_u, dst_v, dst_stride_v, dstW, dstH, flag);
#if 0
{
std::string f = "c:/temp/yuv-fn";
f += std::to_string(i) + ".ppm";
yuv420p_save(I1.buf_, I1.sz_, desW, desH, f.c_str());
}
#endif
}

void autocut::resizeYuvVideoFrm(RawFrm &dst, const RawFrm &src, const enum libyuv::FilterMode flag)
{
const int dstW = dst.w_;
const int dstH = dst.h_;
#if _DEBUG
assert(dstW % 2 == 0);
assert(dstH % 2 == 0);
#endif

uint8* dst_y = dst.buf_;
uint8* dst_u = dst_y + dstW*dstH;
uint8* dst_v = dst_u + (dstW*dstH) / 4;
int dst_stride_y = dstW;
int dst_stride_u = dstW / 2;
int dst_stride_v = dstW / 2;

const int srcW = src.w_;
const int srcH = src.h_;
const uint8* src_y = src.buf_;
const uint8* src_u = src_y + srcW*srcH;
const uint8* src_v = src_u + (srcW*srcH) / 4;
int src_stride_y = srcW;
int src_stride_v = srcW / 2;
int src_stride_u = srcW / 2;

int tmp = libyuv::I420Scale(src_y, src_stride_y, src_u, src_stride_u, src_v, src_stride_v, srcW, srcH,
dst_y, dst_stride_y, dst_u, dst_stride_u, dst_v, dst_stride_v, dstW, dstH, flag);
#if 0
{
std::string f = "c:/temp/yuv-fn";
f += std::to_string(i) + ".ppm";
yuv420p_save(I1.buf_, I1.sz_, desW, desH, f.c_str());
}
#endif
}

void autocut::resizeYuvVideoFrms(const int srcW, const int srcH, const std::vector<RawFrm> &vSrc, const int desW, const int desH, std::vector<RawFrm> &vDes)
{

int src_width = srcW;
int src_height = srcH;
int src_stride_y = srcW;
int src_stride_v = srcW / 2;
int src_stride_u = srcW / 2;

int dst_width = desW;
int dst_height = desH;
int dst_stride_y = desW;
int dst_stride_u = desW / 2;
int dst_stride_v = desW / 2;
int dsSz = (desW*desH * 3) / 2;

RawFrm I1(FrmType_V_FRM, 0, dsSz);
uint8* dst_y = I1.buf_;
uint8* dst_u = dst_y + desW*desH;
uint8* dst_v = dst_u + (desW*desH) / 4;

size_t n = vSrc.size();
for (int i = 0; i < n; ++i) {
const RawFrm &I0 = vSrc[i];
const uint8* src_y = I0.buf_;
const uint8* src_u = src_y + srcW*srcH;
const uint8* src_v = src_u + (srcW*srcH) / 4;

int tmp = libyuv::I420Scale(src_y, src_stride_y, src_u, src_stride_u, src_v, src_stride_v, src_width, src_height,
dst_y, dst_stride_y, dst_u, dst_stride_u, dst_v, dst_stride_v, dst_width, dst_height, libyuv::kFilterBilinear);

#if 0
{
std::string f = "c:/temp/yuv-fn";
f += std::to_string(i) + ".ppm";
yuv420p_save(I1.buf_, I1.sz_, desW, desH, f.c_str());
}
#endif
I1.fn_ = I0.fn_;
vDes.push_back(I1);
}
}

void autocut::resizeYuvVideoFrms(const int srcW, const int srcH, const std::vector<RawFrmPtr> &vSrc, const int desW, const int desH, std::vector<RawFrmPtr> &vDes)
{

int src_width = srcW;
int src_height = srcH;
int src_stride_y = srcW;
int src_stride_v = srcW / 2;
int src_stride_u = srcW / 2;

int dst_width = desW;
int dst_height = desH;
int dst_stride_y = desW;
int dst_stride_u = desW / 2;
int dst_stride_v = desW / 2;
int dsSz = (desW*desH * 3) / 2;

size_t n = vSrc.size();
for (int i = 0; i < n; ++i) {
const RawFrmPtr &I0 = vSrc[i];
const uint8* src_y = I0->buf_;
const uint8* src_u = src_y + srcW*srcH;
const uint8* src_v = src_u + (srcW*srcH) / 4;

RawFrmPtr I1(new RawFrm(FrmType_V_FRM, 0, dsSz));
uint8* dst_y = I1->buf_;
uint8* dst_u = dst_y + desW*desH;
uint8* dst_v = dst_u + (desW*desH) / 4;
I1->fn_ = I0->fn_;

int tmp = libyuv::I420Scale(src_y, src_stride_y, src_u, src_stride_u, src_v, src_stride_v, src_width, src_height,
dst_y, dst_stride_y, dst_u, dst_stride_u, dst_v, dst_stride_v, dst_width, dst_height, libyuv::kFilterBilinear);

#if 0
{
std::string f = "c:/temp/yuv-fn";
f += std::to_string(i) + ".ppm";
yuv420p_save(I1.buf_, I1.sz_, desW, desH, f.c_str());
}
#endif
vDes.push_back(I1);
}
}

bool autocut::saveAudioStreamInfo(const std::string &filePath, const MyAudioStreamInfo &info)
{
std::ofstream myFile(filePath, std::ios::out | std::ios::binary);
if (!myFile.is_open()) {
xLog("ProjProfile::saveVideoStreamInfo(): cannot open file: %s", filePath.c_str());
return false;
}
myFile.write((char*)&info, sizeof(MyAudioStreamInfo));
myFile.close();
return true;
}

bool autocut::saveVideoStreamInfo(const std::string &filePath, const MyVideoStreamInfo &info)
{
std::ofstream myFile(filePath, std::ios::out | std::ios::binary);
if (!myFile.is_open()) {
xLog("ProjProfile::saveVideoStreamInfo(): cannot open file: %s", filePath.c_str());
return false;
}
myFile.write((char*)&info, sizeof(MyVideoStreamInfo));
myFile.close();
return true;
}

//static funcs
bool autocut::loadVideoStreamInfo(const std::string &fpath, MyVideoStreamInfo &info)
{
ifstream myFile(fpath, ios::in | ios::binary);
if (!myFile.is_open()) {
xLog("ProjProfile::loadVideoStreamInfo(): cannot open file: %s", fpath.c_str());
return false;
}
myFile.read((char*)&info, sizeof(MyVideoStreamInfo));
myFile.close();
return true;
}

bool autocut::loadAudioStreamInfo(const std::string &fpath, MyAudioStreamInfo &info)
{
ifstream myFile(fpath, ios::in | ios::binary);
if (!myFile.is_open()) {
xLog("ProjProfile::loadAudioStreamInfo(): cannot open file: %s", fpath.c_str());
return false;
}
myFile.read((char*)&info, sizeof(MyAudioStreamInfo));
myFile.close();
return true;
}
