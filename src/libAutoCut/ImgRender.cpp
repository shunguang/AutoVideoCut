/*
*-------------------------------------------------------------------
* ImgRender.cpp
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
#include "ImgRender.h"

using namespace std;
using namespace cv;
using namespace autocut;

ImgRender::ImgRender(const int w, const int h, const CfgRenderImg  &cfg )
: m_w(w)
, m_h(h) 
, m_param ( cfg )
{
}

ImgRender::~ImgRender()
{
}

cv::Mat ImgRender::getImg( const std::string &s )
{
cv::Mat I(m_h, m_w, CV_8UC3, m_param.imgBgColor);

int baseline;
cv::Size textSize = getTextSize( s, m_param.vFontFace[0], m_param.vFontScale[0], m_param.vThickness[0], &baseline);
baseline += m_param.vThickness[0];

// center the text
Point textOrg( (I.cols- textSize.width) / 2,	(I.rows + textSize.height) / 2);

putText( I, s, textOrg, m_param.vFontFace[0], m_param.vFontScale[0], m_param.fontColor, m_param.vThickness[0], 8);

return I;
}

cv::Mat ImgRender::getImg( const std::vector <std::string> &v)
{
cv::Mat x(m_h, m_w, CV_8UC3);
return x;
}

cv::Mat ImgRender::getGameImg(const CfgGame &game)
{
cv::Mat I(m_h, m_w, CV_8UC3, m_param.imgBgColor);
int baseline, L = 0;

string s1 = game.name;
if (s1.empty()) {
s1 = "Game Name Unknown";
}
string s2 = game.date;
if (s2.empty()) {
s2 = "Game Date: Unknown";
}
string s3 = game.score;

cv::Size textSize = getTextSize( s1, m_param.vFontFace[L], m_param.vFontScale[L], m_param.vThickness[L], &baseline);
int y = m_h/2 - textSize.height - 25;
if (y < 0) {
y = textSize.height;
}
Point po((m_w - textSize.width) / 2, y);
putText(I, s1, po, m_param.vFontFace[L], m_param.vFontScale[L], m_param.fontColor, m_param.vThickness[L], 8);

textSize = getTextSize(s2, m_param.vFontFace[L], m_param.vFontScale[L], m_param.vThickness[L], &baseline);
po.y += (textSize.height + 25);
po.x = (m_w - textSize.width) / 2;
putText(I, s2, po, m_param.vFontFace[L], m_param.vFontScale[L], m_param.fontColor, m_param.vThickness[L], 8);

if (!s3.empty()) {
textSize = getTextSize(s3, m_param.vFontFace[L], m_param.vFontScale[L], m_param.vThickness[L], &baseline);
po.y += (textSize.height + 25);
po.x = (m_w - textSize.width) / 2;
putText(I, s3, po, m_param.vFontFace[L], m_param.vFontScale[L], m_param.fontColor, m_param.vThickness[L], 8);
}

return I;
}


int ImgRender::getTeamImgs(std::vector<cv::Mat> &vImgs, const CfgTeam &team, const bool isRenderTeamInfo)
{
const int outSideB = 50;   //like page margin 
const int nPlayersPerImg = 6;           //# of players on one image
int nPlayers = team.vPlayer.size();
int nManagers = team.vManager.size() + team.vCoach.size() + team.vTreasurer.size();
int nImgs = ceil( nPlayers / (float)nPlayersPerImg);   

int nCols = 2;
int nRows = nPlayersPerImg/nCols;
int lineSpace = 20;
int colSpace  = 30;
int dy0 = 20;
int baseline;
int L = 0;
//decide tile size
cv::Size textSize = getTextSize( team.name, m_param.vFontFace[L], m_param.vFontScale[L], m_param.vThickness[L], &baseline);
Point titleOrg( (m_w - textSize.width) / 2, textSize.height+50 );

int photoW = 0.75*(m_w - 2 * outSideB - colSpace - 100) / 4;
int photoH = (m_h - nRows * lineSpace - 2*(textSize.height + 50) ) / nRows;

photoW = 2 * floor(photoW / 2.0);
photoH = 2 * floor(photoH / 2.0);
photoW = (photoW > photoH) ? photoH : photoW;
photoH = photoW;
if (photoW < 0) {
xLog("ImgRender::getTeamImgs(): photoW cannot be negative, which means imag size <m_w,m_h> is set too small!");
return -1;
}

L = 1;
textSize = getTextSize("LastName, FirstName", m_param.vFontFace[L], m_param.vFontScale[L], m_param.vThickness[L], &baseline);
if ( 2*textSize.height + dy0 > photoH) {
xLog("ImgRender::getTeamImgs(): photoH is too small, messed up!");
}

cv::Rect roi;
roi.width = photoW;
roi.height = photoH;
int x1 = outSideB, x2 = m_w/2 + colSpace;
int yo = titleOrg.y+50;

//computer where to write names and photos
vector<cv::Point> vNameOri, vNumOri;
vector<cv::Rect> vPhotoRoi;
for (int i = 0; i < nPlayersPerImg; ++i) {
if (i < nRows) {
roi.x = x1;
}
else {
if (i == nRows) {
yo = titleOrg.y + 50;
}
roi.x = x2;
}
roi.y = yo;

//<p>: bottom left of the name
cv::Point p = roi.tl();
p.x += (photoW + colSpace);
p.y += (photoH / 2 - textSize.height);

vNameOri.push_back(p);
vPhotoRoi.push_back( roi );
p.y += (textSize.height + lineSpace);
vNumOri.push_back(p);

//---next player ----
yo += photoH + lineSpace;
}

vImgs.clear();

//front page
cv::Mat I(m_h, m_w, CV_8UC3, m_param.imgBgColor);
string jpgFile = team.folder + "/" + team.picFile;
cv::Mat J = imread(jpgFile);
L = 0;
if (!J.empty()) {
cv::resize(J, I, cv::Size(m_w, m_h), 0, 0, CV_INTER_LINEAR);
textSize = getTextSize(team.name, m_param.vFontFace[L], m_param.vFontScale[L], m_param.vThickness[L], &baseline);
cv::Point po((m_w - textSize.width)/2, textSize.height + 10);
cv::putText(I, team.name, po, m_param.vFontFace[L], m_param.vFontScale[L], m_param.fontColor, m_param.vThickness[L], 8);
}
else {
textSize = getTextSize(team.name, m_param.vFontFace[L], m_param.vFontScale[L], m_param.vThickness[L], &baseline);
cv::Point po( (m_w - textSize.width)/2, (m_h - textSize.height) / 2 );
cv::putText(I, team.name, po, m_param.vFontFace[L], m_param.vFontScale[L], m_param.fontColor, m_param.vThickness[L], 8);
}
vImgs.push_back(I);

if (isRenderTeamInfo) {
//create players info images
int id0 = 0;
for (int i = 0; i < nImgs; ++i) {
cv::Mat I(m_h, m_w, CV_8UC3, m_param.imgBgColor);
addPlayers(I, vPhotoRoi, vNameOri, vNumOri, team, id0);
vImgs.push_back(I);
id0 += nPlayersPerImg;
}

//create coach and management team
if (nManagers >= 1) { //the extral one for coach ans team managers if
nImgs = nImgs + 1;
cv::Mat I(m_h, m_w, CV_8UC3, m_param.imgBgColor);

textSize = getTextSize("Coaches and Support Team", m_param.vFontFace[L], m_param.vFontScale[L], m_param.vThickness[L], &baseline);
titleOrg.x = (m_w - textSize.width) / 2;
titleOrg.y = textSize.height + 50;
cv::putText(I, "Coaches and Support Team", titleOrg, m_param.vFontFace[0], m_param.vFontScale[0], m_param.fontColor, m_param.vThickness[0], 8);

//write coaches
L = 1;
Point po(0, titleOrg.y + 100);
for (int ii = 0; ii < 2; ii++) {
string s1;
std::vector<std::string> vStr;
if (ii == 0) {
s1 = "Coaches";
vStr = team.vCoach;
}
else if (ii == 1) {
s1 = "Managers";
vStr = team.vManager;
}
//else if (ii == 2) {
//	s1 = "Treasurers";
//	vStr = team.vTreasurer;
//}

textSize = getTextSize(s1, m_param.vFontFace[L], m_param.vFontScale[L], m_param.vThickness[L], &baseline);
po.x = (m_w - textSize.width) / 2;
cv::putText(I, s1, po, m_param.vFontFace[L], m_param.vFontScale[L], m_param.fontColor, m_param.vThickness[L], 8);
for (int i = 0; i < vStr.size(); i++) {
const string &s = vStr[i];
if (i % 2 == 0) {
po.y = po.y + 50;
textSize = getTextSize(s, m_param.vFontFace[L], m_param.vFontScale[L], m_param.vThickness[L], &baseline);
po.x = m_w / 2 - textSize.width - 50;
}
else {
po.x = m_w / 2 + 50;
}
cv::putText(I, s, po, m_param.vFontFace[L], m_param.vFontScale[L], m_param.fontColor, m_param.vThickness[L], 8);
}

po.y += 100;
}

vImgs.push_back(I);
}
}
return vImgs.size();
}


void ImgRender::addPlayers( cv::Mat &I, const std::vector<cv::Rect> &vPhotoRoi, const std::vector<cv::Point> &vNameOri, const std::vector<cv::Point> &vNumOri, const CfgTeam  &team, const int startId)
{
int nPlayersToShow = vNameOri.size();
int id  = startId;

int baseline;
cv::Size textSize = getTextSize("Roster", m_param.vFontFace[0], m_param.vFontScale[0], m_param.vThickness[0], &baseline);
Point titleOrg((m_w - textSize.width) / 2, textSize.height + 50);
putText(I, "Roster", titleOrg, m_param.vFontFace[0], m_param.vFontScale[0], m_param.fontColor, m_param.vThickness[0], 8);

const std::vector<Player> &vPlayer = team.vPlayer;
for (int i = 0; i < nPlayersToShow; ++i) {
if (id >= vPlayer.size()) {
break;
}

const Player &pp = vPlayer[id];
const cv::Rect &rect = vPhotoRoi[i];
string jpgFile = team.folder + "/" + pp.picFile;
cv::Mat J = imread( jpgFile );
bool hasPhoto = !J.empty();
if (hasPhoto) {
float r1 = rect.width / (float) J.cols;
float r2 = rect.height/ (float) J.rows;
r1 = ( r1 > r2 ) ? r2 : r1;
int newW = J.cols*r1;
int newH = J.rows*r1;
cv::Mat J2;
cv::resize(J, J2, cv::Size(newW, newH),0,0, CV_INTER_LINEAR);
Mat dst = I(cv::Rect(rect.x, rect.y, newW, newH));
J2.copyTo(dst);
}
else {
cv::Mat J2(rect.height, rect.width, CV_8UC3, cv::Scalar(128,128,128));
Mat dst = I( rect );
J2.copyTo(dst);
}

string name = pp.lastName + "," + pp.firstName;
string num = string("(Num ") + std::to_string(pp.num) + ")";
cv::putText(I, name, vNameOri[i], m_param.vFontFace[0], m_param.vFontScale[1], m_param.fontColor, m_param.vThickness[1], 8);
cv::putText(I, num, vNumOri[i], m_param.vFontFace[0], m_param.vFontScale[1], m_param.fontColor, m_param.vThickness[1], 8);
//------------------
id++;
}
}
