/*
*-------------------------------------------------------------------
* RawFrm.h
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

#ifndef __RAW_FRM_DATA_H__
#define __RAW_FRM_DATA_H__

#include "DataTypes.h"
#include "UtilDefs.h"
namespace autocut {
enum FrmType
{
FrmType_V_FRM = 0,          //video frm
FrmType_A_FRM,              //audio frm
FrmType_UNKN                //audio frm
};

struct FrmIdx {
FrmIdx(const uint8 type_ = 0, const uint64 idx_ = 0, const uint64  nsamples_ = 0, const uint64  frmSzInByte_ = 0)
: type(type_), idx(idx_), nsamples(nsamples_), frmSzInByte(frmSzInByte_) {}

uint8   type;
uint64  idx;
uint64  nsamples;             //# of smaples for a audio frm
uint64  frmSzInByte;
};

struct FrmRange {
FrmRange(const uint64 vid1_ = 0, const uint64 vid2_ = 0, const uint64 aid1_ = 0, const uint64 aid2_ = 0)
: vid1(vid1_), vid2(vid2_), vFrmSzInByte(0), aid1(aid1_), aid2(aid2_), aFrmSzInByte(0){}
FrmRange(const FrmRange &x) : vid1 (x.vid1), vid2 (x.vid2),	vFrmSzInByte (x.vFrmSzInByte),aid1 (x.aid1), aid2 (x.aid2),	aFrmSzInByte (x.aFrmSzInByte){}
FrmRange& operator =(const FrmRange &x) {
if (this != &x) {
vid1 = x.vid1;
vid2 = x.vid2;
vFrmSzInByte = x.vFrmSzInByte;

aid1 = x.aid1;
aid2 = x.aid2;
aFrmSzInByte = x.aFrmSzInByte;
}
return *this;
}


bool isInRngV(const uint64 id) const {	
return (id >= vid1 && id <= vid2);
}

bool isInRngA(const uint64 id) const {
return (id >= aid1 && id <= aid2);
}

//for acending sorting 
bool operator < (const FrmRange& x) const{
return ( aid1 < x.aid1 );
}

//for descending  sorting 
bool operator > (const FrmRange& x) const {
return (aid1 > x.aid1);
}

std::string toStr() const {
char buf[128];
sprintf_s(buf, 128, "vid[%llu,%llu], vFrmSzInByte=%llu; aid[%llu,%llu], aFrmSzInByte=%llu", vid1, vid2, vFrmSzInByte, aid1, aid2, aFrmSzInByte);
return std::string(buf);
}

std::string toStr2() const {
char buf[128];
sprintf_s(buf, 128, "aid[%llu,%llu], aFrmSzInByte=%llu, nFrms=%llu", aid1, aid2, aFrmSzInByte, aid2-aid1);

return std::string(buf);
}

uint64  vid1, vid2;  //video frm indices
uint64  vFrmSzInByte;

uint64  aid1, aid2;  //audio frm indices
uint64  aFrmSzInByte;
};

class UTIL_EXPORT RawFrm {
public:
RawFrm(const FrmType type = FrmType_UNKN, const uint64 fn = 0, const uint64 sz = 1024);
RawFrm(const FrmType type, const uint64 fn, const int w, const int h, const int sz);
RawFrm(const cv::Size &imgSz, const uint64 fn);

//soft copy
//assert(bufSz == w*h*3/2)
RawFrm(const FrmType type, const uint64 fn, const int w, const int h, uint8 *buf_, int bufSz);

RawFrm(const RawFrm &x);
~RawFrm();
//hard copy
RawFrm& operator = (const RawFrm &x);

void resetSz( FrmType newType, const uint64 newSz );

void toBGR(cv::Mat &picBGR ) {
cv::Mat picYV12 = cv::Mat( h_ * 3 / 2, w_, CV_8UC1, buf_);
cv::cvtColor(picYV12, picBGR, CV_YUV420p2RGB);
}

void setToZeros() {
memset( buf_, 0, sz_ );
}

uint64 getNumOfFloatSmpPts() const {
return sz_ / sizeof(float);
}

uint64 getNumOfUint8SmpPts() const {
return sz_ ;
}

uint64 copyFrom(const uint8 *buf, const uint64 sz, const uint64 fn=0) {
if (sz != sz_) {
printf("RawFrm::copyFrom(): buf sz doest match, data loss!\n");
assert(0);
}
memcpy_s( buf_, sz_, buf, sz);
fn_ = fn;
return sz;
}

uint64 copyTo( uint8 *buf, const uint64 sz, uint64 &fn) {
if (sz != sz_) {
printf("RawFrm::copyTo(): buf sz doest match, data loss!\n");
assert(0);
}
memcpy_s(buf, sz, buf_, sz_);
fn = fn_;
return sz_;
}

void dump(const std::string &folder) {
char buf[1024];
snprintf(buf, 1024, "%s/fn-%05d.png", folder.c_str(), fn_);
cv::Mat bgr;
toBGR(bgr);
cv::imwrite(buf, bgr);
}

private:
void errMsg(const char* msgStr) const;
void creatBuf();
void deleteBuf();

public:
FrmType  type_;			//video or audio
uint64   keyFrmId_;		//the index of key frms
uint64   fn_;			//frm # in original video or audio
uint64   sz_;			//buf size
int      w_;			//width and height if its a video frm
int      h_;
int64    offset_;       //offset in Bytes  in original file

bool	isAllocatedBuf_; //if only construct a head, we do not allocated buf on it
uint8   *buf_;    //for nonplanar audio, <buf_> is filled by [smp1_ch1,smp1_ch2,smp2_ch1,smp2_ch2, ..., smpn_ch1,smpn_ch2] assume nChs=2, similar for nChs>2
//for planar audio, <buf_> is filled by [smp1_ch1,smp2_ch1, ..., smpn_ch1, smp1_ch2, smp2_ch2, ..., smpn_ch2] assume nChs=2, similar for  nChs>2
//
//for video: it's the yuv420p format
//where YUV420p is a planar format, meaning that the Y, U, and V values are grouped together instead of interspersed.
//The reason for this is that by grouping the U and V values together, the image becomes much more compressible.
//When given an array of an image in the YUV420p format, all the Y? values come first, followed by all the U values, 
//followed finally by all the V values.
//to see the image: call yuv420p_save(const uint8 *yuv420pRaw, const int sz, const int w, const int h, const char *fname);
};
typedef std::shared_ptr<RawFrm>		RawFrmPtr;

class UTIL_EXPORT RawClip
{
public:
RawClip() {};

public:
std::vector<RawFrm> m_vVideoFrms, m_vAudioFrms;
};
typedef std::shared_ptr<RawClip>		RawClipPtr;

class UTIL_EXPORT RawClip2
{
public:
RawClip2() {};

public:
std::vector<RawFrmPtr> m_vVideoFrms, m_vAudioFrms;
};
typedef std::shared_ptr<RawClip2>		RawClip2Ptr;

}
#endif
