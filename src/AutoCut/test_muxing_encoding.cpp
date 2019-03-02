/*
*-------------------------------------------------------------------
* test_muxing_encoding.cpp
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
#include "libAutoCut/DemuxingDecoding2.h"
#include "libAutoCut/MuxingEncoding2.h"
#include "libAutoCut/ReadVideoUI.h"

using namespace std;
using namespace autocut;

int test_muxing_encoding(int argc, char **argv)
{
//read short video frms inserting between two clips
char *insertVideoFile = "C:/Projects/2018/AutoCut/data/BetweenClips_AShortOceanVideo.mp4";

int insertVieoFrmW, insertVideoFrmH;
std::vector<RawFrmPtr> vInsertVideoFrms, vInsertAudioFrms;
readVideo(insertVideoFile, true, true,insertVieoFrmW, insertVideoFrmH, vInsertVideoFrms, vInsertAudioFrms);

//read test1*.raw
int desW = 640, desH = 360;
char *fA1 = "C:/Projects/2018/AutoCut/data/test1_audio.raw";
char *fV1 = "C:/Projects/2018/AutoCut/data/test1_video.raw";
vector<char> rawA1;
vector<char> rawV1;
loadRaw(fA1, rawA1);
loadRaw(fV1, rawV1);

//read test2*.raw
char *fA2 = "C:/Projects/2018/AutoCut/data/test2_audio.raw";
char *fV2 = "C:/Projects/2018/AutoCut/data/test2_video.raw";
vector<char> rawA2;
vector<char> rawV2;
loadRaw(fA2, rawA2);
loadRaw(fV2, rawV2);

std::vector<RawFrmPtr> resizedInsertVideoFrms;
bool isResized = false;
if (desW != insertVieoFrmW || desH != insertVideoFrmH) {
resizeYuvVideoFrms(insertVieoFrmW, insertVideoFrmH, vInsertVideoFrms, desW, desH, resizedInsertVideoFrms );
isResized = true;
}

MyVideoStreamInfo vOutInfo;
vOutInfo.w = desW;
vOutInfo.h = desH;

MyAudioStreamInfo aOutInfo;
aOutInfo.channels = 2;
aOutInfo.frmSzInBytes = 4096;

MuxingEncoding2 x;
char *f0 = "C:/Projects/2018/AutoCut/data/out2.MP4";
x.openOutputFile(f0, vOutInfo, aOutInfo);

#if 0
x.testByWrtSynFrms();
#else
//write <test1*.raw>
uint64 vFrmSz = ( (desW *desH) * 3 ) / 2;
uint32 aFrmSz = 4096;
x.wrtSomeFrms( (uint8 *)rawV1.data(), rawV1.size(), vFrmSz, (uint8 *)rawA1.data(), rawA1.size(), aFrmSz);
//write inserted video
if (isResized) {
x.wrtSomeFrms(resizedInsertVideoFrms, vInsertAudioFrms, 1);
}
else {
x.wrtSomeFrms( vInsertVideoFrms, vInsertAudioFrms, 1 );
}
//write <test2*.raw>
x.wrtSomeFrms((uint8 *)rawV2.data(), rawV2.size(), vFrmSz, (uint8 *)rawA2.data(), rawA2.size(), aFrmSz);
#endif

x.closeOpenedFile();

return 0;
}
