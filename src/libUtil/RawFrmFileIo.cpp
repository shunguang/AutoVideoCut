/*
*-------------------------------------------------------------------
* RawFrmFileIo.cpp
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
#include "VisLog.h"
#include "RawFrmFileIo.h"
using namespace std;
using namespace autocut;
RawFrmFileIo::RawFrmFileIo()
{
}

bool RawFrmFileIo::saveRawFrms(const std::string &filePath, const std::vector<RawFrmPtr> &v)
{
RawFrmFileHd h;
h.nTotFrms = v.size();
//todo: 

ofstream myFile(filePath, ios::out | ios::binary);
if (!myFile.is_open()) {
xLog("RawFrmFileIo::saveRawFrms(): cannot open file: %s", filePath.c_str());
return false;
}

myFile.write((char*)&h, sizeof(RawFrmFileHd));
BOOST_FOREACH( const RawFrmPtr &frm, v) {
myFile.write((char*)&frm->type_, sizeof(FrmType));
myFile.write((char*)&frm->keyFrmId_, sizeof(uint64));
myFile.write((char*)&frm->fn_, sizeof(uint64));
myFile.write((char*)&frm->sz_, sizeof(uint64));
myFile.write((char*)&frm->w_, sizeof(int));
myFile.write((char*)&frm->h_, sizeof(int));
myFile.write((char*)frm->buf_, frm->sz_);
}
myFile.close();
return true;
}

bool RawFrmFileIo::loadRawFrms(const std::string &filePath, std::vector<RawFrmPtr> &v)
{
RawFrmFileHd h;
ifstream myFile(filePath, ios::in | ios::binary);
if (!myFile.is_open()) {
xLog("RawFrmFileIo::loadRawFrms(): cannot open file: %s", filePath.c_str());
return false;
}


FrmType  type_;   //video or audio
uint64   keyFrmId_;		//the index of key frms
uint64   fn_;     //frm # in original video or audio
uint64   sz_;     //buf size
int      w_;      //width and height if its a video frm
int      h_;
v.clear();
myFile.read( (char*)&h, sizeof(RawFrmFileHd) );
for (uint64 i = 0; i < h.nTotFrms; ++i) {
myFile.read((char*)&type_, sizeof(FrmType));
myFile.read((char*)&keyFrmId_, sizeof(uint64));
myFile.read((char*)&fn_, sizeof(uint64));
myFile.read((char*)&sz_, sizeof(uint64));
myFile.read((char*)&w_, sizeof(int));
myFile.read((char*)&h_, sizeof(int));

RawFrmPtr newFrm( new RawFrm( type_, fn_, w_, h_, sz_) );
myFile.read((char*)newFrm->buf_, sz_);
newFrm->keyFrmId_ = keyFrmId_;
v.push_back(newFrm);
}
myFile.close();
return true;
}


bool RawFrmFileIo::saveFrmIdxQ(const std::string &filePath, const std::vector<FrmIdx> &v)
{
RawFrmFileHd h;
h.nTotFrms = v.size();
//todo: 

ofstream myFile(filePath, ios::out | ios::binary);
if (!myFile.is_open()) {
xLog("RawFrmFileIo::saveFrmIdxQ(): cannot open file: %s", filePath.c_str());
return false;
}

myFile.write((char*)&h, sizeof(RawFrmFileHd));
BOOST_FOREACH(const FrmIdx &frm, v) {
myFile.write((char*)&frm, sizeof(FrmIdx));
}
myFile.close();
return true;

}

bool RawFrmFileIo::loadFrmIdxQ(const std::string &filePath, std::vector<FrmIdx> &v)
{
RawFrmFileHd h;
ifstream myFile(filePath, ios::in | ios::binary);
if (!myFile.is_open()) {
xLog("RawFrmFileIo::loadFrmIdxQ(): cannot open file: %s", filePath.c_str());
return false;
}

FrmIdx frm;
v.clear();
myFile.read((char*)&h, sizeof(RawFrmFileHd));
for (uint64 i = 0; i < h.nTotFrms; ++i) {
myFile.read((char*)&frm, sizeof(FrmIdx));
v.push_back(frm);
}
myFile.close();
return true;

}

