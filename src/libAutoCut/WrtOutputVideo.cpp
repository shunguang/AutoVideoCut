/*
*-------------------------------------------------------------------
* WrtOutputVideo.cpp
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

#include "WrtOutputVideo.h"

using namespace std;
using namespace autocut;

WrtOutputVideo::WrtOutputVideo()
: m_cfg(0)
, m_mp3FrmId(-1)
, m_mux( new MuxingEncoding2() )
, m_clip(new RawClip2())
, m_imgRender(0)
{
}

WrtOutputVideo :: ~WrtOutputVideo()
{
endProcess();
}

void WrtOutputVideo::respReset()
{
endProcess();
}

void WrtOutputVideo ::setCfg( CfgAcPtr &cfg ) 
{
m_cfg = cfg;
}

void WrtOutputVideo::setOutputAudioInfo(const MyAudioStreamInfo &info)
{
m_outpAudioStreamInfo = info;
}

void WrtOutputVideo::setOutputVideoInfo(const MyVideoStreamInfo &info)
{
m_outpVideoStreamInfo = info;
m_outputVideoW = info.w;
m_outputVideoH = info.h;
}

void  WrtOutputVideo::endProcess()
{
if (m_mux->isOpened()) {
m_mux->closeOpenedFile();
}
}

uint32 WrtOutputVideo::initProcess()
{

boost::posix_time::ptime t0 = POSIX_LOCAL_TIME;
const CfgGame &game = m_cfg->m_vGames[0];

string outVideoFilePath = createOutputVideoFilePath(game);
m_mux->openOutputFile(outVideoFilePath.c_str(), m_outpVideoStreamInfo, m_outpAudioStreamInfo);

m_imgRender.reset(new ImgRender(m_outputVideoW, m_outputVideoH, m_cfg->m_imgRender));

uint32 dt = timeIntervalMillisec(t0);
return dt;
}

uint32 WrtOutputVideo::wrtHeaderTeamGameInfo()
{
boost::posix_time::ptime t0 = POSIX_LOCAL_TIME;

if (m_cfg->isWrtHeader()) {
wrtHeader();
}

if (m_cfg->isWrtTeamInfo()) {
wrtTeamInfo();
}

const CfgGame &game = m_cfg->m_vGames[0];
wrtGameInfo(game);

uint32 dt = timeIntervalMillisec(t0);
return dt;
}

uint32  WrtOutputVideo::wrtEventTimeFrms(float timeOfEventInSec)
{
boost::posix_time::ptime t0 = POSIX_LOCAL_TIME;
//-----------------------------
// write timestampe clip at the selcted moment
//-----------------------------
string timeStr = timeSecToHHMMSSxx(timeOfEventInSec);
cv::Mat I0 = m_imgRender->getImg(timeStr);
renderClipFromImage(I0, m_clip);
m_mux->wrtSomeFrms(m_clip->m_vVideoFrms, m_clip->m_vAudioFrms, 2);

uint32 dt = timeIntervalMillisec(t0);
return dt;
}

uint32  WrtOutputVideo::wrtFrms( const std::vector<RawFrmPtr> &vVideoFrms, const std::vector<RawFrmPtr> &vAudioFrms_1ch_FLT)
{
boost::posix_time::ptime t0 = POSIX_LOCAL_TIME;
//-----------------------------
// write selected video
//-----------------------------
//read one clip from input video
uint32  nAudioFrms = vAudioFrms_1ch_FLT.size();
if (nAudioFrms < 1) {
xLog(" WrtOutputVideo::writeOutputVideo(): no audio data in <vAudioFrms>");
return timeIntervalMillisec(t0);
}

//mix with mp3
std::vector<RawFrmPtr> vMixedAudioFrms_2ch_FLTP;
mixAudioWithMp3(vAudioFrms_1ch_FLT, vMixedAudioFrms_2ch_FLTP);
m_mux->wrtSomeFrms(vVideoFrms, vMixedAudioFrms_2ch_FLTP, 2);

uint32 dt = timeIntervalMillisec(t0);
return dt;
}


void WrtOutputVideo::wrtHeader()
{
boost::filesystem::path p0(m_cfg->m_wrtVideo.headerVidoeFilePath);
if (!boost::filesystem::exists(p0)) {
return;
}

uint32 nMaxFrms = 100;
int ret = prepareVideoClip(m_clip, m_cfg->m_wrtVideo.headerVidoeFilePath, nMaxFrms);
if (ret == 0) {
m_mux->wrtSomeFrms(m_clip->m_vVideoFrms, m_clip->m_vAudioFrms, 2);
}
}

void WrtOutputVideo::wrtTeamInfo()
{
//render team infor images
std::vector<cv::Mat>		vTeamInfoImg;
m_imgRender->getTeamImgs(vTeamInfoImg, m_cfg->m_team, m_cfg->m_wrtVideo.isRenderTeamInfo);

bool suc;
//write team Image info video
BOOST_FOREACH(const cv::Mat &I, vTeamInfoImg) {
suc = renderClipFromImage(I, m_clip);
if (suc) {
m_mux->wrtSomeFrms(m_clip->m_vVideoFrms, m_clip->m_vAudioFrms, 2);
}
}
}

void WrtOutputVideo::wrtGameInfo(const CfgGame &game)
{
std::vector<cv::Mat>		vTeamInfoImg;
m_imgRender->getTeamImgs(vTeamInfoImg, m_cfg->m_team, m_cfg->m_wrtVideo.isRenderTeamInfo);

//write game info video
cv::Mat I = m_imgRender->getGameImg(game);
bool suc = renderClipFromImage(I, m_clip);
if (suc) {
m_mux->wrtSomeFrms(m_clip->m_vVideoFrms, m_clip->m_vAudioFrms, 2);
}
}


std::string  WrtOutputVideo::createOutputVideoFilePath(const CfgGame &game)
{
std::string outVideoFilePath("");
if ( game.vInputVideoFileNames.size() < 1) {
return outVideoFilePath;
}

outVideoFilePath = m_cfg->m_wrtVideo.outputVideoFolder;
uint32 n2 = outVideoFilePath.size();
if (outVideoFilePath[n2 - 1] != '/') {
outVideoFilePath += "/";
}


string s = game.date;  //'08/01/2018'
string_trim(s, ' ');
string_find_n_replace(s, '/', '_');  //'08_01_1028'
string yyyy = s.substr(6);
string mmdd = s.substr(0, 5);

string sz = "_frmSz" + std::to_string(m_outputVideoW)+ "x" + std::to_string(m_outputVideoH);
string segLen = num_to_string(m_cfg->m_wrtVideo.highlightSegmentLengthInSec, "seg");
outVideoFilePath += yyyy + "_" + mmdd + "_" + game.name + "_" + segLen + "_" + game.score + sz + ".mp4";


return outVideoFilePath;
}


//I-BGR img
bool WrtOutputVideo::renderClipFromImage(const cv::Mat &I, RawClip2Ptr &clip) 
{
#if _DEBUG
assert( I.rows == m_outputVideoH );
assert( I.cols == m_outputVideoW );
#endif

cv::Mat yuvImg;
cv::cvtColor(I, yuvImg, CV_BGR2YUV_I420);

float dt = 1.0;
float fps = m_outpVideoStreamInfo.frame_rate.num/(float)m_outpVideoStreamInfo.frame_rate.den;
int nVideoFrms = dt * fps;
uint64 vSz = 3 * (m_outputVideoW* m_outputVideoH / 2);
RawFrmPtr vRaw( new RawFrm(FrmType_V_FRM, 0, vSz) );
vRaw->copyFrom(yuvImg.data, vSz);
clip->m_vVideoFrms.clear();
for (int i = 0; i < nVideoFrms; ++i) {
clip->m_vVideoFrms.push_back(vRaw);
}

float a_fps = m_outpAudioStreamInfo.sample_rate;
int nSmps = dt * a_fps;
int nAudioFrms = nSmps / m_outpAudioStreamInfo.nb_samples_perfrm;
clip->m_vAudioFrms.clear();
for (int i = 0; i < nAudioFrms; ++i) {
const RawFrmPtr &mp3Frm = getNextMp3Frm();
clip->m_vAudioFrms.push_back(mp3Frm);
}
return true;
}


uint32 WrtOutputVideo::decodeMp3(const std::string &mp3FilePath)
{
boost::posix_time::ptime t0 = POSIX_LOCAL_TIME;

AudioDecoding x;
int ret = -1;

if ( !mp3FilePath.empty() ) {
ret = x.decode_audio( mp3FilePath, m_outpAudioStreamInfo, m_vMp3Frms, m_mp3Params);
}
if (ret < 0) {  //mp3 fiel does not exist
//fack a silence audio frm
uint64 aSz = m_outpAudioStreamInfo.getAudioRawFrmSizeInBytes();
RawFrmPtr aRaw( new RawFrm(FrmType_A_FRM, 0, aSz) );
aRaw->setToZeros();
m_vMp3Frms.clear();
m_vMp3Frms.push_back(aRaw);
}
m_mp3FrmId = 0;   //music restart for each game

uint32 dt = timeIntervalMillisec(t0);

return dt;
}

uint32	WrtOutputVideo :: setMp3Frms(const std::vector<RawFrmPtr> &vMp3Frms)
{
m_vMp3Frms.clear();
BOOST_FOREACH(const RawFrmPtr &frm, vMp3Frms ) {
m_vMp3Frms.push_back(frm);
}
return 0;
}

const RawFrmPtr& WrtOutputVideo::getNextMp3Frm()
{
m_mp3FrmId++;
if (m_mp3FrmId >= m_vMp3Frms.size()) {
m_mp3FrmId = 0;
}
return m_vMp3Frms[m_mp3FrmId];
}

void WrtOutputVideo::mixAudioWithMp3(const std::vector<RawFrmPtr> &vAudioFromVideoFile_1ch_FLT, std::vector<RawFrmPtr> &vMixedAudioFrms_2ch_FLTP )
{
vMixedAudioFrms_2ch_FLTP.clear();
uint32 inFrms = vAudioFromVideoFile_1ch_FLT.size();
if ( inFrms == 0) {
return;
}

uint32 inFrmSz =  vAudioFromVideoFile_1ch_FLT[0]->sz_;
uint32 inTotSz = inFrms * inFrmSz;
uint32 outFrmSz_1ch = m_outpAudioStreamInfo.nb_samples_perfrm * av_get_bytes_per_sample(m_outpAudioStreamInfo.sample_fmt);
uint32 outFrms = inTotSz / outFrmSz_1ch;

//--initial <vMixedAudioFrms_2ch_FLTP> from <vAudioFromVideoFile_1ch_FLT>,  multi-channel output RawFrms from one channel input
vector<uint8_t> vRaw(inTotSz);
uint8_t *des = vRaw.data();
uint32 nBytesCopied = 0;
BOOST_FOREACH(const RawFrmPtr &frm, vAudioFromVideoFile_1ch_FLT) {
if (nBytesCopied + inFrmSz <= inTotSz) {
memcpy_s(des, inFrmSz, frm->buf_, inFrmSz );
des += inFrmSz;
nBytesCopied += inFrmSz;
}
}

int  nChs = m_outpAudioStreamInfo.channels;
uint32 outFrmSz_all_Chs = m_outpAudioStreamInfo.getAudioRawFrmSizeInBytes();
uint8_t *src = vRaw.data();
for (int i = 0; i < outFrms; ++i) {
RawFrmPtr outFrm( new RawFrm(FrmType_A_FRM, 0, outFrmSz_all_Chs) );
uint8_t *d = outFrm->buf_;
for (int j = 0; j < nChs; ++j) {
memcpy_s(d, outFrmSz_1ch, src, outFrmSz_1ch);
d += outFrmSz_1ch;
}
vMixedAudioFrms_2ch_FLTP.push_back(outFrm);
src += outFrmSz_1ch;
}

if ( fabs(m_cfg->m_wrtVideo.originalAudioIntensity-1) < 1e-5 ) {
return;
}

assert( m_vMp3Frms.size() >= 1 );
assert( m_vMp3Frms[0]->sz_ == outFrmSz_all_Chs);

float s1 = m_cfg->m_wrtVideo.originalAudioIntensity;
if (s1 < 0) {s1 = 0; }
else if (s1 > 1) {s1 = 1;}
float sMp3 = 1 - s1;
uint32 nFloatPts = m_outpAudioStreamInfo.nb_samples_perfrm * nChs;
BOOST_FOREACH(RawFrmPtr &mixedFrm, vMixedAudioFrms_2ch_FLTP) {
const RawFrmPtr &mp3Frm = getNextMp3Frm();
float32 *p1 = (float32 *)mixedFrm->buf_;
const float32 *p2 = (float32 *)mp3Frm->buf_;
for (uint32 i = 0; i < nFloatPts; ++i, ++p1, ++p2) {
*p1 = s1 * (*p1) + sMp3 * (*p2);
}
}
}

#include "DemuxingDecoding2.h"
void WrtOutputVideo::readVideoFromFile(const char *filePath, const bool readVideo, const bool readAudio, int &vdieoFrmW, int &vdieoFrmH, 
std::vector<RawFrmPtr> &vFrm, std::vector<RawFrmPtr> &aFrm)
{
bool isRefCount = false;
CfgDemuxingDecodingPtr  cfg( new CfgDemuxingDecoding() );
cfg->demuxingVideo = 1;
cfg->dumpRawVideo = 0;
cfg->storeRawVideoInMemo = readVideo;

cfg->demuxingAudio = 1;
cfg->dumpRawAudio = 0;
cfg->storeRawAudioInMemo = readAudio;

DemuxingDecoding2 x(cfg);
x.openVideoFileToRead(filePath, isRefCount);
x.decodeAll(NULL, NULL);


AVPixelFormat  fmt = x.getRawVideoFrameFormat();
AVCodecContext ctx = x.getVideoCodecContext();

vdieoFrmW = ctx.width;
vdieoFrmH = ctx.height;

vFrm = x.m_vVideoFrms;
aFrm = x.m_vAudioFrms;

x.closeOpenedFiles();
}

int WrtOutputVideo::prepareVideoClip(RawClip2Ptr &clip, const std::string &videoFile, const uint32 nMaxFrms)
{
//char *insertVideoFile = "C:/Projects/2018/WrtOutputVideo/data/BetweenClips_AShortOceanVideo.mp4";

int insertVieoFrmW, insertVideoFrmH;
std::vector<RawFrmPtr> vInsertVideoFrms;
readVideoFromFile(videoFile.c_str(), true, true, insertVieoFrmW, insertVideoFrmH, vInsertVideoFrms, clip->m_vAudioFrms);

if (clip->m_vAudioFrms.size() > 0) {
uint32 n0 = av_get_bytes_per_sample((AVSampleFormat)m_outpAudioStreamInfo.sample_fmt);
uint32 n = m_outpAudioStreamInfo.nb_samples_perfrm * n0;
if (clip->m_vAudioFrms[0]->sz_ != n) {
BOOST_FOREACH(RawFrmPtr &r, clip->m_vAudioFrms) {
r->resetSz(FrmType_A_FRM, n);
memset(r->buf_, 0, n);
}
}
}

int n = (vInsertVideoFrms.size() > nMaxFrms) ? nMaxFrms : vInsertVideoFrms.size();
bool isResized = false;
if (m_outputVideoW != insertVieoFrmW || m_outputVideoH != insertVideoFrmH) {
clip->m_vVideoFrms.clear();
const cv::Size imgSz(m_outputVideoW, m_outputVideoH);
for (int i = 0; i < n; ++i) {
RawFrmPtr dst( new RawFrm(imgSz, i) );
resizeYuvVideoFrm( *dst.get(), *vInsertVideoFrms[i].get(), libyuv::kFilterBilinear);
clip->m_vVideoFrms.push_back(dst);
}
}
else {
clip->m_vVideoFrms = vInsertVideoFrms;
}
return 0;
}
