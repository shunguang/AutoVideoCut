/*
*-------------------------------------------------------------------
* MultiRngSliders.cpp
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
#include "MultiRngSliders.h"

#define MOUSE_LOCATED_ERR  10
#define MIN_BOUNDARY_SPACE  2    //mini space between two sliders 
using namespace std;

MultiRngSliders::MultiRngSliders(QWidget* aParent, const int nMaxElements)
: QLabel(aParent)
, m_nActives(0)
, m_nMaxElements(nMaxElements)
, m_currId(-1)
, m_currFlag('L')
, m_isEnabled(false)
, m_needRepait(true)
, m_rectColor(QColor(0xff, 0xff, 0xff))
, m_currRectColor( QColor(Qt::green).rgb() )
, m_lastId(0)
, m_currGroup(0)
{
}

void MultiRngSliders::reset()
{
deActiveAll();
//todo:
}


void MultiRngSliders::initGroups(const int nTotGroups)
{
m_vGroups.clear();
for (int i = 0; i < nTotGroups; ++i) {
RngSlidersGroupPtr p( new RngSlidersGroup(i, "name_"+ std::to_string(i), m_nMaxElements));
m_vGroups.push_back(p);
}
setCurrGroup(0);
}

void MultiRngSliders::setCurrGroup(const int groupId)
{
if (groupId >= 0 && groupId < m_vGroups.size()) {
m_currGroup = m_vGroups[groupId];
}
}


RngSlider* MultiRngSliders::getSlider(const int idx)
{
if ( idx<0 || idx > m_nMaxElements - 1) {
return 0;
}

int id = m_currGroup->vIds[idx];
if ( id < 0 || !m_currGroup->vRngSilders[id]->active ) {
return 0;
}
return m_currGroup->vRngSilders[id].get();
}

void MultiRngSliders::deActiveAll()
{
if (m_currGroup == 0) {
return;
}

for (int i = 0; i < m_nMaxElements; ++i) {
m_currGroup->vRngSilders[i]->setActive( false );
m_currGroup->vIds[i] = -1;
}
m_nActives = 0;
}

//<v> is in order
void MultiRngSliders::resetSliders(const int groupId, const std::vector<RngWorld> &vRngs, const RngWorld &boundary)
{
setCurrGroup(groupId);
if (m_currGroup == 0) {
return;
}

deActiveAll();

int g = this->width();
m_scale_w2g = (double)g/(double)boundary.width();

int n = vRngs.size();
if ( n > m_nMaxElements) {
n = m_nMaxElements;
}
//graph bundary
m_gMax = m_scale_w2g* boundary.upper-1;
m_gMin = m_scale_w2g* boundary.lower+1;

//[minVal, maxVal] boundaried for each range
int minVal = m_gMin;
int maxVal;
for (int i = 0; i < n; ++i) {
if (i == n - 1) { maxVal = m_gMax; }
else { maxVal = (m_scale_w2g*vRngs[i + 1].lower) - MIN_BOUNDARY_SPACE; }

RngGraph r = rngW2G(vRngs[i], m_scale_w2g);
RngSliderPtr &sld = m_currGroup->vRngSilders[i];
sld->id = i;
sld->setActive(true);
sld->setRange(minVal, r.lower, r.upper, maxVal );

minVal = r.upper + MIN_BOUNDARY_SPACE;
m_currGroup->vIds[i] = i;

cout << sld->to_string("init slider") << endl;
}
for (int i = n; i < m_nMaxElements; ++i) {
RngSliderPtr &sld = m_currGroup->vRngSilders[i];
sld->id = i;
}
//todo: set current id always the closest one to the value of keyfrmIdx slider 
m_nActives = n;
m_currId = n - 1;
m_currSlider = getSlider(m_currId);
m_leftSlider = getSlider(m_currId - 1);
m_rightSlider = getSlider(m_currId + 1);

drawInitRect();
resetLastRect(0);
}

void MultiRngSliders::resetLastRect(const int id)
{
if ( id<0 || id>m_nActives-1 ) {
return;
}

m_lastId = id;
RngSlider *s = getSlider(id);
m_lastRect.setLeft( s->lowVal);
m_lastRect.setRight( s->upVal);
}

void MultiRngSliders::getRngs(std::vector<RngWorld> &v)
{
v.clear();
BOOST_FOREACH( const RngSliderPtr &s, m_currGroup->vRngSilders) {
if (s->isActive()) {
RngWorld r =  rngG2W(RngGraph(s->lowVal, s->upVal), m_scale_w2g);
v.push_back( r );
}
}
}

bool MultiRngSliders::findNewNeighbors( const int x,  int &idxL, int &idxR)
{
if (m_nActives < 1) { //empty slider
idxL = -1; idxR = -1;
return true;
}

//check if in the left of the 0th range
RngSlider *sld = getSlider(0);
if ( x < sld->lowVal ) {
idxL = -1; idxR = 0;
return true;
}

//check if in the right of the last range
sld = getSlider(m_nActives-1);
if (x > sld->upVal) {
idxL = m_nActives - 1; idxR = -1;
return true;
}
//check other
for (int i = 0; i < m_nActives-1; ++i) {
RngSlider *sL = getSlider(i);
RngSlider *sR = getSlider(i+1);
if (x > sL->upVal && x <sR->lowVal) {
idxL = i;idxR = i+1;
return true;
}
}
return false;
}

bool MultiRngSliders::insertByCenter(const float xWorld)
{
if (m_nActives >= m_nMaxElements) {
emit sigWarningMsg(QString("Already reached maximum, cannot add anymore!"));
return false;
}

int x = round(m_scale_w2g * xWorld);
int idx;
if (locateExistRange(x, idx)) {
RngSlider *s = getSlider(idx);
emit sigWarningMsg(QStringLiteral("Already in range [%1,%2]!").arg(s->lowVal).arg(s->upVal) ) ;
return false;
}


int idxL, idxR;
bool found = findNewNeighbors(x, idxL, idxR);
if (!found) {
//this should never be reached
return false;
}

//find available slot: since we allocate <m_nMaxElements> # of RngSliders at begining, we are not "new" a RngSlider at there, just find its availiable slot 
RngSlider *newRngSlider = 0;
BOOST_FOREACH( RngSliderPtr &s, m_currGroup->vRngSilders ){
if (!s->isActive()) {
newRngSlider = s.get();
break;
}
}
#if _DEBUG
assert(newRngSlider);
#endif
RngSlider *sL = getSlider(idxL);
RngSlider *sR = getSlider(idxR);
int iBeg = m_nActives, iEnd = -1, iInsert = -1;
int minV, lowV, upV, maxV;
if ( sL==0  && sR == 0) { //insert to an empty sliders 
assert(m_nActives == 0);
int w = this->width();
minV = 0; maxV = w;
iEnd = 0; iInsert = 0;
}
else if ( sL==0 ) {  //no left, insert at beging
iEnd = 0; iInsert = 0;
minV = sR->minVal; 	maxV = sR->lowVal- MIN_BOUNDARY_SPACE;
}
else if ( sR == 0) { //no right, insert at end
iEnd = m_nActives; iInsert = m_nActives;
minV = sL->upVal+MIN_BOUNDARY_SPACE; 	maxV = sL->maxVal;
}
else { //insert in middle
iEnd = idxR; iInsert = idxR;
minV = sL->upVal + MIN_BOUNDARY_SPACE; 	maxV = sR->lowVal- MIN_BOUNDARY_SPACE;
}

if (maxV - minV < 3) {
emit sigWarningMsg(QString("do not have enough space to insert!"));
return false;
}

int d = (maxV - minV) / 3;
lowV = minV + d;	upV = maxV - d;

//now do real change
std::vector<int> &vIds = m_currGroup->vIds;
for (int i = iBeg; i > iEnd; i--) {
vIds[i] = vIds[i - 1];
}
vIds[iInsert] = newRngSlider->id;
newRngSlider->setRange(minV, lowV, upV, maxV);
newRngSlider->setActive(true);
m_nActives++;

//update neiborhood max or min
if (sL) {
sL->maxVal = lowV - MIN_BOUNDARY_SPACE;
}
if(sR){
sR->minVal = upV + MIN_BOUNDARY_SPACE;
}

//draw the new insered and set it as <current slider
if (m_currId >= 0) {
//previous <m_currSlider> exist, recover it to <m_rectColor>
drawRect_xor(m_currSlider, m_currRectColor);
drawRect_xor(m_currSlider, m_rectColor);
}

m_currId = iInsert;
m_currSlider = getSlider(m_currId);;
m_leftSlider = getSlider(m_currId - 1);
m_rightSlider = getSlider(m_currId + 1);

drawRect_xor(m_currSlider,m_currRectColor );

return true;
}

bool MultiRngSliders::locateExistRange(const int x, int &idx)
{
idx = -1;
for (int i = 0; i < m_nActives; ++i) {
RngSlider* sld = getSlider(i);
if (x >= sld->lowVal && x <= sld->upVal ) {
idx = i;
return true;
}
}
return false;
}

void MultiRngSliders::removeCurrent()
{
if (m_nActives <= 0 || m_currId<0 ) {
emit sigWarningMsg(QString("Already empty, cannot delete!"));
return;
}
int newCurrId = -1;                      //nex id after removing current one
int idx = m_currId;
RngSlider* sM = getSlider(idx);			 //to be removed
RngSlider* sL = getSlider(idx - 1);      //idx-1
RngSlider* sR = getSlider(idx + 1);      //idx+1

//2. change the data
assert( sM != 0 );
std::vector<int> &vIds = m_currGroup->vIds;
for (int i = idx; i < m_nActives - 1; ++i) {
vIds[i] = vIds[i + 1];
}
vIds[m_nActives - 1] = -1;

if (sL && sR ) {
newCurrId = idx;  
sL->maxVal = sM->maxVal;
sR->minVal = sM->minVal;
}
else if (sL) {  //no right, the deleted one is the last one
sL->maxVal = sM->maxVal;
newCurrId = m_nActives - 2;
}
else if (sR) { //no left, the deleted one is the beging
sR->minVal = sM->minVal;
newCurrId = 0;
}

//3. change the graphics
drawRect_xor(m_currSlider, m_currRectColor);

m_currId = newCurrId;
m_currSlider = getSlider(m_currId);
m_leftSlider = getSlider(m_currId - 1);
m_rightSlider = getSlider(m_currId + 1);

drawRect_xor(m_currSlider, m_rectColor);
drawRect_xor(m_currSlider, m_currRectColor);

//4. now mark it as removed
sM->setActive(false);
m_nActives--;
}


//update <m_currFlag>
//return the index used to update <m_currId>
int MultiRngSliders::findCurrSlider(const int x)
{
if (m_nActives < 1) {
return -1;
}
int idx = -1;
int d, dMin = INT_MAX;
for ( int i = 0; i < m_nActives; ++i ) {
RngSlider *s = getSlider(i);
d = std::min<int>(abs(s->lowVal - x), abs(s->upVal - x));
if (d < dMin) {
dMin = d;
idx = i;
}
printf("findCurrSlider(): i=%d, x=%d, %s, d=%d\n", i, x, s->to_string().c_str(),d);
}

RngSlider *s = getSlider(idx);
m_currFlag = 'R';
if (abs(s->lowVal - x) < abs(s->upVal - x)) {
m_currFlag = 'L';
}
return idx;
}

void MultiRngSliders::setMouseMovingRng()
{
assert(m_currId >= 0);
m_mouseMoveRng.lower = m_currSlider->minVal;
m_mouseMoveRng.upper = m_currSlider->maxVal;
cout << m_mouseMoveRng.to_string("m_mouseMoveRng=") << endl;
}

void MultiRngSliders::mousePressEvent(QMouseEvent *event)
{
if (!m_isEnabled || m_nActives < 1) {
return;
}

//qDebug() << "mouse pressed:"<< event->pos().x() << "," << event->pos().y();
bool currChged = false;
if (event->button() == Qt::LeftButton) {
m_begPoint = event->pos();
m_isStarted = true;
m_isMoved = false;
int newId = findCurrSlider(m_begPoint.x());
printf("====mousePressEvent(): m_currId=%d, newId=%d, flag=%c, m_begPoint:(x=%d,y=%d)=====\n", m_currId, newId,  m_currFlag, m_begPoint.x(), m_begPoint.y());
if ( newId >= 0) {
//before update <m_currSlider>
if (m_currId != newId) {
//draw current with <m_currRectColor>
drawRect_xor(m_currSlider, m_currRectColor);
//draw current with <m_rectColor>
drawRect_xor(m_currSlider, m_rectColor);
currChged = true;
}
m_currId = newId;
m_currSlider = getSlider(newId);
m_leftSlider = getSlider(newId-1);
m_rightSlider = getSlider(newId+1);
if (m_leftSlider) { cout << m_leftSlider->to_string("m_leftSlider") << endl; }
if (m_currSlider) { cout << m_currSlider->to_string("m_currSlider") << endl; }
if (m_rightSlider) { cout << m_rightSlider->to_string("m_rightSlider") << endl; }

setMouseMovingRng();

if (currChged) {
//draw current with <m_rectColor>
drawRect_xor(m_currSlider, m_rectColor); 

//draw current with <m_currRectColor>
drawRect_xor(m_currSlider, m_currRectColor);
}

m_lastRect.setLeft(m_currSlider->lowVal);
m_lastRect.setRight(m_currSlider->upVal);
}
}
}

void MultiRngSliders::mouseMoveEvent( QMouseEvent *event )
{
//qDebug() << event->pos().x() << "," << event->pos().y();
if ( !m_isEnabled || m_currId==-1 || m_nActives < 1) {
return;
}
if ((event->buttons() & Qt::LeftButton) && m_isStarted) {
m_endPoint = event->pos();
int dx = abs(m_endPoint.x() - m_begPoint.x());
const int &xEnd = m_endPoint.x();
//printf("mouseMoveEvent(): m_endPoint(x=%d, y=%d), %s\n", m_endPoint.x(), m_endPoint.y(), m_mouseMoveRng.to_string("m_mouseMoveRng=").c_str());
if ( dx > 0 && m_mouseMoveRng.inclusiveIn( xEnd )) {
m_isMoved = true;
drawRectDuringMouseMove( xEnd, false);
}
else {
_beep(2000, 1000);
}
}
}

void MultiRngSliders::mouseReleaseEvent(QMouseEvent *event)
{
//qDebug() << "mouse released:" << event->pos().x() << "," << event->pos().y();
if (!m_isEnabled || m_currId == -1 || m_nActives < 1) {
return;
}

#if 0		
if (m_leftSlider) { cout << m_leftSlider->to_string("AAA-mouseReleaseEvent(): left") << endl; }
if (m_currSlider) { cout << m_currSlider->to_string("AAA-mouseReleaseEvent(): curr") << endl; }
if (m_rightSlider) { cout << m_rightSlider->to_string("AAA-mouseReleaseEvent(): right") << endl; }
#endif

if (event->button() == Qt::LeftButton && m_isStarted) {
m_endPoint = event->pos();
int xEnd = m_endPoint.x();
int dx = abs(xEnd - m_begPoint.x());
printf("mouseReleaseEvent(): m_begPoint(x=%d,y=%d), m_endPoint(x=%d,y=%d), dx=%d\n", m_begPoint.x(), m_begPoint.y(), m_endPoint.x(), m_endPoint.y(), dx);
if (dx > 0 && m_mouseMoveRng.inclusiveIn(xEnd)) {
drawRectDuringMouseMove(xEnd, true);
}
updateRelatedSliders();
m_isStarted = false;
#if 0		
if (m_leftSlider) { cout << m_leftSlider->to_string("BBB-mouseReleaseEvent(): left") << endl; }
if (m_currSlider) { cout << m_currSlider->to_string("BBB-mouseReleaseEvent(): curr") << endl; }
if (m_rightSlider) { cout << m_rightSlider->to_string("BBB-mouseReleaseEvent(): right") << endl; }
#endif
}
}

void MultiRngSliders::updateRelatedSliders()
{
int xL = m_lastRect.left();
int xR = m_lastRect.right();
#if _DEBUG
assert(  xL < xR );
#endif
m_currSlider->lowVal = xL;
m_currSlider->upVal = xR;

if (m_leftSlider) {
m_leftSlider->maxVal = xL - MIN_BOUNDARY_SPACE;
}
if (m_rightSlider) {
m_rightSlider->minVal = xR + MIN_BOUNDARY_SPACE;
}
}

//todo: remove <painter> to class varaibale, and only initialed once
void MultiRngSliders::drawRect_xor(const RngSlider* sld, const QColor &pen)
{
QPixmap *p = (QPixmap *)this->pixmap();
if (p == 0 || sld == 0) {
return;
}
m_tmpRect.setLeft(sld->lowVal);
m_tmpRect.setRight(sld->upVal);

QPainter painter(p);
painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
painter.setRenderHint(QPainter::Antialiasing);
painter.setPen(pen);
painter.drawRect(m_tmpRect);
update();
}

void MultiRngSliders::drawRectDuringMouseMove(const int xEnd, const bool isMouseRealsed)
{
int xL, xR;
if (m_currFlag == 'L') {
xL = xEnd;
xR = m_currSlider->upVal;  //original right edge
}
else {
xL = m_currSlider->lowVal; ////original left edge
xR = xEnd;
}
if (abs(xR - xL) < 1) {
return;
}
//cout<<m_currSlider->to_string("drawRect(): -- m_currSlider=")<<endl;
//printf(" m_lastRect1=(L=%d,R=%d), xL=%d,xR=%d\n", m_lastRect.left(),  m_lastRect.right(), xL,xR);

QPixmap *p = (QPixmap *)this->pixmap();
if (p) {
QPainter painter(p);
painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
painter.setRenderHint(QPainter::Antialiasing);
painter.setPen(m_currRectColor);
//erase the old one
if (m_lastRect.width() > 0) {
painter.drawRect(m_lastRect);
}

//update <m_lastRect>
if (xL < xR) {
m_lastRect.setLeft(xL);
m_lastRect.setRight(xR);
}
else {
m_lastRect.setLeft(xR);
m_lastRect.setRight(xL);
}
//draw it
painter.drawRect(m_lastRect);
update();
}
//printf("drawRect(): m_lastRect=(L=%d,R=%d)-----end!\n", m_lastRect.left(), m_lastRect.right());
}

void MultiRngSliders::drawInitRect()
{
//printf("%d,%d\n", this->width(), this->height());
QPixmap *p = (QPixmap *)this->pixmap();
if (p) {
QPainter painter(p);
painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
painter.setRenderHint(QPainter::Antialiasing);
for (int i = 0; i < m_nActives; ++i) {
RngSlider *sld = getSlider(i);
if (sld->id == m_currId) {
painter.setPen(m_currRectColor);
}
else {
painter.setPen(m_rectColor);
}
m_tmpRect.setLeft(sld->lowVal);
m_tmpRect.setRight(sld->upVal);

//draw it
//printf("m_lastRect=(L=%d,R=%d)\n", m_lastRect.left(), m_lastRect.right());
painter.drawRect(m_tmpRect);
}
update();
}
}

#if 0
void MultiRngSliders::fillRects( const QColor &fillingColor, const QColor &borderColor)
{
printf("%d,%d\n", this->width(), this->height());
QPixmap *p = (QPixmap *)this->pixmap();
if (p) {
QPainter painter(p);
QPainterPath path;
QPen pen(borderColor, 10);

painter.setRenderHint(QPainter::Antialiasing);
painter.setPen(pen);
QRect rect(0, 0, 100, this->height());
for (int i = 0; i < m_nActives; ++i) {
RngSlider *sld = getSlider(i);
if (sld == 0) continue;

rect.setLeft(sld->lowVal);
rect.setRight(sld->upVal);
//path.addRoundedRect( rect, 10, 10);
//painter.fillPath(path, fillingColor);
//path.addRoundedRect( rect, 10, 10);
painter.fillRect(rect, fillingColor);
//painter.drawPath(path);
}
update();
}
}


//A paint event is a request to repaint all or part of a widget.It can happen for one of the following reasons :
//repaint() or update() was invoked,
void MultiRngSliders::paintEvent(QPaintEvent *event)
{
//if (!m_needRepait) {
//	return;
//}
QPixmap *p = (QPixmap *)this->pixmap();
if (p == 0) {
return;
}

QPainter painter(p);
//painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
painter.setRenderHint(QPainter::Antialiasing);
painter.setPen(QColor(0xff, 0xff, 0xff));
BOOST_FOREACH(const QRect &r, m_vRects) {
printf("Rect=(%d,%d,%d,%d)\n", r.x(), r.y(), r.width(), r.height());
painter.drawRect(r);
}
//m_needRepait = false;
}
#endif
