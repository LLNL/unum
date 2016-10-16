
#ifndef MPX_H_
#define MPX_H_

#include "gmp.h"
#include "gmp_aux.h"

/* TODO: move USE_MPN to config.h */
// #define USE_MPN 1

/* Number of limbs in MPX variable */
#ifndef NLIMBS
// #error "NLIMBS must be defined by the application to use MPX"
#endif

/* Precision in limbs of MPF variable */
#ifndef PLIMBS
// #error "PLIMBS must be defined by the application to use MPF"
#endif

/* MPF_BITS2LIMBS applies a minimum 53 bits, rounds upwards to a whole
   limb and adds an extra limb. MPF_LIMBS2BITS drops that extra limb,
   hence giving back the user's size in bits rounded up. Notice that
   converting limbs->bits->limbs gives an unchanged value. */
#define MPF_BITS2LIMBS(n) \
  ((mp_size_t) ((__GMP_MAX (53, n) + 2 * GMP_NUMB_BITS - 1) / GMP_NUMB_BITS))
#define MPF_LIMBS2BITS(n) \
  ((mp_bitcnt_t) (n) * GMP_NUMB_BITS - GMP_NUMB_BITS)

#define MPF_VAR2(name,bits) \
  int _##name##_p = MPF_BITS2LIMBS(bits); \
  mp_limb_t _##name##_d[_##name##_p+1]; \
  mpf_t name = {{_##name##_p,0,0,_##name##_d}}

#define MPF_VAR(name) \
  mp_limb_t _##name##_d[PLIMBS+1]; \
  mpf_t name = {{(int)PLIMBS,0,0,_##name##_d}}

#ifdef USE_MPN

/* * * * * * * * * * use mpn * * * * * * * * * */

typedef mp_limb_t mpx_s;
typedef mp_limb_t *mpx_t;
typedef mp_ptr mpx_ptr;
typedef mp_srcptr mpx_srcptr;

#define MPX_ALLOC(x) NLIMBS
#define MPX_SIZ(x)   NLIMBS
#define MPX_PTR(x)   (x)

/* no extra limbs */
#define MPX_BITS2LIMBS(n) (((n)-1)/GMP_NUMB_BITS+1)

/* not initialized to zero like the mpz version */
#define MPX_VAR2(name,bits) \
  mp_limb_t name[MPX_BITS2LIMBS(bits)]

#define MPX_VAR(name) \
  mp_limb_t name[NLIMBS]

#define mpx_init2(x,n)    do {x = MPN_ALLOC_LIMBS(MPX_BITS2LIMBS(n)); mpn_zero(x,MPX_BITS2LIMBS(n));} while (0)
#define mpx_clear(x)      MPN_FREE_LIMBS(x,0)

#define mpx_set(x,a)      mpn_copyi(x,a,NLIMBS)
#define mpx_set_ui(x,a)   do {mpn_zero((x)+1,NLIMBS-1); (x)[0]=a;} while (0)
#define mpx_get_ui(x)     ((x)[0])

#define mpx_add_ui(x,a,b) mpn_add_1(x,a,NLIMBS,b)
#define mpx_sub_ui(x,a,b) mpn_sub_1(x,a,NLIMBS,b)
#define mpx_mul_ui(x,a,b) mpn_mul_1(x,a,NLIMBS,b)

#define mpx_add(x,a,b)    mpn_add_n(x,a,b,NLIMBS)
#define mpx_sub(x,a,b)    mpn_sub_n(x,a,b,NLIMBS)
#define mpx_mul(x,a,b)    mpn_mul_n(x,a,b,NLIMBS) /* x needs 2*n limbs */

#define mpx_cmp(a,b)      mpn_cmp(a,b,NLIMBS)
#define mpx_zero_p(a)     mpn_zero_p(a,NLIMBS)

#define mpx_and(x,a,b)    mpn_and_n(x,a,b,NLIMBS)
#define mpx_ior(x,a,b)    mpn_ior_n(x,a,b,NLIMBS)
#define mpx_xor(x,a,b)    mpn_xor_n(x,a,b,NLIMBS)

#define mpx_lshift(x,a,n) mpn_lshift_n(x,a,NLIMBS,n)
#define mpx_rshift(x,a,n) mpn_rshift_n(x,a,NLIMBS,n)

#define mpx_clrbit(x,n)   mpn_clrbit(x,NLIMBS,n)
#define mpx_setbit(x,n)   mpn_setbit(x,NLIMBS,n)
#define mpx_tstbit(x,n)   mpn_tstbit(x,NLIMBS,n)

#define mpx_sizeinbase(x,b) mpn_sizeinbase_n(x,NLIMBS,b)

#define mpx_import_b(x,a,n) mpn_import_b(x,NLIMBS,a,n)
#define mpx_export_b(a,n,x) mpn_export_b(a,n,x,NLIMBS)

/* TODO: mpn (not mpz) versions of these set functions */
/* for mpx_set_f: assert((f)->_mp_exp <= NLIMBS); */
#define _PAD(x) mpn_zero((x)->_mp_d+__GMP_ABS((x)->_mp_size),NLIMBS-__GMP_ABS((x)->_mp_size))
#define mpx_set_f(x,f) do {mpz_t ztmp={{(int)NLIMBS,0,x}}; mpz_set_f(ztmp,f); _PAD(ztmp);} while (0)
#define mpf_set_x(f,x) do {mpz_t ztmp; mpf_set_z(f,mpz_roinit_n(ztmp,x,NLIMBS));} while (0)

#else /* USE_MPN */

/* * * * * * * * * * use mpz * * * * * * * * * */

typedef __mpz_struct mpx_s;
typedef mpz_t mpx_t;
typedef mpz_ptr mpx_ptr;
typedef mpz_srcptr mpx_srcptr;

#define MPX_ALLOC(x) ((x)->_mp_alloc)
#define MPX_SIZ(x)   ((x)->_mp_size)
#define MPX_PTR(x)   ((x)->_mp_d)

/* needs at least one extra limb */
#define MPX_BITS2LIMBS(n) (((n)-1)/GMP_NUMB_BITS+2)

#define MPX_VAR2(name,bits) \
  int _##name##_a = MPX_BITS2LIMBS(bits); \
  mp_limb_t _##name##_d[_##name##_a]; \
  mpz_t name = {{_##name##_a,0,_##name##_d}}

#define MPX_VAR(name) \
  mp_limb_t _##name##_d[NLIMBS]; \
  mpz_t name = {{(int)NLIMBS,0,_##name##_d}}

/* plus one limb to match MPX_BITS2LIMBS */
#define mpx_init2(x,n)    mpz_init2(x,n+GMP_NUMB_BITS)
#define mpx_clear(x)      mpz_clear(x)

#define mpx_set(x,a)      mpz_set(x,a)
#define mpx_set_ui(x,a)   mpz_set_ui(x,a)
#define mpx_get_ui(x)     mpz_get_ui(x)

#define mpx_add_ui(x,a,b) mpz_add_ui(x,a,b)
#define mpx_sub_ui(x,a,b) mpz_sub_ui(x,a,b)
#define mpx_mul_ui(x,a,b) mpz_mul_ui(x,a,b)

#define mpx_add(x,a,b)    mpz_add(x,a,b)
#define mpx_sub(x,a,b)    mpz_sub(x,a,b)
#define mpx_mul(x,a,b)    mpz_mul(x,a,b)

#define mpx_cmp(a,b)      mpz_cmp(a,b)
#define mpx_zero_p(a)     (!mpz_sgn(a))

#define mpx_and(x,a,b)    mpz_and(x,a,b)
#define mpx_ior(x,a,b)    mpz_ior(x,a,b)
#define mpx_xor(x,a,b)    mpz_xor(x,a,b)

#define mpx_lshift(x,a,n) mpz_mul_2exp(x,a,n)
#define mpx_rshift(x,a,n) mpz_tdiv_q_2exp(x,a,n)

#define mpx_clrbit(x,n)   mpz_clrbit(x,n)
#define mpx_setbit(x,n)   mpz_setbit(x,n)
#define mpx_tstbit(x,n)   mpz_tstbit(x,n)

#define mpx_sizeinbase(x,b) mpz_sizeinbase(x,b)

#define mpx_import_b(x,a,n) mpz_import_b(x,a,n)
#define mpx_export_b(a,n,x) mpz_export_b(a,n,x)

#define mpx_set_f(x,f)    mpz_set_f(x,f)
#define mpf_set_x(f,x)    mpf_set_z(f,x)

#endif /* USE_MPN */

#endif /* MPX_H_ */
