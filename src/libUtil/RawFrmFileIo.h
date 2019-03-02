/*
*-------------------------------------------------------------------
* RawFrmFileIo.h
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
#ifndef __RAW_FRM_FILE_IO_H__
#define __RAW_FRM_FILE_IO_H__

#include "RawFrm.h"
#include "UtilDefs.h"
namespace autocut {
struct RawFrmFileHd {
uint64  nTotFrms;
};
class UTIL_EXPORT RawFrmFileIo {
public:
RawFrmFileIo();
bool saveRawFrms(const std::string &filePath, const std::vector<RawFrmPtr> &v);
bool loadRawFrms(const std::string &filePath, std::vector<RawFrmPtr> &v);

bool saveFrmIdxQ(const std::string &filePath, const std::vector<FrmIdx> &v);
bool loadFrmIdxQ(const std::string &filePath, std::vector<FrmIdx> &v);
};

typedef std::shared_ptr<RawFrmFileIo>		RawFrmFileIoPtr;
}

#endif

