/*
*-------------------------------------------------------------------
* AutoCutEnums.h
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

#ifndef __AUTO_CUT_ENUMS_H__
#define __AUTO_CUT_ENUMS_H__

#include <stdio.h>
namespace autocut {
enum AutoCutMethod {
AUTO_CUT_MANMUAL = 0,
AUTO_CUT_SEMI_AUTO,
AUTO_CUT_FULL_AUTO,
AUTO_CUT_UNKN
};

enum DetectMethod {
DETECT_APPLAUSING = 0,
DETECT_OTHER,
};

enum ThreadId_t {
Thread_ONE = 0,
Thread_TWO,
Thread_THREE,
Thread_FOUR,
Thread_FIVE,
};
}
#endif
