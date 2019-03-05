/*
*-------------------------------------------------------------------
* CfgUtil.h
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
#ifndef __CFG_UTIL_H__
#define __CFG_UTIL_H__

#include "libUtil/DataTypes.h"
#include "libUtil/AutoCutEnums.h"
#include "libUtil/VisLog.h"

namespace autocut {
	struct CfgGame {
		CfgGame() : name("My Game"), date("01/01/2018") {}
		std::string  name;
		std::string  date;
		std::string  score;
		bool		 skip;
		std::string  inputVideoFolder;
		std::string  inputVideoFileFilter;
		std::vector<std::string>  vInputVideoFileNames;
		std::string  mp3File;
	};

	struct Player {
		Player() : num(0), firstName("FirstName"), lastName("LastName"), picFile("FirstName.pic") {}
		int			num;
		std::string firstName;
		std::string lastName;
		std::string picFile;
	};

	struct CfgTeam {
		CfgTeam() : name("SAC PreAcademy 2014"), folder("c:/temp") {}
		std::string					name;
		std::string					folder;
		std::string					picFile;
		std::vector<std::string>	vCoach;
		std::vector<std::string>	vManager;
		std::vector<std::string>	vTreasurer;
		std::vector<Player>			vPlayer;
	};

	struct CfgDemuxingDecoding {
		CfgDemuxingDecoding()
			:demuxingVideo(false), dumpRawVideo(false), demuxingAudio(true), dumpRawAudio(true),
			storeRawAudioInMemo(false), storeRawVideoInMemo(false), videoFmt(0),
			storeThumbnailKeyFrms(false), thumbnailWidth(64), thumbnailHeight(64), outputImgWidth(1280), outputImgHeight(720)
		{}
		bool demuxingVideo;
		bool dumpRawVideo;
		bool demuxingAudio;
		bool dumpRawAudio;
		bool storeRawAudioInMemo;
		bool storeRawVideoInMemo;

		bool storeThumbnailKeyFrms;
		int  thumbnailWidth;
		int  thumbnailHeight;
		int  outputImgWidth;    //-1, if keep the original image width
		int  outputImgHeight;	//-1, if keep the original image width	
		//for future using
		uint8 videoFmt;
	};
	typedef std::shared_ptr<CfgDemuxingDecoding>	CfgDemuxingDecodingPtr;


	struct CfgTimeFreqAna {
		CfgTimeFreqAna() :winSz(1024), stepSz(256), nFFT(2048), nSec4EachRawAppluasing(15), nMaxHotSeg4EachVideoToCut(5) {}

		int winSz;
		int stepSz;
		int nFFT;
		int nSec4EachRawAppluasing;     //given a video with t sec length, we will pickup the top  ceil(t/nSec4EachRawAppluasing) raw applausings inside it. 
		int nMaxHotSeg4EachVideoToCut;
	};
	typedef std::shared_ptr<CfgTimeFreqAna>		CfgTimeFreqAnaPtr;


	struct CfgRenderImg {
		CfgRenderImg()
			: vFontFace{ 0/*cv::FONT_HERSHEY_SIMPLEX*/, 1 /*cv::FONT_HERSHEY_PLAIN*/ }
			, vFontScale{ 2, 1.2 }
			, vThickness{ 2, 2 }
			, imgBgColor(cv::Scalar(255, 0, 0))   //BGR
			, fontColor(cv::Scalar(255, 255, 255))
		{}

		CfgRenderImg(const CfgRenderImg &x)
			: imgBgColor(x.imgBgColor)
			, fontColor(x.fontColor)
		{
			for (int i = 0; i < 2; ++i) {
				vFontFace[i] = x.vFontFace[i];
				vFontScale[i] = x.vFontScale[i];
				vThickness[i] = x.vThickness[i];
			}
		}

		//level 0 for title, level 1 for rugular
		int						vFontFace[2];
		double					vFontScale[2];
		int						vThickness[2];
		cv::Scalar				imgBgColor;
		cv::Scalar				fontColor;
	};

	//paramer set gui and read from gui
	struct CfgWrtVideo {
		CfgWrtVideo() {}
		std::string  outputVideoFolder;
		float		 outputVideoFrmRate;
		cv::Size	 outputVideoFrmSize;
		int			 outputVideoFmtIdx;
		int			 outputVideoSizeIdx;
		int			 outputVideoFpsIdx;
		int          uploadPlatformIdx;

		std::vector<std::string> vOutputVideoFileFmts;
		std::vector<std::string> vOutputVideoFrmSizes;
		std::vector<std::string> vOutputVideoFrmRates;
		std::vector<std::string> vUploadPlatforms;

		std::vector<cv::Size> vOutputVideoFrmSizes_value;
		std::vector<float>    vOutputVideoFrmRates_value;

		std::string  headerVidoeFilePath;
		std::string  insertingVidoeFilePath;
		float32		 originalAudioIntensity;
		float32      highlightSegmentLengthInSec;         //the video segment in [tEndAppausing-highlightSegmentLengthInSec, tEndAppausing] will be cut
		float32      minApplausingTimeSec;
		float32		 minSegmentTimeSec;
		bool		 isRenderTeamInfo;
		bool		 showDebugMsg;
		AutoCutMethod cutMethod;
	};
	typedef std::shared_ptr<CfgWrtVideo>		CfgWrtVideoPtr;

	struct CfgGui {
		int	fontSize;
		int dispImgWidth;
		int dispImgHeight;
		int dispImgWidthInit;
		int dispImgHeightInit;
		int thumbnailWidth;
		int thumbnailHeight;
	};
	typedef std::shared_ptr<CfgGui>		CfgGuiPtr;

	struct CfgLog {
		CfgLog() : logFolder("c:/temp"), projProfileFolder("c:/temp/projectProfile"),
			isDumpLog(1), isShowLogInConsole(0), isDumpSelectedRawAudio(0) {}

		std::string logFolder;
		std::string projProfileFolder;
		bool        isDumpLog;
		bool        isShowLogInConsole;
		bool		isDumpSelectedRawAudio;  //dump the applausing  audio piece for evaluate the applausing detetctio algorithm
	};
	typedef std::shared_ptr<CfgLog>		CfgLogPtr;


	//cfg for DataContainer
	struct  CfgDc {
		CfgDc() : qSize(10) {};
		int		qSize;
	};

}
#endif
