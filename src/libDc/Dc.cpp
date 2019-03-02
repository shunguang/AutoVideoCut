/*
*-------------------------------------------------------------------
* Dc.cpp
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
#include "Dc.h"

#define IS_CHECK_TIME_PROFILE		1
#if  IS_CHECK_TIME_PROFILE
#include <time.h>
#endif

using namespace std;
using namespace cv;
using namespace autocut;

Dc::Dc()
: m_cfg(0)
, m_size(0)
{
for (uint32 i = 0; i < APP_MAX_THREADS; ++i) {
m_head[i] = 0;
m_nTotFrmWrt2q[i] = 0;
m_isFull[i] = false;
m_curFrmIdx[i] = -1;
}
}

Dc::Dc(CfgAc *cfg)
: m_cfg(cfg)
{
for (uint32 i = 0; i < APP_MAX_THREADS; ++i) {
m_head[i] = 0;
m_nTotFrmWrt2q[i] = 0;
m_isFull[i] = false;
m_curFrmIdx[i] = -1;
}
allocQ();
}

Dc::~Dc()
{
}

void Dc::allocQ()
{
//const uint32 w0, const uint32 h0,
//	const uint32 inputImgDepth, const int inputImgChannels, const uint32 qSize
for (uint32 i = 0; i < APP_MAX_THREADS; ++i) {
m_head[i] = 0;
m_nTotFrmWrt2q[i] = 0;
m_isFull[i] = false;
}

m_size = m_cfg->m_dc.qSize;
m_q.clear();
for (uint32 i = 0; i < m_size; ++i) {
FrmSamplePtr p(new FrmSample( m_cfg ));
m_q.push_back(p);
}

}

//frmsample was written into q, but after process we found it is a "bad", we just want to ignore it
void Dc::ignorCurFrmIdx(const ThreadId_t tId)
{
uint32  &curFrmIdx = m_curFrmIdx[tId];
uint32  &head = m_head[tId];

if (m_nTotFrmWrt2q[tId] > m_size) {
if (head > 0) {
--head;
}
else {
head = m_size - 1;
}
m_nTotFrmWrt2q[tId] -= 1;
m_isFull[tId] = true;
}
else {
if (head > 0) {
--head;
m_nTotFrmWrt2q[tId] -= 1;
}
else {
head = 0;
}
m_isFull[tId] = false;
}

curFrmIdx = head;
m_curFrm[tId] = (m_q[curFrmIdx]).get();
}

void Dc:: setCurFrmIdx(const ThreadId_t tId)
{
uint32  &curFrmIdx = m_curFrmIdx[tId];
uint32  &head = m_head[tId];
m_nTotFrmWrt2q[tId] += 1;

curFrmIdx = head;
m_curFrm[tId] = (m_q[curFrmIdx]).get();

//move head to the next slot
++head;
if( head >= m_size ){
head = 0;
m_isFull[tId] = 1;
}
}

void Dc :: reset(const int newW_L0, const int newH_L0)
{
m_mutexRW.lock();

for (uint32 i = 0; i < APP_MAX_THREADS; ++i) {
m_head[i] = 0;
m_isFull[i] = false;
m_curFrmIdx[i] = -1;
m_nTotFrmWrt2q[i] = 0;
}

for (uint32 i = 0; i<m_size; ++i) {
m_q[i]->updateFrmSize(newW_L0, newH_L0);
}

m_mutexRW.unlock();
}



int  Dc :: nEffectiveItems(const ThreadId_t tId) const
{
if( !m_isFull[tId] )
return m_head[tId];
else
return m_size;
}

bool Dc:: isEmpty(const ThreadId_t tId) const
{
if( m_isFull[tId] )
return false;
else {
if ( m_head[tId] == 0 )
return true;
else
return false;
}

}


int Dc ::  getCurFrmIdx(const ThreadId_t tId) const
{
return m_curFrmIdx[tId];
}

FrmSample* Dc::getFrmSmpByQueIdx( const int frmIdx )
{
if( frmIdx>=0 && frmIdx< m_size ){
return ( (m_q[frmIdx]).get() );
}

return NULL;
}


FrmSample*  Dc :: getCurFrm(const ThreadId_t tId)
{
return m_curFrm[tId];
}

FrmSample* Dc :: getPreFrm( const int frmDist, const ThreadId_t tId)
{
int idx = m_curFrmIdx[tId];
idx -= frmDist;
if( m_isFull[tId] ){
if ( idx<0 ){
idx +=m_size;
if( idx<0 ){
printf( "Dc :: getPreFrmSample(): frmDist is too large!" );
return NULL;
}
}
}
else {
if ( idx < 0 ){
printf( "Dc :: getPreFrmSample(): frmDist is too large!" );
return NULL;
}
}
return ((m_q[idx]).get());
}



//this is the last call about <m_q[qIdx]>
DispSample   Dc::getDispSmp_hdcp(const int qIdx, uint32 &frmNum)
{
DispSample x;
if (qIdx < 0 || qIdx >= m_size) {
xLog("Dc::getDispSmp_hdcp(): idx out of order!");
return x;
}


FrmSamplePtr &fs = m_q[qIdx];
m_mutexRW.lock();
fs->m_dispSmp.hdCopyTo(x);
frmNum = fs->m_fn;
fs->close4FutherUsing();
m_mutexRW.unlock();

return x;
}

