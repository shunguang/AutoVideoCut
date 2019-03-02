/*
*-------------------------------------------------------------------
* swdMatSvd.h
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

#ifndef SWD_MAT_SVD_H
#define SWD_MAT_SVD_H

#include <math.h>
#include <conio.h>

#include "swMatDefs.h"
#include "swdMat.h"

class SWMAT_EXPORT swdMatSvd {
public:
// Constructors/Destructors 
swdMatSvd(const swdMat &X ); // Factor X = U*S*V', X.rows() >= X.cols();
~swdMatSvd(); 

// Accessors
swdMat getU() { return m_U.getSlice(1, 1, m_m-1, m_n-1); }
swdMat getS() { return m_S.getSlice(1, 1, m_n-1, m_n-1); }
swdMat getV() { return m_V.getSlice(1, 1, m_n-1, m_n-1); }
int   rank( const double tol ) const;
int   rank() const;
private:
void errMsg( const char *msgStr);
void svdcmp(double **a, int m, int n, double *w, double **v);
double PYTHAG( const double a, const double b );
double MAX   ( const double a, const double b );
double SIGN  ( const double a, const double b );
void  print( double **a, const int m, const int n);


unsigned int m_m; // = X.rows()+1
unsigned int m_n; // = X.cols()+1
swdMat m_U;       //store U, m_m x m_n
swdMat m_S;       //store S, m_n x m_n
swdMat m_V;       //store V, m_n x m_n

double** m_pptrU;   //a pointer array save the each row's address of m_U
double** m_pptrV;   //a pointer array save the each row's address of m_V
double*  m_ptrW;    //save the diagnol of m_S
};
#endif

