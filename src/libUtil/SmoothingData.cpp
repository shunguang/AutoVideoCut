/*
*-------------------------------------------------------------------
* SmoothingData.cpp
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

#include "SmoothingData.h"

using namespace std;
using namespace autocut;

SmoothingData::SmoothingData(const SmoothType type, const int winSz)
: m_type(type)
, m_winSz(winSz)
{}

SmoothingData::~SmoothingData()
{
}


void  SmoothingData::gaussKernel( const int samples0, const float32 sigma)
{
int samples = samples0;
bool doubleCenter = false;
if (samples % 2 == 0) {
doubleCenter = true;
samples--;
}
int steps = (samples - 1) / 2;
float32 stepSize = (3 * sigma) / steps;

float32 x, s = 0;
m_kernel.clear();
for (int i = steps; i >= 1; i--) {
x = gauss(sigma, i * stepSize * -1);
m_kernel.push_back( x );
s += x;
}

x = gauss(sigma, 0);

m_kernel.push_back(x );
s += x;
if (doubleCenter) {
m_kernel.push_back(x);
s += x;
}

for (int i = 1; i <= steps; i++) {
x = gauss(sigma, i * stepSize);
m_kernel.push_back(x);
s += x;
}

for (auto it = m_kernel.begin(); it != m_kernel.end(); ++it) {
*it /= s;
}

#if _DEBUG
float32 s2 = 0;
std::cout << "The kernel contains " << m_kernel.size() << " entries:";
for (auto it = m_kernel.begin(); it != m_kernel.end(); ++it) {
std::cout << ' ' << *it;
s2 += *it;
}
std::cout << "s2=" << s2 << std::endl;
#endif
}


void SmoothingData :: process(float32 *out, const float32 *in, const uint32 n )
{
int samples = m_winSz;
float32 sigma = 2.0 * samples / 3;

gaussKernel(samples, sigma);
samples = m_kernel.size();

assert(samples % 2 == 1);

int sampleSide = samples / 2;

//todo: average the beging points
for (uint32 i = 0; i < sampleSide; i++) {
out[i] = in[i];
}

uint32 n2 = n - sampleSide;
for ( uint32 i = sampleSide; i < n2; i++) {
float32 s = 0;
const float32 *pIn = in + (i - sampleSide);
for ( uint32 k=0; k < samples; k++) {
s += (m_kernel[k] *  pIn[k]);
}
out[i] = s / samples;
}

//dodo
for (uint32 i = n2; i < n; i++) {
out[i] = in[i];
}

}

