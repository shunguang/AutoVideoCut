/*
*-------------------------------------------------------------------
* swdMat.h
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

//2d matrix, data stored row by row wise
//
#ifndef SWD_MAT_H
#define SWD_MAT_H 1

#include <math.h>
#include <stdio.h>
#include <conio.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "swMatDefs.h"

using namespace std;

class SWMAT_EXPORT swdMat{
//non-member friend functions
friend ostream& operator<<( ostream &os, swdMat &x );
friend swdMat   operator+( const double scale, const swdMat &x ); // 5+x
friend swdMat   operator-( const double scale, const swdMat &x ); // 5-x
friend swdMat   operator*( const double scale, const swdMat &x ); // 5*x
friend swdMat   operator/( const double scale, const swdMat &x ); // 5/x

public:
swdMat( const unsigned int nR=0, const unsigned int nC=0 );
swdMat( const unsigned int nR, const unsigned int nC, const double initialValue );
swdMat( const unsigned int nR, const unsigned int nC, const double *buf, const unsigned int bufLength );
swdMat( const swdMat &initMat);
swdMat(const unsigned int nCols, const char* fileName);

~swdMat();

unsigned int cols() const; //get # of cols
double*  getBuf();
const double* data() const;               //get the data buffer  
void  data( double **p );
void  delCol( const unsigned int c );  // delete coloumn c
void  delCols( const unsigned int c1, const unsigned int c2, const unsigned int skip=1 );
void  delRow( const unsigned int r );  // delete row r
void  delRows( const unsigned int r1, const unsigned int r2, const unsigned int skip=1 );

//keep the original values at the original place, but extened number of rows
//and the garbage are in the extended places
void extendRows( const unsigned int numNewRows );

swdMat getCol( const unsigned int c ) const;    // get a column
swdMat getCols( const unsigned int cBeg, const unsigned int cEnd, const unsigned int nSkip=1 ) const;
swdMat getDiagonal() const;						// get the diagnal
swdMat getRow( const unsigned int r ) const;	// get a row
double* getRowAddress( const unsigned int &r );
const double* getRowAddress2( const unsigned int &r ) const;

swdMat getRows( const unsigned int rBeg, const unsigned int rEnd, const unsigned int nSkip=1 ) const;
swdMat getSlice( const unsigned int r0,   const unsigned int c0,           //top left position
const unsigned int rowL, const unsigned int colL ) const;     // row and col length 
swdMat getSubCol( const unsigned int colNum, const unsigned int r0, const unsigned int rowL ) const;
swdMat getSubRow( const unsigned int rowNum, const unsigned int c0, const unsigned int colL ) const;

void  insertCol( const unsigned int c, const double val );     // insert a column with same val
void  insertCol( const unsigned int c, const double* buf );    // insert a column with data from buf[]
void  insertCol( const unsigned int c, const swdMat &x );      // insert a column with data from matrix x
void  insertRow( const unsigned int r, const double val );     // insert a row with same data value
void  insertRow( const unsigned int r, const double* buf );    // insert a row with data from buf[]
void  insertRow( const unsigned int r, const swdMat& x );      // insert a row with data from matrix x

void  print(const char* str=" ") const;                   
void  print(const char* fmt, const char* str) const;  

void  cprint(const char* str=" ") const;                   

swdMat& randomize();       // randomize the matrix
//change size of the matrix, 
void resize( const unsigned int newRows, const unsigned int newCols );
void resize( const unsigned int newRows, const unsigned int newCols, const double val );
// keep the value, and size, but chnage raws and cols, and the data are stored row by row
void reshape( const unsigned int newRows, const unsigned int newCols );	
unsigned int rows() const;  //get # of rows


void  setCol( const unsigned int c, const double val );   // set the col elements as val
void  setCol( const unsigned int c, const double* buf, const unsigned int bufSize);  // set the col elements as vals stored in buf[] 
void  setCol( const unsigned int c, const swdMat &x );   // set the col elements as vals stored in vector x 
void  setSubCol( const unsigned int c, const unsigned int r0, const unsigned int r1, const double val);
void  setSubCol( const unsigned int c, const unsigned int r0, const unsigned int r1, const swdMat &x);
void  setData( const double *buf, const unsigned int bufSize );
void  setDiagonal( const double val );  // set the diagnoal elements as val
void  setDiagonal( const swdMat &x );  // set the diagnoal elements as vals stored in vector x 
void  setDiagonal( const double *buf, const unsigned int bufSize );  // set the diagnoal elements as vals stored in vector x 
void  setRow( const unsigned int &r, const double &val );  // set the row elements as val
void  setRow( const unsigned int &r, const swdMat &x, const bool &isByRow=true );  // set the row elements as vals stored in vector x 
void  setRow( const unsigned int &r, const double *pSrc ); 
void  setRows( const unsigned int r0, const unsigned int r1, const swdMat &x );  // set the rows as vals stored in vector x 
void  setSubRow( const unsigned int r, const unsigned int c0, const unsigned int c1, const double val);
void  setSubRow( const unsigned int r, const unsigned int c0, const unsigned int c1, const swdMat &x);

void  setSlice( const unsigned int r0, const unsigned int c0, //top left position
const unsigned int rowL, const unsigned int colL,    // row and col length
const double val );
void  setSlice( const unsigned int r0, const unsigned int c0, //top left position
const unsigned int rowL, const unsigned int colL,        // row and col length
const swdMat &x );

unsigned int size() const; //get the size # 
double trace() const;     // get trace
swdMat  transpose() const;  // get transpose
void appendToFile( FILE *fp, char *fmtStr, char *msgStr=NULL, bool isPrintSize=true ) const;
void writeToFile( const char* fileName, const char *fmt=NULL ) const;
void writeToFile( const char* fileName, const vector<string> &titleLines, const char *fmt=NULL ) const;
void writeToFile2( const std::string &path, const std::string &fname_wo_ext, const std::string &fmt, const int &fn, const int &idx=-1);

void readFromFile( const unsigned int nCols, const char* fileName, const int nMaxLines=-1);
void readFromFile( const char* fileName, const unsigned int numOfLinesSkiped, const unsigned int nCols,  const long nMaxLines=-1);
void readFromFile2( const std::string &path, const std::string &fname_wo_ext, const int &fn, const int &idx=-1);

//public operator overloads
swdMat& operator =( const swdMat &x); //assignment matrix x to this
swdMat& operator =(const double d);  //assignment d to all elements of this

bool   operator==( const swdMat &x) const; //compare equal
bool   operator!=( const swdMat &x) const; //compare not equal
double  operator()( const unsigned int i, const unsigned int j ) const;   // get a(i,j)
double& operator()( const unsigned int i, const unsigned int j );         // get a(i,j)
double  operator()( const unsigned int k ) const; 
double& operator()( const unsigned int k ); 

// in the following explaination. we assume $a$ is the matrix at the left hand side
swdMat operator -() const;                   //by add minus sign for each element
swdMat operator +(const swdMat &x) const;     // a+x  add by element
swdMat operator +(const double scale) const; // a+scale add scale to every element
swdMat operator -(const swdMat &x) const;     // a-x  minus by element
swdMat operator -(const double scale) const; // a-scale  minus ascale to every element
swdMat operator *(const swdMat &x) const;     // a*x  times by element
swdMat operator *(const double scale) const; // a*scale  times scale to every element
swdMat operator /(const swdMat &x) const;     // a/x  dividion by element
swdMat operator /(const double scale) const; // a/scale  divided every element by the scale
swdMat operator %(const swdMat &x) const;     // a%x  matrix prodcution

swdMat& operator +=( const swdMat &x);     //a +=x (a=a+x), by element
swdMat& operator +=( const double scale); //a+=scale, matrix plus a scale
swdMat& operator -=( const swdMat &x);     //a -= x, by element
swdMat& operator -=( const double scale); //a -= scale, matrix minus a scale
swdMat& operator *=( const swdMat &x);     //a *=x, by element
swdMat& operator *=( const double scale); //a *= scale, amatrix times a scale
swdMat& operator /=( const swdMat &x);     //a /= x, by element
swdMat& operator /=( const double scale); //a /= scale, by element

private:
void creatBuf();
void deleteBuf();
void deepCopy( double* buf, const unsigned int size ) const;
void delElementsFromBuf(const double* oldBuf, const unsigned int oldSize, 
const unsigned int* deletedIdBuf, const unsigned int deletedIdBufSize, double* newBuf);
void errMsg ( const char* msgStr) const;
void warningMsg ( const char* msgStr) const;

unsigned int m_rows;  //rows
unsigned int m_cols;  //cols
unsigned int m_size; // m_rows * m_cols
double* m_buf;
};

#endif

