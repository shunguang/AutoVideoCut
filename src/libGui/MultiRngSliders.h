/*
*-------------------------------------------------------------------
* MultiRngSliders.h
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
#ifndef __MULTI_RNG_SLIDERS__
#define __MULTI_RNG_SLIDERS__

#include "libUtil/util.h"
#include <QtWidgets/QLabel>
#include <QtGui/QPainter>
#include <QtGui/QMouseEvent>

struct RngWorld {
RngWorld(const uint32 l=0, const uint32 u=0) : lower(l), upper(u) {}

uint32 width() const { 
return (upper - lower + 1); 
}
uint32 lower;
uint32 upper;
};

struct RngGraph {
RngGraph(const int l=0, const int u=0) : lower(l), upper(u) {}

bool  inclusiveIn(int x){
return (lower <= x) && (x <= upper);
}

bool  exclusiveIn(int x) {
return (lower < x) && (x < upper);
}

float width() const {
return (upper - lower);
}

std::string to_string( const std::string &msg="") {
char buf[32];
snprintf(buf, 32, "[%d,%d]", lower, upper);
return msg+std::string(buf);
}

int lower;
int upper;
};


struct RngSlider{
//all the units are in graphic drawing panel
RngSlider(int lowVal_=0, int upVal_=0) : lowVal(lowVal_), upVal(upVal_) {
minVal = lowVal_;
maxVal = upVal_;
}
RngSlider(int minVal_, int lowVal_, int upVal_, int maxVal_)
: minVal(minVal_), lowVal(lowVal_), upVal(upVal_), maxVal(maxVal_) {
assert( check() );
}

void setActive(bool f) {
active = f;
}

void setRange(const int minVal_, const int lowVal_, const int upVal_, const int maxVal_)
{
minVal = minVal_;
lowVal = lowVal_;
upVal  = upVal_;
maxVal = maxVal_;
}

//bool inside( const int x) {
//	return false;
//}

int range() {
return upVal - lowVal + 1;
}
bool valid() {
return (upVal - lowVal + 1 ) > 0;
}
bool check() {
return (minVal <= lowVal && lowVal <= upVal && upVal <= maxVal);
}
bool isActive() const {
return active;
}

std::string to_string( const std::string &msg="" ) {
char buf[128];
snprintf(buf, 128, "active=%d,id=%d, [%d,%d,%d,%d]", active, id, minVal, lowVal, upVal, maxVal);
return msg+std::string(buf);
}

//----------------------//
bool active;
int  id;
int lowVal, upVal;  //range values
int minVal, maxVal; //potential boundary
};
typedef std::shared_ptr<RngSlider> RngSliderPtr;


struct RngSlidersGroup {
RngSlidersGroup(int gId=0, std::string gName="",  int nMaxElements = 10) 
: id( gId), name(gName)
, vIds(nMaxElements, -1)
{
vRngSilders.clear();
for (int i = 0; i < nMaxElements; ++i) {
RngSliderPtr p(new RngSlider());
vRngSilders.push_back(p);
}
}

int						    id;			//group id
std::string				    name;		//group id
std::vector<RngSliderPtr>	vRngSilders;
std::vector<int>			vIds;		//store the idx of ordered ranges
//assert( vRngSilders[ m_vIds[i] ] in the left of vRngSilders[ m_vIds[i+1] ] )

};
typedef std::shared_ptr<RngSlidersGroup> RngSlidersGroupPtr;


class MultiRngSliders : public QLabel
{
Q_OBJECT
public:
MultiRngSliders( QWidget* aParent=0, const int nMaxElements=10);

//initial by ranges in world unit
void initGroups( const int nTotGroups );
void setCurrGroup( const int groupId );
void resetSliders( const int groupId, const std::vector<RngWorld> &vRngs, const RngWorld &boundary);
void reset();

//the following accesses the <m_currGroup>
void getRngs(std::vector<RngWorld> &v); //get back the ranges in world unit
void deActiveAll();
bool insertByCenter( const float xWorld );   //add one if it is not in
void removeCurrent();

void setEnabled(bool f) {
m_isEnabled = f;
}
virtual int setGeometry(int x, int y, int w, int h) {
QLabel::setGeometry(x, y, w, h);
m_lastRect.setTop(1);
m_lastRect.setBottom(h-2);
m_tmpRect.setTop(1);
m_tmpRect.setBottom(h-2);
return 0;
}

static RngGraph rngW2G(const RngWorld &w, float scale_w2g){
int L = round(w.lower * scale_w2g);
int U = round(w.upper * scale_w2g);
return RngGraph(L, U);
}
static RngWorld rngG2W(const RngGraph &w, float scale_w2g){
uint32 L = round(w.lower/scale_w2g);
uint32 U = round(w.upper/scale_w2g);
return RngWorld(L, U);
}

signals:
void sigWarningMsg(const QString &msg);

protected:
void mousePressEvent(QMouseEvent *event) override;
void mouseMoveEvent(QMouseEvent *event) override;
void mouseReleaseEvent(QMouseEvent *event) override;
void drawRectDuringMouseMove(const int xEnd, const bool isMouseRealsed = false);
void drawRect_xor(const RngSlider* sld, const QColor &pen);
//void paintEvent(QPaintEvent *event);
RngSlider* getSlider(const int idx);
bool findNewNeighbors(const int x, int &idL, int &idR);
bool locateExistRange(const int x, int &idx);
int findCurrSlider(const int x);
void setMouseMovingRng();
void drawInitRect();
void updateRelatedSliders();
//void fillRects(const QColor &fillingColor, const QColor &borderColor);
void resetLastRect( const int id );

public:
RngGraph    m_mouseMoveRng;
RngSlider*  m_leftSlider;        //left of <current>
RngSlider*  m_currSlider;        //curren
RngSlider*  m_rightSlider;       //right of <current>

int         m_currId;     //ID of current slider
char		m_currFlag;   //'L' or 'R' edge of current slider
bool		m_isEnabled;
float       m_scale_w2g;  //world to graphic scale
int			m_nMaxElements;
int			m_nActives;   //# of activers

RngSlidersGroupPtr				m_currGroup;
std::vector<RngSlidersGroupPtr> m_vGroups;


bool	m_isStarted;
bool	m_isMoved;
int		m_myPenWidth;
QColor	m_myPenColor;
QPoint	m_begPoint;
QPoint  m_endPoint;
QRect   m_lastRect;        //the last rect just drawed when pressing and moving mouse

bool   m_needRepait;
//std::vector<QRect>  m_vRects;

int m_gMax, m_gMin;                //boundary of all sliders, mous eonly move in [m_gMin, m_gMax]  
QRect   m_tmpRect;

const QColor            m_rectColor;
const QColor            m_currRectColor;
int						m_lastId;
};
#endif
