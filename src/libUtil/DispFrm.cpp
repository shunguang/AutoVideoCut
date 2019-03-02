/*
*-------------------------------------------------------------------
* DispFrm.cpp
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

#include "DispFrm.h"

using namespace std;
using namespace cv;
using namespace autocut;

DispFrm::DispFrm(int w, int h, int type)
: m_fn(0)
, m_keyFrmIdx(0)
, m_timeStamp_ms(0)
, m_img(w, h, type)
{
}

DispFrm::DispFrm(cv::Size sz, int type)
: m_fn(0)
, m_keyFrmIdx(0)
, m_timeStamp_ms(0)
, m_img(sz, type)
{
}

DispFrm::~DispFrm()
{
}

DispFrm::DispFrm(const DispFrm &x)
: m_fn(x.m_fn)
, m_keyFrmIdx(x.m_keyFrmIdx)
, m_timeStamp_ms(x.m_timeStamp_ms)
, m_img(x.m_img)
{
}

//hard copy
DispFrm& DispFrm::operator = (const DispFrm &x)
{
if (this != &x) {
m_fn = x.m_fn;
m_keyFrmIdx = x.m_keyFrmIdx;
m_timeStamp_ms = x.m_timeStamp_ms;
x.m_img.copyTo(m_img);
}
return *this;
}


void DispFrm::dump(const std::string &folderPath) const
{
myImgWrite(m_img, folderPath, "img", m_fn, 0);
}

