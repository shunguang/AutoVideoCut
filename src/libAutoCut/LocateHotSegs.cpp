/*
*-------------------------------------------------------------------
* LocateHotSegs.cpp
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

#include "LocateHotSegs.h"

using namespace std;
using namespace autocut;


LocateApplausingSegs::LocateApplausingSegs(const int nMaxApplausingSegs)
: m_nMaxApplausingSegs(nMaxApplausingSegs)
{
}

LocateApplausingSegs:: ~LocateApplausingSegs()
{
}


void LocateApplausingSegs::process( const swfMat &cusum, const std::vector<uint64> &vFrmIdInRawAudio)
{
uint64 n = cusum.size();
swfMat x(1, n);
x(0) = 0;
for (uint64 i = 1; i < n; ++i) {
x(i) = cusum(i) - cusum(i-1);
}

m_vHotSegStfft.clear();
m_vHotSegRawAudio.clear();

std::vector<uint64> vIdx;
std::vector<float32> vVal;

swMatMinK( x, m_nMaxApplausingSegs, vVal, vIdx );

uint32 k = vVal.size();
for (int i = 0; i < k; ++i) {
uint64 frmIdEnd = vIdx[i];
if (frmIdEnd < 1) {
continue;
}

uint64 frmIdBeg = findIdBeg(cusum, frmIdEnd);

HotSeg hs1(frmIdBeg, frmIdEnd);
HotSeg hs2(vFrmIdInRawAudio[frmIdBeg], vFrmIdInRawAudio[frmIdEnd]);

m_vHotSegStfft.push_back(hs1);
m_vHotSegRawAudio.push_back(hs2);
}
m_nMaxApplausingSegs = k;
}


uint64 LocateApplausingSegs::findIdBeg(const swfMat &x, const uint64 idEnd) 
{
assert(x(idEnd) < MY_EPS_FLT32 );
uint64 idBeg = idEnd - 1;
if (idBeg <= 1) {
idBeg = 1;
}

float32 thd = 0.001* x(idBeg);
for (uint64 i = idBeg; i > 0; --i) {
if (x(i) < x(i - 1) || fabs(x(i)) < thd) {
idBeg = i;
break;
}
}
return idBeg;
}

void LocateApplausingSegs::swMatMinK(const swfMat &x, const int k, std::vector<float> &y, std::vector<uint64> &vIdx)
{
int k2 = k;
uint64 n = x.size();
if ( n < k2 ) {
k2 = n;
}

swfMat z(x);

y.resize( k2 );
vIdx.resize(k2);

uint32 idx;
float32 val;
for (int j = 0; j < k; ++j) {
val = swMatMinValue( z, idx );

y[j] = val;
vIdx[j] = idx;
z(idx) = std::numeric_limits<float>::infinity();
}

}



//-------------- class LocateHotSegsToCutVideo  -----------------
LocateHotSegsToCutVideo::LocateHotSegsToCutVideo(const float fs, const float dt4eachHotVideoSeg_sec, const int nMaxHotSeg4EachVideoToCut)
:m_fs(fs)
,m_dt(dt4eachHotVideoSeg_sec)
,m_nMaxSegs(nMaxHotSeg4EachVideoToCut)
{
}

LocateHotSegsToCutVideo::~LocateHotSegsToCutVideo()
{
}

void LocateHotSegsToCutVideo::process(const std::vector<HotSeg> &vRawApplausingSegs, const bool isKeepApplausing, const float32 *rawAudio, const uint64 nRawAudioSmpPts)
{
float dtThd_sec = 0.5;
int nNoneEmptySegs;
swfMat score0, score1 ;
bool isDescending = true;
swiMat I;

mergeApplausingSegs(vRawApplausingSegs, dtThd_sec);   //<vRawApplausingSegs> --> <m_vMergedSeg>

//hotSegVec_print(m_vMergedSeg, "LocateHotSegsToCutVideo::process() m_vMergedSeg=");
computeScore(rawAudio, nRawAudioSmpPts, nNoneEmptySegs, score0);    //compute <score0>  in each <m_vMergedSeg>
swMatSort( score0, score1, I, isDescending);	   //sorting <score0> ---> <score1>, idex <I>

//score0.print("score0=");
//score1.print("score1=");
//I.print("I=");

//pick up <vCadidateSegs> base on <I> and extend segments by <backSteps>
int n = (m_nMaxSegs < nNoneEmptySegs) ? m_nMaxSegs : nNoneEmptySegs;
std::vector<HotSeg> vCadidateSegs;
int backSteps = m_fs * m_dt;
for (int i = 0; i < n; ++i) {
const HotSeg &item = m_vMergedSeg[I(i)];
if (item.empty()) {
continue;
}

//video seg start
uint64 idx1 = (item.idx1 > backSteps) ? (item.idx1 - backSteps) : 0;   //start cut video index
//video seg end
uint64 idx2 = item.idx1;
if (isKeepApplausing) {
idx2 = item.idx2;
}

vCadidateSegs.push_back(HotSeg(idx1, idx2));
//printf("i=%d, idx1=%d, idx2=%d, t1=%s,t2=%s\n", i, idx1, idx2, timeSecToHHMMSSxx(idx1/m_fs).c_str(), timeSecToHHMMSSxx(idx2/m_fs).c_str() );
}

//merge <vCadidateSegs> -->  <m_vMergedSeg>, which is the final results.
//hotSegVec_print(vCadidateSegs, "before merger vCadidateSegs=");
mergeApplausingSegs(vCadidateSegs, 1);

//filtering out the too small segments
n = m_vMergedSeg.size();
for (int i = 0; i < n; ++i) {
HotSeg &item = m_vMergedSeg[i];
if (item.empty()) {
continue;
}

if ( (item.idx2 - item.idx1) / m_fs < 1.0) {
//future operation will ignore it
item.idx1 = 0;
item.idx2 = 0;
}
}
hotSegVec_print(m_vMergedSeg, "LocateHotSegsToCutVideo::process(): final=");
}

void LocateHotSegsToCutVideo::mergeApplausingSegs(const std::vector<HotSeg> &v, const float dtThd_sec)
{
//accending sorting
m_vMergedSeg = v;
hotSegVec_sort(m_vMergedSeg);

//hotSegVec_print(m_vMergedSeg, "mergeApplausingSegs(): beg --- m_vMergedSeg=");

std::vector<HotSeg> &v2 = m_vMergedSeg; //<v2> from matlab
uint32 n = v2.size();                 //# of segs in original applausings
uint32 d0 = dtThd_sec*m_fs;           //sec->idx
uint32 idx0 = 0;                      //init idx to find <seg1>
while (true) {
//find 1st element to compare
const HotSeg *seg1 = NULL;
uint32 idx1 = 0;
for (uint32 i = idx0; i < n; ++i) {
if (v2[i].empty()) {
continue;
}
else {
seg1 = &v2[i];
idx1 = i;
break;
}
}
if (seg1 == NULL) {
break;
}

//find 2nd element
const HotSeg *seg2 = NULL;
uint32 idx2 = 0;
for (uint32 i = idx1 + 1; i < n; ++i) {
if (v2[i].empty()) {
continue;
}
else {
seg2 = &v2[i];
idx2 = i;
break;
}
}
if (seg2 == NULL) {
break;
}

bool flag0 = (seg2->idx1 <= seg1->idx2);
uint64 d = (seg2->idx1 > seg1->idx2) ? (seg2->idx1 - seg1->idx2) : (seg1->idx2 - seg2->idx1);
if ( flag0 || (d <= d0) ) {
//merger  <seg2> into <seg1>
v2[idx1].idx2 = v2[idx2].idx2;

//seg v2(idx2) empty
v2[idx2].idx1 = 0;
v2[idx2].idx2 = 0;
}
else {
idx0 = idx1 + 1;
}
}
//hotSegVec_print(v2, "mergeApplausingSegs(): end --- v2=");

}

void LocateHotSegsToCutVideo::computeScore(const float32 *rawAudio, const uint64 nRawAudioSmpPts, int &nNoneEmptySegs, swfMat &score)
{
int n = m_vMergedSeg.size();

score.resize(1, n, 0.0f);
nNoneEmptySegs = 0;
for (int i = 0; i < n; ++i) {
HotSeg &seg = m_vMergedSeg[i];
if (seg.empty()) {
continue;
}
if (seg.idx1 >= nRawAudioSmpPts) {
continue;
}
if (seg.idx2 >= nRawAudioSmpPts) {
seg.idx2 = nRawAudioSmpPts;
}

float s = 0;
const float32 *p = rawAudio + seg.idx1;
for (uint64 k = seg.idx1; k <= seg.idx2; ++k) {
s += fabs(*p++);
}
score(i) = s / (seg.idx2 - seg.idx1 + 1);
nNoneEmptySegs = nNoneEmptySegs + 1;
}
}

void LocateHotSegsToCutVideo::saveHotSegsToWav(const float32 *rawAudio, const uint64 nRawAudioSmpPts, const std::string &outFileBase)
{

int j = 0;
int n = m_vMergedSeg.size();
const std::vector<HotSeg> &vHotSegInRawAudio = m_vMergedSeg;
for (int i = 0; i < n; ++i) {
const HotSeg &hotSegRaw = vHotSegInRawAudio[i];
if (hotSegRaw.empty()) {
continue;
}

j = j + 1;
uint64 idx1 = hotSegRaw.idx1;
uint64 idx2 = hotSegRaw.idx2;
float dt = round(100 * (idx2 - idx1) / m_fs) / 100;
if (idx1 >= nRawAudioSmpPts || idx2 >= nRawAudioSmpPts) {
xLog("LocateHotSegsToCutVideo::saveHotSegsToWav() index out of range!");
continue;
}
string s1 = timeSecToHHMMSSxx( (double)idx1/(double)m_fs );
string s2 = timeSecToHHMMSSxx( (double)idx2/(double)m_fs );
string_find_n_replace(s1, ':', '_');
string_find_n_replace(s2, ':', '_');
string ff = outFileBase + "_hotSeg" + num_to_string(j) + "_" + s1 + "-" + s2 + "_dt" + num_to_string(dt) + "_cpp.raw";
uint8 *p = (uint8*)rawAudio + sizeof(float32)*idx1;
dumpRaw( ff.c_str(), p, (idx2 - idx1 + 1) * sizeof(float32));
raw2wav( ff, true );
}
}


//---------- gloabl funcs ------------------
void autocut::hotSegVec_print(const std::vector<HotSeg> &v, const std::string &msg)
{
printf("%s\n", msg.c_str());
for (uint32 i = 0; i < v.size(); ++i) {
printf("i=%d, idx1=%llu, idx2=%llu\n", i, v[i].idx1, v[i].idx2);
}

}

void autocut::hotSegVec_sort(std::vector<HotSeg> &v)
{
#if _DEBUG
hotSegVec_print(v, "before sorting:");
#endif

//accending sorting
std::sort(v.begin(), v.end());
#if _DEBUG
hotSegVec_print(v, "after sorting:");
#endif
}
