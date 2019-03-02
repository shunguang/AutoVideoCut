/*
*-------------------------------------------------------------------
* swRandom.cpp
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
#include "swRandom.h"

// constructor:
swRandom::swRandom( const unsigned long seed) {
// Check that compiler supports 80-bit long double:
//assert(sizeof(long double)>9);

// initialize
randSeed( seed );
}


// returns a random number between 0 and 1:
double swRandom::randDouble() 
{
long double c;
c = (long double)2111111111.0 * m_x[3] +
1492.0 * (m_x[3] = m_x[2]) +
1776.0 * (m_x[2] = m_x[1]) +
5115.0 * (m_x[1] = m_x[0]) +
m_x[4];
m_x[4] = (double) floor(c);
m_x[0] = c - m_x[4];
m_x[4] = m_x[4] * (1./(65536.*65536.));
return m_x[0];
}

// returns an integer random number in desired interval:
int swRandom::randInteger(const int min, const int max) 
{
int iinterval = max - min + 1;
if (iinterval <= 0) return 0x80000000; // error
int i = int(iinterval * randDouble());     // truncate
if (i >= iinterval) i = iinterval-1;
return min + i;
}


// this function initializes the random number generator
void swRandom::randSeed ( const unsigned long seed) 
{
int i;
unsigned long s = seed;
// make random numbers and put them into the buffer
for (i=0; i<5; i++) {
s = s * 29943829 - 1;
m_x[i] = s * (1./(65536.*65536.));}

// randomize some more
for (i=0; i<19; i++) randDouble();

m_gaussianTmp2 = -1 + 2 * randDouble();

}

//----------------------------------------------------------
// Gaussian distribution with sigma=1 and mean=0
// by Polar (Box-Mueller) method; See Knuth v2, 3rd ed, p122 
// [1] Knuth, D.E., 1981; The Art of Computer Programming, 
//     Volume 2 Seminumerical Algorithms, Addison-Wesley, Reading Mass., 688 pages, 
//     ISBN 0-201-03822-6
//----------------------------------------------------------
double swRandom :: randGaussian()
{
double r2;

do{
/* choose two random numbers in uniform square (-1,-1) to (+1,+1) */
m_gaussianTmp1 = m_gaussianTmp2;
m_gaussianTmp2 = -1.0 + 2.0 * randDouble();

/* see if it is in the unit circle */
r2 = m_gaussianTmp1 * m_gaussianTmp1 + m_gaussianTmp2 * m_gaussianTmp2;
}while ( r2 >= 1.0 );

/* Box-Muller transform */
return m_gaussianTmp2 * sqrt (-2.0 * log (r2) / r2);
}


// Gaussian distribution with given sigma and mean
double swRandom :: randGaussian(const double sigma, const double mean)
{
return mean + sigma * randGaussian();
}

