/*
*-------------------------------------------------------------------
* DataContainer.cpp
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
#include "DataContainer.h"

using namespace autocut;
using namespace std;


DataContainer::DataContainer(int qSize)
: m_videoFrmQ( qSize )
, m_audioFrmQ( qSize )
, m_dispFrmQ( qSize )
{
}

DataContainer::~DataContainer()
{
}

void DataContainer::respReset()
{
m_videoFrmQ.clear();
m_audioFrmQ.clear();
m_dispFrmQ.clear();
}

void DataContainer::pushVideoFrm(const RawFrmPtr &f) 
{
m_videoFrmQ.push(f);
}

bool DataContainer::tryPopVideoFrm(RawFrmPtr &f) 
{
return m_videoFrmQ.try_pop(f);
}

bool DataContainer::waitPopVideoFrm(RawFrmPtr &f)
{
m_videoFrmQ.wait_and_pop(f);
return true;
}

void DataContainer::pushAudioFrm(const RawFrmPtr &f) 
{
m_audioFrmQ.push(f);
}

bool DataContainer::tryPopAudioFrm(RawFrmPtr &f) 
{
return m_audioFrmQ.try_pop(f);
}

bool DataContainer::waitPopAudioFrm(RawFrmPtr &f)
{
m_audioFrmQ.wait_and_pop(f);
return true;
}

void DataContainer::pushDispFrm(const DispFrmPtr &f)
{
m_dispFrmQ.push(f);
}

bool DataContainer::tryPopDispFrm(DispFrmPtr &f)
{
return m_dispFrmQ.try_pop(f);
}

bool DataContainer::waitPopDispFrm(DispFrmPtr &f)
{
m_dispFrmQ.wait_and_pop(f);
return true;
}


bool  DataContainer:: rawFrm2DispFrm(const RawFrmPtr &raw, DispFrm &disp)
{
if (raw == 0) {
return false;
}

disp.m_fn = raw->fn_;
disp.m_keyFrmIdx = raw->keyFrmId_;
disp.m_timeStamp_ms = 0;

cv::Size &desSz = disp.m_img.size();
if (raw->w_ != desSz.width || raw->h_ != desSz.height) {
cv::Mat oriImg;
raw->toBGR(oriImg);
cv::resize(oriImg, disp.m_img, desSz );
}
else {
raw->toBGR(disp.m_img);
}
return true;
}
