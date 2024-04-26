/*

   BLIS
   An object-based framework for developing high-performance BLAS-like
   libraries.

   Copyright (C) 2014, The University of Texas at Austin

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


//
// Prototype object-based check functions.
//

#undef  GENTPROT
#define GENTPROT( opname ) \
\
void PASTEMAC(opname,_check) \
     ( \
       const obj_t*  x, \
       const obj_t*  y  \
    );

GENTPROT( addd )
GENTPROT( copyd )
GENTPROT( subd )


#undef  GENTPROT
#define GENTPROT( opname ) \
\
void PASTEMAC(opname,_check) \
     ( \
       const obj_t*  alpha, \
       const obj_t*  x, \
       const obj_t*  y  \
    );

GENTPROT( axpyd )
GENTPROT( scal2d )


#undef  GENTPROT
#define GENTPROT( opname ) \
\
void PASTEMAC(opname,_check) \
     ( \
       const obj_t*  x  \
    );

GENTPROT( invertd )


#undef  GENTPROT
#define GENTPROT( opname ) \
\
void PASTEMAC(opname,_check) \
     ( \
       const obj_t*  alpha, \
       const obj_t*  x  \
    );

GENTPROT( invscald )
GENTPROT( scald )
GENTPROT( setd )
GENTPROT( setrd )
GENTPROT( setid )
GENTPROT( shiftd )


#undef  GENTPROT
#define GENTPROT( opname ) \
\
void PASTEMAC(opname,_check) \
     ( \
       const obj_t*  x, \
       const obj_t*  beta, \
       const obj_t*  y  \
    );

GENTPROT( xpbyd )


// -----------------------------------------------------------------------------

void bli_l1d_xy_check
     (
       const obj_t*  x,
       const obj_t*  y
     );

void bli_l1d_axy_check
     (
       const obj_t*  alpha,
       const obj_t*  x,
       const obj_t*  y
     );

void bli_l1d_x_check
     (
       const obj_t*  x
     );

void bli_l1d_ax_check
     (
       const obj_t*  alpha,
       const obj_t*  x
     );

