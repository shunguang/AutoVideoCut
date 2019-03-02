/*
*-------------------------------------------------------------------
* swMatUtilities.h
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
#ifndef SW_MAT_UTILITIES
#define SW_MAT_UTILITIES

#include <assert.h>

#include "swMatDefs.h"
#include "swfMat.h"   //double matrix computation
#include "swdMat.h"   //double matrix computation
#include "swiMat.h"   //int matrix computation
#include "swMatLu.h"   //int matrix computation

double SWMAT_EXPORT swFactorial(const int n);

swdMat SWMAT_EXPORT swMatAbs( const swdMat &x );
double SWMAT_EXPORT swMatAngleBetweenTwoVectorsInRad(const swdMat &refVec, const swdMat &insVec);

swdMat SWMAT_EXPORT swMatCos( const swdMat &x );
swdMat SWMAT_EXPORT swMatCrossProduct(const swdMat &x,const swdMat &y);

double SWMAT_EXPORT swMatDotProduct( const swdMat &x, const swdMat &y );
float  SWMAT_EXPORT swMatDotProduct(const swfMat &x, const swfMat &y);
float  SWMAT_EXPORT swMatDotProduct(const float *p1, const float *p2, const uint32_t n);

swdMat SWMAT_EXPORT swMatOutProduct( const swdMat &v1, const swdMat &v2 );

double SWMAT_EXPORT swMatDist2( const swdMat &x, const swdMat &y);
double SWMAT_EXPORT swMatDistMahalanobis( const swdMat &v1, const swdMat &v2, const swdMat &cov1, const swdMat &cov2);
double SWMAT_EXPORT swMatDistMahalanobis2( const swdMat &v1, const swdMat &v2, const swdMat &cov1, const swdMat &cov2);

swdMat SWMAT_EXPORT swMatExp( swdMat &x );
swdMat SWMAT_EXPORT swMatEye( const unsigned int m );

//--------------------------------------
//h1[2], heading and its variance for the 1st
//h2[2], heading and its variance for the 2nd
//return heading dist in radus
//--------------------------------------
double SWMAT_EXPORT swdMatHeadingFromVel( const double vx, const double vy );  
double SWMAT_EXPORT swMatHeadingDist( const double hInRad1, const double hInRad2 );
double SWMAT_EXPORT swMatHeadingDist( const double *h1, const double *h2 );

//inverse & det functions
float SWMAT_EXPORT swMatDet3by3( const swfMat &x );

bool SWMAT_EXPORT swMatInv2by2( const swfMat &x, swfMat &xInv, float &xDet );

bool SWMAT_EXPORT swMatInv2by2( swdMat &x );
bool SWMAT_EXPORT swMatInv2by2( const swdMat &x, swdMat &xInv, double &det );
bool SWMAT_EXPORT swMatInv3by3( swdMat &x );
bool SWMAT_EXPORT swMatInv3by3( const swdMat &x, swdMat &xInv, double &det );
bool SWMAT_EXPORT swMatInv3by3( const swfMat &x, swfMat &xInv, float &det );
bool SWMAT_EXPORT swMatInvByLu ( const swdMat &x, swdMat &xInv, double &det );
bool SWMAT_EXPORT swMatInv( const swdMat &x, swdMat &xInv, double &det );
bool SWMAT_EXPORT swMatVerifyInv( const swdMat &x, const swdMat &y );

bool SWMAT_EXPORT swMatIsEmpty( const swdMat &x );
bool SWMAT_EXPORT swMatIsEmpty( const swfMat &x );
bool SWMAT_EXPORT swMatIsEmpty( const swiMat &x );

//x, m x n, matrix
//return the idxs satisfying the condition
swiMat SWMAT_EXPORT swMatFindGreater  ( const swiMat &x, const int threshhold );
swiMat SWMAT_EXPORT swMatFindGreater  ( const swfMat &x, const float threshhold );
swiMat SWMAT_EXPORT swMatFindGreater  ( const swdMat &x, const double threshhold );

swiMat SWMAT_EXPORT swMatFindGreaterEq( const swfMat &x, const float threshhold );
swiMat SWMAT_EXPORT swMatFindSmaller  ( const swfMat &x, const float threshhold );
swiMat SWMAT_EXPORT swMatFindSmaller  ( const swdMat &x, const double threshhold );
swiMat SWMAT_EXPORT swMatFindSmallerEq( const swfMat &x, const float threshhold );

swdMat SWMAT_EXPORT swMatMax( const swdMat &x, const int dimFlag =1 );

int	   SWMAT_EXPORT swMatMaxValue( const swiMat &x, int &idx );
float  SWMAT_EXPORT swMatMaxValue( const swfMat &x );
float  SWMAT_EXPORT swMatMaxValue( const swfMat &x, int &idx );
double SWMAT_EXPORT swMatMaxValue( const swdMat &x );
double SWMAT_EXPORT swMatMaxValue( const swdMat &x, int &idx );

swdMat SWMAT_EXPORT swMatMean( const swdMat &x, const int dimFlag =1 );
double SWMAT_EXPORT swMatMeanValue( const swdMat &x );
float  SWMAT_EXPORT swMatMeanValue( const swfMat &x, const int &n );
bool   SWMAT_EXPORT swMatMeanStd( const swdMat &x, double &mean, double &std);
bool   SWMAT_EXPORT swMatMeanStd( const swfMat &x, const int &n, float &mean, float &std);
swdMat SWMAT_EXPORT swMatMedian( const swdMat &x, const int dimFlag =1 );
double SWMAT_EXPORT swMatMedianValue(const swdMat &x );

swdMat SWMAT_EXPORT swMatMin( const swdMat &x,  const int dimFlag =1 );

float SWMAT_EXPORT swMatMinValue(const  swfMat &x);
float SWMAT_EXPORT swMatMinValue(const  swfMat &x, unsigned int &idx);
double SWMAT_EXPORT swMatMinValue(const  swdMat &x );

double SWMAT_EXPORT swMatNorm(const swdMat &x);

swdMat SWMAT_EXPORT swMatOnes( const unsigned int m);
swdMat SWMAT_EXPORT swMatOnes( const unsigned int m, const unsigned int n );

swdMat SWMAT_EXPORT swMatReshape( const swdMat &x, const unsigned int m, const unsigned int n );
void SWMAT_EXPORT swMatReshape( const swdMat &x, swdMat &y );

swdMat SWMAT_EXPORT swMatSelectColEq( const swdMat &x, const int col,  const double checkVal );

swdMat SWMAT_EXPORT swMatSin( const swdMat &x );
void SWMAT_EXPORT swMatSort( const swfMat &x, swfMat &y, swiMat &I, const bool isDescending=false );
void SWMAT_EXPORT swMatSort( const swdMat &x, swdMat &y, swiMat &I, const bool isDescending=false );
double SWMAT_EXPORT swMatSumValue(  const swdMat &x );
float SWMAT_EXPORT swMatSumValue(  const swfMat &x );
int SWMAT_EXPORT swMatSumValue(  const swiMat &x );

swfMat SWMAT_EXPORT swMatSum(  const swfMat &x, const int dimFlag =1 );
swdMat SWMAT_EXPORT swMatSum(  const swdMat &x, const int dimFlag =1 );
swiMat SWMAT_EXPORT swMatSum(  const swiMat &x, const int dimFlag =1 );

swdMat SWMAT_EXPORT swMatTan( const swdMat &x );

swdMat SWMAT_EXPORT swMatZeros( const unsigned int m);
swdMat SWMAT_EXPORT swMatZeros( const unsigned int m, const unsigned int n );

//find all the peaks in vector x
void SWMAT_EXPORT swMatPeakFinder( const swfMat &x, swfMat &peakValues, swiMat &peakIds );

//find all the peaks which values are greater or equal than max_peak_value * threshold.
//where $0 <= threshold <= 1$
void SWMAT_EXPORT swMatPeakFinder( const swfMat &x, swfMat &peakValues, swiMat &peakIds, const float threshold );

//find SNR from a given peak
float SWMAT_EXPORT swMatSnr( const swfMat &x, const int peakId, const float coeffThd,
int &nSignalBins, int &nTotalBins);
float SWMAT_EXPORT swMatCrossCorrelation( const swfMat &vSignal, const swfMat &vMask,  swfMat &vCorr );
float SWMAT_EXPORT swMatCircularCorrelation( const swfMat &h1, const swfMat &h2 );

//x = U*S*V'
bool SWMAT_EXPORT swMatSvd2x2 ( const swfMat &x, swfMat &U, swfMat &S, swfMat &V  );
bool SWMAT_EXPORT swMatEigen2x2 ( const swfMat &A, swfMat &vec, swfMat &val  );

void SWMAT_EXPORT swMatErrMsg ( const char* msgStr);
void SWMAT_EXPORT swMatWarningMsg ( const char* msgStr);


#endif
