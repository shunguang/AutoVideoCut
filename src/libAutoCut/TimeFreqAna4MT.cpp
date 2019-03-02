/*
*-------------------------------------------------------------------
* TimeFreqAna4MT.cpp
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
#include "TimeFreqAna4MT.h"

using namespace std;
using namespace autocut;

#define CAL_SP_ENERGY  0
TimeFreqAna4MT::TimeFreqAna4MT(const CfgTimeFreqAna &cfg, const int planId )
: m_cfg( &cfg )
, m_planId(planId)
, m_fftIn(NULL)
, m_fftOut(NULL)
{
assert(planId >= 0 && planId < MAX_FFT_PLANS);
m_fftBufSzInBytes = 2 * m_cfg->nFFT * sizeof(FFT_ELEMEMT);
allocFFT();

//make sure image part is zet a zeros at very bginging, and no one touch it durinbg process
memset((void*)m_fftIn, 0, m_fftBufSzInBytes);  
}

TimeFreqAna4MT :: ~TimeFreqAna4MT()
{
freeFFT();
}

int TimeFreqAna4MT::process(const RawFrm &x, const float fs, const float hilightSegmentLengthInSec, const std::string &dumpFileBase4Debug)
{
calStfftImg(x);  // cal <m_stfftImg>

uint32 m = m_stfftImg.rows();
uint32 n = m_stfftImg.cols();
m_spFlux.resize(1, m, 0);
m_cuSumFlux.resize(1, m, 0);
#if CAL_SP_ENERGY
m_spEnergy.resize(1, m, 0);
m_cuSumEnergy.resize(1, m, 0);
#endif

calSpFlux_n_Energy();

smoothSpFlux_n_Energy();

float32 a = 1.5*swMatMeanValue(m_spFlux, m_spFlux.size());
calCuSum(m_spFlux, a, m_cuSumFlux);

#if CAL_SP_ENERGY
a = 1.5*swMatMeanValue(m_spFlux, m_spFlux.size());
calCuSum(m_spEnergy, a, m_cuSumEnergy );
#endif

//detect the raw applausing segments
float videoLengthSec	= x.getNumOfFloatSmpPts() / fs;
int nMaxApplausingSegs	= round(videoLengthSec / m_cfg->nSec4EachRawAppluasing);
locateApplausingSegments(nMaxApplausingSegs );

//hotSegVec_print(m_vApplaussingSegs, "after locateApplausingSegments(): ");

//prepare final cut segments in <m_vSelectedAudioSegs>
findFinalAudioSegments(x, fs, hilightSegmentLengthInSec, dumpFileBase4Debug);

return 0;
}

int TimeFreqAna4MT::smoothSpFlux_n_Energy()
{
int nWinSz = 11;
if (nWinSz % 2 == 0) {
nWinSz++;
}
SmoothingData x( STYPE_GAUSSIAN, nWinSz);

uint32 n = m_spFlux.size();
swfMat y(1, n);
#if CAL_SP_ENERGY
x.process( y.getBuf(), m_spEnergy.getBuf_const(), n);
m_spEnergy = y;
#endif

x.process( y.getBuf(), m_spFlux.getBuf_const(), n);
m_spFlux = y;
return 0;
}

int TimeFreqAna4MT::calCuSum( const swfMat &sp, const float32 a, swfMat &cuSum )
{
uint32 n = sp.size();
swfMat y = sp - a;

cuSum = 0;
const float32 *p = y.getBuf_const();
++p;
for (uint32 k = 1; k < n; ++k) {
if ( *p > 0) {
cuSum(k) = cuSum(k - 1) + *p;
}

++p;
}

return 0;
}

//fill <m_vApplaussingSegs>
int TimeFreqAna4MT::locateApplausingSegments( const int nMaxApplausingSegs )
{
LocateApplausingSegs x( nMaxApplausingSegs );
x.process( m_cuSumFlux, m_vFrmIdInRawAudio);


std::vector<HotSeg> &vSeg = x.m_vHotSegRawAudio;         //audio raw time series indices
std::vector<HotSeg> &vSeg2 = x.m_vHotSegStfft;           //audio FFT frm indices

m_vApplaussingSegs.clear();
int i = 0;
BOOST_FOREACH(const HotSeg &seg, vSeg) {
m_vApplaussingSegs.push_back( seg );
#if _DEBUG
printf("i=%d, audioRawTimeSeriesIdxRng=[%lld,%lld]\n", i++,  seg.idx1, seg.idx2 );
#endif
}
return 0;
}

//input   m_stfftImg
//output  m_spFlux,m_spEnergy
int TimeFreqAna4MT::calSpFlux_n_Energy()
{
uint32 m = m_stfftImg.rows();  //frm or time domain
uint32 n = m_stfftImg.cols();  //spectrum domain, n= nFFT/2 + 1
SpectralFluxPtr spFluxAlg( new SpectralFlux( n ) );

//todo: getRow() is too expensive, avoiding hard copy
swfMat r = m_stfftImg.getRow( 0 );
spFluxAlg->setCurSp(r);
for (uint32 i = 1; i < m; ++i) {
swfMat r = m_stfftImg.getRow(i);
spFluxAlg->setCurSp(r);
m_spFlux(i) = spFluxAlg->calSF();

#if CAL_SP_ENERGY
m_spEnergy(i) = calFrmSpEnergy( r );
#endif
}
return 0;
}

float32 TimeFreqAna4MT::calFrmSpEnergy( const swfMat &sp)
{
float32 se = 0;
#if 0
swfMat sp2 = sp * sp;
se = swMatSumValue(sp2) + 1e-7;
sp2 /= se;						//psd
swfMat sp1 = swMatLog2(sp2);	//log2(psd)
sp2 *= sp1;                     

se = -swMatSumValue( sp2 );
#endif
return se;

}

int TimeFreqAna4MT::calStfftImg(const RawFrm &x)
{
//compute spetral image <m_stfftImg>
const uint64 nFrms = calStfftFrms( x.sz_ );
const uint32 nSp = m_cfg->nFFT / 2 + 1;
const uint32 winSzHalf = m_cfg->winSz / 2;

m_stfftImg.resize(nFrms, nSp);
m_vFrmIdInRawAudio.resize(nFrms);

float32 *p = (float32 *)x.buf_;
float32  *sp = m_stfftImg.getRowPtr(0);
uint64 i1 = 0;
for (uint64 i = 0; i < nFrms; ++i) {
setFFTInput(p, m_cfg->winSz);
//saveFft(m_fftIn, m_nFFT, "c:/temp/raw", "_frm", i );

//printMaxEle(m_fftIn, m_nFFT, "mic Raw", m_frm.fn,  chIdx);
//do FFT: input is <m_fftIn> and output is <m_fftOut>  
fftwf_execute_dft( m_vFftPlans[m_planId], m_fftIn, m_fftOut);
//saveFft(m_fftOut, m_nFFT, "c:/temp/rawFFT", "_frm", i);

calSpFromFftOutput(sp, nSp);
m_vFrmIdInRawAudio[i] = i1 + winSzHalf;

//for next frm
p += m_cfg->stepSz;
sp += nSp;
i1 += m_cfg->stepSz;
}

return 0;
}

//input <m_fftOut>, the <m_cfg->nFFT point fft, Complex array
//output <Y>
void TimeFreqAna4MT::calSpFromFftOutput( float32 *Y, uint32 nSp )
{
#if _DEBUG
assert( nSp == ( m_cfg->nFFT / 2 + 1) );
#endif

int nFFT = m_cfg->nFFT;
FFT_ELEMEMT R, I;

const FFT_ELEMEMT *p = (FFT_ELEMEMT *)m_fftOut;

//dc
R = *p++;
I = *p++;
Y[0] = sqrt(R*R + I*I) / nFFT;

float32    *q = Y+1;
for (int k = 1; k < (nSp-1); ++k) {
R = *p++;
I = *p++;
*q++ = 2.0f*sqrt(R*R + I*I) / nFFT;
}

//Fs/2 pt
R = *p++;
I = *p++;
Y[nSp-1] = sqrt(R*R + I*I) / nFFT;
}

void TimeFreqAna4MT::setFFTInput(const float32 *x, const uint32 nWinSz)
{
#if _DEBUG
assert( nWinSz <= m_cfg->nFFT );
#endif

//in-enfficent, image part was already set a zeros in constructor
//memset((void*)m_fftIn, 0, m_fftBufSzInBytes);
FFT_ELEMEMT *p = (FFT_ELEMEMT*)m_fftIn;
int i = 0;
for (; i < nWinSz; ++i) {
*p = *x++;		//real
p += 2;
}

for (; i < m_cfg->nFFT; ++i) {
*p = 0;		//real
p += 2;
}
}

uint64 TimeFreqAna4MT::calStfftFrms( const uint64 nBytes )
{
uint64 nPts = nBytes / sizeof(float32);
uint64 n = (nPts - m_cfg->winSz) / m_cfg->stepSz;
return n;
}

/*
1. FFTW_MEASURE or FFTW_ESTIMATE:
The flags argument is usually either FFTW_MEASURE or FFTW_ESTIMATE.

FFTW_MEASURE instructs FFTW to run and measure the execution time of several FFTs in order to find the
best way to compute the transform of size n.This process takes some time(usually a few
seconds), depending on your machine and on the size of the transform.

FFTW_ESTIMATE,on the contrary, does not run any computation and just builds a reasonable plan that is
probably sub - optimal.In short, if your program performs many transforms of the same size
and initialization time is not important, use FFTW_MEASURE; otherwise use the estimate.

2. FFTW_FORWARD or FFTW_BACKWARD
The fourth argument, sign, can be either FFTW_FORWARD(-1) or FFTW_BACKWARD(+1), and
indicates the direction of the transform you are interested in; technically, it is the sign of
the exponent in the transform.


3. computtation precisions
fftw -- float64
fftwf -- float32
fftwl -- float128
*/
bool TimeFreqAna4MT::allocFFT()
{
const int nFFT = m_cfg->nFFT;
const int nSp = 1 + nFFT / 2;

m_fftIn		 = new FFT_COMPLEX[nFFT];
m_fftOut	 = new FFT_COMPLEX[nFFT];

/* forward Fourier transform, save the result in 'out' */
m_vFftPlans[m_planId] = FFT_CREATE_PLAN_1D(nFFT, m_fftIn, m_fftOut, FFTW_FORWARD, FFTW_MEASURE);

m_freq.resize(1, nSp);
float64 f = 0, df = 1 / (nSp - 1);
for (int i = 0; i < nSp; ++i) {
m_freq(i) = f;
f += df;
}

return true;
}

void TimeFreqAna4MT::freeFFT()
{
FFT_DESTROY(m_vFftPlans[m_planId]);
FFT_CLEANUP();

if (m_fftIn) {
delete[] m_fftIn;
}

if (m_fftOut) {
delete[] m_fftOut;
}
}

void TimeFreqAna4MT::fftSetToNull()
{
m_fftIn = 0;
m_fftOut = 0;
}

void TimeFreqAna4MT::printFFT( const FFT_COMPLEX *p, const uint32 n, const int scale, const std::string &msg )
{
FFT_ELEMEMT *p0 = (FFT_ELEMEMT *)p;
for (int i = 0; i < n; i+=64) {
float R = (*p0++)/scale;
float I = (*p0++)/scale;
printf("%d, [%f, %f]\n", i, R, I);
}
}

void TimeFreqAna4MT::normalFFT(const FFT_COMPLEX *p, const uint32 nStart, const uint32 nPts, const char flag)
{
FFT_ELEMEMT *p00 = (FFT_ELEMEMT *)p;
p00 = p00 + (2*nStart);
if (flag == 'i' || flag == 'I') {
p00++;
}

FFT_ELEMEMT *p0 = p00;
FFT_ELEMEMT x0 = 0;
for ( uint32 i = 0; i < nPts; ++i) {
if (x0 < *p0) {
x0 = *p0;
}
p0 += 2;
}
x0 *= 2;
if ( x0 < 1e-7) {
x0 = 1e-7;
}

p0 = p00; 
for (uint32 i = 0; i < nPts; ++i) {
*p0 = *p0/x0;
p0 += 2;
}

}

void TimeFreqAna4MT::saveFft(const FFT_COMPLEX *fft, const uint32 n,  const string &fnameMain, const string &tag1, const int32 num1)
{
string fpath;
fpath = fnameMain + tag1 + std::to_string(num1)  + ".txt";

FILE *f = fopen(fpath.c_str(), "w");
if (f == NULL) {
return;
}

const FFT_ELEMEMT *p = (FFT_ELEMEMT *)fft;
for (int i = 0; i < n; ++i) {
fprintf(f, "%.9f,%.9f\n", p[0], p[1]);
p += 2;
}
fclose(f);

return;
}


void TimeFreqAna4MT::printMaxEle(const FFT_COMPLEX *fft, const uint32 n, const char *msg)
{
FFT_ELEMEMT t, x0=0;
const FFT_ELEMEMT *p = (FFT_ELEMEMT *)fft;
for (int i = 0; i < n; ++i) {
t = p[0]*p[0] + p[1]*p[1];
if (x0 < t) {
x0 = t;
}

p += 2;
}
printf("%s: maxEle=%f\n", msg, sqrt(x0));
return;
}


int TimeFreqAna4MT::findFinalAudioSegments(const RawFrm &x, const float fs, const float hilightSegmentLengthInSec, const std::string &dumpFileBase4Debug)
{
const uint64 nSmpPts = x.getNumOfFloatSmpPts();
const float32 *rawAudio = (const float32*)x.buf_;
bool isKeepAppluasing = true;

LocateHotSegsToCutVideo y(fs, hilightSegmentLengthInSec, m_cfg->nMaxHotSeg4EachVideoToCut);

y.process(m_vApplaussingSegs, isKeepAppluasing, rawAudio, nSmpPts);

if (dumpFileBase4Debug.length() > 0) {
y.saveHotSegsToWav(rawAudio, nSmpPts, dumpFileBase4Debug);
}
m_vSelectedAudioSegs = y.getResults();
return 0;
}
