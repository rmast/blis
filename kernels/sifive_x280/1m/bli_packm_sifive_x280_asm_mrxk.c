/*

   BLIS
   An object-based framework for developing high-performance BLAS-like
   libraries.

   Copyright (C) 2023, SiFive, Inc.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    - Neither the name(s) of the copyright holder(s) nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
   HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include "blis.h"
#include "../riscv_cmul_macros_asm.h"
#include <math.h>
#include <riscv_vector.h>
#include <stdbool.h>
#include <stddef.h>

#define FLT_SIZE 4
#define VLE "vle32.v "
#define VLSE "vlse32.v "
#define VSE "vse32.v "
#define VSSSEG7 "vssseg7e32.v "

void bli_spackm_sifive_x280_asm_7xk
     (
             conj_t           conja,
             pack_t           schema,
             dim_t            cdim,
             dim_t            n,
             dim_t            n_max,
       const void*   restrict kappa_,
       const void*   restrict a_, inc_t inca, inc_t lda,
             void*   restrict p_,             inc_t ldp,
       const cntx_t*          cntx
     )
{
    (void) conja;
    (void) cntx;
    const float* kappa = kappa_;
    const float* a = a_;
    float* p = p_;

    float kappa_cast = *kappa;
    if (lda == 1) {
        __asm__ volatile("vsetvli zero, %0, e%1, m1, ta, ma" : : "r"(n), "i"(8 * FLT_SIZE));
        switch (cdim) {
            case 0: __asm__("vmv.v.i v0, 0");
            case 1: __asm__("vmv.v.i v1, 0");
            case 2: __asm__("vmv.v.i v2, 0");
            case 3: __asm__("vmv.v.i v3, 0");
            case 4: __asm__("vmv.v.i v4, 0");
            case 5: __asm__("vmv.v.i v5, 0");
            case 6: __asm__("vmv.v.i v6, 0");
        }
        a += (cdim - 1) * inca;
        size_t avl = n;
        while (avl) {
            const float* a_tmp = a;
            size_t vl;
            __asm__ volatile("vsetvli %0, %1, e%2, m1, ta, ma" : "=r"(vl) : "r"(avl), "i"(8 * FLT_SIZE));
            switch (cdim) {
                case 7:
                    __asm__(VLE "v6, (%0)" : : "r"(a_tmp));
                    a_tmp -= inca;
                case 6:
                    __asm__(VLE "v5, (%0)" : : "r"(a_tmp));
                    a_tmp -= inca;
                case 5:
                    __asm__(VLE "v4, (%0)" : : "r"(a_tmp));
                    a_tmp -= inca;
                case 4:
                    __asm__(VLE "v3, (%0)" : : "r"(a_tmp));
                    a_tmp -= inca;
                case 3:
                    __asm__(VLE "v2, (%0)" : : "r"(a_tmp));
                    a_tmp -= inca;
                case 2:
                    __asm__(VLE "v1, (%0)" : : "r"(a_tmp));
                    a_tmp -= inca;
                case 1:
                    __asm__(VLE "v0, (%0)" : : "r"(a_tmp));
            }
            if (kappa_cast != 1.f) {
                switch (cdim) {
                    case 7: __asm__("vfmul.vf v6, v6, %0" : : "f"(kappa_cast));
                    case 6: __asm__("vfmul.vf v5, v5, %0" : : "f"(kappa_cast));
                    case 5: __asm__("vfmul.vf v4, v4, %0" : : "f"(kappa_cast));
                    case 4: __asm__("vfmul.vf v3, v3, %0" : : "f"(kappa_cast));
                    case 3: __asm__("vfmul.vf v2, v2, %0" : : "f"(kappa_cast));
                    case 2: __asm__("vfmul.vf v1, v1, %0" : : "f"(kappa_cast));
                    case 1: __asm__("vfmul.vf v0, v0, %0" : : "f"(kappa_cast));
                }
            }
            __asm__(VSSSEG7 "v0, (%0), %1" : : "r"(p), "r"(FLT_SIZE * ldp));
            a += vl;
            p += vl * ldp;
            avl -= vl;
        }
        __asm__ volatile("vsetivli zero, 7, e%0, m1, ta, ma" : : "i"(8 * FLT_SIZE));
        __asm__("vmv.v.i v0, 0");
        for (size_t i = n; i < n_max; ++i) {
            __asm__(VSE "v0, (%0)" : : "r"(p));
            p += ldp;  
        }
    }
    else {
        inca *= FLT_SIZE;
        __asm__ volatile("vsetivli zero, 7, e%0, m1, ta, ma" : : "i"(8 * FLT_SIZE));
        __asm__("vmv.v.i v0, 0");
        for (size_t i = 0; i < n; ++i) {
            __asm__ volatile("vsetvli zero, %0, e%1, m1, tu, ma" : : "r"(cdim), "i"(8 * FLT_SIZE));
            if (inca == FLT_SIZE) {
                __asm__(VLE "v0, (%0)" : : "r"(a));
            }
            else {
                __asm__(VLSE "v0, (%0), %1" : : "r"(a), "r"(inca));
            }
            if (kappa_cast != 1.f) {
                __asm__("vfmul.vf v0, v0, %0" : : "f"(kappa_cast));
            }
            __asm__ volatile("vsetivli zero, 7, e%0, m1, ta, ma" : : "i"(8 * FLT_SIZE));
            __asm__(VSE "v0, (%0)" : : "r"(p));
            a += lda;
            p += ldp;
        }
        __asm__ volatile("vsetivli zero, 7, e%0, m1, ta, ma" : : "i"(8 * FLT_SIZE));
        __asm__("vmv.v.i v0, 0");
        for (size_t i = n; i < n_max; ++i) {
            __asm__(VSE "v0, (%0)" : : "r"(p));
            p += ldp;  
        }
    }
    return;
}

#undef FLT_SIZE
#undef VLE
#undef VLSE
#undef VSE
#undef VSSSEG7

#define FLT_SIZE 8
#define VLE "vle64.v "
#define VLSE "vlse64.v "
#define VSE "vse64.v "
#define VSSSEG7 "vssseg7e64.v "

void bli_dpackm_sifive_x280_asm_7xk
     (
             conj_t           conja,
             pack_t           schema,
             dim_t            cdim,
             dim_t            n,
             dim_t            n_max,
       const void*   restrict kappa_,
       const void*   restrict a_, inc_t inca, inc_t lda,
             void*   restrict p_,             inc_t ldp,
       const cntx_t*          cntx
     )
{
    (void) conja;
    (void) cntx;
    const double* kappa = kappa_;
    const double* a = a_;
    double* p = p_;

    double kappa_cast = *kappa;
    if (lda == 1) {
        __asm__ volatile("vsetvli zero, %0, e%1, m1, ta, ma" : : "r"(n), "i"(8 * FLT_SIZE));
        switch (cdim) {
            case 0: __asm__("vmv.v.i v0, 0");
            case 1: __asm__("vmv.v.i v1, 0");
            case 2: __asm__("vmv.v.i v2, 0");
            case 3: __asm__("vmv.v.i v3, 0");
            case 4: __asm__("vmv.v.i v4, 0");
            case 5: __asm__("vmv.v.i v5, 0");
            case 6: __asm__("vmv.v.i v6, 0");
        }
        a += (cdim - 1) * inca;
        size_t avl = n;
        while (avl) {
            const double* a_tmp = a;
            size_t vl;
            __asm__ volatile("vsetvli %0, %1, e%2, m1, ta, ma" : "=r"(vl) : "r"(avl), "i"(8 * FLT_SIZE));
            switch (cdim) {
                case 7:
                    __asm__(VLE "v6, (%0)" : : "r"(a_tmp));
                    a_tmp -= inca;
                case 6:
                    __asm__(VLE "v5, (%0)" : : "r"(a_tmp));
                    a_tmp -= inca;
                case 5:
                    __asm__(VLE "v4, (%0)" : : "r"(a_tmp));
                    a_tmp -= inca;
                case 4:
                    __asm__(VLE "v3, (%0)" : : "r"(a_tmp));
                    a_tmp -= inca;
                case 3:
                    __asm__(VLE "v2, (%0)" : : "r"(a_tmp));
                    a_tmp -= inca;
                case 2:
                    __asm__(VLE "v1, (%0)" : : "r"(a_tmp));
                    a_tmp -= inca;
                case 1:
                    __asm__(VLE "v0, (%0)" : : "r"(a_tmp));
            }
            if (kappa_cast != 1.) {
                switch (cdim) {
                    case 7: __asm__("vfmul.vf v6, v6, %0" : : "f"(kappa_cast));
                    case 6: __asm__("vfmul.vf v5, v5, %0" : : "f"(kappa_cast));
                    case 5: __asm__("vfmul.vf v4, v4, %0" : : "f"(kappa_cast));
                    case 4: __asm__("vfmul.vf v3, v3, %0" : : "f"(kappa_cast));
                    case 3: __asm__("vfmul.vf v2, v2, %0" : : "f"(kappa_cast));
                    case 2: __asm__("vfmul.vf v1, v1, %0" : : "f"(kappa_cast));
                    case 1: __asm__("vfmul.vf v0, v0, %0" : : "f"(kappa_cast));
                }
            }
            __asm__(VSSSEG7 "v0, (%0), %1" : : "r"(p), "r"(FLT_SIZE * ldp));
            a += vl;
            p += vl * ldp;
            avl -= vl;
        }
        __asm__ volatile("vsetivli zero, 7, e%0, m1, ta, ma" : : "i"(8 * FLT_SIZE));
        __asm__("vmv.v.i v0, 0");
        for (size_t i = n; i < n_max; ++i) {
            __asm__(VSE "v0, (%0)" : : "r"(p));
            p += ldp;  
        }
    }
    else {
        inca *= FLT_SIZE;
        __asm__ volatile("vsetivli zero, 7, e%0, m1, ta, ma" : : "i"(8 * FLT_SIZE));
        __asm__("vmv.v.i v0, 0");
        for (size_t i = 0; i < n; ++i) {
            __asm__ volatile("vsetvli zero, %0, e%1, m1, tu, ma" : : "r"(cdim), "i"(8 * FLT_SIZE));
            if (inca == FLT_SIZE) {
                __asm__(VLE "v0, (%0)" : : "r"(a));
            }
            else {
                __asm__(VLSE "v0, (%0), %1" : : "r"(a), "r"(inca));
            }
            if (kappa_cast != 1.) {
                __asm__("vfmul.vf v0, v0, %0" : : "f"(kappa_cast));
            }
            __asm__ volatile("vsetivli zero, 7, e%0, m1, ta, ma" : : "i"(8 * FLT_SIZE));
            __asm__(VSE "v0, (%0)" : : "r"(p));
            a += lda;
            p += ldp;
        }
        __asm__ volatile("vsetivli zero, 7, e%0, m1, ta, ma" : : "i"(8 * FLT_SIZE));
        __asm__("vmv.v.i v0, 0");
        for (size_t i = n; i < n_max; ++i) {
            __asm__(VSE "v0, (%0)" : : "r"(p));
            p += ldp;  
        }
    }
    return;
}

#undef FLT_SIZE
#undef VLE
#undef VLSE
#undef VSE
#undef VSSSEG7

#define FLT_SIZE 4
#define VLSEG2 "vlseg2e32.v "
#define VLSSEG2 "vlsseg2e32.v "
#define VSSEG2 "vsseg2e32.v "
#define VSSSEG6 "vssseg6e32.v "

void bli_cpackm_sifive_x280_asm_6xk
     (
             conj_t           conja,
             pack_t           schema,
             dim_t            cdim,
             dim_t            n,
             dim_t            n_max,
       const void*   restrict kappa_,
       const void*   restrict a_, inc_t inca, inc_t lda,
             void*   restrict p_,             inc_t ldp,
       const cntx_t*          cntx
     )
{
    (void) cntx;
    const scomplex* kappa = kappa_;
    const scomplex* a = a_;
    scomplex* p = p_;

    scomplex kappa_cast = *kappa;
    if (lda == 1) {
        __asm__ volatile("vsetvli zero, %0, e%1, m1, ta, ma" : : "r"(n), "i"(8 * FLT_SIZE));
        if (kappa_cast.real == 1.f && kappa_cast.imag == 0.f) {
            switch (cdim) {
                case 0:
                    __asm__("vmv.v.i v0, 0");
                    __asm__("vmv.v.i v1, 0");
                case 1:
                    __asm__("vmv.v.i v2, 0");
                    __asm__("vmv.v.i v3, 0");
                case 2:
                    __asm__("vmv.v.i v4, 0");
                    __asm__("vmv.v.i v5, 0");
                case 3:
                    __asm__("vmv.v.i v6, 0");
                    __asm__("vmv.v.i v7, 0");
                case 4:
                    __asm__("vmv.v.i v8, 0");
                    __asm__("vmv.v.i v9, 0");
                case 5:
                    __asm__("vmv.v.i v10, 0");
                    __asm__("vmv.v.i v11, 0");
            }
        }
        else {
            switch (cdim) {
                case 0:
                    __asm__("vmv.v.i v12, 0");
                    __asm__("vmv.v.i v13, 0");
                case 1:
                    __asm__("vmv.v.i v14, 0");
                    __asm__("vmv.v.i v15, 0");
                case 2:
                    __asm__("vmv.v.i v16, 0");
                    __asm__("vmv.v.i v17, 0");
                case 3:
                    __asm__("vmv.v.i v18, 0");
                    __asm__("vmv.v.i v19, 0");
                case 4:
                    __asm__("vmv.v.i v20, 0");
                    __asm__("vmv.v.i v21, 0");
                case 5:
                    __asm__("vmv.v.i v22, 0");
                    __asm__("vmv.v.i v23, 0");
            }
        }
        a += (cdim - 1) * inca;
        size_t avl = n;
        while (avl) {
            const scomplex* a_tmp = a;
            size_t vl;
            __asm__ volatile("vsetvli %0, %1, e%2, m1, ta, ma" : "=r"(vl) : "r"(avl), "i"(8 * FLT_SIZE));
            switch (cdim) {
                case 6:
                    __asm__(VLSEG2 "v10, (%0)" : : "r"(a_tmp));
                    a_tmp -= inca;
                case 5:
                    __asm__(VLSEG2 "v8, (%0)" : : "r"(a_tmp));
                    a_tmp -= inca;
                case 4:
                    __asm__(VLSEG2 "v6, (%0)" : : "r"(a_tmp));
                    a_tmp -= inca;
                case 3:
                    __asm__(VLSEG2 "v4, (%0)" : : "r"(a_tmp));
                    a_tmp -= inca;
                case 2:
                    __asm__(VLSEG2 "v2, (%0)" : : "r"(a_tmp));
                    a_tmp -= inca;
                case 1:
                    __asm__(VLSEG2 "v0, (%0)" : : "r"(a_tmp));
            }
            if (kappa_cast.real == 1.f && kappa_cast.imag == 0.f) {
                if (conja == BLIS_CONJUGATE) {
                    switch (cdim) {
                        case 6: __asm__("vfneg.v v11, v11");
                        case 5: __asm__("vfneg.v v9, v9");
                        case 4: __asm__("vfneg.v v7, v7");
                        case 3: __asm__("vfneg.v v5, v5");
                        case 2: __asm__("vfneg.v v3, v3");
                        case 1: __asm__("vfneg.v v1, v1");
                    }
                }
                __asm__(VSSSEG6 "v0, (%0), %1" : : "r"(p), "r"(2 * FLT_SIZE * ldp));
                __asm__(VSSSEG6 "v6, (%0), %1" : : "r"(p + 3), "r"(2 * FLT_SIZE * ldp));
            }
            else {
                if (conja == BLIS_NO_CONJUGATE) {
                    switch (cdim) {
                        case 6: vcmul_vf2(v22, v23, v10, v11, kappa_cast.real, kappa_cast.imag);
                        case 5: vcmul_vf2(v20, v21, v8, v9, kappa_cast.real, kappa_cast.imag);
                        case 4: vcmul_vf2(v18, v19, v6, v7, kappa_cast.real, kappa_cast.imag);
                        case 3: vcmul_vf2(v16, v17, v4, v5, kappa_cast.real, kappa_cast.imag);
                        case 2: vcmul_vf2(v14, v15, v2, v3, kappa_cast.real, kappa_cast.imag);
                        case 1: vcmul_vf2(v12, v13, v0, v1, kappa_cast.real, kappa_cast.imag);
                    }
                }
                else {
                    switch (cdim) {
                        case 6: vcmul_vf_conj2(v22, v23, v10, v11, kappa_cast.real, kappa_cast.imag);
                        case 5: vcmul_vf_conj2(v20, v21, v8, v9, kappa_cast.real, kappa_cast.imag);
                        case 4: vcmul_vf_conj2(v18, v19, v6, v7, kappa_cast.real, kappa_cast.imag);
                        case 3: vcmul_vf_conj2(v16, v17, v4, v5, kappa_cast.real, kappa_cast.imag);
                        case 2: vcmul_vf_conj2(v14, v15, v2, v3, kappa_cast.real, kappa_cast.imag);
                        case 1: vcmul_vf_conj2(v12, v13, v0, v1, kappa_cast.real, kappa_cast.imag);
                    }
                }
                __asm__(VSSSEG6 "v12, (%0), %1" : : "r"(p), "r"(2 * FLT_SIZE * ldp));
                __asm__(VSSSEG6 "v18, (%0), %1" : : "r"(p + 3), "r"(2 * FLT_SIZE * ldp));
            }
            a += vl;
            p += vl * ldp;
            avl -= vl;
        }
        __asm__ volatile("vsetivli zero, 6, e%0, m1, ta, ma" : : "i"(8 * FLT_SIZE));
        __asm__("vmv.v.i v0, 0");
        __asm__("vmv.v.i v1, 0");
        for (size_t i = n; i < n_max; ++i) {
            __asm__(VSSEG2 "v0, (%0)" : : "r"(p));
            p += ldp;  
        }
    }
    else {
        inca *= 2 * FLT_SIZE;
        __asm__ volatile("vsetivli zero, 6, e%0, m1, ta, ma" : : "i"(8 * FLT_SIZE));
        __asm__("vmv.v.i v0, 0");
        __asm__("vmv.v.i v1, 0");
        __asm__("vmv.v.i v2, 0");
        __asm__("vmv.v.i v3, 0");
        for (size_t i = 0; i < n; ++i) {
            __asm__ volatile("vsetvli zero, %0, e%1, m1, tu, ma" : : "r"(cdim), "i"(8 * FLT_SIZE));
            if (inca == 2 * FLT_SIZE) {
                __asm__(VLSEG2 "v0, (%0)" : : "r"(a));
            }
            else {
                __asm__(VLSSEG2 "v0, (%0), %1" : : "r"(a), "r"(inca));
            }
            if (kappa_cast.real == 1.f && kappa_cast.imag == 0.f) {
                if (conja == BLIS_CONJUGATE) {
                    __asm__("vfneg.v v1, v1");
                }
                __asm__ volatile("vsetivli zero, 6, e%0, m1, ta, ma" : : "i"(8 * FLT_SIZE));
                __asm__(VSSEG2 "v0, (%0)" : : "r"(p));
            }
            else {
                if (conja == BLIS_NO_CONJUGATE) {
                    vcmul_vf2(v2, v3, v0, v1, kappa_cast.real, kappa_cast.imag);
                }
                else {
                    vcmul_vf_conj2(v2, v3, v0, v1, kappa_cast.real, kappa_cast.imag);
                }
                __asm__ volatile("vsetivli zero, 6, e%0, m1, ta, ma" : : "i"(8 * FLT_SIZE));
                __asm__(VSSEG2 "v2, (%0)" : : "r"(p));
            }
            a += lda;
            p += ldp;
        }
        __asm__ volatile("vsetivli zero, 6, e%0, m1, ta, ma" : : "i"(8 * FLT_SIZE));
        __asm__("vmv.v.i v0, 0");
        __asm__("vmv.v.i v1, 0");
        for (size_t i = n; i < n_max; ++i) {
            __asm__(VSSEG2 "v0, (%0)" : : "r"(p));
            p += ldp;  
        }
    }
    return;
}

#undef FLT_SIZE
#undef VLSEG2
#undef VLSSEG2
#undef VSSEG2
#undef VSSSEG6

#define FLT_SIZE 8
#define VLSEG2 "vlseg2e64.v "
#define VLSSEG2 "vlsseg2e64.v "
#define VSSEG2 "vsseg2e64.v "
#define VSSSEG6 "vssseg6e64.v "

void bli_zpackm_sifive_x280_asm_6xk
     (
             conj_t           conja,
             pack_t           schema,
             dim_t            cdim,
             dim_t            n,
             dim_t            n_max,
       const void*   restrict kappa_,
       const void*   restrict a_, inc_t inca, inc_t lda,
             void*   restrict p_,             inc_t ldp,
       const cntx_t*          cntx
     )
{
    (void) cntx;
    const dcomplex* kappa = kappa_;
    const dcomplex* a = a_;
    dcomplex* p = p_;

    dcomplex kappa_cast = *kappa;
    if (lda == 1) {
        __asm__ volatile("vsetvli zero, %0, e%1, m1, ta, ma" : : "r"(n), "i"(8 * FLT_SIZE));
        if (kappa_cast.real == 1. && kappa_cast.imag == 0.) {
            switch (cdim) {
                case 0:
                    __asm__("vmv.v.i v0, 0");
                    __asm__("vmv.v.i v1, 0");
                case 1:
                    __asm__("vmv.v.i v2, 0");
                    __asm__("vmv.v.i v3, 0");
                case 2:
                    __asm__("vmv.v.i v4, 0");
                    __asm__("vmv.v.i v5, 0");
                case 3:
                    __asm__("vmv.v.i v6, 0");
                    __asm__("vmv.v.i v7, 0");
                case 4:
                    __asm__("vmv.v.i v8, 0");
                    __asm__("vmv.v.i v9, 0");
                case 5:
                    __asm__("vmv.v.i v10, 0");
                    __asm__("vmv.v.i v11, 0");
            }
        }
        else {
            switch (cdim) {
                case 0:
                    __asm__("vmv.v.i v12, 0");
                    __asm__("vmv.v.i v13, 0");
                case 1:
                    __asm__("vmv.v.i v14, 0");
                    __asm__("vmv.v.i v15, 0");
                case 2:
                    __asm__("vmv.v.i v16, 0");
                    __asm__("vmv.v.i v17, 0");
                case 3:
                    __asm__("vmv.v.i v18, 0");
                    __asm__("vmv.v.i v19, 0");
                case 4:
                    __asm__("vmv.v.i v20, 0");
                    __asm__("vmv.v.i v21, 0");
                case 5:
                    __asm__("vmv.v.i v22, 0");
                    __asm__("vmv.v.i v23, 0");
            }
        }
        a += (cdim - 1) * inca;
        size_t avl = n;
        while (avl) {
            const dcomplex* a_tmp = a;
            size_t vl;
            __asm__ volatile("vsetvli %0, %1, e%2, m1, ta, ma" : "=r"(vl) : "r"(avl), "i"(8 * FLT_SIZE));
            switch (cdim) {
                case 6:
                    __asm__(VLSEG2 "v10, (%0)" : : "r"(a_tmp));
                    a_tmp -= inca;
                case 5:
                    __asm__(VLSEG2 "v8, (%0)" : : "r"(a_tmp));
                    a_tmp -= inca;
                case 4:
                    __asm__(VLSEG2 "v6, (%0)" : : "r"(a_tmp));
                    a_tmp -= inca;
                case 3:
                    __asm__(VLSEG2 "v4, (%0)" : : "r"(a_tmp));
                    a_tmp -= inca;
                case 2:
                    __asm__(VLSEG2 "v2, (%0)" : : "r"(a_tmp));
                    a_tmp -= inca;
                case 1:
                    __asm__(VLSEG2 "v0, (%0)" : : "r"(a_tmp));
            }
            if (kappa_cast.real == 1. && kappa_cast.imag == 0.) {
                if (conja == BLIS_CONJUGATE) {
                    switch (cdim) {
                        case 6: __asm__("vfneg.v v11, v11");
                        case 5: __asm__("vfneg.v v9, v9");
                        case 4: __asm__("vfneg.v v7, v7");
                        case 3: __asm__("vfneg.v v5, v5");
                        case 2: __asm__("vfneg.v v3, v3");
                        case 1: __asm__("vfneg.v v1, v1");
                    }
                }
                __asm__(VSSSEG6 "v0, (%0), %1" : : "r"(p), "r"(2 * FLT_SIZE * ldp));
                __asm__(VSSSEG6 "v6, (%0), %1" : : "r"(p + 3), "r"(2 * FLT_SIZE * ldp));
            }
            else {
                if (conja == BLIS_NO_CONJUGATE) {
                    switch (cdim) {
                        case 6: vcmul_vf2(v22, v23, v10, v11, kappa_cast.real, kappa_cast.imag);
                        case 5: vcmul_vf2(v20, v21, v8, v9, kappa_cast.real, kappa_cast.imag);
                        case 4: vcmul_vf2(v18, v19, v6, v7, kappa_cast.real, kappa_cast.imag);
                        case 3: vcmul_vf2(v16, v17, v4, v5, kappa_cast.real, kappa_cast.imag);
                        case 2: vcmul_vf2(v14, v15, v2, v3, kappa_cast.real, kappa_cast.imag);
                        case 1: vcmul_vf2(v12, v13, v0, v1, kappa_cast.real, kappa_cast.imag);
                    }
                }
                else {
                    switch (cdim) {
                        case 6: vcmul_vf_conj2(v22, v23, v10, v11, kappa_cast.real, kappa_cast.imag);
                        case 5: vcmul_vf_conj2(v20, v21, v8, v9, kappa_cast.real, kappa_cast.imag);
                        case 4: vcmul_vf_conj2(v18, v19, v6, v7, kappa_cast.real, kappa_cast.imag);
                        case 3: vcmul_vf_conj2(v16, v17, v4, v5, kappa_cast.real, kappa_cast.imag);
                        case 2: vcmul_vf_conj2(v14, v15, v2, v3, kappa_cast.real, kappa_cast.imag);
                        case 1: vcmul_vf_conj2(v12, v13, v0, v1, kappa_cast.real, kappa_cast.imag);
                    }
                }
                __asm__(VSSSEG6 "v12, (%0), %1" : : "r"(p), "r"(2 * FLT_SIZE * ldp));
                __asm__(VSSSEG6 "v18, (%0), %1" : : "r"(p + 3), "r"(2 * FLT_SIZE * ldp));
            }
            a += vl;
            p += vl * ldp;
            avl -= vl;
        }
        __asm__ volatile("vsetivli zero, 6, e%0, m1, ta, ma" : : "i"(8 * FLT_SIZE));
        __asm__("vmv.v.i v0, 0");
        __asm__("vmv.v.i v1, 0");
        for (size_t i = n; i < n_max; ++i) {
            __asm__(VSSEG2 "v0, (%0)" : : "r"(p));
            p += ldp;  
        }
    }
    else {
        inca *= 2 * FLT_SIZE;
        __asm__ volatile("vsetivli zero, 6, e%0, m1, ta, ma" : : "i"(8 * FLT_SIZE));
        __asm__("vmv.v.i v0, 0");
        __asm__("vmv.v.i v1, 0");
        __asm__("vmv.v.i v2, 0");
        __asm__("vmv.v.i v3, 0");
        for (size_t i = 0; i < n; ++i) {
            __asm__ volatile("vsetvli zero, %0, e%1, m1, tu, ma" : : "r"(cdim), "i"(8 * FLT_SIZE));
            if (inca == 2 * FLT_SIZE) {
                __asm__(VLSEG2 "v0, (%0)" : : "r"(a));
            }
            else {
                __asm__(VLSSEG2 "v0, (%0), %1" : : "r"(a), "r"(inca));
            }
            if (kappa_cast.real == 1. && kappa_cast.imag == 0.) {
                if (conja == BLIS_CONJUGATE) {
                    __asm__("vfneg.v v1, v1");
                }
                __asm__ volatile("vsetivli zero, 6, e%0, m1, ta, ma" : : "i"(8 * FLT_SIZE));
                __asm__(VSSEG2 "v0, (%0)" : : "r"(p));
            }
            else {
                if (conja == BLIS_NO_CONJUGATE) {
                    vcmul_vf2(v2, v3, v0, v1, kappa_cast.real, kappa_cast.imag);
                }
                else {
                    vcmul_vf_conj2(v2, v3, v0, v1, kappa_cast.real, kappa_cast.imag);
                }
                __asm__ volatile("vsetivli zero, 6, e%0, m1, ta, ma" : : "i"(8 * FLT_SIZE));
                __asm__(VSSEG2 "v2, (%0)" : : "r"(p));
            }
            a += lda;
            p += ldp;
        }
        __asm__ volatile("vsetivli zero, 6, e%0, m1, ta, ma" : : "i"(8 * FLT_SIZE));
        __asm__("vmv.v.i v0, 0");
        __asm__("vmv.v.i v1, 0");
        for (size_t i = n; i < n_max; ++i) {
            __asm__(VSSEG2 "v0, (%0)" : : "r"(p));
            p += ldp;  
        }
    }
    return;
}
