/*
*-------------------------------------------------------------------
* CfgAc.h
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

//todo : extend to thread safe
#ifndef __CFG_AC_H__
#define __CFG_AC_H__

#include "libUtil/DataTypes.h"
#include "libUtil/VisLog.h"
#include "libUtil/UtilFuncs.h"
#include "libUtil/CfgUtil.h"
namespace autocut {
	//--------------------------------------
	class CfgAc {
	public:
		CfgAc();
		~CfgAc();

		CfgAc(const CfgAc &x);
		CfgAc& operator =(const CfgAc &x);

		void readFromFileTeamInfo(const char *fname);
		void writeToFileTeamInfo(const char *fanme);

		void readFromFile(const char *fanme);
		void writeToFile(const char *fanme);
		std::string toStr();

		std::string getGameNameDateScore(std::string &score, int &yyyy, int &mm, int &dd) {
			std::string name, sdate;
			{
				boost::mutex::scoped_lock lock(m_mutexCfg);
				name = m_vGames[0].name;
				score = m_vGames[0].score;
				sdate = m_vGames[0].date;
			}
			dateStrToNum(sdate, yyyy, mm, dd);

			return name;
		}

		AutoCutMethod get_cutMethod() {
			AutoCutMethod ret;
			{
				boost::mutex::scoped_lock lock(m_mutexCfg);
				ret = m_wrtVideo.cutMethod;
			}
			return ret;
		}

		void updateGameName(std::string &name) {
			boost::mutex::scoped_lock lock(m_mutexCfg);
			m_vGames[0].name = name;
		}

		void updateGameDate(std::string &mmddyy)
		{
			m_vGames[0].date = mmddyy;
		}

		void updateGameScore(std::string &score)
		{
			m_vGames[0].score = score;
		}
		void updateMp3File(const std::string &fileName) {
			m_vGames[0].mp3File = fileName;
		}

		void updateInputVideoFolder(std::string &folder) {
			m_vGames[0].inputVideoFolder = folder;
		}

		void updateInputVideoFileNames(const std::vector<std::string> &v) {
			m_vGames[0].vInputVideoFileNames.clear();
			m_vGames[0].vInputVideoFileNames = v;
		}

		bool isWrtHeader() {
			std::string f;
			{
				boost::mutex::scoped_lock lock(m_mutexCfg);
				f = m_wrtVideo.headerVidoeFilePath;
			}

			if (f.length() == 0) { return false; }
			if (!boost::filesystem::exists(f)) { return false; }
			return true;
		}

		bool isWrtTeamInfo() {
			bool f;
			boost::mutex::scoped_lock lock(m_mutexCfg);
			f = m_wrtVideo.isRenderTeamInfo;
			return f;
		}

		//inserting some frms between two highlight segments
		bool isWrtInsertingFrms() {
			std::string f;
			{
				boost::mutex::scoped_lock lock(m_mutexCfg);
				f = m_wrtVideo.insertingVidoeFilePath;
			}
			if (f.length() == 0) { return false; }
			if (!boost::filesystem::exists(f)) { return false; }
			return true;
		}

		void updateOutputVideoFolder(std::string &folder) {
			boost::mutex::scoped_lock lock(m_mutexCfg);
			m_wrtVideo.outputVideoFolder = folder;
		}

		void updateOutputVideoFmt(const int idx) {
			boost::mutex::scoped_lock lock(m_mutexCfg);
			m_wrtVideo.outputVideoFmtIdx = idx;
		}

		void updateOutputVideoFrmSize(const int idx) {
			boost::mutex::scoped_lock lock(m_mutexCfg);
			m_wrtVideo.outputVideoSizeIdx = idx;
		}
		void updateOutputVideoFrmRate(const int idx) {
			boost::mutex::scoped_lock lock(m_mutexCfg);
			m_wrtVideo.outputVideoFpsIdx = idx;
		}
		void updateUploadPlatform(const int idx) {
			boost::mutex::scoped_lock lock(m_mutexCfg);
			m_wrtVideo.uploadPlatformIdx = idx;
		}

		void updateGui_cutMethod(const int f) {
			boost::mutex::scoped_lock lock(m_mutexCfg);
			m_wrtVideo.cutMethod = (AutoCutMethod)f;
		}
		void updateGui_renderTeamInfo(const bool f) {
			boost::mutex::scoped_lock lock(m_mutexCfg);
			m_wrtVideo.isRenderTeamInfo = f;
		}

		std::string getOutputVideoFolder() { return m_wrtVideo.outputVideoFolder; }
		const std::vector<std::string> &getOutputVideoFmts() const { return m_wrtVideo.vOutputVideoFileFmts; }
		const std::vector<std::string> &getOutputFrmSizes() const { return m_wrtVideo.vOutputVideoFrmSizes; }
		const std::vector<std::string> &getOutputFrmRates() const { return m_wrtVideo.vOutputVideoFrmRates; }
		const std::vector<std::string> &getUploadPlatforms() const { return m_wrtVideo.vUploadPlatforms; }

		std::string getOutputVideoFmt() {
			int idx;
			{
				boost::mutex::scoped_lock lock(m_mutexCfg);
				idx = m_wrtVideo.outputVideoFmtIdx;
			}
			return m_wrtVideo.vOutputVideoFileFmts[idx];
		}

		cv::Size getOutputVideoSize(const cv::Size &inputSize) {
			int idx;
			{
				boost::mutex::scoped_lock lock(m_mutexCfg);
				idx = m_wrtVideo.outputVideoSizeIdx;
			}
			if (idx == 0) {
				return inputSize;
			}
			else {
				return m_wrtVideo.vOutputVideoFrmSizes_value[idx];
			}
		}

		float getOutputVideoFrameRate(const float inputFrmRate) {
			int idx;
			{
				boost::mutex::scoped_lock lock(m_mutexCfg);
				idx = m_wrtVideo.outputVideoFpsIdx;
			}
			if (idx == 0) {
				return inputFrmRate;
			}
			else {
				return m_wrtVideo.vOutputVideoFrmRates_value[idx];
			}
		}

		int getUploadPlatFormIdx() {
			int idx;
			{
				boost::mutex::scoped_lock lock(m_mutexCfg);
				idx = m_wrtVideo.uploadPlatformIdx;
			}
			return idx;
		}

		/*
		from AutoCutGui.cpp:
		m_param_sliders_names[0] = "Original Audio Intensity";
		m_param_sliders_names[1] = "Min Applausing Time(sec)";
		m_param_sliders_names[2] = "Min Hightlight Length(sec)";
		m_param_sliders_names[3] = "Num. of Hightlights in Each Video";
		m_param_sliders_names[4] = "Final Hightlight Length(sec)";
		*/

		void updateParaFromSliders(int sliderId, int val) {
			if (sliderId == 0) {
				float t = (100 - val) / 100.0;
				if (t < 0) t = 0;
				if (t > 1) t = 1;
				m_wrtVideo.originalAudioIntensity = t;
			}
			else if (sliderId == 1) {
				m_wrtVideo.minApplausingTimeSec = val;
			}
			else if (sliderId == 2) {
				m_wrtVideo.minSegmentTimeSec = val;
			}
			else if (sliderId == 3) {
				m_tfAna.nMaxHotSeg4EachVideoToCut = val;
			}
			else if (sliderId == 4) {
				m_wrtVideo.highlightSegmentLengthInSec = val;
			}
		}

		void addGameVideo(const std::string &fileName) {
		}


		void getDispImgSize(int &w, int &h) {
			w = m_gui.dispImgWidth;
			h = m_gui.dispImgHeight;
		}
		void getDispImgSizeInit(int &w, int &h) {
			w = m_gui.dispImgWidthInit;
			h = m_gui.dispImgHeightInit;
		}

		int getDispImgW() {
			return m_gui.dispImgWidth;
		}

		std::string getMp3FilePath() {
			const CfgGame &g = m_vGames[0];
			return g.mp3File;
		}

		CfgGame getGame(int idx) {
			return m_vGames[idx];
		}

		std::string getInputFolder() { return m_vGames[0].inputVideoFolder; }

		std::vector<std::string> getInputVideoFileNames() {
			return m_vGames[0].vInputVideoFileNames;
		}

		std::vector<std::string> getInputVideoFilePaths() {
			const CfgGame &g = m_vGames[0];
			std::vector<std::string> v;
			BOOST_FOREACH(const std::string &f, g.vInputVideoFileNames) {
				v.push_back(g.inputVideoFolder + "/" + f);
			}
			return v;
		}



		std::string getMp3File(const int gameId) {
			std::string f;
			{
				boost::mutex::scoped_lock lock(m_mutexCfg);
				f = m_vGames[gameId].mp3File;
			}
			return f;
		}

		std::string getProjProfilerFolder() {
			std::string p;
			{
				boost::mutex::scoped_lock lock(m_mutexCfg);
				p = m_log.projProfileFolder;
			}
			return p;
		}

	private:
		virtual boost::property_tree::ptree toPropertyTree();
		virtual void fromPropertyTree(const boost::property_tree::ptree &pt);

		virtual boost::property_tree::ptree toPropertyTreeTeamInfo();
		virtual void fromPropertyTreeTeamInfo(const boost::property_tree::ptree &pt);

	public:
		CfgDemuxingDecoding m_demux;

		std::vector<CfgGame> m_vGames;

		CfgTeam				m_team;
		CfgTimeFreqAna		m_tfAna;
		CfgWrtVideo			m_wrtVideo;
		CfgLog				m_log;
		CfgRenderImg        m_imgRender;
		CfgDc				m_dc0;            //m_dc is already used, for less confusion in search
		CfgGui              m_gui;
	private:
		boost::mutex	m_mutexCfg;
	};
	typedef std::shared_ptr<CfgAc>		CfgAcPtr;
}
#endif
