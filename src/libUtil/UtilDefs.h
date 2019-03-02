/*
*-------------------------------------------------------------------
* UtilDefs.h
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

#ifndef _UTIL_DEFS_H
#define _UTIL_DEFS_H

#ifdef _WINDOWS
//------- win32 dll ------------
# if _USRDLL
#	ifdef UTIL_EXPORTS
#		define UTIL_EXPORT __declspec( dllexport )
#	else
#		define UTIL_EXPORT __declspec( dllimport )
#	endif
# else
#	define UTIL_EXPORT
# endif
//------ win32 static lib ------
#else
#	define UTIL_EXPORT
#endif

#endif

