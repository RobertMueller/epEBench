/*-------------------------------------------------------------------
 * LICENSE      BSD-3
 *
 * Copyright (c) 2016, Robert Müller. All rights reserved.			  
 * Copyright (c) 2016, Fernuniversität in Hagen. All rights reserved.		  
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *      * Redistributions of source code must retain the above copyright
 *        notice, this list of conditions and the following disclaimer.
 *      * Redistributions in binary form must reproduce the above copyright
 *        notice, this list of conditions and the following disclaimer in the
 *        documentation and/or other materials provided with the distribution.
 *      * Neither the name of the Åbo Akademi University nor the
 *        names of its contributors may be used to endorse or promote products
 *        derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * -------------------------------------------------------------------*/

#ifndef EBLOADS_H_INCLUDED
#define EBLOADS_H_INCLUDED

/*#define gettid() syscall(__NR_gettid)
#define handle_error_en(en, msg) \
               do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)
*/

#if defined (__i686__) || defined (__x86_64__) || defined (__i586__)
#define ARCH_X86
#include <xmmintrin.h>

#define _mm_replicate_x_ps(v) _mm_shuffle_ps((v), (v), SHUFFLE_PARAM(0, 0, 0, 0))
#define _mm_replicate_y_ps(v) _mm_shuffle_ps((v), (v), SHUFFLE_PARAM(1, 1, 1, 1))
#define _mm_replicate_z_ps(v) _mm_shuffle_ps((v), (v), SHUFFLE_PARAM(2, 2, 2, 2))
#define _mm_replicate_w_ps(v) _mm_shuffle_ps((v), (v), SHUFFLE_PARAM(3, 3, 3, 3))
#define SHUFFLE_PARAM(x, y, z, w) ((x) | ((y) << 2) | ((z) << 4) | ((w) << 6))
#define _mm_madd_ps(a, b, c) _mm_add_ps(_mm_mul_ps((a), (b)), (c))
#endif

#ifdef __arm__
#include <arm_neon.h>
#define ARCH_ARM
#endif

// SIMD
void *run_m4x4smul_SIMD(int);
void* run_v1x4smul_SIMD(int);
void* run_dmul64_SIMD(int);
void* run_smul32_SIMD(int);
void* run_ssub32_SIMD(int);
void* run_dsub64_SIMD(int);
void* run_dmem64_SIMD(int);
void* run_smem32_SIMD(int);
void* run_vmov_SIMD(int);
void* run_vconvert_SIMD(int);

// ALU / FP
void *run_m4x4smul(int);
void *run_dmul(int);
void *run_dadd(int);
void *run_iadd(int);
void *run_imul(int);
void *run_icompare(int);
void *run_logic(int);
void *run_branch(int);
void *run_imem(int);
void *run_dmem(int);
void *run_imov(int);
void *run_shift(int);
void *run_bitbyte(int);
void *run_nop(int);

#endif // LOADS_H_INCLUDED




