/*
*-------------------------------------------------------------------
* SmoothingData.h
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
#ifndef __SMOOTHING_DATA_H__
#define __SMOOTHING_DATA_H__

#include "DataTypes.h"
#include "UtilDefs.h"

namespace autocut {

enum SmoothType{
STYPE_GAUSSIAN = 0,
STYPE_MOV_MEAN,
STYPE_MOV_MEDIAN
};

class SmoothingData {
public:
SmoothingData(const SmoothType type = STYPE_GAUSSIAN, const int winSz =10);
~SmoothingData();

void process(float32 *out, const float32 *in, const uint32 inSz);

private:
void gaussKernel( const int samples0, const float32 sigma);

inline float32 gauss(float32 sigma, float32 x) {
float32 expVal = -1 * (pow(x, 2) / pow(2 * sigma, 2));
float32 divider = sqrt(2 * MY_PI_FLT32 * pow(sigma, 2));
return (1 / divider) * exp(expVal);
}


private:
std::vector<float32> m_kernel;
SmoothType			 m_type;
int					 m_winSz;
};

typedef std::tr1::shared_ptr<SmoothingData> SmoothingDataPtr;
}
#endif
