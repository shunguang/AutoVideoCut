/*
*-------------------------------------------------------------------
* MultiRngSliders_tmp.cpp
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

#define MOUSE_LOCATED_ERR  5
#define MIN_BOUNDARY_SPACE  2    //mini space between two sliders 
using namespace std;

MultiRngSliders::MultiRngSliders(QWidget* aParent, const int nMaxElements)
: QLabel(aParent)
, m_nActives(0)
, m_nMaxElements(nMaxElements)
, m_vIds(nMaxElements, -1)
, m_currId(-1)
, m_currFlag('L')
, m_isEnabled(false)
, m_needRepait(true)
, m_vTmp1(nMaxElements, 0)
, m_vTmp2(nMaxElements, 0)
{
m_vRngSilders.clear();
for (int i = 0; i < m_nMaxElements; ++i) {
RngSliderPtr p(new RngSlider());
m_vRngSilders.push_back(p);
}
}

RngSlider* MultiRngSliders::getSlider(const int idx)
{
if ( idx<0 || idx > m_nMaxElements - 1) {
return 0;
}

int id = m_vIds[idx];
if ( id < 0 || !m_vRngSilders[id]->active ) {
return 0;
}
return m_vRngSilders[id].get();
}

void MultiRngSliders::deActiveAll()
{
for (int i = 0; i < m_nMaxElements; ++i) {
m_vRngSilders[i]->setActive( false );
m_vIds[i] = -1;
}
m_nActives = 0;
}

//<v> is in order
void MultiRngSliders::resetSliders(std::vector<RngWorld> &vRngs, const RngWorld &boundary)
{
deActiveAll();

int g = this->width();
m_scale_w2g = g/boundary.width();

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
RngSliderPtr &sld = m_vRngSilders[i];
sld->id = i;
sld->setActive(true);
sld->setRange(minVal, r.lower, r.upper, maxVal );

minVal = r.upper + MIN_BOUNDARY_SPACE;
m_vIds[i] = i;
}
for (int i = n; i < m_nMaxElements; ++i) {
RngSliderPtr &sld = m_vRngSilders[i];
sld->id = i;
}

m_lastRect.setTop(0);
m_lastRect.setBottom(this->height());

m_nActives = n;
drawInitRect();

}

void MultiRngSliders::getRngs(std::vector<RngWorld> &v)
{
v.clear();
BOOST_FOREACH( const RngSliderPtr &s, m_vRngSilders) {
if (s->isActive()) {
RngWorld r =  rngG2W(RngGraph(s->lowVal, s->upVal), m_scale_w2g);
v.push_back( r );
}
}
}

bool MultiRngSliders::findNewNeighbors( const int x,  int &idxL, int &idxR)
{
if (m_nActives < 1) {
return false;
}
//check if in the left of the 0th range
RngSliderPtr &sld = m_vRngSilders[m_vIds[0]];
if ( x < sld->lowVal ) {
idxL = -1;
idxR = 0;
return true;
}

//check if in the right of the last range
sld = m_vRngSilders[ m_vIds[m_nActives-1] ];
if (x > sld->upVal) {
idxL = 0;
idxR = -1;
return true;
}

if (m_nActives ==1) {
return false;
}

//check other
for (int i = 0; i < m_nActives-1; ++i) {
int idL = m_vIds[i];
int idR = m_vIds[i+1];
int leftUpper = m_vRngSilders[idL]->upVal;
int rightLower = m_vRngSilders[idR]->lowVal;
if (x > leftUpper && x < rightLower ) {
idxL = i;
idxR = i+1;
return true;
}
}
return false;
}

void MultiRngSliders::insertToBeg( const int x) 
{
assert(m_nActives == 1);
assert(m_nActives < m_nMaxElements);
RngSliderPtr &preBeg = m_vRngSilders[m_vIds[0]];
int xMin = preBeg->minVal, xMax = preBeg->lowVal - 1;
int xLow = xMin, xUp = xMax;

int n = m_nActives + 1;
for ( int i = n; i > 0; i--) {
m_vIds[i] = m_vIds[i - 1];
}

//set values to newSlider
//m_vIds[0] = n;
//RangeSliderPtr &newSld = m_vRngSilders[m_vIds[n]];

//change boundaries of the 1st slider
RngSliderPtr &lSlider = m_vRngSilders[m_vIds[0]];
RngSliderPtr &rSlider = m_vRngSilders[m_vIds[1]];
rSlider->minVal = lSlider->maxVal+1;
}

void MultiRngSliders::insertToEnd(const int x)
{
assert(m_nActives < m_nMaxElements);

//change boundaries of the 1st slider
RngSliderPtr &lSlider = m_vRngSilders[m_vIds[m_nActives-2]];
RngSliderPtr &rSlider = m_vRngSilders[m_vIds[m_nActives-1]];
lSlider->maxVal = rSlider->minVal-1;
}

void MultiRngSliders::insertToMiddle(const int x, const int idxL, const int idxR)
{
assert(m_nActives < m_nMaxElements);

//change boundaries of the 1st slider
//RangeSliderPtr &lSlider = m_vRngSilders[m_vIds[m_nActives - 2]];
//RangeSliderPtr &rSlider = m_vRngSilders[m_vIds[m_nActives - 1]];
//lSlider->SetMaximum(rSlider->GetMinimun() - 1);
}

void MultiRngSliders::insertByCenter(const int x)
{
if (m_nActives >= m_nMaxElements) {
emit sigWarningMsg(QString("Already reached maximum, cannot add!"));
return;
}

int idxL, idxR;
bool found = findNewNeighbors(x, idxL, idxR);
if (!found) {
return;
}

if (idxL == -1) {
insertToBeg(x);
}
else if (idxR == -1) {
insertToEnd(x);
}
else {
insertToMiddle(x, idxL, idxR);
}
}

bool MultiRngSliders::locateExistRange(const int x, int &idx)
{
assert(m_nActives >= 1);
//check other
for (int i = 0; i < m_nActives; ++i) {
RngSliderPtr &sld = m_vRngSilders[ m_vIds[i] ];
if (x >= sld->minVal && x <= sld->maxVal ) {
idx = i;
return true;
}
}
return false;
}

void MultiRngSliders::removeByCenter(const int x)
{
if (m_nActives <=0) {
emit sigWarningMsg(QString("Already empty, cannot delete!"));
return;
}
int idx;
bool f = locateExistRange(x,idx);   //idx in  <m_vIds>
if ( !f ) {
return;
}
int idxL = idx - 1;
int idxR = idx + 1;

RngSlider* sM = getSlider(idx);       //to be removed
RngSlider* sL = getSlider(idxL);      //idx-1
RngSlider* sR = getSlider(idxR);      //idx+1

assert( sM != 0 );

if (sL && sR) {                 //<sM> in middle, has both L and R sliders
}
else if (sL == 0 && sR == 0) {	//only <sM> no left no right slider
}
else if( sL==0) {				//<sM> is as the begging, no left slider
//remove the beging one
for (int i = 0; i < m_nActives - 1; ++i) {
m_vIds[i] = m_vIds[i + 1];
}
m_vIds[m_nActives - 1] = -1;
RngSliderPtr &newBeg = m_vRngSilders[m_vIds[0]];
newBeg->minVal = sM->minVal;
}
else{   //<sM> is at the end, no right slider
//remove the last one
RngSliderPtr &newEnd = m_vRngSilders[m_vIds[idx-1]];
newEnd->maxVal = sM->maxVal;
m_vIds[idx] = -1;
}

//now mark it as removed
sM->setActive(false);  
m_nActives--;
}

void MultiRngSliders::mousePressEvent(QMouseEvent *event)
{
if (!m_isEnabled) {
return;
}

//qDebug() << "mouse pressed:"<< event->pos().x() << "," << event->pos().y();
if (event->button() == Qt::LeftButton) {
m_begPoint = event->pos();
m_isStarted = true;
m_isMoved = false;
findCurrSlider( m_begPoint.x() );
printf( "mousePressEvent(): m_currId=%d, flag=%c, m_begPoint:(x=%d,y=%d)\n", m_currId, m_currFlag, m_begPoint.x(), m_begPoint.y());
if (m_currId >= 0) {
m_currSlider = getSlider(m_currId);
setMouseMovingRng();
m_lastRect.setLeft(m_currSlider->lowVal);
m_lastRect.setRight(m_currSlider->upVal);
}
}
}

//update <m_currId> and <m_currFlag>
void MultiRngSliders::findCurrSlider(const int x)
{
m_currId = -1;
for ( int i = 0; i < m_nActives; ++i ) {
RngSlider *s = getSlider(i);
printf( "findCurrSlider(): i=%d, x=%d, %s\n", i, x, s->to_string().c_str() );
m_vTmp1[i] = abs(s->lowVal - x);
m_vTmp2[i] = abs(s->upVal - x);
}

int idx = distance(m_vTmp1.begin(), min_element(m_vTmp1.begin(), m_vTmp1.begin()+m_nActives) );
if ( m_vTmp1[idx] < MOUSE_LOCATED_ERR ) {
m_currId = idx;
m_currFlag = 'L';
return;
}

idx = distance(m_vTmp2.begin(), min_element(m_vTmp2.begin(), m_vTmp2.begin() + m_nActives));
if (m_vTmp2[idx] < MOUSE_LOCATED_ERR) {
m_currId = idx;
m_currFlag = 'R';
return;
}
}

void MultiRngSliders::setMouseMovingRng()
{
assert(m_currId >= 0);
if (m_currFlag == 'R') {
m_mouseMoveRng.lower = m_currSlider->lowVal + 1;
m_mouseMoveRng.upper = m_currSlider->maxVal;
}
else {
if (m_currId == 0) {
m_mouseMoveRng.lower = m_currSlider->minVal;
}
else {
RngSlider*  left = getSlider(m_currId - 1);
m_mouseMoveRng.lower = left->upVal+1;
}
m_mouseMoveRng.upper = m_currSlider->upVal-1;
}
cout << m_mouseMoveRng.to_string("m_mouseMoveRng=") << endl;
}

void MultiRngSliders::mouseMoveEvent( QMouseEvent *event )
{
//qDebug() << event->pos().x() << "," << event->pos().y();
if ( !m_isEnabled || m_currId==-1) {
return;
}

if ((event->buttons() & Qt::LeftButton) && m_isStarted) {
m_endPoint = event->pos();
int dx = abs(m_endPoint.x() - m_begPoint.x());
int xEnd = m_endPoint.x();
printf("mouseMoveEvent(): m_endPoint(x=%d, y=%d), %s\n", m_endPoint.x(), m_endPoint.y(), m_mouseMoveRng.to_string("m_mouseMoveRng=").c_str());
if ( dx > 0 && m_mouseMoveRng.exclusiveIn( xEnd )) {
m_isMoved = true;
drawRect( xEnd, false);
}
else {
_beep(2000, 1000);
}
}

#if 0
if (m_currFlag == 'R') {
if (xEnd > m_currSlider->maxVal) {
xEnd = m_currSlider->maxVal;
_beep(2000, 1000);
}
else if (xEnd <= m_currSlider->lowVal) {
xEnd = m_currSlider->lowVal + 1;
_beep(2000, 1000);
}
}
else {
if (xEnd >= m_currSlider->upVal) {
xEnd = m_currSlider->upVal - 1;
_beep(2000, 1000);
}
else if (xEnd < m_currSlider->minVal) {
xEnd = m_currSlider->minVal;
_beep(2000, 1000);
}
}
#endif
}

void MultiRngSliders::mouseReleaseEvent(QMouseEvent *event)
{
//qDebug() << "mouse released:" << event->pos().x() << "," << event->pos().y();
if (!m_isEnabled || m_currId == -1) {
return;
}

#if 1		
cout << m_currSlider->to_string("AAA-mouseReleaseEvent(): curr=") << endl;
RngSlider *left = getSlider(m_currId - 1);
if (left) {
cout << left->to_string("AAA-mouseReleaseEvent(): left=") << endl;
}
RngSlider *right = getSlider(m_currId + 1);
if (right) {
cout << right->to_string("AAA-mouseReleaseEvent(): right=") << endl;
}
#endif

if (event->button() == Qt::LeftButton && m_isStarted) {
m_endPoint = event->pos();
int xEnd = m_endPoint.x();
int dx = abs(xEnd - m_begPoint.x());
printf("mouseReleaseEvent(): m_begPoint(x=%d,y=%d), m_endPoint(x=%d,y=%d), dx=%d, xEnd=%d\n", m_begPoint.x(), m_begPoint.y(), m_endPoint.x(), m_endPoint.y(), dx);
if (dx > 0 && m_mouseMoveRng.exclusiveIn(xEnd)) {
#if 0
if (m_currFlag == 'R') {
if (xEnd > m_currSlider->maxVal) {
xEnd = m_currSlider->maxVal;
}
else if (xEnd <= m_currSlider->lowVal) {
xEnd = m_currSlider->lowVal + 1;
}
}
else {
if (xEnd >= m_currSlider->upVal) {
xEnd = m_currSlider->upVal - 1;
}
else if (xEnd < m_currSlider->minVal) {
xEnd = m_currSlider->minVal;
}
}
if (xEnd < m_gMin) xEnd = m_gMin;
if (xEnd > m_gMax) xEnd = m_gMax;
#endif
//drawRect(xEnd, true);
updateRelatedSliders(xEnd);
}
m_isStarted = false;
#if 1		
cout << m_currSlider->to_string("BBB--mouseReleaseEvent(): curr=") << endl;
RngSlider *left = getSlider(m_currId - 1);
if (left) {
cout << left->to_string("BBB--mouseReleaseEvent(): left=") << endl;
}
RngSlider *right = getSlider(m_currId + 1);
if (right) {
cout << right->to_string("BBB--mouseReleaseEvent(): right=") << endl;
}
#endif
}
}

void MultiRngSliders::updateRelatedSliders(const int xEnd)
{
//QPainter painter(&image);
if (m_currFlag == 'L') {
m_currSlider->lowVal= xEnd;
RngSlider*  left = getSlider(m_currId-1);
if (left) {
left->maxVal = xEnd - 1;
}
}
else {
m_currSlider->upVal = xEnd;
RngSlider*  right = getSlider(m_currId + 1);
if (right) {
right->minVal = xEnd + 1;
}
}
}

void MultiRngSliders::drawRect(const int xEnd, const bool isMouseRealsed)
{
int xL, xR;
//QPainter painter(&image);
if (m_currFlag == 'L') {
xL = xEnd;
xR = m_currSlider->upVal;
}
else {
xL = m_currSlider->lowVal;
xR = xEnd;
}

cout<<m_currSlider->to_string("m_currSlider=")<<endl;
printf("drawRect(): m_lastRect1=(%d,%d,%d,%d), xL=%d,xR=%d\n", m_lastRect.x(), m_lastRect.y(), m_lastRect.width(), m_lastRect.height(), xL,xR);

QPixmap *p = (QPixmap *)this->pixmap();
if (p) {
QPainter painter(p);
painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
painter.setRenderHint(QPainter::Antialiasing);
painter.setPen(QColor(0xff, 0xff, 0xff));
//erase the old one
if (m_lastRect.width() > 0) {
painter.drawRect(m_lastRect);
}

//update <m_lastRect>
m_lastRect.setLeft(xL);
m_lastRect.setRight(xR);

//draw it
printf("drawRect(): m_lastRect2=(%d,%d,%d,%d)\n", m_lastRect.x(), m_lastRect.y(), m_lastRect.width(), m_lastRect.height());
painter.drawRect(m_lastRect);
update();
}
}



void MultiRngSliders::drawInitRect()
{
printf("%d,%d\n", this->width(), this->height());
QPixmap *p = (QPixmap *)this->pixmap();
if (p) {
QPainter painter(p);
painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
painter.setRenderHint(QPainter::Antialiasing);
painter.setPen(QColor(0xff, 0xff, 0xff));
for (int i = 0; i < m_nActives; ++i) {
RngSliderPtr &sld = m_vRngSilders[i];
m_lastRect.setLeft(sld->lowVal);
m_lastRect.setRight(sld->upVal);

//draw it
printf("m_lastRect=(%d,%d,%d,%d)\n", m_lastRect.x(), m_lastRect.y(), m_lastRect.width(), m_lastRect.height());
painter.drawRect(m_lastRect);
}
update();
}
}
