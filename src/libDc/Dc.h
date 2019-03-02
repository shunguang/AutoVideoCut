/*
*-------------------------------------------------------------------
* Dc.h
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
#ifndef _DC_H_
#define _DC_H_

#include "DcDefs.h"
#include "FrmSample.h"

#define APP_MAX_THREADS		5
namespace autocut {

class DC_EXPORT Dc{

public:
Dc();
Dc(CfgAc *cfg);
~Dc();

void		reset(const int newW_L0, const int newH_L0);

bool		isEmpty(const ThreadId_t tid = Thread_ONE) const;
int			nEffectiveItems(const ThreadId_t tid = Thread_ONE) const;

void		ignorCurFrmIdx(const ThreadId_t tid = Thread_ONE);
void		setCurFrmIdx(const ThreadId_t tid = Thread_ONE);
int			getCurFrmIdx(const ThreadId_t tid = Thread_ONE) const;

FrmSample*	getFrmSmpByQueIdx( const int qIdx );
FrmSample*  getCurFrm(const ThreadId_t tid = Thread_ONE);
//assume cur frm # is $n$, thsi function retur the FrmSample at $n-frmDist$
FrmSample*  getPreFrm( const int frmDist=1, const ThreadId_t tid = Thread_ONE);

DispSample   getDispSmp_hdcp(const int qIdx, uint32 &frmNum);
protected:
void allocQ();

//data 
private:
CfgAc		*m_cfg;
//-----------------------------------------------------------------------------
//access queue
//-----------------------------------------------------------------------------
uint32				m_size;								//the predefined size of the queue.
uint32				m_head[APP_MAX_THREADS];			//the index to write	
uint32				m_curFrmIdx[APP_MAX_THREADS];	//the current frm index in Q
bool				m_isFull[APP_MAX_THREADS];
FrmSample			*m_curFrm[APP_MAX_THREADS];

std::vector< FrmSamplePtr >	m_q;

//---temperoal add for ignorCurFrmIdx() ---------------
uint64             m_nTotFrmWrt2q[APP_MAX_THREADS];   //total # of frms writen into q since start 

boost::mutex m_mutexRW;
};

typedef std::shared_ptr<Dc>		DcPtr;
}

#endif		
