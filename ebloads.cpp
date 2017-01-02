/*-------------------------------------------------------------------
 * PROJECT      epEBench - enhanced parametrizable Energy Benchmark
 * FILE NAME    ebloads.cpp
 * INSTITUTION  Fernuni Hagen
 * REVISION     1.0.4
 * PREFIX
 * DESCRIPTION  benchmark loads
 * CREATED      06-March-16, Robert Mueller
 * MODIFIED     n/a
 *
 *-------------------------------------------------------------------
 * Full Description: n.a.
 *
 *-------------------------------------------------------------------
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

#include <stdio.h>
#include <stdlib.h>

#include "ebloads.h"
#include "globals.h"

//***** test ******
//#define CNT_DIV 1
//int done0 = 0;
//int instcnt = 0;
//*****************

using namespace std;

//#define ARCH_X86
#define N 20

volatile int imemBuf[MEM_SIZE];
volatile double dmemBuf[MEM_SIZE];
volatile float fmemBuf[MEM_SIZE];

#ifdef ARCH_X86
// using SSE instructions for INTEL platform
void* run_m4x4smul_SIMD(int counts) {


/*    float m1[4*4]={1.0,     2.323,      -234.6565,        234.434,
                   234.22,  -423.434,   27.5678,            456.333,
                   1.66,    987.34,     234.345,            776.424,
                   0.34,    234.423,    555.232,            0.31};

    float m2[4*4]={1.1,     2.323,      -2234.6565,        234.434,
                   234.22,  -423.434,   274.5678,            456.333,
                   1.66,    987.34,     234.345,            776.424,
                   0.34,    234.423,    09555.232,            1.131};

    float m3[4*4]; */

    float* a = (float*)fmemBuf;
    float* b = (float*)fmemBuf;
    float* c = (float*)fmemBuf;
    int idx = 0;

    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {

    idx = (idx + 12) % (MEM_SIZE-12);
	// load matrices a and b as column-major order
	__m128 ma_col_0 = _mm_load_ps(a + idx);
	__m128 ma_col_1 = _mm_load_ps(a + idx + 4);
	__m128 ma_col_2 = _mm_load_ps(a + idx + 8);
	__m128 ma_col_3 = _mm_load_ps(a + idx + 12);

	__m128 mb_col_0 = _mm_load_ps(b + idx);
	__m128 mb_col_1 = _mm_load_ps(b + idx + 4);
	__m128 mb_col_2 = _mm_load_ps(b + idx + 8);
	__m128 mb_col_3 = _mm_load_ps(b + idx + 12);

	// get ready to store the result
	__m128 result0;
	__m128 result1;
	__m128 result2;
	__m128 result3;

	// result = first column of B x first row of A
	result0 = _mm_mul_ps(mb_col_0, _mm_replicate_x_ps(ma_col_0));
	result1 = _mm_mul_ps(mb_col_0, _mm_replicate_x_ps(ma_col_1));
	result2 = _mm_mul_ps(mb_col_0, _mm_replicate_x_ps(ma_col_2));
	result3 = _mm_mul_ps(mb_col_0, _mm_replicate_x_ps(ma_col_3));

	// result += second column of B x second row of A
	result0 = _mm_madd_ps(mb_col_1, _mm_replicate_y_ps(ma_col_0), result0);
	result1 = _mm_madd_ps(mb_col_1, _mm_replicate_y_ps(ma_col_1), result1);
	result2 = _mm_madd_ps(mb_col_1, _mm_replicate_y_ps(ma_col_2), result2);
	result3 = _mm_madd_ps(mb_col_1, _mm_replicate_y_ps(ma_col_3), result3);

	// result += third column of B x third row of A
	result0 = _mm_madd_ps(mb_col_2, _mm_replicate_z_ps(ma_col_0), result0);
	result1 = _mm_madd_ps(mb_col_2, _mm_replicate_z_ps(ma_col_1), result1);
	result2 = _mm_madd_ps(mb_col_2, _mm_replicate_z_ps(ma_col_2), result2);
	result3 = _mm_madd_ps(mb_col_2, _mm_replicate_z_ps(ma_col_3), result3);

	// result += last column of B x last row of A
	result0 = _mm_madd_ps(mb_col_3, _mm_replicate_w_ps(ma_col_0), result0);
	result1 = _mm_madd_ps(mb_col_3, _mm_replicate_w_ps(ma_col_1), result1);
	result2 = _mm_madd_ps(mb_col_3, _mm_replicate_w_ps(ma_col_2), result2);
	result3 = _mm_madd_ps(mb_col_3, _mm_replicate_w_ps(ma_col_3), result3);

	// store the result to memory
	_mm_store_ps(c + idx, result0);
	_mm_store_ps(c+idx+4, result1);
	_mm_store_ps(c+idx+8, result2);
	_mm_store_ps(c+idx+12, result3);
    }
    return NULL;
}

void* run_v1x4smul_SIMD(int counts) {

    float v1[4]={1.0,     2.323,      -234.6565,        234.434};

    float v2[4]={234.22,  -423.434,   274.5678,            456.333};

    float v3[4*4];

    float *a = v1;
    float *b = v2;
    float *c = v3;

    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {

	// load vectors a and b
	__m128 ma_col_0 = _mm_load_ps(a);
	__m128 ma_col_1 = _mm_load_ps(a + 4);
	__m128 ma_col_2 = _mm_load_ps(a + 8);
	__m128 ma_col_3 = _mm_load_ps(a + 12);

	__m128 mb_col_0 = _mm_load_ps(b);
	__m128 mb_col_1 = _mm_load_ps(b + 4);
	__m128 mb_col_2 = _mm_load_ps(b + 8);
	__m128 mb_col_3 = _mm_load_ps(b + 12);

	// get ready to store the result
	__m128 result0;
	__m128 result1;
	__m128 result2;
	__m128 result3;

	// result = first column of B x first row of A
	result0 = _mm_mul_ps(mb_col_0, _mm_replicate_x_ps(ma_col_0));
	result1 = _mm_mul_ps(mb_col_0, _mm_replicate_x_ps(ma_col_1));
	result2 = _mm_mul_ps(mb_col_0, _mm_replicate_x_ps(ma_col_2));
	result3 = _mm_mul_ps(mb_col_0, _mm_replicate_x_ps(ma_col_3));

	// result += second column of B x second row of A
	result0 = _mm_madd_ps(mb_col_1, _mm_replicate_y_ps(ma_col_0), result0);
	result1 = _mm_madd_ps(mb_col_1, _mm_replicate_y_ps(ma_col_1), result1);
	result2 = _mm_madd_ps(mb_col_1, _mm_replicate_y_ps(ma_col_2), result2);
	result3 = _mm_madd_ps(mb_col_1, _mm_replicate_y_ps(ma_col_3), result3);

	// result += third column of B x third row of A
	result0 = _mm_madd_ps(mb_col_2, _mm_replicate_z_ps(ma_col_0), result0);
	result1 = _mm_madd_ps(mb_col_2, _mm_replicate_z_ps(ma_col_1), result1);
	result2 = _mm_madd_ps(mb_col_2, _mm_replicate_z_ps(ma_col_2), result2);
	result3 = _mm_madd_ps(mb_col_2, _mm_replicate_z_ps(ma_col_3), result3);

	// result += last column of B x last row of A
	result0 = _mm_madd_ps(mb_col_3, _mm_replicate_w_ps(ma_col_0), result0);
	result1 = _mm_madd_ps(mb_col_3, _mm_replicate_w_ps(ma_col_1), result1);
	result2 = _mm_madd_ps(mb_col_3, _mm_replicate_w_ps(ma_col_2), result2);
	result3 = _mm_madd_ps(mb_col_3, _mm_replicate_w_ps(ma_col_3), result3);

	// store the result to memory
	_mm_store_ps(c, result0);
	_mm_store_ps(c+4, result1);
	_mm_store_ps(c+8, result2);
	_mm_store_ps(c+12, result3);
    }
    return NULL;
}

void* run_dmul64_SIMD(int counts) {

    volatile __m128d vec1;
    volatile __m128d vec2;
    volatile __m128d vec3;

    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {
       vec3 = _mm_mul_pd(vec1, vec2);
       vec3 = _mm_mul_pd(vec1, vec2);
       vec3 = _mm_mul_pd(vec1, vec2);
       vec3 = _mm_mul_pd(vec1, vec2);
       vec3 = _mm_mul_pd(vec1, vec2);
       vec3 = _mm_mul_pd(vec1, vec2);
       vec3 = _mm_mul_pd(vec1, vec2);
       vec3 = _mm_mul_pd(vec1, vec2);
       vec3 = _mm_mul_pd(vec1, vec2);
       vec3 = _mm_mul_pd(vec1, vec2);
       vec3 = _mm_mul_pd(vec1, vec2);
       vec3 = _mm_mul_pd(vec1, vec2);
       vec3 = _mm_mul_pd(vec1, vec2);
       vec3 = _mm_mul_pd(vec1, vec2);
       vec3 = _mm_mul_pd(vec1, vec2);
       vec3 = _mm_mul_pd(vec1, vec2);
       vec3 = _mm_mul_pd(vec1, vec2);
       vec3 = _mm_mul_pd(vec1, vec2);
       vec3 = _mm_mul_pd(vec1, vec2);
       vec3 = _mm_mul_pd(vec1, vec2);
    }
    return NULL;
}

void* run_smul32_SIMD(int counts) {

    volatile __m128 vec1;
    volatile __m128 vec2;
    volatile __m128 vec3;

    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {
       vec3 = _mm_mul_ps(vec1, vec2);
       vec3 = _mm_mul_ps(vec1, vec2);
       vec3 = _mm_mul_ps(vec1, vec2);
       vec3 = _mm_mul_ps(vec1, vec2);
       vec3 = _mm_mul_ps(vec1, vec2);
       vec3 = _mm_mul_ps(vec1, vec2);
       vec3 = _mm_mul_ps(vec1, vec2);
       vec3 = _mm_mul_ps(vec1, vec2);
       vec3 = _mm_mul_ps(vec1, vec2);
       vec3 = _mm_mul_ps(vec1, vec2);
       vec3 = _mm_mul_ps(vec1, vec2);
       vec3 = _mm_mul_ps(vec1, vec2);
       vec3 = _mm_mul_ps(vec1, vec2);
       vec3 = _mm_mul_ps(vec1, vec2);
       vec3 = _mm_mul_ps(vec1, vec2);
       vec3 = _mm_mul_ps(vec1, vec2);
       vec3 = _mm_mul_ps(vec1, vec2);
       vec3 = _mm_mul_ps(vec1, vec2);
       vec3 = _mm_mul_ps(vec1, vec2);
       vec3 = _mm_mul_ps(vec1, vec2);
    }
    return NULL;
}

void* run_ssub32_SIMD(int counts) {

    volatile __m128 vec1;
    volatile __m128 vec2;
    volatile __m128 vec3;

    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {

        vec3 = _mm_add_ps(vec1, vec2);
        vec3 = _mm_add_ps(vec1, vec2);
        vec3 = _mm_add_ps(vec1, vec2);
        vec3 = _mm_add_ps(vec1, vec2);
        vec3 = _mm_add_ps(vec1, vec2);
        vec3 = _mm_add_ps(vec1, vec2);
        vec3 = _mm_add_ps(vec1, vec2);
        vec3 = _mm_add_ps(vec1, vec2);
        vec3 = _mm_add_ps(vec1, vec2);
        vec3 = _mm_add_ps(vec1, vec2);
        vec3 = _mm_add_ps(vec1, vec2);
        vec3 = _mm_add_ps(vec1, vec2);
        vec3 = _mm_add_ps(vec1, vec2);
        vec3 = _mm_add_ps(vec1, vec2);
        vec3 = _mm_add_ps(vec1, vec2);
        vec3 = _mm_add_ps(vec1, vec2);
        vec3 = _mm_add_ps(vec1, vec2);
        vec3 = _mm_add_ps(vec1, vec2);
        vec3 = _mm_add_ps(vec1, vec2);
        vec3 = _mm_add_ps(vec1, vec2);

    }
    return NULL;
}

void* run_dsub64_SIMD(int counts) {

    volatile __m128d vec1;
    volatile __m128d vec2;
    volatile __m128d vec3;

    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {

        vec3 = _mm_add_pd(vec1, vec2);
        vec3 = _mm_add_pd(vec1, vec2);
        vec3 = _mm_add_pd(vec1, vec2);
        vec3 = _mm_add_pd(vec1, vec2);
        vec3 = _mm_add_pd(vec1, vec2);
        vec3 = _mm_add_pd(vec1, vec2);
        vec3 = _mm_add_pd(vec1, vec2);
        vec3 = _mm_add_pd(vec1, vec2);
        vec3 = _mm_add_pd(vec1, vec2);
        vec3 = _mm_add_pd(vec1, vec2);
        vec3 = _mm_add_pd(vec1, vec2);
        vec3 = _mm_add_pd(vec1, vec2);
        vec3 = _mm_add_pd(vec1, vec2);
        vec3 = _mm_add_pd(vec1, vec2);
        vec3 = _mm_add_pd(vec1, vec2);
        vec3 = _mm_add_pd(vec1, vec2);
        vec3 = _mm_add_pd(vec1, vec2);
        vec3 = _mm_add_pd(vec1, vec2);
        vec3 = _mm_add_pd(vec1, vec2);
        vec3 = _mm_add_pd(vec1, vec2);

    }
    return NULL;
}

void *run_dmem64_SIMD(int counts) {


//    double v1[1*4]={1.0, 2.323, -234.6565, 234.434};
    double *A = (double*)dmemBuf;
    int idx = 0;
    volatile __m128d vec1;

    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {

        idx = (idx + 14564) % (MEM_SIZE);
        vec1 = _mm_load_pd(&A[idx]);
        vec1 = _mm_load_pd(&A[idx]);
        vec1 = _mm_load_pd(&A[idx]);
        vec1 = _mm_load_pd(&A[idx]);
        vec1 = _mm_load_pd(&A[idx]);
        vec1 = _mm_load_pd(&A[idx]);
        vec1 = _mm_load_pd(&A[idx]);
        vec1 = _mm_load_pd(&A[idx]);
        vec1 = _mm_load_pd(&A[idx]);
        vec1 = _mm_load_pd(&A[idx]);
    }
    return NULL;
}

void *run_smem32_SIMD(int counts) {

//    float v1[1*4]={1.0, 2.323, -234.6565, 234.434};
    float *A = (float*)fmemBuf;
    volatile __m128 vec1;
    int idx = 0;

    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {

        idx = (idx + 14564) % MEM_SIZE-10;

        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);

    }
    return NULL;
}

void *run_vmov_SIMD(int counts) {

    float *A = (float*)fmemBuf;
    volatile __m128 vec1;
    int idx = 0;

    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {

        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
        vec1 = _mm_load_ps(&A[idx]);
    }
    return NULL;
}

void *run_vconvert_SIMD(int counts) {
//tbd
    float v1[1*4]={1.0, 2.323, -234.6565, 234.434};
    float *A = v1;
    volatile __m128 vec1;

    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {

        vec1 = _mm_load_ps(&A[0]);
        vec1 = _mm_load_ps(&A[0]);
        vec1 = _mm_load_ps(&A[0]);
        vec1 = _mm_load_ps(&A[0]);
        vec1 = _mm_load_ps(&A[0]);
        vec1 = _mm_load_ps(&A[0]);
        vec1 = _mm_load_ps(&A[0]);
        vec1 = _mm_load_ps(&A[0]);
        vec1 = _mm_load_ps(&A[0]);
        vec1 = _mm_load_ps(&A[0]);
        vec1 = _mm_load_ps(&A[0]);
        vec1 = _mm_load_ps(&A[0]);
        vec1 = _mm_load_ps(&A[0]);
        vec1 = _mm_load_ps(&A[0]);
        vec1 = _mm_load_ps(&A[0]);
        vec1 = _mm_load_ps(&A[0]);
        vec1 = _mm_load_ps(&A[0]);
        vec1 = _mm_load_ps(&A[0]);
        vec1 = _mm_load_ps(&A[0]);
        vec1 = _mm_load_ps(&A[0]);
    }
    return NULL;
}

#endif // ARCH_X86

#ifdef ARCH_ARM
// using NEON instruction for ARM NEON platform
//foo 0
void* run_m4x4smul_SIMD(int counts) {

 /*    float m1[4*4]={1.0,     2.323,      -234.6565,        234.434,
                   234.22,  -423.434,   27.5678,            456.333,
                   1.66,    987.34,     234.345,            776.424,
                   0.34,    234.423,    555.232,            0.31};

        float m2[4*4]={1.1,     2.323,      -2234.6565,        234.434,
                   234.22,  -423.434,   274.5678,            456.333,
                   1.66,    987.34,     234.345,            776.424,
                   0.34,    234.423,    09555.232,            1.131};

        float m3[4*4];

	float *a = m1;
        float *b = m2;
        volatile float *c = m3; */


        float *a = (float*)fmemBuf;
        float *b = (float*)fmemBuf;
        volatile float *c = fmemBuf;
        int idx = 0;

        instcnt += counts*CNT_DIV;
		while ((counts--) && !done0) {
		    idx = (idx + 16) % (MEM_SIZE-100);   // provoke many cache misses
		    asm volatile (
		        "vldmia      %1, {q8-q11}\n"     // load 16 elements of matrix left
		        "vldmia      %2, {q0-q3}\n"      // load 16 elements of matrix right

		        "vmul.f32    q12, q8, d0[0]\n"   // rslt col0  = (mat0 col0) * (mat1 col0 elt0)
		        "vmul.f32    q13, q8, d2[0]\n"   // rslt col1  = (mat0 col0) * (mat1 col1 elt0)
		        "vmul.f32    q14, q8, d4[0]\n"   // rslt col2  = (mat0 col0) * (mat1 col2 elt0)
		        "vmul.f32    q15, q8, d6[0]\n"   // rslt col3  = (mat0 col0) * (mat1 col3 elt0)

		        "vmla.f32    q12, q9, d0[1]\n"   // rslt col0 += (mat0 col1) * (mat1 col0 elt1)
		        "vmla.f32    q13, q9, d2[1]\n"   // rslt col1 += (mat0 col1) * (mat1 col1 elt1)
		        "vmla.f32    q14, q9, d4[1]\n"   // rslt col2 += (mat0 col1) * (mat1 col2 elt1)
		        "vmla.f32    q15, q9, d6[1]\n"   // rslt col3 += (mat0 col1) * (mat1 col3 elt1)

		        "vmla.f32    q12, q10, d1[0]\n"  // rslt col0 += (mat0 col2) * (mat1 col0 elt2)
		        "vmla.f32    q13, q10, d3[0]\n"  // rslt col1 += (mat0 col2) * (mat1 col1 elt2)
		        "vmla.f32    q14, q10, d5[0]\n"  // rslt col2 += (mat0 col2) * (mat1 col2 elt2)
		        "vmla.f32    q15, q10, d7[0]\n"  // rslt col3 += (mat0 col2) * (mat1 col2 elt2)

		        "vmla.f32    q12, q11, d1[1]\n"  // rslt col0 += (mat0 col3) * (mat1 col0 elt3)
		        "vmla.f32    q13, q11, d3[1]\n"  // rslt col1 += (mat0 col3) * (mat1 col1 elt3)
		        "vmla.f32    q14, q11, d5[1]\n"  // rslt col2 += (mat0 col3) * (mat1 col2 elt3)
		        "vmla.f32    q15, q11, d7[1]\n"  // rslt col3 += (mat0 col3) * (mat1 col3 elt3)

		        // output = result registers
		        "vstmia      %0, {q12-q15}\n"

		        // no output
		        :
		        // input - note *value* of pointer doesn't change
		        : "r" (c+idx), "r" (a+idx), "r" (b+idx)
		        : "memory", "q0", "q1", "q2", "q3", "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15");
			}
		return NULL;
}

//foo 1
void* run_v1x4smul_SIMD(int counts) {

        float v1[1*4]={1.0, 2.323, -234.6565, 234.434};
        float v2[1*4]={1.1, 3.123, -2234.6565, 234.434};
        float v3[1*4];

        float *a = v1;
        float *b = v2;
        float *c = v3;

        instcnt += counts*CNT_DIV;
        while ((counts--) && !done0) {
            asm volatile (
                        // Store matrix & vector leaving room at top of registers for result (q0)
                        "vldmia      %1, {q1-q4}\n"      // q1-q4 = m
                        "vldmia      %2, {q5}\n"         // q0 = v
                        "vmul.f32    q0, q1, d10[0]\n"
                        "vmla.f32    q0, q2, d10[1]\n"
                        "vmla.f32    q0, q3, d11[0]\n"
                        "vmla.f32    q0, q4, d11[1]\n"
                        "vstmia      %0, {q0}"           // output = result registers

                        : // no output
                        : "r" (c), "r" (a), "r" (b) // input - note *value* of pointer doesn't change
                        : "memory", "q0", "q1", "q2", "q3", "q4", "q5" //clobber
                        );
        }
        return NULL;
}

//foo 2
void* run_dmul64_SIMD(int counts) {

        instcnt += counts*CNT_DIV;
        while ((counts--) && !done0) {
            asm volatile (
                        "vmul.f64    d0, d1, d2\n"
                        "vmul.f64    d0, d1, d2\n"
                        "vmul.f64    d0, d1, d2\n"
                        "vmul.f64    d0, d1, d2\n"
                        "vmul.f64    d0, d1, d2\n"
                        "vmul.f64    d0, d1, d2\n"
                        "vmul.f64    d0, d1, d2\n"

                        "vmla.f64    d0, d1, d2\n"
                        "vmla.f64    d0, d1, d2\n"
                        "vmla.f64    d0, d1, d2\n"
                        "vmla.f64    d0, d1, d2\n"
                        "vmla.f64    d0, d1, d2\n"
                        "vmla.f64    d0, d1, d2\n"
                        "vmla.f64    d0, d1, d2\n"
                        "vmla.f64    d0, d1, d2\n"
                        "vmla.f64    d0, d1, d2\n"
                        "vmla.f64    d0, d1, d2\n"
                        "vmla.f64    d0, d1, d2\n"
                        "vmla.f64    d0, d1, d2\n"
                        "vmla.f64    d0, d1, d2\n"

                        : // no output
                        :  // no input
                        : "memory", "d0", "d1" //clobber
                        );
        }
        return NULL;
}

//foo 3
void* run_smul32_SIMD(int counts) {

        instcnt += counts*CNT_DIV;
        while ((counts--) && !done0) {
            asm volatile (
                        "vmul.f32    q0, q1, d10[0]\n"
                        "vmul.f32    q0, q1, d10[1]\n"
                        "vmul.f32    q0, q1, d10[0]\n"
                        "vmul.f32    q0, q1, d10[1]\n"
                        "vmul.f32    q0, q1, d10[0]\n"
                        "vmul.f32    q0, q1, d10[1]\n"
                        "vmul.f32    q0, q1, d10[0]\n"
                        "vmul.f32    q0, q1, d10[1]\n"
                        "vmul.f32    q0, q1, d10[0]\n"
                        "vmul.f32    q0, q1, d10[1]\n"
                        "vmul.f32    q0, q1, d10[0]\n"
                        "vmul.f32    q0, q1, d10[1]\n"
                        "vmul.f32    q0, q1, d10[0]\n"
                        "vmul.f32    q0, q1, d10[1]\n"
                        "vmul.f32    q0, q1, d10[0]\n"
                        "vmul.f32    q0, q1, d10[1]\n"
                        "vmul.f32    q0, q1, d10[0]\n"
                        "vmul.f32    q0, q1, d10[1]\n"
                        "vmul.f32    q0, q1, d10[0]\n"
                        "vmul.f32    q0, q1, d10[1]\n"

                        : // no output
                        :  // no input
                        : "memory", "q0", "q1" //clobber
                        );
        }
        return NULL;
}

//foo 4
void* run_ssub32_SIMD(int counts) {

        instcnt += counts*CNT_DIV;
        while ((counts--) && !done0) {
            asm volatile (
                        "vsub.f32    s0, s1\n"
                        "vsub.f32    s1, s1\n"
                        "vsub.f32    s3, s1\n"
                        "vsub.f32    s4, s1\n"
                        "vsub.f32    s5, s1\n"
                        "vsub.f32    s0, s1\n"
                        "vsub.f32    s1, s1\n"
                        "vsub.f32    s2, s1\n"
                        "vsub.f32    s3, s1\n"
                        "vsub.f32    s4, s1\n"
                        "vsub.f32    s5, s1\n"
                        "vsub.f32    s0, s1\n"
                        "vsub.f32    s1, s1\n"
                        "vsub.f32    s2, s1\n"
                        "vsub.f32    s3, s1\n"
                        "vsub.f32    s4, s1\n"
                        "vsub.f32    s5, s1\n"
                        "vsub.f32    s0, s1\n"
                        "vsub.f32    s1, s1\n"
                        "vsub.f32    s2, s1\n"

                        : // no output
                        :  // no input
                        : "memory", "s0", "s1" , "s2" , "s3" , "s4" , "s5"//clobber
                        );
        }
        return NULL;
}

//foo 5
void* run_dsub64_SIMD(int counts) {

        instcnt += counts*CNT_DIV;
        while ((counts--) && !done0) {
            asm volatile (
                        "vsub.f64    d0, d1\n"
                        "vsub.f64    d1, d1\n"
                        "vsub.f64    d2, d1\n"
                        "vsub.f64    d3, d1\n"
                        "vsub.f64    d4, d1\n"
                        "vsub.f64    d0, d1\n"
                        "vsub.f64    d1, d1\n"
                        "vsub.f64    d2, d1\n"
                        "vsub.f64    d3, d1\n"
                        "vsub.f64    d4, d1\n"
                        "vsub.f64    d0, d1\n"
                        "vsub.f64    d1, d1\n"
                        "vsub.f64    d2, d1\n"
                        "vsub.f64    d3, d1\n"
                        "vsub.f64    d4, d1\n"
                        "vsub.f64    d0, d1\n"
                        "vsub.f64    d1, d1\n"
                        "vsub.f64    d2, d1\n"
                        "vsub.f64    d3, d1\n"
                        "vsub.f64    d4, d1\n"

                        : // no output
                        :  // no input
                        : "memory", "d0", "d1","d2","d3","d4" //clobber
                        );
        }
        return NULL;
}

//foo 6
void* run_dmem64_SIMD(int counts) {

//	double m1[2*5];
//	double m2[2*5];

	volatile double *a = dmemBuf;
	volatile double *b = dmemBuf;
	unsigned short idx = 0;

        instcnt += counts*CNT_DIV;
        while ((counts--) && !done0) {

            idx++;
            asm volatile (
                        "vldr    d0, [%0, #0*8]\n"
                        "vldr    d1, [%0, #1*8]\n"
                        "vldr    d2, [%0, #2*8]\n"
                        "vldr    d3, [%0, #3*8]\n"
                        "vldr    d4, [%0, #4*8]\n"
                        "vldr    d5, [%0, #5*8]\n"
                        "vldr    d6, [%0, #6*8]\n"
                        "vldr    d7, [%0, #7*8]\n"
                        "vldr    d8, [%0, #8*8]\n"
                        "vldr    d9, [%0, #9*8]\n"
                        "vstr    d0, [%1, #0*8]\n"
                        "vstr    d1, [%1, #1*8]\n"
                        "vstr    d2, [%1, #2*8]\n"
                        "vstr    d3, [%1, #3*8]\n"
                        "vstr    d4, [%1, #4*8]\n"
                        "vstr    d5, [%1, #5*8]\n"
                        "vstr    d6, [%1, #6*8]\n"
                        "vstr    d7, [%1, #7*8]\n"
                        "vstr    d8, [%1, #8*8]\n"
                        "vstr    d9, [%1, #9*8]\n"

                        : // no output
                        : "r" (a+idx), "r" (b+idx)
                	: "memory", "d0", "d1", "d2", "d3", "d4", "d5", "d6", "d7", "d8", "d9");
        }
        return NULL;
}

//foo 7
void* run_smem32_SIMD(int counts) {

//	float m1[2*5];
//	float m2[2*5];

	float *a = (float*)fmemBuf;
	float *b = (float*)fmemBuf;
	unsigned short idx = 0;

        instcnt += counts*CNT_DIV;
        while ((counts--) && !done0) {
            idx++;
            asm volatile (
                        "vldr    s0, [%0, #0*4]\n"
                        "vldr    s1, [%0, #1*4]\n"
                        "vldr    s2, [%0, #2*4]\n"
                        "vldr    s3, [%0, #3*4]\n"
                        "vldr    s4, [%0, #4*4]\n"
                        "vldr    s5, [%0, #5*4]\n"
                        "vldr    s6, [%0, #6*4]\n"
                        "vldr    s7, [%0, #7*4]\n"
                        "vldr    s8, [%0, #8*4]\n"
                        "vldr    s9, [%0, #9*4]\n"
                        "vstr    s0, [%1, #0*4]\n"
                        "vstr    s1, [%1, #1*4]\n"
                        "vstr    s2, [%1, #2*4]\n"
                        "vstr    s3, [%1, #3*4]\n"
                        "vstr    s4, [%1, #4*4]\n"
                        "vstr    s5, [%1, #5*4]\n"
                        "vstr    s6, [%1, #6*4]\n"
                        "vstr    s7, [%1, #7*4]\n"
                        "vstr    s8, [%1, #8*4]\n"
                        "vstr    s9, [%1, #9*4]\n"

                        : // no output
                        : "r" (a+idx), "r" (b+idx)
                	: "memory", "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9");
        }
        return NULL;
}

//foo 8
void* run_vmov_SIMD(int counts) {

        instcnt += counts*CNT_DIV;
        while ((counts--) && !done0) {
            asm volatile (
                        "vmov.f64    d0, d1\n"
                        "vmov.f64    d0, d1\n"
                        "vmov.f64    d0, d1\n"
                        "vmov.f64    d0, d1\n"
                        "vmov.f64    d0, d1\n"
                        "vmov.f64    d0, d1\n"
                        "vmov.f64    d0, d1\n"
                        "vmov.f64    d0, d1\n"
                        "vmov.f64    d0, d1\n"
                        "vmov.f64    d0, d1\n"
                        "vmov.f64    d0, d1\n"
                        "vmov.f64    d0, d1\n"
                        "vmov.f64    d0, d1\n"
                        "vmov.f64    d0, d1\n"
                        "vmov.f64    d0, d1\n"
                        "vmov.f64    d0, d1\n"
                        "vmov.f64    d0, d1\n"
                        "vmov.f64    d0, d1\n"
                        "vmov.f64    d0, d1\n"
                        "vmov.f64    d0, d1\n"
                        : // no output
                        :  // no input
                        : "memory", "d0", "d1" //clobber
                        );
        }
        return NULL;
}

//foo 9
void* run_vconvert_SIMD(int counts) {

    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {
        asm volatile (
                        "vcvt.f64.f32    d0, s0\n"
                        "vcvt.f64.f32    d0, s0\n"
                        "vcvt.f64.f32    d0, s0\n"
                        "vcvt.f64.f32    d0, s0\n"
                        "vcvt.f64.f32    d0, s0\n"
                        "vcvt.f64.f32    d0, s0\n"
                        "vcvt.f64.f32    d0, s0\n"
                        "vcvt.f64.f32    d0, s0\n"
                        "vcvt.f64.f32    d0, s0\n"
                        "vcvt.f64.f32    d0, s0\n"
                        "vcvt.f64.f32    d0, s0\n"
                        "vcvt.f64.f32    d0, s0\n"
                        "vcvt.f64.f32    d0, s0\n"
                        "vcvt.f64.f32    d0, s0\n"
                        "vcvt.f64.f32    d0, s0\n"
                        "vcvt.f64.s32    d0, s0\n"
                        "vcvt.f64.s32    d0, s0\n"
                        "vcvt.f64.s32    d0, s0\n"
                        "vcvt.f64.s32    d0, s0\n"
                        "vcvt.f64.s32    d0, s0\n"
                        : // no output
                        :  // no input
                        : "memory", "d0", "s0" //clobber
                        );
        }
        return NULL;
}

#endif // ARCH_ARM

//foo 10
void* run_m4x4smul(int counts) {

/*    float m1[4*4]={1.0,     2.323,      -234.6565,        234.434,
                   234.22,  -423.434,   27.5678,            456.333,
                   1.66,    987.34,     234.345,            776.424,
                   0.34,    234.423,    555.232,            0.31};

    float m2[4*4]={1.1,     2.323,      -2234.6565,        234.434,
                   234.22,  -423.434,   274.5678,            456.333,
                   1.66,    987.34,     234.345,            776.424,
                   0.34,    234.423,    09555.232,            1.131};


    float m3[4*4];
    */

    float *mat_a = (float*)fmemBuf;
    float *mat_b = (float*)fmemBuf;
    volatile float *mat_c = fmemBuf;
    unsigned int idx = 0;

    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {

        idx = (idx + 16) % MEM_SIZE-20;      // provoke cache misses

        for (unsigned int i = idx; i < 16+idx; i += 4)
            for (unsigned int j = idx; j < 4+idx; ++j)
                mat_c[i + j] = (mat_b[i + 0] * mat_a[j +  0])
                                + (mat_b[i + 1] * mat_a[j +  4])
                                + (mat_b[i + 2] * mat_a[j +  8])
                                + (mat_b[i + 3] * mat_a[j + 12]);
    }
    return NULL;
}

#ifdef ARCH_ARM
//foo 11 ok
void* run_dmul(int counts)
 {
    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {
        asm volatile (
                        "fmuld	d24, d26, d23\n"
                        "fmuld	d24, d26, d23\n"
                        "fmuld	d24, d26, d23\n"
                        "fmuld	d24, d26, d23\n"
                        "fmuld	d24, d26, d23\n"
                        "fmuld	d24, d26, d23\n"
                        "fmuld	d24, d26, d23\n"
                        "fmuld	d24, d26, d23\n"
                        "fmuld	d24, d26, d23\n"
                        "fmuld	d24, d26, d23\n"
                        "fmuld	d24, d26, d23\n"
                        "fmuld	d24, d26, d23\n"
                        "fmuld	d24, d26, d23\n"
                        "fmuld	d24, d26, d23\n"
                        "fmuld	d24, d26, d23\n"
                        "fmuld	d24, d26, d23\n"
                        "fmuld	d24, d26, d23\n"
                        "fmuld	d24, d26, d23\n"
                        "fmuld	d24, d26, d23\n"
                        "fmuld	d24, d26, d23\n"
                    	: // no output
                        :  // no input
                        : "memory", "d26", "d23", "d24" //clobber
                        );
	}
	return NULL;
 }

//foo 12 ok
void* run_dadd(int counts)
 {
    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {
        asm volatile (
                        "faddd	d23, d26, d24\n"
                        "faddd	d23, d26, d24\n"
                        "faddd	d23, d26, d24\n"
                        "faddd	d23, d26, d24\n"
                        "faddd	d23, d26, d24\n"
                        "faddd	d23, d26, d24\n"
                        "faddd	d23, d26, d24\n"
                        "faddd	d23, d26, d24\n"
                        "faddd	d23, d26, d24\n"
                        "faddd	d23, d26, d24\n"
                        "faddd	d23, d26, d24\n"
                        "faddd	d23, d26, d24\n"
                        "faddd	d23, d26, d24\n"
                        "faddd	d23, d26, d24\n"
                        "faddd	d23, d26, d24\n"
                        "faddd	d23, d26, d24\n"
                        "faddd	d23, d26, d24\n"
                        "faddd	d23, d26, d24\n"
                        "faddd	d23, d26, d24\n"
                        "faddd	d23, d26, d24\n"
                    	: // no output
                        :  // no input
                        : "memory", "d26", "d24", "d23" //clobber
                        );
	}
	return NULL;
 }

//foo 13 ok
void* run_imul(int counts)
 {
    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {
	asm volatile (
                        "mul	r4, r3, r2\n"
                        "mul	r4, r3, r2\n"
                        "mul	r4, r3, r2\n"
                        "mul	r4, r3, r2\n"
                        "mul	r4, r3, r2\n"
                        "mul	r4, r3, r2\n"
                        "mul	r4, r3, r2\n"
                        "mul	r4, r3, r2\n"
                        "mul	r4, r3, r2\n"
                        "mul	r4, r3, r2\n"
                        "mul	r4, r3, r2\n"
                        "mul	r4, r3, r2\n"
                        "mul	r4, r3, r2\n"
                        "mul	r4, r3, r2\n"
                        "mul	r4, r3, r2\n"
                        "mul	r4, r3, r2\n"
                        "mul	r4, r3, r2\n"
                        "mul	r4, r3, r2\n"
                        "mul	r4, r3, r2\n"
                        "mul	r4, r3, r2\n"
                    	: // no output
                        :  // no input
                        : "memory", "r3", "r2", "r4" //clobber
                        );
	}
	return NULL;
 }

//foo 14 ok
void* run_iadd(int counts)
 {
    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {
        asm volatile (
                        "add	r3, r2, #1\n"
                        "add	r3, r2, #1\n"
                        "add	r3, r2, #1\n"
                        "add	r3, r2, #1\n"
                        "add	r3, r2, #1\n"
                        "add	r3, r2, #1\n"
                        "add	r3, r2, #1\n"
                        "add	r3, r2, #1\n"
                        "add	r3, r2, #1\n"
                        "add	r3, r2, #1\n"
                        "add	r3, r2, #1\n"
                        "add	r3, r2, #1\n"
                        "add	r3, r2, #1\n"
                        "add	r3, r2, #1\n"
                        "add	r3, r2, #1\n"
                        "add	r3, r2, #1\n"
                        "add	r3, r2, #1\n"
                        "add	r3, r2, #1\n"
                        "add	r3, r2, #1\n"
                        "add	r3, r2, #1\n"
                    	: // no output
                        :  // no input
                        : "memory", "r2", "r3" //clobber
                        );
	}
	return NULL;
 }
#else

//foo 11
void* run_dmul(int counts)
 {
    volatile double x = 1.123, y = 0.23, z = 6.32;

    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {
		x*=1.121;
		y*=1.122;
		z*=1.123;
		x*=1.124;
		y*=1.125;
		z*=1.126;
		x*=1.127;
		y*=1.128;
		z*=1.129;
		x*=1.120;
		y*=1.121;
		z*=1.122;
		x*=1.123;
		y*=1.124;
		z*=1.125;
		x*=1.126;
		y*=1.127;
		z*=1.128;
		x*=1.129;
		y*=1.120;
	}
	return NULL;
 }

//foo 12
void* run_dadd(int counts)
 {
    volatile double x = 3.21, y = 0.2, z = 1.34;

    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {
		x+=1.121;
		y+=1.122;
		z+=1.123;
		x+=1.124;
		y+=1.125;
		z+=1.126;
		x+=1.127;
		y+=1.128;
		z+=1.129;
		x+=1.120;
		y+=1.121;
		z+=1.122;
		x+=1.123;
		y+=1.124;
		z+=1.125;
		x+=1.126;
        y+=1.127;
		z+=1.128;
		x+=1.129;
		y+=1.120;
	}
	return NULL;
 }

//foo 13
void* run_imul(int counts)
 {
    volatile int x = 2, y = 1, z = 3;

    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {
		x*=2;
		y*=8;
		z*=7;
		x*=6;
		y*=5;
		z*=6;
		x*=7;
		y*=3;
		z*=2;
		x*=3;
        y*=2;
		z*=8;
		x*=7;
		y*=6;
		z*=5;
		x*=6;
		y*=7;
		z*=3;
		x*=2;
		y*=3;
	}
	return NULL;
 }

//foo 14
void* run_iadd(int counts)
 {
    volatile int x = 0, y = 1, z = 3;

    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {
		x+=1;
		y+=2;
		z+=3;
		x+=4;
		y+=5;
		z+=6;
		x+=7;
		y+=8;
		z+=9;
		x+=10;
        y+=1;
		z+=2;
		x+=3;
		y+=4;
		z+=5;
		x+=6;
		y+=7;
		z+=8;
		x+=9;
		y+=10;
	}
	return NULL;
 }
#endif

#ifdef ARCH_ARM
//foo 15 ok
void* run_icompare(int counts)
 {
    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {
        asm volatile (
                        "cmp	r3, #1\n"
                        "cmp	r3, #2\n"
                        "cmp	r3, #3\n"
                        "cmp	r3, r4\n"
                        "cmp	r4, #5\n"
                        "cmp	r3, #6\n"
                        "cmp	r3, #7\n"
                        "cmp	r3, #8\n"
                        "cmp	r3, r4\n"
                        "cmp	r4, #10\n"
                        "cmp	r3, #11\n"
                        "cmp	r3, #12\n"
                        "cmp	r3, #13\n"
                        "cmp	r3, r4\n"
                        "cmp	r4, #15\n"
                        "cmp	r3, #16\n"
                        "cmp	r3, #17\n"
                        "cmp	r3, #18\n"
                        "cmp	r3, r4\n"
                        "cmp	r4, #20\n"
                    	: // no output
                        :  // no input
                        : "memory", "r3", "r4" //clobber
                        );
	}
	return NULL;
 }
#else
//foo 15 ok
void* run_icompare(int counts)
 {
    volatile int x = 1;

    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {
		if (x == 1) x = 2;
		if (x == 2) x = 3;
		if (x == 3) x = 4;
		if (x == 4) x = 5;
		if (x == 5) x = 6;
		if (x == 6) x = 7;
		if (x == 7) x = 8;
		if (x == 8) x = 9;
		if (x == 9) x = 10;
		if (x == 10) x = 11;
		if (x == 11) x = 12;
		if (x == 12) x = 13;
		if (x == 13) x = 14;
		if (x == 14) x = 15;
		if (x == 15) x = 16;
		if (x == 16) x = 17;
		if (x == 17) x = 18;
		if (x == 18) x = 19;
		if (x == 19) x = 20;
		if (x == 20) x = 1;
	}
	return NULL;
 }
#endif

#ifdef ARCH_ARM
//foo 16 ok
void* run_logic(int counts)
{
	instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {
        asm volatile (
                        "and	r4, r3, #1\n"
                        "orr	r4, r3, #4\n"
                        "and	r4, r3, #15\n"
                        "orr	r4, r3, #255\n"
                        "and	r4, r3, #1\n"
                        "orr	r4, r3, #4\n"
                        "and	r4, r3, #15\n"
                        "orr	r4, r3, #255\n"
                        "and	r4, r3, #1\n"
                        "orr	r4, r3, #4\n"
                        "and	r4, r3, #15\n"
                        "orr	r4, r3, #255\n"
                        "and	r4, r3, #1\n"
                        "orr	r4, r3, #4\n"
                        "and	r4, r3, #15\n"
                        "orr	r4, r3, #255\n"
                        "and	r4, r3, #1\n"
                        "orr	r4, r3, #4\n"
                        "and	r4, r3, #15\n"
                        "orr	r4, r3, #255\n"
                    	: // no output
                        :  // no input
                        : "memory", "r3", "r4" //clobber
                        );
    }
	return NULL;
}


#else
//foo 16 ok
void* run_logic(int counts)
 {
	volatile int x;
    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {
		x = x & 1;
		x = x | 4;
		x = x & 0xf;
		x = x & 0xffff;
		x = x | 0xff;
		x = x & 1;
		x = x | 4;
		x = x & 0xf;
		x = x & 0xffff;
		x = x | 0xff;
		x = x & 1;
		x = x | 4;
		x = x & 0xf;
		x = x & 0xffff;
		x = x | 0xff;
		x = x & 1;
		x = x | 4;
		x = x & 0xf;
		x = x & 0xffff;
		x = x | 0xff;
	}
	return NULL;
 }
#endif

//foo 17 ok
#ifdef ARCH_ARM
void* run_branch(int counts)
{
    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {
 	asm volatile (
                        "b    l1\n"
                        "l1: b l2\n"
                        "l2: b l3\n"
                        "l3: b l4\n"
                        "l4: b l5\n"
                        "l5: b l6\n"
                        "l6: b l7\n"
                        "l7: b l8\n"
                        "l8: b l9\n"
                        "l9: b l10\n"
                        "l10: b l11\n"
                        "l11: b l12\n"
                        "l12: b l13\n"
                        "l13: b l14\n"
                        "l14: b l15\n"
                        "l15: b l16\n"
                        "l16: b l17\n"
                        "l17: b l18\n"
                        "l18: b l19\n"
                        "l19: b l20\n"
                        "l20:\n"
                        : // no output
                        :  // no input
                        : // "memory", "q0", "q1" //clobber
                        );
    }
	return NULL;
}
#else
// tbd
void* __attribute__((optimize("O0"))) run_branch(int counts)
{
    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {
            l1: goto l20;
            l2: goto l19;
            l3: goto l18;
            l4: goto l17;
            l5: goto l16;
            l6: goto l15;
            l7: goto l14;
            l8: goto l13;
            l9: goto l12;
            l10: goto l11;
            l11: goto ende;
            l12: goto l10;
            l13: goto l9;
            l14: goto l8;
            l15: goto l7;
            l16: goto l6;
            l17: goto l5;
            l18: goto l4;
            l19: goto l3;
            l20: goto l2;
            ende:;
    }
    return NULL;
}
#endif

//foo 18 ok
// generates 10x ldr + str instructions
void* run_imem(int counts)
{
    int a;
    //volatile int y[20];
    unsigned short index = 0;
    instcnt += counts*CNT_DIV;

    while ((counts--) && !done0) {

        index++;
        imemBuf[index] = imemBuf[index];
        imemBuf[index] = imemBuf[index];
        imemBuf[index] = imemBuf[index];
        imemBuf[index] = imemBuf[index];
        imemBuf[index] = imemBuf[index];
        imemBuf[index] = imemBuf[index];
        imemBuf[index] = imemBuf[index];
        imemBuf[index] = imemBuf[index];
        imemBuf[index] = imemBuf[index];
        imemBuf[index] = imemBuf[index];

	}
	return NULL;
}

//foo 19 ok
// generates 10x ldrd + strd instructions
void* run_dmem(int counts)
{
    // use this function, if heavy cache misses are required
    // which lead to "real" memory access

//    volatile double x[20];
//    volatile double y;
    int index = 0;
    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {

        index = (index + 14564) % MEM_SIZE;
        dmemBuf[index] = dmemBuf[index];
        dmemBuf[index] = dmemBuf[index];
        dmemBuf[index] = dmemBuf[index];
        dmemBuf[index] = dmemBuf[index];
        dmemBuf[index] = dmemBuf[index];
        dmemBuf[index] = dmemBuf[index];
        dmemBuf[index] = dmemBuf[index];
        dmemBuf[index] = dmemBuf[index];
        dmemBuf[index] = dmemBuf[index];
        dmemBuf[index] = dmemBuf[index];

	}
	return NULL;
}


#ifdef ARCH_ARM
// foo 20 ok
void* run_imov(int counts)
{
    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {
        asm volatile (
                        "movs    r0, #1\n"
                        "movs    r0, #2\n"
                        "movs    r0, #3\n"
                        "movs    r1, r0\n"
                        "movs    r0, #5\n"
                        "movs    r0, #6\n"
                        "movs    r0, #7\n"
                        "movs    r0, #8\n"
                        "movs    r0, r1\n"
                        "movs    r0, #10\n"
                        "movs    r0, #11\n"
                        "movs    r0, #12\n"
                        "movs    r0, #13\n"
                        "movs    r0, r0\n"
                        "movs    r0, #15\n"
                        "movs    r0, #16\n"
                        "movs    r0, #17\n"
                        "movs    r0, #18\n"
                        "movs    r0, #19\n"
                        "movs    r1, r0\n"
                        : // no output
                        :  // no input
                        : "memory", "r0", "r1" //clobber
                        );
	}
	return NULL;
}

#else
// foo 20 ok
void* __attribute__((optimize("O0"))) run_imov(int counts)
{
    int x;

    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {

        x = 1;
        x = 2;
        x = 3;
        x = 4;
        x = 5;
        x = 6;
        x = 7;
        x = 8;
        x = 9;
        x = 10;
        x = 11;
        x = 12;
        x = 13;
        x = 14;
        x = 15;
        x = 16;
        x = 17;
        x = 18;
        x = 19;
        x = 20;

	}
	return NULL;
}
#endif //ARCH_ARM

#ifdef ARCH_ARM
//foo 21 ok
void *run_shift(int counts)
{

    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {
        asm volatile (
                        "lsr    r0, r1, #1\n"
                        "lsr    r0, r1, #1\n"
                        "lsr    r0, r1, #1\n"
                        "lsr    r0, r1, #1\n"
                        "lsr    r0, r1, #1\n"
                        "lsr    r0, r1, #1\n"
                        "lsr    r0, r1, #1\n"
                        "lsr    r0, r1, #1\n"
                        "lsr    r0, r1, #1\n"
                        "lsr    r0, r1, #1\n"
                        "lsr    r0, r1, #1\n"
                        "lsr    r0, r1, #1\n"
                        "lsr    r0, r1, #1\n"
                        "lsr    r0, r1, #1\n"
                        "lsr    r0, r1, #1\n"
                        "lsr    r0, r1, #1\n"
                        "asrs   r0, r1, #1\n"
                        "asrs   r0, r1, #1\n"
                        "asrs   r0, r1, #1\n"
                        "asrs   r0, r1, #1\n"
                        : // no output
                        :  // no input
                        : "memory", "r0", "r1" //clobber
                        );
    }
    return NULL;
}
#endif

#ifdef ARCH_X86
//foo 21 ok
void* __attribute__((optimize("O0"))) run_shift(int counts)
{
    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {
        asm volatile (
                        "shr $1,%%eax\n"
                        "shr $1,%%eax\n"
                        "shr $1,%%eax\n"
                        "shr $1,%%eax\n"
                        "shr $1,%%eax\n"
                        "shr $1,%%eax\n"
                        "shr $1,%%eax\n"
                        "shr $1,%%eax\n"
                        "shr $1,%%eax\n"
                        "shr $1,%%eax\n"
                        "shr $1,%%eax\n"
                        "shr $1,%%eax\n"
                        "shr $1,%%eax\n"
                        "shr $1,%%eax\n"
                        "shr $1,%%eax\n"
                        "shr $1,%%eax\n"
                        "shr $1,%%eax\n"
                        "shr $1,%%eax\n"
                        "shr $1,%%eax\n"
                        "shr $1,%%eax\n"
                        : // no output
                        :  // no input
                        : // "memory" //clobber
                        );
    }
    return NULL;
}
#endif

#ifdef ARCH_ARM
//foo 22 ok
void *run_bitbyte(int counts)
{
    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {
        asm volatile (
                        "ubfx   r0, r1, #3, #4\n"
                        "ubfx   r0, r1, #2, #6\n"
                        "ubfx   r0, r1, #0, #15\n"
                        "ubfx   r1, r0, #6, #4\n"
                        "ubfx   r1, r0, #8, #10\n"
                        "ubfx   r0, r1, #3, #4\n"
                        "ubfx   r0, r1, #2, #6\n"
                        "ubfx   r0, r1, #0, #15\n"
                        "ubfx   r1, r0, #6, #4\n"
                        "ubfx   r1, r0, #8, #10\n"
                        "ubfx   r0, r1, #3, #4\n"
                        "ubfx   r0, r1, #2, #6\n"
                        "ubfx   r0, r1, #0, #15\n"
                        "ubfx   r1, r0, #6, #4\n"
                        "ubfx   r1, r0, #8, #10\n"
                        "ubfx   r0, r1, #3, #4\n"
                        "ubfx   r0, r1, #2, #6\n"
                        "ubfx   r0, r1, #0, #15\n"
                        "ubfx   r1, r0, #6, #4\n"
                        "ubfx   r1, r0, #8, #10\n"
                        : // no output
                        :  // no input
                        : "memory", "r0", "r1" //clobber
                        );
    }
    return NULL;
}

#else
//foo 22
void* run_bitbyte(int counts)
{
    volatile int x = 0x1ff;
    volatile int y;

    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {

        y = x & 0xf;
        y = x & 0xff;
        y = x & 0x1;
        y = x & 0x1ff;
        y = x & 0x6;
        y = x & 0xf;
        y = x & 0xff;
        y = x & 0x1;
        y = x & 0x1ff;
        y = x & 0x6;
        y = x & 0xf;
        y = x & 0xff;
        y = x & 0x1;
        y = x & 0x1ff;
        y = x & 0x6;
        y = x & 0xf;
        y = x & 0xff;
        y = x & 0x1;
        y = x & 0x1ff;
        y = x & 0x6;

	}
	return NULL;
}
#endif

//foo 23 ok
// for test purpose only
void* run_nop(int counts)
{
    instcnt += counts*CNT_DIV;
    while ((counts--) && !done0) {
            asm volatile (
                        "nop\n"
                        "nop\n"
                        "nop\n"
                        "nop\n"
                        "nop\n"
                        "nop\n"
                        "nop\n"
                        "nop\n"
                        "nop\n"
                        "nop\n"
                        "nop\n"
                        "nop\n"
                        "nop\n"
                        "nop\n"
                        "nop\n"
                        "nop\n"
                        "nop\n"
                        "nop\n"
                        "nop\n"
                        "nop\n"

                        : // no output
                        :  // no input
                        :  // no clobber
                        );
        }
        return NULL;
}








