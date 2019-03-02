/*
*-------------------------------------------------------------------
* LocateHotSegs.h
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

#ifndef _LOCATE_HOT_SEGS_H_
#define _LOCATE_HOT_SEGS_H_

#include "libUtil/DataTypes.h"
#include "libUtil/UtilFuncs.h"

#include "libSwMat/swiMat.h"
#include "libSwMat/swdMat.h"
#include "libSwMat/swfMat.h"
#include "libSwMat/swMatUtilities.h"

#include "avUtils.h"

namespace autocut {

struct HotSeg {
HotSeg(const uint64 idx1_, const uint64 idx2_) : idx1(idx1_), idx2(idx2_) {}
//for acending sorting 
bool operator < (const HotSeg& x) const {
return (idx1 < x.idx1);
}

//for descending  sorting 
bool operator > (const HotSeg& x) const {
return (idx1 > x.idx1);
}

bool empty() const {
return (idx1 == idx2);
}
uint64 idx1;   //beg frm #
uint64 idx2;   //endFrm #
};

void hotSegVec_print(const std::vector<HotSeg> &v, const std::string &msg);
void hotSegVec_sort( std::vector<HotSeg> &v );

class  LocateApplausingSegs {
public:
LocateApplausingSegs( const int nMaxApplausingSegs );
~LocateApplausingSegs();
void process(const swfMat &cusum, const std::vector<uint64> &vFrmIdInRawAudio);

protected:
uint64 findIdBeg( const swfMat &x, const uint64 idEnd );
void swMatMinK(const swfMat &x, const int k, std::vector<float> &y, std::vector<uint64> &vIdx);

public:
std::vector<HotSeg> m_vHotSegStfft;      //hot segments in STFFT spectrum image
std::vector<HotSeg> m_vHotSegRawAudio;   //hot segments in raw audio

private:
int			m_nMaxApplausingSegs;                //maximum # of applausing segments want to select
};
typedef std::tr1::shared_ptr<LocateApplausingSegs> LocateHotSegsPtr;


class LocateHotSegsToCutVideo {
public:
LocateHotSegsToCutVideo(const float fs, const float dt4eachHotVideoSeg_sec, const int nMaxHotSeg4EachVideoToCut);
~LocateHotSegsToCutVideo();
void process( const std::vector<HotSeg> &vRawApplausingSegs, const bool isKeepApplausing,  const float32 *rawAudio, const uint64 nRawAudioSmpPts);
const std::vector<HotSeg>& getResults() const {
return m_vMergedSeg;
}
void saveHotSegsToWav(const float32 *rawAudio, const uint64 nRawAudioSmpPts, const std::string &otFileBase);
private:
void mergeApplausingSegs(const std::vector<HotSeg> &vInputSegs, const float dtThd_sec);
void computeScore(const float32 *rawAudio, const uint64 nRawAudioSmpPts, int &nNoneEmptySegs, swfMat &score);

private:
float32 m_fs;
float32 m_dt;
int		m_nMaxSegs;                //maximum # of segments want to select
std::vector<HotSeg> m_vMergedSeg;
};
typedef std::tr1::shared_ptr<LocateHotSegsToCutVideo> LocateHotSegsToCutVideoPtr;
}
#endif
