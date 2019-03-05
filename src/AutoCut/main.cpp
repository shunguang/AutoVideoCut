/*
*-------------------------------------------------------------------
* main.cpp
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

//todo:
//12/16: modify MuxingEncoding2::wrtSomeFrms() to adapte video frm size change. 
//1/13: progressBar_subprocess[3]->setRange(0, 0);  //busy bar
//

#include "libUtil/DataTypes.h"
#include "libUtil/VisLog.h"
#include "libUtil/UtilFuncs.h"
#include <QtCore/QCoreApplication>
int test_demuxing_decoding(int argc, char **argv);
int test_muxing_encoding(int argc, char **argv);
int test_libyuv(int argc, char **argv);
int test_Autocut(int argc, char **argv);
int test_timeFreqAna(int argc, char **argv);
int test_audioDeEn_coding(int argc, char **argv);
int test_audio_fmt_convert(int argc, char **argv);
int test_cv_peopleDetect(int argc, char **argv);
//int test_draw_rect(int argc, char *argv[]);
int main(int argc, char* argv[])
{
	QCoreApplication::addLibraryPath("./");

	int x;
#if 0
	std::string ymd0 = autocut::getPrettyTimeStamp();
	std::string logFile = "c:/temp/log_" + ymd0 + ".txt";
	autocut::VisLog::SetLoggerFilename(logFile);
	autocut::VisLog::SetLoggerShowInConsoleFlag(true);
	autocut::VisLog::SetLoggerIsDumpFlag(true);

	x = test_demuxing_decoding(argc, argv);

	//x = test_muxing_encoding(argc, argv);
	//x = test_libyuv(argc, argv);
	//x = test_timeFreqAna(argc, argv);
	//x=test_audioDeEn_coding( argc, argv );
	//x = test_cv_peopledetect(argc, argv);
	//x = test_audio_fmt_convert( argc, argv);

#else	
	x = test_Autocut(argc, argv);
	//x = test_draw_rect(argc, argv);
#endif

	return x;
}


