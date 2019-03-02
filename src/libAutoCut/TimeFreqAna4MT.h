/*
*-------------------------------------------------------------------
* TimeFreqAna4MT.h
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
#ifndef _TIME_FREQ_ANA_4MT_H_
#define _TIME_FREQ_ANA_4MT_H_

#include "libUtil/DataTypes.h"
#include "libUtil/utilFFT.h"
#include "libUtil/RawFrm.h"
#include "libUtil/SmoothingData.h"
#include "libUtil/CfgAc.h"

#include "libSwMat/swiMat.h"
#include "libSwMat/swdMat.h"
#include "libSwMat/swfMat.h"

#include "SpectralFlux.h"
#include "LocateHotSegs.h"

namespace autocut {
#define MAX_FFT_PLANS	10
FFT_PLAN	 m_vFftPlans[MAX_FFT_PLANS];         //FFT plans

class  TimeFreqAna4MT {
public:
TimeFreqAna4MT( const CfgTimeFreqAna &cfg, const int planId );
~TimeFreqAna4MT();

int process(const RawFrm &x, const float fs, const float hilightSegmentLengthInSec, const std::string &dumpFileBase4Debug ="");

const std::vector<HotSeg> &getSelectedAudioSegs() const {
return m_vSelectedAudioSegs;
}

protected:
int calStfftImg(const RawFrm &x);

uint64 calStfftFrms(const uint64 nBytes );
void setFFTInput( const float32 *x, const uint32 nPts /*# of pts in x*/);
void calSpFromFftOutput(float32 *Y, uint32 nSp);
int  calSpFlux_n_Energy();
float32 calFrmSpEnergy(const swfMat &sp );
int smoothSpFlux_n_Energy();
int calCuSum( const swfMat &sp, const float32 a,  swfMat &cuSum );
int locateApplausingSegments(const int nMaxApplausingSegs);
int findFinalAudioSegments(const RawFrm &x, const float fs, const float hilightSegmentLengthInSec, const std::string &dumpFileBase4Debug);

void fftSetToNull();
bool allocFFT();
void freeFFT();
void normalFFT(const FFT_COMPLEX *p, const uint32 nStart,  const uint32 nPts, const char flag);
void saveFft(const FFT_COMPLEX *fft, const uint32 n, const string &fnameMain, const string &tag1, const int32 num1);

void printFFT(const FFT_COMPLEX *p, const uint32 n, const int scale, const std::string &msg);
void printMaxEle(const FFT_COMPLEX *fft, const uint32 n, const char *msg);
private:
const int			 m_planId;
const CfgTimeFreqAna *m_cfg;

//float pionting FFT
FFT_COMPLEX* m_fftIn;           //FFT input data 
FFT_COMPLEX* m_fftOut;          //FFT output

std::vector<uint64>     m_vFrmIdInRawAudio;      //1D vector of corresponding frm idex,   m_frmId(i) <-->  m_stfftImg(i,:)
swdMat                  m_freq;            //1D vecor of normalizled freq (in [0,...,1] <---> [0, ..., fs/2]), m_freq(i)  <--> m_stfftImg(:,i)
swfMat					m_stfftImg;        //2D matrix, STFFT img for input data

swfMat					m_spFlux;            //1D vetcor store the spectrum flux of each frame;
swfMat					m_spEnergy;          //1D vetcor store the spectrum flux of each frame;
swfMat                  m_cuSumFlux;
swfMat                  m_cuSumEnergy;

std::vector<HotSeg>		m_vApplaussingSegs;        //the appausing segments from 1d audio
std::vector<HotSeg>     m_vSelectedAudioSegs;      //the final selected audio segments in 1d time series  audio

uint32              m_nFFT;     //# of pts to do FFT, m_nFFT=m_nSmpsInQueFrm + m_nXir; 
//In order to get circular convoltuion from FFT->IFFT approach,
//we need to padding <m_nXir> zeros

uint32				m_fftBufSzInBytes; //m_fftBufSzInBytes = 2*m_nFFT*sizeof(FFT_ELEMEMT)
};
typedef std::tr1::shared_ptr<TimeFreqAna4MT> TimeFreqAna4MT_Ptr;
}
#endif
