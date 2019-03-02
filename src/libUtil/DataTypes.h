/*
*-------------------------------------------------------------------
* DataTypes.h
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

#ifndef __DATA_TYPES_H__
#define __DATA_TYPES_H__

#include <limits>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <inttypes.h>
#include <assert.h>

#include <cstddef>  
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <deque>
#include <locale>
#include <algorithm>

//---------- boost ---------
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
//only used in CfgAppVmti project to/read xml files
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>


#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/videostab/global_motion.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

//--------- my owen defs ----------------
#define int8		char
#define uint8		unsigned char
#define int16		short
#define uint16		unsigned short
#define int32		int
#define uint32		uint32_t
#define int64		int64_t
#define uint64		uint64_t
#define float32		float
#define float64		double

#define UINT32_NAN  0XFFFFFFFF
#define UINT64_NAN  0XFFFFFFFFFFFFFFFF

#define MY_PI_FLT32	3.1415926f
#define MY_EPS_FLT32	1e-7
#define MY_MAX_FLT32	1e+38

#define POSIX_LOCAL_TIME		(boost::posix_time::microsec_clock::local_time())
#define POSIX_UNIVERSAL_TIME	(boost::posix_time::microsec_clock::universal_time())
#define POPUP_MSG_WIN_TITLE		"RgbVideoEditor (V1.0)"

#define LAST_FRM_NUM    0XFFFFFFFFFFFFFFFF
#endif

