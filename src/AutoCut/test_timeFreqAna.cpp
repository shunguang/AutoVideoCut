/*
*-------------------------------------------------------------------
* test_timeFreqAna.cpp
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
#include "libyuv/libyuv.h"
#include "libutil/UtilFuncs.h"

#include "libAutoCut/avUtils.h"
#include "libAutoCut/TimeFreqAna.h"

using namespace std;
using namespace autocut;
using namespace libyuv;


int test_timeFreqAna(int argc, char **argv)
{
int fs = 48000;
CfgTimeFreqAna cfg;
cfg.winSz = fs / 10;
cfg.stepSz = fs / 20;
cfg.nFFT = 12000;
float hilightSegLengthInSec = 20;
const std::string dirName = "E:/autoCutAudioData/raw";
std::vector<std::string> vFileNames;
uint32_t nFiles = getFileNameList(dirName, "raw", vFileNames);

std::string folderName, fileName, fname_wo_ext, ext, dumpBase;
for (int i = 0; i < nFiles; ++i) {
//char *rawFile = "E:/autoCutAudioData/raw/EDPOpenCup_SACBAPreAcademy2004vsPipelinePreAcademy_OT1_audio.raw";
splitExt(vFileNames[i], fname_wo_ext, ext);
int L = fname_wo_ext.length();
dumpBase = "c:/temp/" + fname_wo_ext.substr(0,L-6);

fileName = dirName + "/" + vFileNames[i];
const char *rawFile = vFileNames[i].c_str();
vector<char> rawV;
loadRaw(rawFile, rawV);
int sz = rawV.size();
RawFrm x(FrmType_A_FRM, 0, sz);
x.copyFrom((const uint8*)rawV.data(), sz, 0);
TimeFreqAna y(cfg);
y.process(x, fs, hilightSegLengthInSec, dumpBase);
}
return 0;
}
