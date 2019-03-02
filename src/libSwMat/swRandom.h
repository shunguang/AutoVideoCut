/*
*-------------------------------------------------------------------
* swRandom.h
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
#ifndef SW_RAND_INCLUDE
#define SW_RAND_INCLUDE
#include <math.h>
#include <conio.h>
#include <iostream>
#include "swMatDefs.h"

class SWMAT_EXPORT swRandom{                                       // a random number generator
public:
swRandom( const unsigned long seed=4096 );        // constructor
void   randSeed( const unsigned long seed);       // set seed to reinitialization
int    randInteger(const int min, const int max); // get integer uniform distributed random number in desired interval
double randDouble();                              // get floating point uniform distributed random number
double randGaussian();                            // Gaussian distribution with sigma=1 and mean=0
double randGaussian(const double sigma, const double mean=0); // Gaussian distribution with given sigma and mean

protected:
double m_x[5];                                    // history buffer

private:
double m_gaussianTmp1;  //temporal uniform random # for generator a gaussian rand #
double m_gaussianTmp2;  //temporal uniform random # for generator a gaussian rand #
};

#endif
