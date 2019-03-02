/*
*-------------------------------------------------------------------
* test_AutoCut.cpp
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
//#include "libAutoCut/AutoCut.h"
#include "libRun/RunAutoCutGui.h"

using namespace std;
using namespace autocut;
using namespace libyuv;

std::string locateCfgFile(const std::string &f0);

int test_Autocut(int argc, char **argv)
{
//--------------------------------------------------------------------
//read cfg file
//--------------------------------------------------------------------
std::string cfgTeamInfoFile = locateCfgFile("cfgTeamInfo.xml");
std::string cfgAutoCutFile  = locateCfgFile("cfgVideoEditor.xml");
if ( cfgAutoCutFile.empty() ) {
return -1;
}

CfgAcPtr cfg(new CfgAc());
bool hasTeamInfo = false;
if (!cfgTeamInfoFile.empty()) {
cfg->readFromFileTeamInfo(cfgTeamInfoFile.c_str());
hasTeamInfo = true;
}

cfg->readFromFile(cfgAutoCutFile.c_str());
cfg->m_wrtVideo.isRenderTeamInfo = hasTeamInfo;


//--------------------------------------------------------------------
//create log file
//--------------------------------------------------------------------
myCreateDir( cfg->m_log.logFolder );
string ymd0 = getPrettyTimeStamp();
string logFile = cfg->m_log.logFolder + "/log_" + ymd0 + ".txt";
VisLog::SetLoggerFilename(logFile);
VisLog::SetLoggerShowInConsoleFlag(cfg->m_log.ishowLogInConsole);
VisLog::SetLoggerIsDumpFlag( true );

//dump current cfg into log file
xLog("----------cfg----------------");
string cfgStr = cfg->toStr();
std::vector<std::string> v;
string_to_vector2(v, cfgStr, "\n", false);
BOOST_FOREACH(const std::string &s, v) {
xLog(s);
}
VisLog::SetLoggerIsDumpFlag(cfg->m_log.isDumpLog);

string f = cfg->m_log.logFolder + "/cfgAutoCut_latest.xml";
cfg->writeToFile( f.c_str() );
f = cfg->m_log.logFolder + "/cfgTeamInfo_latest.xml";
cfg->writeToFileTeamInfo(f.c_str() );


//--------------------------------------------------------------------
//run AutoCut
//--------------------------------------------------------------------
#if 0
AutoCutPtr x( new AutoCut() );
x->setCfg(cfg);
x->process();
return 0;
#else
QApplication app(argc, argv);
//qRegisterMetaType<std::string>();

QFont font;
font.setFamily(font.defaultFamily());
font.setPointSize(cfg->m_gui.fontSize);
app.setFont(font);

RunAutoCutGui runGui(cfg);
runGui.show();
return app.exec();

#endif
}

std::string locateCfgFile(const std::string &f0)
{
//install folder
string cfgFile1 = "../cfg/" + f0;

//developing folder
string cfgFile2 = "C:/Projects/AutoCut/src/AutoCut/res/" + f0;

string cfgFile;
if (boost::filesystem::exists(cfgFile1)) {
cfgFile = cfgFile1;
}
else if (boost::filesystem::exists(cfgFile2)) {
cfgFile = cfgFile2;
}
else {
printf("Error: Both config files <%s> and <%s> are not available!", cfgFile1.c_str(), cfgFile2.c_str());
return "";
}
return cfgFile;
}
