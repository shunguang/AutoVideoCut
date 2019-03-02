/*
*-------------------------------------------------------------------
* ProjProfile.h
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
#ifndef __PROJ_PROFILE_H__
#define __PROJ_PROFILE_H__
#include "libUtil/UtilFuncs.h"
#include "libUtil/RawFrmFileIo.h"
#include "libUtil/CfgAc.h"
#include "avUtils.h"
namespace autocut {
class ProjProfile {
public:
ProjProfile();
~ProjProfile();

void respReset();
void setCfg(const CfgGame &cfg);
void setRootFolder(const std::string &rootFolder);

bool saveMp3Frms(const std::vector<RawFrmPtr> &v);
bool loadMp3Frms(std::vector<RawFrmPtr> &v);

bool saveAudioFrms(const int fileId, const std::vector<RawFrmPtr> &v);
bool loadAudioFrms(const int fileId, std::vector<RawFrmPtr> &v);

bool saveVideoKeyFrms(const int fileId, const std::vector<RawFrmPtr> &v);
bool loadVideoKeyFrms(const int fileId, std::vector<RawFrmPtr> &v);

bool saveFrmIdxQ(const int fileId, const std::vector<FrmIdx> &v);
bool loadFrmIdxQ(const int fileId, std::vector<FrmIdx> &v);

bool saveAudioStreamInfo(const int fielId, const MyAudioStreamInfo &info);
bool loadAudioStreamInfo(const int fielId, MyAudioStreamInfo &info);

bool saveVideoStreamInfo(const int fielId, const MyVideoStreamInfo &info);
bool loadVideoStreamInfo(const int fielId, MyVideoStreamInfo &info);

bool genFileNames();

public:
CfgGame						m_cfgGame;

RawFrmFileIoPtr				m_rawFrmFileIo;

std::string					m_profileRootFolder;
std::string					m_mp3FrmFile;
std::vector<DecodedProfileFiles> m_vProfFiles;
};
typedef std::shared_ptr<ProjProfile> ProjProfilePtr;
}
#endif

