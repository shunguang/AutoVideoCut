/*
*-------------------------------------------------------------------
* DataContainer.h
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
#ifndef _DATA_CONTAINER_H_
#define _DATA_CONTAINER_H_

#include "UtilDefs.h"
#include "FifoQue.h"
#include "RawFrm.h"
#include "DispFrm.h"
#include "CfgAc.h"
namespace autocut {
class UTIL_EXPORT DataContainer{
public:
DataContainer( int qSize=50);
~DataContainer();

static bool  rawFrm2DispFrm(const RawFrmPtr &raw, DispFrm &disp);

void respReset();

bool isVideoQueFull() { 
return m_videoFrmQ.isFull(); 
}
bool isAudioQueFull() {
return m_audioFrmQ.isFull();
}
bool isVideoQueEmpty() {
return m_videoFrmQ.empty();
}
bool isAudioQueEmpty() {
return m_audioFrmQ.empty();
}
void clearVideoQ() {
m_videoFrmQ.clear();
}
void clearAudioQ() {
m_audioFrmQ.clear();
}
void clearDispQ() {
m_dispFrmQ.clear();
}

void pushVideoFrm( const RawFrmPtr &f);
void pushAudioFrm(const RawFrmPtr &f);
void pushDispFrm(const DispFrmPtr &f);

bool tryPopVideoFrm(RawFrmPtr &f);
bool waitPopVideoFrm(RawFrmPtr &f);

bool tryPopAudioFrm(RawFrmPtr &f);
bool waitPopAudioFrm(RawFrmPtr &f);

bool tryPopDispFrm(DispFrmPtr &f);
bool waitPopDispFrm(DispFrmPtr &f);

protected:
FifoQue<RawFrmPtr>				m_videoFrmQ;
FifoQue<RawFrmPtr>				m_audioFrmQ;

//todo change to cirqular que
FifoQue<DispFrmPtr>				m_dispFrmQ;
};

typedef std::shared_ptr<DataContainer>		DataContainerPtr;
}

#endif		
