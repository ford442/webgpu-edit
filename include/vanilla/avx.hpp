#pragma once

#include <wasm_simd128.h>
// #include <tmmintrin.h> // ssse 3
// #include <smmintrin.h>  // sse 4.1
// #include <avxintrin.h>  // AVX

#define VLEAVE _mm256_zeroupper

// #include <xmmintrin.h>
#include <immintrin.h> 

// #include <mmintrin.h>  x86
// #include <nmmintrin.h>  //  sse 4.2
// #include <unistd.h>



#define __m64i  __m64

//MMX
__m64 _mm_unpackhi_pi32_(__m64 a, __m64 b);
__m64 _mm_unpacklo_pi32_(__m64 a, __m64 b);
// int _mm_extract_epi16_(__m128i a, int imm8);
__m64 _mm_setzero_si64_();
__m64 _m_pmaddwd_(__m64 a, __m64 b);
__m64 _m_paddd_(__m64 a, __m64 b);
__m64 _m_paddw_(__m64 a, __m64 b);
__m64 _m_psradi_(__m64 a, int imm8);
__m64 _m_psrlqi_(__m64 a, int imm8);
__m64 _mm_xor_si64_(__m64 a, __m64 b);
__m64 _mm_packs_pi32_(__m64 a, __m64 b);
int _mm_cvtsi64_si32_(__m64 a);
int _m_to_int_(__m64 a);
__m64 _mm_mullo_pi16_(__m64 a, __m64 b);
__m64 _mm_adds_pi16_(__m64 a, __m64 b);
__m64 _mm_subs_pi16_(__m64 a, __m64 b);
__m64 _mm_srai_pi16_(__m64 a, int imm8);
__m64 _mm_hadds_pi16_(__m64 a, __m64 b);
__m64 _mm_or_si64_(__m64 a, __m64 b);
__m64 _mm_cmpeq_pi16_(__m64 a, __m64 b);
__m64 _mm_sign_pi16_(__m64 a, __m64 b);

#define _mm_unpackhi_pi32 _mm_unpackhi_pi32_
#define _mm_unpacklo_pi32 _mm_unpacklo_pi32_
// #define _mm_extract_epi16 _mm_extract_epi16_
#define _mm_setzero_si64 _mm_setzero_si64_
#define _mm_packs_pi32 _mm_packs_pi32_
#define _m_pmaddwd _m_pmaddwd_
#define _m_paddd _m_paddd_
#define _m_paddw _m_paddw_
#define _m_psradi _m_psradi_
#define _m_psrlqi _m_psrlqi_
#define _m_to_int _m_to_int_
#define _mm_xor_si64 _mm_xor_si64_
#define _mm_cvtsi64_si32 _mm_cvtsi64_si32_
#define _mm_mullo_pi16 _mm_mullo_pi16_
#define _mm_adds_pi16 _mm_adds_pi16_
#define _mm_subs_pi16 _mm_subs_pi16_
#define _mm_srai_pi16 _mm_srai_pi16_
#define _mm_hadds_pi16 _mm_hadds_pi16_
#define _mm_or_si64 _mm_or_si64_
#define _mm_cmpeq_pi16 _mm_cmpeq_pi16_
#define _mm_sign_pi16 _mm_sign_pi16_


