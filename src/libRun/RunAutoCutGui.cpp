/*
*-------------------------------------------------------------------
* RunAutoCutGui.cpp
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
#include "RunAutoCutGui.h"
using namespace std;
using namespace autocut;

#define MY_QT_CONN				Qt::UniqueConnection
RunAutoCutGui::RunAutoCutGui(CfgAcPtr &cfg, QWidget *parent)
: m_cfg ( cfg )
, m_frmNum( 0 )
, m_guiReady(false)
, m_analyzingDone(false)
, m_ui( new autocut::AutoCutGui() )
, m_sharedDC(0)
//, m_browseThread1( 0 )
, m_mgrThread(0)
, m_runMgr(0)
, m_runBrowseVideo(0)
, m_thumbnailFrm( 0 )
, m_dispFrm( new DispFrm( cfg->m_gui.dispImgWidth, cfg->m_gui.dispImgHeight, CV_8UC3 ) )
, m_currFileId(0)
, m_startProcess_button_task(0)
, m_saveDispImg4FileIdSwitch(false)
, m_currVideoKeyFrmIdx(0)
, m_isShowKeyFrm(false)
{
//--------------------------------------------------------------------
//start GUI
//--------------------------------------------------------------------
m_sharedDC.reset( new DataContainer( m_cfg->m_dc0.qSize ) );

m_mgrThread.reset(new RunMgr());
m_mgrThread->setCfg(m_cfg);
m_mgrThread->setDC(m_sharedDC);
m_mgrThread->start();
m_runMgr = dynamic_cast<RunMgr *>(m_mgrThread.get());

m_browseThread.reset(new RunBrowseVideo());
m_browseThread->setDC(m_sharedDC);
m_browseThread->setCfg(m_cfg);
m_browseThread->start();   //entering into RunThread::run() forever loop
m_runBrowseVideo = dynamic_cast<RunBrowseVideo *>(m_browseThread.get());

//start GUI
m_ui->setupUi(this, m_cfg );

//Menu item actions
QObject::connect(m_ui->actionExit, SIGNAL(triggered()), this, SLOT(on_actionExit_triggered()), MY_QT_CONN);
QObject::connect(m_ui->actionAbout, SIGNAL(triggered()), this, SLOT(on_actionAbout_triggered()), MY_QT_CONN);
QObject::connect(m_ui->actionHelp, SIGNAL(triggered()), this, SLOT(on_actionHelp_triggered()), MY_QT_CONN);

//Game Info group_box
QObject::connect(m_ui->lineEdit_gameName, SIGNAL(textEdited(const QString &)), this, SLOT(on_lineEdit_gameName_edited(const QString &)), MY_QT_CONN);
QObject::connect(m_ui->dateEdit_gameDate, SIGNAL(dateChanged(const QDate &)), this, SLOT(on_dateEdit_gameDate_changed(const QDate &)),  MY_QT_CONN);
QObject::connect(m_ui->lineEdit_gameScore, SIGNAL(textEdited(const QString &)), this, SLOT(on_lineEdit_gameScore_edited(const QString &)), MY_QT_CONN);
QObject::connect(m_ui->lineEdit_gameVideoFolder, SIGNAL(textEdited(const QString &)), this, SLOT(on_lineEdit_inputVideoFolder_edited(const QString &)), MY_QT_CONN);
QObject::connect(m_ui->pushBotton_browseVideoFolder, SIGNAL(clicked()), this, SLOT(on_pushBotton_browseInputVideoFolder_clicked()), MY_QT_CONN);
QObject::connect(m_ui->lineEdit_gameMp3Path, SIGNAL(textEdited(const QString &)), this, SLOT(on_lineEdit_gameMp3Path_edited(const QString &)), MY_QT_CONN);
QObject::connect(m_ui->pushBotton_addMp3File, SIGNAL(clicked()), this, SLOT(on_pushBotton_addMp3File_clicked()), MY_QT_CONN);
QObject::connect(m_ui->pushBotton_removeAll, SIGNAL(clicked()), this, SLOT(on_pushBotton_removeAll_clicked()), MY_QT_CONN);
QObject::connect(m_ui->pushBotton_removeOne, SIGNAL(clicked()), this, SLOT(on_pushBotton_removeOne_clicked()), MY_QT_CONN);
QObject::connect(m_ui->pushBotton_addAll, SIGNAL(clicked()), this, SLOT(on_pushBotton_addAll_clicked()), MY_QT_CONN);
QObject::connect(m_ui->pushBotton_addOne, SIGNAL(clicked()), this, SLOT(on_pushBotton_addOne_clicked()), MY_QT_CONN);
QObject::connect(m_ui->tableWidget_gameVidoeFiles, SIGNAL(clicked(const QModelIndex &)), this, SLOT(on_tableWidget_clicked(const QModelIndex &)), MY_QT_CONN);


//core parameter group_box
QObject::connect(m_ui->slider_param[0], SIGNAL(valueChanged(int )), this, SLOT(on_slider0_valueChgd(int)), MY_QT_CONN);
QObject::connect(m_ui->slider_param[1], SIGNAL(valueChanged(int)), this, SLOT(on_slider1_valueChgd(int)), MY_QT_CONN);
QObject::connect(m_ui->slider_param[2], SIGNAL(valueChanged(int)), this, SLOT(on_slider2_valueChgd(int)), MY_QT_CONN);
QObject::connect(m_ui->slider_param[3], SIGNAL(valueChanged(int)), this, SLOT(on_slider3_valueChgd(int)), MY_QT_CONN);
QObject::connect(m_ui->slider_param[4], SIGNAL(valueChanged(int)), this, SLOT(on_slider4_valueChgd(int)), MY_QT_CONN);

//ouput group_box
QObject::connect(m_ui->pushBotton_browseOutputFolder, SIGNAL(clicked()), this, SLOT(on_pushBotton_browseOutputVideoFolder_clicked()), MY_QT_CONN);
QObject::connect(m_ui->lineEdit_outputFolder, SIGNAL(textEdited(const QString &)), this, SLOT(on_lineEdit_outputVideoFolder_edited(const QString &)), MY_QT_CONN);
QObject::connect(m_ui->comboBox_outputVideoFileFmt, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_outputVideoFileFmt_currentIndexChanged(int)), MY_QT_CONN);
QObject::connect(m_ui->comboBox_outputFrmSize, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_outputVideoFrmSize_currentIndexChanged(int)), MY_QT_CONN);
QObject::connect(m_ui->comboBox_outputFrmRate, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_outputVideoFrmRate_currentIndexChanged(int)), MY_QT_CONN);
QObject::connect(m_ui->checkBox_renderTeamInfo, SIGNAL(stateChanged(int)), this, SLOT(on_checkBox_renderTeamInfo_stateChgd(int)), MY_QT_CONN);

//cmdButtons groupBox
QObject::connect(m_ui->pushButton_startProcess, SIGNAL(clicked()), this, SLOT(on_pushButton_startProcess_clicked()), MY_QT_CONN);
QObject::connect(m_ui->pushButton_quit, SIGNAL(clicked()), this, SLOT(on_pushButton_quit_clicked()), MY_QT_CONN);
QObject::connect(m_ui->pushButton_reset, SIGNAL(clicked()), this, SLOT(on_pushButton_reset_clicked()), MY_QT_CONN);
QObject::connect(m_ui->comboBox_uploadPlatforms, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_uploadPlatform_currentIndexChanged(int)), MY_QT_CONN);
QObject::connect(m_ui->pushButton_upload, SIGNAL(clicked()), this, SLOT(on_pushButton_upload_clicked()), MY_QT_CONN);
QObject::connect(m_ui->comboBox_methods, SIGNAL(currentIndexChanged(int)), this, SLOT(on_comboBox_methods_currentIndexChanged(int)), MY_QT_CONN);

//if user selected an roi from current frame
//QObject::connect(m_ui->label_img, SIGNAL(sigRectSelected( const QRect&)), this, SLOT(on_roi_selected_by_user(const QRect&)), MY_QT_CONN);

//Browse slider
QObject::connect(m_ui->slider_browseKeyFrm, SIGNAL(valueChanged(int)), this, SLOT(on_keyFrmSlider_valueChgd(int)), MY_QT_CONN);
QObject::connect(m_ui->slider_browseKeyFrm, SIGNAL(sliderPressed()), this, SLOT(on_keyFrmSlider_pressed()), MY_QT_CONN);
//QObject::connect(m_ui->slider_browseKeyFrm, SIGNAL(sliderMoved()), this, SLOT(on_keyFrmSlider_moved()), MY_QT_CONN);
QObject::connect(m_ui->slider_browseKeyFrm, SIGNAL(sliderReleased()), this, SLOT(on_keyFrmSlider_released()), MY_QT_CONN);
QObject::connect(m_ui->pushbutton_addRngSlider, SIGNAL(clicked()), this, SLOT(on_pushbutton_addSlider_clicked()), MY_QT_CONN);
QObject::connect(m_ui->pushbutton_delRngSlider, SIGNAL(clicked()), this, SLOT(on_pushbutton_delSlider_clicked()), MY_QT_CONN);
QObject::connect(m_ui->rngSliderMgr, SIGNAL(sigWarningMsg(const QString &)), this, SLOT(respns_warning_msg(const QString&)), MY_QT_CONN);


QObject::connect(m_ui->player, SIGNAL(play()), this, SLOT(respns_play()), MY_QT_CONN);
QObject::connect(m_ui->player, SIGNAL(pause()), this, SLOT(respns_pause()), MY_QT_CONN);
QObject::connect(m_ui->player, SIGNAL(stop()), this, SLOT(respns_stop()), MY_QT_CONN);
QObject::connect(m_ui->player, SIGNAL(next()), this, SLOT(respns_next()), MY_QT_CONN);
QObject::connect(m_ui->player, SIGNAL(previous()), this, SLOT(respns_previous()), MY_QT_CONN);
QObject::connect(m_ui->player, SIGNAL(changeVolume( int )), this, SLOT(respns_changeVolume( int )), MY_QT_CONN);
QObject::connect(m_ui->player, SIGNAL(changeMuting(bool)), this, SLOT(respns_changeMuting( bool )), MY_QT_CONN);
QObject::connect(m_ui->player, SIGNAL(changeRate(qreal)), this, SLOT(respns_changePlayFrmRate(qreal)), MY_QT_CONN);

QObject::connect(m_mgrThread.get(), SIGNAL(sigHasErr(const QString &)), this, SLOT(respns_mgrHasErr(const QString&)), MY_QT_CONN);
QObject::connect(m_mgrThread.get(), SIGNAL(sigProgress(const QString &)), this, SLOT(respns_mgrProgress_msg(const QString&)), MY_QT_CONN);
QObject::connect(m_mgrThread.get(), SIGNAL(sigAllDexed(const QString &)), this, SLOT(respns_mgrAllDexed(const QString &)), MY_QT_CONN);
QObject::connect(m_mgrThread.get(), SIGNAL(sigAllAnalyzed(const QString &)), this, SLOT(respns_mgrAllAnalyzed(const QString &)), MY_QT_CONN);
QObject::connect(m_mgrThread.get(), SIGNAL(sigWrtOutputDone(const QString &)), this, SLOT(respns_mgrWrtOutputDone(const QString &)), MY_QT_CONN);

QObject::connect(m_browseThread.get(), SIGNAL(sigDispImg()), this, SLOT(respns_dispImg()), MY_QT_CONN);
QObject::connect(m_browseThread.get(), SIGNAL(sigHasErr(const QString&)), this, SLOT(respns_mgrHasErr(const QString&)), MY_QT_CONN);
QObject::connect(m_browseThread.get(), SIGNAL(sigDexFrms()), m_mgrThread.get(), SLOT(wakeupWorkerToDex()), MY_QT_CONN);

int w0 = 300, h0 = 100;
QRect r = m_ui->groupBox_imgWin->rect();
boost::this_thread::sleep(boost::posix_time::milliseconds(500));
m_guiReady = true;
}


RunAutoCutGui::~RunAutoCutGui()
{
}


//showing thumbs up image groupBox
void RunAutoCutGui::on_pushButton_startProcess_clicked()
{
if (m_startProcess_button_task == 0) {
vector<string> v;
m_ui->getInputVideoFiles(v);
m_nTotInputFiles = v.size();
m_ui->rngSliderMgr->initGroups(m_nTotInputFiles);
m_ui->setGuiEnabelbility(false);
m_ui->tableWidget_gameVidoeFiles->selectRow(0); //always set to start row

if (m_nTotInputFiles < 1) {
popupInfoMsg(POPUP_MSG_WIN_TITLE, "There is no input video files, please add some", 2000 /*, QRect *placeRect = NULL*/);
return;
}
m_cfg->updateInputVideoFileNames(v);
m_runMgr->setProjProfileInfo();
if (!m_runMgr->resetWorkers()) {
m_ui->setGuiEnabelbility(true);
return;
};
m_runMgr->startDexWhole();
for (int i = 0; i < 3; ++i) {
m_progressPercentage[i] = 0;
m_timeUsedSec[i] = 0;
}

DispFrm df(m_dispFrm->m_img.size(), CV_8UC3);
m_currFileId = 0;
m_vUsrSelectedKeyDispFrms.clear();
m_vUsrSelectedKeyFrmRngs.resize(m_nTotInputFiles);
for (int i = 0; i < m_nTotInputFiles; ++i) {
m_runMgr->getFirstFrm2Disp(i, df);
m_vUsrSelectedKeyDispFrms.push_back( df );
}
//dispImg( &m_vUsrSelectedKeyDispFrms[m_currFileId] );
m_ui->progressBar_subprocess[0]->setValue(5); //fake to do analyzing
}
else {
//update user selected keyFrmRngs
std::vector<RngWorld> vKeyFrmRngs;   //v[i].lower -- startKeyFrmId, v[i].upper -- endKeyFrmId  
m_ui->rngSliderMgr->getRngs(vKeyFrmRngs);
m_vUsrSelectedKeyFrmRngs[m_currFileId] = vKeyFrmRngs;

//then start writing output video
m_runMgr->startWriteOutputVideo(m_vUsrSelectedKeyFrmRngs);
m_ui->setGuiEnabelbility(false);
m_ui->pushButton_startProcess->setEnabled(false);
m_ui->pushButton_reset->setEnabled(false);
m_ui->groupBox_output->setEnabled(false);
m_ui->progressBar_subprocess[2]->setValue(5); //fake to start analyzing
}

}

void RunAutoCutGui::on_pushButton_upload_clicked()
{
int idx = m_cfg->getUploadPlatFormIdx();
cout << idx << m_cfg->m_wrtVideo.vUploadPlatforms[idx] << endl;
//todo
}

void RunAutoCutGui::on_pushButton_reset_clicked()
{
m_sharedDC->respReset();
m_runBrowseVideo->respReset();
m_runMgr->respReset();          //dex all files in parallel 
m_ui->respReset();
m_startProcess_button_task = 0;
}


void RunAutoCutGui::on_pushButton_quit_clicked()
{
respns_stop();
on_actionExit_triggered();
}

void RunAutoCutGui::on_pushBotton_addGameVideo_clicked()
{
CfgGame &g = m_cfg->m_vGames[0];
QString initFolder = QString::fromStdString(g.inputVideoFolder);
QString initFilter = QString::fromStdString(g.inputVideoFileFilter);

QString dir = QFileDialog::getOpenFileName(this, tr("Open Video File"), initFolder, initFilter);

//m_ui->lineEdit_folderName->setText(dir);

//crashed on  dir.toStdString(), need to build QT5.5 w/ VS2015
std::string fname = dir.toStdString();
m_cfg->addGameVideo( fname );
}

//Game info group_box
void RunAutoCutGui::on_lineEdit_gameName_edited(const QString &s)
{
//crashed on  dir.toStdString()
std::string gameName = s.toStdString();
m_cfg->updateGameName( gameName );
}

void RunAutoCutGui::on_dateEdit_gameDate_changed( const QDate &d)
{
std::string mmddyyyyy = d.toString().toStdString();
m_cfg->updateGameDate(mmddyyyyy);
}

void RunAutoCutGui::on_lineEdit_gameScore_edited(const QString &s)
{
std::string score = s.toStdString();
m_cfg->updateGameScore(score);
}

void RunAutoCutGui::on_lineEdit_inputVideoFolder_edited(const QString &s)
{
//crashed on  dir.toStdString(), need to build QT5.5 w / VS2015
std::string dir2 = s.toStdString();
m_cfg->updateInputVideoFolder(dir2);
m_ui->updateEnabilityGameInfoButtons();
}

void RunAutoCutGui::on_pushBotton_browseInputVideoFolder_clicked()
{
CfgGame &g = m_cfg->m_vGames[0];
QString initFolder = QString::fromStdString(g.inputVideoFolder);
QString dir = QFileDialog::getExistingDirectory(this, tr("Open Input Video Folder"), initFolder,
QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

m_ui->lineEdit_gameVideoFolder->setText(dir);
on_lineEdit_inputVideoFolder_edited(dir);
}
void RunAutoCutGui::on_pushBotton_removeAll_clicked()
{
m_ui->removeAllInputVideos();
}

void RunAutoCutGui::on_pushBotton_removeOne_clicked()
{
int idx;
string fname;
m_ui->getCurrentInputVideoFileName(idx, fname);
if (idx >= 0) {
m_ui->removeOneInputVideo(idx);
}
}

void RunAutoCutGui::on_pushBotton_addAll_clicked()
{
const CfgGame &g = m_cfg->m_vGames[0];
const string &folder = g.inputVideoFolder;
boost::filesystem::path p(folder);
if (!boost::filesystem::exists(p)) {
std::string msg = "The input video folder: " + folder + "does not exist!";
popupMsg(this, POPUP_MSG_WIN_TITLE, msg);
return;
}

vector<std::string> vFileNames;
uint32_t n = getFileNameList( folder, "*", vFileNames);
m_ui->appendAllInputVideos(vFileNames);
}

void RunAutoCutGui::on_tableWidget_clicked(const QModelIndex &idx)
{
int fileId = -1;
if (idx.isValid()) {
fileId = idx.row();
}

//RunMgr *p = dynamic_cast<RunMgr *>(m_mgrThread.get());
if (fileId != m_runMgr->getCurrWorkerId()  && m_analyzingDone) {
//save previous values user changed from GUI
std::vector<RngWorld> vKeyFrmRngs;   //v[i].lower -- startKeyFrmId, v[i].upper -- endKeyFrmId  
m_ui->rngSliderMgr->getRngs(vKeyFrmRngs);
m_vUsrSelectedKeyFrmRngs[m_currFileId] = vKeyFrmRngs;

//m_dispFrm->m_img.copyTo(m_vUsrSelectedKeyFrmSliderImgs[m_currFileId]);
//cv::imwrite("c:/temp/disp.png", m_dispFrm->m_img);
//cv::imwrite( num_to_string(m_currFileId, "c:/temp/saved_") + ".png", m_vUsrSelectedKeyFrmSliderImgs[m_currFileId]);
m_runMgr->stopDecoding();

//update ...
m_runMgr->setCurrWorkerId(fileId);
m_runBrowseVideo->clearDispQ();
m_runBrowseVideo->setLastFrmReached(false);
updateCurrVideoGuiWhenSwitchFiles(fileId);

m_currFileId = fileId;
on_keyFrmSlider_released();
}
}

void RunAutoCutGui::on_pushBotton_addOne_clicked()
{
const CfgGame &g = m_cfg->m_vGames[0];
const string &folder = g.inputVideoFolder;
boost::filesystem::path p(folder);
if (!boost::filesystem::exists(p)) {
std::string msg = "The input video folder: " + folder + "does not exist!";
popupMsg(this, POPUP_MSG_WIN_TITLE, msg); 
return;
}
QString initFolder = QString::fromStdString(folder);
QString initFilter = QString::fromStdString("*.*");

QString ff = QFileDialog::getOpenFileName(this, tr("Open mp3 File"), initFolder, initFilter);
string head, fname;
split(ff.toStdString(), head, fname);
string errMsg = m_ui->appendOneInputVideo( fname );
if (!errMsg.empty() ) {
popupMsg(this, POPUP_MSG_WIN_TITLE, errMsg);
}
}

void RunAutoCutGui::on_lineEdit_gameMp3Path_edited(const QString &s)
{
std::string f = s.toStdString();
m_cfg->updateMp3File(f);
}

void RunAutoCutGui::on_pushBotton_addMp3File_clicked()
{
string f0 = m_cfg->m_vGames[0].mp3File;
std::string head, tail;
autocut::split(f0, head, tail);

boost::filesystem::path p(head);
if ( !boost::filesystem::exists(p) ) {
head = "c:/myApps";
}
QString initFolder = QString::fromStdString(head);
QString initFilter = QString::fromStdString("*.mp3");

QString ff = QFileDialog::getOpenFileName(this, tr("Open mp3 File"), initFolder, initFilter);
m_ui->lineEdit_gameMp3Path->setText(ff);
m_cfg->updateMp3File( ff.toStdString() );
}

//ouput group_box
void RunAutoCutGui::on_lineEdit_outputVideoFolder_edited(const QString &s)
{
//crashed on  dir.toStdString()
std::string dir2 = s.toStdString();
m_cfg->updateOutputVideoFolder(dir2);
}

void RunAutoCutGui::on_pushBotton_browseOutputVideoFolder_clicked()
{
QString initFolder = QString::fromStdString(m_cfg->m_wrtVideo.outputVideoFolder);
QString dir = QFileDialog::getExistingDirectory(this, tr("Open Output Video Folder"), initFolder,
QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

m_ui->lineEdit_outputFolder->setText(dir);
//crashed on  dir.toStdString(), need to build QT5.5 w/ VS2015
std::string dir2 = dir.toStdString();
m_cfg->updateOutputVideoFolder(dir2);
}

void RunAutoCutGui::on_comboBox_outputVideoFileFmt_currentIndexChanged(int idx)
{
int newIdx = idx;
m_cfg->updateOutputVideoFmt(newIdx);
}

void RunAutoCutGui::on_comboBox_outputVideoFrmSize_currentIndexChanged(int idx)
{
m_cfg->updateOutputVideoFrmSize(idx);
}
void RunAutoCutGui::on_comboBox_outputVideoFrmRate_currentIndexChanged(int idx)
{
m_cfg->updateOutputVideoFrmRate(idx);
}

void RunAutoCutGui::on_comboBox_uploadPlatform_currentIndexChanged(int idx) {
m_cfg->updateUploadPlatform(idx);
}

//--- Core parameters
void RunAutoCutGui::on_comboBox_methods_currentIndexChanged(int idx)
{
m_cfg->updateGui_cutMethod( idx );
//idx ==0 <--> AUTO_CUT_MANMUAL
bool f = true;
if (idx == 0) {
f = false;
}
m_ui->groupBox_param->setEnabled(f);
}

void RunAutoCutGui::on_checkBox_renderTeamInfo_stateChgd(int state)
{
bool chked = (state == Qt::Checked);
m_cfg->updateGui_renderTeamInfo(chked);
}

void RunAutoCutGui::on_slider0_valueChgd(int value)
{
//cout << "sldie0:" << value << endl;
m_ui->label_param_sliders_cur[0]->setText( QString::number(value) );
m_cfg->updateParaFromSliders(0, value);
}
void RunAutoCutGui::on_slider1_valueChgd(int value)
{
//cout << "sldie1:" << value << endl;
m_ui->label_param_sliders_cur[1]->setText(QString::number(value));
m_cfg->updateParaFromSliders(1, value);
}
void RunAutoCutGui::on_slider2_valueChgd(int value)
{
//cout << "sldie2:" << value << endl;
m_ui->label_param_sliders_cur[2]->setText(QString::number(value));
m_cfg->updateParaFromSliders(2, value);
}

void RunAutoCutGui::on_slider3_valueChgd(int value)
{
//cout << "sldie3:" << value << endl;
m_ui->label_param_sliders_cur[3]->setText(QString::number(value));
m_cfg->updateParaFromSliders(3, value);
}
void RunAutoCutGui::on_slider4_valueChgd(int value)
{
//cout << "sldie4:" << value << endl;
m_ui->label_param_sliders_cur[4]->setText(QString::number(value));
m_cfg->updateParaFromSliders(4, value);
}


void RunAutoCutGui::on_keyFrmSlider_valueChgd(int keyFrmIdx)
{
//cout << "sldier_keyFrm:" << value << endl;
uint64 keyFrmNum;
m_currVideoKeyFrmIdx = keyFrmIdx;
float64 t = m_runMgr->getCurrVideoTimeSecFromKeyFrmIdx(keyFrmIdx, keyFrmNum);
m_ui->label_browseTimeCurrent->setText(QString::fromStdString(timeSecToHHMMSSxx(t)));

if (m_isShowKeyFrm) {
bool suc = m_runMgr->getThumbnailFrm(keyFrmIdx, m_thumbnailFrm);
if (suc) {
dispThumbnail();
}
}
}

void RunAutoCutGui::on_keyFrmSlider_released()
{
uint32 dt = 0, timeout_ms = 300 * 1000;

//m_progDlg->reset();
//m_progDlg->resize( 200, 100 );
//m_progDlg->setProgress(10, "Find new access location ...");
m_ui->setBusyBarGeometry(true);
QProgressBar *pb = m_ui->progressBar_subprocess[3];
pb->reset();
pb->setRange(0, 0);  //busy bar
pb->setValue(1);
//start from the previous frm, and read only one frm to current frm will be reach current frm
//uint32 searchToKeyFrmId = (m_currVideoKeyFrmIdx > 0) ? (m_currVideoKeyFrmIdx - 1) : 0;
m_runMgr->startDecoding(m_currVideoKeyFrmIdx);

m_runMgr->printBrief();
//cout << "on_keyFrmSlider_released(): m_currVideoKeyFrmIdx=" << m_currVideoKeyFrmIdx << endl;

m_saveDispImg4FileIdSwitch = true;
m_runBrowseVideo->setLastFrmReached(false);
m_runBrowseVideo->startDisp(BROWSE_FIRST_FRM_ONLY);

//m_progDlg->setProgress(100, "Find new access location ...");
dt = 1; //todo: cal <dt>
if (dt > timeout_ms) {
m_ui->setBusyBarGeometry(false);
popupMsg(this, POPUP_MSG_WIN_TITLE, "timeout to seek video file!", QMessageBox::Critical);
return;
}

//pb->setValue(100);
//m_ui->label_progress_time[3]->setText(QString::number(dt));
boost::this_thread::sleep(boost::posix_time::milliseconds(50));
m_ui->setBusyBarGeometry(false);

//m_ui->label_progress_time[3]->setText(QString("  "));
m_ui->turnThumbnailKeyFrmWinOff();
}

void RunAutoCutGui::on_keyFrmSlider_pressed()
{
m_isShowKeyFrm = true;
m_runMgr->stopDecoding();
}

void RunAutoCutGui::on_keyFrmSlider_moved()
{
//todo:
}


void RunAutoCutGui::on_pushbutton_addSlider_clicked()
{
cout << "m_currVideoKeyFrmIdx=" << m_currVideoKeyFrmIdx << endl;
m_ui->rngSliderMgr->insertByCenter(m_currVideoKeyFrmIdx);

}

void RunAutoCutGui::on_pushbutton_delSlider_clicked()
{
m_ui->rngSliderMgr->removeCurrent();
//cout << "on_pushbutton_delSlider_clicked()" << endl;
}


void RunAutoCutGui::on_actionHelp_triggered()
{
std::vector<std::string> v;

v.push_back("=== Auto Clip Cut ===");
v.push_back("line1");
v.push_back("L2 ...");
v.push_back("L3 ...");
v.push_back("L4 ...");
v.push_back("L5 ...");

std::string s = "\n";
for (int i = 0; i < v.size(); ++i) {
s += v[i] + "\n";
}

QMessageBox::information(this, POPUP_MSG_WIN_TITLE, QString::fromStdString(s));

}
void RunAutoCutGui::on_actionAbout_triggered()
{
QMessageBox::information(this, POPUP_MSG_WIN_TITLE, "          Auto Clip Cut\n          Shunguang Wu 2018.         ");
}


void RunAutoCutGui::closeEvent(QCloseEvent *event)
{
QMessageBox::StandardButton resBtn = QMessageBox::question(this, tr(POPUP_MSG_WIN_TITLE),
tr("Do you really want to close?\n"),
QMessageBox::No | QMessageBox::Yes,
QMessageBox::Yes);

if (resBtn != QMessageBox::Yes) {
event->ignore();
}
else {
on_actionExit_triggered();
//event->accept();
}
}

void RunAutoCutGui::on_actionExit_triggered()
{

std::shared_ptr<ProgDialog> progDlg( new ProgDialog(this, "Starting ...", 0, 0, 300, 100) );
progDlg->setProgress(50, "Reading configuration file!");

progDlg->setProgress(100, "Reading configuration file!");

progDlg->reset();
progDlg->resize(200, 100);
progDlg->resetWinTitle("Preparing Quit");

xLog("RunAutoCutGui::on_actionExit_triggered()--B");

progDlg->setProgress(50, "Trerminal internal threads ...");

//clear up 
m_runMgr->forceAllWorkersQuit();
m_mgrThread->forceThreadQuit();
m_browseThread->forceThreadQuit();

progDlg->setProgress(100, "Successfully clean up!");
boost::this_thread::sleep(boost::posix_time::milliseconds(100));
QApplication::quit();
}


void  RunAutoCutGui::showDebugMsg(const char *fmt, ...)
{
va_list args;
va_start(args, fmt);
vsnprintf(m_tmpBuf, TMP_BUF_SIZE, fmt, args);
va_end(args);

showDebugMsg(std::string(m_tmpBuf));
}

void RunAutoCutGui::showDebugMsg( const std::string &msg )
{
QString s("");
if (!m_guiReady || !m_cfg->m_wrtVideo.showDebugMsg) {
return;
}

m_debugMsgMutex.lock();

std::string ts = autocut::getPrettyTimeStamp(true) + ": " + msg;
m_debugMsgQ.push_back(ts);
if (m_debugMsgQ.size() > 5 ) {
m_debugMsgQ.pop_front();
}

for (int i = 0; i < m_debugMsgQ.size(); i++) {
s += QString::fromStdString(m_debugMsgQ.at(i)) + "\n";
}
m_debugMsgMutex.unlock();

m_ui->showDebugMsgs(s);

QApplication::processEvents();
}

void RunAutoCutGui::respns_resizeGui()
{
int w, h;
m_cfg->getDispImgSize(w, h);
m_ui->resizeGuiWin(w, h, false);
}

void RunAutoCutGui::respns_jobDone()
{
m_ui->setEnablebilityButtons4AutoMode(false);
m_ui->setEnablebilityGameInfo(false);
m_ui->setEnablebilityMenu(false);

popupAutoDispearMsg("Job Done!");

m_ui->setEnablebilityGameInfo(true);
m_ui->setEnablebilityMenu(true);
m_ui->pushButton_startProcess->setEnabled(true);
m_ui->pushButton_reset->setEnabled(true);
}

void RunAutoCutGui::respns_warning_msg(const QString &msg)
{
popupInfoMsg(POPUP_MSG_WIN_TITLE, msg.toStdString(), 5000 /*, QRect *placeRect = NULL*/);
}

void RunAutoCutGui::respns_mgrHasErr(const QString &errMsg)
{
popupMsg(this, POPUP_MSG_WIN_TITLE, errMsg.toStdString());
}

void RunAutoCutGui::respns_mgrProgress_msg(const QString &msg)
{
popupAutoDispearMsg(msg.toStdString());
ProgressMsg x = decodeProgressMsg(msg);
if ( x.fileIdx < 0 || x.fileIdx>=m_nTotInputFiles) {
return;
}
int id = -1;
if (x.taskId == PRO_MSG_DEMUX_AUDIO) {
id = 0;
}
else if (x.taskId == PRO_MSG_ANA_AUDIO) {
id = 1;
}
else if (x.taskId == PRO_MSG_WRT_VIDEO) {
id = 2;
}
if (id >= 0) {
m_timeUsedSec[id] += x.usedTimeSec;
m_progressPercentage[id] += x.percent;
m_ui->progressBar_subprocess[id]->setValue(m_progressPercentage[id]);
m_ui->label_progress_time[id]->setText(QString::number(m_timeUsedSec[id]));
}
xLog(msg.toStdString());
}

void RunAutoCutGui::respns_mgrAllDexed(const QString &msg)
{
xLog("RunAutoCutGui::respns_mgrAllDexed()--AAA");
m_ui->progressBar_subprocess[0]->setValue(100);
m_ui->progressBar_subprocess[1]->setValue(5); //fake to do analyzing

m_runMgr->startAnaAudio();
}

void RunAutoCutGui::respns_mgrWrtOutputDone(const QString &msg)
{
m_ui->progressBar_subprocess[2]->setValue(100);

QRect rect(10, m_ui->getGuiRect().bottom() - 120, 400, 100);
popupInfoMsg( POPUP_MSG_WIN_TITLE, msg.toStdString(), 3600000, &rect);
m_ui->pushButton_startProcess->setEnabled(true);
m_ui->pushButton_reset->setEnabled(true);
m_ui->pushButton_quit->setEnabled(true);
m_ui->groupBox_output->setEnabled(true);
}

void RunAutoCutGui::respns_mgrAllAnalyzed(const QString &msg)
{
m_currFileId = 0;
m_ui->progressBar_subprocess[1]->setValue(100);
popupAutoDispearMsg(msg.toStdString());
//xLog(msg.toStdString());

AutoCutMethod m = m_cfg->get_cutMethod();
if (m == AUTO_CUT_FULL_AUTO) {
m_runMgr->startWriteOutputVideo(m_vUsrSelectedKeyFrmRngs);
}
else {
std::vector<RngWorld> vKeyFrmRngs;
for (int i = 0; i < m_nTotInputFiles; ++i) {
m_runMgr->getAlgSelectedKeyFrmRanges(i, vKeyFrmRngs);
m_vUsrSelectedKeyFrmRngs[i] = vKeyFrmRngs;

for ( const RngWorld& r : vKeyFrmRngs ) {
cout << "fileIdx=" << i << ", rang=[" << r.lower << "," << r.upper << "]" << endl;;
}
}
respns_resizeGui();
updateCurrVideoGuiWhenSwitchFiles(m_currFileId);
m_ui->setEnablebilityKeyFrmSilder(true);
m_ui->setEnablebilityImgGroup(true);
m_ui->setEnablebilityMenu(true);
m_ui->rngSliderMgr->setEnabled(true);
m_analyzingDone = true;

//set process button
m_ui->pushButton_startProcess->setText("Save Video");
m_ui->pushButton_startProcess->setEnabled(true);
m_ui->pushButton_reset->setEnabled(true);
m_ui->pushButton_quit->setEnabled(true);
m_ui->groupBox_output->setEnabled(true);

m_startProcess_button_task = 1;
on_keyFrmSlider_pressed();
on_keyFrmSlider_released();
}
}

void RunAutoCutGui::popupAutoDispearMsg(const std::string &msg)
{
QRect rect(10, m_ui->getGuiRect().bottom() - 120, 400, 100);
popupInfoMsg(POPUP_MSG_WIN_TITLE,  msg, 1000, &rect);
}


void RunAutoCutGui::updateCurrVideoGuiWhenSwitchFiles(int fileId)
{
//RunMgr *p = dynamic_cast<RunMgr *> (m_mgrThread.get());
const DispFrm *pDispFrm = &m_vUsrSelectedKeyDispFrms[fileId];
m_currVideoKeyFrmIdx = pDispFrm->m_keyFrmIdx;
m_currVideoTotalKeyFrms = m_runMgr->getCurrVideoKeyFrms();
m_currVideoDurationSec = m_runMgr->getCurrVideoDurationSec();
m_ui->updateKeyFrmSlider(m_currVideoTotalKeyFrms-1, m_currVideoDurationSec, pDispFrm->m_keyFrmIdx );
dispImg(pDispFrm);

const std::vector<RngWorld> &vKeyFrmRngs = m_vUsrSelectedKeyFrmRngs[fileId];
RngWorld frmRngBoundary(0, m_currVideoTotalKeyFrms-1);
m_ui->updateRangemultiRngSliders(fileId, vKeyFrmRngs, frmRngBoundary);
m_ui->setEnabledGameInfo(true, false);
}

//--------------
void RunAutoCutGui::respns_play()
{
m_saveDispImg4FileIdSwitch = false;
m_runBrowseVideo->startDisp(BROWSE_UTIL_PAUSE);
m_ui->set4PlaycontrolButtonPressed(QMediaPlayer::PlayingState);
m_ui->slider_browseKeyFrm->setEnabled(false);
m_ui->pushButton_startProcess->setEnabled(false);
m_ui->pushButton_reset->setEnabled(false);
m_ui->turnThumbnailKeyFrmWinOff();
m_isShowKeyFrm = false;
}

void RunAutoCutGui::respns_pause()
{
m_runMgr->stopDecoding();  //stop decoding if need       
m_runBrowseVideo->pauseJob();
m_ui->set4PlaycontrolButtonPressed(QMediaPlayer::PausedState);
m_ui->slider_browseKeyFrm->setEnabled(true);
m_ui->pushButton_startProcess->setEnabled(true);
m_ui->pushButton_reset->setEnabled(true);

boost::mutex::scoped_lock(m_runGuiMutex);
m_vUsrSelectedKeyDispFrms[m_currFileId] = *m_dispFrm.get();
}

void RunAutoCutGui::respns_stop()
{
m_runMgr->stopDecoding();  //stop decoding if need       
m_runBrowseVideo->pauseJob();
if ( m_vUsrSelectedKeyDispFrms.size() > 0 ) {
m_vUsrSelectedKeyDispFrms[m_currFileId] = *m_dispFrm.get();
}
m_ui->set4PlaycontrolButtonPressed(QMediaPlayer::StoppedState);
m_ui->slider_browseKeyFrm->setEnabled(true);
}


void RunAutoCutGui::respns_next()
{

uint32 keyFrmIdx = getNextKeyFrmIdx();
if (keyFrmIdx == m_currVideoKeyFrmIdx) {
return;
}
on_keyFrmSlider_pressed();   //stop decoding if need       
on_keyFrmSlider_valueChgd(keyFrmIdx);
on_keyFrmSlider_released();
m_ui->slider_browseKeyFrm->setValue(keyFrmIdx);
}

void RunAutoCutGui::respns_previous()
{
uint32 keyFrmIdx = getPrevKeyFrmIdx();
if (keyFrmIdx == m_currVideoKeyFrmIdx) { 
return;
}
on_keyFrmSlider_pressed(); //stop decoding if need
on_keyFrmSlider_valueChgd(keyFrmIdx);
on_keyFrmSlider_released();
m_ui->slider_browseKeyFrm->setValue(keyFrmIdx);
}

void RunAutoCutGui::respns_changeVolume(int v)
{
}

void RunAutoCutGui::respns_changeMuting(bool flag)
{
}

void RunAutoCutGui::respns_changePlayFrmRate(qreal r)
{
m_runBrowseVideo->setFrmRate( r*30 );
}

//todo
uint32_t RunAutoCutGui::getPrevKeyFrmIdx()
{
return 0;
}

uint32_t RunAutoCutGui::getNextKeyFrmIdx()
{
return m_currVideoTotalKeyFrms;
}


void RunAutoCutGui::respns_dispImg()
{
if (m_sharedDC->tryPopDispFrm(m_dispFrm)) {
//cout << "respns_dispImg(): m_dispFrm: m_fn=" << m_dispFrm->m_fn << ", m_keyFrmIdx=" << m_dispFrm->m_keyFrmIdx << endl;
dispImg(m_dispFrm.get());
}

if ( m_saveDispImg4FileIdSwitch ) {
m_vUsrSelectedKeyDispFrms[m_currFileId] = *m_dispFrm.get();
}
}

#if 0
void RunAutoCutGui::dispImg(cv::Mat &I)
{
QPixmap I2 = autocut::cvMatToQPixmap(I);
//I2.save("c:/temp/imgToshow2.png", "PNG");
m_ui->showImg(I2);
}
#endif

void RunAutoCutGui::dispImg(const DispFrm *p)
{
if ( p->m_fn == LAST_FRM_NUM) {
respns_stop();
return;
}

QPixmap I = autocut::cvMatToQPixmap(p->m_img);
//	I.save("c:/temp/imgToshow2.png", "PNG");
m_ui->showImg(I);

uint64 keyFrmNum;
float64 t = m_runMgr->getCurrVideoTimeSecFromKeyFrmIdx(p->m_keyFrmIdx, keyFrmNum);
m_ui->label_browseTimeCurrent->setText(QString::fromStdString(timeSecToHHMMSSxx(t)));
m_ui->slider_browseKeyFrm->setValue(p->m_keyFrmIdx);

//cout << "keyFrmIdx=" << p->m_keyFrmIdx << ",keyFrmNum=" << keyFrmNum << ",fn=" << p->m_fn << endl;
}

void RunAutoCutGui::dispThumbnail()
{
cv::Mat img;

int w0 = m_cfg->getDispImgW();
int w = m_thumbnailFrm->w_;
m_thumbnailFrm->toBGR(img);

//if the dumped color is correct ehcih means the order BGR in cv matrix
//string fn = num_to_string(keyFrmIdx, "c:/temp/keyFrm") + ".png";
//cv::imwrite(fn, m_thumbnailImg); 
QPixmap I = cvMatToQPixmap( img, true /*isBGR*/);
//string fn = num_to_string(keyFrmIdx, "c:/temp/keyFrm-q") + ".png";
//I.save(QString::fromStdString(fn));

float s = (float32)m_thumbnailFrm->keyFrmId_ / (float32)m_currVideoTotalKeyFrms;
int x0 = s * w0 - w / 2;
w0 -= w;  //now <x0> is the right border
if (x0 < 0) { x0 = 0; }
else if (x0 > w0) { x0 = w0; }

m_ui->showThumnailImg(I, x0);
}

