/*
*-------------------------------------------------------------------
* swMatLu.h
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
#ifndef SWD_MAT_LU_H
#define SWD_MAT_LU_H

#include <math.h>
#include <conio.h>

#include "swMatDefs.h"
#include "swdMat.h"
#include "swfMat.h"

class SWMAT_EXPORT swMatLu {
public:
// Constructors/Destructors 
swMatLu( const swdMat &A ); // Factor A = L*U 
swMatLu( const swfMat &A ); // Factor A = L*U 
~swMatLu(); 

// Accessors
bool  isGood() const;
bool  isFail() const;
bool  isSingular() const;
double det() const;   // return det(A)
bool inv( swdMat &invA );
bool inv( swfMat &invA );
void  solve( swdMat& b); // Solve L*U*x = b to find x, the results is in b
void  solve( swfMat& b); // Solve L*U*x = b to find x, the results is in b
private:
void initProb();
void print( const double *a, const int n);
void errorMsg( const char *msgStr);
void ludcmp(double* a, const int n, int *indx, double& d);
void lubksb( const double* a, const int n, const int* indx, double* b );

swdMat m_LU;       //store the pivoted LU matrix
unsigned int m_n; //the dimension of the matrix
int  m_infoFlag;  //a flag to show if LU is fail or the input matrix is singular
double m_d;        // =1, or -1 depending on whether the number of row interchnage
//is even or order, it is used to calculate the determinant value 

int* m_indx;      //store the pivoting/permutation row index
double *m_ptrLU;   //a pointer points to the data of m_LU matrix
double* m_vv;      //a temporal vector used in inv() and ludcmp()  
};
#endif

