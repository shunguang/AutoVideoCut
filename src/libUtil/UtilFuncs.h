/*
*-------------------------------------------------------------------
* UtilFuncs.h
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
#ifndef __UTIL_FUNCS_H__
#define __UTIL_FUNCS_H__

#include "DataTypes.h"
#include <random>
#include <chrono>

#include <stdio.h>
#include <direct.h>
#include <io.h>				// For access().
#include <sys/types.h>		// For stat().
#include <sys/stat.h>		// For stat().

//#ifdef _WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
//#else
//#include <unistd.h>
//#define GetCurrentDir getcwd
//#endif

#include "UtilDefs.h"

namespace autocut {
	std::string UTIL_EXPORT num_to_string(const bool x, const std::string &msg = "");
	std::string UTIL_EXPORT num_to_string(const int32 x, const std::string &msg = "");

	std::string UTIL_EXPORT num_to_string(const uint8 x, const std::string &msg = "");
	std::string UTIL_EXPORT num_to_string(const uint16 x, const std::string &msg = "");
	std::string UTIL_EXPORT num_to_string(const uint32 x, const std::string &msg = "");
	std::string UTIL_EXPORT num_to_string(const uint64 x, const std::string &msg = "");

	std::string UTIL_EXPORT num_to_string(const float x, const std::string &msg = "");
	std::string UTIL_EXPORT num_to_string(const double x, const std::string &msg = "");
	std::string UTIL_EXPORT vec_to_string(const std::vector<std::string> &v, const std::string &msg = "");

	bool	UTIL_EXPORT string_in_vector(const std::string &s, const std::vector<std::string> &v);
	int		UTIL_EXPORT string_in_vector_relax(const std::string &s, const std::vector<std::string> &v);

	bool	UTIL_EXPORT string_to_bool(const std::string &s);
	uint64	UTIL_EXPORT string_to_uint64(const std::string &s);
	uint32	UTIL_EXPORT string_to_vector(std::vector<std::string> &v, const std::string &s, const std::string &key, const bool keepKey = false);
	uint32	UTIL_EXPORT string_to_vector2(std::vector<std::string> &v, const std::string &s, const std::string &key, const bool keepKey = false);

	int8	UTIL_EXPORT int32_to_int8(const int32 x);

	std::string	UTIL_EXPORT vector_to_string(const std::vector<std::string> &v, const std::string &seperateKey);

	//remove all chars $ch$ at the beging and end of the string.
	void	UTIL_EXPORT string_trim(std::string &x, const char ch = ' ');
	void	UTIL_EXPORT string_trim(std::string &x, const std::vector<char> &keys);
	void	UTIL_EXPORT string_find_n_replace(std::string &x, const char chFind, const char chRaplace);

	std::string  UTIL_EXPORT string_parse(const std::string &x, const std::string &key);
	std::string  UTIL_EXPORT string_find_mac(const std::string &x, const char &seperationKey = ':');

	//process vector of string
	void UTIL_EXPORT vstr_read_txt_file(std::vector<std::string> &vLines, const std::string &fileName, const char commentChar);
	void UTIL_EXPORT vstr_select_util(std::vector<std::string> &retLines, const std::vector<std::string> &vAllLines, const std::string &startKey, const std::string &stopKey);
	std::string UTIL_EXPORT vstr_find_value(const std::vector<std::string> &vLines, const std::string &key);

	//time related funcs
	std::string UTIL_EXPORT getPrettyTimeStamp(const bool dayTimeOnly = false);
	std::string UTIL_EXPORT getPrettyTimeStamp(const boost::posix_time::ptime &t, const bool dayTimeOnly = false);
	std::string UTIL_EXPORT timeSecToHHMMSSxx(const float seconds);

	uint32 UTIL_EXPORT checkTimeOut(const boost::posix_time::ptime &start, const uint32 thdInMillisec);
	uint32 UTIL_EXPORT timeIntervalMillisec(const boost::posix_time::ptime &start);
	uint32 UTIL_EXPORT timeIntervalMillisec(const boost::posix_time::ptime &start, const boost::posix_time::ptime &end);

	void UTIL_EXPORT printfStr(const std::string &s, const std::string &msg);
	void UTIL_EXPORT myCreateDir(const std::string &p);

	//generate about <n> distinct/unique uniform distributed random number in [a,b]
	void UTIL_EXPORT genUniformDistinctRands(std::vector<int> &v, const int n, const int a, const int b);

	std::string UTIL_EXPORT rgbToStr(const uint8 *rgb);
	void UTIL_EXPORT rgbFromStr(uint8 *rgb, const std::string &s);

	//fPath="c:/abc/efg/a.txt", head="c:/abc/efg", tail="a.txt"
	void UTIL_EXPORT split(const std::string &fPath, std::string &head, std::string &tail);
	//fPath="c:/abc/efg/a.txt", head="c:/abc/efg/a", tail="txt"
	void UTIL_EXPORT splitExt(const std::string &fPath, std::string &head, std::string &ext);
	uint32_t UTIL_EXPORT getFileNameList(const std::string &dirName, const std::string &ext, std::vector<std::string> &vFileNames);
	void UTIL_EXPORT dateStrToNum(const std::string &sdate, int &yyyy, int &mm, int &dd);
}

#endif
