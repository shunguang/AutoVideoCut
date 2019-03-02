/*
*-------------------------------------------------------------------
* VisLog.cpp
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
using namespace std;
using namespace autocut;

VisLog* VisLog::_logger = NULL;
std::string VisLog::_logFilename = string("");
bool   VisLog::_logShowInConsole = false;
bool   VisLog::_logIsDump = true;

void VisLog::SetLoggerFilename(const string &logFilename)
{
//for debug purpose, alown to change log file name during the process
if ( VisLog::_logger == NULL ){
VisLog::_logFilename = logFilename;
}
else{
VisLog::_logger->_logMutex.lock();

VisLog::_logFilename = logFilename;

VisLog::_logger->_logMutex.unlock();
}
}


void VisLog :: SetLoggerShowInConsoleFlag( const bool flag )
{
VisLog::_logShowInConsole = flag;
}

void VisLog::SetLoggerIsDumpFlag(const bool flag)
{
VisLog::_logIsDump = flag;
}

VisLog* VisLog::getInstance() {
if (VisLog::_logger == NULL){
VisLog::_logger = new VisLog();
}
return VisLog::_logger;
}

void VisLog::log( const string &msg) {

std::string t = getTime();
_logMutex.lock();

if (VisLog::_logIsDump){
if (VisLog::_logFilename.length() > 0) {
ofstream outfile(VisLog::_logFilename.c_str(), ios_base::app);
if (outfile.is_open()){
outfile << t << " -> " << msg << endl;
outfile.flush();
outfile.close();
}
}
}

if (VisLog :: _logShowInConsole ){
cout << endl << t << " -> " << msg << endl;
}

_logMutex.unlock();
}

void VisLog::log(const char* msg) {
string s(msg);
log(s);
}

VisLog::VisLog()
: _logMutex()
{
_t0 = std::time(0);
if( VisLog::_logFilename.length() > 0 ) {
time_t t = time(0);   // get time now
struct tm *now = localtime( &t );

char buf[64];
sprintf(buf, "%4d-%02d%02d-%02d%02d%02d", (now->tm_year + 1900),  (now->tm_mon + 1 ), now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec );
ofstream outfile(VisLog::_logFilename.c_str(), ios_base::trunc);
if ( outfile.is_open() ){
outfile << string(buf) << endl;
outfile.flush();
outfile.close();
}
}
}

VisLog::~VisLog() {;}

std::string VisLog :: getTime()
{
char buf[64];
struct tm now;
std::time_t t = std::time(0);   // get time now
uint32 dt = t - _t0;

#if _WINDOWS
#if LOG_USING_LOCAL_TIME
localtime_s(&now, &t);
#else
gmtime_s(&now, &t);
#endif
#else
#if LOG_USING_LOCAL_TIME
localtime_r(&t, &now);
#else
gmtime_r(&t, &now);
#endif
#endif

snprintf(buf, 64, "%02d/%02d-%02d:%02d:%02d (%ld)", (now.tm_mon + 1 ), now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec, dt );
return string(buf);
}

void autocut::xLog( const char *fmt, ... )
{
//todo: remove this LOG_MAX_MSG_LEN, using dynamic allocation idea
char buffer[LOG_MAX_MSG_LEN];
va_list args;
va_start (args, fmt);
vsnprintf (buffer,LOG_MAX_MSG_LEN,fmt, args);
va_end (args);

VisLog::getInstance()->log( buffer );
}

void autocut::xLog( const std::string &x, ... )
{
autocut::xLog( x.c_str() );
}

void autocut::xLogExit(const char* x, ...)
{
//1 log msg
xLog(x);


//2. todo: close log file for new threaded log method

//3. exit()
exit(1);
}


void autocut::xLogExit(const std::string &x, ...)
{
autocut::xLogExit(x.c_str());
}
