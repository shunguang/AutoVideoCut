/*
*-------------------------------------------------------------------
* swdMatSvd.cpp
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
#include "swdMatSvd.h"

// Factor X = U*S*V', X.rows() >= X.cols();
swdMatSvd :: swdMatSvd( const swdMat &X ):
m_m ( X.rows()+1 ),
m_n ( X.cols()+1 ),
m_U (m_m, m_n),  //also store U part when return
m_S (m_n, m_n, 0),  //store S
m_V (m_n, m_n)   //store V
{

m_U.setSlice(1, 1, m_m-1, m_n-1, X); //copy X into A


m_pptrU	= new double* [ m_m ];
m_pptrV	= new double* [ m_n ];
m_ptrW  = new double  [ m_n ];

m_U.data( m_pptrU );
m_V.data( m_pptrV );

//print( m_pptrU, m_m, m_n);

svdcmp( m_pptrU, m_m-1, m_n-1, m_ptrW, m_pptrV);

m_S.setDiagonal( m_ptrW, m_n );
}

swdMatSvd :: ~swdMatSvd()
{
if ( m_pptrU )
delete [] m_pptrU;

if ( m_ptrW )
delete [] m_ptrW ;

if ( m_pptrV )
delete [] m_pptrV;
}

int swdMatSvd :: rank() const
{
return rank( SWD_MAT_EPS );
}
int swdMatSvd :: rank( const double tol ) const
{
int n=0;
for (unsigned int i=1; i<m_n; i++)
if ( m_ptrW[i] > tol )
n++;

return n;
}

void swdMatSvd :: svdcmp(double **a, int m, int n, double *w, double **v)
{
int flag,i,its,j,jj,k,l,nm;
double c,f,h,s,x,y,z;
double anorm=0.0,g=0.0,scale=0.0;
double *rv1;

if (m < n) errMsg("swdMatSvd :: svdcmp():  You must augment A with extra zero rows");

rv1 =  new double [n+1];
if ( !rv1 ) errMsg("swdMatSvd :: svdcmp():  allocate memory fail!");

for (i=1;i<=n;i++) {
l=i+1;
rv1[i]=scale*g;
g=s=scale=0.0;
if (i <= m) {
for (k=i;k<=m;k++) scale += fabs(a[k][i]);
if (scale) {
for (k=i;k<=m;k++) {
a[k][i] /= scale;
s += a[k][i]*a[k][i];
}
f=a[i][i];
g = -SIGN(sqrt(s),f);
h=f*g-s;
a[i][i]=f-g;
if (i != n) {
for (j=l;j<=n;j++) {
for (s=0.0,k=i;k<=m;k++) s += a[k][i]*a[k][j];
f=s/h;
for (k=i;k<=m;k++) a[k][j] += f*a[k][i];
}
}
for (k=i;k<=m;k++) a[k][i] *= scale;
}
}
w[i]=scale*g;
g=s=scale=0.0;
if (i <= m && i != n) {
for (k=l;k<=n;k++) scale += fabs(a[i][k]);
if (scale) {
for (k=l;k<=n;k++) {
a[i][k] /= scale;
s += a[i][k]*a[i][k];
}
f=a[i][l];
g = -SIGN(sqrt(s),f);
h=f*g-s;
a[i][l]=f-g;
for (k=l;k<=n;k++) rv1[k]=a[i][k]/h;
if (i != m) {
for (j=l;j<=m;j++) {
for (s=0.0,k=l;k<=n;k++) s += a[j][k]*a[i][k];
for (k=l;k<=n;k++) a[j][k] += s*rv1[k];
}
}
for (k=l;k<=n;k++) a[i][k] *= scale;
}
}
anorm=MAX(anorm,(fabs(w[i])+fabs(rv1[i])));
}
for (i=n;i>=1;i--) {
if (i < n) {
if (g) {
for (j=l;j<=n;j++)
v[j][i]=(a[i][j]/a[i][l])/g;
for (j=l;j<=n;j++) {
for (s=0.0,k=l;k<=n;k++) s += a[i][k]*v[k][j];
for (k=l;k<=n;k++) v[k][j] += s*v[k][i];
}
}
for (j=l;j<=n;j++) v[i][j]=v[j][i]=0.0;
}
v[i][i]=1.0;
g=rv1[i];
l=i;
}
for (i=n;i>=1;i--) {
l=i+1;
g=w[i];
if (i < n)
for (j=l;j<=n;j++) a[i][j]=0.0;
if (g) {
g=1.0/g;
if (i != n) {
for (j=l;j<=n;j++) {
for (s=0.0,k=l;k<=m;k++) s += a[k][i]*a[k][j];
f=(s/a[i][i])*g;
for (k=i;k<=m;k++) a[k][j] += f*a[k][i];
}
}
for (j=i;j<=m;j++) a[j][i] *= g;
} else {
for (j=i;j<=m;j++) a[j][i]=0.0;
}
++a[i][i];
}
for (k=n;k>=1;k--) {
for (its=1;its<=300;its++) {
flag=1;
for (l=k;l>=1;l--) {
nm=l-1;
if (fabs(rv1[l])+anorm == anorm) {
flag=0;
break;
}
if (fabs(w[nm])+anorm == anorm) break;
}
if (flag) {
c=0.0;
s=1.0;
for (i=l;i<=k;i++) {
f=s*rv1[i];
if (fabs(f)+anorm != anorm) {
g=w[i];
h=PYTHAG(f,g);
w[i]=h;
h=1.0/h;
c=g*h;
s=(-f*h);
for (j=1;j<=m;j++) {
y=a[j][nm];
z=a[j][i];
a[j][nm]=y*c+z*s;
a[j][i]=z*c-y*s;
}
}
}
}
z=w[k];
if (l == k) {
if (z < 0.0) {
w[k] = -z;
for (j=1;j<=n;j++) v[j][k]=(-v[j][k]);
}
break;
}
if (its == 300) errMsg("swdMatSvd :: svdcmp(): No convergence in 300 SVDCMP iterations");
x=w[l];
nm=k-1;
y=w[nm];
g=rv1[nm];
h=rv1[k];
f=((y-z)*(y+z)+(g-h)*(g+h))/(2.0*h*y);
g=PYTHAG(f,1.0);
f=((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
c=s=1.0;
for (j=l;j<=nm;j++) {
i=j+1;
g=rv1[i];
y=w[i];
h=s*g;
g=c*g;
z=PYTHAG(f,h);
rv1[j]=z;
c=f/z;
s=h/z;
f=x*c+g*s;
g=g*c-x*s;
h=y*s;
y=y*c;
for (jj=1;jj<=n;jj++) {
x=v[jj][j];
z=v[jj][i];
v[jj][j]=x*c+z*s;
v[jj][i]=z*c-x*s;
}
z=PYTHAG(f,h);
w[j]=z;
if (z) {
z=1.0/z;
c=f*z;
s=h*z;
}
f=(c*g)+(s*y);
x=(c*y)-(s*g);
for (jj=1;jj<=m;jj++) {
y=a[jj][j];
z=a[jj][i];
a[jj][j]=y*c+z*s;
a[jj][i]=z*c-y*s;
}
}
rv1[l]=0.0;
rv1[k]=f;
w[k]=x;
}
}

if ( rv1 )
delete [] rv1 ;
}

static double at,bt,ct;


//computes (a^2 + b^2)^(1/2) without destructive underflow or overflow 
double swdMatSvd :: PYTHAG( const double a, const double b )
{
double at,bt, ct;

at=fabs(a);
bt=fabs(b);

if (at > bt){
ct=bt/at;
return  at*sqrt(1.0+ct*ct);  
}
else if	(bt > 0.0){
ct = at/bt;
return bt*sqrt(1.0+ct*ct);
}
else
return 0.0;
}

double swdMatSvd :: MAX( const double a, const double b )
{
return ( (a > b) ? a : b ) ;

}
double swdMatSvd :: SIGN( const double a, const double b )
{
return ( (b >= 0.0) ? fabs(a) : -fabs(a)  );
}

void swdMatSvd :: errMsg( const char *msgStr)
{
cout << msgStr << endl;
getchar();
//throw runtime_error( msgStr );
}


//this function is from numerical recipe
void swdMatSvd :: print( double **a, const int m, const int n)
{
cout << m <<" x " <<n << endl;
for(int i=0; i<m; i++){
for(int j=0; j<n; j++)
cout << a[i][j] << "   ";
cout<<endl;
}
}
