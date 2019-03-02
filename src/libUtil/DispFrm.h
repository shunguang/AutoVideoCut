/*
*-------------------------------------------------------------------
* DispFrm.h
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
#ifndef _DISP_FRM_H_
#define _DISP_FRM_H_
#include "DataTypes.h"
#include "CvUtilFuncs.h"
#include "UtilDefs.h"
namespace autocut {
class UTIL_EXPORT DispFrm{
public:
DispFrm(int w, int h, int type = CV_8UC3);
DispFrm(cv::Size sz, int type = CV_8UC3);
DispFrm( const DispFrm &x);
~DispFrm();
DispFrm& operator = (const DispFrm &x);

void resetSz(int w, int h);
void dump(const std::string &folderPath) const;

public:
uint64		m_fn;			//frm # in file
uint64      m_keyFrmIdx;	//key frm idx in file
uint64      m_timeStamp_ms; //time stamp in file in ms
cv::Mat		m_img;			//m x n x k (k=1,3) 8-bit image, CV_8UC1 or CV_8UC3 type image to display


};
typedef std::shared_ptr<DispFrm>		DispFrmPtr;
}  

#endif		
