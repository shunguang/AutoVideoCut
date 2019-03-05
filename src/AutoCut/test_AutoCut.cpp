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
#include "libUtil/util.h"
#include "libyuv/libyuv.h"
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
	const std::string cfgTeamInfoFile = locateCfgFile("cfgTeamInfo.xml");
	const std::string cfgAutoCutFile = locateCfgFile("cfgVideoEditor.xml");
	if (cfgAutoCutFile.empty()) {
		return -1;
	}

	cout << "cfgTeamInfoFile=" << cfgTeamInfoFile << endl;
	cout << "cfgAutoCutFile=" << cfgAutoCutFile << endl;

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
	myCreateDir(cfg->m_log.logFolder);
	string ymd0 = getPrettyTimeStamp();
	string logFile = cfg->m_log.logFolder + "/log_" + ymd0 + ".txt";
	VisLog::SetLoggerFilename(logFile);
	VisLog::SetLoggerShowInConsoleFlag(cfg->m_log.isShowLogInConsole);
	VisLog::SetLoggerIsDumpFlag(true);

	//dump current cfg into log file
	xLog("----------cfg----------------");
	string cfgStr = cfg->toStr();
	std::vector<std::string> v;
	string_to_vector2(v, cfgStr, "\n", false);
	BOOST_FOREACH(const std::string &s, v) {
		xLog(s);
	}
	VisLog::SetLoggerIsDumpFlag(cfg->m_log.isDumpLog);

	//--------------------------------------------------------------------
	//run AutoCut
	//--------------------------------------------------------------------
#if 0
	AutoCutPtr x(new AutoCut());
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
	int ret = app.exec();
#if 1
	//--backup the old config file and save the current settings 
	std::string head, ext;
	splitExt(cfgAutoCutFile, head, ext);
	string oldToSave = head + ymd0 + "." + ext;
	xLog( "copy current cfg file: " + oldToSave );
	boost::filesystem::copy_file(boost::filesystem::path(cfgAutoCutFile), boost::filesystem::path(oldToSave));
	xLog("update current cfg file: " + cfgAutoCutFile);
	cfg->writeToFile( cfgAutoCutFile.c_str() );
#endif
	return ret;
#endif
}

std::string locateCfgFile(const std::string &f0)
{
	//install folder
	string cfgFile1 = "../cfg/" + f0;

	//developing folder
	string cfgFile2 = "C:/Projects/AutoVideoCut/src/AutoCut/res/" + f0;

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
