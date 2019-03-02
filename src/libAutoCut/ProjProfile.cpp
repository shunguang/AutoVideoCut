/*
*-------------------------------------------------------------------
* ProjProfile.cpp
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
#include "ProjProfile.h"

using namespace std;
using namespace autocut;

ProjProfile::ProjProfile()
: m_rawFrmFileIo( new RawFrmFileIo() )
{
}

ProjProfile::~ProjProfile()
{
}

void ProjProfile::respReset()
{

}

//-------end static funcs ----
void ProjProfile::setCfg(const CfgGame &cfg)
{
m_cfgGame = cfg;
}

void ProjProfile::setRootFolder(const std::string &rootFolder)
{
m_profileRootFolder = rootFolder;
}

bool ProjProfile::saveMp3Frms(const std::vector<RawFrmPtr> &v )
{
return m_rawFrmFileIo->saveRawFrms(m_mp3FrmFile, v);
}

bool ProjProfile::loadMp3Frms(std::vector<RawFrmPtr> &v)
{
return m_rawFrmFileIo->loadRawFrms(m_mp3FrmFile, v);
}

bool ProjProfile::saveAudioFrms(const int fileId, const std::vector<RawFrmPtr> &v)
{
return m_rawFrmFileIo->saveRawFrms(m_vProfFiles[fileId].m_audioFrmFile, v);
}
bool ProjProfile::loadAudioFrms(const int fileId, std::vector<RawFrmPtr> &v)
{
return m_rawFrmFileIo->loadRawFrms(m_vProfFiles[fileId].m_audioFrmFile, v);
}

bool ProjProfile::saveVideoKeyFrms(const int fileId, const std::vector<RawFrmPtr> &v)
{
return m_rawFrmFileIo->saveRawFrms(m_vProfFiles[fileId].m_videoKeyFrmFile, v);
}
bool ProjProfile::loadVideoKeyFrms(const int fileId, std::vector<RawFrmPtr> &v)
{
return m_rawFrmFileIo->loadRawFrms(m_vProfFiles[fileId].m_videoKeyFrmFile, v);
}

bool ProjProfile::saveFrmIdxQ(const int fileId, const std::vector<FrmIdx> &v)
{
return m_rawFrmFileIo->saveFrmIdxQ(m_vProfFiles[fileId].m_frmIdxFile, v);
}

bool ProjProfile::loadFrmIdxQ(const int fileId, std::vector<FrmIdx> &v)
{
return m_rawFrmFileIo->loadFrmIdxQ(m_vProfFiles[fileId].m_frmIdxFile, v);
}

bool ProjProfile::saveAudioStreamInfo(const int fileId, const MyAudioStreamInfo &info)
{
const string &f = m_vProfFiles[fileId].m_audioStreamInfoFile;
return autocut::saveAudioStreamInfo(f, info);
}
bool ProjProfile::loadAudioStreamInfo(const int fileId, MyAudioStreamInfo &info)
{
const string &f = m_vProfFiles[fileId].m_audioStreamInfoFile;
return autocut::loadAudioStreamInfo(f, info);
}

bool ProjProfile::saveVideoStreamInfo(const int fileId, const MyVideoStreamInfo &info)
{
const string &f = m_vProfFiles[fileId].m_videoStreamInfoFile;
return autocut::saveVideoStreamInfo(f, info);
}
bool ProjProfile::loadVideoStreamInfo(const int fileId, MyVideoStreamInfo &info)
{
const string &f = m_vProfFiles[fileId].m_videoStreamInfoFile;
return autocut::loadVideoStreamInfo(f, info);
}

bool ProjProfile::genFileNames()
{
string s0 = m_cfgGame.name;
string_find_n_replace(s0, ' ', '_');

boost::filesystem::path p(m_profileRootFolder);
if ( !boost::filesystem::exists( p ) ){
boost::filesystem::create_directories(p);
}

std::string xPath, xName;
split(m_cfgGame.mp3File, xPath, xName);
string p0 = m_profileRootFolder + "/" + s0 + "_";
m_mp3FrmFile = p0 + xName + ".bin";

DecodedProfileFiles pf;
m_vProfFiles.clear();
BOOST_FOREACH(std::string &f0, m_cfgGame.vInputVideoFileNames) {
string ff = p0 + f0 + ".";
pf.m_audioFrmFile = ff + "audioFrms.bin";
pf.m_audioStreamInfoFile = ff + "audioStreamInfo.bin";
pf.m_videoKeyFrmFile = ff + "vidoKeyFrms.bin";
pf.m_videoStreamInfoFile = ff + "vidoStreamInfo.bin";
pf.m_frmIdxFile = ff + "frmIdxQ.bin";
m_vProfFiles.push_back(pf);
}
return true;
}
