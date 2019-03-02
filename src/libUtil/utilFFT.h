/*
*-------------------------------------------------------------------
* utilFFT.h
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
#ifndef _UTIL_FFT_H_
#define _UTIL_FFT_H_

#include <fftw3.h>

#define  FFT_FLOAT32		1

#if FFT_FLOAT32
#   define  FFT_ELEMEMT						float 
#	define  FFT_COMPLEX						fftwf_complex 
#	define	FFT_PLAN						fftwf_plan
#	define  FFT_CREATE_PLAN_1D(a,b,c,d,e)	fftwf_plan_dft_1d(a, b, c, d, e)
#   define  FFT_EXECUTE(a)					fftwf_execute(a)
#	define	FFT_DESTROY(a)					fftwf_destroy_plan(a)
#	define  FFT_CLEANUP()					fftwf_cleanup()
#else
#   define  FFT_ELEMEMT						double
#	define  FFT_COMPLEX						fftw_complex 
#	define	FFT_PLAN						fftw_plan
#	define  FFT_CREATE_PLAN_1D(a,b,c,d,e)	fftw_plan_dft_1d(a, b, c, d, e)
#   define  FFT_EXECUTE(a)					fftw_execute(a)
#	define	FFT_DESTROY(a)					fftw_destroy_plan(a)
#	define  FFT_CLEANUP()					fftw_cleanup()
#endif

#endif
