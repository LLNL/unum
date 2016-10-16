
#include <string.h> /* memcpy, memset */

#include "gmp_aux.h"
/* TODO: #include "config.h" */
#if defined(HAVE_GMP_IMPL_H)
#include "gmp-impl.h" /* from GMP source */
#else
#include "gmp_macro.h"
#endif

/* * * * * * * * */
/* mpn functions */
/* * * * * * * * */

/* left shift, logical */

void mpn_lshift_n(mp_ptr rp, mp_srcptr up, mp_size_t n, mp_bitcnt_t cnt)
{
	mp_size_t limb_cnt;
	mp_bitcnt_t bit_cnt;

	limb_cnt = cnt / GMP_NUMB_BITS;
	bit_cnt = cnt % GMP_NUMB_BITS;
	if (bit_cnt != 0) {
		mpn_lshift(rp+limb_cnt, up, n-limb_cnt, bit_cnt);
	} else {
		MPN_COPY_DECR(rp+limb_cnt, up, n-limb_cnt);
	}
	MPN_ZERO(rp, limb_cnt);
}

/* right shift, logical */

void mpn_rshift_n(mp_ptr rp, mp_srcptr up, mp_size_t n, mp_bitcnt_t cnt)
{
	mp_size_t limb_cnt;
	mp_bitcnt_t bit_cnt;

	limb_cnt = cnt / GMP_NUMB_BITS;
	bit_cnt = cnt % GMP_NUMB_BITS;
	if (bit_cnt != 0) {
		mpn_rshift(rp, up+limb_cnt, n-limb_cnt, bit_cnt);
	} else {
		MPN_COPY_INCR(rp, up+limb_cnt, n-limb_cnt);
	}
	MPN_ZERO(rp+n-limb_cnt, limb_cnt);
}

/* clear bit */

void mpn_clrbit(mp_ptr dp, mp_size_t n, mp_bitcnt_t bit_idx)
{
	mp_size_t limb_idx = bit_idx / GMP_NUMB_BITS;

	if (limb_idx >= n) return;
	dp[limb_idx] &= ~(CNST_LIMB(1) << (bit_idx % GMP_NUMB_BITS));
}

/* set bit */

void mpn_setbit(mp_ptr dp, mp_size_t n, mp_bitcnt_t bit_idx)
{
	mp_size_t limb_idx = bit_idx / GMP_NUMB_BITS;

	if (limb_idx >= n) return;
	dp[limb_idx] |= CNST_LIMB(1) << (bit_idx % GMP_NUMB_BITS);
}

/* test bit */

int mpn_tstbit(mp_srcptr up, mp_size_t n, mp_bitcnt_t bit_idx)
{
	mp_size_t limb_idx = bit_idx / GMP_NUMB_BITS;

	if (limb_idx >= n) return 0;
	return (up[limb_idx] >> (bit_idx % GMP_NUMB_BITS)) & 1;
}

/* number of digits given base */

size_t mpn_sizeinbase_n(mp_srcptr up, mp_size_t n, int base)
{
	MPN_NORMALIZE(up, n);
	return mpn_sizeinbase(up, n, base);
}

/* import bytes */

void mpn_import_b(mp_ptr rp, mp_size_t n, const void *up, size_t nbytes)
{
	ASSERT(GMP_NAIL_BITS == 0); /* does not support nail bits */
	if (n*GMP_LIMB_BYTES < nbytes) nbytes = 0;
	memcpy(rp, up, nbytes);
	memset((char *)rp+nbytes, 0, n*GMP_LIMB_BYTES-nbytes);
}

/* export bytes */

void mpn_export_b(void *rp, size_t nbytes, mp_srcptr up, mp_size_t n)
{
	mp_size_t ubytes = n*GMP_LIMB_BYTES;

	ASSERT(GMP_NAIL_BITS == 0); /* does not support nail bits */
	if (ubytes < nbytes) {
		memset((char *)rp+ubytes, 0, nbytes-ubytes);
		nbytes = ubytes;
	}
	memcpy(rp, up, nbytes);
}

/* * * * * * * * */
/* mpz functions */
/* * * * * * * * */

/* import bytes */

void mpz_import_b(mpz_ptr rp, const void *up, size_t nbytes)
{
	mp_ptr zp;
	mp_size_t zsize = (nbytes + GMP_LIMB_BYTES - 1) / GMP_LIMB_BYTES;

	ASSERT(GMP_NAIL_BITS == 0); /* does not support nail bits */
	zp = MPZ_REALLOC(rp, zsize);
	memcpy(zp, up, nbytes);
	memset((char *)zp+nbytes, 0, zsize*GMP_LIMB_BYTES-nbytes);
	MPN_NORMALIZE(zp, zsize);
	SIZ(rp) = zsize;
}

/* export bytes */

void mpz_export_b(void *rp, size_t nbytes, mpz_srcptr up)
{
	mp_ptr zp = PTR(up);
	mp_size_t ubytes = ABS(SIZ(up))*GMP_LIMB_BYTES;

	ASSERT(GMP_NAIL_BITS == 0); /* does not support nail bits */
	if (ubytes < nbytes) {
		memset((char *)rp+ubytes, 0, nbytes-ubytes);
		nbytes = ubytes;
	}
	memcpy(rp, zp, nbytes);
}
