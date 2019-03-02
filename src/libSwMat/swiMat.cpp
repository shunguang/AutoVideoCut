/*
*-------------------------------------------------------------------
* swiMat.cpp
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
#include "swiMat.h"
using namespace std;

swiMat::swiMat( const unsigned int r, const unsigned int c ):
m_buf( NULL ),
m_rows( r ),
m_cols( c ),
m_size ( r * c )
{
if ( m_size > 0 )
creatBuf();
}


swiMat::swiMat(const unsigned int r, const unsigned int c, const int initValue):
m_buf( NULL ),
m_rows( r ),
m_cols( c ),
m_size ( r * c )
{
creatBuf();

for (unsigned int k=0; k<m_size; k++)
m_buf[k] = initValue;
}

swiMat::swiMat( const unsigned int r, const unsigned int c, const int startVal, const int step ):
m_buf( NULL ),
m_rows( r ),
m_cols( c ),
m_size ( r * c )
{
creatBuf();
int v = startVal;
for (unsigned int k=0; k<m_size; k++){
m_buf[k] = v;
v += step;
}
}

// constructor from a buffer
swiMat::swiMat( const unsigned int r, const unsigned int c, const int *buf, const unsigned int bufLength ):
m_buf( NULL ),
m_rows( r ),
m_cols( c ),
m_size ( r * c )
{
// NOTE: since we do not know the length of buf, there is a pentential risk to use this function
if (  bufLength != m_size ){
errMsg( "Warning! swiMat::swiMat(): the input size not match the buffer size." );
}

creatBuf();

for ( unsigned int k=0; k<m_size; k++ )
m_buf[k] = buf[k];
}

// constructor from a matrix
swiMat::swiMat( const swiMat &x ):
m_buf( NULL ),
m_rows( x.rows() ),
m_cols( x.cols() ),
m_size( x.size() )
{
creatBuf();

for( unsigned int k=0; k<m_size; k++ )
m_buf[k] = x.m_buf[k];
}

swiMat::swiMat(const unsigned int nCols, const char* fileName):
m_buf( NULL ),
m_rows( 1 ),
m_cols( nCols ),
m_size( nCols )
{
creatBuf();
readFromFile( nCols, fileName );
}


swiMat::~swiMat() 
{ 
deleteBuf(); 
}


int* swiMat :: getBuf()
{
return m_buf; 
}

int*  swiMat :: getAddress( const unsigned int row, const unsigned int col )
{
return m_buf + row*m_cols + col;
}

//asign the data of matrix to a int point
void swiMat :: data( int **pp )
{
for( unsigned int i=0; i<m_rows; i++) 
*(pp+i) = &m_buf[ i * m_cols];
}

unsigned int swiMat :: cols() const 
{ 
return m_cols; 
}

unsigned int swiMat :: rows() const 
{ 
return m_rows; 
}  //get # of rows

const int* swiMat :: data() const 
{
return m_buf; 
}

unsigned int swiMat :: size() const 
{ 
return m_size; 
}

void swiMat :: print(const char* str) const
{
if( m_rows==1 || m_cols==1 ){
printf("%s %d x %d [\n", str, m_rows, m_cols); 
}
else{
printf("%s\n", str);
printf("%d x %d [\n", m_rows, m_cols); 
}

for ( unsigned int i=0; i<m_rows; i++ ){
for( unsigned int j=0; j<m_cols-1; j++ )
printf("%d, ", m_buf[i*m_cols + j] );

printf("%d", m_buf[ (i+1)*m_cols -1 ] );
if ( i < m_rows-1 )
printf(";\n");
}

printf("];\n"); 
}

//change the size, donot keep the previous value
void swiMat::resize( const unsigned int newRows, const unsigned int newCols )
{
if ( m_size != newRows * newCols ) { //reallocate memory, uninitinalized 
deleteBuf();
m_size = newRows * newCols;
m_rows = newRows;
m_cols = newCols;
creatBuf();
}
else { //leave the original value there, 
m_size = newRows * newCols;
m_rows = newRows;
m_cols = newCols;
}
}

void swiMat::resize( const unsigned int newRows, const unsigned int newCols, const int val )
{
resize( newRows, newCols );

for (unsigned int k=0; k<m_size; k++)
m_buf[k] = val;

}

//keep the values, just change the shape
void swiMat::reshape( const unsigned int newRows, const unsigned int newCols )
{
#if SW_MAT_BOUNDS_CHECK
if ( m_size != newRows * newCols ) { 
errMsg( "Warning: swiMat::reshape() -- input parameters are not match the orioginal ones!" );
}
#endif

// leave the original value there, now they are stored still in the order of original (raw by raw)
m_rows = newRows;
m_cols = newCols;
}


//-----------------------------------------
// public operator overloads
//-----------------------------------------
//overload assignment operator
//const return avoid: ( a1=a2 ) = a3
swiMat &swiMat::operator =(const swiMat &x) //assignment
{
if (&x != this ){ //check for self assignment
if ( m_size != x.size() ){
deleteBuf();
m_size = x.size();
creatBuf();
}

m_rows = x.rows();
m_cols = x.cols();
for( unsigned int k=0; k<m_size; k++ )
m_buf[k] = x.m_buf[k];
}

return *this; // enables x=y=z;
}

swiMat &swiMat::operator =(const int d) //assignment
{
for( unsigned int k=0; k<m_size; k++ )
m_buf[k] = d;

return *this;
}

bool swiMat::operator==( const swiMat &x) const
{
if ( x.size() != m_size )
return false;

for( unsigned int k=0; k<m_size; k++)
if (m_buf[k] != x.m_buf[k])
return false;

return true;
}

bool swiMat::operator!=( const swiMat &x) const
{
return ( ! ( *this == x ) );
}

swiMat swiMat::operator-() const
{
swiMat tmp(m_rows, m_cols);
for( unsigned int k=0; k<m_size; k++ )
tmp.m_buf[k] = -m_buf[k];

return tmp;
}

//a+x  add by element
swiMat swiMat::operator +(const swiMat &x) const
{
#if SW_MAT_BOUNDS_CHECK
if ( this->m_rows != x.rows() || this->m_cols != x.cols() )
errMsg( "swiMat::operator +(const swiMat &x): size is not match!" );
#endif

swiMat tmp = *this;
for ( unsigned int k=0; k<this->m_size; k++ )
tmp.m_buf[k] += x.m_buf[k];

return tmp;
}

//a+scale  add by element
swiMat swiMat::operator +(const int scale) const
{
swiMat tmp = *this;
for ( unsigned int k=0; k<m_size; k++ )
tmp.m_buf[k] += scale;

return tmp;
}

//a-x  minus by element
swiMat swiMat::operator -(const swiMat &x) const
{
#if SW_MAT_BOUNDS_CHECK
if ( this->m_rows != x.rows() || this->m_cols != x.cols() )
errMsg( "swiMat::operator -(const swiMat &x): size is not match!" );
#endif

swiMat tmp = *this;
for ( unsigned int k = 0; k < this->m_size; k++ )
tmp.m_buf[k] -= x.m_buf[k];

return tmp;
}

//a-scale  minus by scale
swiMat swiMat::operator -(const int scale) const
{
swiMat tmp = *this;
for ( unsigned int k=0; k<this->m_size; k++ )
tmp.m_buf[k] -= scale;

return tmp;
}

//a*x  time by element
swiMat swiMat::operator *(const swiMat &x) const
{
#if SW_MAT_BOUNDS_CHECK
if ( this->m_rows != x.rows() || this->m_cols != x.cols() )
errMsg( "swiMat::operator *(const swiMat &x): size is not match!" );
#endif

swiMat tmp = *this;
for ( unsigned int k=0; k<this->m_size; k++ )
tmp.m_buf[k] *= x.m_buf[k];

return tmp;
}

//a*scale  add by element
swiMat swiMat::operator *(const int scale) const
{
swiMat tmp = *this;
for ( unsigned int k=0; k<this->m_size; k++ )
tmp.m_buf[k] *= scale;

return tmp;
}

//a/x  add by element
swiMat swiMat::operator /(const swiMat &x) const
{
#if SW_MAT_BOUNDS_CHECK
if ( this->m_rows != x.rows() || this->m_cols != x.cols() )
errMsg( "swiMat::operator +(const swiMat &x): size is not match!" );
#endif

swiMat tmp = *this;
for ( unsigned int k=0; k<this->m_size; k++ )
tmp.m_buf[k] /= x.m_buf[k];

return tmp;
}

//a/scale  add by element
swiMat swiMat::operator /(const int scale) const
{
swiMat tmp = *this;
for ( unsigned int k=0; k<this->m_size; k++ )
tmp.m_buf[k] /= scale;

return tmp;
}

// matrix production  A % X
swiMat swiMat::operator %(const swiMat& x) const
{
#if SW_MAT_BOUNDS_CHECK
if ( this->cols() != x.rows() )
errMsg( "swiMat::operator %(const swiMat &x): size is not match!" );
#endif

unsigned int m = this->rows();
unsigned int n = this->cols();
unsigned int k = x.cols();

swiMat tmp( m, k, 0);
for ( unsigned int i=0; i<m; i++)
{
for ( unsigned int j=0; j<k; j++)
{
for ( unsigned int l=0; l<n; l++) 
tmp.m_buf[ i*k + j ] += ( this->m_buf[i*n + l] * x.m_buf[l*k + j] );
}
}

return tmp;
}

int swiMat :: operator()(const unsigned int i, const unsigned int j) const
{
#if SW_MAT_BOUNDS_CHECK
if ( i<0 || i>=this->m_rows || j<0 || j>=this->m_cols ){
errMsg( "swiMat::operator(): subscript out of bounds!" );
}
#endif

return this->m_buf[i*m_cols + j];
}


int& swiMat :: operator()(const unsigned int i, const unsigned int j)
{
#if SW_MAT_BOUNDS_CHECK
if ( i<0 || i>=this->m_rows || j<0 || j>=this->m_cols ){
errMsg( "swiMat::operator(): subscript out of bounds!" );
}
#endif

return this->m_buf[i*m_cols + j];
}

int swiMat :: operator()(const unsigned int k) const
{
#if SW_MAT_BOUNDS_CHECK
if ( k<0 || k>=(this->m_rows * this-> m_cols) ){
errMsg( "swiMat::operator(): subscript out of bounds!" );
}
#endif

return this->m_buf[k];
}

int &swiMat :: operator()(const unsigned int k)
{
#if SW_MAT_BOUNDS_CHECK
if ( k<0 || k>=(this->m_rows * this-> m_cols) ){
errMsg( "swiMat::operator(): subscript out of bounds!" );
}
#endif

return this->m_buf[k];
}

swiMat& swiMat :: operator +=( const swiMat &x)
{
#if SW_MAT_BOUNDS_CHECK
if ( this->m_rows != x.m_rows || this->m_cols != x.m_cols )
errMsg("Error! swiMat :: operator +=() size is not the same.");
#endif

for(unsigned int k=0; k<this->m_size; k++)
this->m_buf[k] += x.m_buf[k];

return *this;
}

swiMat& swiMat :: operator +=( const int scale)
{
for(unsigned int k=0; k<m_size; k++)
m_buf[k] += scale;

return *this;
}

swiMat& swiMat :: operator -=( const swiMat &x)
{
#if SW_MAT_BOUNDS_CHECK
if ( m_rows != x.m_rows || m_cols != x.m_cols )
errMsg("Error! swiMat :: operator -=() size is not the same.");
#endif

for(unsigned int k=0; k<m_size; k++)
m_buf[k] -= x.m_buf[k];

return *this;
}

swiMat& swiMat :: operator -=( const int scale)
{
for(unsigned int k=0; k<m_size; k++)
m_buf[k] -= scale;

return *this;
}

swiMat& swiMat :: operator *=( const swiMat &x)
{
#if SW_MAT_BOUNDS_CHECK
if ( m_rows != x.m_rows || m_cols != x.m_cols )
errMsg("Error! swiMat :: operator -=() size is not the same.");
#endif

for(unsigned int k=0; k<m_size; k++)
m_buf[k] *= x.m_buf[k];

return *this;
}

swiMat& swiMat :: operator *=( const int scale)
{
for(unsigned int k=0; k<m_size; k++)
m_buf[k] *= scale;

return *this;
}

swiMat& swiMat :: operator /=( const swiMat &x)
{
#if SW_MAT_BOUNDS_CHECK
if ( m_rows != x.m_rows || m_cols != x.m_cols )
errMsg("Error! swiMat :: operator -=() size is not the same.");
#endif

for(unsigned int k=0; k<m_size; k++)
m_buf[k] = m_buf[k] / x.m_buf[k];

return *this;
}

swiMat& swiMat :: operator /=( const int scale)
{
for(unsigned int k=0; k<m_size; k++)
m_buf[k] = m_buf[k] / scale;

return *this;
}

//--------------------------------------
// now the friend functions
//--------------------------------------
ostream& operator<<( ostream &os, swiMat &x )
{
x.print();
return os;
}

// 5+x
swiMat operator+( const int scale, const swiMat &x )
{
return (x + scale) ;
}

// 5-x
swiMat   operator-( const int scale, const swiMat &x )
{
return (-x + scale) ;
}

// 5*x
swiMat   operator*( const int scale, const swiMat &x ) 
{
return (x*scale) ;
}

int swiMat :: trace() const
{
int tr = 0;
for (unsigned int i=0; i<m_rows; i++)
tr += m_buf[i*m_cols + i ];

return tr;
}

void swiMat ::  setCol( const unsigned int c, const int val )
{
#if SW_MAT_BOUNDS_CHECK
if ( c<0 || c>= m_cols )
errMsg( "swiMat ::  setCol(): Column number out of order!" );
#endif

for (unsigned int i=0; i<m_rows; i++)
m_buf[i*m_cols + c ] = val;
}

// set the col elements as vals stored in buf[] 
void swiMat :: setCol( const unsigned int c, const int* buf, const unsigned int bufSize )
{
#if SW_MAT_BOUNDS_CHECK
if ( c<0 || c>= m_cols )
errMsg( "swiMat ::  setCol(): Column number out of order!" );

if ( bufSize <  m_rows )
errMsg( "swiMat ::  setCol(): not enough data in buf[]!" );
#endif

for (unsigned int i=0; i<m_rows; i++)
m_buf[i*m_cols + c ] = buf[i];
}

void swiMat ::  setCol( const unsigned int c, const swiMat &x )
{
#if SW_MAT_BOUNDS_CHECK
if ( c<0 || c>= m_cols )
errMsg( "swiMat ::  setCol(): Column number out of order!" );
#endif

for (unsigned int i=0; i<m_rows; i++)
m_buf[i*m_cols + c ] = x.m_buf[i];
}

void  swiMat :: setSubCol( const unsigned int c, const unsigned int r0, const unsigned int r1, const int val)
{
#if SW_MAT_BOUNDS_CHECK
if ( c<0 || c>= m_cols )
errMsg( "swiMat ::  setSubCol(): Column number out of order!" );

if ( r0<0 || r1>= m_rows )
errMsg( "swiMat ::  setSubCol(): Row range out of order!" );
#endif

for (unsigned int i=r0; i<=r1; i++)
m_buf[i*m_cols + c ] = val;

}

void  swiMat :: setSubCol( const unsigned int c, const unsigned int r0, const unsigned int r1, const swiMat &x)
{
#if SW_MAT_BOUNDS_CHECK
if ( c<0 || c >= m_cols )
errMsg( "swiMat ::  setSubCol(): Column number out of order!" );

if ( r0<0 || r1 >= m_rows )
errMsg( "swiMat ::  setSubCol(): Row range out of order!" );

if ( x.size() < r1-r0+1 )
errMsg( "swiMat ::  setSubCol(): not enough data in x!" );
#endif

unsigned int k=0;
for (unsigned int i=r0; i<=r1; i++){
m_buf[i*m_cols + c] = x.m_buf[k];
k++;
}
}

void swiMat :: setData( const int *buf, const unsigned int bufSize )
{
unsigned int n = (bufSize > m_size) ? m_size : bufSize ;

for (unsigned int i=0; i<n; i++)
m_buf[i] = buf[i];
}

void swiMat :: setDiagonal( const int d)
{
for (unsigned int i=0; i<m_rows; i++)
m_buf[i*m_cols + i ] = d;
}

void swiMat :: setDiagonal( const swiMat &d)
{
#if SW_MAT_BOUNDS_CHECK
if ( d.m_rows > 1 && d.m_cols > 1)
errMsg( "swiMat :: setDiaganol() d is not a vector!");
#endif

for (unsigned int i=0; i<m_rows; i++)
m_buf[i*m_cols + i ] = d.m_buf[i];
}

void swiMat :: setDiagonal( const int *buf, const unsigned int nBufSize )
{
#if SW_MAT_BOUNDS_CHECK
if ( m_rows > nBufSize )
errMsg( "swiMat :: setDiaganol() buffer size is too small!");
#endif

for (unsigned int i=0; i<m_rows; i++)
m_buf[i*m_cols + i ] = buf[i];
}

void swiMat :: setRow( const unsigned int r, const int val )
{
#if SW_MAT_BOUNDS_CHECK
if ( r<0 || r>= m_rows )
errMsg( "swiMat ::  setRow(): row number out of order!" );
#endif

for (unsigned int i=0; i<m_cols; i++)
m_buf[r*m_cols + i ] = val;

}

void swiMat :: setRow( const unsigned int r, const swiMat &x )
{
#if SW_MAT_BOUNDS_CHECK
if ( r<0 || r>= m_rows )
errMsg( "swiMat ::  setRow(): row number out of order!" );

if ( x.size() < m_cols )
errMsg( "swiMat ::  setRow(): the size of x is too small!" );
#endif

for (unsigned int i=0; i<m_cols; i++)
m_buf[r*m_cols + i ] = x.m_buf[i];

}

// set the rows as vals stored in vector x 
void  swiMat :: setRows( const unsigned int r0, const unsigned int r1, const swiMat &x )
{
#if SW_MAT_BOUNDS_CHECK
if ( r0<0 || r1>= m_rows )
errMsg( "swiMat ::  setRows(): row number out of order!" );

if ( x.size() < (r1-r0+1)*m_cols )
errMsg( "swiMat ::  setRows(): the size of x is too small!" );
#endif

unsigned int k=0;
for (unsigned int i=r0; i<=r1; i++){
for (unsigned int j=0; j<m_cols; j++){
m_buf[i*m_cols + j ] = x.m_buf[k];
k++;
}
}
}

void  swiMat :: setRows( const unsigned int r0, const unsigned int r1, const int &val )
{
#if SW_MAT_BOUNDS_CHECK
if ( r0<0 || r1>= m_rows )
errMsg( "swiMat ::  setRows(): row number out of order!" );
#endif
int *p = &( m_buf[r0*m_cols] );
unsigned int n = (r1-r0+1) * m_cols;
for (unsigned int i=0; i<n; ++i){
*p++ = val;
}
}

void  swiMat :: setSubRow( const unsigned int r, const unsigned int c0, const unsigned int c1, const int val)
{
#if SW_MAT_BOUNDS_CHECK
if ( r<0 || r >= m_rows )
errMsg( "swiMat ::  setSubRow(): row number out of order!" );

if ( c0<0 || c1 >= m_cols )
errMsg( "swiMat ::  setSubRow(): col number range is out of order!" );
#endif

for (unsigned int i=c0; i<=c1; i++)
m_buf[r*m_cols + i ] = val;
}

void  swiMat :: setSubRow( const unsigned int r, const unsigned int c0, const unsigned int c1, const swiMat &x)
{
#if SW_MAT_BOUNDS_CHECK
if ( r<0 || r >= m_rows )
errMsg( "swiMat ::  setSubRow(): row number out of order!" );

if ( c0<0 || c1 >= m_cols )
errMsg( "swiMat ::  setSubRow(): col number range is out of order!" );

if ( x.size() < c1-c0+1 )
errMsg( "swiMat ::  setSubRow(): not enough data in x!" );
#endif

unsigned int k=0;
for (unsigned int i=c0; i<=c1; i++){
m_buf[r*m_cols + i ] = x.m_buf[k];
k++;
}
}

void swiMat :: setSlice( const unsigned int r0, const unsigned int c0, //top left position
const unsigned int rowL, const unsigned int colL,        // row and col length
const int val )
{
#if SW_MAT_BOUNDS_CHECK
if ( r0 < 0 || c0 < 0 )
errMsg("swiMat :: setSlice() , left top position is out of range!");

if ( rowL <= 0 || colL <= 0 )
errMsg("swiMat :: setSlice() , the height and width is out of range!");

if ( r0 + rowL > m_rows || c0 + colL > m_cols )
errMsg("swiMat :: setSlice() , range out of range!");
#endif

for (unsigned int i=r0; i<r0+rowL; i++)
for (unsigned int j=c0; j<c0+colL; j++)
m_buf[i*m_cols + j] = val;
}

void swiMat :: setSlice( const unsigned int r0, const unsigned int c0, //top left position
const unsigned int rowL, const unsigned int colL,        // row and col length
const swiMat &x )
{
#if SW_MAT_BOUNDS_CHECK
if ( r0 < 0 || c0 < 0 )
errMsg("swiMat :: setSlice() , left top position is out of range!");

if ( rowL <= 0 || colL <= 0 )
errMsg("swiMat :: setSlice() , the height and width is out of range!");

if ( r0 + rowL > m_rows || c0 + colL > m_cols )
errMsg("swiMat :: setSlice() , range out of range!");
#endif

unsigned int k=0;
for (unsigned int i=r0; i<r0+rowL; i++)
for (unsigned int j=c0; j<c0+colL; j++)
m_buf[i*m_cols + j] = x.m_buf[k++];
}



swiMat& swiMat :: randomize()
{
for ( unsigned int k=0; k<m_size; k++ )
m_buf[k] = rand();

return *this;
}

swiMat swiMat :: transpose() const
{
swiMat y(m_cols, m_rows);
for ( unsigned int i=0; i<m_rows; i++ )
for ( unsigned int j=0; j<m_cols; j++ )
y.m_buf[j*m_rows + i ]	= m_buf[i*m_cols + j ];

return y;
}


//getSlice for the matrix is a column vector
swiMat swiMat :: getSubCol( const unsigned int c, const unsigned int r0, const unsigned int rowL ) const
{
#if SW_MAT_BOUNDS_CHECK
if ( c < 0 || c >= m_cols )
errMsg("swiMat :: getSubCol(), top row position is out of range!");
if ( r0 < 0 || rowL <= 0)
errMsg("swiMat :: getSubCol(), start position or height is out of range!");
if ( r0 + rowL > m_rows  )
errMsg("swiMat :: getSubCol(), index is out of range!");
#endif

int k=0;
swiMat y(rowL, 1);
for (unsigned int i=r0; i<r0+rowL; i++)
y.m_buf[k++] = m_buf[i*m_cols + c];

return y;
}


//delete the c-th column
void swiMat :: delCol( const unsigned int c )
{
delCols( c, c );
}

//delete the c-th column
void swiMat :: delCols( const unsigned int c1, const unsigned int c2, const unsigned int skip )
{
#if SW_MAT_BOUNDS_CHECK
if ( c2 >= m_cols )
errMsg("swiMat :: delCols(), c2 is out of range!");

if ( c1 < 0 )
errMsg("swiMat :: delCols(), c1 is out of range!");
#endif
if ( c1 == 0 && c2 == (m_cols-1) && skip==1 ){
resize(0, 0);
return;
}

swiMat x0( *this ); //copy the current matrix

unsigned int deletedCols = (c2-c1)/skip + 1;
unsigned int delIdSize = deletedCols * m_rows;
unsigned int* delIdBuf = new unsigned int[delIdSize];

unsigned i, j, k=0;
for(i=0; i<x0.rows(); i++)
for(j=c1; j<=c2; j += skip)
delIdBuf[k++] = i*m_cols + j;

//create the matrix after deleting
unsigned int newCols = m_cols - deletedCols;
resize(m_rows, newCols);

//put the filtered data in m_buf[]
delElementsFromBuf(x0.m_buf, x0.size(), delIdBuf, delIdSize, m_buf);

delete [] delIdBuf;
}

//delete the r-th row
void  swiMat :: delRow( const unsigned int r )
{
delRows(r, r);
}

void swiMat :: delRows( const unsigned int r1, const unsigned int r2, const unsigned int skip )
{
#if SW_MAT_BOUNDS_CHECK
if ( r1 > r2 )
errMsg("swiMat :: delRows(), r1 must be smaller than or equal r2!");
if ( r1 < 0 )
errMsg("swiMat :: delRows(), r1 is out of range!");
#endif
if ( r1 == 0 && r2 == (m_rows-1) && skip==1 ) //try to delete all
{
resize(0, 0);
return;
}
if( r1 >= m_rows ) return;

unsigned int rr2 = ( r2 >= m_rows ) ? (m_rows-1) : r2;

swiMat x0( *this ); //copy the current matrix

unsigned int deletedRows = (rr2-r1)/skip + 1;
unsigned int delIdSize = deletedRows * m_cols;
unsigned int* delIdBuf = new unsigned int[delIdSize];

unsigned i, j, k=0;
for( i=r1; i<=rr2; i+=skip )
for( j=0; j<m_cols; j++ )
delIdBuf[k++] = i*m_cols + j;

//create the matrix after deleting
unsigned int newRows = m_rows - deletedRows;
resize(newRows, m_cols);

//put the filtered data in m_buf[]
delElementsFromBuf(x0.m_buf, x0.size(), delIdBuf, delIdSize, m_buf);

delete [] delIdBuf;
}

// insert a column with same val
void  swiMat :: insertCol( const unsigned int c, const int val ) 
{
swiMat v(m_rows, 1, val);
insertCol( c, v );
}

// insert a column with data from matrix x
void  swiMat :: insertCol( const unsigned int c, const swiMat &x )    
{
swiMat x0( *this ); //copy the current matrix
#if SW_MAT_BOUNDS_CHECK
if ( c<0 )
errMsg( "swiMat :: insertCol(): column # is out of range!"); 

if ( x.cols() != 1 ) 
errMsg( "swiMat :: insertCol(): x is not a column vector!"); 

if ( x0.rows() != x.rows() )
errMsg( "swiMat :: insertCol(): # of rows are miss matching!"); 
#endif

if ( c == 0  ) //insert at the beginging
{
resize(m_rows, x0.cols() + 1);
setCol( 0, x );
setSlice( 0, 1,	m_rows, x0.cols(), x0 );  
}
else if ( c < x0.cols() ) //insert in beween 
{
unsigned int deltaC = m_cols - c;
swiMat s1 = x0.getSlice(0, 0, m_rows, c);
swiMat s2 = x0.getSlice(0, c, m_rows, deltaC);

resize(m_rows, x0.cols() + 1);

setSlice(0, 0, m_rows, c, s1);
setCol(c, x);
setSlice(0, c+1, m_rows, deltaC, s2); 
}
else //insert in outside
{
resize(m_rows, c+1);
setSlice( 0, 0,	m_rows, x0.cols(), x0 );  
//NOTE! there are some junk data in bewteen
setCol( c, x );
}
}

// insert a row with same data value
void  swiMat :: insertRow( const unsigned int r, const int val ) 
{
swiMat v(1, m_cols, val);
insertRow( r, v );
}

void swiMat :: insertRow( const unsigned int r, const int* buf )
{
swiMat v(1, m_cols, buf, m_cols);
insertRow( r, v );
}

// insert a row with data from matrix x
void  swiMat :: insertRow( const unsigned int r, const swiMat& x )
{
swiMat x0( *this ); //copy the current matrix

#if SW_MAT_BOUNDS_CHECK
if ( r<0 )
errMsg( "swiMat :: insertRow(): row # is out of range!"); 

if ( x.rows() != 1 ) 
errMsg( "swiMat :: insertRow(): x is not a row matrix!"); 

if ( x0.cols() != x.cols() )
errMsg( "swiMat :: insertrow(): # of columns are miss matching!"); 
#endif

if ( r == 0  ) //insert at the beginging
{
resize( x0.rows()+1, x0.cols() );
setRow( 0, x );
setSlice( 1, 0,	x0.rows(), m_cols, x0 );  
}
else if ( r < x0.rows() ) //insert in beween 
{
unsigned int deltaR = m_rows - r;
swiMat s1 = x0.getSlice(0, 0, r, m_cols);
swiMat s2 = x0.getSlice(r, 0, deltaR, m_cols);

resize(x0.rows() + 1, m_cols);

setSlice(0, 0, r, m_cols, s1);
setRow(r, x);
setSlice(r+1, 0, deltaR, m_cols, s2); 
}
else //insert in outside
{
resize(r+1, m_cols);
setSlice( 0, 0,	x0.rows(), m_cols, x0 );  
//NOTE! there are some junk data in bewteen
setRow( r, x );
}
}

//keep the original values at the original place, but extened number of rows
//and the garbage are in the extended places
void swiMat :: extendRows( const unsigned int numNewRows )
{
#if SW_MAT_BOUNDS_CHECK
if (m_rows>= numNewRows){
errMsg("swiMat :: extendRows(): numNewRows <= current # of rows!");
}
#endif
swiMat x0( *this ); //copy the current matrix

resize( numNewRows, m_cols);
setSlice(0,0,x0.rows(),x0.cols(), x0);
}

swiMat swiMat :: getSubRow( const unsigned int r, const unsigned int c0, const unsigned int colL ) const
{
#if SW_MAT_BOUNDS_CHECK
if ( r < 0 || r >= m_rows )
errMsg("swiMat :: getSubRow(), column position is out of range!");

if ( c0 < 0 || c0>=m_cols )
errMsg("swiMat :: getSubRow(), start position or length is out of range!");

if ( c0 + colL > m_cols  )
errMsg("swiMat :: getSubRow(), index is out of range!");

if ( colL<0 || colL > m_cols  )
errMsg("swiMat :: getSubRow(), index is out of range!");
#endif

swiMat y(1, colL);
const int *px = m_buf + r*m_cols + c0;
int *py = y.getBuf();
for (unsigned int j=0; j<colL; j++)
*py++ = *px++;

return y;
}

swiMat swiMat :: getCol( const unsigned int c ) const
{
return getSubCol(c, 0, m_rows);
}

swiMat swiMat :: getCols( const unsigned int cBeg, const unsigned int cEnd, const unsigned int nSkip ) const
{
#if SW_MAT_BOUNDS_CHECK
if ( cBeg<0 || cEnd >= m_cols )
errMsg( "swiMat :: getCols(): bundary out of range!");
#endif

unsigned int nCols = 1 + (cEnd - cBeg) / nSkip;
swiMat x(m_rows, nCols);
unsigned int k=0;
for (unsigned int c=cBeg; c<=cEnd; c += nSkip){
x.setCol(k, getCol(c) );
k++;
}

return x;
}

swiMat swiMat :: getRow( const unsigned int r ) const
{
return getSubRow( r, 0, m_cols );
}

swiMat swiMat :: getRows( const unsigned int rBeg, const unsigned int rEnd, const unsigned int nSkip ) const
{
#if SW_MAT_BOUNDS_CHECK
if ( rBeg<0 || rEnd >= m_rows )
errMsg( "swiMat :: getRows(): bundary out of range!");
#endif

unsigned int nRows = 1 + (rEnd - rBeg) / nSkip;
swiMat x(nRows, m_cols);
unsigned int k=0;
for (unsigned int r=rBeg; r<=rEnd; r += nSkip){
x.setRow(k, getRow(r) );
k++;
}

return x;
}

swiMat swiMat :: getSlice( const unsigned int r0, const unsigned int c0,               //top left position
const unsigned int rowL, const unsigned int colL ) const    // row and col length 
{
#if SW_MAT_BOUNDS_CHECK
if ( r0 < 0 || c0 < 0 )
errMsg("swiMat :: getSlice() , left top position is out of range!");

if ( rowL <= 0 || colL <= 0 )
errMsg("swiMat :: getSlice() , the height and width is out of range!");

if ( r0 + rowL > m_rows || c0 + colL > m_cols )
errMsg("swiMat :: getSlice() , range out of range!");
#endif

int k=0;
swiMat y(rowL, colL);
for (unsigned int i=r0; i<r0+rowL; i++)
for (unsigned int j=c0; j<c0+colL; j++)
y.m_buf[k++] = m_buf[i*m_cols + j];

return y;
}

// return a  min(m_rows, m_cols) x 1 matrix which contains the diagonal elements of *this
swiMat swiMat :: getDiagonal() const
{
unsigned int m = SW_MAT_MIN(m_rows, m_cols);
swiMat y(m, 1);
for (unsigned int j=0;j<m; j++)
y.m_buf[j] = m_buf[ j*m_cols + j] ;

return y;
}

//--------------------------------------------
// the following are class private functions
//--------------------------------------------
// param $size$ is not neccessary, here just for safty consideration
void swiMat :: deepCopy( int* buf, const unsigned int size ) const
{
#if SW_MAT_BOUNDS_CHECK
if ( buf == NULL )
errMsg( "Error: swiMat::deepCopy() -- buf is NULL!" );

if ( size != m_size )
errMsg( "Error: swiMat::deepCopy() -- size not match the m_size!" );
#endif

for( unsigned k=0; k<m_size; k++)
buf[k] = m_buf[k];
}

void swiMat::creatBuf()
{
if (m_size > 0){
if (m_buf)
deleteBuf();

m_buf = new int[m_size];

if ( !m_buf )
errMsg( "swiMat::creatBuf(): cannot allocate memory!" );
}
else{
m_size = 0;
m_rows = 0;
m_cols = 0;
m_buf = NULL;
}
}
void swiMat::deleteBuf()
{
if ( m_buf ){
delete [] m_buf;
m_buf = NULL;
}
}

//-----------------------------------------------------------------------------
//input params
//  oldBuf[],original data buffer
//  oldSize, the elements of oldBuf[]
//  deletedIdBuf[], all the elements in oldBuf[] with the IDs stored in deletedIdBuf[] will be deleted
//                  these elements are stored in the order of from smallest to largest
//  deletedIdBufSize, the elements of deletedIdBuf[]
//output
//  newBufp[], the reminder elements oldBuf[] still keep their original order
//-----------------------------------------------------------------------------
void swiMat :: delElementsFromBuf(const int* oldBuf, const unsigned int oldSize, 
const unsigned int* deletedIdBuf, const unsigned int deletedIdBufSize, int* newBuf)
{
bool isFound;
unsigned int i, j, k=0;
unsigned int n0=0;  //start search index in deletedIdBuf[]
for( i=0; i<oldSize; i++){
//find index i in deletedIdBuf
isFound = false;
for (j=n0; j<deletedIdBufSize; j++){
if (i == deletedIdBuf[j]){
isFound = true;
n0 = j+1;
break;
}
}

if ( !isFound )
newBuf[k++] = oldBuf[i];
}
}

void swiMat :: writeToFile( const char* fileName )
{
vector<string> tLines; 
tLines.clear();

writeToFile( fileName, tLines );
}

void swiMat :: writeToFile( const char* fileName, const vector<string> &headLines )
{
FILE *fp = fopen( fileName, "w+" ) ;

if( fp==NULL ) {
errMsg ( "writeEstimationToFile(): file  cannot be opend!" );
}

//write the title lines
for ( unsigned int i=0; i<headLines.size(); i++ )
{
fprintf(fp, "%s\n", headLines[i].c_str() );
}

//write data
for ( unsigned int i=0; i<m_rows; i++ )
{
if (m_cols > 0) {
for (unsigned int j = 0; j < m_cols - 1; j++) {
fprintf(fp, "%d ", m_buf[i*m_cols + j]);
}
fprintf(fp, "%d\n", m_buf[i*m_cols + m_cols - 1]);
}
}

fclose( fp );
}



void swiMat :: readFromFile( const unsigned int nCols, const char* fileName, const int nMaxLines )
{
readFromFile( fileName, 0, nCols,  nMaxLines);
}

void swiMat :: readFromFile( const char* fileName, const unsigned int numOfLinesSkiped, const unsigned int nCols,  const int nMaxLines)
{
unsigned int i, k=0;
int tmp;
unsigned int nTryRows;
swiMat buf(1,nCols);

if (nMaxLines == -1) //default read all
nTryRows = 1000;
else
nTryRows = nMaxLines;

resize( nTryRows, nCols );

FILE *fp = fopen( fileName, "r" ) ;
if( fp == NULL ) {
cout <<"file: " << fileName << endl;
errMsg("swiMat :: readFromFile(): file  cannot be opend!");
return;
}

//read the title lines, and discharge them
int nMaxChars = 1024;
char line[1024];
for(i=0; i<numOfLinesSkiped; i++){
fgets( line, nMaxChars, fp );
//cout << line <<endl;
}

bool breakFlag = false;
while ( 1 )
{
for (i=0; i<nCols; i++){
if (nMaxLines == -1) //read all
{
if ( feof(fp) ){
breakFlag = true;
break;
}
}
else //read nMaxLines
{
if ( feof(fp) || k>= nTryRows ){
breakFlag = true;
break;
}
}

fscanf( fp, "%d", &tmp);
buf(i) = tmp;
};

if ( breakFlag ) break;

if( k<nTryRows ){
setRow(k, buf);	
}
else{
insertRow( k, buf );
}
k++;
}
fclose (fp );

if ( k>0 && k<nTryRows ){
delRows( k, nTryRows-1 );
}

if ( k==0 ){
delRows( 1, nTryRows-1 ); //only keep the 1st row to save the memory
warningMsg("swiMat :: readFromFile(): There is no data in the file, a junk matrix (1 by n) is returned!");
}
}

void swiMat :: readFromFile2( const std::string &path, const std::string &fname_wo_ext, const int &fn, const int &idx)
{
char fileName[1024];
if( idx<0 )
sprintf_s( fileName, 1024, "%s//%s-%05d.txt", path.c_str(), fname_wo_ext.c_str(), fn );
else
sprintf_s( fileName, 1024, "%s//%s-%05d-%05d.txt", path.c_str(), fname_wo_ext.c_str(), fn, idx );


FILE *fp = fopen( fileName, "r" ) ;
if( fp == NULL ) {
cout <<"file: " << fileName << endl;
errMsg("swdMat :: readFromFile2(): file  cannot be opend!");
return;
}

int i,j,m,n;
fscanf(fp, "%d,%d", &m, &n );

resize(m,n);

int *p = m_buf;
int tmp;
for (j=0; j<m; ++j){
for (i=0; i<n-1; ++i){
fscanf( fp, "%d,", &tmp);
*p++ = tmp;
}
fscanf( fp, "%d", &tmp);
*p++ = tmp;
}
fclose (fp );
}

swiMat swiMat :: getData( const int *id,  int n ) const
{
swiMat y(n, 1);
for ( int i=0; i<n; i++)
y.m_buf[i] = m_buf[ id[i] ];

return y;
}

void swiMat::setFrom(const swiMat &x )
{
resize( x.rows(), x.cols() );	
for ( unsigned int i=0; i<x.size(); i++ ){
m_buf[i] = x.m_buf[i];
}
}

void swiMat::errMsg ( const char* msgStr) const
{
//debug version
cerr << msgStr << endl; 
getchar();

//release version
throw runtime_error( msgStr );
}

void swiMat::warningMsg ( const char* msgStr) const
{
//debug version
cerr << msgStr << endl; 
getchar();
}
