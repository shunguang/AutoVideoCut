/*
*-------------------------------------------------------------------
* swMatTools.cpp
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
#include "swMatTools.h"

using namespace std;

//out = x(:, vIdx)
//if logical=true, vIdx(i) can only be 1 or 0,  otherwise it be the col numbers of x
bool swMatGetCols( const swiMat &x, const swiMat &vIdx, const bool &logical, swiMat &out )
{
unsigned int i, n = vIdx.size();
if ( n > x.cols() ){
#if _DEBUG
assert(0);
#endif
return false;
}

if( logical ){
unsigned int m = swMatSumValue( vIdx );
out.resize( x.rows(), m );
unsigned int k=0;
for( i=0; i<n; ++i ){
if( vIdx(i)!=0 ){
out.setCol(k, x.getCol( i ) );
k++;
}
}
assert(k==m);
}
else{

out.resize( x.rows(), n );
for( i=0; i<n; ++i ){
#if _DEBUG
assert( vIdx(i) < x.cols() );
#endif
out.setCol(i, x.getCol( vIdx(i) ) );
}
}
return true;
}

//out = x(:, vIdx)
//if logical=true, vIdx(i) can only be 1 or 0,  otherwise it be the col numbers of x
bool swMatGetCols( const swfMat &x, const swiMat &vIdx, const bool &logical, swfMat &out )
{
unsigned int i, n = vIdx.size();
if ( n > x.cols() ){
#if _DEBUG
assert(0);
#endif
return false;
}

if( logical ){
unsigned int m = swMatSumValue( vIdx );
out.resize( x.rows(), m );
unsigned int k=0;
for( i=0; i<n; ++i ){
if( vIdx(i)!=0 ){
out.setCol(k, x.getCol( i ) );
k++;
}
}
assert(k==m);
}
else{

out.resize( x.rows(), n );
for( i=0; i<n; ++i ){
#if _DEBUG
assert( vIdx(i) < x.cols() );
#endif
out.setCol(i, x.getCol( vIdx(i) ) );
}
}
return true;
}

bool swMatGetCols( const swdMat &x, const swiMat &vIdx, const bool &logical, swdMat &out )
{
unsigned int i, n = vIdx.size();
if ( n > x.cols() ){
#if _DEBUG
assert(0);
#endif
return false;
}

if( logical ){
unsigned int m = swMatSumValue( vIdx );
out.resize( x.rows(), m );
unsigned int k=0;
for( i=0; i<n; ++i ){
if( vIdx(i)!=0 ){
out.setCol(k, x.getCol( i ) );
k++;
}
}
assert(k==m);
}
else{

out.resize( x.rows(), n );
for( i=0; i<n; ++i ){
#if _DEBUG
assert( vIdx(i) < x.cols() );
#endif
out.setCol(i, x.getCol( vIdx(i) ) );
}
}
return true;
}


//out = x(vIdx, :)
//if logical=true, vIdx(i) can only be 1 or 0, otherwise it be the row numbers of x
bool swMatGetRows( const swiMat &x, const swiMat &vIdx, const bool &logical, swiMat &out )
{
unsigned int i, n = vIdx.size();
if ( n > x.rows() ){
#if _DEBUG
assert(0);
#endif
return false;
}

if( logical ){
unsigned int m = swMatSumValue( vIdx );
out.resize( m, x.cols() );
unsigned int k=0;
for( i=0; i<n; ++i ){
if( vIdx(i)!=0 ){
out.setRow(k, x.getRow( i ) );
k++;
}
}
assert(k==m);
}
else{
out.resize( n, x.cols() );
for( i=0; i<n; ++i ){
#if _DEBUG
assert( vIdx(i) < x.rows() );
#endif
out.setRow(i, x.getRow( vIdx(i) ) );
}
}
return true;
}


//out = x(vIdx, :)
//if logical=true, vIdx(i) can only be 1 or 0, otherwise it be the row numbers of x
bool swMatGetRows( const swfMat &x, const swiMat &vIdx, const bool &logical, swfMat &out )
{
unsigned int i, n = vIdx.size();
if ( n > x.rows() ){
#if _DEBUG
assert(0);
#endif
return false;
}

if( logical ){
unsigned int m = swMatSumValue( vIdx );
out.resize( m, x.cols() );
unsigned int k=0;
for( i=0; i<n; ++i ){
if( vIdx(i)!=0 ){
out.setRow(k, x.getRow( i ) );
k++;
}
}
assert(k==m);
}
else{
out.resize( n, x.cols() );
for( i=0; i<n; ++i ){
#if _DEBUG
assert( vIdx(i) < x.rows() );
#endif
out.setRow(i, x.getRow( vIdx(i) ) );
}
}
return true;
}

bool swMatGetRows( const swdMat &x, const swiMat &vIdx, const bool &logical, swdMat &out )
{
unsigned int i, n = vIdx.size();
if ( n > x.rows() ){
#if _DEBUG
assert(0);
#endif
return false;
}

if( logical ){
unsigned int m = swMatSumValue( vIdx );
out.resize( m, x.cols() );
unsigned int k=0;
for( i=0; i<n; ++i ){
if( vIdx(i)!=0 ){
out.setRow(k, x.getRow( i ) );
k++;
}
}
assert(k==m);
}
else{

out.resize( n, x.cols() );
for( i=0; i<n; ++i ){
#if _DEBUG
assert( vIdx(i) < x.rows() );
#endif
out.setRow(i, x.getRow( vIdx(i) ) );
}
}
return true;
}

//out = x > thd,  $out$ is a logical matrxi with the same size of $x$
int swMatLogicGT( const swfMat &x, const float &thd, swiMat &out )
{
const float	*px = x.data();
int		*po = out.getBuf();
int		n=0;
unsigned int i;
for( i=0; i<x.size(); ++i, ++px, ++po ){
if( *px>thd ){
*po = 1;
++n;
}
else{
*po = 0;
}
}
return n;
}
int swMatLogicGT( const swdMat &x, const double &thd, swiMat &out )
{
const double	*px = x.data();
int		*po = out.getBuf();
int		n=0;
unsigned int i;
for( i=0; i<x.size(); ++i, ++px, ++po ){
if( *px>thd ){
*po = 1;
++n;
}
else{
*po = 0;
}
}
return n;
}

//out = x < thd,  $out$ is a logical matrxi with the same size of $x$
int swMatLogicGE( const swfMat &x, const float &thd, swiMat &out )
{
const float	*px = x.data();
int		*po = out.getBuf();
int		n=0;
unsigned int i;
for( i=0; i<x.size(); ++i, ++px, ++po ){
if( *px<thd ){
*po = 1;
++n;
}
else{
*po = 0;
}
}
return n;
}
int swMatLogicGE( const swdMat &x, const double &thd, swiMat &out )
{
const double	*px = x.data();
int		*po = out.getBuf();
int		n=0;
unsigned int i;
for( i=0; i<x.size(); ++i, ++px, ++po ){
if( *px>=thd ){
*po = 1;
++n;
}
else{
*po = 0;
}
}
return n;
}

//out = x >= thd, $out$ is a logical matrxi with the same size of $x$
int swMatLogicST( const swfMat &x, const float &thd, swiMat &out )
{
const float	*px = x.data();
int		*po = out.getBuf();
int		n=0;
unsigned int i;
for( i=0; i<x.size(); ++i, ++px, ++po ){
if( *px>=thd ){
*po = 1;
++n;
}
else{
*po = 0;
}
}
return n;
}
int swMatLogicST( const swdMat &x, const double &thd, swiMat &out )
{
const double	*px = x.data();
int		*po = out.getBuf();
int		n=0;
unsigned int i;
for( i=0; i<x.size(); ++i, ++px, ++po ){
if( *px<thd ){
*po = 1;
++n;
}
else{
*po = 0;
}
}
return n;
}

//out = x <= thd, $out$ is a logical matrxi with the same size of $x$
int swMatLogicSE( const swfMat &x, const float &thd, swiMat &out )
{
const float	*px = x.data();
int		*po = out.getBuf();
int		n=0;
unsigned int i;
for( i=0; i<x.size(); ++i, ++px, ++po ){
if( *px<=thd ){
*po = 1;
++n;
}
else{
*po = 0;
}
}
return n;
}

int swMatLogicSE( const swdMat &x, const double &thd, swiMat &out )
{
const double	*px = x.data();
int		*po = out.getBuf();
int		n=0;
unsigned int i;
for( i=0; i<x.size(); ++i, ++px, ++po ){
if( *px<=thd ){
*po = 1;
++n;
}
else{
*po = 0;
}
}
return n;
}

int swMatLogicEQ( const swiMat &x, const int &thd, swiMat &out )
{
const int	*px = x.data();
int		*po = out.getBuf();
int		n=0;
unsigned int i;
for( i=0; i<x.size(); ++i, ++px, ++po ){
if( *px==thd ){
*po = 1;
++n;
}
else{
*po = 0;
}
}
return n;
}

int swMatLogicEQ( const swdMat &x, const double &thd, swiMat &out )
{
const double	*px = x.data();
int		*po = out.getBuf();
int		n=0;
unsigned int i;
for( i=0; i<x.size(); ++i, ++px, ++po ){
if( fabs(*px-thd)<1e-9 ){
*po = 1;
++n;
}
else{
*po = 0;
}
}
return n;
}

int swMatLogicAND( const swiMat &x, const swiMat &y, swiMat &out )
{
const int	*px = x.data();
const int	*py = y.data();
int		*po = out.getBuf();
int		n=0;
unsigned int i;
for( i=0; i<x.size(); ++i, ++px, ++py, ++po ){
if( *px!=0 && *py!=0 ){
*po = 1;
++n;
}
else{
*po = 0;
}
}
return n;
}

int swMatLogicOR ( const swiMat &x, const swiMat &y, swiMat &out )
{
const int	*px = x.data();
const int	*py = y.data();
int		*po = out.getBuf();
int		n=0;
unsigned int i;
for( i=0; i<x.size(); ++i, ++px, ++py, ++po ){
if( *px!=0 || *py!=0 ){
*po = 1;
++n;
}
else{
*po = 0;
}
}
return n;
}

//x: m x n, logic matrix
//out: 1 x n (if dimFlag==1) or  m x 1 (dimFlag==2) logic vector
int swMatLogicAll( const swiMat &x, const int &dimFlag, swiMat &out )
{
int mx = x.rows();
int nx = x.cols();
swiMat y = swMatSum(  x, dimFlag );

const int *py = y.data();
int *pOut = out.getBuf();
out = 0;
int n=0;
if ( dimFlag == 1 ){
for	(int i=0; i<nx; ++i, ++py, ++pOut){
if ( *py == mx ){
*pOut = 1;
++n;
}
}
}
else if ( dimFlag==2 ){
for	(int i=0; i<mx; ++i, ++py, ++pOut){
if ( *py == nx ){
*pOut = 1;
++n;
}
}
}
else{
assert(0);
}

return n;
}

bool swMatLogicAnyGT( const swdMat &x, const double &thd )
{
const double *px = x.data();
for( unsigned int i=0; i<x.size(); ++i){
if( *px++ > thd )
return true;
}

return false;
}


bool swMatMin( const swiMat &x, const swiMat &y, swiMat &out)
{
unsigned int n=x.size();

assert(  n == y.size() );
assert(  n == out.size() );
for(unsigned int i=0; i<n; ++i ){
out(i) = ( x(i)> y(i) ) ? y(i) : x(i);
}

return true;
}

bool swMatMin( const swiMat &x, const swiMat &y, swdMat &out)
{
unsigned int n=x.size();

assert(  n == y.size() );
assert(  n == out.size() );
for(unsigned int i=0; i<n; ++i ){
out(i) = ( x(i)> y(i) ) ? y(i) : x(i);
}

return true;
}

void swMatRandperm( const size_t &n, swiMat &x )
{
#if _DEBUG
assert( x.size() >= n );
#endif

unsigned int i, j, t;

for(i=0; i<n; i++)
x(i) = i;

for(i=0; i<n; i++) {
j = rand()%(n-i)+i;
t = x(j);
x(j) = x(i);
x(i) = t;
}
}

void swMatRepMat( const swfMat &x, const unsigned int &m, const unsigned int &n, swfMat &y)
{	
unsigned int mx = x.rows();
unsigned int nx = x.cols();

y.resize( m*mx, n*nx );

unsigned int i,j, r0, c0;
for ( i=0, r0=0; i<m; ++i, r0+=mx ){
for ( j=0, c0=0; j<n; ++j, c0+=nx ){
y.setSlice(r0,c0,mx, nx, x);
}
}
}

void swMatRepMat( const swdMat &x, const unsigned int &m, const unsigned int &n, swdMat &y)
{	
unsigned int mx = x.rows();
unsigned int nx = x.cols();

y.resize( m*mx, n*nx );

unsigned int i,j, r0, c0;
for ( i=0, r0=0; i<m; ++i, r0+=mx ){
for ( j=0, c0=0; j<n; ++j, c0+=nx ){
y.setSlice(r0,c0,mx, nx, x);
}
}
}

//rounds the elements of x to the nearest integers
void swMatRound( const swfMat &x, swiMat &y )
{
#if _DEBUG
assert( y.size() == x.size() );
#endif

for( unsigned int i=0; i<x.size(); ++i){
y(i) = SW_MAT_ROUND( x(i) );
}
}

void swMatRound( const swdMat &x, swiMat &y )
{
#if _DEBUG
assert( y.size() == x.size() );
#endif

for( unsigned int i=0; i<x.size(); ++i){
y(i) = SW_MAT_ROUND( x(i) );
}
}


int   swMatCalNumElements( const float &x1, const float &dx, const float &x2 )
{
int n = (int) ( floor( (x2-x1)/dx ) + 1 );

assert( (x1 + (n-1)*dx) <= x2 ) ;

return n;
}

// find max(m) such that x1+m*dx <= x2
// n = m+1;
int   swMatCalNumElements( const double &x1, const double &dx, const double &x2 )
{
int m = (int) ( floor(x2-x1)/dx );

#if _DEBUG	
assert( (x1 + m*dx) <= x2 ) ;
#endif

return m+1;
}

void  swMatGenVec( const float &x1, const float &dx, const float &x2, swfMat &v )
{
int n = swMatCalNumElements(x1,dx,x2);
v.resize(n,1);
float x = x1;
for( int i=0; i<n; ++i ){
v(i) = x;
x += dx;
}
}

void  swMatGenVec( const double &x1, const double &dx, const double &x2, swdMat &v )
{
int n = swMatCalNumElements( x1, dx, x2);
v.resize(n,1);
double x = x1;
for( int i=0; i<n; ++i ){
v(i) = x;
x += dx;
}
}

void  swMatGenVec( const double &x1, const double &dx, const double &x2, swiMat &v )
{
int n = swMatCalNumElements( x1, dx, x2);
v.resize(n,1);
double x = x1;
for( int i=0; i<n; ++i ){
v(i) = SW_MAT_ROUND( x );
x += dx;
}
}

void swMatTwoTuples( const swiMat &x, const swiMat &y, swiMat &z )
{
unsigned int nx = x.size();
unsigned int ny = y.size();
unsigned int nz = nx*ny;
unsigned int two=2;

z.resize( nz, two );
int k=0;
const int *px = x.data();
for( unsigned int i=0; i<nx; ++i, ++px){
const int *py = y.data();
for( unsigned int j=0; j<ny; ++j, ++py){
z(k,0) = *px;
z(k,1) = *py;
k++;
}
}

assert( k==nz );
}

void swMatTwoTuples( const swfMat &x, const swfMat &y, swfMat &z )
{
unsigned int nx = x.size();
unsigned int ny = y.size();
unsigned int nz = nx*ny;
unsigned int two=2;

z.resize( nz, two );
int k=0;
const float *px = x.data();
for( unsigned int i=0; i<nx; ++i, ++px){
const float *py = y.data();
for( unsigned int j=0; j<ny; ++j, ++py){
z(k,0) = *px;
z(k,1) = *py;
k++;
}
}

assert( k==nz );
}

void swMatTwoTuples( const swdMat &x, const swdMat &y, swdMat &z )
{
unsigned int nx = x.size();
unsigned int ny = y.size();
unsigned int nz = nx*ny;
unsigned int two=2;

z.resize( nz, two );
int k=0;
const double *px = x.data();
for( unsigned int i=0; i<nx; ++i, ++px){
const double *py = y.data();
for( unsigned int j=0; j<ny; ++j, ++py){
z(k,0) = *px;
z(k,1) = *py;
k++;
}
}

assert( k==nz );
}


//x: n x dim;
//d: 1 x (n*n-1)/2)
void swMatPdistEuclidean( const swdMat &x, swdMat &d )
{
int n = x.rows();
int m = n*(n-1)/2;
int dim = x.cols();

d.resize(1, m);

int k=0;
double s, dx;
const double *pi = x.data();
for( int i=0; i<n; ++i, pi += dim ){

const double *pj = pi+dim;
for( int j=i+1; j<n; ++j, pj+=dim ){

s=0;
for( int ii=0; ii<dim; ++ii ){
dx = pi[ii] - pj[ii];
s += (dx*dx);
}

d(k) = sqrt(s);
k++;
}
}


}

//return the unique element of input $x$
swiMat swMatUnique( const swiMat &x )
{
const int *myints = x.data();
int L = x.size();
vector<int> myvector (L);
vector<int>::iterator it;

// using default comparison:
it=unique_copy (myints,myints+L,myvector.begin());
sort (myvector.begin(),it);

// using predicate comparison:
it=unique_copy (myvector.begin(), it, myvector.begin() );

int n = it - myvector.begin();

swiMat out(n,1);
for( int i=0; i<n; ++i){
out(i) = myvector[i];
}

out.print("out=");

return out;
}

void my_text_writer2 ( const swdMat &x, const std::string &path, const std::string &fname_wo_ext, const std::string &fmt, const int &fn, const int &idx )
{

int m =  x.rows();
int n =  x.cols();

char fileName[1024];
if( idx<0 )
sprintf_s( fileName, 1024, "%s\\%s-%05d.txt", path.c_str(), fname_wo_ext.c_str(), fn );
else
sprintf_s( fileName, 1024, "%s\\%s-%05d-%05d.txt", path.c_str(), fname_wo_ext.c_str(), fn, idx );

std::string fmt1 = fmt + ",";
std::string fmt2 = fmt + "\n";

FILE *fp = fopen( fileName, "w" );
if( fp == NULL ) {
printf("my_text_writer2(): file: \n %s \n  cannot be opened!\n", fileName);
return;
}
const double *px = x.data();
int i,j;
fprintf(fp, "%d,%d\n", m, n );
for (j=0; j<m; ++j){  //row
for (i=0; i<n-1; ++i){  //col
fprintf( fp, fmt1.c_str(), *px++ );
}
fprintf( fp, fmt2.c_str(), *px++ );
}

fclose (fp );
}

void my_text_writer2 ( const swiMat &x, const std::string &path, const std::string &fname_wo_ext, const std::string &fmt, const int &fn, const int &idx )
{

int m =  x.rows();
int n =  x.cols();

char fileName[1024];
if( idx<0 )
sprintf_s( fileName, 1024, "%s\\%s-%05d.txt", path.c_str(), fname_wo_ext.c_str(), fn );
else
sprintf_s( fileName, 1024, "%s\\%s-%05d-%05d.txt", path.c_str(), fname_wo_ext.c_str(), fn, idx );

std::string fmt1 = fmt + ",";
std::string fmt2 = fmt + "\n";

FILE *fp = fopen( fileName, "w" );
if( fp == NULL ) {
printf("my_text_writer2(): file: \n %s \n  cannot be opened!\n", fileName);
return;
}
const int *px = x.data();
int i,j;
fprintf(fp, "%d,%d\n", m, n );
for (j=0; j<m; ++j){  //row
for (i=0; i<n-1; ++i){  //col
fprintf( fp, fmt1.c_str(), *px++ );
}
fprintf( fp, fmt2.c_str(), *px++ );
}

fclose (fp );
}


void swMatD2I( const swdMat &x, swiMat &y )
{
assert( y.size() == x.size() );
const double *px = x.data();
int *py = y.getBuf();
for (int i=0; i<x.size(); ++i){
*py++ = (int)*px++;
}
}


template <typename Iterator>
bool next_combination(const Iterator first, Iterator k, const Iterator last)
{
/* Credits: Thomas Draper */
if ((first == last) || (first == k) || (last == k)){
return false;
}

Iterator itr1 = first;
Iterator itr2 = last;
++itr1;
if (last == itr1)
return false;
itr1 = last;
--itr1;
itr1 = k;
--itr2;
while (first != itr1)
{
if (*--itr1 < *itr2)
{
Iterator j = k;
while (!(*itr1 < *j)) ++j;
std::iter_swap(itr1,j);
++itr1;
++j;
itr2 = k;
std::rotate(itr1,j,last);
while (last != j)
{
++j;
++itr2;
}
std::rotate(k,itr2,last);
return true;
}
}
std::rotate(first,k,last);
return false;
}

//todo: unsafe for large numbers
size_t swMat_n_choose_k( size_t n, size_t k )
{
if (k > n) return 0;
if (k * 2 > n) k = n-k;
if (k == 0) return 1;

size_t result = n;
for( int i = 2; i <= k; ++i ) {
result *= (n-i+1);
result /= i;
}
return result;
}


void swMat_n_choose_k_pattern( const size_t n, const size_t k,  const std::vector<size_t> &in, swiMat &out )
{
size_t m = swMat_n_choose_k( n, k );
out.resize( m, k );

std::vector<size_t> ints(in);
size_t j = 0;
do{
for (int i = 0; i < k; ++i)
{
out(j,i) = ints[i];

//std::cout << ints[i];
}
//std::cout << "\n";

++j;
} while(next_combination(ints.begin(),ints.begin() + k,ints.end()));

assert( m == j );
}
