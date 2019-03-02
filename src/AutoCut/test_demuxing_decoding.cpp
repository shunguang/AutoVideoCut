/*
*-------------------------------------------------------------------
* test_demuxing_decoding.cpp
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
#include "libUtil/VisLog.h"
#include "libUtil/UtilFuncs.h"
#include "libAutoCut/DemuxingDecoding2.h"
#include "libAutoCut/ReadVideoUI.h"

using namespace std;
using namespace autocut;

int test_demuxing_decoding1(int argc, char **argv);
int test_demuxing_decoding2(int argc, char **argv);
int test_seek(int argc, char **argv);
int test_batch_preDecode(int argc, char **argv);

int test_demuxing_decoding(int argc, char **argv)
{
int x;
//x=test_demuxing_decoding1(argc, argv);
//x=test_demuxing_decoding2(argc, argv);
//x= test_seek(argc, argv);
x = test_batch_preDecode(argc, argv);

return x;
}

int test_demuxing_decoding1(int argc, char **argv)
{
const char *filePath = "C:/Projects/AutoCut/data/head1.mp4";
const char *filePath2 = "C:/Projects/AutoCut/data/PreAcademySoccer/SACBaltimoreArmourPreAcademyvsSouthCarolinaUnitedFCElite_Part1.MTS";

MyVideoStreamInfo vInfo;
MyAudioStreamInfo aInfo;
getVideoInfo(filePath, vInfo, aInfo);

getVideoInfo(filePath2, vInfo, aInfo);

const bool rVideo = true;
const bool rAudio = true;
int  vdieoFrmW, vdieoFrmH;
std::vector<RawFrmPtr> vFrm;
std::vector<RawFrmPtr> aFrm;
readVideo( filePath,  rVideo, rAudio, vdieoFrmW, vdieoFrmH, vFrm, aFrm);
return 0;
}


int test_demuxing_decoding2(int argc, char **argv)
{
CfgDemuxingDecodingPtr  cfg ( new CfgDemuxingDecoding() );
cfg->demuxingVideo = 1;
cfg->dumpRawVideo = 0;
cfg->storeRawVideoInMemo = false;

cfg->demuxingAudio = 1;
cfg->dumpRawAudio = 1;
cfg->storeRawAudioInMemo = true;

char *f0 = "C:/Projects/2018/AutoCut/data/big_buck_bunny.mp4 ";
char *f1 = "C:/Projects/2018/AutoCut/data/v.raw";
char *f2 = "C:/Projects/2018/AutoCut/data/a.raw";
bool isRefCount = false;
uint64 vFrmSzInBytes, aFrmSzInBytes;

DemuxingDecoding2 x(cfg);
x.openVideoFileToRead(f0, isRefCount);
x.decodeAll(f1, f2);
aFrmSzInBytes = x.getRawAudioFrameSize();
vFrmSzInBytes = x.getRawVideoFrameSize();
x.closeOpenedFiles();

//prepare frm ranges
uint32 aId1, aId2, vId1, vId2;
FrmRange frmRng[4];
aId2 = 50;
for (int i = 0; i < 4; ++i) {
aId1 = aId2 + i * 100;
aId2 = aId1 + 100;
x.findVideoFrmIds(aId1, aId2, vId1, vId2);  //make sure video cliips is inside audio range
FrmRange r(vId1, vId2, aId1, aId2);
r.aFrmSzInByte = aFrmSzInBytes;
r.vFrmSzInByte = vFrmSzInBytes;

frmRng[i] = r;
}

//read raw video and audio in each <frmRng>
cfg->demuxingVideo = 1;
cfg->dumpRawVideo = 0;
cfg->storeRawVideoInMemo = true;
cfg->demuxingAudio = 1;
cfg->dumpRawAudio = 0;
cfg->storeRawAudioInMemo = true;

uint64 vSzAllocated, aSzAllocated;
uint64 vSzRead, aSzRead;
x.openVideoFileToRead(f0, isRefCount);
for (int i = 0; i < 4; ++i) {
x.decodeSomeVideo(1, true);
#if 0
vSzAllocated = (r.vid2 - r.vid1 + 1) * vFrmSzInBytes;
aSzAllocated = (r.aid2 - r.aid1 + 1) * aFrmSzInBytes;
uint8_t *rawVideo = (uint8_t *)malloc(vSzAllocated * sizeof(uint8_t));
uint8_t *rawAudio = (uint8_t *)malloc(aSzAllocated * sizeof(uint8_t));
//todo 

//process <rawVideo> <rawAudio>
string f = string("c:/temp/V") + to_string(i) + ".raw";
dumpRaw(f.c_str(), rawVideo, vSzRead);
f = string("c:/temp/A") + to_string(i) + ".raw";
dumpRaw(f.c_str(), rawAudio, aSzRead);

//free memoery
free(rawVideo);
free(rawAudio);
#endif
}
return 0;
}


//test random access
int test_seek(int argc, char **argv)
{
CfgDemuxingDecodingPtr  cfg (new CfgDemuxingDecoding() );
//video params
cfg->demuxingVideo = 1;
cfg->dumpRawVideo = 0;
cfg->storeRawVideoInMemo = 0;
cfg->storeThumbnailKeyFrms = 1;
cfg->thumbnailWidth = 240;
cfg->thumbnailHeight = 130;
cfg->outputImgWidth = -1;
cfg->outputImgHeight = -1;

//audio params
cfg->demuxingAudio = 0;
cfg->dumpRawAudio = 0;
cfg->storeRawAudioInMemo = 0;


//char *f0 = "C:/Projects/AutoCut/data/testRandomReadFrms/big_buck_bunny-highlight1.mp4";
char *f0 = "C:/Projects/AutoCut/data/PreAcademySoccer/testA.mp4";
bool isRefCount = false;
uint64 vFrmSzInBytes, aFrmSzInBytes;

//generate profile file names	
DecodedProfileFiles pf;
string ff, head, tail;
split(f0, head, tail);
ff = "c:/temp/projectProfile/" + tail + ".";
pf.m_audioFrmFile = ff + "audioFrms.bin";
pf.m_audioStreamInfoFile = ff + "audioStreamInfo.bin";
pf.m_videoKeyFrmFile = ff + "vidoKeyFrms.bin";
pf.m_videoStreamInfoFile = ff + "vidoStreamInfo.bin";
pf.m_frmIdxFile = ff + "frmIdxQ.bin";


DemuxingDecoding2 x(cfg);
x.setDecodedProfileFileNames(pf);
x.openVideoFileToRead(f0, isRefCount);
xLog("start");

bool suc = x.loadDecodedAudioVideoFrms();
if (!suc) {
x.decodeAll(NULL, NULL);
x.update_audio_streamInfo();
x.update_video_streamInfo();
x.saveDecodedAudioVideoFrms();
}
x.printProperties();

for (uint32 i = 0; i < x.m_vVideoFrms.size(); i += 1) {
const RawFrmPtr &frm = x.m_vVideoFrms[i];
//frm->dump("c:/temp");
printf("keyIdx=%" PRIu64, ",fn=%" PRIu64, ",offset=%" PRId64 "\n", (long long)frm->keyFrmId_, frm->fn_, frm->offset_);
}

if (0) {
//test seekByOffset();
int64_t vOff[6] = { 32816,294960,688176,983088,1081392, 1212464 };
uint64  vKeyId[6] = { 0,10,20,30, 40,50 };
uint64  vFn[6] = { 0, 109,215,311,399,484 };
for (int i = 0; i < 6; i++) {
x.seekByOffset(vOff[i], vFn[i], vKeyId[i]);
x.decodeSome(true, 1, true, 0);
for (uint32 j = 0; j < x.m_vVideoFrms.size(); j += 1) {
const RawFrmPtr &frm = x.m_vVideoFrms[j];
frm->dump("c:/temp/t");
}
}
}

if (1) {
uint64 n = x.m_vThumbnailKeyFrms.size();
uint64 vKeyFrmIdx[] = { n / 10, n / 2, n / 10, 0 };
for ( const uint64 id : vKeyFrmIdx ) {
float64 t_sec = x.seekVideoFrm( id );
x.decodeSome(true, 1, true, 2);
for (const RawFrmPtr frm : x.m_vVideoFrms) {
frm->dump("c:/temp/t");
}
}
}

aFrmSzInBytes = x.getRawAudioFrameSize();
vFrmSzInBytes = x.getRawVideoFrameSize();
int64_t nDuration = x.getDuration();
uint32 nTotVideoFrms = x.getTotalVideoFrms();
uint32 nTotAudioFrms = x.getTotalAudioFrms();

x.closeOpenedFiles();


xLog("well done!");
return 0;
}


#include "libAutoCut/ProjProfile.h"
int test_batch_preDecode(int argc, char **argv)
{
CfgDemuxingDecodingPtr  cfg(new CfgDemuxingDecoding());
//video params
cfg->demuxingVideo = 1;
cfg->dumpRawVideo = 0;
cfg->storeRawVideoInMemo = 0;
cfg->storeThumbnailKeyFrms = 1;
cfg->thumbnailWidth = 240;
cfg->thumbnailHeight = 130;
cfg->outputImgWidth = -1;
cfg->outputImgHeight = -1;
//audio params
cfg->demuxingAudio = 1;
cfg->dumpRawAudio = 0;
cfg->storeRawAudioInMemo = 1;


string cfgFile = "C:/Projects/AutoCut/src/AutoCut/res/cfgAutoCut_2017Games.xml";
CfgAcPtr cfg0(new CfgAc());
cfg0->readFromFile(cfgFile.c_str());
for (const CfgGame &g : cfg0->m_vGames) {
ProjProfilePtr projProfile(new ProjProfile());
string 	p0 = cfg0->getProjProfilerFolder();
projProfile->setRootFolder(p0);
projProfile->setCfg( g );
projProfile->genFileNames();

int  i = 0;
for (const string &f0 : g.vInputVideoFileNames ) {

//generate profile file names	
const DecodedProfileFiles &pf = projProfile->m_vProfFiles[i++];

DemuxingDecoding2 x(cfg);
x.setDecodedProfileFileNames(pf);
string srcVideoFile = g.inputVideoFolder + "/" + f0;
xLog("start: %s", srcVideoFile.c_str());

int ret = x.openVideoFileToRead(srcVideoFile.c_str(), false);
if (ret < 0) {
continue;
}

//bool suc = x.loadDecodedAudioVideoFrms();
//if (!suc) {
ret = x.decodeAll(NULL, NULL);
if (ret >= 0) {
x.update_audio_streamInfo();
x.update_video_streamInfo();
x.saveDecodedAudioVideoFrms();
}
//}
x.printProperties();
}
}
return 0;
}

