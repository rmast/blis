/*

   BLIS
   An object-based framework for developing high-performance BLAS-like
   libraries.

   Copyright (C) 2021, The University of Texas at Austin

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

#include <tci.h>

using namespace tci;

class MemBuffer
{
    protected:
        mem_t mem_{};
        rntm_t* rntm_ = nullptr;
        bool is_master_ = false;

    public:
        MemBuffer() {}

        MemBuffer( MemBuffer&& other );

        MemBuffer( dim_t size, packbuf_t pack_buf_type, rntm_t* rntm, const communicator& thread );

        MemBuffer& operator=( MemBuffer&& other );

        ~MemBuffer();

        operator mem_t*() { return &mem_; }

        operator bool() const { return bli_mem_is_alloc( const_cast<mem_t*>( &mem_ ) ); }

        void swap( MemBuffer& other );

        friend void swap( MemBuffer& lhs, MemBuffer& rhs ) { lhs.swap( rhs ); }
};

void bls_packm_finalize_mem
     (
       rntm_t* rntm,
       mem_t*  mem,
       tci_comm* thread
     );

void bls_packm
     (
       mdim_t              dim,
       num_t               dt,
       conj_t              conj,
       dim_t               mn_alloc,
       dim_t               k_alloc,
       dim_t               mn,
       dim_t               k,
       dim_t               mrnr,
       char*               ab, inc_t  rs_ab, inc_t cs_ab,
       char*&              p,  inc_t& ps_p,
       cntx_t*             cntx,
       rntm_t*             rntm,
       MemBuffer&          mem,
       const communicator& thread
     );