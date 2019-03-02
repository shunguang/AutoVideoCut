/*
*-------------------------------------------------------------------
* RunAutoCutGui.h
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
#ifndef __RUN_AUTOLABEL_GUI_H__
#define __RUN_AUTOLABEL_GUI_H__

#include "libUtil/DataTypes.h"
#include "libUtil/VisLog.h"
#include "libUtil/CfgAc.h"
#include "libUtil/DataContainer.h"

#include "libGui/QtHeaders.h"
#include "libGui/GuiUtil.h"
#include "libGui/AutoCutGui.h"
#include "libGui/MsgBox.h"
#include "libGui/ProgDialog.h"

#include "RunThread.h"
#include "RunBrowseVideo.h"
#include "RunMgr.h"
#include "RunDefs.h"

#define TMP_BUF_SIZE 1024
namespace autocut {
class LIBRUN_EXPORT RunAutoCutGui : public QMainWindow
{
Q_OBJECT

public:
//all the threads share the same cfg located at only one physical address
RunAutoCutGui( CfgAcPtr &cfg, QWidget *parent = 0);
~RunAutoCutGui();

public slots:
void on_actionExit_triggered();
void on_actionHelp_triggered();
void on_actionAbout_triggered();

//showing thumbs up image groupBox
void on_pushButton_startProcess_clicked();
void on_pushButton_upload_clicked();
void on_pushButton_quit_clicked();
void on_pushButton_reset_clicked();
void on_pushBotton_addGameVideo_clicked();

//gameInfo group_box
void on_lineEdit_gameName_edited(const QString &s);
void on_dateEdit_gameDate_changed(const QDate &date);
void on_lineEdit_gameScore_edited(const QString &s);
void on_lineEdit_inputVideoFolder_edited(const QString &s);
void on_pushBotton_browseInputVideoFolder_clicked();
void on_lineEdit_gameMp3Path_edited(const QString &s);
void on_pushBotton_addMp3File_clicked();	
void on_pushBotton_removeAll_clicked();
void on_pushBotton_removeOne_clicked();
void on_pushBotton_addAll_clicked();
void on_pushBotton_addOne_clicked();
void on_tableWidget_clicked(const QModelIndex &idx);

//core parameters
void on_comboBox_methods_currentIndexChanged(int idx);
void on_checkBox_renderTeamInfo_stateChgd(int state);
void on_slider0_valueChgd(int value);
void on_slider1_valueChgd(int value);
void on_slider2_valueChgd(int value);
void on_slider3_valueChgd(int value);
void on_slider4_valueChgd(int value);

//ouput group_box
void on_lineEdit_outputVideoFolder_edited(const QString &s);
void on_pushBotton_browseOutputVideoFolder_clicked();
void on_comboBox_outputVideoFileFmt_currentIndexChanged(int idx);
void on_comboBox_outputVideoFrmSize_currentIndexChanged(int idx);
void on_comboBox_outputVideoFrmRate_currentIndexChanged(int idx);
void on_comboBox_uploadPlatform_currentIndexChanged(int idx);
//Browse video slider Group_box
void on_keyFrmSlider_valueChgd(int value);
void on_keyFrmSlider_released();
void on_keyFrmSlider_pressed();
void on_keyFrmSlider_moved();

void on_pushbutton_addSlider_clicked();
void on_pushbutton_delSlider_clicked();

//other
void respns_jobDone();
void respns_warning_msg(const QString &msg);
void respns_resizeGui();

//respns from worker manager
void respns_mgrHasErr(const QString &errMsg);
void respns_mgrProgress_msg(const QString &msg);
void respns_mgrAllDexed(const QString &msg);
void respns_mgrAllAnalyzed(const QString &msg);
void respns_mgrWrtOutputDone(const QString &msg);

//respns from player controls
void respns_play();
void respns_pause();
void respns_stop();
void respns_next();
void respns_previous();
void respns_changeVolume(int v);
void respns_changeMuting(bool flag);
void respns_changePlayFrmRate(qreal r);


void respns_dispImg();  //disp img from shared data: <m_sharedDC>
private:
virtual void closeEvent(QCloseEvent *event);
//void checkSettings( const uint8 omapId );
void showDebugMsg(const std::string &msg);
void showDebugMsg(const char *fmt, ...);
void popupAutoDispearMsg(const std::string &msg);
void updateCurrVideoGuiWhenSwitchFiles( const int fileId );
void dispImg( const DispFrm *p );
//void dispImg(cv::Mat &I);
void dispThumbnail();
uint32_t getPrevKeyFrmIdx();
uint32_t getNextKeyFrmIdx();

private:
CfgAcPtr	m_cfg;                   //accessed by multi-threads

uint32			m_frmNum;
bool            m_analyzingDone;
bool			m_guiReady;
AutoCutGui		*m_ui;

//data container shared with other threads/filters
DataContainerPtr	m_sharedDC;
RunThreadPtr		m_browseThread;
RunThreadPtr		m_mgrThread;          //dex all files in parallel 
RunMgr			    *m_runMgr;            //m_runMgr = dynamic_cast<RunMgr *>(m_mgrThread.get());
RunBrowseVideo      *m_runBrowseVideo;    //m_runBrowseVideo=dynamic_cast<RunBrowseVideo *>(m_browseThread.get());

uint64				m_currVideoKeyFrmIdx;    //current browsing key frames idx for current video
uint64				m_currVideoTotalKeyFrms;      //total # of key frames for current video
float64				m_currVideoDurationSec;

boost::mutex m_debugMsgMutex;
char   m_tmpBuf[TMP_BUF_SIZE];
std::deque<std::string> m_debugMsgQ;
RawFrmPtr				m_thumbnailFrm;
DispFrmPtr				m_dispFrm;

int						m_nTotInputFiles;
int                     m_progressPercentage[NUM_SUB_PROGRESS]; //0-PRO_MSG_DEMUX_AUDIO, 1-PRO_MSG_ANA_AUDIO,2-PRO_MSG_WRT_VIDEO
int                     m_timeUsedSec[NUM_SUB_PROGRESS];

int								    m_currFileId;
std::vector<DispFrm>				m_vUsrSelectedKeyDispFrms;    //the latest user selected dispFrms for each video file   
std::vector<std::vector<RngWorld>>	m_vUsrSelectedKeyFrmRngs;     //key frm Rngs

int m_startProcess_button_task;     //0-the task flag of startProcess_button: decoding/analyzing,  1-write output video
bool m_saveDispImg4FileIdSwitch;
bool m_isShowKeyFrm;
boost::mutex m_runGuiMutex;

};
}
#endif // CLIENTGUI_H

