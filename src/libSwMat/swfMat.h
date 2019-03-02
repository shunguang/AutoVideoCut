/*
*-------------------------------------------------------------------
* swfMat.h
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

#ifndef SWF_MAT_H
#define SWF_MAT_H 1

#include <math.h>
#include <stdio.h>
#include <conio.h>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>

#include "swMatDefs.h"

using namespace std;

class SWMAT_EXPORT swfMat{
//non-member friend functions
friend ostream& operator<<( ostream &os, swfMat &x );
friend swfMat   operator+( const float scale, const swfMat &x ); // 5+x
friend swfMat   operator-( const float scale, const swfMat &x ); // 5-x
friend swfMat   operator*( const float scale, const swfMat &x ); // 5*x
friend swfMat   operator/( const float scale, const swfMat &x ); // 5/x

public:
swfMat( const unsigned int nR=0, const unsigned int nC=0 );
swfMat( const unsigned int nR, const unsigned int nC, const float initialValue );
swfMat( const unsigned int nR, const unsigned int nC, const float startVal, const int step );
swfMat( const unsigned int nR, const unsigned int nC, const float *buf, const unsigned int bufLength );
swfMat( const swfMat &initMat);
swfMat(const unsigned int nCols, const char* fileName);

~swfMat();

unsigned int cols() const; //get # of cols

float* getBuf();
const float* getBuf_const() const;               //get the data buffer  
const float* data() const;
const float* getRowPtr_const(const unsigned int &r) const
{
return m_buf + r*m_cols;
}
float* getRowPtr(const unsigned int &r)
{
return m_buf + r*m_cols;
}

void  delCol( const unsigned int c );  // delete coloumn c
void  delCols( const unsigned int c1, const unsigned int c2, const unsigned int skip=1 );
void  delRow( const unsigned int r );  // delete row r
void  delRows( const unsigned int r1, const unsigned int r2, const unsigned int skip=1 );

//keep the original values at the original place, but extened number of rows
//and the garbage are in the extended places
void extendRows( const unsigned int numNewRows );

swfMat getCol( const unsigned int c ) const;    // get a column
swfMat getCols( const unsigned int cBeg, const unsigned int cEnd, const unsigned int nSkip=1 ) const;

swfMat getDiagonal() const;                  // get the diagnal
swfMat getRow( const unsigned int r ) const;    // get a row


swfMat getRows( const unsigned int rBeg, const unsigned int rEnd, const unsigned int nSkip=1 ) const;
swfMat getSlice( const unsigned int r0,   const unsigned int c0,               //top left position
const unsigned int rowL, const unsigned int colL ) const;     // row and col length 
swfMat getSubCol( const unsigned int colNum, const unsigned int r0, const unsigned int rowL ) const;
swfMat getSubRow( const unsigned int rowNum, const unsigned int c0, const unsigned int colL ) const;
swfMat getData( const int *id,  int n ) const;				  //get data from indeces			

void  insertCol( const unsigned int c, const float val );     //insert a column with same val
void  insertCol( const unsigned int c, const float* buf );    //insert a column with data from buf[]
void  insertCol( const unsigned int c, const swfMat &x );     // insert a column with data from matrix x
void  insertRow( const unsigned int r, const float val );     // insert a row with same data value
void  insertRow( const unsigned int r, const float* buf );    // insert a row with data from buf[]
void  insertRow( const unsigned int r, const swfMat& x );     // insert a row with data from matrix x

void  print(const char* fmt, const char* str) const;
void  print(const char* str=" ") const;                   

void randomize();       // randomize the matrix
//change size of the matrix, 
void resize( const unsigned int newRows, const unsigned int newCols );
void resize( const unsigned int newRows, const unsigned int newCols, const float val );
// keep the value, and size, but chnage raws and cols, and the data are stored row by row
void reshape( const unsigned int newRows, const unsigned int newCols );	
void reshapeByCol( const unsigned int newRows, const unsigned int newCols );	

unsigned int rows() const;		 //get # of rows
void  setCol( const unsigned int c, const float val );   // set the col elements as val
void  setCol( const unsigned int c, const float* buf, const unsigned int bufSize);  // set the col elements as vals stored in buf[] 
void  setCol( const unsigned int c, const swfMat &x );   // set the col elements as vals stored in vector x 
void  setSubCol( const unsigned int c, const unsigned int r0, const unsigned int r1, const float val);
void  setSubCol( const unsigned int c, const unsigned int r0, const unsigned int r1, const swfMat &x);
void  setData( const float *buf, const unsigned int bufSize );
void  setDiagonal( const float val );  // set the diagnoal elements as val
void  setDiagonal( const swfMat &x );  // set the diagnoal elements as vals stored in vector x 
void  setDiagonal( const float *buf, const unsigned int bufSize );  // set the diagnoal elements as vals stored in vector x 
void  setRow( const unsigned int r, const float val );  // set the row elements as val
void  setRow( const unsigned int r, const swfMat &x );  // set the row elements as vals stored in vector x 
void  setRows( const unsigned int r0, const unsigned int r1, const swfMat &x );  // set the rows as vals stored in vector x 
void  setSubRow( const unsigned int r, const unsigned int c0, const unsigned int c1, const float val);
void  setSubRow( const unsigned int r, const unsigned int c0, const unsigned int c1, const swfMat &x);

void  setSlice( const unsigned int r0, const unsigned int c0, //top left position
const unsigned int rowL, const unsigned int colL,    // row and col length
const float val );
void  setSlice( const unsigned int r0, const unsigned int c0, //top left position
const unsigned int rowL, const unsigned int colL,    // row and col length
const swfMat &x );
void setFrom(const swfMat &x );								  //comunication with swfMat 	
void setFrom(const double* ptr,int size);					  //comunication with double 		


unsigned int size() const; //get the size # 
float trace() const;       // get trace
swfMat  transpose() const;  // get transpose
void appendToFile( FILE *fp, char *fmtStr, char *msgStr=NULL, bool isPrintSize=true ) const;
void writeToFile( const char* fileName, const char *fmt=NULL ) const;
void writeToFile( const char* fileName, const vector<string> &titleLines, const char *fmt=NULL ) const;
void readFromFile( const unsigned int nCols, const char* fileName, const int nMaxLines=-1);
void readFromFile( const char* fileName, const unsigned int numOfLinesSkiped, const unsigned int nCols,  const long nMaxLines=-1);

//public operator overloads
swfMat& operator =( const swfMat &x); //assignment matrix x to this
swfMat& operator =(const float d);  //assignment d to all elements of this

bool   operator==( const swfMat &x) const; //compare equal
bool   operator!=( const swfMat &x) const; //compare not equal
float  operator()( const unsigned int i, const unsigned int j ) const;   // get a(i,j)
float& operator()( const unsigned int i, const unsigned int j );         // get a(i,j)
float  operator()( const unsigned int k ) const; 
float& operator()( const unsigned int k ); 

// in the following explaination. we assume $a$ is the matrix at the left hand side
swfMat operator -() const;						//by add minus sign for each element
swfMat operator +(const swfMat &x) const;		// a+x  add by element
swfMat operator +(const float scale) const;		// a+scale add scale to every element
swfMat operator -(const swfMat &x) const;		// a-x  minus by element
swfMat operator -(const float scale) const;		// a-scale  minus ascale to every element
swfMat operator *(const swfMat &x) const;		// a*x  times by element
swfMat operator *(const float scale) const;		// a*scale  times scale to every element
swfMat operator /(const swfMat &x) const;		// a/x  dividion by element
swfMat operator /(const float scale) const;		// a/scale  divided every element by the scale
swfMat operator %(const swfMat &x) const;		// a%x  matrix prodcution

swfMat& operator +=( const swfMat &x);		//a +=x (a=a+x), by element
swfMat& operator +=( const float scale);	//a+=scale, matrix plus a scale
swfMat& operator -=( const swfMat &x);		//a -= x, by element
swfMat& operator -=( const float scale);	//a -= scale, matrix minus a scale
swfMat& operator *=( const swfMat &x);		//a *=x, by element
swfMat& operator *=( const float scale);	//a *= scale, amatrix times a scale
swfMat& operator /=( const swfMat &x);		//a /= x, by element
swfMat& operator /=( const float scale);	//a /= scale, by element

private:
void creatBuf();
void deleteBuf();
void deepCopy( float* buf, const unsigned int size ) const;
void delElementsFromBuf(const float* oldBuf, const unsigned int oldSize, 
const unsigned int* deletedIdBuf, const unsigned int deletedIdBufSize, float* newBuf);
void errMsg ( const char* msgStr) const;
void warningMsg ( const char* msgStr) const;

unsigned int m_rows;  //rows
unsigned int m_cols;  //cols
unsigned int m_size; // m_rows * m_cols
float* m_buf;
};

typedef std::tr1::shared_ptr<swfMat>		swfMatPtr;

#endif

