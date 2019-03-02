/*
*-------------------------------------------------------------------
* LocateHighlightFrms.cpp
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
#include "LocateHighlightFrms.h"

using namespace std;
using namespace autocut;
LocateHighlightFrms::LocateHighlightFrms( const int threadId )
: m_cfg(0)
, m_threadId(threadId)
, m_audio4Fft(FrmType_A_FRM, 0, 4096)
{
}

LocateHighlightFrms :: ~LocateHighlightFrms()
{
}

int LocateHighlightFrms::process(const std::vector<RawFrmPtr> &vAudioFrms, const std::string &originalVideoFilePath /*used for cute dump*/)
{
boost::posix_time::ptime tBeg = POSIX_LOCAL_TIME;

string head, tail, ext;
autocut::split(originalVideoFilePath, head, tail);
autocut::splitExt(tail, head, ext);

preppareRawAudio4Stfft(vAudioFrms);

string ff ="";
if (m_cfg->m_log.isDumpSelectedRawAudio) {
ff = m_cfg->m_log.logFolder + "/" + head + "_audio.raw";
dumpRaw(ff.c_str(), m_audio4Fft.buf_, m_audio4Fft.sz_);
raw2wav(ff, false);
}

//compute applausing segments in raw timeseries
//if ff is empty no dump, other dump results in wav format for debug
doTimeFreqAna(ff);    

uint32 dt =  timeIntervalMillisec(tBeg);
return dt;
}


//input:  m_demux->m_vAudioFrms
//ouptu:  m_audio4Fft -- all the frms are put together 
//put the raw audio frms into a consecutive memory holded by <m_audio4Fft>
//todo: this might not work for long video files
int LocateHighlightFrms::preppareRawAudio4Stfft(const std::vector<RawFrmPtr> &v)
{
uint32 n = v.size();
if (n < 1) {
return -1;
}

int sz0 = v[0]->sz_;
int totSz = n*sz0;

m_audio4Fft.resetSz(FrmType_A_FRM, totSz);
uint64 m = 0;
uint8 *p0 = m_audio4Fft.buf_;
BOOST_FOREACH(const RawFrmPtr &x, v) {
memcpy_s(p0, sz0, x->buf_, sz0 );
p0 += sz0;

if (sz0 != x->sz_) {
xLog("LocateHighlightFrms::preppareRawAudio4Stfft(): inconsists audio frm sz!");
}
}
return 0;
}

//cal audio ID range part in <m_vSelectedFrmRngs>
int LocateHighlightFrms::doTimeFreqAna(const std::string &dumpFilenameTag)
{
TimeFreqAna tfAna(m_cfg->m_tfAna);

tfAna.process(m_audio4Fft, m_inpAudioStreamInfo.sample_rate, m_cfg->m_wrtVideo.highlightSegmentLengthInSec, dumpFilenameTag);

const std::vector<HotSeg>  &vAudioSegs = tfAna.getSelectedAudioSegs();
if (m_cfg->m_log.isDumpSelectedRawAudio) {
const std::string &dumpFolder = m_cfg->m_log.logFolder;
BOOST_FOREACH(const HotSeg &seg, vAudioSegs) {
string s0 = timeSecToHHMMSSxx(m_inpAudioStreamInfo.getAudioTimeSec2(seg.idx1));
string s1 = timeSecToHHMMSSxx(m_inpAudioStreamInfo.getAudioTimeSec2(seg.idx2));
string_find_n_replace(s0, ':', '_');
string_find_n_replace(s1, ':', '_');
string ff = dumpFolder + "/" + dumpFilenameTag + "_AudioRawIdx_" + s0 + "-" + s1 + ".raw";
uint8 *p = m_audio4Fft.buf_ + sizeof(float32)*seg.idx1;
dumpRaw(ff.c_str(), p, (seg.idx2 - seg.idx1 +1) * sizeof(float32));
raw2wav(ff);
}
}

//convt time series index rang in <vAudioSegs> to audio frame range in mpeg video
m_vSelectedFrmRngs.clear();
uint64 nPts = m_audio4Fft.sz_ / sizeof(float32);   //sample pts of audio timeseries in float format
uint64 nTotFrms = floor(nPts / m_inpAudioStreamInfo.nb_samples_perfrm);  //audio frames, one frm has 1024 or other # of floating  sample points
BOOST_FOREACH(const HotSeg &seg, vAudioSegs) {
//r.aid2 -- applausing end 1d float32 vector timeseries index
uint64 idx1 = floor(seg.idx1 / m_inpAudioStreamInfo.nb_samples_perfrm);  //applausing start audio frm #
uint64 idx2 = ceil(seg.idx2  / m_inpAudioStreamInfo.nb_samples_perfrm);   //appluaisng end audio frm #
if (idx2 >= nTotFrms){
idx2 = nTotFrms-1;
}
FrmRange r(0, 0, idx1, idx2);
m_vSelectedFrmRngs.push_back(r);
}
return 0;
}
