
#ifndef __GMP_MACRO_H__
#define __GMP_MACRO_H__

#define ABS(x) ((x) >= 0 ? (x) : -(x))

#define SIZ(x) ((x)->_mp_size)
#define PTR(x) ((x)->_mp_d)
#define EXP(x) ((x)->_mp_exp)
#define PREC(x) ((x)->_mp_prec)
#define ALLOC(x) ((x)->_mp_alloc)

#define GMP_LIMB_BYTES sizeof(mp_limb_t)

#if defined(_LONG_LONG_LIMB)
#define CNST_LIMB(C) ((mp_limb_t) C##LL)
#else /* not _LONG_LONG_LIMB */
#define CNST_LIMB(C) ((mp_limb_t) C##L)
#endif /* _LONG_LONG_LIMB */

#define ASSERT(expr) do {} while (0)

#define MPN_COPY_INCR(dst, src, n) mpn_copyi(dst, src, n)
#define MPN_COPY_DECR(dst, src, n) mpn_copyd(dst, src, n)
#define MPN_ZERO(dst, n) mpn_zero(dst, n)

#define MPN_NORMALIZE(dst, n) \
  do { \
    while ((n) > 0) { \
      if ((dst)[(n) - 1] != 0) break; \
      (n)--; \
    } \
  } while (0)

#define MPZ_REALLOC(z,n) \
  (__GMP_UNLIKELY((n) > ALLOC(z)) ? (mp_ptr)_mpz_realloc(z,n) : PTR(z))

#endif /* __GMP_MACRO_H__ */
