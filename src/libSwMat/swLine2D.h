/*
*-------------------------------------------------------------------
* swLine2D.h
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

#ifndef SW_LINE_2D_H
#define SW_LINE_2D_H 1

#include <math.h>
#include <stdio.h>
#include <conio.h>
#include <fstream>
#include <assert.h>

#include "swMatDefs.h"
#include "swMatUtilities.h"

class SWMAT_EXPORT swLine2D{
public:
//set line from two intersection pts
swLine2D();
swLine2D( const float a, const float b, const float c );
swLine2D( const swfMat &p1, const swfMat &p2 );
swLine2D( const swfMat &p, const float slope );

//a line is defined by three parameters, i.e., a*x + b*y + c=0.
//user can get these parameters by calling the following functions
float getA() const {return m_a;}
float getB() const {return m_b;}
float getC() const {return m_c;}

float getX( const float y) const;        //given y on the line, find x
float getY( const float x) const;        //given x on the line, find y
float getSlopeAng() const;				 //given the angle (in rad.) between line and x-axis	
float getDist( const swfMat &pt ) const;  //get the distance from a pt to the line 
bool getIntersectionPoint( const swLine2D &L, swfMat &intsectionPt) const;

//Find the minimum distance from a pt to a line segment if $m_isLineSegment$ is true
//return the minDist
float getMinDistFromPt2LineSegment( const swfMat &givenPt, swfMat &intersectionPt) const;

//Find the perpendicular project pt on the line from a given pt
//return the projection point on the THIS line,which is a $1 x 2$ matrix
swfMat getPerpendicularProjPt( const swfMat &givenPt ) const;

//return n equal distance sample pts long the line but around (x0,y0), 
//which means each side has equal # of point if n is odd.
void getSamplePtsAloneLine( const float x0, const float y0, const float distL, 
const int n, swfMat &tab );

//return n sample pts starting from (x0,y0) on the line along $dirAngInRad$ direction
void getSamplePtsAloneLine( const float x0, const float y0, const float dirAngInRad, 
const float distL, const int n, swfMat &tab );

bool isParallelAxisX() const;
bool isParallelAxisY() const;
bool isLineSegment() const { return m_isLineSegment;}
bool isPointInsideLineSegment( swfMat &p) const;
bool isCloseAxisX() const;
void resetLine( const swfMat &p1, const swfMat &p2 );
void resetLine( const swfMat &p, const float slope );
void print( const char *str ) const;
private:
float dist2D( const swfMat &p1, const swfMat &p2) const;
void errMsg( const char *msgStr ) const;
//a line is defined by three parameters, i.e., a*x + b*y + c=0.
float  m_a;  
float  m_b;  
float  m_c;  

//if the following two pts exist, this is a line-segment
bool     m_isLineSegment;
swfMat  m_begPt;
swfMat  m_endPt;
};

#endif
