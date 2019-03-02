/*
*-------------------------------------------------------------------
* ImgRender.h
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

#ifndef __IMG_RENDER_H__
#define __IMG_RENDER_H__

#include "libUtil/DataTypes.h"
#include "libUtil/visLog.h"
#include "libUtil/CfgAc.h"

namespace autocut {
class ImgRender {
public:
ImgRender( const int w=640, const int h=480, const CfgRenderImg  &param= CfgRenderImg() );
~ImgRender();

//ImgRender(const ImgRender &x);
//ImgRender& operator = (const ImgRender &x);

cv::Mat getImg( const std::string &s );
cv::Mat getImg( const std::vector <std::string> &v);
cv::Mat getGameImg(const CfgGame &game );
int getTeamImgs(std::vector<cv::Mat> &vImgs, const CfgTeam &team, const bool isRenderTeamInfo);

private:
void addPlayers(cv::Mat &I, const std::vector<cv::Rect> &vPhotoRoi, const std::vector<cv::Point> &vNameOri, const std::vector<cv::Point> &vNumOri, const CfgTeam  &team, const int startId);
public:
int				m_w;
int				m_h;
CfgRenderImg	m_param;
};
typedef std::shared_ptr<ImgRender> ImgRenderPtr;
}
#endif
