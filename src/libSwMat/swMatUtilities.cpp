/*
*-------------------------------------------------------------------
* swMatUtilities.cpp
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
//#include "stdafx.h"
#include "swMatUtilities.h"

double swFactorial(const int n)
{
double x = 1.0;
if (n==0)
return x;

for(int i=1; i<n+1; i++)
x *= (double) i;

return x;
}

//--------------------------------------
//
//--------------------------------------
swdMat swMatAbs( const swdMat &x )
{
if ( swMatIsEmpty(x) ){
swMatWarningMsg( "swMatAbs(): x  is an emppty matrix!");
return x;
}

swdMat y(x);

double* p=y.getBuf();
for (unsigned int i=0; i<y.size(); i++){
if (p[i] < 0)
p[i] = -p[i];
}
return y;
}

//get angle betwee two vectors
//refVec, 2 x 1,
//insVec, 2 x 1,
//return [0,pi]  if ins. vector is located at the left of ref. vector
//		 [-pi,0) if ins. vector is located at the right of ref. vector
//		 SWF_MAT_REALMAX if in valid input	
double swMatAngleBetweenTwoVectorsInRad(const swdMat &refVec, const swdMat &insVec)
{
double refL2 =  swMatDotProduct( refVec, refVec );
double insL2 =  swMatDotProduct( insVec, insVec );
if ( refL2 < SWF_MAT_EPS || insL2 < SWF_MAT_EPS ){
return SWF_MAT_REALMAX;
}
swdMat R(2,2), newInsVec(2,1);

//rotate the inspection vector
refL2 = sqrt( refL2 );
R(0,0) = refVec(0)/refL2; R(0,1) = refVec(1)/refL2;
R(1,0) = -R(0,1);    R(1,1) = R(0,0);
newInsVec = R%insVec;

double result = atan2( newInsVec(1), newInsVec(0) );

//printf( "refAng=%f, newInsVec(%f,%f), result=%f\n", refAng*SW_MAT_R2D, newInsVec(0), newInsVec(1),result*SW_MAT_R2D);
return result;
}
//--------------------------------------
//
//--------------------------------------
swdMat swMatCrossProduct(const swdMat &a,const swdMat &b)
{
if ((a.cols()*a.rows()!=3) ||(b.cols()*b.rows()!=3))
{
printf( "cross product only works on 3 vector!" );
swdMat zero(0, 0, 0);
return zero;
}
swdMat c(a.cols(),a.rows(),0);
c(0) = a(1)*b(2)-a(2)*b(1);
c(1) = a(2)*b(0)-a(0)*b(2);
c(2) = a(0)*b(1)-a(1)*b(0);
return c;
}

//--------------------------------------
//
//--------------------------------------
double  swMatDotProduct( const swdMat &x, const swdMat &y )
{
#if SW_MAT_BOUNDS_CHECK
if ( x.size() != y.size() ){
swMatErrMsg( "dotProduct(): size is not match!");
}

if ( swMatIsEmpty(x) || swMatIsEmpty(y) ){
swMatErrMsg( "dotProduct(): x or y is an emppty matrix!");
}
#endif

const double* px = x.data();
const double* py = y.data();

double d = 0.0;
for(unsigned int i=0; i<x.size(); i++)
d += (*(px+i)) * (*(py+i));

return d;
}

//--------------------------------------
//
//--------------------------------------
float  swMatDotProduct(const float *p1, const float *p2, const uint32_t n)
{
float d = 0.0;
for (uint32_t i = 0; i < n; i++) {
d += (*p1++) * (*p2++);
}

return d;

}

float  swMatDotProduct( const swfMat &x, const swfMat &y )
{
#if SW_MAT_BOUNDS_CHECK
if ( x.size() != y.size() ){
swMatErrMsg( "dotProduct(): size is not match!");
}

if ( swMatIsEmpty(x) || swMatIsEmpty(y) ){
swMatErrMsg( "dotProduct(): x or y is an emppty matrix!");
}
#endif

const float* px = x.data();
const float* py = y.data();

return swMatDotProduct(px, py, x.size());
}

swdMat swMatOutProduct( const swdMat &v1, const swdMat &v2 )
{
int i, j;
int m = v1.size();
int n = v2.size();
swdMat A(m, n);

for (i=0; i<m; i++)
for(j=0; j<n; j++)
A(i,j) = v1(i) * v2(j) ;

return ( A );
}

double swMatDist2( const swdMat &x, const swdMat &y)
{
#if SW_MAT_BOUNDS_CHECK
if ( x.size() != y.size() ){
swMatErrMsg( "swMatDist2(): size is not match!");
}

if ( swMatIsEmpty(x) || swMatIsEmpty(y) ){
swMatErrMsg( "swMatDist2(): x or y is an emppty matrix!");
}
#endif

const double* px = x.data();
const double* py = y.data();

double d2 = 0.0;
double tmp;
for(unsigned int i=0; i<x.size(); i++){
tmp = (*(px+i)) - (*(py+i));
d2 += tmp*tmp;
}

return d2;
}

//v1, v2, n x 1
//cov1, cov2, n x n,
double swMatDistMahalanobis( const swdMat &v1, const swdMat &v2, const swdMat &cov1, const swdMat &cov2)
{
#if SW_MAT_BOUNDS_CHECK
if ( swMatIsEmpty(v1) || swMatIsEmpty(v2) ){
swMatErrMsg( "swMatDistMahalanobis(): v1 or v2 is an emppty matrix!");
}

if ( v1.size() != v2.size() ){
swMatErrMsg( "swMatDistMahalanobis(): sizes of v1 and v2 are not match!");
}
//TODO: also check the sizes of cov1 and cov2
#endif

swdMat nu(v1 - v2);
swdMat S( cov1 + cov2 );
swdMat invS( v1.size(), v1.size() );
double detS;
#if 0
v1.print	("%10.5f ", "v1=");
v2.print	("%10.5f ", "v2=");
cov1.print	("%10.5f ", "cov1=");
cov2.print	("%10.5f ", "cov2=");
S.print		("%10.5f ", "S=");
#endif

if ( swMatInvByLu ( S, invS, detS ) ){
detS = sqrt( swMatDotProduct( nu, invS%nu ) );
return detS;
}
else{
printf("swMatDistMahalanobis(): inv S is not exist, return -1!");
v1.print	("%10.5f ", "v1=");
v2.print	("%10.5f ", "v2=");
cov1.print	("%10.5f ", "cov1=");
cov2.print	("%10.5f ", "cov2=");
S.print		("%10.5f ", "S=");
printf("--------------- swMatDistMahalanobis() -------------!");
return -1.0;
}
}

//v1, v2, n x 1
//cov1, cov2, n x 1, only diagonal terms
double swMatDistMahalanobis2( const swdMat &v1, const swdMat &v2, const swdMat &cov1, const swdMat &cov2)
{
#if SW_MAT_BOUNDS_CHECK
if ( swMatIsEmpty(v1) || swMatIsEmpty(v2) ){
swMatErrMsg( "swMatDistMahalanobis(): v1 or v2 is an emppty matrix!");
}

if ( v1.size() != v2.size() ){
swMatErrMsg( "swMatDistMahalanobis(): sizes of v1 and v2 are not match!");
}
//TODO: also check the sizes of cov1 and cov2
#endif

swdMat nu(v1 - v2);
swdMat S( cov1 + cov2 );
double d=0.0;
unsigned int n=v1.size();
for (unsigned int i=0; i<n; i++){
if( S(i)> SWD_MAT_EPS ){
d += ( nu(i)*nu(i) / S(i) );
}else{
return -1;
}
}

return sqrt(d);
}

// start from E
//--------------------------------------
//
//--------------------------------------
swdMat  swMatEye( const unsigned int m)
{
swdMat x(m,m,0);
x.setDiagonal( 1.0 );
return x;
}

swiMat swMatFindGreater( const swiMat &x, const int threshhold )
{
int n = x.size();
int i, k=0;

swiMat y( 1, n );
for (i=0; i<n; i++ ){
if ( x(i) > threshhold ){
y(k) = i; k++;
}
}

return y.getSubRow(0, 0, k);
}

swiMat swMatFindGreater( const swfMat &x, const float threshhold )
{
int n = x.size();
int i, k=0;

swiMat y( 1, n );
for (i=0; i<n ; i++ ){
if ( x(i) > threshhold ){
y(k) = i; k++;
}
}

return y.getSubRow(0, 0, k);
}

swiMat swMatFindGreater( const swdMat &x, const double threshhold )
{
int n = x.size();
int i, k=0;

swiMat y( 1, n );
const double *px = x.data();
int *py = y.getBuf();
for (i=0; i<n ; ++i, ++px ){
if ( *px > threshhold ){
*py++ = i;
++k;
}
}

return y.getSubRow(0, 0, k);
}

swiMat swMatFindGreaterEq( const swfMat &x, const float threshhold )
{
int n = x.size();
int i, k=0;

swiMat y( 1, n );
for (i=0; i<n ; i++ ){
if ( x(i) >= threshhold ){
y(k) = i; k++;
}
}

return y.getSubRow(0, 0, k);
}

swiMat swMatFindSmaller( const swfMat &x, const float threshhold )
{
int n = x.size();
int i, k=0;

swiMat y( 1, n );
for (i=0; i<n ; i++ ){
if ( x(i) < threshhold ){
y(k) = i; k++;
}
}

return y.getSubRow(0, 0, k);
}

swiMat swMatFindSmaller( const swdMat &x, const double threshhold )
{
int n = x.size();
int i, k=0;

swiMat y( 1, n );

const double *px=x.data();
int *py=y.getBuf();
for (i=0; i<n ; ++i, ++px ){
if ( *px < threshhold ){
*py++ = i; 
k++;
}
}

return y.getSubRow(0, 0, k);
}

swiMat swMatFindSmallerEq( const swfMat &x, const float threshhold )
{
int n = x.size();
int i, k=0;

swiMat y( 1, n );
for (i=0; i<n ; i++ ){
if ( x(i) <= threshhold ){
y(k) = i; k++;
}
}
return y.getSubRow(0, 0, k);
}

//--------------------------------------
//
//--------------------------------------
double swMatNorm(const swdMat &x)
{
//swdMatSvd a(x);
//double res=swMatMaxValue(a.getS());
double res=0; double item=0;
for (unsigned int i=0;i<x.cols();i++)
for (unsigned int j=0;j<x.rows();j++)
{
item = x(j,i);
res += item*item;
}
return sqrt(res);
}

//cal $m_heading$ w.r.t. to vx
//return [0,2pi]
double swdMatHeadingFromVel( const double vx, const double vy )  
{
double heading;
if( fabs(vx)<1e-6 && fabs(vy)< 1e-6 ){
heading = SWF_MAT_REALMAX;		//invalid
}
else{
heading = atan2(vy, vx);
}

if ( heading < 0 ){
heading += SW_MAT_TWO_PI;
}
return heading; //in rad.
}

double swMatHeadingDist( const double hInRad1, const double hInRad2 )
{
double dotProduct =  cos( hInRad1 ) * cos( hInRad2 ) + sin( hInRad1 ) * sin( hInRad2 );

if ( dotProduct > 1 )
dotProduct = 1 - SWF_MAT_EPS;
else if ( dotProduct < -1 )
dotProduct = -1 + SWF_MAT_EPS;

double dAng = acos(dotProduct);
//printf( "h1=%f, h2=%f, dotProd=%f, dAng=%f\n", hInRad1, hInRad2, dotProduct, dAng );
return dAng;  //in radius
}

//--------------------------------------
//h1[2], heading and its variance for the 1st
//h2[2], heading and its variance for the 2nd
//return heading dist in radus
//--------------------------------------
double swMatHeadingDist( const double *h1, const double *h2 )
{
double ang1 = h1[0] * SW_MAT_D2R; 
double ang2 = h2[0] * SW_MAT_D2R;
double var = ( sqrt(h1[1]) + sqrt(h2[1]) ) * SW_MAT_D2R;  //in radus
double dotProduct =  cos( ang1 ) * cos( ang2 ) + sin( ang1 ) * sin( ang2 );

if ( dotProduct > 1 )
dotProduct = 1 - SWF_MAT_EPS;
else if ( dotProduct < -1 )
dotProduct = -1 + SWF_MAT_EPS;

ang1 = acos(dotProduct) + var;
//printf( "trk(%f,%f), obs(%f,%f), var=%f, dotProduct=%f, dist=%f\n", h1[0],h1[1],h2[0],h2[1], var, dotProduct, ang1);
return ang1;  //in radius
}


//--------------------------------------
//
//--------------------------------------
bool swMatIsEmpty( const swdMat &x )
{
return ( x.size()>0 ? false : true );
}

bool swMatIsEmpty( const swfMat &x )
{
return ( x.size()>0 ? false : true );
}

bool swMatIsEmpty( const swiMat &x )
{
return ( x.size()>0 ? false : true );
}

//--------------------------------------
//
//--------------------------------------
swdMat  swMatOnes( const unsigned int m)
{
return swMatOnes(m, m);
}

//--------------------------------------
//
//--------------------------------------
swdMat  swMatOnes( const unsigned int m, const unsigned int n)
{
swdMat x(m, n, 1);
return x;
}

//--------------------------------------
//
//--------------------------------------
swdMat  swMatMax( const swdMat &x, const int byRowColFlag )
{
if ( swMatIsEmpty(x) ){
swMatWarningMsg( "swMatMax(): x  is an emppty matrix!");
return x;
}

if (byRowColFlag==1) //return a row vector
{
unsigned int n = x.cols();
swdMat y(1, n);
for(unsigned int i=0; i<n; i++){
y(i) = swMatMaxValue( x.getCol(i) ) ;
}

return y;
}
else //return a col vector
{
unsigned int m = x.rows();
swdMat y(m, 1);
for(unsigned int i=0; i<m; i++){
y(i) = swMatMaxValue( x.getRow(i) ) ;
}
return y;
}
}

float  swMatMaxValue( const swfMat &x )
{
#if SW_MAT_BOUNDS_CHECK
if ( swMatIsEmpty(x) ){
swMatErrMsg( "swMatMaxValue(): x  is an emppty matrix!");
}
#endif

const float* px = x.data();
float d = *px;

for(unsigned int i=1; i<x.size(); i++){
if  ( d < *(px+i) ){
d = *(px+i);
}
}
return d;
}


//--------------------------------------
//
//--------------------------------------
float  swMatMaxValue( const swfMat &x, int &idx )
{
#if SW_MAT_BOUNDS_CHECK
if ( swMatIsEmpty(x) ){
swMatErrMsg( "swMatMaxValue(): x  is an emppty matrix!");
}
#endif

const float* px = x.data();
float d = *px;

for(unsigned int i=1; i<x.size(); i++){
if  ( d < *(px+i) ){
d = *(px+i);
idx = i;
}
}

return d;
}

double  swMatMaxValue( const swdMat &x )
{
#if SW_MAT_BOUNDS_CHECK
if ( swMatIsEmpty(x) ){
swMatErrMsg( "swMatMaxValue(): x  is an emppty matrix!");
}
#endif

const double* px = x.data();
double d = *px;
for(unsigned int i=0; i<x.size(); ++i, ++px){
if  ( d < *px ){
d = *px;
}
}

return d;
}

//--------------------------------------
//
//--------------------------------------
double  swMatMaxValue( const swdMat &x, int &idx )
{
if ( swMatIsEmpty(x) ){
return SW_MAT_NAN;
idx = -1;
//swMatErrMsg( "swMatMaxValue(): x  is an emppty matrix!");
}

const double* px = x.data();
double d = *px;

idx = 0;
for(unsigned int i=0; i<x.size(); ++i, ++px ){
if  ( d < *px ){
d = *px;
idx = i;
}
}

return d;
}

int	swMatMaxValue( const swiMat &x, int &idx )
{

#if SW_MAT_BOUNDS_CHECK
if ( swMatIsEmpty(x) ){
idx=-1;
return -1;
}
#endif

const int* px = x.data();
int theMax = *px;
idx = 0;
for(unsigned int i=0; i<x.size(); ++i, ++px){
if  ( theMax < *px ){
theMax = *px;
idx = i;
}
}

return theMax;
}

//--------------------------------------
//
//--------------------------------------
swdMat  swMatMean( const swdMat &x, const int byRowColFlag )
{
if ( swMatIsEmpty(x) ){
swMatWarningMsg( "swMatMean(): x  is an emppty matrix!");
return x;
}

if (byRowColFlag==1) //return a row vector
{
unsigned int n = x.cols();
swdMat y(1, n);
for(unsigned int i=0; i<n; i++){
y(i) = swMatMeanValue( x.getCol(i) ) ;
}

return y;
}
else //return a col vector
{
unsigned int m = x.rows();
swdMat y(m, 1);
for(unsigned int i=0; i<m; i++){
y(i) = swMatMeanValue( x.getRow(i) ) ;
}
return y;
}
}

//--------------------------------------
//
//--------------------------------------
double  swMatMeanValue( const swdMat &x )
{
if ( x.size() == 0 ){
return SW_MAT_NAN;
}

#if SW_MAT_BOUNDS_CHECK
if ( swMatIsEmpty(x) ){
swMatWarningMsg( "mean(): x  is an emppty matrix!");
return SWD_MAT_REALMAX;
}
#endif

const double* px = x.data();
double d = 0.0;
for(unsigned int i=0; i<x.size(); i++){
d += (*px++);
}
return d/x.size();
}

//--------------------------------------
//
//--------------------------------------
float  swMatMeanValue( const swfMat &x, const int &n )
{
#if SW_MAT_BOUNDS_CHECK
assert( n<= (int)x.size() );
if ( swMatIsEmpty(x) ){
swMatWarningMsg( "mean(): x  is an emppty matrix!");
return SWF_MAT_REALMAX;
}
#endif

const float* px = x.data();
float d = 0.0;
for(int i=0; i<n; i++){
d += *(px++);
}
return d/n;
}

bool swMatMeanStd( const swdMat &x, double &mean, double &std)
{
if ( swMatIsEmpty(x) ){
swMatWarningMsg( "mean(): x  is an emppty matrix!");
mean = SW_MAT_NAN; 
std  = SW_MAT_NAN;
return false;
}

//x.print();

mean = swMatMeanValue( x );

int  n = x.size(); 
double dx,s = 0.0;
const double *px = x.data();
for(int i=0; i<n; i++){
dx = *(px++) - mean;
s += dx*dx;
}

if ( n>1 )
n--;

std = sqrt ( s/(double)n );

return true;
}

bool swMatMeanStd( const swfMat &x, const int &n, float &mean, float &std)
{
//x.print();
mean = swMatMeanValue( x, n );

float dx,s = 0.0;
const float *px = x.data();
for(int i=0; i<n; i++){
dx = (*px++)-mean;
s += dx*dx;
}

int m = n;
if ( m>1 )
m--;

std = sqrt ( s/m );

return true;
}

//--------------------------------------
//
//--------------------------------------
swdMat  swMatMedian( const swdMat &x, const int byRowColFlag )
{
if ( swMatIsEmpty(x) ){
swMatWarningMsg( "swMatMax(): x  is an emppty matrix!");
return x;
}

if (byRowColFlag==1) //return a row vector
{
unsigned int n = x.cols();
swdMat y(1, n);
for(unsigned int i=0; i<n; i++){
y(i) = swMatMedianValue( x.getCol(i) ) ;
}

return y;
}
else //return a col vector
{
unsigned int m = x.rows();
swdMat y(m, 1);
for(unsigned int i=0; i<m; i++){
y(i) = swMatMedianValue( x.getRow(i) ) ;
}
return y;
}
}

//--------------------------------------
//
//--------------------------------------
double  swMatMedianValue(const swdMat &x )
{
#if SW_MAT_BOUNDS_CHECK
if ( swMatIsEmpty(x) ){
swMatWarningMsg( "median(): x  is an emppty matrix!");
return SWD_MAT_REALMAX;
}
#endif

double a = swMatMinValue(x);
double b = swMatMaxValue(x);

return 0.5*(a+b);
}

//--------------------------------------
//
//--------------------------------------
swdMat  swMatMin( const swdMat &x,  const int byRowColFlag )
{
if ( swMatIsEmpty(x) ){
swMatWarningMsg( "swMatMin(): x  is an emppty matrix!");
return x;
}

if (byRowColFlag==1) //return a row vector
{
unsigned int n = x.cols();
swdMat y(1, n);
for(unsigned int i=0; i<n; i++){
y(i) = swMatMinValue( x.getCol(i) ) ;
}

return y;
}
else //return a col vector
{
unsigned int m = x.rows();
swdMat y(m, 1);
for(unsigned int i=0; i<m; i++){
y(i) = swMatMinValue( x.getRow(i) ) ;
}
return y;
}
}

//--------------------------------------
//
//--------------------------------------
float  swMatMinValue( const swfMat &x )
{
#if SW_MAT_BOUNDS_CHECK
if ( swMatIsEmpty(x) ){
swMatErrMsg( "minValue(): x  is an emppty matrix!");
}
#endif

const float* px = x.data();
float d = SWF_MAT_REALMAX;

for(unsigned int i=0; i<x.size(); i++){
if  ( d > *(px+i) )
d = *(px+i);
}

return d;
}

float  swMatMinValue(const swfMat &x, unsigned int &idx)
{
#if SW_MAT_BOUNDS_CHECK
if (swMatIsEmpty(x)) {
swMatErrMsg("minValue(): x  is an emppty matrix!");
}
#endif

const float* px = x.data();
float d = SWF_MAT_REALMAX;
idx = 0;
for (unsigned int i = 0; i<x.size(); i++) {
if (d > *(px + i)) {
d = *(px + i);
idx = i;
}
}

return d;
}


//--------------------------------------
//
//--------------------------------------
double  swMatMinValue( const swdMat &x )
{
#if SW_MAT_BOUNDS_CHECK
if ( swMatIsEmpty(x) ){
swMatErrMsg( "minValue(): x  is an emppty matrix!");
}
#endif

const double* px = x.data();
double d = SWD_MAT_REALMAX;

for(unsigned int i=0; i<x.size(); i++){
if  ( d > *(px+i) )
d = *(px+i);
}

return d;
}

bool swMatInv( const swdMat &x, swdMat &xInv, double &det )
{

#if SW_MAT_BOUNDS_CHECK
if ( x.rows() != x.cols() )
throw runtime_error( "swMatInv(): x is not a square matrix!");

if ( x.rows() != xInv.rows() || x.cols() != xInv.cols() )
throw runtime_error( "swMatInv(): xInv size is not correct!");
#endif

unsigned int m = x.rows();
bool isInvExist;
if ( m==1){
det = x(0,0);
if( fabs( det ) > SWD_MAT_EPS ){
isInvExist = true;
xInv(0,0) = 1.0/det;
}
else{
isInvExist = false;
}
}
else if ( m==2 ){
isInvExist = swMatInv2by2( x, xInv, det );
}
else if ( m==3 ){
isInvExist = swMatInv3by3( x, xInv, det );
}
else{ //LU decoposition
isInvExist = swMatInvByLu( x, xInv, det );
}

return isInvExist;
}

bool swMatVerifyInv( const swdMat &x, const swdMat &y )
{

#if SW_MAT_BOUNDS_CHECK
if ( x.rows() != x.cols() ||  y.rows() != y.cols() ){
throw runtime_error( "swMatVerifyInv(): x or y is not a square matrix!");
}

if ( x.cols() != y.rows() ){
throw runtime_error( "swMatVerifyInv(): sizes of x and y are not match!");
}
#endif

swdMat I( x%y );

unsigned int m = I.rows(); 

//check diagonal elements
for( unsigned int i=0; i<m; ++i ){
double tmp = fabs( I(i,i) -1.0 );
if( tmp > SWD_MAT_EPS2 ){
I.print("swMatVerifyInv(): x%y=");
return false;
}
}

//check non-diagonal elements
for( unsigned int i=0; i<m; ++i ){
for( unsigned int j=0; j<m; ++j ){
if ( i!= j ){
double tmp = fabs( I(i,j) );
if( tmp > SWD_MAT_EPS2 ){
I.print("swMatVerifyInv(): x%y=");
return false;
}
}
}
}

return true;
}

//--------------------------------------
//
//--------------------------------------
bool swMatInv2by2( swdMat &x )
{
#if SW_MAT_BOUNDS_CHECK
if ( x.rows() != x.cols() ||  x.rows() !=2 )
throw runtime_error( "swMatInv2by2(): x is not a square matrix!");
#endif

double *ptrBuf = x.getBuf();

double	detA	= ptrBuf[0] * ptrBuf[3]  - ptrBuf[1] * ptrBuf[2];
if ( fabs(detA) < SWD_MAT_EPS )
return	false;

double tmp = ptrBuf[0];
ptrBuf[0]	=  ptrBuf[3] / detA;
ptrBuf[1]	= -ptrBuf[1] / detA;
ptrBuf[2]	= -ptrBuf[2] / detA;
ptrBuf[3]	=  tmp  / detA;

return	true;	
}

bool swMatInv2by2( const swdMat &x, swdMat &xInv, double &xDet )
{
#if SW_MAT_BOUNDS_CHECK
if ( x.rows() != x.cols() ||  x.rows() !=2 )
throw runtime_error( "swMatInv2by2(): x is not a square matrix!");
#endif

const double *ptrBuf = x.data();
xDet = ptrBuf[0] * ptrBuf[3]  - ptrBuf[1] * ptrBuf[2];
if ( fabs(xDet) < SWD_MAT_EPS )
return	false;

xInv(0)	=  ptrBuf[3] / xDet;
xInv(1)	= -ptrBuf[1] / xDet;
xInv(2)	= -ptrBuf[2] / xDet;
xInv(3)	=  ptrBuf[0] / xDet;

return	true;	
}

bool swMatInv2by2( const swfMat &x, swfMat &xInv, float &xDet )
{
#if SW_MAT_BOUNDS_CHECK
if ( x.rows() != x.cols() ||  x.rows() !=2 )
throw runtime_error( "swMatInv2by2(): x is not a square matrix!");
#endif

const float *ptrBuf = x.data();
xDet = ptrBuf[0] * ptrBuf[3]  - ptrBuf[1] * ptrBuf[2];
if ( fabs(xDet) < SWD_MAT_EPS )
return	false;

xInv(0)	=  ptrBuf[3] / xDet;
xInv(1)	= -ptrBuf[1] / xDet;
xInv(2)	= -ptrBuf[2] / xDet;
xInv(3)	=  ptrBuf[0] / xDet;

return	true;	
}

//--------------------------------------
//
//--------------------------------------
bool swMatInv3by3 ( swdMat &x )
{
#if SW_MAT_BOUNDS_CHECK
if ( x.rows() != x.cols() ||  x.rows() !=3 )
throw runtime_error( "swMatInv3by3(): x is not a 3 x 3 matrix!");
#endif

double *a = x.getBuf();
double	detA	=	a[0] * a[4] * a[8] + a[1] * a[5] * a[6] +
a[2] * a[3] * a[7] - a[2] * a[4] * a[6] -
a[5] * a[7] * a[0] - a[1] * a[3] * a[8];
if ( fabs(detA) < SWD_MAT_EPS )
return	false;

double b[9];
b[0]	= ( a[4] * a[8] - a[5] * a[7] ) / detA;
b[1]	= ( a[2] * a[7] - a[1] * a[8] ) / detA;
b[2]	= ( a[1] * a[5] - a[2] * a[4] ) / detA;
b[3]	= ( a[5] * a[6] - a[3] * a[8] ) / detA;
b[4]	= ( a[0] * a[8] - a[2] * a[6] ) / detA;
b[5]	= ( a[2] * a[3] - a[0] * a[5] ) / detA;
b[6]	= ( a[3] * a[7] - a[4] * a[6] ) / detA;
b[7]	= ( a[1] * a[6] - a[0] * a[7] ) / detA;
b[8]	= ( a[0] * a[4] - a[1] * a[3] ) / detA;

for (int k=0; k<9; k++)
a[k] = b[k];

return	true;	
}

bool swMatInv3by3( const swdMat &x, swdMat &xInv, double &xDet )
{
#if SW_MAT_BOUNDS_CHECK
if ( x.rows() != x.cols() ||  x.rows() !=3 )
throw runtime_error( "swMatInv3by3(): x is not a 3 x 3 matrix!");
#endif

const double *a = x.data();
xDet	=	a[0] * a[4] * a[8] + a[1] * a[5] * a[6] +
a[2] * a[3] * a[7] - a[2] * a[4] * a[6] -
a[5] * a[7] * a[0] - a[1] * a[3] * a[8];
if ( fabs(xDet) < SWD_MAT_EPS )
return	false;

xInv(0)	= ( a[4] * a[8] - a[5] * a[7] ) / xDet;
xInv(1)	= ( a[2] * a[7] - a[1] * a[8] ) / xDet;
xInv(2)	= ( a[1] * a[5] - a[2] * a[4] ) / xDet;
xInv(3)	= ( a[5] * a[6] - a[3] * a[8] ) / xDet;
xInv(4)	= ( a[0] * a[8] - a[2] * a[6] ) / xDet;
xInv(5)	= ( a[2] * a[3] - a[0] * a[5] ) / xDet;
xInv(6)	= ( a[3] * a[7] - a[4] * a[6] ) / xDet;
xInv(7)	= ( a[1] * a[6] - a[0] * a[7] ) / xDet;
xInv(8)	= ( a[0] * a[4] - a[1] * a[3] ) / xDet;

return	true;	
}

float swMatDet3by3( const swfMat &x )
{
#if SW_MAT_BOUNDS_CHECK
if ( x.rows() != x.cols() ||  x.rows() !=3 )
throw runtime_error( "swMatDet3by3(): x is not a 3 x 3 matrix!");
#endif

const float *a = x.data();
float xDet	=	a[0] * a[4] * a[8] + a[1] * a[5] * a[6] +
a[2] * a[3] * a[7] - a[2] * a[4] * a[6] -
a[5] * a[7] * a[0] - a[1] * a[3] * a[8];

return	xDet;	
}


bool swMatInv3by3( const swfMat &x, swfMat &xInv, float &xDet )
{
#if SW_MAT_BOUNDS_CHECK
if ( x.rows() != x.cols() ||  x.rows() !=3 )
throw runtime_error( "swMatInv3by3(): x is not a 3 x 3 matrix!");
#endif

const float *a = x.data();
xDet	=	a[0] * a[4] * a[8] + a[1] * a[5] * a[6] +
a[2] * a[3] * a[7] - a[2] * a[4] * a[6] -
a[5] * a[7] * a[0] - a[1] * a[3] * a[8];
if ( fabs(xDet) < SWF_MAT_EPS )
return	false;

xInv(0)	= ( a[4] * a[8] - a[5] * a[7] ) / xDet;
xInv(1)	= ( a[2] * a[7] - a[1] * a[8] ) / xDet;
xInv(2)	= ( a[1] * a[5] - a[2] * a[4] ) / xDet;
xInv(3)	= ( a[5] * a[6] - a[3] * a[8] ) / xDet;
xInv(4)	= ( a[0] * a[8] - a[2] * a[6] ) / xDet;
xInv(5)	= ( a[2] * a[3] - a[0] * a[5] ) / xDet;
xInv(6)	= ( a[3] * a[7] - a[4] * a[6] ) / xDet;
xInv(7)	= ( a[1] * a[6] - a[0] * a[7] ) / xDet;
xInv(8)	= ( a[0] * a[4] - a[1] * a[3] ) / xDet;

return	true;	
}

bool swMatInvByLu ( const swdMat &x, swdMat &xInv, double &det )
{
swMatLu LU( x );

if ( LU.isSingular() ){
return false;
}

LU.inv( xInv );
det = LU.det();

return true;
}

//--------------------------------------
//
//--------------------------------------
void swMatErrMsg ( const char* msgStr)
{
//debug version
cerr << msgStr << endl; 
getchar();

//release version
throw runtime_error( msgStr );
}

//--------------------------------------
//
//--------------------------------------
void swMatWarningMsg ( const char* msgStr)
{
//debug version
cerr << msgStr << endl; 
//getchar();
}

//************************************************
// start from S
//************************************************

//--------------------------------------
//
//--------------------------------------
swdMat  swMatSum( const swdMat &x, const int byRowColFlag )
{
if ( swMatIsEmpty(x) ){
swMatWarningMsg( "swMatSum(): x  is an emppty matrix!");
return x;
}

if (byRowColFlag==1) //return a row vector
{
unsigned int n = x.cols();
swdMat y(1, n);
for(unsigned int i=0; i<n; i++){
y(i) = swMatSumValue( x.getCol(i) ) ;
}
return y;
}
else //return a col vector
{
unsigned int m = x.rows();
swdMat y(m, 1);
for(unsigned int i=0; i<m; i++){
y(i) = swMatSumValue( x.getRow(i) ) ;
}
return y;
}
}

//--------------------------------------
//
//--------------------------------------
double  swMatSumValue( const swdMat &x )
{
#if SW_MAT_BOUNDS_CHECK
if ( swMatIsEmpty(x) ){
swMatWarningMsg( "swMatSumValue(): x  is an emppty matrix!");
return SWD_MAT_REALMAX;
}
#endif

const double *px = x.data();
double d = 0.0;
for(unsigned int i=0; i<x.size(); i++){
d += *(px+i);
}
return d;
}

swfMat  swMatSum( const swfMat &x, const int byRowColFlag )
{
if ( swMatIsEmpty(x) ){
swMatWarningMsg( "swMatSum(): AAA-x  is an emppty matrix!");
return x;
}

if (byRowColFlag==1) //return a row vector
{
unsigned int n = x.cols();
swfMat y(1, n);
for(unsigned int i=0; i<n; i++){
y(i) = swMatSumValue( x.getCol(i) ) ;
}
return y;
}
else //return a col vector
{
unsigned int m = x.rows();
swfMat y(m, 1);
for(unsigned int i=0; i<m; i++){
y(i) = swMatSumValue( x.getRow(i) ) ;
}
return y;
}
}

//--------------------------------------
//
//--------------------------------------
float  swMatSumValue( const swfMat &x )
{
#if SW_MAT_BOUNDS_CHECK
if ( swMatIsEmpty(x) ){
swMatWarningMsg( "swMatSumValue(): x  is an emppty matrix!");
return SWF_MAT_REALMAX;
}
#endif

const float *px = x.data();
float d = 0.0;
for(unsigned int i=0; i<x.size(); i++){
d += *(px+i);
}
return d;
}

swiMat  swMatSum( const swiMat &x, const int byRowColFlag )
{
if ( swMatIsEmpty(x) ){
swMatWarningMsg( "swMatSum(): AAA-x  is an emppty matrix!");
return x;
}

if (byRowColFlag==1) //return a row vector
{
unsigned int n = x.cols();
swiMat y(1, n);
for(unsigned int i=0; i<n; i++){
y(i) = swMatSumValue( x.getCol(i) ) ;
}
return y;
}
else //return a col vector
{
unsigned int m = x.rows();
swiMat y(m, 1);
for(unsigned int i=0; i<m; i++){
y(i) = swMatSumValue( x.getRow(i) ) ;
}
return y;
}
}

int  swMatSumValue( const swiMat &x )
{

const int* px = x.data();
int d = 0;
for(unsigned int i=0; i<x.size(); i++){
d += *(px+i);
}
return d;
}

//--------------------------------------
//
//--------------------------------------
swdMat  swMatZeros( const unsigned int m)
{
return swMatZeros(m, m);
}

//--------------------------------------
//
//--------------------------------------
swdMat  swMatZeros( const unsigned int m, const unsigned int n)
{
swdMat x(m, n, 0);
return x;
}

//--------------------------------------
//arithmatic functions
//--------------------------------------
swdMat  swMatExp( swdMat &x )
{
if ( swMatIsEmpty(x) ){
swMatWarningMsg( "swMatExp(): x  is an emppty matrix!");
return x;
}

unsigned int m=x.rows();
unsigned int n=x.cols();
swdMat y(m, n);

const double* px=x.data();
double* py=y.getBuf();
for (unsigned int i=0; i<m*n; i++){
py[i] = exp( (double)px[i] ) ;
}

return y;
}

swdMat  swMatSin( const swdMat &x )
{
if ( swMatIsEmpty(x) ){
swMatWarningMsg( "swMatSin(): x  is an emppty matrix!");
return x;
}

unsigned int m=x.rows();
unsigned int n=x.cols();
swdMat y(m, n);

const double* px=x.data();
double* py=y.getBuf();
for (unsigned int i=0; i<m*n; i++){
py[i] = sin( (double)px[i] ) ;
}

return y;
}

swdMat  swMatCos( const swdMat &x )
{
if ( swMatIsEmpty(x) ){
swMatWarningMsg( "swMatCos(): x  is an emppty matrix!");
return x;
}

unsigned int m=x.rows();
unsigned int n=x.cols();
swdMat y(m, n);

const double* px=x.data();
double* py=y.getBuf();
for (unsigned int i=0; i<m*n; i++){
py[i] = cos( (double) px[i] ) ;
}

return y;
}

swdMat  swMatTan( const swdMat &x )
{
if ( swMatIsEmpty(x) ){
swMatWarningMsg( "swMatTan(): x  is an emppty matrix!");
return x;
}

unsigned int m=x.rows();
unsigned int n=x.cols();
swdMat y(m, n);

const double* px=x.data();
double* py=y.getBuf();
for (unsigned int i=0; i<m*n; i++){
py[i] = tan( (double)px[i] ) ;
}

return y;
}

swdMat  swMatReshape( const swdMat &x, const unsigned int m, const unsigned int n )
{
#if SW_MAT_BOUNDS_CHECK	
if (x.size() != m*n ){
swMatErrMsg("swMatReshape(): size is not match!");
}
#endif

swdMat y(m, n, x.data(), x.size());

return y;
}

void swMatReshape( const swdMat &x, swdMat &y )
{
#if SW_MAT_BOUNDS_CHECK	
if (x.size() != y.cols()*y.rows() ){
swMatErrMsg("swMatReshape(): size is not match!");
}
#endif

y.setData( x.data(), x.size() );
}


//---------------------------------------------------------------------------------
// a Bubble sort alg in Acending order
// x: original vector;
// y: sortted vector
// I: index vector to satisfy: y(i) = X ( I(i) ) 
//--------------------------------------------------------------------------------
void swMatSort( const swfMat &x, swfMat &y, swiMat &I, const bool isDescending )
{
int i, j, idTmp;
float temp;

if ( y.size() != x.size() ){
y.resize( x.rows(), x.cols() );
}
if ( I.size() != x.size() ){
I.resize( x.rows(), x.cols() );
}

int n  = x.size();
y = x;
for (i=0; i<n; i++)
I(i) = i;

//I.print("I=");

for (i = (n - 1); i >= 0; i--){
for (j = 1; j <= i; j++){
if ( y(j-1) > y(j) ){
temp = y(j-1);
idTmp = I(j-1);

y(j-1) = y(j);
I(j-1) = I(j);

y(j) = temp;
I(j) = idTmp;
}
}
}

if ( isDescending ){
swfMat y1 (y);
swiMat I1 (I);
n = y.size();
for (int i=0; i<n; i++){
y(i) = y1(n-1-i);
I(i) = I1(n-1-i);
}
}
}


//---------------------------------------------------------------------------------
// a Bubble sort alg in Acending order
// x: original vector;
// y: sortted vector
// I: index vector to satisfy: y(i) = X ( I(i) ) 
//--------------------------------------------------------------------------------
void swMatSort( const swdMat &x, swdMat &y, swiMat &I, const bool isDescending )
{
int i, j, idTmp;
double temp;

if ( y.size() != x.size() ){
y.resize( x.rows(), x.cols() );
}
if ( I.size() != x.size() ){
I.resize( x.rows(), x.cols() );
}

int n  = x.size();
y = x;
for (i=0; i<n; i++)
I(i) = i;

//I.print("I=");

for (i = (n - 1); i >= 0; i--){
for (j = 1; j <= i; j++){
if ( y(j-1) > y(j) ){
temp = y(j-1);
idTmp = I(j-1);

y(j-1) = y(j);
I(j-1) = I(j);

y(j) = temp;
I(j) = idTmp;
}
}
}

if ( isDescending ){
swdMat y1 (y);
swiMat I1 (I);
n = y.size();
for (int i=0; i<n; i++){
y(i) = y1(n-1-i);
I(i) = I1(n-1-i);
}
}
}


swdMat swMatSelectColEq( const swdMat &x, const int col,  const double checkVal )
{
int m = x.rows();
int n = x.cols();

if ( col < 0 || col >= n ){
swMatErrMsg("swMatSelectColEq(): wrong col idx!");
}

swdMat y( m, n );
int j=0;
for( int i=0; i<m; i++ ){
if ( x(i, col) == checkVal ){
y.setRow( j, x.getRow(i) );
j++;
}
}
y.delRows( j, m-1 );

return y;
}


//find all peaks and sorted by the peak values
//input
// x, 1 x n, vector
//output
// peakValues, 1 x nPeaks, vector
// peakId, 1 x nPeaks, 
void swMatPeakFinder( const swfMat &x, swfMat &peakValues, swiMat &peakIds )
{
//get all peaks
int nX = x.size();
if( nX<3 ){
peakValues.resize(0,0);
peakIds.resize(0,0);
return;
}

int i, j, k;
swfMat peaks(1, nX);
swiMat ids(1, nX);
k=0;
if( x(0) > x(1) ){
peaks(k) = x(0);
ids(k) = 0;
k++;
}

i=1;
while( i < nX-1 ){
if  ( x(i) > x(i-1) && x(i) > x(i+1) ){ 
peaks(k) = x(i);
ids(k) = i;
k++;
}
else if ( x(i) > x(i-1) && x(i) == x(i+1) ){
j=i;
while(1){
i++;
//printf( "%d, %f, %f, %d\n", i, x(i), x(i+1), ( abs(x(i) - x(i+1)) < SWF_MAT_EPS ) );
if ( abs ( x(i) - x(i+1) ) > SWF_MAT_EPS  || i>=nX-1 )
break;
}

if( x(i+1) < x(i) ){
j += i;
j /= 2; 

peaks(k) = x(j);
ids(k) = j;
k++;
}
}
i++;		
}

if( x(nX-1) > x(nX-2) ){
peaks(k) = x(nX-1);
ids(k) = nX-1;
k++;
}

if( k>0 )
peakValues=peaks.getSubRow(0,0,k),
peakIds = ids.getSubRow(0,0,k);
else{
peakValues.resize(0,0);
peakIds.resize(0,0);
}
}

void swMatPeakFinder( const swfMat &x, swfMat &peakValues, swiMat &peakIds, const float threshold )
{
swfMat allPeakValues;
swiMat allPeakIds;

swMatPeakFinder( x, allPeakValues, allPeakIds );

int n = allPeakValues.size();
if ( n > 1 ){
swiMat F ( swMatFindGreaterEq( allPeakValues, threshold * allPeakValues(0) ) );
int n = F.size();

int *p = new int [n];
if (p==NULL){
swMatErrMsg( "swMatPeakFinder(): not enough memory!");
}

for (int i=0; i<n; i++){
p[i] = (int) F(i);
}

peakValues = allPeakValues.getData( p, n );
peakIds = allPeakIds.getData( p, n );
delete [] p;
}
else{
peakValues.setFrom( allPeakValues );
peakIds.setFrom( allPeakIds );
}
}


//---------------------------------------------------------------------
//calculate the SNR (in db) of a given signal at particular peak
//x,		1d vector, x(i)>=0 for all i's
//coeffThd, in (0,1], default 0.6065
//---------------------------------------------------------------------
float swMatSnr( const swfMat &x, const int peakIdx, const float coeffThd,  
int &nSignalBins, int &nTotalBins )
{
int nX = x.size();
if ( peakIdx>=nX || peakIdx<0 ){
swMatErrMsg( "swMatSnr(): input peakId is out of range!"); 
nSignalBins = 0;
nTotalBins = 0;
return -2000.0f;							////20*log10(1e-100)
}

//search signal + noise boundary
int i, iL, iR;
float xMinL, xMinR;
//Left boundary
for( i=peakIdx-1; i>=0; i-- ){
if ( x(i) > x(i+1) ){
break;
}
}
iL = i+1;
xMinL = x(iL);

//Right boundary
for( i=peakIdx+1; i< nX; i++ ){
if ( x(i) > x(i-1) ){
break;
}
}
iR=i-1;
xMinR = x(iR);

//cal.ignal and noise strenth
float dx;
float As(0.0f), A(0.0f);
float xThd =  (  x(peakIdx) - xMinL ) * coeffThd;

nTotalBins = iR-iL+1; 
nSignalBins = 0;
for( i=iL; i<=peakIdx; i++){
dx = x(i)-xMinL;
A += x(i);
if( dx >= xThd ){
nSignalBins++;
As += x(i);
}
}

xThd =  (  x(peakIdx) - xMinR ) * coeffThd;
for( i=peakIdx+1; i<=iR; i++){
dx = x(i)-xMinR;
A += x(i);
if( dx >= xThd ){
nSignalBins++;
As += x(i);
}
}


if ( nSignalBins < 1 ){
swMatErrMsg( "swMatSnr(): input data is invalid!"); 
return -2000.0f;							//20*log10(1e-100)
}
else{
float snr;
if ( nTotalBins == nSignalBins ){			//no noise
snr = 2000.0f;							//20*log10(1e+100)
}
else{
A = (A-As)/(nTotalBins-nSignalBins);	//avearge noise
As = As/nSignalBins;					//average signal
if ( As > 0.0f ){
snr = 20 * log10 ( As/( A+SWF_MAT_EPS ) ) ; //snr in db
}
}
return snr;
}
}


//------------------------------------------------
//cross correlation bewteen signal and mask
//vSignal, nSignal x 1;
//vMask,   nMask   x 1,
//vCorr,    nSignal x 1
//------------------------------------------------
float swMatCrossCorrelation( const swfMat &vSignal, const swfMat &vMask,  swfMat &vCorr )
{
int nSignal = vSignal.size();
int nMask = vMask.size();		//$nMask$ must be odd
assert( nMask%2 == 1 );   

int kk = nMask/2;

//TODO: this is not neccessary to allocate $vPad$, but just for easy programing
swfMat vPad (nSignal+kk*2, 1, 0.0f);
vPad.setSubCol( 0, kk, kk+nSignal-1, vSignal );

if( vCorr.size() != nSignal ){
vCorr.resize(nSignal,1);
}

int k;
float s, sMax=0.0f;
for (int i=0; i<nSignal; i++ ){ //for mask center location
s=0.0;
k=i;
for(int j=0; j<nMask; j++, k++){
s += ( vMask(j)*vPad(k) );
}
vCorr(i) = s;

if ( s>sMax ){
sMax = s;
}
}

return sMax;
}


//h1 and h2 have the same size
//h1 and h2 are normalized, i.e. sum(h1)=sum(h2)=1.0;
float swMatCircularCorrelation( const swfMat &h1, const swfMat &h2 )
{
int i, j, k, n = (int)h1.size();
assert( n == (int)h2.size() );

float roh, rohMax = -1.0f;
for ( k=0;k<n; k++){
roh = 0;
for ( i=0, j=k; i<n-k; ++i, ++j){
roh += sqrt( h1(j)*h2(i) );
}
for ( i=n-k, j=0; i<n; ++i, ++j){
roh += sqrt( h1(j)*h2(i) );
}

if( roh>rohMax ){
rohMax = roh;
}
}
//printf( "rohMax=%f\n", rohMax);
return rohMax;
}


bool swMatSvd2x2 ( const swfMat &x, swfMat &U, swfMat &S, swfMat &V  )
{
swfMat A( x % x.transpose() );
swfMat B( (x.transpose()) % x );

swfMat eigVecA(2,2), eigValA(2,1);
swfMat eigVecB(2,2), eigValB(2,1);

swMatEigen2x2 ( A, eigVecA, eigValA  );
swMatEigen2x2 ( B, eigVecB, eigValB  );

#if _DEBUG
assert( fabs(eigValA(0)-eigValB(0))<1e-6f );
assert( fabs(eigValA(1)-eigValB(1))<1e-6f );
#endif

S = 0.0f;
S(0,0) = eigValA(0);
S(1,1) = eigValA(1);
U = eigVecA;
V = eigVecB.transpose();

#if _DEBUG
x.print("x=");
A = U%S%V.transpose();
A.print("A=");
A -= x;
A.print("A=");
#endif

return true;
}


bool swMatEigen2x2 ( const swfMat &A, swfMat &vec, swfMat &val  )
{
float t = A(0) + A(3);
float t2 = sqrt( t*t - 4*(A(0)*A(3) - A(1)*A(2) ) );

val(0) =  (t + t2 )/2;
val(1) =  (t - t2 )/2;
assert( val(0) > val(1) );

float lamda, x1, x2;
for( int i=0; i<2; i++){
lamda = val(i);
if( fabs(A(3) - lamda)< 1e-6 ){
x2 = 1;
x1 = A(1)/( lamda-A(0) );
}
else{
x1 = 1;
x2 = A(2)/( lamda-A(3) );
}

//normalizing eig. vector
lamda = sqrt( x1*x1 + x2 * x2 );
vec( 0, i ) = x1 / lamda;
vec( 1, i ) = x2  / lamda;
}

#if _DEBUG
swfMat L = A%vec;
swfMat tmp(2,2,0); tmp(0) = val(0); tmp(3) = val(1);	
swfMat R = vec%tmp;
L.print("L=");
R.print("R=");
#endif
return true;
}
