/*
*-------------------------------------------------------------------
* RawFrm.cpp
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

#include "RawFrm.h"

using namespace std;
using namespace autocut;

RawFrm::RawFrm(const FrmType type, const uint64 fn, const uint64 sz )
: type_(type), fn_(fn), sz_(sz), buf_(NULL)
, w_(NAN), h_(NAN)
, keyFrmId_(0)
, offset_(0)
{
creatBuf();
}

RawFrm::RawFrm(const FrmType type, const uint64 fn, const int w, const int h, const int sz)
: type_(type), fn_(fn)
, w_(w), h_(h)
, sz_(sz), buf_(NULL)
, keyFrmId_(0)
, offset_(0)
{
creatBuf();
}

RawFrm::RawFrm(const cv::Size &imgSz, const uint64 fn)
: type_(FrmType_V_FRM)
, fn_(fn)
, sz_((imgSz.width*imgSz.height *3)/2)
, w_(imgSz.width), h_(imgSz.height)
, buf_(NULL)
, keyFrmId_(0)
, offset_(0)
{
creatBuf();
}

//soft copy 
RawFrm::RawFrm(const FrmType type, const uint64 fn, const int w, const int h, uint8 *buf, int bufSz)
: type_(FrmType_V_FRM)
, fn_(fn)
, sz_(bufSz)
, w_(w), h_(h)
, buf_( buf)
, keyFrmId_(0)
, offset_(0)
, isAllocatedBuf_(false)
{
#if DEBUG_
assert( bufSz == (w*h*3)/2);
#endif
}

RawFrm::~RawFrm()
{
deleteBuf();
}

RawFrm::RawFrm(const RawFrm &x) 
: type_(x.type_)
, fn_(x.fn_)
, sz_(x.sz_)
, buf_(NULL)
, w_ (x.w_)
, h_ (x.h_)
, keyFrmId_( x.keyFrmId_ )
, offset_( x.offset_)
{
creatBuf();
memcpy_s(buf_, sz_, x.buf_, x.sz_);
}

RawFrm &RawFrm::operator = (const RawFrm &x) 
{
if (&x != this) { //check for self assignment
if (sz_ != x.sz_) {
deleteBuf();
sz_ = x.sz_;
creatBuf();
}

type_ = x.type_;
fn_ = x.fn_;
memcpy_s(buf_, sz_, x.buf_, x.sz_);
w_ = x.w_;
h_ = x.h_;
keyFrmId_ = x.keyFrmId_;
offset_ = x.offset_;
}

return *this; // enables x=y=z;
}

void RawFrm::resetSz(FrmType newType, const uint64 newSz )
{
type_ = newType;
if (newSz != sz_) {
deleteBuf();
sz_ = newSz;
creatBuf();
}
}

void RawFrm::creatBuf()
{

if( sz_ > 0 ){
isAllocatedBuf_ = true;
buf_ = new uint8[sz_];
if (!buf_) {
errMsg("RawFrm::creatBuf(): cannot allocate memory!");
}
}
else {
buf_ = NULL;
}
}

void RawFrm::deleteBuf()
{
if (isAllocatedBuf_) {
delete[] buf_;
buf_ = NULL;
}
}

void RawFrm::errMsg(const char* msgStr) const
{
//debug version
cerr << msgStr << endl;
getchar();

//release version
throw runtime_error(msgStr);
}
