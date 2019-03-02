/*
*-------------------------------------------------------------------
* SpectralFlux.h
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

#ifndef _SPECTRAL_FLUX_H_
#define _SPECTRAL_FLUX_H_

#include "libUtil/DataTypes.h"
#include "libUtil/VisLog.h"
#include "libUtil/RawFrm.h"

#include "libSwMat/swiMat.h"
#include "libSwMat/swdMat.h"
#include "libSwMat/swfMat.h"
#include "libSwMat/swMatUtilities.h"

namespace autocut {
enum NormType {
NT_NO_NORMAL = 0,	// 0 - none normal,
NT_PSD,				//Power spectral density normalisation
NT_PEAK				//Peak normalisation
};
static char *g_normTypeStr[] = {"NT_NO_NORMAL", "NT_PSD", "NT_PEAK"};

class  SpectralFlux {
public:
static void densNorm(const swfMat &sp1, swfMat &sp2) {
float32 s = swMatSumValue(sp1);
sp2 = sp1;
sp2 /= s;
}

static void peakNorm(const swfMat &sp1, swfMat &sp2) {
float32 s = swMatMaxValue(sp1);
sp2 = sp1;
sp2 /= s;
}

public:
SpectralFlux( uint32 n=1024 ): m_normFlag(NT_PSD), m_numEle(n), m_preSp(1,n,0), m_curSp(1,n,0){}

void setCurSp(const swfMat &sp) {
m_preSp = m_curSp;
if (m_normFlag == NT_NO_NORMAL) {
m_curSp = sp;
}
else if (m_normFlag == NT_PSD) {
SpectralFlux::densNorm(sp, m_curSp);
}
else if (m_normFlag == NT_PEAK) {
SpectralFlux::peakNorm(sp, m_curSp);
}
}

float32 calSF()
{
swfMat dp = m_curSp - m_preSp;
swfMat tmp = dp * dp;
float sf = swMatSumValue(tmp);
return sf;
}

std::string getNormFlag() {
return std::string(g_normTypeStr[m_normFlag]);
}

public:
NormType m_normFlag; 
uint32   m_numEle;   //# of ele in (m_curSp> and <m_preSp>
swfMat   m_curSp;
swfMat   m_preSp;
};

typedef std::tr1::shared_ptr<SpectralFlux> SpectralFluxPtr;
}
#endif
