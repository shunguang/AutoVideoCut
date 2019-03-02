/*
*-------------------------------------------------------------------
* AutoCutGui.h
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

#ifndef __AUTOCUT_GUI_H__
#define __AUTOCUT_GUI_H__

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QProgressBar>

#include "libUtil/CfgAc.h"
#include "libUtil/UtilFuncs.h"
#include "MsgBox.h"
#include "ImgLabel.h"
#include "PlayerControls.h"
#include "GuiDefs.h"
#include "MultiRngSliders.h"
namespace autocut {
class AutoCutGui : public  QWidget
{
public:
AutoCutGui();
void setupUi(QMainWindow *MainWindow, CfgAcPtr &cfg_ /*its value may be changed through gui settings*/);
const QRect & getGuiRect();
QPoint getGuiCenter();

int  getOutputVideoFmtIdx() const;
int  getOutputFrmSizeIdx() const;
int  getOutputFrmRateIdx() const;
void getCurrentInputVideoFileName(int &idx, std::string &fname);

void showImg(const QPixmap &x);
void showThumnailImg(const QPixmap &x, const int locationX);

void showDebugMsgs(const QString &msg);

void set4PlaycontrolButtonPressed(const QMediaPlayer::State s);

void setEnablebilityKeyFrmSilder(const bool f){
groupBox_browseVideo->setEnabled(f);
slider_browseKeyFrm->setEnabled(f);
}
void setEnablebilityImgGroup(const bool f) {
groupBox_imgWin->setEnabled(f);
label_img->setEnabled(f);
label_thumnailImg->setEnabled(f);
}
void setEnablebilityMenu(const bool f );
void setEnablebilityButtons4AutoMode( const bool f);
void setEnablebilityGameInfo(const bool enableEdt);

void updateEnabilityGameInfoButtons();
void updateEnabilityStartProcessButton();
void updateKeyFrmSlider( uint32 nTotKeyFrms, float32 videoDurationInSec=-1, uint32 startValue=0);
void setEnabledGameInfo(bool f1, bool f2);

void resizeGuiWin(int dispImgW, int dispImgH, bool isForInit=false);
void appendAllInputVideos( const std::vector<std::string> &vFileNames );
std::string appendOneInputVideo( const std::string  &fileName );
void removeAllInputVideos();
void removeOneInputVideo( const int idx );
void getInputVideoFiles( std::vector<std::string> &vFileNames );

void setGuiEnabelbility( const bool f );
void updateRangemultiRngSliders(const int groupId=0, const std::vector<RngWorld> &v=std::vector<RngWorld>(), const RngWorld &boundary= RngWorld());
void respReset();
void turnThumbnailKeyFrmWinOff() {
label_thumnailImg->setGeometry(QRect(0, 0, 0, 0));
}
void setBusyBarGeometry(bool on);

private:
void setupMenu(QMainWindow *mainWin);
void setupUi(QMainWindow *MainWindow);
void setupUi_gameInfo();
void setupUi_output();
void setupUi_parameters();
void setupUi_progress();
void setupUi_cmdBox();
void setupUi_browseAndPlay();
void setupUi_progressBox();

void setGroupBoxBrowseVideoGeometry();
void setGroupBoxGameInfoGeometry();
void setGroupBoxOutputGeometry();
void setGroupBoxParamGeometry();
void setGroupBoxCmdGeometry();
void setGroupBoxProgressGeometry();

void retranslateUi(QMainWindow *MainWindow);
void initSettings();
void calRects(int imgW, int imgH);
public:
QWidget *centralWidget;

//--img/video window group box
QGroupBox *groupBox_imgWin;
ImgLabel  *label_img;
QLabel    *label_thumnailImg;

//--browse video group box
QGroupBox *groupBox_browseVideo;
//QFrame    *frame_multiRngSlidersParent;
QSlider   *slider_browseKeyFrm;
QLabel    *label_browseTimeCurrent; //HH:MM:SS
QLabel    *label_browseDuration;    //HH:MM:SS
QGroupBox *playerCotrolgroup;
MultiRngSliders  *rngSliderMgr;
QPushButton		 *pushbutton_addRngSlider;
QPushButton		 *pushbutton_delRngSlider;

//------- game info ----------
QGroupBox *groupBox_gameInfo;
QLabel    *label_gameName;
QLabel    *label_gameDate;
QLabel    *label_gameScore;
QLabel    *label_gameVideoFolder;
QLabel    *label_gameAddVideos;

QLineEdit *lineEdit_gameName;
QLineEdit *lineEdit_gameScore;
QLineEdit *lineEdit_gameVideoFolder;

QPushButton *pushBotton_browseVideoFolder;

QPushButton	*pushBotton_addAll;
QPushButton *pushBotton_removeAll;
QPushButton *pushBotton_addOne;
QPushButton *pushBotton_removeOne;

QDateEdit *dateEdit_gameDate;
QTableWidget *tableWidget_gameVidoeFiles;

//------- output info ----------
QGroupBox *groupBox_output;
QLabel    *label_outputFolder;
QLineEdit *lineEdit_outputFolder;
QPushButton *pushBotton_browseOutputFolder;
QLabel    *label_outputVar[3];
QComboBox *comboBox_outputVideoFileFmt;
QComboBox *comboBox_outputFrmSize;                //w x h 
QComboBox *comboBox_outputFrmRate;            //frms/sec
QLabel    *label_gameAddMp3;
QLineEdit *lineEdit_gameMp3Path;
QPushButton *pushBotton_addMp3File;
QCheckBox *checkBox_renderTeamInfo;

//------- processing parameters ----------
#define MAX_PARAM_SLIDERS 5
QGroupBox *groupBox_param;
QLabel    *label_param_sliders_name[MAX_PARAM_SLIDERS];
QLabel    *label_param_sliders_min[MAX_PARAM_SLIDERS];
QLabel    *label_param_sliders_max[MAX_PARAM_SLIDERS];
QLabel    *label_param_sliders_cur[MAX_PARAM_SLIDERS];
QSlider   *slider_param[MAX_PARAM_SLIDERS];

//--- cmd  buttons ---
QGroupBox *groupBox_cmdButtons;
QPushButton *pushButton_startProcess;
QPushButton *pushButton_reset;
QPushButton *pushButton_quit;
QPushButton *pushButton_upload;
QLabel    *label_upload;
QComboBox *comboBox_uploadPlatforms;
QLabel    *label_methods;
QComboBox *comboBox_methods;

//---PROGRESS box -----
#define NUM_SUB_PROGRESS  4 
QGroupBox *groupBox_progress;
QLabel    *label_progress_name[NUM_SUB_PROGRESS];
QLabel    *label_progress_time[NUM_SUB_PROGRESS];
QProgressBar *progressBar_subprocess[NUM_SUB_PROGRESS];

//--- log ---
QGroupBox *groupBox_log;
QLabel *label_logMsg;

//--- menu bar and items ---
QMenuBar *menuBar;
QMenu *menuFile;
QMenu *menuHelp;
//QToolBar *toolBar;
//QStatusBar *statusbar;
QAction *actionExit;
QAction *actionHelp;
QAction *actionAbout;

PlayerControls		*player;
private:
CfgAcPtr	m_cfg;
bool		m_isShowDebugMsg;

QMainWindow *m_mainWin;
QRect	 	m_mainWinRect;
QRect		m_imgBox;
QRect		m_thumnailImgBox;  //w.r.t. <m_imgBox>

QRect		m_browseVdieoBox;
QRect		m_gameInfoBox;
QRect		m_outputBox;
QRect		m_paramBox;

QRect		m_cmdBox;
QRect		m_progressBox;

std::string m_param_sliders_names[MAX_PARAM_SLIDERS];
int         m_param_sliders_mins[MAX_PARAM_SLIDERS];
int			m_param_sliders_maxs[MAX_PARAM_SLIDERS];
int			m_param_sliders_steps[MAX_PARAM_SLIDERS];
int			m_param_sliders_curValues[MAX_PARAM_SLIDERS];
QStringList m_tableHeader;

QPixmap					m_addIcon;
QPixmap                 m_delIcon;
QPixmap					m_initBkg;
int						m_dispImgW, m_dispImgH;
int						m_dispImgWInit, m_dispImgHInit;

};
}

#endif // UI_SATGUI_H
