/*
*-------------------------------------------------------------------
* swiMat.h
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

#ifndef SWI_MAT_H
#define SWI_MAT_H 1

#include <math.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

#include "swMatDefs.h"

class SWMAT_EXPORT swiMat{
//non-member friend functions
friend ostream& operator<<( ostream &os, swiMat &x );
friend swiMat   operator+( const int scale, const swiMat &x ); // 5+x
friend swiMat   operator-( const int scale, const swiMat &x ); // 5-x
friend swiMat   operator*( const int scale, const swiMat &x ); // 5*x

public:
swiMat( const unsigned int nR=0, const unsigned int nC=0 );
swiMat( const unsigned int nR, const unsigned int nC, const int initialValue );
swiMat( const unsigned int nR, const unsigned int nC, const int startVal, const int step );
swiMat( const unsigned int nR, const unsigned int nC, const int *buf, const unsigned int bufLength );
swiMat( const swiMat &initMat);
swiMat(const unsigned int nCols, const char* fileName);

~swiMat();

unsigned int cols() const; //get # of cols
int*  getBuf();
int*  getAddress( const unsigned int r, const unsigned int c );
const int*  data() const;               //get the data buffer  
void  data( int **p );

const int* getRowPtr_const(const unsigned int &r) const
{
return m_buf + r*m_cols;
}
int* getRowPtr(const unsigned int &r)
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

swiMat getCol( const unsigned int c ) const;    // get a column
swiMat getCols( const unsigned int cBeg, const unsigned int cEnd, const unsigned int nSkip=1 ) const;
swiMat getDiagonal() const;                  // get the diagnal
swiMat getRow( const unsigned int r ) const;    // get a row
swiMat getRows( const unsigned int rBeg, const unsigned int rEnd, const unsigned int nSkip=1 ) const;
swiMat getSlice( const unsigned int r0,   const unsigned int c0,               //top left position
const unsigned int rowL, const unsigned int colL ) const;     // row and col length 
swiMat getSubCol( const unsigned int colNum, const unsigned int r0, const unsigned int rowL ) const;
swiMat getSubRow( const unsigned int rowNum, const unsigned int c0, const unsigned int colL ) const;
swiMat getData( const int *id,  int n ) const;				  //get data from indeces			

void  insertCol( const unsigned int c, const int val );     // insert a column with same val
void  insertCol( const unsigned int c, const int* buf );    // insert a column with data from buf[]
void  insertCol( const unsigned int c, const swiMat &x );       // insert a column with data from matrix x
void  insertRow( const unsigned int r, const int val );     // insert a row with same data value
void  insertRow( const unsigned int r, const int* buf );    // insert a row with data from buf[]
void  insertRow( const unsigned int r, const swiMat& x );       // insert a row with data from matrix x

void  print(const char* str=" ") const;                   

swiMat& randomize();       // randomize the matrix
//change size of the matrix, 
void resize( const unsigned int newRows, const unsigned int newCols);
void resize( const unsigned int newRows, const unsigned int newCols, const int val );

// keep the value, and size, but chnage raws and cols, and the data are stored row by row
void reshape( const unsigned int newRows, const unsigned int newCols );	
unsigned int rows() const ;  //get # of rows


void  setCol( const unsigned int c, const int val );   // set the col elements as val
void  setCol( const unsigned int c, const int* buf, const unsigned int bufSize);  // set the col elements as vals stored in buf[] 
void  setCol( const unsigned int c, const swiMat &x );   // set the col elements as vals stored in vector x 
void  setSubCol( const unsigned int c, const unsigned int r0, const unsigned int r1, const int val);
void  setSubCol( const unsigned int c, const unsigned int r0, const unsigned int r1, const swiMat &x);
void  setData( const int *buf, const unsigned int bufSize );
void  setDiagonal( const int val );  // set the diagnoal elements as val
void  setDiagonal( const swiMat &x );  // set the diagnoal elements as vals stored in vector x 
void  setDiagonal( const int *buf, const unsigned int bufSize );  // set the diagnoal elements as vals stored in vector x 
void  setRow( const unsigned int r, const int val );  // set the row elements as val
void  setRow( const unsigned int r, const swiMat &x );  // set the row elements as vals stored in vector x 
void  setRows( const unsigned int r0, const unsigned int r1, const swiMat &x );  // set the rows as vals stored in vector x 
void  setRows( const unsigned int r0, const unsigned int r1, const int &val );   // set the rows as val
void  setSubRow( const unsigned int r, const unsigned int c0, const unsigned int c1, const int val);
void  setSubRow( const unsigned int r, const unsigned int c0, const unsigned int c1, const swiMat &x);

void  setSlice( const unsigned int r0, const unsigned int c0, //top left position
const unsigned int rowL, const unsigned int colL,    // row and col length
const int val );
void  setSlice( const unsigned int r0, const unsigned int c0, //top left position
const unsigned int rowL, const unsigned int colL,        // row and col length
const swiMat &x );
void setFrom(const swiMat &x );								  //comunication with swfMat 	

unsigned int size() const; //get the size # 
int trace() const;     // get trace
swiMat  transpose() const;  // get transpose
void writeToFile( const char* fileName );
void writeToFile( const char* fileName, const vector<string> &titleLines );
void readFromFile( const unsigned int nCols, const char* fileName, const int nMaxLines=-1);
void readFromFile( const char* fileName, const unsigned int numOfLinesSkiped, const unsigned int nCols,  const int nMaxLines=-1);
void readFromFile2( const std::string &path, const std::string &fname_wo_ext, const int &fn, const int &idx=-1);

//public operator overloads
swiMat& operator =( const swiMat &x); //assignment matrix x to this
swiMat& operator =(const int d);  //assignment d to all elements of this

bool   operator==( const swiMat &x) const; //compare equal
bool   operator!=( const swiMat &x) const; //compare not equal
int  operator()( const unsigned int i, const unsigned int j ) const;   // get a(i,j)
int& operator()( const unsigned int i, const unsigned int j );         // get a(i,j)
int  operator()( const unsigned int k ) const; 
int& operator()( const unsigned int k ); 

// in the following explaination. we assume $a$ is the matrix at the left hand side
swiMat operator -() const;                   //by add minus sign for each element
swiMat operator +(const swiMat &x) const;     // a+x  add by element
swiMat operator +(const int scale) const; // a+scale add scale to every element
swiMat operator -(const swiMat &x) const;     // a-x  minus by element
swiMat operator -(const int scale) const; // a-scale  minus ascale to every element
swiMat operator *(const swiMat &x) const;     // a*x  times by element
swiMat operator *(const int scale) const; // a*scale  times scale to every element
swiMat operator /(const swiMat &x) const;     // a/x  dividion by element
swiMat operator /(const int scale) const; // a/scale  divided every element by the scale
swiMat operator %(const swiMat &x) const;     // a%x  matrix prodcution

swiMat& operator +=( const swiMat &x);     //a +=x (a=a+x), by element
swiMat& operator +=( const int scale); //a+=scale, matrix plus a scale
swiMat& operator -=( const swiMat &x);     //a -= x, by element
swiMat& operator -=( const int scale); //a -= scale, matrix minus a scale
swiMat& operator *=( const swiMat &x);     //a *=x, by element
swiMat& operator *=( const int scale); //a *= scale, amatrix times a scale
swiMat& operator /=( const swiMat &x);     //a /= x, by element
swiMat& operator /=( const int scale); //a /= scale, by element

private:
void creatBuf();
void deleteBuf();
void deepCopy( int* buf, const unsigned int size ) const;
void delElementsFromBuf(const int* oldBuf, const unsigned int oldSize, 
const unsigned int* deletedIdBuf, const unsigned int deletedIdBufSize, int* newBuf);
void errMsg ( const char* msgStr) const;
void warningMsg ( const char* msgStr) const;

unsigned int m_rows;  //rows
unsigned int m_cols;  //cols
unsigned int m_size; // m_rows * m_cols
int* m_buf;
};

#endif

