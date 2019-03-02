/*
*-------------------------------------------------------------------
* CfgAc.cpp
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
#include "CfgAc.h"

using namespace std;
using namespace autocut;

CfgAc::CfgAc()
: m_demux()
, m_team()
, m_tfAna()
, m_wrtVideo()
, m_log()
, m_imgRender()
{
}

CfgAc::~CfgAc()
{
}

CfgAc& CfgAc::operator = (const CfgAc &x )
{
if (this != &x) {
m_demux = x.m_demux;
m_vGames	= x.m_vGames;
m_team = x.m_team;
m_tfAna = x.m_tfAna;
m_wrtVideo = x.m_wrtVideo;
m_log = x.m_log;
m_imgRender = x.m_imgRender;
}
return *this;
}

void CfgAc::readFromFileTeamInfo(const char *fname)
{
boost::property_tree::ptree pt;
boost::property_tree::xml_parser::read_xml(fname, pt, boost::property_tree::xml_parser::no_comments);
this->fromPropertyTreeTeamInfo(pt.get_child("cfg"));
}
void CfgAc::writeToFileTeamInfo(const char *fname)
{
boost::property_tree::ptree pt = toPropertyTree();
boost::property_tree::xml_parser::xml_writer_settings<std::string> settings(' ', 4);
boost::property_tree::xml_parser::write_xml(fname, pt, std::locale(), settings);
}


void CfgAc::readFromFile(const char *fname)
{
boost::property_tree::ptree pt;
boost::property_tree::xml_parser::read_xml(fname, pt, boost::property_tree::xml_parser::no_comments);
this->fromPropertyTree(pt.get_child("cfg"));
}

void CfgAc::writeToFile(const char *fname)
{
boost::property_tree::ptree pt = toPropertyTree();
boost::property_tree::xml_parser::xml_writer_settings<std::string> settings(' ', 4);
boost::property_tree::xml_parser::write_xml(fname, pt, std::locale(), settings);
}

std::string CfgAc::toStr()
{
boost::property_tree::ptree pt1 = toPropertyTree();
boost::property_tree::ptree pt2 = toPropertyTreeTeamInfo();
std::ostringstream oss1, oss2;
boost::property_tree::xml_parser::xml_writer_settings<std::string> settings(' ', 4);
boost::property_tree::xml_parser::write_xml(oss1, pt1, settings);
boost::property_tree::xml_parser::write_xml(oss2, pt2, settings);

return oss1.str() + "\n" + oss2.str();
}


void CfgAc::fromPropertyTree(const boost::property_tree::ptree &pt0 )
{
boost::property_tree::ptree pt;
boost::property_tree::ptree pt1;

//---------------------------------
pt = pt0.get_child("games");
m_vGames.clear();
cout << pt.size() << endl;
BOOST_FOREACH(const boost::property_tree::ptree::value_type &g, pt) {
CfgGame game;
game.name = g.second.get<std::string>("name");
game.date = g.second.get<std::string>("date");
game.score = g.second.get<std::string>("score");
game.skip = (bool)g.second.get<int>("skip");
game.inputVideoFileFilter = g.second.get<std::string>("inputVideoFileFilter");
game.inputVideoFolder = g.second.get<std::string>("inputVideoFolder");

pt1 = g.second.get_child("videoFiles");
//cout << game.name << ", " << pt1.size() << endl;
game.vInputVideoFileNames.clear();
BOOST_FOREACH(const boost::property_tree::ptree::value_type &vk, pt1) {
const string fpath = vk.second.data();
//if (!boost::filesystem::exists(fpath)) {
//	xLog( "CfgAc::fromPropertyTree(): file %s does not exist!", fpath.c_str() );
//}
game.vInputVideoFileNames.push_back( fpath );
}
game.mp3File = g.second.get<std::string>("mp3File");
m_vGames.push_back(game);
}

//---------------------------------
pt = pt0.get_child("timeFreqAna");
m_tfAna.nFFT	= pt.get<int>("nFFT");
m_tfAna.stepSz	= pt.get<int>("stepSz");
m_tfAna.winSz	= pt.get<int>("winSz");
m_tfAna.nSec4EachRawAppluasing = pt.get<int>("nSec4EachRawAppluasing");
m_tfAna.nMaxHotSeg4EachVideoToCut = pt.get<int>("nMaxHotSeg4EachVideoToCut");

pt = pt0.get_child("output");
m_wrtVideo.headerVidoeFilePath = pt.get<std::string>("headerVidoeFilePath");
m_wrtVideo.highlightSegmentLengthInSec = pt.get<float>("highlightSegmentLengthInSec");
m_wrtVideo.minApplausingTimeSec = pt.get<float>("minApplausingTimeSec");
m_wrtVideo.minSegmentTimeSec = pt.get<float>("minSegmentTimeSec");
m_wrtVideo.insertingVidoeFilePath = pt.get<std::string>("insertingVidoeFilePath");
m_wrtVideo.originalAudioIntensity = pt.get<float>("originalAudioIntensity");
m_wrtVideo.outputVideoFolder = pt.get<std::string>("outputVideoFolder");
m_wrtVideo.isRenderTeamInfo = (bool)pt.get<uint8>("isRenderTeamInfo");
//todo:
m_wrtVideo.showDebugMsg = false;
m_wrtVideo.cutMethod = AUTO_CUT_SEMI_AUTO;
m_wrtVideo.vOutputVideoFileFmts.clear();
m_wrtVideo.vOutputVideoFileFmts.push_back("mp4");
m_wrtVideo.vOutputVideoFileFmts.push_back("avi");
m_wrtVideo.outputVideoFmtIdx = 0;


m_wrtVideo.vOutputVideoFrmSizes.clear();
m_wrtVideo.vOutputVideoFrmSizes.push_back("Original");
m_wrtVideo.vOutputVideoFrmSizes.push_back("1280x720");
m_wrtVideo.vOutputVideoFrmSizes.push_back("640x480");
m_wrtVideo.vOutputVideoFrmSizes_value.clear();
m_wrtVideo.vOutputVideoFrmSizes_value.push_back(cv::Size(0, 0));
m_wrtVideo.vOutputVideoFrmSizes_value.push_back(cv::Size(1280, 720));
m_wrtVideo.vOutputVideoFrmSizes_value.push_back(cv::Size(640, 480));
m_wrtVideo.outputVideoSizeIdx = 0;

m_wrtVideo.vOutputVideoFrmRates.clear();
m_wrtVideo.vOutputVideoFrmRates.push_back("Original");
m_wrtVideo.vOutputVideoFrmRates.push_back("30");
m_wrtVideo.vOutputVideoFrmRates.push_back("60");
m_wrtVideo.outputVideoFpsIdx = 0;

m_wrtVideo.vOutputVideoFrmRates_value.clear();
m_wrtVideo.vOutputVideoFrmRates_value.push_back(0);   //
m_wrtVideo.vOutputVideoFrmRates_value.push_back(30);
m_wrtVideo.vOutputVideoFrmRates_value.push_back(60);

m_wrtVideo.uploadPlatformIdx = 0;
m_wrtVideo.vUploadPlatforms.push_back("YouTube");
m_wrtVideo.vUploadPlatforms.push_back("Facebook");
m_wrtVideo.vUploadPlatforms.push_back("Twitter");
m_wrtVideo.vUploadPlatforms.push_back("Instagram");
m_wrtVideo.vUploadPlatforms.push_back("Youku");
//gui
pt = pt0.get_child("gui");
m_gui.fontSize = pt.get<int>("fontSize");;
m_gui.dispImgWidth = pt.get<int>("dispImgWidth");
m_gui.dispImgHeight = pt.get<int>("dispImgHeight");
m_gui.dispImgWidthInit = pt.get<int>("dispImgWidthInit");
m_gui.dispImgHeightInit = pt.get<int>("dispImgHeightInit");
m_gui.thumbnailWidth = pt.get<int>("thumbnailWidth");
m_gui.thumbnailHeight = pt.get<int>("thumbnailHeight");



//----
pt = pt0.get_child("dataContainer");
m_dc0.qSize = pt.get<int>("qSize");

//----
pt = pt0.get_child("log");
m_log.isDumpLog = (bool)pt.get<uint8>("isDumpLog");
m_log.ishowLogInConsole = (bool)pt.get<uint8>("isShowLogInConsole");
m_log.isDumpSelectedRawAudio = (bool)pt.get<uint8>("isDumpSelectedRawAudio");
m_log.logFolder = pt.get<std::string>("logFolder");
m_log.projProfileFolder = pt.get<std::string>("projProfileFolder");
}

void CfgAc::fromPropertyTreeTeamInfo(const boost::property_tree::ptree &pt0)
{
boost::property_tree::ptree pt;
boost::property_tree::ptree pt1;
//---------------------------------
pt = pt0.get_child("team");
m_team.name = pt.get<std::string>("name");
m_team.folder = pt.get<std::string>("folder");
m_team.picFile = pt.get<std::string>("picFile");

m_team.vCoach.clear();
pt1 = pt.get_child("coachs");
BOOST_FOREACH(const boost::property_tree::ptree::value_type &k, pt1) {
m_team.vCoach.push_back(k.second.data());
}
m_team.vManager.clear();
pt1 = pt.get_child("managers");
BOOST_FOREACH(const boost::property_tree::ptree::value_type &k, pt1) {
m_team.vManager.push_back(k.second.data());
}
m_team.vTreasurer.clear();
pt1 = pt.get_child("treasurers");
BOOST_FOREACH(const boost::property_tree::ptree::value_type &k, pt1) {
m_team.vTreasurer.push_back(k.second.data());
}

m_team.vPlayer.clear();
pt1 = pt.get_child("roster");
cout << pt1.size() << endl;
BOOST_FOREACH(const boost::property_tree::ptree::value_type &kv, pt1) {
Player x;
x.num = kv.second.get<int>("num");
x.firstName = kv.second.get<std::string>("firstName");
x.lastName = kv.second.get<std::string>("lastName");
x.picFile = kv.second.get<std::string>("picFile");
//cout << x.num << endl;
m_team.vPlayer.push_back(x);
}
}

boost::property_tree::ptree CfgAc::toPropertyTree()
{
boost::property_tree::ptree pt;

//todo: 
BOOST_FOREACH(const CfgGame &g,  m_vGames) {
boost::property_tree::ptree pt0;
pt0.put("name", g.name);
pt0.put("date", g.date);
pt0.put("score", g.score);
pt0.put("skip", (int)g.skip);
pt0.put("inputVideoFileFilter", g.inputVideoFileFilter);
pt0.put("inputVideoFolder", g.inputVideoFolder);
BOOST_FOREACH(const string &fp, g.vInputVideoFileNames) {
pt0.add("videoFiles.video", fp);
}
pt0.put("mp3File", g.mp3File);
pt.add_child("cfg.games.game", pt0);
}

//gui
pt.put("cfg.gui.fontSize", m_gui.fontSize);
pt.put("cfg.gui.thumbnailWidth", m_gui.thumbnailWidth);
pt.put("cfg.gui.thumbnailHeight", m_gui.thumbnailHeight);
pt.put("cfg.gui.dispImgWidth", m_gui.dispImgWidth);
pt.put("cfg.gui.dispImgHeight", m_gui.dispImgHeight);
pt.put("cfg.gui.dispImgWidthInit", m_gui.dispImgWidthInit);
pt.put("cfg.gui.dispImgHeightInit", m_gui.dispImgHeightInit);

//timeFreqAna
pt.put("cfg.timeFreqAna.nFFT", m_tfAna.nFFT);
pt.put("cfg.timeFreqAna.stepSz", m_tfAna.stepSz);
pt.put("cfg.timeFreqAna.winSz", m_tfAna.winSz);
pt.put("cfg.timeFreqAna.nSec4EachRawAppluasing", m_tfAna.nSec4EachRawAppluasing);
pt.put("cfg.timeFreqAna.nMaxHotSeg4EachVideoToCut", m_tfAna.nMaxHotSeg4EachVideoToCut);

//output
pt.put("cfg.output.headerVidoeFilePath", m_wrtVideo.headerVidoeFilePath);
pt.put("cfg.output.insertingVidoeFilePath", m_wrtVideo.insertingVidoeFilePath);
pt.put("cfg.output.originalAudioIntensity", m_wrtVideo.originalAudioIntensity);

pt.put("cfg.output.outputVideoFolder", m_wrtVideo.outputVideoFolder);
pt.put("cfg.output.highlightSegmentLengthInSec", m_wrtVideo.highlightSegmentLengthInSec);
pt.put("cfg.output.minApplausingTimeSec", m_wrtVideo.minApplausingTimeSec);
pt.put("cfg.output.minSegmentTimeSec", m_wrtVideo.minSegmentTimeSec);
pt.put("cfg.output.isRenderTeamInfo", (uint8)m_wrtVideo.isRenderTeamInfo);

//log
pt.put("cfg.log.isDumpLog", (uint8)m_log.isDumpLog);
pt.put("cfg.log.ishowLogInConsole", (uint8)m_log.ishowLogInConsole);
pt.put("cfg.log.isDumpSelectedRawAudio", (uint8)m_log.isDumpSelectedRawAudio);
pt.put("cfg.log.logFolder", m_log.logFolder);
pt.put("cfg.log.projProfileFolder", m_log.projProfileFolder);

return pt;
}

boost::property_tree::ptree CfgAc::toPropertyTreeTeamInfo()
{
boost::property_tree::ptree pt;

//team
pt.put("cfg.team.name", m_team.name);
pt.put("cfg.team.folder", m_team.folder);
pt.put("cfg.team.picFile", m_team.picFile);
BOOST_FOREACH(const string &s, m_team.vCoach) {
pt.add("cfg.team.coaches.coach", s);
}
BOOST_FOREACH(const string &s, m_team.vManager) {
pt.add("cfg.team.managers.manager", s);
}
BOOST_FOREACH(const string &s, m_team.vTreasurer) {
pt.add("cfg.team.treasurers.treasurer", s);
}
BOOST_FOREACH(const Player &p, m_team.vPlayer) {
boost::property_tree::ptree pt0;
pt0.put("num", p.num);
pt0.put("firstName", p.firstName);
pt0.put("lastName", p.lastName);
pt0.put("picFile", p.picFile);

pt.add_child("cfg.roster.player", pt0);
}
return pt;
}
