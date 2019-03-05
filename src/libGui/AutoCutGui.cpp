/*
*-------------------------------------------------------------------
* AutoCutGui.cpp
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
#include "AutoCutGui.h"

#define MAX_RNG_SLIDERS_IN_ONE_VIDEO 20
using namespace std;
using namespace autocut;

AutoCutGui::AutoCutGui()
	: m_cfg(0)
	, m_mainWinRect(10, 20, 1200, 900)
{
	Q_INIT_RESOURCE(appRes);
	//defined in appRes.qrc
	//and pre gnerated by coustom-buid tools

	m_addIcon = QPixmap(QString::fromUtf8(":/resource/add_icon.png"));
	m_delIcon = QPixmap(QString::fromUtf8(":/resource/del_icon.png"));
	m_initBkg = QPixmap(QString::fromUtf8(":/resource/InitBackgroundImg.png"));

	//QFile file("c:/temp/test.png");
	//file.open(QIODevice::WriteOnly);
	//m_addIcon.save(&file, "PNG");
}

const QRect& AutoCutGui::getGuiRect()
{
	return m_mainWinRect;
}

QPoint AutoCutGui::getGuiCenter()
{
	return m_mainWinRect.center();
}

int AutoCutGui::getOutputVideoFmtIdx() const
{
	int idx = comboBox_outputVideoFileFmt->currentIndex();
	return idx;
}

int AutoCutGui::getOutputFrmSizeIdx() const
{
	int idx = comboBox_outputFrmSize->currentIndex();
	return idx;
}

int AutoCutGui::getOutputFrmRateIdx() const
{
	int idx = comboBox_outputFrmRate->currentIndex();
	return idx;
}

void AutoCutGui::getCurrentInputVideoFileName(int &idx, string &fname)
{
	idx = -1;
	fname = "";
	if (tableWidget_gameVidoeFiles->rowCount() == 0) {
		return;
	}

	idx = tableWidget_gameVidoeFiles->currentRow();
	QTableWidgetItem* item = tableWidget_gameVidoeFiles->currentItem();
	fname = item->text().toStdString();
	cout << idx << ", fname=" << fname << endl;
}

void AutoCutGui::setupUi(QMainWindow *mainWin, CfgAcPtr &cfg_)
{
	//todo: from user set cfg
	//note: do nto change this order
	int i = 0;
	m_param_sliders_names[i] = "Mp3 %";
	m_param_sliders_mins[i] = 0;
	m_param_sliders_maxs[i] = 99;
	m_param_sliders_steps[i] = 10;

	++i;
	m_param_sliders_names[i] = "Min Applausing Time(sec)";
	m_param_sliders_mins[i] = 1;
	m_param_sliders_maxs[i] = 5;
	m_param_sliders_steps[i] = 1;

	++i;
	m_param_sliders_names[i] = "Min Hightlight Length(sec)";
	m_param_sliders_mins[i] = 3;
	m_param_sliders_maxs[i] = 10;
	m_param_sliders_steps[i] = 1;

	++i;
	m_param_sliders_names[i] = "Num. of Hightlights per Video";
	m_param_sliders_mins[i] = 2;
	m_param_sliders_maxs[i] = 10;
	m_param_sliders_steps[i] = 1;

	++i;
	m_param_sliders_names[i] = "Hightlight Length(sec)";
	m_param_sliders_mins[i] = 3;
	m_param_sliders_maxs[i] = 30;
	m_param_sliders_steps[i] = 1;

	m_cfg = cfg_;

	float tmp = 1 - m_cfg->m_wrtVideo.originalAudioIntensity;
	tmp = (tmp >= 0) ? tmp : 0;
	m_param_sliders_curValues[0] = 100 * tmp;
	m_param_sliders_curValues[1] = m_cfg->m_wrtVideo.minApplausingTimeSec;
	m_param_sliders_curValues[2] = m_cfg->m_wrtVideo.minSegmentTimeSec;
	m_param_sliders_curValues[3] = m_cfg->m_tfAna.nMaxHotSeg4EachVideoToCut;
	m_param_sliders_curValues[4] = m_cfg->m_wrtVideo.highlightSegmentLengthInSec;

	m_mainWin = mainWin;
	m_isShowDebugMsg = false;  //not changed during running time

	//define main window 
	if (mainWin->objectName().isEmpty()) {
		mainWin->setObjectName(QStringLiteral("mainWin"));
	}
	mainWin->setAnimated(false);

	//set central Widget
	centralWidget = new QWidget(mainWin);
	centralWidget->setObjectName(QStringLiteral("centralWidget"));

	mainWin->setCentralWidget(centralWidget);

	setupMenu(mainWin);
	setupUi(mainWin);
	//after setup
	retranslateUi(mainWin);


	m_cfg->getDispImgSizeInit(m_dispImgWInit, m_dispImgHInit);
	m_cfg->getDispImgSize(m_dispImgW, m_dispImgH);
	resizeGuiWin(m_dispImgWInit, m_dispImgHInit, true);
	m_dispImgWInit = m_imgBox.width();
	initSettings();

	//QMetaObject::connectSlotsByName(mainWin);
}


void AutoCutGui::setupUi(QMainWindow *MainWindow)
{
	//image Windows
	groupBox_imgWin = new QGroupBox(centralWidget);
	groupBox_imgWin->setObjectName(QStringLiteral("groupBox_imgWin"));

	label_img = new ImgLabel(groupBox_imgWin);
	label_img->setObjectName(QStringLiteral("label_img"));
	label_img->setEnabled(true);
	label_thumnailImg = new QLabel(label_img);
	label_thumnailImg->setObjectName(QStringLiteral("label_thumnailImg"));
	label_thumnailImg->setEnabled(true);

	//other group boxes
	setupUi_browseAndPlay();
	setupUi_gameInfo();
	setupUi_output();
	setupUi_parameters();
	setupUi_progress();
	setupUi_cmdBox();
	setupUi_progressBox();

	//log group box
	if (m_isShowDebugMsg) {
		groupBox_log = new QGroupBox(centralWidget);
		groupBox_log->setObjectName(QStringLiteral("groupBox_log"));
		label_logMsg = new QLabel(groupBox_log);
		label_logMsg->setObjectName(QStringLiteral("label_logMsg"));
	}
} // setupUi

//browse video panel
void AutoCutGui::setupUi_browseAndPlay()
{
	groupBox_browseVideo = new QGroupBox(centralWidget);
	groupBox_browseVideo->setObjectName(QStringLiteral("groupBox_browseVideo"));

	rngSliderMgr = new MultiRngSliders(groupBox_browseVideo, MAX_RNG_SLIDERS_IN_ONE_VIDEO);
	rngSliderMgr->setObjectName(QStringLiteral("rngSliderMgr"));
	rngSliderMgr->setEnabled(true);

	//frame_multiRngSlidersParent = new QFrame(groupBox_browseVideo);
	//frame_multiRngSlidersParent->setObjectName(QStringLiteral("frame_multiRngSlidersParent"));

	slider_browseKeyFrm = new QSlider(Qt::Horizontal, groupBox_browseVideo);
	slider_browseKeyFrm->setObjectName(QString("slider_browseKeyFrm"));
	slider_browseKeyFrm->setFocusPolicy(Qt::StrongFocus);

	label_browseTimeCurrent = new QLabel(groupBox_browseVideo); //HH:MM:SS
	label_browseTimeCurrent->setObjectName(QString("label_browseTimeCurrent"));

	label_browseDuration = new QLabel(groupBox_browseVideo);    //HH:MM:SS
	label_browseDuration->setObjectName(QString("label_browseDuration"));

	playerCotrolgroup = new QGroupBox(groupBox_browseVideo);
	playerCotrolgroup->setObjectName(QString("playerCotrolgroup"));
	playerCotrolgroup->setFlat(true);
	playerCotrolgroup->setStyleSheet("border:0;");
	player = new PlayerControls(playerCotrolgroup);

	pushbutton_addRngSlider = new QPushButton(groupBox_browseVideo);
	pushbutton_addRngSlider->setObjectName(QString("pushbutton_addRngSlider"));
	pushbutton_delRngSlider = new QPushButton(groupBox_browseVideo);
	pushbutton_delRngSlider->setObjectName(QString("pushbutton_delRngSlider"));

}

void AutoCutGui::setupUi_gameInfo()
{
	groupBox_gameInfo = new QGroupBox(centralWidget);
	groupBox_gameInfo->setObjectName(QStringLiteral("groupBox_gameInfo"));

	label_gameName = new QLabel(groupBox_gameInfo);
	label_gameDate = new QLabel(groupBox_gameInfo);
	label_gameScore = new QLabel(groupBox_gameInfo);
	label_gameVideoFolder = new QLabel(groupBox_gameInfo);
	label_gameAddVideos = new QLabel(groupBox_gameInfo);

	label_gameName->setObjectName(QStringLiteral("label_gameName"));
	label_gameDate->setObjectName(QStringLiteral("label_gameDate"));
	label_gameScore->setObjectName(QStringLiteral("label_gameScore"));
	label_gameVideoFolder->setObjectName(QStringLiteral("label_gameVideoFolder"));
	label_gameAddVideos->setObjectName(QStringLiteral("label_gameAddVideos"));

	lineEdit_gameName = new QLineEdit(groupBox_gameInfo);
	lineEdit_gameScore = new QLineEdit(groupBox_gameInfo);
	lineEdit_gameVideoFolder = new QLineEdit(groupBox_gameInfo);

	lineEdit_gameName->setObjectName(QStringLiteral("lineEdit_gameName"));
	lineEdit_gameScore->setObjectName(QStringLiteral("lineEdit_gameScore"));
	lineEdit_gameVideoFolder->setObjectName(QStringLiteral("lineEdit_gameVideoFolder"));


	pushBotton_browseVideoFolder = new QPushButton(groupBox_gameInfo);
	pushBotton_addAll = new QPushButton(groupBox_gameInfo);
	pushBotton_removeAll = new QPushButton(groupBox_gameInfo);
	pushBotton_addOne = new QPushButton(groupBox_gameInfo);
	pushBotton_removeOne = new QPushButton(groupBox_gameInfo);

	pushBotton_addAll->setObjectName(QStringLiteral("pushBotton_addAll"));
	pushBotton_removeAll->setObjectName(QStringLiteral("pushBotton_removeAll"));
	pushBotton_browseVideoFolder->setObjectName(QStringLiteral("pushBotton_browseVideoFolder"));
	pushBotton_addOne->setObjectName(QStringLiteral("pushBotton_addOne"));
	pushBotton_removeOne->setObjectName(QStringLiteral("pushBotton_removeOne"));

	dateEdit_gameDate = new QDateEdit(groupBox_gameInfo);
	dateEdit_gameDate->setObjectName(QStringLiteral("dateEdit_gameDate"));

	tableWidget_gameVidoeFiles = new QTableWidget(0, 1, groupBox_gameInfo);
	tableWidget_gameVidoeFiles->setObjectName(QStringLiteral("tableWidget_gameVidoeFiles"));
	m_tableHeader << "video file list";
	tableWidget_gameVidoeFiles->setHorizontalHeaderLabels(m_tableHeader);
	tableWidget_gameVidoeFiles->verticalHeader()->setVisible(false);
	tableWidget_gameVidoeFiles->setEditTriggers(QAbstractItemView::NoEditTriggers);
	tableWidget_gameVidoeFiles->setSelectionBehavior(QAbstractItemView::SelectRows);
	tableWidget_gameVidoeFiles->setSelectionMode(QAbstractItemView::SingleSelection);
	tableWidget_gameVidoeFiles->setShowGrid(false);
	tableWidget_gameVidoeFiles->setStyleSheet("QTableView {selection-background-color: blue;}");
	tableWidget_gameVidoeFiles->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	tableWidget_gameVidoeFiles->verticalHeader()->setDefaultSectionSize(18);
}


void AutoCutGui::setupUi_output()
{
	groupBox_output = new QGroupBox(centralWidget);
	groupBox_output->setObjectName(QStringLiteral("groupBox_output"));

	label_outputFolder = new QLabel(groupBox_output);
	label_outputFolder->setObjectName(QStringLiteral("label_outputFolder"));

	label_gameAddMp3 = new QLabel(groupBox_output);
	label_gameAddMp3->setObjectName(QStringLiteral("label_gameAddMp3"));
	lineEdit_gameMp3Path = new QLineEdit(groupBox_output);
	lineEdit_gameMp3Path->setObjectName(QStringLiteral("lineEdit_gameMp3Path"));
	pushBotton_addMp3File = new QPushButton(groupBox_output);
	pushBotton_addMp3File->setObjectName(QStringLiteral("pushBotton_addMp3File"));

	checkBox_renderTeamInfo = new  QCheckBox(groupBox_output);
	checkBox_renderTeamInfo->setObjectName(QStringLiteral("checkBox_renderTeamInfo"));

	label_param_sliders_name[0] = new QLabel(groupBox_output);
	label_param_sliders_name[0]->setObjectName(QStringLiteral("label_param_sliders_name0"));

	label_param_sliders_min[0] = new QLabel(groupBox_output);
	label_param_sliders_min[0]->setObjectName(QStringLiteral("label_param_sliders_min0"));

	label_param_sliders_max[0] = new QLabel(groupBox_output);
	label_param_sliders_max[0]->setObjectName(QStringLiteral("label_param_sliders_max0"));

	label_param_sliders_cur[0] = new QLabel(groupBox_output);
	label_param_sliders_cur[0]->setObjectName(QStringLiteral("label_param_sliders_cur0"));

	slider_param[0] = new QSlider(Qt::Horizontal, groupBox_output);
	slider_param[0]->setObjectName(QStringLiteral("slider_param0"));

	label_outputVar[0] = new QLabel(groupBox_output);
	label_outputVar[1] = new QLabel(groupBox_output);
	label_outputVar[2] = new QLabel(groupBox_output);
	label_outputVar[0]->setObjectName(QStringLiteral("label_outputVar0"));
	label_outputVar[1]->setObjectName(QStringLiteral("label_outputVar1"));
	label_outputVar[2]->setObjectName(QStringLiteral("label_outputVar2"));

	lineEdit_outputFolder = new QLineEdit(groupBox_output);
	lineEdit_outputFolder->setObjectName(QStringLiteral("lineEdit_outputFolder"));

	pushBotton_browseOutputFolder = new QPushButton(groupBox_output);
	pushBotton_browseOutputFolder->setObjectName(QStringLiteral("pushBotton_browseOutputFolder"));

	comboBox_outputVideoFileFmt = new QComboBox(groupBox_output);
	comboBox_outputVideoFileFmt->setObjectName(QStringLiteral("comboBox_outputVideoFileFmt"));
	comboBox_outputVideoFileFmt->setEditable(false);
	comboBox_outputVideoFileFmt->setMaxVisibleItems(3);
	comboBox_outputVideoFileFmt->setMaxCount(3);

	comboBox_outputFrmSize = new QComboBox(groupBox_output);
	comboBox_outputFrmSize->setObjectName(QStringLiteral("comboBox_outputFrmSize"));
	comboBox_outputFrmSize->setEditable(false);
	comboBox_outputFrmSize->setMaxVisibleItems(3);
	comboBox_outputFrmSize->setMaxCount(3);

	comboBox_outputFrmRate = new QComboBox(groupBox_output);
	comboBox_outputFrmRate->setObjectName(QStringLiteral("outputFrmRate"));
	comboBox_outputFrmRate->setEditable(false);
	comboBox_outputFrmRate->setMaxVisibleItems(3);
	comboBox_outputFrmRate->setMaxCount(3);

}

void AutoCutGui::setupUi_parameters()
{
	groupBox_param = new QGroupBox(centralWidget);
	groupBox_param->setObjectName(QStringLiteral("groupBox_param"));

	//case i=0: moved into ouput Group
	for (int i = 1; i < MAX_PARAM_SLIDERS; ++i) {
		std::string s = num_to_string(i, "label_param_sliders_name");
		label_param_sliders_name[i] = new QLabel(groupBox_param);
		label_param_sliders_name[i]->setObjectName(QString::fromStdString(s));

		s = num_to_string(i, "label_param_sliders_min");
		label_param_sliders_min[i] = new QLabel(groupBox_param);
		label_param_sliders_min[i]->setObjectName(QString::fromStdString(s));

		s = num_to_string(i, "label_param_sliders_max");
		label_param_sliders_max[i] = new QLabel(groupBox_param);
		label_param_sliders_max[i]->setObjectName(QString::fromStdString(s));

		s = num_to_string(i, "label_param_sliders_cur");
		label_param_sliders_cur[i] = new QLabel(groupBox_param);
		label_param_sliders_cur[i]->setObjectName(QString::fromStdString(s));

		s = num_to_string(i, "slider_param");
		slider_param[i] = new QSlider(Qt::Horizontal, groupBox_param);
		slider_param[i]->setObjectName(QString::fromStdString(s));
	}

}
void AutoCutGui::setupUi_progress()
{

}

void AutoCutGui::setupUi_cmdBox()
{
	//buttons group box
	groupBox_cmdButtons = new QGroupBox(centralWidget);
	groupBox_cmdButtons->setObjectName(QStringLiteral("groupBox_cmdButtons"));

	pushButton_startProcess = new QPushButton(groupBox_cmdButtons);
	pushButton_startProcess->setObjectName(QStringLiteral("pushButton_startProcess"));

	pushButton_reset = new QPushButton(groupBox_cmdButtons);
	pushButton_reset->setObjectName(QStringLiteral("pushButton_reset"));

	pushButton_quit = new QPushButton(groupBox_cmdButtons);
	pushButton_quit->setObjectName(QStringLiteral("pushButton_quit"));

	pushButton_upload = new QPushButton(groupBox_cmdButtons);
	pushButton_upload->setObjectName(QStringLiteral("pushButton_upload"));

	label_upload = new QLabel(groupBox_cmdButtons);
	label_upload->setObjectName(QStringLiteral("label_upload"));

	comboBox_uploadPlatforms = new QComboBox(groupBox_cmdButtons);
	comboBox_uploadPlatforms->setObjectName(QStringLiteral("comboBox_uploadPlatforms"));

	label_methods = new QLabel(groupBox_cmdButtons);
	label_methods->setObjectName(QStringLiteral("label_methods"));
	comboBox_methods = new QComboBox(groupBox_cmdButtons);
	comboBox_methods->setObjectName(QStringLiteral("comboBox_methods"));
}

void AutoCutGui::setupUi_progressBox()
{
	groupBox_progress = new QGroupBox(centralWidget);
	groupBox_progress->setObjectName(QStringLiteral("groupBox_progress"));
	for (int i = 0; i < NUM_SUB_PROGRESS; ++i) {
		label_progress_name[i] = new QLabel(groupBox_progress);
		label_progress_name[i]->setObjectName(QString("label_progress_name_%1").arg(i));
		label_progress_time[i] = new QLabel(groupBox_progress);
		label_progress_time[i]->setObjectName(QString("label_progress_time_%1").arg(i));
		progressBar_subprocess[i] = new QProgressBar(groupBox_progress);
		progressBar_subprocess[i]->setObjectName(QString("progressBar_subprocess_%1").arg(i));
		progressBar_subprocess[i]->setRange(0, 100);
	}
	progressBar_subprocess[3]->setRange(0, 0);  //busy bar
}

void AutoCutGui::retranslateUi(QMainWindow *MainWindow)
{
	MainWindow->setWindowTitle(QApplication::translate("MainWindow", POPUP_MSG_WIN_TITLE, 0));
	menuFile->setTitle(QApplication::translate("MainWindow", "File", 0));
	menuHelp->setTitle(QApplication::translate("MainWindow", "Help", 0));
	actionExit->setText(QApplication::translate("MainWindow", "Exit", 0));
	actionHelp->setText(QApplication::translate("MainWindow", "Help", 0));
	actionAbout->setText(QApplication::translate("MainWindow", "About", 0));

	//game info
	groupBox_gameInfo->setTitle(QApplication::translate("MainWindow", "Game Info", 0));
	label_gameName->setText(QApplication::translate("MainWindow", "Game Name", 0));
	label_gameDate->setText(QApplication::translate("MainWindow", "Date", 0));
	label_gameScore->setText(QApplication::translate("MainWindow", "Score", 0));
	label_gameVideoFolder->setText(QApplication::translate("MainWindow", "Video Folder", 0));
	label_gameAddVideos->setText(QApplication::translate("MainWindow", "Add Videos", 0));
	label_gameAddMp3->setText(QApplication::translate("MainWindow", "Add Mp3", 0));

	//lineEdit_gameName->setText(QApplication::translate("MainWindow", "Game Info", 0));
	//lineEdit_gameScore->setText(QApplication::translate("MainWindow", "Game Info", 0));
	//lineEdit_gameVideoFolder->setText(QApplication::translate("MainWindow", "Game Info", 0));
	//lineEdit_gameMp3Path->setText(QApplication::translate("MainWindow", "Game Info", 0));

	pushBotton_browseVideoFolder->setText(QApplication::translate("MainWindow", "Browse", 0));
	pushBotton_addOne->setText(QApplication::translate("MainWindow", "Append One", 0));
	pushBotton_removeOne->setText(QApplication::translate("MainWindow", "Remove One", 0));
	pushBotton_addMp3File->setText(QApplication::translate("MainWindow", "Browse", 0));

	pushBotton_addAll->setText(QApplication::translate("MainWindow", "Load All", 0));
	pushBotton_removeAll->setText(QApplication::translate("MainWindow", "Remove All", 0));

	//output
	groupBox_output->setTitle(QApplication::translate("MainWindow", "Output", 0));
	label_outputFolder->setText(QApplication::translate("MainWindow", "Folder", 0));
	label_outputVar[0]->setText(QApplication::translate("MainWindow", "Format", 0));
	label_outputVar[1]->setText(QApplication::translate("MainWindow", "Size", 0));
	label_outputVar[2]->setText(QApplication::translate("MainWindow", "FPS", 0));
	pushBotton_browseOutputFolder->setText(QApplication::translate("MainWindow", "Browse", 0));
	checkBox_renderTeamInfo->setText(QString("Render Team Info"));

	//parameters
	groupBox_param->setTitle(QApplication::translate("MainWindow", "Applausing Detection", 0));
	for (int i = 0; i < MAX_PARAM_SLIDERS; ++i) {
		label_param_sliders_name[i]->setText(QString::fromStdString(m_param_sliders_names[i]));
		label_param_sliders_min[i]->setText(QString::number(m_param_sliders_mins[i]));
		label_param_sliders_max[i]->setText(QString::number(m_param_sliders_maxs[i]));
	}

	groupBox_cmdButtons->setTitle(QString());
	pushButton_startProcess->setText(QApplication::translate("MainWindow", "Processing", 0));
	pushButton_reset->setText(QApplication::translate("MainWindow", "Reset", 0));
	pushButton_quit->setText(QApplication::translate("MainWindow", "Quit", 0));
	if (m_isShowDebugMsg) {
		groupBox_log->setTitle(QApplication::translate("MainWindow", "Log Msgs", 0));
		label_logMsg->setText(QString());
	}


	label_methods->setText(QApplication::translate("MainWindow", "Approach", 0));
	label_upload->setText(QApplication::translate("MainWindow", "UploadTo", 0));
	pushButton_upload->setText(QApplication::translate("MainWindow", "Upload", 0));


	groupBox_progress->setTitle(QApplication::translate("MainWindow", "Progress", 0));
	label_progress_name[0]->setText(QString(" Decoding:"));
	label_progress_name[1]->setText(QString("Analyzing:"));
	label_progress_name[2]->setText(QString("  Writing:"));
	label_progress_name[3]->setText(QString(""));

	pushbutton_addRngSlider->setText(QApplication::translate("MainWindow", "+", 0));
	//pushbutton_addRngSlider->setFo
	//pushbutton_addRngSlider->setIconSize(QSize(16, 16));
	pushbutton_delRngSlider->setText(QApplication::translate("MainWindow", "-", 0));
	//pushbutton_delRngSlider->setIconSize(QSize(16, 16));


} // retranslateUi

void AutoCutGui::initSettings()
{
	//game info
	std::string score;
	int yyyy, mm, dd;
	string gameName = m_cfg->getGameNameDateScore(score, yyyy, mm, dd);
	lineEdit_gameName->setText(QString::fromStdString(gameName));
	lineEdit_gameScore->setText(QString::fromStdString(score) );
	lineEdit_gameVideoFolder->setText(QString::fromStdString(m_cfg->getInputFolder()));
	lineEdit_gameMp3Path->setText(QString::fromStdString(m_cfg->getMp3FilePath()));

	QDate date(yyyy, mm, dd);
	if (yyyy == 0) {
		date = QDate::currentDate();
	}
	dateEdit_gameDate->setDate(date);

	pushBotton_browseVideoFolder->setEnabled(true);
	pushBotton_addMp3File->setEnabled(true);

	int j = 0;
	vector<string> v = m_cfg->getInputVideoFileNames();
	BOOST_FOREACH(const string &filename, v) {
		string errMsg = appendOneInputVideo(filename);
		if (!errMsg.empty()) {
			popupInfoMsg(POPUP_MSG_WIN_TITLE, errMsg, 100);
		}
	}
	updateEnabilityGameInfoButtons();

	//output
	const std::string &s = m_cfg->getOutputVideoFolder();
	lineEdit_outputFolder->setText(QString::fromStdString(s));
	v = m_cfg->getOutputVideoFmts();
	BOOST_FOREACH(const string &fmt, v) {
		comboBox_outputVideoFileFmt->addItem(QString::fromStdString(fmt));
	}
	comboBox_outputVideoFileFmt->setCurrentText(QString::fromStdString(v[0]));
	comboBox_outputVideoFileFmt->setEnabled(true);

	//img sz
	v = m_cfg->getOutputFrmSizes();
	BOOST_FOREACH(const string &sz, v) {
		comboBox_outputFrmSize->addItem(QString::fromStdString(sz));
	}
	comboBox_outputFrmSize->setCurrentText(QString::fromStdString(v[0]));
	comboBox_outputFrmSize->setEnabled(true);

	//frm rate
	v = m_cfg->getOutputFrmRates();
	BOOST_FOREACH(const string &fr, v) {
		comboBox_outputFrmRate->addItem(QString::fromStdString(fr));
	}
	comboBox_outputFrmRate->setCurrentText(QString::fromStdString(v[0]));
	comboBox_outputFrmRate->setEnabled(true);

	//upload platforms
	v = m_cfg->getUploadPlatforms();
	BOOST_FOREACH(const string &pf, v) {
		comboBox_uploadPlatforms->addItem(QString::fromStdString(pf));
	}
	comboBox_uploadPlatforms->setCurrentText(QString::fromStdString(v[0]));
	comboBox_uploadPlatforms->setEnabled(true);


	/* match def. in
	enum AutoCutMethod {
	AUTO_CUT_MANMUAL = 0,
	AUTO_CUT_SEMI_AUTO,
	AUTO_CUT_FULL_AUTO,
	};
	*/
	string vM[] = { "Manual", "Semi","Auto" };
	int mIdx = (int)(m_cfg->get_cutMethod() - AUTO_CUT_MANMUAL);
	comboBox_methods->addItem(QString::fromStdString(vM[0]));
	comboBox_methods->addItem(QString::fromStdString(vM[1]));
	comboBox_methods->addItem(QString::fromStdString(vM[2]));
	comboBox_methods->setCurrentText(QString::fromStdString(vM[mIdx]));
	comboBox_methods->setEnabled(true);
	//-------------------------------
	//initial process parameters
	//-------------------------------
	checkBox_renderTeamInfo->setChecked(true);
	for (int i = 0; i < MAX_PARAM_SLIDERS; ++i) {
		slider_param[i]->setMaximum(m_param_sliders_maxs[i]);
		slider_param[i]->setMinimum(m_param_sliders_mins[i]);
		slider_param[i]->setSingleStep(m_param_sliders_steps[i]);
		slider_param[i]->setTickPosition(QSlider::TicksAbove);
		if (i == 0) {
			slider_param[i]->setTickInterval(10);
		}
		else {
			slider_param[i]->setTickInterval(1);
		}

		slider_param[i]->setValue(m_param_sliders_curValues[i]);
		label_param_sliders_cur[i]->setText(QString::number(m_param_sliders_curValues[i]));
	}

	//Browse key frame pannel
	updateKeyFrmSlider(100, -1, 0);

	//-------------------
	pushButton_quit->setEnabled(true);
	setEnablebilityGameInfo(true);
	updateEnabilityStartProcessButton();
	setEnablebilityKeyFrmSilder(false);

	//init <rngSliderMgr>
	//printf(" rngSliderMgr->width()=%d, rngSliderMgr->height()=%d\n", rngSliderMgr->width(), rngSliderMgr->height());
	updateRangemultiRngSliders();

	pushButton_reset->setEnabled(false);

	showImg(m_initBkg.scaled(m_dispImgWInit, m_dispImgHInit, Qt::IgnoreAspectRatio));
}

void AutoCutGui::updateRangemultiRngSliders(const int groupId, const std::vector<RngWorld> &v, const RngWorld &boundary)
{
	QImage img(rngSliderMgr->width(), rngSliderMgr->height(), QImage::Format_RGB888);
	img.fill(QColor(Qt::gray).rgb());
	rngSliderMgr->setPixmap(QPixmap::fromImage(img));

	//for test <rngSliderMgr>purpose
	rngSliderMgr->resetSliders(groupId, v, boundary);
}

void AutoCutGui::updateKeyFrmSlider(uint32 nTotKeyFrms, float32 videoDurationInSec, uint32 startValue)
{
	slider_browseKeyFrm->setMaximum(nTotKeyFrms);
	slider_browseKeyFrm->setMinimum(0);
	slider_browseKeyFrm->setSingleStep(1);
	slider_browseKeyFrm->setTickPosition(QSlider::TicksBelow); //QSlider::NoTicks
	//slider_browseKeyFrm->setTickInterval(100);
	slider_browseKeyFrm->setValue(startValue);
	if (videoDurationInSec > 0) {
		label_browseDuration->setText(QString::fromStdString(timeSecToHHMMSSxx(videoDurationInSec)));
	}
	else {
		label_browseDuration->setText(QString::fromStdString("         "));
	}
}
void AutoCutGui::updateEnabilityStartProcessButton()
{
	if (tableWidget_gameVidoeFiles->rowCount() > 0) {
		pushButton_startProcess->setEnabled(true);
		pushButton_upload->setEnabled(true);
	}
	else {
		pushButton_startProcess->setEnabled(false);
		pushButton_upload->setEnabled(false);
	}
}

void AutoCutGui::getInputVideoFiles(std::vector<std::string> &vFileNames)
{
	vFileNames.clear();
	int n = tableWidget_gameVidoeFiles->rowCount();
	for (int i = 0; i < n; ++i) {
		const QTableWidgetItem *it = tableWidget_gameVidoeFiles->item(i, 0);
		vFileNames.push_back(it->text().toStdString());
	}
}

void AutoCutGui::appendAllInputVideos(const std::vector<std::string> &vFileNames)
{
	BOOST_FOREACH(const string &f, vFileNames) {
		appendOneInputVideo(f);
	}
}

std::string  AutoCutGui::appendOneInputVideo(const std::string  &fileName)
{
	QList<QTableWidgetItem *> v = tableWidget_gameVidoeFiles->findItems(QString::fromStdString(fileName), Qt::MatchExactly);
	if (v.size() > 0) {
		string msg = "file: <" + fileName + "> is already in list!";
		return msg;
	}

	int row = tableWidget_gameVidoeFiles->rowCount();
	QTableWidgetItem *fileNameItem = new QTableWidgetItem(QString::fromStdString(fileName));
	tableWidget_gameVidoeFiles->insertRow(row);
	tableWidget_gameVidoeFiles->setItem(row, 0, fileNameItem);
	return "";
}

void AutoCutGui::removeAllInputVideos()
{
	while (tableWidget_gameVidoeFiles->rowCount() > 0) {
		removeOneInputVideo(0);
	}
}

void AutoCutGui::removeOneInputVideo(const int idx)
{
	if (idx >= 0 && idx < tableWidget_gameVidoeFiles->rowCount()) {
		tableWidget_gameVidoeFiles->removeRow(idx);
	}
}

void AutoCutGui::setupMenu(QMainWindow *mainWin)
{
	//menu actions
	actionExit = new QAction(mainWin);
	actionExit->setObjectName(QStringLiteral("actionE_xit"));
	actionExit->setCheckable(false);
	//QIcon icon;
	//icon.addFile(QStringLiteral(":/ClientGui/Resources/exit_icon.jpg"), QSize(), QIcon::Normal, QIcon::Off);
	//actionExit->setIcon(icon);

	actionAbout = new QAction(mainWin);
	actionAbout->setObjectName(QStringLiteral("actionA_bout"));
	actionAbout->setCheckable(true);

	actionHelp = new QAction(mainWin);
	actionHelp->setObjectName(QStringLiteral("actionH_elp"));
	actionHelp->setCheckable(true);

	//menu bar
	menuBar = new QMenuBar(mainWin);
	menuBar->setObjectName(QStringLiteral("menuBar"));
	menuBar->setNativeMenuBar(false);

	//File
	menuFile = new QMenu(menuBar);
	menuFile->setObjectName(QStringLiteral("menu_File"));

	//Help
	menuHelp = new QMenu(menuBar);
	menuHelp->setObjectName(QStringLiteral("menu_Help"));

	//mainToolBar = new QToolBar(mainWin);
	//mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
	//mainWin->addToolBar(Qt::TopToolBarArea, mainToolBar);
	//statusBar = new QStatusBar(mainWin);
	//statusBar->setObjectName(QStringLiteral("statusBar"));
	//mainWin->setStatusBar(statusBar);

	//add actions to menu bar
	menuBar->addAction(menuFile->menuAction());
	menuBar->addAction(menuHelp->menuAction());

	menuFile->addAction(actionExit);

	menuHelp->addAction(actionHelp);
	menuHelp->addSeparator();
	menuHelp->addAction(actionAbout);

	mainWin->setMenuBar(menuBar);
}

void AutoCutGui::setEnablebilityMenu(const bool f)
{
	menuBar->setEnabled(f);
}

void AutoCutGui::setGuiEnabelbility(const bool f)
{
	menuBar->setEnabled(f);
	groupBox_imgWin->setEnabled(true);
	groupBox_browseVideo->setEnabled(f);
	groupBox_gameInfo->setEnabled(f);
	groupBox_output->setEnabled(f);
	groupBox_param->setEnabled(f);

	pushButton_startProcess->setEnabled(f);
	pushButton_upload->setEnabled(f);
	pushButton_quit->setEnabled(f);
	comboBox_methods->setEnabled(f);
}
void AutoCutGui::updateEnabilityGameInfoButtons()
{
	CfgGame &g = m_cfg->m_vGames[0];
	if (g.vInputVideoFileNames.empty()) {
		pushBotton_removeAll->setEnabled(false);
		pushBotton_removeOne->setEnabled(false);
	}
	else {
		pushBotton_removeAll->setEnabled(true);
		pushBotton_removeOne->setEnabled(true);
	}
}


void AutoCutGui::setEnabledGameInfo(bool f1, bool f2)
{
	groupBox_gameInfo->setEnabled(true);
	tableWidget_gameVidoeFiles->setEnabled(f1);
	label_gameName->setEnabled(f2);
	label_gameDate->setEnabled(f2);
	label_gameScore->setEnabled(f2);
	label_gameVideoFolder->setEnabled(f2);
	label_gameAddVideos->setEnabled(f2);
	lineEdit_gameName->setEnabled(f2);
	lineEdit_gameScore->setEnabled(f2);
	lineEdit_gameVideoFolder->setEnabled(f2);
	pushBotton_browseVideoFolder->setEnabled(f2);
	pushBotton_addAll->setEnabled(f2);
	pushBotton_removeAll->setEnabled(f2);
	pushBotton_addOne->setEnabled(f2);
	pushBotton_addOne->setEnabled(f2);
	pushBotton_removeOne->setEnabled(f2);
	dateEdit_gameDate->setEnabled(f2);

	//label_gameAddMp3->setEnabled(f2);
	//lineEdit_gameMp3Path->setEnabled(f2);
	//pushBotton_addMp3File->setEnabled(f2);
}

void AutoCutGui::setEnablebilityButtons4AutoMode(const bool f)
{
	pushButton_quit->setEnabled(f);
}

void AutoCutGui::setEnablebilityGameInfo(const bool enableEdt)
{
	groupBox_gameInfo->setEnabled(enableEdt);
}

void AutoCutGui::showDebugMsgs(const QString &msg)
{
	if (m_isShowDebugMsg) {
		label_logMsg->setText(msg);
	}
}

void AutoCutGui::showImg(const QPixmap &x)
{
	label_img->setPixmap(x);
	label_img->resetNewFram();
}

void AutoCutGui::showThumnailImg(const QPixmap &x, const int locationX)
{
	//m_thumnailImgBox.setX(locationX); has a bug
	//label_thumnailImg->setGeometry(m_thumnailImgBox);
	label_thumnailImg->setGeometry(QRect(locationX, m_thumnailImgBox.y(), m_thumnailImgBox.width(), m_thumnailImgBox.height()));
	label_thumnailImg->setPixmap(x);
	label_thumnailImg->setScaledContents(true);
}

void AutoCutGui::respReset()
{
	resizeGuiWin(m_dispImgWInit, m_dispImgHInit, true);
	setEnabledGameInfo(true, true);
	groupBox_browseVideo->setEnabled(false);
	groupBox_param->setEnabled(true);
	groupBox_output->setEnabled(true);

	pushButton_startProcess->setText("Processing");
	pushButton_startProcess->setEnabled(true);
	pushButton_upload->setEnabled(false);
	pushButton_quit->setEnabled(true);
	pushButton_reset->setEnabled(false);
	comboBox_methods->setEnabled(true);

	showImg(m_initBkg.scaled(m_dispImgWInit, m_dispImgHInit, Qt::IgnoreAspectRatio));
	groupBox_imgWin->setEnabled(true);

	slider_browseKeyFrm->setEnabled(false);

	//reset <rngSliderMgr>
	QImage img(rngSliderMgr->width(), rngSliderMgr->height(), QImage::Format_RGB888);
	img.fill(QColor(Qt::gray).rgb());
	rngSliderMgr->setPixmap(QPixmap::fromImage(img));
	rngSliderMgr->reset();
	rngSliderMgr->setEnabled(false);
	for (int i = 0; i < NUM_SUB_PROGRESS; ++i) {
		progressBar_subprocess[i]->reset();
		label_progress_time[i]->setText(QString("   "));
	}

}

void AutoCutGui::resizeGuiWin(int dispImgW, int dispImgH, bool isForInit)
{
	calRects(dispImgW, dispImgH);

	int w = m_mainWinRect.width();
	int h = m_mainWinRect.height();

	m_mainWin->setGeometry(m_mainWinRect);
	//m_mainWin->setMaximumSize(w, h);
	//m_mainWin->resize(w, h);

	centralWidget->setGeometry(0, 0, w, h);
	menuBar->setGeometry(QRect(0, 0, w, 21));

	//--
	groupBox_imgWin->setGeometry(m_imgBox);
	if (isForInit) {
		label_img->setGeometry(QRect(0, 0, m_imgBox.width(), m_imgBox.height()));
	}
	else {
		int xo = (m_imgBox.width() - dispImgW) / 2;
		int yo = (m_imgBox.height() - dispImgH) / 2;
		label_img->setGeometry(QRect(xo, yo, dispImgW, dispImgH));
	}
	label_thumnailImg->setGeometry(QRect(0, 0, 0, 0));  //donot show 

	setGroupBoxBrowseVideoGeometry();
	setGroupBoxGameInfoGeometry();
	setGroupBoxOutputGeometry();
	setGroupBoxParamGeometry();
	setGroupBoxCmdGeometry();
	setGroupBoxProgressGeometry();
}

void AutoCutGui::setGroupBoxBrowseVideoGeometry()
{
	int w = 60, h = 15, b = 5;
	int y = 2;
	groupBox_browseVideo->setGeometry(m_browseVdieoBox);

	rngSliderMgr->setGeometry(1, 0, m_browseVdieoBox.width() - 2, 15);

	y += 15;
	slider_browseKeyFrm->setGeometry(1, y, m_browseVdieoBox.width(), 15);

	y += 15;
	label_browseTimeCurrent->setGeometry(0, y, w, h);
	label_browseDuration->setGeometry(w + b, y, w, h);    //HH:MM:SS
	playerCotrolgroup->setGeometry(150, y, 300, 25);

	int d = 16;
	pushbutton_addRngSlider->setGeometry(450, y, d, d);
	pushbutton_delRngSlider->setGeometry(450 + d + 10, y, d, d);
}
void AutoCutGui::setGroupBoxGameInfoGeometry()
{
	int y = 10, x1 = 5, x2 = 80, x3 = 300, x4 = 340, x5 = 430, x6 = 460;
	int w1 = 45, w2 = 70, h1 = 21, h2 = 21;
	int dy = h2 + 5;

	groupBox_gameInfo->setGeometry(m_gameInfoBox);
	//line 1
	label_gameName->setGeometry(QRect(x1, y, x2 - x1 - 3, h1));
	lineEdit_gameName->setGeometry(QRect(x2, y, x3 - x2 - 5, h1));
	label_gameDate->setGeometry(QRect(x3, y, 35, h1));
	dateEdit_gameDate->setGeometry(QRect(x4, y, 75, h1));
	label_gameScore->setGeometry(QRect(x5, y, 35, h1));
	lineEdit_gameScore->setGeometry(QRect(x6, y, w1, h1));
	QRect r = lineEdit_gameScore->geometry();
	int xEnd = r.right();

	//line 2
	y += dy;
	label_gameVideoFolder->setGeometry(QRect(x1, y, w2, h1));
	lineEdit_gameVideoFolder->setGeometry(QRect(x2, y, x6 - x2 - 5, h1));;
	pushBotton_browseVideoFolder->setGeometry(QRect(x6, y, w1, h1));

	//line 3
	y += dy;
	label_gameAddVideos->setGeometry(QRect(x1, y, w2, h1));
	int y3 = y;

	//line 4-7
	y += dy + 15;
	pushBotton_addAll->setGeometry(QRect(x1, y, w2, h2));
	y += dy;
	pushBotton_removeAll->setGeometry(QRect(x1, y, w2, h2));
	y += dy;
	pushBotton_addOne->setGeometry(QRect(x1, y, w2, h2));
	y += dy;
	pushBotton_removeOne->setGeometry(QRect(x1, y, w2, h2));


	//input file table
	h2 = m_gameInfoBox.height() - (y3 + 10);
	tableWidget_gameVidoeFiles->setColumnWidth(0, xEnd - x2);
	tableWidget_gameVidoeFiles->setGeometry(QRect(x2, y3, xEnd - x2, h2));
}

void AutoCutGui::setGroupBoxOutputGeometry()
{
	int w0 = m_outputBox.width();
	int y = 10, x1 = 5, dy = 26, w1 = 50, h1 = 21, w3 = 51;
	int x3 = w0 - w3 - 5; //last one
	int x2 = x1 + w1 + 5;
	int w2 = x3 - x2 - 10;

	groupBox_output->setGeometry(m_outputBox);

	//line 1
	label_gameAddMp3->setGeometry(QRect(x1, y, w1, h1));
	lineEdit_gameMp3Path->setGeometry(QRect(x2, y, w2, h1));
	pushBotton_addMp3File->setGeometry(QRect(x3, y, w3, h1));

	//line 2
	y += dy;
	label_param_sliders_name[0]->setGeometry(QRect(x1, y, w1, h1));
	label_param_sliders_min[0]->setGeometry(QRect(x2, y, 10, h1));
	slider_param[0]->setGeometry(QRect(x2 + 15, y, 70, h1));
	label_param_sliders_max[0]->setGeometry(QRect(x2 + 90, y, 30, h1));
	label_param_sliders_cur[0]->setGeometry(QRect(x2 + 125, y, 30, h1));
	checkBox_renderTeamInfo->setGeometry(QRect(x2 + 160, y, 120, h1));

	//line 3
	y += dy;
	label_outputFolder->setGeometry(QRect(x1, y, w1, h1));
	lineEdit_outputFolder->setGeometry(QRect(x2, y, w2, h1));
	pushBotton_browseOutputFolder->setGeometry(QRect(x3, y, w3, h1));

	//line 4
	y += dy;
	w2 = 51;
	label_outputVar[0]->setGeometry(QRect(x1, y, w1, h1));
	comboBox_outputVideoFileFmt->setGeometry(QRect(x2, y, w2, h1));

	x2 = 141;
	w2 = 27; w3 = 81;
	label_outputVar[1]->setGeometry(QRect(x2, y, w2, h1));
	comboBox_outputFrmSize->setGeometry(QRect(x2 + w2, y, w3, h1));

	x3 = w0 - 51 - 5; w3 = 51;
	w2 = 21;
	label_outputVar[2]->setGeometry(QRect(x3 - w2 - 5, y, w2, h1));
	comboBox_outputFrmRate->setGeometry(QRect(x3, y, w3, h1));
}

void AutoCutGui::setGroupBoxParamGeometry()
{
	int dy = 21, h = 17;
	int w0 = m_paramBox.width(),
		w1 = 150, w2 = 10, w3 = 70, w4 = 30, w5 = 30;

	int y = 15, x1 = 10;
	int x2 = x1 + w1 + 5; //min
	int x3 = x2 + w2 + 5; //slider
	int x4 = x3 + w3 + 5; //max 
	int x5 = x4 + w4 + 5;

	groupBox_param->setGeometry(m_paramBox);
	//case i=0: moved into ouput Group
	for (int i = 1; i < MAX_PARAM_SLIDERS; ++i) {
		label_param_sliders_name[i]->setGeometry(QRect(x1, y, w1, h));
		label_param_sliders_min[i]->setGeometry(QRect(x2, y, w2, h));
		slider_param[i]->setGeometry(QRect(x3, y, w3, h));
		label_param_sliders_max[i]->setGeometry(QRect(x4, y, w4, h));
		label_param_sliders_cur[i]->setGeometry(QRect(x5, y, w5, h));
		y += dy;
	}
}

void AutoCutGui::setGroupBoxCmdGeometry()
{
	int w0 = m_cmdBox.width();
	int w1 = 70, w2 = 50, w3 = 50, h = 20;
	int vSpace = 10;
	int hSpace = (w0 - (w1 + w2 + w3)) / 4;
	int y1 = 10;
	int x1 = hSpace;
	const int x2 = x1 + w1 + hSpace;
	int x3 = x2 + w2 + hSpace;
	groupBox_cmdButtons->setGeometry(m_cmdBox);

	pushButton_startProcess->setGeometry(QRect(x1, y1, w1, h));
	pushButton_reset->setGeometry(QRect(x2, y1, w2, h));
	pushButton_quit->setGeometry(QRect(x3, y1, w3, h));

	y1 += (h + vSpace); w1 = 45; w2 = 65;
	label_methods->setGeometry(QRect(x1, y1, w1, h));
	comboBox_methods->setGeometry(QRect(x1 + w1 + 5, y1, w2, h));

	y1 += (h + vSpace);
	label_upload->setGeometry(QRect(x1, y1, w1, h));
	x1 += w1 + 5;
	comboBox_uploadPlatforms->setGeometry(QRect(x1, y1, w2, h));
	if (x1 + w2 + 10 <= x2) {  //when there is enough spacem align with "reset"
		x3 = x2;
	}
	pushButton_upload->setGeometry(QRect(x3, y1, w3, h));
}

void AutoCutGui::setGroupBoxProgressGeometry()
{
	const int b = 5;
	const int h0 = m_progressBox.height();
	const int w0 = m_progressBox.width();
	const int w1 = 50, w3 = 30, h = 20;
	int y = 15, x1 = 5;
	const int x2 = x1 + w1 + b;
	const int w2 = w0 - (w1 + w3 + b + b);
	const int x3 = x2 + w2 + b;

	groupBox_progress->setGeometry(m_progressBox);
	for (int i = 0; i < NUM_SUB_PROGRESS - 1; ++i) {
		label_progress_name[i]->setGeometry(QRect(x1, y, w1, h));
		progressBar_subprocess[i]->setGeometry(QRect(x2, y, w2, h - 5));
		label_progress_time[i]->setGeometry(QRect(x3, y, w3, h));
		y += (h + b);
	}
	//int t = m_cmdBox.width();
	setBusyBarGeometry(false);
}

void AutoCutGui::setBusyBarGeometry(bool on)
{
	if (on) {
		const int h0 = m_progressBox.height();
		const int w0 = m_progressBox.width();
		progressBar_subprocess[3]->setGeometry(QRect(5, h0 - 15, w0, 10));
	}
	else {
		progressBar_subprocess[3]->setGeometry(QRect(0, 0, 0, 0));
	}
}

void AutoCutGui::calRects(int imgW, int imgH)
{
	//params
	int b = 5;				//border size inside box
	int buttonBox_w = 100;
	int gameInfoBox_w = 521, gameInfoBox_h = 231, outputBox_h = 115;
	int paramBox_w = 350;
	int outputBox_w = paramBox_w, paramBox_h = gameInfoBox_h - outputBox_h - b;
	int cmdBox_w = 240, cmdBox_h = 100;
	QPoint spaceBtwGroups(10, 5);

	int minImgW = gameInfoBox_w + paramBox_w + cmdBox_w + 3 * b;
	int minImgH = 0;
	int browseVideoBoxH = 55;

	int imgW0 = (imgW > minImgW) ? imgW : minImgW;
	int imgH0 = (imgH > minImgH) ? imgH : minImgH;

	//calculated params
	m_imgBox.setRect(b, b, imgW0, imgH0);
	cmdBox_w = m_imgBox.width() - (gameInfoBox_w + paramBox_w + 2 * b);

	m_browseVdieoBox.setRect(b, m_imgBox.bottom(), m_imgBox.width(), browseVideoBoxH);
	int y = m_browseVdieoBox.bottom() + spaceBtwGroups.y();
	m_gameInfoBox.setRect(b, y, gameInfoBox_w, gameInfoBox_h);

	m_outputBox.setRect(m_gameInfoBox.right() + b, y, outputBox_w, outputBox_h);
	m_paramBox.setRect(m_gameInfoBox.right() + b, m_outputBox.bottom() + b, paramBox_w, paramBox_h);

	m_cmdBox.setRect(m_outputBox.right() + b, y, cmdBox_w, cmdBox_h);
	m_progressBox.setRect(m_cmdBox.left(), m_cmdBox.bottom() + b, m_cmdBox.width(), gameInfoBox_h - cmdBox_h - b);
	m_thumnailImgBox.setRect(0, imgH - m_cfg->m_gui.thumbnailHeight - 10, m_cfg->m_gui.thumbnailWidth, m_cfg->m_gui.thumbnailHeight);

	m_mainWinRect.setWidth(imgW0 + 20);
	m_mainWinRect.setHeight(m_gameInfoBox.bottom() - m_imgBox.top() + 30);
}

void AutoCutGui::set4PlaycontrolButtonPressed(const QMediaPlayer::State s)
{
	player->setState(s);
	switch (s) {
	case QMediaPlayer::PlayingState:
		tableWidget_gameVidoeFiles->setEnabled(false);
		pushbutton_addRngSlider->setEnabled(false);
		pushbutton_delRngSlider->setEnabled(false);
		break;
	case QMediaPlayer::PausedState:
	case QMediaPlayer::StoppedState:
		tableWidget_gameVidoeFiles->setEnabled(true);
		pushbutton_addRngSlider->setEnabled(true);
		pushbutton_delRngSlider->setEnabled(true);
		break;
	}
}
