/*
*-------------------------------------------------------------------
* swLine2D.cpp
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

//In this calss, a 2D line is defined by three parameters, i.e., a*x + b*y + c=0.

#include "swLine2D.h"

swLine2D :: swLine2D():
m_a( 0.0f ),
m_b( 0.0f ), 
m_c( 0.0f ),
m_begPt(1,2),
m_endPt(1,2),
m_isLineSegment ( false )
{
}

swLine2D :: swLine2D( const float a, const float b, const float c ):
m_a( a ),
m_b( b ), 
m_c( c ),
m_begPt(1,2),
m_endPt(1,2),
m_isLineSegment ( false )
{
}

/*
%----------------------------
%input
% p1, 1 x 2, pt1
% p2, 1 x 2, pt2
%output
% a*x + b*y + c = 0
%----------------------------
*/
swLine2D :: swLine2D( const swfMat &p1, const swfMat &p2 ):
m_a( 0.0f ),
m_b( 0.0f ), 
m_c( 0.0f ),
m_begPt(1,2),
m_endPt(1,2),
m_isLineSegment ( false )
{
resetLine( p1, p2 );
}

/*
%----------------------------
%input
% p, 1 x 2, pt1
% k, 1 x 1, the slope
%----------------------------
*/
swLine2D :: swLine2D( const swfMat &p, const float slope ):
m_a( 0.0f ),
m_b( 0.0f ), 
m_c( 0.0f ),
m_begPt(1,2),
m_endPt(1,2),
m_isLineSegment ( false )
{
resetLine( p, slope );
}


float swLine2D :: getDist( const swfMat &p ) const
{
float tmp = sqrt ( m_a*m_a + m_b*m_b );

if ( tmp > SWF_MAT_EPS ){
return fabs( m_a * p(0) + m_b * p(1) + m_c ) / tmp;
}
else{
errMsg( "swLine2D :: getDist(): sth wrong about the line");
return SWF_MAT_REALMAX ;
}
}

/*
%----------------------------
%get the intersection pt between this line and and the given line $L$
% line1: a1x+b1y+c1 = 0
% line2: a2x+b2y+c2 = 0
%output
% xy, 1 x 2, the intersection pt
% isExist, 1 x 1, if exist flag
%----------------------------
*/
bool swLine2D :: getIntersectionPoint( const swLine2D &L, swfMat &intsectionPt) const
{
float a2 = L.getA();
float b2 = L.getB();
float c2 = L.getC();

float e = m_a*b2 - a2*m_b;

if ( fabs(e) < SWF_MAT_EPS ){
intsectionPt(0) = SWF_MAT_REALMAX ;
intsectionPt(1) = SWF_MAT_REALMAX ;
return false;
}

intsectionPt(0) = ( c2*m_b - m_c*b2 ) / e ;
intsectionPt(1) = ( a2*m_c - m_a*c2 ) / e ;

return true;
}

//given y on the line, find x
float swLine2D :: getX( const float y) const
{
if ( isParallelAxisX() ) //the line is parallel to x-axis, x can be any value.
return  SWF_MAT_REALMAX ;
else
return  - ( m_b*y + m_c ) / m_a ;
}

//given x on the line, find y
float swLine2D :: getY( const float x) const
{
if ( isParallelAxisY() ) //the line is parallel to x-y, y can be any value.
return  SWF_MAT_REALMAX ;
else
return  - ( m_a*x + m_c ) / m_b ;
}


bool swLine2D :: isParallelAxisX() const
{
return ( fabs(m_a) <= SWF_MAT_EPS );
}

bool swLine2D :: isParallelAxisY() const
{
return ( fabs(m_b) <= SWF_MAT_EPS );
}


bool swLine2D :: isCloseAxisX() const
{
if ( isParallelAxisY() ){
return false;
}
else{
float slop = -m_a/m_b;
if ( slop >= -1.0f && slop<=1.0f ){
return true;
}
else{
return false;
}
}
}

//given the angle (in rad.) between line and x-axis	
float swLine2D :: getSlopeAng() const				 
{
if ( isParallelAxisY() ){
return (float)SW_MAT_HALF_PI;
}
else{
return atan( -m_a/m_b );
}
}

bool swLine2D :: isPointInsideLineSegment( swfMat &p) const
{
if ( !m_isLineSegment ){
printf("Warning!!! at swLine2D :: isPointInsideLineSegment(): this is not a line segment! \n" ); 
return false;
}

float d12 = dist2D( m_begPt, m_endPt); 
float d1 = dist2D( p, m_begPt ); 
float d2 = dist2D( p, m_endPt ); 

if ( fabs( d12 - (d1 + d2) ) < SWF_MAT_EPS )
return true;
else
return false;
}

/*
%-----------------------------
%input
%   givenPt, 1 x 2, the coordinates of the pt
%output
%   intersectionPt, 1 x 2, the closet pt from $givenPt$ to the points on the line between
%   [m_begPt, m_endPt]
%-----------------------------
*/
float swLine2D :: getMinDistFromPt2LineSegment( const swfMat &givenPt, swfMat &intersectionPt) const
{
//makesure p1 is at the left and p2 is at the right
swfMat p1(1,2), p2(1,2);
if ( m_begPt(0) > m_endPt(0) ){
p1= m_endPt;
p2= m_begPt;
}
else{
p1= m_begPt;
p2= m_endPt;
}

//find the interscetion pts between line p1p2 (ax + bz + c = 0) and its perpedicular line
//which passes $pt$ 
swLine2D L2;
if ( isParallelAxisX() ){
swfMat synPt( givenPt );
synPt(1) += 1.0;
L2.resetLine ( givenPt, synPt );
}
else {
float slope = m_b/m_a;  //the slope of the line  who is perpendicular to $p1  p2$
L2.resetLine( givenPt, slope );
}

bool isExist = this -> getIntersectionPoint( L2, intersectionPt);
assert( isExist );  //$isExist$ should always be $true$ at this point

bool isInside =  this -> isPointInsideLineSegment( intersectionPt ); 

if ( isInside )
return dist2D( givenPt, intersectionPt );

//cal. the distance from $pt$ to the two end points
float d1 = dist2D(givenPt, p1);
float d2 = dist2D(givenPt, p2);

if ( d1 < d2 ){
intersectionPt = p1;
return d1;
}
else{
intersectionPt = p2;
return  d2;
}
}

swfMat swLine2D :: getPerpendicularProjPt( const swfMat &givenPt ) const
{
swfMat projPtOnLine(1, 2, 0.0);

if ( isParallelAxisX () ) {
projPtOnLine(0) = givenPt( 0 );
projPtOnLine(1) = getY( givenPt(0) );
}
else if ( isParallelAxisY() ) {
projPtOnLine(0) = getX( givenPt(1) );
projPtOnLine(1) = givenPt( 1 );
}
else{
swLine2D L1( givenPt, m_b/m_a );
bool isIntersetion = getIntersectionPoint( L1,  projPtOnLine );
if ( !isIntersetion ){
errMsg ( "swLine2D :: getPerpendicularProjPt(): sth is wrong!" );
}
}

return  projPtOnLine;

}

//p1, 1 x 2;
//p2, 1 x 2;
float swLine2D ::  dist2D( const swfMat &p1, const swfMat &p2) const
{
float dx = p1(0) - p2(0);
float dy = p1(1) - p2(1);

return  sqrt( dx*dx + dy * dy);
}

void swLine2D :: errMsg ( const char* msgStr) const
{
//debug version
printf("Error!!! %s\n", msgStr ); 
}


void swLine2D :: resetLine( const swfMat &p1, const swfMat &p2 )
{
float d;

d = dist2D (p1, p2);
if ( d < SWF_MAT_EPS ){  //the line is parallel to y-axis
errMsg( "swLine2D :: resetLine():  two points are too closer!");
m_isLineSegment = false;
return;
}


d = p2(0) - p1(0);
if ( fabs(d) < SWF_MAT_EPS ){  //the line is parallel to y-axis
m_a = 1.0;
m_b = 0.0;
m_c = -p1(0);
}
else{
m_a  = ( p2(1) - p1(1) ) / d ;
m_b = -1.0;
m_c = p1(1) - m_a*p1(0);
}

m_isLineSegment = true;
m_begPt = p1;
m_endPt = p2;
}


void swLine2D :: resetLine( const swfMat &p, const float slope )
{
m_a = slope;
m_b = -1.0;
m_c = p(1)-slope*p(0);

m_isLineSegment = false;
}

//-------------------------------------------------------------
//input
//sampling n equal-distance pts from the center of (x0,y0) on the line
//(x0,y0): given point on the line
//distL:   the distance between two sampling pts
//n:       total # of sampleing pts
//
//output
//tab:     2 x n, return points table
//-------------------------------------------------------------
void swLine2D :: getSamplePtsAloneLine( const float x0, const float y0, 
const float distL, const int n, swfMat &tab )
{
//const float c1 = atan(1.0f);
int m = n/2;
bool isCloseX = isCloseAxisX();
float theta = getSlopeAng();
if( isCloseX ){
//assert( theta >= -c1 && theta <= c1 );
float step = distL * cos(theta);  
float x = x0-m*step;
for(int i=0; i<n; i++){
tab(0, i) = x;
tab(1, i) = getY(x);
x += step;
}
}
else{
//assert( theta < -c1 || theta > c1 );
float step = fabs( distL * sin(theta) );
float y = y0-m*step;
for(int i=0; i<n; i++){
tab(0, i) = getX(y);
tab(1, i) = y;
y += step; 
}
}
}

//-------------------------------------------------------------
//input
//sampling n equal-distance pts start from (x0,y0), and along particular direction
//(x0,y0):		given point on the line
//dirAngInRad:  theta or theta + pi, where tan(theta)=slope of the line
//distL:		the distance between two sampling pts
//n:			total # of sampleing pts
//
//output
//tab:     2 x n, return points table
//-------------------------------------------------------------
void swLine2D :: getSamplePtsAloneLine( const float x0, const float y0, const float dirAngInRad, 
const float distL, const int n, swfMat &tab )
{
//const float c1 = atan(1.0f);
bool isCloseX = isCloseAxisX();
if( isCloseX ){
//assert( theta >= -c1 && theta <= c1 );
float step = distL * cos(dirAngInRad);  
float x = x0;
for(int i=0; i<n; i++){
tab(0, i) = x;
tab(1, i) = getY(x);
x += step;
}
}
else{
//assert( theta < -c1 || theta > c1 );
float step = distL * sin(dirAngInRad) ;
float y = y0;
for(int i=0; i<n; i++){
tab(0, i) = getX(y);
tab(1, i) = y;
y += step; 
}
}
}

void swLine2D :: print( const char *str ) const
{
printf("%s, m_a=%f, m_b=%f, m_c=%f \n", str, m_a, m_b, m_c);
}

