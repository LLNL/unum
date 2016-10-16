
#ifndef __GMP_AUX_H__
#define __GMP_AUX_H__

#include "gmp.h"

typedef __mpz_struct mpz_s;
typedef __mpf_struct mpf_s;
typedef __mpq_struct mpq_s;

__GMP_DECLSPEC extern void * (*__gmp_allocate_func)(size_t);
__GMP_DECLSPEC extern void * (*__gmp_reallocate_func)(void *, size_t, size_t);
__GMP_DECLSPEC extern void   (*__gmp_free_func)(void *, size_t);

#define MP_ALLOC(n) (*__gmp_allocate_func)(n)
#define MP_REALLOC(p,o,n) (*__gmp_reallocate_func)(p, o, n)
#define MP_FREE(p,n) (*__gmp_free_func)(p, n)

#if defined(__cplusplus)
extern "C" {
#endif

/* * * * * * * * */
/* mpn functions */
/* * * * * * * * */

#define MPN_ALLOC_LIMBS(n) \
  ((mp_limb_t *)(*__gmp_allocate_func)((n) * sizeof(mp_limb_t)))

#define MPN_REALLOC_LIMBS(p,o,n) \
  ((mp_limb_t *)(*__gmp_reallocate_func)(p, (o)*sizeof(mp_limb_t), (n)*sizeof(mp_limb_t)))

#define MPN_FREE_LIMBS(p,n) \
  (*__gmp_free_func)(p, (n)*sizeof(mp_limb_t))

void mpn_lshift_n(mp_ptr rp, mp_srcptr up, mp_size_t n, mp_bitcnt_t cnt);
void mpn_rshift_n(mp_ptr rp, mp_srcptr up, mp_size_t n, mp_bitcnt_t cnt);

void mpn_clrbit(mp_ptr dp, mp_size_t n, mp_bitcnt_t bit_idx);
void mpn_setbit(mp_ptr dp, mp_size_t n, mp_bitcnt_t bit_idx);
int mpn_tstbit(mp_srcptr up, mp_size_t n, mp_bitcnt_t bit_idx);

size_t mpn_sizeinbase_n(mp_srcptr up, mp_size_t n, int base);

void mpn_import_b(mp_ptr rp, mp_size_t n, const void *up, size_t nbytes);
void mpn_export_b(void *rp, size_t nbytes, mp_srcptr up, mp_size_t n);

/* * * * * * * * */
/* mpz functions */
/* * * * * * * * */

void mpz_import_b(mpz_ptr rp, const void *up, size_t nbytes);
void mpz_export_b(void *rp, size_t nbytes, mpz_srcptr up);

#if defined (__cplusplus)
}
#endif

#endif /* __GMP_AUX_H__ */
