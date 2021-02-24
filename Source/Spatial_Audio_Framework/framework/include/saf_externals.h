/*
 * Copyright 2020 Leo McCormack
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/**
 * @file saf_externals.h
 * @brief Include header for SAF externals
 *
 * @note Including this header is optional and only needed if you wish to have
 *       access to these external libraries. (i.e. not including this header
 *       will mean that these libraries are only used internally by SAF and will
 *       not be accessible in your project).
 *
 * @warning Using ATLAS (SAF_USE_ATLAS) as the performance library is not
 *          recommended, since some LAPACK routines are not implemented by the
 *          library! However, if you don't mind losing some SAF functionality
 *          (namely: certain veclib utility functions), then it may still be a
 *          good choice for your particular project.
 *
 * ## Required Dependencies
 *   A performance library comprising CBLAS and LAPACK routines is required by
 *   SAF. Add one of the following FLAGS to your project's preprocessor
 *   definitions in order to enable one of these suitable performance libraries,
 *   (which must be correctly linked when building SAF):
 *   - SAF_USE_INTEL_MKL:
 *       to enable Intel's Math Kernal Library with the Fortran LAPACK interface
 *   - SAF_USE_OPENBLAS_WITH_LAPACKE:
 *       to enable OpenBLAS with the LAPACKE interface
 *   - SAF_USE_APPLE_ACCELERATE:
 *       to enable the Accelerate framework with the Fortran LAPACK interface
 *   - SAF_USE_ATLAS:
 *       to enable ATLAS BLAS routines and ATLAS's CLAPACK interface
 *
 * ## Optional dependencies
 *   netcdf is required by the optional saf_sofa_reader module, which is enabled
 *   with the following pre-processor flag: SAF_ENABLE_SOFA_READER_MODULE
 *
 *   Intel IPP may be optionally used with the flag: SAF_USE_INTEL_IPP
 *
 * @see More information can be found here:
 *          docs/PERFORMANCE_LIBRARY_INSTRUCTIONS.md
 *
 * @author Leo McCormack
 * @date 06.08.2020
 */

#ifndef __SAF_EXTERNALS_H_INCLUDED__
#define __SAF_EXTERNALS_H_INCLUDED__

/* Assert that only one CBLAS/LAPACK performance library has been specified */
#if (defined(SAF_USE_INTEL_MKL) + \
     defined(SAF_USE_OPEN_BLAS_AND_LAPACKE) + \
     defined(SAF_USE_ATLAS) + \
     defined(SAF_USE_APPLE_ACCELERATE)) > 1
# error Only one performance library flag can be defined!
#endif

/* ========================================================================== */
/*                        Performance Library to Employ                       */
/* ========================================================================== */

#if defined(SAF_USE_INTEL_MKL)
/*
 * Using Intel's Math Kernal Library (MKL)
 * (Generally the fastest library for x86 based architectures)
 */
# define VECLIB_USE_LAPACK_FORTRAN_INTERFACE /**< Fortran LAPACK interface */
# include "mkl.h"

#elif defined(SAF_USE_OPEN_BLAS_AND_LAPACKE)
/*
 * Using OpenBLAS and the LAPACKE interface
 * (A good option for both x86 and ARM based architectures)
 */
# define VECLIB_USE_LAPACKE_INTERFACE /**< LAPACKE interface */
# include "cblas.h"
# include "lapacke.h"

#elif defined(SAF_USE_ATLAS)
/*
 * Using the Automatically Tuned Linear Algebra Software (ATLAS) library
 * (Not recommended, since some saf_veclib functions do not work with ATLAS)
 */ 
# define VECLIB_USE_CLAPACK_INTERFACE /**< CLAPACK interface */
# include "cblas-atlas.h"
# include "clapack.h"
# warning Note: CLAPACK does not include all LAPACK routines!

#elif defined(__APPLE__) && defined(SAF_USE_APPLE_ACCELERATE)
/*
 * Using Apple's Accelerate library (vDSP)
 * (Solid choice for both x86 and ARM, but only works under MacOSX and is not as
 * fast as Intel MKL for x86 systems)
 */
# define VECLIB_USE_LAPACK_FORTRAN_INTERFACE /**< Fortran LAPACK interface */
# include "Accelerate/Accelerate.h"

#else
/*
 * Note that if you would like to use some other CBLAS/LAPACK supporting library
 * then please get in touch! :-)
 */
# error SAF requires a library (or libraries) which supports CBLAS and LAPACK
#endif


/* ========================================================================== */
/*                        Optional External Libraries                         */
/* ========================================================================== */

#if defined(SAF_USE_INTEL_IPP)
/*
 * The use of Intel's Integrated Performance Primitives (IPP) is optional, but
 * does lead to some minor performance improvements for saf_fft (compared with
 * Intel MKL).
 */
# include "ipp.h"
#endif


/* ========================================================================== */
/*     External Libraries Required by the Optional saf_sofa_reader Module     */
/* ========================================================================== */

#ifdef SAF_ENABLE_SOFA_READER_MODULE
/*
 * If your compiler stopped at this point, then please add the path to the
 * netcdf include file to your project's include header paths.
 * Instructions for linking the required "netcdf" library may also be found
 * here: docs/SOFA_READER_MODULE_DEPENDENCIES.md
 */
# include <netcdf.h>
#endif


#endif /* __SAF_EXTERNALS_H_INCLUDED__ */
