/*
*-------------------------------------------------------------------
* VisLog.h
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
#ifndef __VISLOG_H__
#define __VISLOG_H__


#include "DataTypes.h"

#include <stdarg.h>
#include <fstream>
#include <time.h>
#include "UtilDefs.h"
#define LOG_USING_LOCAL_TIME  1
#define	LOG_MAX_MSG_LEN		2048

#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

namespace autocut{
class  UTIL_EXPORT VisLog {

public:
static void SetLoggerShowInConsoleFlag(const bool flag);
static void SetLoggerIsDumpFlag(const bool flag);
static void SetLoggerFilename(const std::string &logFilename);
static VisLog* getInstance();
void log(const std::string &msg);
void log(const char* msg);
std::string getLogFilename() { return  _logFilename; }

public:
boost::mutex		_logMutex;

private:
static VisLog      *_logger;
static std::string _logFilename;
static bool        _logShowInConsole;
static bool        _logIsDump;

std::string getTime();

std::time_t 		_t0;
VisLog();
~VisLog();
};

void  UTIL_EXPORT xLog(const char* x, ...);
void  UTIL_EXPORT xLog(const  std::string &x, ...);

void  UTIL_EXPORT xLogExit(const  std::string &x, ...);
void  UTIL_EXPORT xLogExit(const char* x, ...);
}
#endif

