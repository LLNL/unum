/* unumxx.h -- C++ class wrapper for unum types.  -*- C++ -*-

This file is a modified version of gmpxx.h from the GNU MP Library.
Changed: September 3, 2016 by Scott Lloyd

Copyright 2001-2003, 2006, 2008, 2011, 2012 Free Software Foundation, Inc.

This file is part of the UNUM Library.

The UNUM Library is free software; you can redistribute it and/or modify
it under the terms of either:

  * the GNU Lesser General Public License as published by the Free
    Software Foundation; either version 3 of the License, or (at your
    option) any later version.

or

  * the GNU General Public License as published by the Free Software
    Foundation; either version 2 of the License, or (at your option) any
    later version.

or both in parallel, as here.

The UNUM Library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received copies of the GNU General Public License and the
GNU Lesser General Public License along with the UNUM Library.  If not,
see https://www.gnu.org/licenses/.  */

#ifndef __UNUM_PLUSPLUS__
#define __UNUM_PLUSPLUS__

#include <iosfwd>
#include <ostream> // std::ostream::write

#include <limits> // numeric_limits
#include <algorithm> // swap
#include <string> // c_str
#include <cstring> // strlen

#include "uenv.h"
#include "conv.h"
#include "hlayer.h"
#include "support.h"
#include "ubnd.h"

// wrapper for gcc's __builtin_constant_p
// __builtin_constant_p has been in gcc since forever,
// but g++-3.4 miscompiles it.
#if __GMP_GNUC_PREREQ(4, 2)
#define __UNUMXX_CONSTANT(X) __builtin_constant_p(X)
#else
#define __UNUMXX_CONSTANT(X) false
#endif
#define __UNUMXX_CONSTANT_TRUE(X) (__UNUMXX_CONSTANT(X) && (X))

// Use C++11 features
#ifndef __UNUMXX_USE_CXX11
#if __cplusplus >= 201103L
#define __UNUMXX_USE_CXX11 1
#else
#define __UNUMXX_USE_CXX11 0
#endif
#endif

#if __UNUMXX_USE_CXX11
#define __UNUMXX_NOEXCEPT noexcept
#include <type_traits> // for common_type
#else
#define __UNUMXX_NOEXCEPT
#endif

#define NOT_USED 0

#if defined(AUTO_GUESS)
#define AGUESS(mp) do {guessu(mp->l, mp); mp->p = 0;} while (0);
#else
#define AGUESS(mp)
#endif

typedef const unum_s *unum_srcptr;
typedef unum_s *unum_ptr;
typedef const ubnd_s *ubnd_srcptr;
typedef ubnd_s *ubnd_ptr;


/**************** Function objects ****************/
/* Any evaluation of a __unum_expr ends up calling one of these functions
   all intermediate functions being inline, the evaluation should optimize
   to a direct call to the relevant function, thus yielding no overhead
   over the C interface. */

struct __unum_unary_plus
{
  static void eval(ubnd_ptr f, ubnd_srcptr g) { ubnd_copy(f, g); }
};

struct __unum_unary_minus
{
  static void eval(ubnd_ptr f, ubnd_srcptr g) { negateu(f, g); }
};

struct __unum_unary_com
{
};

struct __unum_binary_plus
{
  static void eval(ubnd_ptr f, ubnd_srcptr g, ubnd_srcptr h)
  { plusu(f, g, h); }

  static void eval(ubnd_ptr f, ubnd_srcptr g, unsigned long int l)
  { UB_VAR(temp); ui2ub(temp, l); plusu(f, g, temp); }
  static void eval(ubnd_ptr f, unsigned long int l, ubnd_srcptr g)
  { UB_VAR(temp); ui2ub(temp, l); plusu(f, temp, g);  }

  static void eval(ubnd_ptr f, ubnd_srcptr g, signed long int l)
  { UB_VAR(temp); si2ub(temp, l); plusu(f, g, temp); }
  static void eval(ubnd_ptr f, signed long int l, ubnd_srcptr g)
  { UB_VAR(temp); si2ub(temp, l); plusu(f, temp, g);  }

  static void eval(ubnd_ptr f, ubnd_srcptr g, double d)
  { UB_VAR(temp); d2ub(temp, d); plusu(f, g, temp); }
  static void eval(ubnd_ptr f, double d, ubnd_srcptr g)
  { UB_VAR(temp); d2ub(temp, d); plusu(f, temp, g);  }
};

struct __unum_binary_minus
{
  static void eval(ubnd_ptr f, ubnd_srcptr g, ubnd_srcptr h)
  { minusu(f, g, h); }

  static void eval(ubnd_ptr f, ubnd_srcptr g, unsigned long int l)
  { UB_VAR(temp); ui2ub(temp, l); minusu(f, g, temp); }
  static void eval(ubnd_ptr f, unsigned long int l, ubnd_srcptr g)
  { UB_VAR(temp); ui2ub(temp, l); minusu(f, temp, g);  }

  static void eval(ubnd_ptr f, ubnd_srcptr g, signed long int l)
  { UB_VAR(temp); si2ub(temp, l); minusu(f, g, temp); }
  static void eval(ubnd_ptr f, signed long int l, ubnd_srcptr g)
  { UB_VAR(temp); si2ub(temp, l); minusu(f, temp, g);  }

  static void eval(ubnd_ptr f, ubnd_srcptr g, double d)
  { UB_VAR(temp); d2ub(temp, d); minusu(f, g, temp); }
  static void eval(ubnd_ptr f, double d, ubnd_srcptr g)
  { UB_VAR(temp); d2ub(temp, d); minusu(f, temp, g);  }
};

struct __unum_binary_lshift
{
};

struct __unum_binary_rshift
{
};

struct __unum_binary_multiplies
{
  static void eval(ubnd_ptr f, ubnd_srcptr g, ubnd_srcptr h)
  { if (g == h) squareu(f, g); else timesu(f, g, h); }

  static void eval(ubnd_ptr f, ubnd_srcptr g, unsigned long int l)
  { UB_VAR(temp); ui2ub(temp, l); timesu(f, g, temp); }
  static void eval(ubnd_ptr f, unsigned long int l, ubnd_srcptr g)
  { UB_VAR(temp); ui2ub(temp, l); timesu(f, temp, g);  }

  static void eval(ubnd_ptr f, ubnd_srcptr g, signed long int l)
  { UB_VAR(temp); si2ub(temp, l); timesu(f, g, temp); }
  static void eval(ubnd_ptr f, signed long int l, ubnd_srcptr g)
  { UB_VAR(temp); si2ub(temp, l); timesu(f, temp, g);  }

  static void eval(ubnd_ptr f, ubnd_srcptr g, double d)
  { UB_VAR(temp); d2ub(temp, d); timesu(f, g, temp); }
  static void eval(ubnd_ptr f, double d, ubnd_srcptr g)
  { UB_VAR(temp); d2ub(temp, d); timesu(f, temp, g);  }
};

struct __unum_binary_divides
{
  static void eval(ubnd_ptr f, ubnd_srcptr g, ubnd_srcptr h)
  { divideu(f, g, h); }

  static void eval(ubnd_ptr f, ubnd_srcptr g, unsigned long int l)
  { UB_VAR(temp); ui2ub(temp, l); divideu(f, g, temp); }
  static void eval(ubnd_ptr f, unsigned long int l, ubnd_srcptr g)
  { UB_VAR(temp); ui2ub(temp, l); divideu(f, temp, g);  }

  static void eval(ubnd_ptr f, ubnd_srcptr g, signed long int l)
  { UB_VAR(temp); si2ub(temp, l); divideu(f, g, temp); }
  static void eval(ubnd_ptr f, signed long int l, ubnd_srcptr g)
  { UB_VAR(temp); si2ub(temp, l); divideu(f, temp, g);  }

  static void eval(ubnd_ptr f, ubnd_srcptr g, double d)
  { UB_VAR(temp); d2ub(temp, d); divideu(f, g, temp); }
  static void eval(ubnd_ptr f, double d, ubnd_srcptr g)
  { UB_VAR(temp); d2ub(temp, d); divideu(f, temp, g);  }
};

struct __unum_binary_modulus
{
};

struct __unum_binary_and
{
};

struct __unum_binary_ior
{
};

struct __unum_binary_xor
{
};

struct __unum_cmp_function
{
};

struct __unum_binary_equal
{
  static bool eval(ubnd_srcptr f, ubnd_srcptr g)
  { return sameuQ(f, g) != 0; }

  static bool eval(ubnd_srcptr f, unsigned long int l)
  { UB_VAR(temp); ui2ub(temp, l); return sameuQ(f, temp) != 0; }
  static bool eval(unsigned long int l, ubnd_srcptr f)
  { UB_VAR(temp); ui2ub(temp, l); return sameuQ(temp, f) != 0; }

  static bool eval(ubnd_srcptr f, signed long int l)
  { UB_VAR(temp); si2ub(temp, l); return sameuQ(f, temp) != 0; }
  static bool eval(signed long int l, ubnd_srcptr f)
  { UB_VAR(temp); si2ub(temp, l); return sameuQ(temp, f) != 0; }

  static bool eval(ubnd_srcptr f, double d)
  { UB_VAR(temp); d2ub(temp, d); return sameuQ(f, temp) != 0; }
  static bool eval(double d, ubnd_srcptr f)
  { UB_VAR(temp); d2ub(temp, d); return sameuQ(temp, f) != 0; }
};

struct __unum_binary_less
{
  static bool eval(ubnd_srcptr f, ubnd_srcptr g)
  { return ltuQ(f, g) != 0; }

  static bool eval(ubnd_srcptr f, unsigned long int l)
  { UB_VAR(temp); ui2ub(temp, l); return ltuQ(f, temp) != 0; }
  static bool eval(unsigned long int l, ubnd_srcptr f)
  { UB_VAR(temp); ui2ub(temp, l); return ltuQ(temp, f) != 0; }

  static bool eval(ubnd_srcptr f, signed long int l)
  { UB_VAR(temp); si2ub(temp, l); return ltuQ(f, temp) != 0; }
  static bool eval(signed long int l, ubnd_srcptr f)
  { UB_VAR(temp); si2ub(temp, l); return ltuQ(temp, f) != 0; }

  static bool eval(ubnd_srcptr f, double d)
  { UB_VAR(temp); d2ub(temp, d); return ltuQ(f, temp) != 0; }
  static bool eval(double d, ubnd_srcptr f)
  { UB_VAR(temp); d2ub(temp, d); return ltuQ(temp, f) != 0; }
};

struct __unum_binary_greater
{
  static bool eval(ubnd_srcptr f, ubnd_srcptr g)
  { return gtuQ(f, g) != 0; }

  static bool eval(ubnd_srcptr f, unsigned long int l)
  { UB_VAR(temp); ui2ub(temp, l); return gtuQ(f, temp) != 0; }
  static bool eval(unsigned long int l, ubnd_srcptr f)
  { UB_VAR(temp); ui2ub(temp, l); return gtuQ(temp, f) != 0; }

  static bool eval(ubnd_srcptr f, signed long int l)
  { UB_VAR(temp); si2ub(temp, l); return gtuQ(f, temp) != 0; }
  static bool eval(signed long int l, ubnd_srcptr f)
  { UB_VAR(temp); si2ub(temp, l); return gtuQ(temp, f) != 0; }

  static bool eval(ubnd_srcptr f, double d)
  { UB_VAR(temp); d2ub(temp, d); return gtuQ(f, temp) != 0; }
  static bool eval(double d, ubnd_srcptr f)
  { UB_VAR(temp); d2ub(temp, d); return gtuQ(temp, f) != 0; }
};

struct __unum_binary_nequal
{
  static bool eval(ubnd_srcptr f, ubnd_srcptr g)
  { return nequQ(f, g) != 0; }

  static bool eval(ubnd_srcptr f, unsigned long int l)
  { UB_VAR(temp); ui2ub(temp, l); return nequQ(f, temp) != 0; }
  static bool eval(unsigned long int l, ubnd_srcptr f)
  { UB_VAR(temp); ui2ub(temp, l); return nequQ(temp, f) != 0; }

  static bool eval(ubnd_srcptr f, signed long int l)
  { UB_VAR(temp); si2ub(temp, l); return nequQ(f, temp) != 0; }
  static bool eval(signed long int l, ubnd_srcptr f)
  { UB_VAR(temp); si2ub(temp, l); return nequQ(temp, f) != 0; }

  static bool eval(ubnd_srcptr f, double d)
  { UB_VAR(temp); d2ub(temp, d); return nequQ(f, temp) != 0; }
  static bool eval(double d, ubnd_srcptr f)
  { UB_VAR(temp); d2ub(temp, d); return nequQ(temp, f) != 0; }
};

struct __unum_unary_increment
{
  static void eval(ubnd_ptr f)
  { UB_VAR(temp); ui2ub(temp, 1); plusu(f, f, temp); }
};

struct __unum_unary_decrement
{
  static void eval(ubnd_ptr f)
  { UB_VAR(temp); ui2ub(temp, 1); minusu(f, f, temp); }
};

struct __unum_abs_function
{
  static void eval(ubnd_ptr f, ubnd_srcptr g) { absu(f, g); }
};

struct __unum_trunc_function
{
};

struct __unum_floor_function
{
};

struct __unum_ceil_function
{
};

struct __unum_guess_function
{
  static void eval(ubnd_ptr f, ubnd_srcptr g) { guessu(f->l, g); f->p = 0; }
};

struct __unum_sqrt_function
{
  static void eval(ubnd_ptr f, ubnd_srcptr g) { sqrtu(f, g); }
};

struct __unum_hypot_function
{
};

struct __unum_sgn_function
{
};

struct __unum_gcd_function
{
};

struct __unum_lcm_function
{
};

struct __unum_rand_function
{
};

struct __unum_min_function
{
  static void eval(ubnd_ptr f, ubnd_srcptr g, ubnd_srcptr h)
  { minu(f, g, h); }

  static void eval(ubnd_ptr f, ubnd_srcptr g, unsigned long int l)
  { UB_VAR(temp); ui2ub(temp, l); minu(f, g, temp); }
  static void eval(ubnd_ptr f, unsigned long int l, ubnd_srcptr g)
  { UB_VAR(temp); ui2ub(temp, l); minu(f, temp, g);  }

  static void eval(ubnd_ptr f, ubnd_srcptr g, signed long int l)
  { UB_VAR(temp); si2ub(temp, l); minu(f, g, temp); }
  static void eval(ubnd_ptr f, signed long int l, ubnd_srcptr g)
  { UB_VAR(temp); si2ub(temp, l); minu(f, temp, g);  }

  static void eval(ubnd_ptr f, ubnd_srcptr g, double d)
  { UB_VAR(temp); d2ub(temp, d); minu(f, g, temp); }
  static void eval(ubnd_ptr f, double d, ubnd_srcptr g)
  { UB_VAR(temp); d2ub(temp, d); minu(f, temp, g);  }
};

struct __unum_max_function
{
  static void eval(ubnd_ptr f, ubnd_srcptr g, ubnd_srcptr h)
  { maxu(f, g, h); }

  static void eval(ubnd_ptr f, ubnd_srcptr g, unsigned long int l)
  { UB_VAR(temp); ui2ub(temp, l); maxu(f, g, temp); }
  static void eval(ubnd_ptr f, unsigned long int l, ubnd_srcptr g)
  { UB_VAR(temp); ui2ub(temp, l); maxu(f, temp, g);  }

  static void eval(ubnd_ptr f, ubnd_srcptr g, signed long int l)
  { UB_VAR(temp); si2ub(temp, l); maxu(f, g, temp); }
  static void eval(ubnd_ptr f, signed long int l, ubnd_srcptr g)
  { UB_VAR(temp); si2ub(temp, l); maxu(f, temp, g);  }

  static void eval(ubnd_ptr f, ubnd_srcptr g, double d)
  { UB_VAR(temp); d2ub(temp, d); maxu(f, g, temp); }
  static void eval(ubnd_ptr f, double d, ubnd_srcptr g)
  { UB_VAR(temp); d2ub(temp, d); maxu(f, temp, g);  }
};


/**************** Auxiliary classes ****************/

// general expression template class
template <class T, class U>
class __unum_expr;


#if !defined(NO_EXPRESS)

// templates for resolving expression types
template <class T>
struct __unum_resolve_ref
{
  typedef T ref_type;
};

template <class T, class U>
struct __unum_resolve_ref<__unum_expr<T, U> >
{
  typedef const __unum_expr<T, U> & ref_type;
};


template <class T, class U = T>
struct __unum_resolve_expr;

template <>
struct __unum_resolve_expr<ubnd_t>
{
  typedef ubnd_t value_type;
  typedef ubnd_ptr ptr_type;
  typedef ubnd_srcptr srcptr_type;
};

#if 0
/* NOTE: use when another type is declared */
template <>
struct __unum_resolve_expr<ubnd_t, other_t>
{
  typedef ubnd_t value_type;
};
#endif

#if __UNUMXX_USE_CXX11
namespace std {
  template <class T, class U, class V, class W>
  struct common_type <__unum_expr<T, U>, __unum_expr<V, W> >
  {
  private:
    typedef typename __unum_resolve_expr<T, V>::value_type X;
  public:
    typedef __unum_expr<X, X> type;
  };

  template <class T, class U>
  struct common_type <__unum_expr<T, U> >
  {
    typedef __unum_expr<T, T> type;
  };

#define __UNUMXX_DECLARE_COMMON_TYPE(typ) \
  template <class T, class U> \
  struct common_type <__unum_expr<T, U>, typ > \
  { \
    typedef __unum_expr<T, T> type; \
  }; \
  \
  template <class T, class U> \
  struct common_type <typ, __unum_expr<T, U> > \
  { \
    typedef __unum_expr<T, T> type; \
  }

  __UNUMXX_DECLARE_COMMON_TYPE(signed char);
  __UNUMXX_DECLARE_COMMON_TYPE(unsigned char);
  __UNUMXX_DECLARE_COMMON_TYPE(signed int);
  __UNUMXX_DECLARE_COMMON_TYPE(unsigned int);
  __UNUMXX_DECLARE_COMMON_TYPE(signed short int);
  __UNUMXX_DECLARE_COMMON_TYPE(unsigned short int);
  __UNUMXX_DECLARE_COMMON_TYPE(signed long int);
  __UNUMXX_DECLARE_COMMON_TYPE(unsigned long int);
  __UNUMXX_DECLARE_COMMON_TYPE(float);
  __UNUMXX_DECLARE_COMMON_TYPE(double);
#undef __UNUMXX_DECLARE_COMMON_TYPE
}
#endif

// classes for evaluating unary and binary expressions
template <class T, class Op>
struct __unum_unary_expr
{
  typename __unum_resolve_ref<T>::ref_type val;

  __unum_unary_expr(const T &v) : val(v) { }
private:
  __unum_unary_expr();
};

template <class T, class U, class Op>
struct __unum_binary_expr
{
  typename __unum_resolve_ref<T>::ref_type val1;
  typename __unum_resolve_ref<U>::ref_type val2;

  __unum_binary_expr(const T &v1, const U &v2) : val1(v1), val2(v2) { }
private:
  __unum_binary_expr();
};

#endif // NO_EXPRESS


/**************** Macros for in-class declarations ****************/
/* This is just repetitive code that is easier to maintain if it's written
   only once */

#if !defined(NO_EXPRESS)
#define __UNUMP_DECLARE_COMPOUND_OPERATOR(fun) \
  template <class T, class U> \
  __unum_expr<value_type, value_type> & fun(const __unum_expr<T, U> &);
#else // NO_EXPRESS
#define __UNUMP_DECLARE_COMPOUND_OPERATOR(fun) \
  __unum_expr & fun(const __unum_expr &);
#endif // NO_EXPRESS

#define __UNUMN_DECLARE_COMPOUND_OPERATOR(fun) \
  __unum_expr & fun(signed char); \
  __unum_expr & fun(unsigned char); \
  __unum_expr & fun(signed int); \
  __unum_expr & fun(unsigned int); \
  __unum_expr & fun(signed short int); \
  __unum_expr & fun(unsigned short int); \
  __unum_expr & fun(signed long int); \
  __unum_expr & fun(unsigned long int); \
  __unum_expr & fun(float); \
  __unum_expr & fun(double);

#define __UNUM_DECLARE_COMPOUND_OPERATOR(fun) \
__UNUMP_DECLARE_COMPOUND_OPERATOR(fun) \
__UNUMN_DECLARE_COMPOUND_OPERATOR(fun)

#define __UNUM_DECLARE_COMPOUND_OPERATOR_UI(fun) \
  __unum_expr & fun(mp_bitcnt_t);

#define __UNUM_DECLARE_INCREMENT_OPERATOR(fun) \
  inline __unum_expr & fun(); \
  inline __unum_expr fun(int);

#define __UNUMXX_DEFINE_ARITHMETIC_CONSTRUCTORS \
  __unum_expr(signed char c) { init_si(c); } \
  __unum_expr(unsigned char c) { init_ui(c); } \
  __unum_expr(signed int i) { init_si(i); } \
  __unum_expr(unsigned int i) { init_ui(i); } \
  __unum_expr(signed short int s) { init_si(s); } \
  __unum_expr(unsigned short int s) { init_ui(s); } \
  __unum_expr(signed long int l) { init_si(l); } \
  __unum_expr(unsigned long int l) { init_ui(l); } \
  __unum_expr(float f) { init_d(f); } \
  __unum_expr(double d) { init_d(d); }

#define __UNUMXX_DEFINE_ARITHMETIC_ASSIGNMENTS \
  __unum_expr & operator=(signed char c) { assign_si(c); return *this; } \
  __unum_expr & operator=(unsigned char c) { assign_ui(c); return *this; } \
  __unum_expr & operator=(signed int i) { assign_si(i); return *this; } \
  __unum_expr & operator=(unsigned int i) { assign_ui(i); return *this; } \
  __unum_expr & operator=(signed short int s) { assign_si(s); return *this; } \
  __unum_expr & operator=(unsigned short int s) { assign_ui(s); return *this; } \
  __unum_expr & operator=(signed long int l) { assign_si(l); return *this; } \
  __unum_expr & operator=(unsigned long int l) { assign_ui(l); return *this; } \
  __unum_expr & operator=(float f) { assign_d(f); return *this; } \
  __unum_expr & operator=(double d) { assign_d(d); return *this; }


/**************** ubnd_c -- wrapper for ubnd_t ****************/

template <>
class __unum_expr<ubnd_t, ubnd_t>
{
private:
  typedef ubnd_t value_type;
  value_type mp;

  // Helper functions used for all arithmetic types
  void assign_ui(unsigned long l) { ui2ub(mp, l); AGUESS(mp) }
  void assign_si(signed long l) { si2ub(mp, l); AGUESS(mp) }
  void assign_d(double d) { d2ub(mp, d); AGUESS(mp) }

  void init_ui(unsigned long l) { ubnd_init(mp); ui2ub(mp, l); }
  void init_si(signed long l) { ubnd_init(mp); si2ub(mp, l); }
  void init_d(double d) { ubnd_init(mp); d2ub(mp, d); }

public:
  mp_bitcnt_t get_prec() const { return NOT_USED; }

  // constructors and destructor
  __unum_expr() { ubnd_init(mp); }

  __unum_expr(const __unum_expr &f)
  { ubnd_init(mp); ubnd_copy(mp, f.mp); }
#if __UNUMXX_USE_CXX11
  __unum_expr(__unum_expr &&f)
  { *mp = *f.mp; ubnd_init(f.mp); }
#endif
#if !defined(NO_EXPRESS)
  template <class T, class U>
  __unum_expr(const __unum_expr<T, U> &expr)
  { ubnd_init(mp); __unum_set_expr(mp, expr); }
#endif

  __UNUMXX_DEFINE_ARITHMETIC_CONSTRUCTORS

  explicit __unum_expr(const char *s)
  { ubnd_init(mp); sscan_ub(s, mp); }
  explicit __unum_expr(const std::string &s)
  { ubnd_init(mp); sscan_ub(s.c_str(), mp); }

  explicit __unum_expr(unum_srcptr ub)
  { ubnd_init(mp); unum_copy(mp->l, ub); }
  explicit __unum_expr(ubnd_srcptr ub)
  { ubnd_init(mp); ubnd_copy(mp, ub); }

  ~__unum_expr() { ubnd_clear(mp); }

  void swap(__unum_expr& f) __UNUMXX_NOEXCEPT { std::swap(*mp, *f.mp); }

  // assignment operators
  __unum_expr & operator=(const __unum_expr &f)
  { ubnd_copy(mp, f.mp); AGUESS(mp) return *this; }
#if __UNUMXX_USE_CXX11
  __unum_expr & operator=(__unum_expr &&f) noexcept
  { swap(f); AGUESS(mp) return *this; }
#endif
#if !defined(NO_EXPRESS)
  template <class T, class U>
  __unum_expr<value_type, value_type> & operator=(const __unum_expr<T, U> &expr)
  { __unum_set_expr(mp, expr); AGUESS(mp) return *this; }
#endif

  __UNUMXX_DEFINE_ARITHMETIC_ASSIGNMENTS

  __unum_expr & operator=(const char *s)
  { sscan_ub(s, mp); AGUESS(mp) return *this; }
  __unum_expr & operator=(const std::string &s)
  { sscan_ub(s.c_str(), mp); AGUESS(mp) return *this; }

  // conversion functions
  //__unum_expr guess() const { UB_VAR(ub); guessu(ub->l, mp); return __unum_expr(ub); }
  ubnd_srcptr __get_mp() const { return mp; }
  ubnd_ptr __get_mp() { return mp; }
  ubnd_srcptr get_ubnd_t() const { return mp; }
  ubnd_ptr get_ubnd_t() { return mp; }

  signed long int get_si() const { return ub2si(mp); }
  unsigned long int get_ui() const { return ub2ui(mp); }
  double get_d() const { return ub2d(mp); }

  int interval() const { return (mp->p) ? 2 : inexQ(mp->l) ? 1 : 0; }
  int spans0() const { return mp->p && spanszerouQ(mp); }
  int clipl(const __unum_expr &f) { return cliplu(mp, mp, f.mp); }
  int cliph(const __unum_expr &f) { return cliphu(mp, mp, f.mp); }

#if __UNUMXX_USE_CXX11
  explicit operator bool() const
  { UB_VAR(temp); ui2ub(temp, 0); return sameuQ(mp, temp) == 0; }
  explicit operator double() const { return ub2d(mp); }
#endif

  // compound assignments
  __UNUM_DECLARE_COMPOUND_OPERATOR(operator+=)
  __UNUM_DECLARE_COMPOUND_OPERATOR(operator-=)
  __UNUM_DECLARE_COMPOUND_OPERATOR(operator*=)
  __UNUM_DECLARE_COMPOUND_OPERATOR(operator/=)

  //__UNUM_DECLARE_COMPOUND_OPERATOR_UI(operator<<=)
  //__UNUM_DECLARE_COMPOUND_OPERATOR_UI(operator>>=)

  __UNUM_DECLARE_INCREMENT_OPERATOR(operator++)
  __UNUM_DECLARE_INCREMENT_OPERATOR(operator--)
};

typedef __unum_expr<ubnd_t, ubnd_t> ubnd_c;


/**************** User-defined literals ****************/

#if __UNUMXX_USE_CXX11
inline ubnd_c operator"" _ubnd(const char* s)
{
  return ubnd_c(s);
}
#endif


/**************** I/O operators ****************/

inline std::ostream& operator<< (std::ostream &o, ubnd_srcptr f)
{
  // unsigned int n = ((esizemax+2)/3 + (fsizemax+3)/3 + 4) * 2 + 4;
  // over estimate
  unsigned int n = ((esizemax+2) + (fsizemax+3) + 4) * 2 + 4 + 100;
  char *p = static_cast<char *>(MP_ALLOC(n));
  // TODO: use asprint_ub
  sprint_ub(p, f);
  o.write(p, strlen(p));
  MP_FREE(p, n);
  return o;
}

inline std::istream& operator>> (std::istream &i, ubnd_ptr f)
{
  // TODO: extract formatted input
  return i;
}

#if !defined(NO_EXPRESS)

template <class T, class U>
inline std::ostream & operator<<(std::ostream &o, const __unum_expr<T, U> &expr)
{
  __unum_expr<T, T> const& temp(expr);
  return o << temp.__get_mp();
}

template <class T>
inline std::istream & operator>>(std::istream &i, __unum_expr<T, T> &expr)
{
  return i >> expr.__get_mp();
}

#else // NO_EXPRESS

inline std::ostream & operator<<(std::ostream &o, const ubnd_c &val)
{
  return o << val.__get_mp();
}

inline std::istream & operator>>(std::istream &i, ubnd_c &val)
{
  return i >> val.__get_mp();
}

#endif // NO_EXPRESS

#if !defined(NO_EXPRESS)

/**************** Functions for type conversion ****************/

inline void __unum_set_expr(ubnd_ptr f, const ubnd_c &g)
{
  ubnd_copy(f, g.__get_mp());
}

template <class T>
inline void __unum_set_expr(ubnd_ptr f, const __unum_expr<ubnd_t, T> &expr)
{
  expr.eval(f);
#if defined(AUTO_SPANSZERO)
  if (f->p && spanszerouQ(f)) abort();
#endif
}

/* Temporary objects */

template <class T>
class __unum_temp
{
  __unum_expr<T, T> val;
  public:
  template<class U, class V>
  __unum_temp(U const& u, V) : val (u) {}
  typename __unum_resolve_expr<T>::srcptr_type
  __get_mp() const { return val.__get_mp(); }
};

template <>
class __unum_temp <ubnd_t>
{
  ubnd_c val;
  public:
  template<class U>
  __unum_temp(U const& u, ubnd_ptr res) : val (u) {}
  ubnd_srcptr __get_mp() const { return val.__get_mp(); }
};


/**************** Specializations of __unum_expr ****************/
/* The eval() method of __unum_expr<T, U> evaluates the corresponding
   expression and assigns the result to its argument,
   as specified by the T argument.
   Compound expressions are evaluated recursively (temporaries are created
   to hold intermediate values), while for simple expressions the eval()
   method of the appropriate function object (available as the Op argument
   of either __unum_unary_expr<T, Op> or __unum_binary_expr<T, U, Op>) is
   called. */


/**************** Unary expressions ****************/
/* cases:
   - simple:   argument is mp*_c, that is, __unum_expr<T, T>
   - compound: argument is __unum_expr<T, U> (with U not equal to T) */


// simple expressions

template <class T, class Op>
class __unum_expr<T, __unum_unary_expr<__unum_expr<T, T>, Op> >
{
private:
  typedef __unum_expr<T, T> val_type;

  __unum_unary_expr<val_type, Op> expr;
public:
  explicit __unum_expr(const val_type &val) : expr(val) { }
  void eval(typename __unum_resolve_expr<T>::ptr_type p) const
  { Op::eval(p, expr.val.__get_mp()); }
  const val_type & get_val() const { return expr.val; }
  mp_bitcnt_t get_prec() const { return expr.val.get_prec(); }
};


// simple expressions, U is a built-in numerical type

template <class T, class U, class Op>
class __unum_expr<T, __unum_unary_expr<U, Op> >
{
private:
  typedef U val_type;

  __unum_unary_expr<val_type, Op> expr;
public:
  explicit __unum_expr(const val_type &val) : expr(val) { }
  void eval(typename __unum_resolve_expr<T>::ptr_type p) const
  { Op::eval(p, expr.val); }
  const val_type & get_val() const { return expr.val; }
  mp_bitcnt_t get_prec() const { return NOT_USED; }
};


// compound expressions

template <class T, class U, class Op>
class __unum_expr<T, __unum_unary_expr<__unum_expr<T, U>, Op> >
{
private:
  typedef __unum_expr<T, U> val_type;

  __unum_unary_expr<val_type, Op> expr;
public:
  explicit __unum_expr(const val_type &val) : expr(val) { }
  void eval(typename __unum_resolve_expr<T>::ptr_type p) const
  { expr.val.eval(p); Op::eval(p, p); }
  const val_type & get_val() const { return expr.val; }
  mp_bitcnt_t get_prec() const { return expr.val.get_prec(); }
};


/**************** Binary expressions ****************/
/* simple:
   - arguments are both mp*_c
   - one argument is mp*_c, one is a built-in type
   compound:
   - one is mp*_c, one is __unum_expr<T, U>
   - one is __unum_expr<T, U>, one is built-in
   - both arguments are __unum_expr<...> */


// simple expressions

template <class T, class Op>
class __unum_expr
<T, __unum_binary_expr<__unum_expr<T, T>, __unum_expr<T, T>, Op> >
{
private:
  typedef __unum_expr<T, T> val1_type;
  typedef __unum_expr<T, T> val2_type;

  __unum_binary_expr<val1_type, val2_type, Op> expr;
public:
  __unum_expr(const val1_type &val1, const val2_type &val2)
    : expr(val1, val2) { }
  void eval(typename __unum_resolve_expr<T>::ptr_type p) const
  { Op::eval(p, expr.val1.__get_mp(), expr.val2.__get_mp()); }
  const val1_type & get_val1() const { return expr.val1; }
  const val2_type & get_val2() const { return expr.val2; }
  mp_bitcnt_t get_prec() const
  {
    mp_bitcnt_t prec1 = expr.val1.get_prec(),
      prec2 = expr.val2.get_prec();
    return (prec1 > prec2) ? prec1 : prec2;
  }
};


// simple expressions, U is a built-in numerical type

template <class T, class U, class Op>
class __unum_expr<T, __unum_binary_expr<__unum_expr<T, T>, U, Op> >
{
private:
  typedef __unum_expr<T, T> val1_type;
  typedef U val2_type;

  __unum_binary_expr<val1_type, val2_type, Op> expr;
public:
  __unum_expr(const val1_type &val1, const val2_type &val2)
    : expr(val1, val2) { }
  void eval(typename __unum_resolve_expr<T>::ptr_type p) const
  { Op::eval(p, expr.val1.__get_mp(), expr.val2); }
  const val1_type & get_val1() const { return expr.val1; }
  const val2_type & get_val2() const { return expr.val2; }
  mp_bitcnt_t get_prec() const { return expr.val1.get_prec(); }
};

template <class T, class U, class Op>
class __unum_expr<T, __unum_binary_expr<U, __unum_expr<T, T>, Op> >
{
private:
  typedef U val1_type;
  typedef __unum_expr<T, T> val2_type;

  __unum_binary_expr<val1_type, val2_type, Op> expr;
public:
  __unum_expr(const val1_type &val1, const val2_type &val2)
    : expr(val1, val2) { }
  void eval(typename __unum_resolve_expr<T>::ptr_type p) const
  { Op::eval(p, expr.val1, expr.val2.__get_mp()); }
  const val1_type & get_val1() const { return expr.val1; }
  const val2_type & get_val2() const { return expr.val2; }
  mp_bitcnt_t get_prec() const { return expr.val2.get_prec(); }
};


// compound expressions, one argument is a subexpression

template <class T, class U, class V, class Op>
class __unum_expr
<T, __unum_binary_expr<__unum_expr<T, T>, __unum_expr<U, V>, Op> >
{
private:
  typedef __unum_expr<T, T> val1_type;
  typedef __unum_expr<U, V> val2_type;

  __unum_binary_expr<val1_type, val2_type, Op> expr;
public:
  __unum_expr(const val1_type &val1, const val2_type &val2)
    : expr(val1, val2) { }
  void eval(typename __unum_resolve_expr<T>::ptr_type p) const
  {
    if(p != expr.val1.__get_mp())
    {
      __unum_set_expr(p, expr.val2);
      Op::eval(p, expr.val1.__get_mp(), p);
    }
    else
    {
      __unum_temp<T> temp(expr.val2, p);
      Op::eval(p, expr.val1.__get_mp(), temp.__get_mp());
    }
  }
  const val1_type & get_val1() const { return expr.val1; }
  const val2_type & get_val2() const { return expr.val2; }
  mp_bitcnt_t get_prec() const
  {
    mp_bitcnt_t prec1 = expr.val1.get_prec(),
      prec2 = expr.val2.get_prec();
    return (prec1 > prec2) ? prec1 : prec2;
  }
};

template <class T, class U, class V, class Op>
class __unum_expr
<T, __unum_binary_expr<__unum_expr<U, V>, __unum_expr<T, T>, Op> >
{
private:
  typedef __unum_expr<U, V> val1_type;
  typedef __unum_expr<T, T> val2_type;

  __unum_binary_expr<val1_type, val2_type, Op> expr;
public:
  __unum_expr(const val1_type &val1, const val2_type &val2)
    : expr(val1, val2) { }
  void eval(typename __unum_resolve_expr<T>::ptr_type p) const
  {
    if(p != expr.val2.__get_mp())
    {
      __unum_set_expr(p, expr.val1);
      Op::eval(p, p, expr.val2.__get_mp());
    }
    else
    {
      __unum_temp<T> temp(expr.val1, p);
      Op::eval(p, temp.__get_mp(), expr.val2.__get_mp());
    }
  }
  const val1_type & get_val1() const { return expr.val1; }
  const val2_type & get_val2() const { return expr.val2; }
  mp_bitcnt_t get_prec() const
  {
    mp_bitcnt_t prec1 = expr.val1.get_prec(),
      prec2 = expr.val2.get_prec();
    return (prec1 > prec2) ? prec1 : prec2;
  }
};

template <class T, class U, class Op>
class __unum_expr
<T, __unum_binary_expr<__unum_expr<T, T>, __unum_expr<T, U>, Op> >
{
private:
  typedef __unum_expr<T, T> val1_type;
  typedef __unum_expr<T, U> val2_type;

  __unum_binary_expr<val1_type, val2_type, Op> expr;
public:
  __unum_expr(const val1_type &val1, const val2_type &val2)
    : expr(val1, val2) { }
  void eval(typename __unum_resolve_expr<T>::ptr_type p) const
  {
    if(p != expr.val1.__get_mp())
    {
      __unum_set_expr(p, expr.val2);
      Op::eval(p, expr.val1.__get_mp(), p);
    }
    else
    {
      __unum_temp<T> temp(expr.val2, p);
      Op::eval(p, expr.val1.__get_mp(), temp.__get_mp());
    }
  }
  const val1_type & get_val1() const { return expr.val1; }
  const val2_type & get_val2() const { return expr.val2; }
  mp_bitcnt_t get_prec() const
  {
    mp_bitcnt_t prec1 = expr.val1.get_prec(),
      prec2 = expr.val2.get_prec();
    return (prec1 > prec2) ? prec1 : prec2;
  }
};

template <class T, class U, class Op>
class __unum_expr
<T, __unum_binary_expr<__unum_expr<T, U>, __unum_expr<T, T>, Op> >
{
private:
  typedef __unum_expr<T, U> val1_type;
  typedef __unum_expr<T, T> val2_type;

  __unum_binary_expr<val1_type, val2_type, Op> expr;
public:
  __unum_expr(const val1_type &val1, const val2_type &val2)
    : expr(val1, val2) { }
  void eval(typename __unum_resolve_expr<T>::ptr_type p) const
  {
    if(p != expr.val2.__get_mp())
    {
      __unum_set_expr(p, expr.val1);
      Op::eval(p, p, expr.val2.__get_mp());
    }
    else
    {
      __unum_temp<T> temp(expr.val1, p);
      Op::eval(p, temp.__get_mp(), expr.val2.__get_mp());
    }
  }
  const val1_type & get_val1() const { return expr.val1; }
  const val2_type & get_val2() const { return expr.val2; }
  mp_bitcnt_t get_prec() const
  {
    mp_bitcnt_t prec1 = expr.val1.get_prec(),
      prec2 = expr.val2.get_prec();
    return (prec1 > prec2) ? prec1 : prec2;
  }
};


// one argument is a subexpression, one is a built-in

template <class T, class U, class V, class Op>
class __unum_expr<T, __unum_binary_expr<__unum_expr<T, U>, V, Op> >
{
private:
  typedef __unum_expr<T, U> val1_type;
  typedef V val2_type;

  __unum_binary_expr<val1_type, val2_type, Op> expr;
public:
  __unum_expr(const val1_type &val1, const val2_type &val2)
    : expr(val1, val2) { }
  void eval(typename __unum_resolve_expr<T>::ptr_type p) const
  {
    expr.val1.eval(p);
    Op::eval(p, p, expr.val2);
  }
  const val1_type & get_val1() const { return expr.val1; }
  const val2_type & get_val2() const { return expr.val2; }
  mp_bitcnt_t get_prec() const { return expr.val1.get_prec(); }
};

template <class T, class U, class V, class Op>
class __unum_expr<T, __unum_binary_expr<U, __unum_expr<T, V>, Op> >
{
private:
  typedef U val1_type;
  typedef __unum_expr<T, V> val2_type;

  __unum_binary_expr<val1_type, val2_type, Op> expr;
public:
  __unum_expr(const val1_type &val1, const val2_type &val2)
    : expr(val1, val2) { }
  void eval(typename __unum_resolve_expr<T>::ptr_type p) const
  {
    expr.val2.eval(p);
    Op::eval(p, expr.val1, p);
  }
  const val1_type & get_val1() const { return expr.val1; }
  const val2_type & get_val2() const { return expr.val2; }
  mp_bitcnt_t get_prec() const { return expr.val2.get_prec(); }
};


// both arguments are subexpressions

template <class T, class U, class V, class W, class Op>
class __unum_expr
<T, __unum_binary_expr<__unum_expr<T, U>, __unum_expr<V, W>, Op> >
{
private:
  typedef __unum_expr<T, U> val1_type;
  typedef __unum_expr<V, W> val2_type;

  __unum_binary_expr<val1_type, val2_type, Op> expr;
public:
  __unum_expr(const val1_type &val1, const val2_type &val2)
    : expr(val1, val2) { }
  void eval(typename __unum_resolve_expr<T>::ptr_type p) const
  {
    __unum_temp<T> temp2(expr.val2, p);
    expr.val1.eval(p);
    Op::eval(p, p, temp2.__get_mp());
  }
  const val1_type & get_val1() const { return expr.val1; }
  const val2_type & get_val2() const { return expr.val2; }
  mp_bitcnt_t get_prec() const
  {
    mp_bitcnt_t prec1 = expr.val1.get_prec(),
      prec2 = expr.val2.get_prec();
    return (prec1 > prec2) ? prec1 : prec2;
  }
};

template <class T, class U, class V, class W, class Op>
class __unum_expr
<T, __unum_binary_expr<__unum_expr<U, V>, __unum_expr<T, W>, Op> >
{
private:
  typedef __unum_expr<U, V> val1_type;
  typedef __unum_expr<T, W> val2_type;

  __unum_binary_expr<val1_type, val2_type, Op> expr;
public:
  __unum_expr(const val1_type &val1, const val2_type &val2)
    : expr(val1, val2) { }
  void eval(typename __unum_resolve_expr<T>::ptr_type p) const
  {
    __unum_temp<T> temp1(expr.val1, p);
    expr.val2.eval(p);
    Op::eval(p, temp1.__get_mp(), p);
  }
  const val1_type & get_val1() const { return expr.val1; }
  const val2_type & get_val2() const { return expr.val2; }
  mp_bitcnt_t get_prec() const
  {
    mp_bitcnt_t prec1 = expr.val1.get_prec(),
      prec2 = expr.val2.get_prec();
    return (prec1 > prec2) ? prec1 : prec2;
  }
};

template <class T, class U, class V, class Op>
class __unum_expr
<T, __unum_binary_expr<__unum_expr<T, U>, __unum_expr<T, V>, Op> >
{
private:
  typedef __unum_expr<T, U> val1_type;
  typedef __unum_expr<T, V> val2_type;

  __unum_binary_expr<val1_type, val2_type, Op> expr;
public:
  __unum_expr(const val1_type &val1, const val2_type &val2)
    : expr(val1, val2) { }
  void eval(typename __unum_resolve_expr<T>::ptr_type p) const
  {
    __unum_temp<T> temp2(expr.val2, p);
    expr.val1.eval(p);
    Op::eval(p, p, temp2.__get_mp());
  }
  const val1_type & get_val1() const { return expr.val1; }
  const val2_type & get_val2() const { return expr.val2; }
  mp_bitcnt_t get_prec() const
  {
    mp_bitcnt_t prec1 = expr.val1.get_prec(),
      prec2 = expr.val2.get_prec();
    return (prec1 > prec2) ? prec1 : prec2;
  }
};

#endif // NO_EXPRESS


/**************** Macros for defining functions ****************/
/* Results of operators and functions are instances of __unum_expr<T, U>.
   T determines the numerical type of the expression.
   When the arguments of a binary
   expression have different numerical types, __unum_resolve_expr is used
   to determine the "larger" type.
   U is either __unum_unary_expr<V, Op> or __unum_binary_expr<V, W, Op>,
   where V and W are the arguments' types -- they can in turn be
   expressions, thus allowing to build compound expressions to any
   degree of complexity.
   Op is a function object that must have an eval() method accepting
   appropriate arguments.
   Actual evaluation of a __unum_expr<T, U> object is done when it gets
   assigned to an mp*_c ("lazy" evaluation): this is done by calling
   its eval() method. */


// non-member unary operators and functions

#if !defined(NO_EXPRESS)

#define __UNUM_DEFINE_UNARY_FUNCTION(fun, eval_fun) \
 \
template <class T, class U> \
inline __unum_expr<T, __unum_unary_expr<__unum_expr<T, U>, eval_fun> > \
fun(const __unum_expr<T, U> &expr) \
{ \
  return __unum_expr<T, __unum_unary_expr<__unum_expr<T, U>, eval_fun> >(expr); \
}

#else // NO_EXPRESS

#define __UNUM_DEFINE_UNARY_FUNCTION(fun, eval_fun) \
inline const ubnd_c fun(const ubnd_c &op) \
{ \
  ubnd_c res; \
  eval_fun::eval(res.__get_mp(), op.__get_mp()); \
  return res; \
}

#endif // NO_EXPRESS

#if !defined(NO_EXPRESS)

#define __UNUM_DEFINE_UNARY_TYPE_FUNCTION(type, fun, eval_fun) \
 \
template <class T, class U> \
inline type fun(const __unum_expr<T, U> &expr) \
{ \
  __unum_expr<T, T> const& temp(expr); \
  return eval_fun::eval(temp.__get_mp()); \
}

#else // NO_EXPRESS

#define __UNUM_DEFINE_UNARY_TYPE_FUNCTION(type, fun, eval_fun) \
inline type fun(const ubnd_c &op) \
{ \
  return eval_fun::eval(op.__get_mp()); \
}

#endif // NO_EXPRESS


// non-member binary operators and functions

#if !defined(NO_EXPRESS)

#define __UNUMP_DEFINE_BINARY_FUNCTION(fun, eval_fun) \
 \
template <class T, class U, class V, class W> \
inline __unum_expr<typename __unum_resolve_expr<T, V>::value_type, \
__unum_binary_expr<__unum_expr<T, U>, __unum_expr<V, W>, eval_fun> > \
fun(const __unum_expr<T, U> &expr1, const __unum_expr<V, W> &expr2) \
{ \
  return __unum_expr<typename __unum_resolve_expr<T, V>::value_type, \
     __unum_binary_expr<__unum_expr<T, U>, __unum_expr<V, W>, eval_fun> > \
    (expr1, expr2); \
}

#define __UNUMNN_DEFINE_BINARY_FUNCTION(fun, eval_fun, type, bigtype) \
 \
template <class T, class U> \
inline __unum_expr \
<T, __unum_binary_expr<__unum_expr<T, U>, bigtype, eval_fun> > \
fun(const __unum_expr<T, U> &expr, type t) \
{ \
  return __unum_expr \
    <T, __unum_binary_expr<__unum_expr<T, U>, bigtype, eval_fun> >(expr, t); \
} \
 \
template <class T, class U> \
inline __unum_expr \
<T, __unum_binary_expr<bigtype, __unum_expr<T, U>, eval_fun> > \
fun(type t, const __unum_expr<T, U> &expr) \
{ \
  return __unum_expr \
    <T, __unum_binary_expr<bigtype, __unum_expr<T, U>, eval_fun> >(t, expr); \
}

#else // NO_EXPRESS

#define __UNUMP_DEFINE_BINARY_FUNCTION(fun, eval_fun) \
inline const ubnd_c fun(const ubnd_c &op1, const ubnd_c &op2) \
{ \
  ubnd_c res; \
  eval_fun::eval(res.__get_mp(), op1.__get_mp(), op2.__get_mp()); \
  return res; \
}

#define __UNUMNN_DEFINE_BINARY_FUNCTION(fun, eval_fun, type, bigtype) \
inline const ubnd_c fun(const ubnd_c &op1, type op2) \
{ \
  ubnd_c res; \
  eval_fun::eval(res.__get_mp(), op1.__get_mp(), static_cast<bigtype>(op2)); \
  return res; \
} \
inline const ubnd_c fun(type op1, const ubnd_c &op2) \
{ \
  ubnd_c res; \
  eval_fun::eval(res.__get_mp(), static_cast<bigtype>(op1), op2.__get_mp()); \
  return res; \
}

#endif // NO_EXPRESS

#define __UNUMNS_DEFINE_BINARY_FUNCTION(fun, eval_fun, type) \
__UNUMNN_DEFINE_BINARY_FUNCTION(fun, eval_fun, type, signed long int)

#define __UNUMNU_DEFINE_BINARY_FUNCTION(fun, eval_fun, type) \
__UNUMNN_DEFINE_BINARY_FUNCTION(fun, eval_fun, type, unsigned long int)

#define __UNUMND_DEFINE_BINARY_FUNCTION(fun, eval_fun, type) \
__UNUMNN_DEFINE_BINARY_FUNCTION(fun, eval_fun, type, double)

#define __UNUMN_DEFINE_BINARY_FUNCTION(fun, eval_fun) \
__UNUMNS_DEFINE_BINARY_FUNCTION(fun, eval_fun, signed char) \
__UNUMNU_DEFINE_BINARY_FUNCTION(fun, eval_fun, unsigned char) \
__UNUMNS_DEFINE_BINARY_FUNCTION(fun, eval_fun, signed int) \
__UNUMNU_DEFINE_BINARY_FUNCTION(fun, eval_fun, unsigned int) \
__UNUMNS_DEFINE_BINARY_FUNCTION(fun, eval_fun, signed short int) \
__UNUMNU_DEFINE_BINARY_FUNCTION(fun, eval_fun, unsigned short int) \
__UNUMNS_DEFINE_BINARY_FUNCTION(fun, eval_fun, signed long int) \
__UNUMNU_DEFINE_BINARY_FUNCTION(fun, eval_fun, unsigned long int) \
__UNUMND_DEFINE_BINARY_FUNCTION(fun, eval_fun, float) \
__UNUMND_DEFINE_BINARY_FUNCTION(fun, eval_fun, double)

#define __UNUM_DEFINE_BINARY_FUNCTION(fun, eval_fun) \
__UNUMP_DEFINE_BINARY_FUNCTION(fun, eval_fun) \
__UNUMN_DEFINE_BINARY_FUNCTION(fun, eval_fun)

#define __UNUM_DEFINE_BINARY_FUNCTION_UI(fun, eval_fun) \
 \
template <class T, class U> \
inline __unum_expr \
<T, __unum_binary_expr<__unum_expr<T, U>, mp_bitcnt_t, eval_fun> > \
fun(const __unum_expr<T, U> &expr, mp_bitcnt_t l) \
{ \
  return __unum_expr<T, __unum_binary_expr \
    <__unum_expr<T, U>, mp_bitcnt_t, eval_fun> >(expr, l); \
}

#if !defined(NO_EXPRESS)

#define __UNUMP_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun) \
 \
template <class T, class U, class V, class W> \
inline type fun(const __unum_expr<T, U> &expr1, \
const __unum_expr<V, W> &expr2) \
{ \
  __unum_expr<T, T> const& temp1(expr1); \
  __unum_expr<V, V> const& temp2(expr2); \
  return eval_fun::eval(temp1.__get_mp(), temp2.__get_mp()); \
}

#define __UNUMNN_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, \
type2, bigtype) \
 \
template <class T, class U> \
inline type fun(const __unum_expr<T, U> &expr, type2 t) \
{ \
  __unum_expr<T, T> const& temp(expr); \
  return eval_fun::eval(temp.__get_mp(), static_cast<bigtype>(t)); \
} \
 \
template <class T, class U> \
inline type fun(type2 t, const __unum_expr<T, U> &expr) \
{ \
  __unum_expr<T, T> const& temp(expr); \
  return eval_fun::eval(static_cast<bigtype>(t), temp.__get_mp()); \
}

#else // NO_EXPRESS

#define __UNUMP_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun) \
inline type fun(const ubnd_c &op1, const ubnd_c &op2) \
{ \
  return eval_fun::eval(op1.__get_mp(), op2.__get_mp()); \
}

#define __UNUMNN_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, \
type2, bigtype) \
inline type fun(const ubnd_c &op1, type2 op2) \
{ \
  return eval_fun::eval(op1.__get_mp(), static_cast<bigtype>(op2)); \
} \
inline type fun(type2 op1, const ubnd_c &op2) \
{ \
  return eval_fun::eval(static_cast<bigtype>(op1), op2.__get_mp()); \
}

#endif // NO_EXPRESS

#define __UNUMNS_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, type2) \
__UNUMNN_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, \
type2, signed long int)

#define __UNUMNU_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, type2) \
__UNUMNN_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, \
type2, unsigned long int)

#define __UNUMND_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, type2) \
__UNUMNN_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, type2, double)

#define __UNUMN_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun) \
__UNUMNS_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, signed char) \
__UNUMNU_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, unsigned char) \
__UNUMNS_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, signed int) \
__UNUMNU_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, unsigned int) \
__UNUMNS_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, signed short int) \
__UNUMNU_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, unsigned short int) \
__UNUMNS_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, signed long int) \
__UNUMNU_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, unsigned long int) \
__UNUMND_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, float) \
__UNUMND_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun, double)

#define __UNUM_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun) \
__UNUMP_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun) \
__UNUMN_DEFINE_BINARY_TYPE_FUNCTION(type, fun, eval_fun)


// member operators

#if !defined(NO_EXPRESS)

#define __UNUMP_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun) \
 \
template <class T, class U> \
inline type##_c & type##_c::fun(const __unum_expr<T, U> &expr) \
{ \
  __unum_set_expr(mp, __unum_expr<type##_t, __unum_binary_expr \
		 <type##_c, __unum_expr<T, U>, eval_fun> >(*this, expr)); \
  AGUESS(mp) return *this; \
}

#define __UNUMNN_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, \
type2, bigtype) \
 \
inline type##_c & type##_c::fun(type2 t) \
{ \
  __unum_set_expr(mp, __unum_expr<type##_t, __unum_binary_expr \
		 <type##_c, bigtype, eval_fun> >(*this, t)); \
  AGUESS(mp) return *this; \
}

#else // NO_EXPRESS

#define __UNUMP_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun) \
inline type##_c & type##_c::fun(const type##_c &op) \
{ \
  eval_fun::eval(mp, mp, op.__get_mp()); \
  AGUESS(mp) return *this; \
}

#define __UNUMNN_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, \
type2, bigtype) \
inline type##_c & type##_c::fun(type2 op) \
{ \
  eval_fun::eval(mp, mp, static_cast<bigtype>(op)); \
  AGUESS(mp) return *this; \
}

#endif // NO_EXPRESS

#define __UNUMNS_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, type2) \
__UNUMNN_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, \
type2, signed long int)

#define __UNUMNU_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, type2) \
__UNUMNN_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, \
type2, unsigned long int)

#define __UNUMND_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, type2) \
__UNUMNN_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, type2, double)

#define __UNUMN_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun) \
__UNUMNS_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, signed char) \
__UNUMNU_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, unsigned char) \
__UNUMNS_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, signed int) \
__UNUMNU_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, unsigned int) \
__UNUMNS_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, signed short int) \
__UNUMNU_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, unsigned short int) \
__UNUMNS_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, signed long int) \
__UNUMNU_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, unsigned long int) \
__UNUMND_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, float) \
__UNUMND_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun, double)

#define __UNUM_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun) \
__UNUMP_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun) \
__UNUMN_DEFINE_COMPOUND_OPERATOR(type, fun, eval_fun)

#define __UNUMUB_DEFINE_COMPOUND_OPERATOR(fun, eval_fun) \
__UNUM_DEFINE_COMPOUND_OPERATOR(ubnd, fun, eval_fun)

#define __UNUM_DEFINE_COMPOUND_OPERATOR_UI(type, fun, eval_fun) \
 \
inline type##_c & type##_c::fun(mp_bitcnt_t l) \
{ \
  __unum_set_expr(mp, __unum_expr<type##_t, __unum_binary_expr \
    <type##_c, mp_bitcnt_t, eval_fun> >(*this, l)); \
  return *this; \
}

#define __UNUMUB_DEFINE_COMPOUND_OPERATOR_UI(fun, eval_fun) \
__UNUM_DEFINE_COMPOUND_OPERATOR_UI(ubnd, fun, eval_fun)

#define __UNUM_DEFINE_INCREMENT_OPERATOR(type, fun, eval_fun) \
 \
inline type##_c & type##_c::fun() \
{ \
  eval_fun::eval(mp); \
  return *this; \
} \
 \
inline type##_c type##_c::fun(int) \
{ \
  type##_c temp(*this); \
  eval_fun::eval(mp); \
  return temp; \
}

#define __UNUMUB_DEFINE_INCREMENT_OPERATOR(fun, eval_fun) \
__UNUM_DEFINE_INCREMENT_OPERATOR(ubnd, fun, eval_fun)


/**************** Arithmetic operators and functions ****************/

// non-member operators and functions

__UNUM_DEFINE_UNARY_FUNCTION(operator+, __unum_unary_plus)
__UNUM_DEFINE_UNARY_FUNCTION(operator-, __unum_unary_minus)
//__UNUM_DEFINE_UNARY_FUNCTION(operator~, __unum_unary_com)

__UNUM_DEFINE_BINARY_FUNCTION(operator+, __unum_binary_plus)
__UNUM_DEFINE_BINARY_FUNCTION(operator-, __unum_binary_minus)
__UNUM_DEFINE_BINARY_FUNCTION(operator*, __unum_binary_multiplies)
__UNUM_DEFINE_BINARY_FUNCTION(operator/, __unum_binary_divides)
//__UNUM_DEFINE_BINARY_FUNCTION(operator%, __unum_binary_modulus)
//__UNUM_DEFINE_BINARY_FUNCTION(operator&, __unum_binary_and)
//__UNUM_DEFINE_BINARY_FUNCTION(operator|, __unum_binary_ior)
//__UNUM_DEFINE_BINARY_FUNCTION(operator^, __unum_binary_xor)

//__UNUM_DEFINE_BINARY_FUNCTION_UI(operator<<, __unum_binary_lshift)
//__UNUM_DEFINE_BINARY_FUNCTION_UI(operator>>, __unum_binary_rshift)

__UNUM_DEFINE_BINARY_TYPE_FUNCTION(bool, operator==, __unum_binary_equal)
__UNUM_DEFINE_BINARY_TYPE_FUNCTION(bool, operator!=, ! __unum_binary_equal)
__UNUM_DEFINE_BINARY_TYPE_FUNCTION(bool, operator<, __unum_binary_less)
__UNUM_DEFINE_BINARY_TYPE_FUNCTION(bool, operator<=, ! __unum_binary_greater)
__UNUM_DEFINE_BINARY_TYPE_FUNCTION(bool, operator>, __unum_binary_greater)
__UNUM_DEFINE_BINARY_TYPE_FUNCTION(bool, operator>=, ! __unum_binary_less)
__UNUM_DEFINE_BINARY_TYPE_FUNCTION(bool, neq, __unum_binary_nequal)
__UNUM_DEFINE_BINARY_TYPE_FUNCTION(bool, seq, ! __unum_binary_nequal)

__UNUM_DEFINE_UNARY_FUNCTION(abs, __unum_abs_function)
//__UNUM_DEFINE_UNARY_FUNCTION(trunc, __unum_trunc_function)
//__UNUM_DEFINE_UNARY_FUNCTION(floor, __unum_floor_function)
//__UNUM_DEFINE_UNARY_FUNCTION(ceil, __unum_ceil_function)
__UNUM_DEFINE_UNARY_FUNCTION(guess, __unum_guess_function)
__UNUM_DEFINE_UNARY_FUNCTION(sqrt, __unum_sqrt_function)
//__UNUM_DEFINE_BINARY_FUNCTION(hypot, __unum_hypot_function)
//__UNUM_DEFINE_BINARY_FUNCTION(gcd, __unum_gcd_function)
//__UNUM_DEFINE_BINARY_FUNCTION(lcm, __unum_lcm_function)
__UNUM_DEFINE_BINARY_FUNCTION(min, __unum_min_function)
__UNUM_DEFINE_BINARY_FUNCTION(max, __unum_max_function)

//__UNUM_DEFINE_UNARY_TYPE_FUNCTION(int, sgn, __unum_sgn_function)
//__UNUM_DEFINE_BINARY_TYPE_FUNCTION(int, cmp, __unum_cmp_function)

template <class T>
void swap(__unum_expr<T, T>& x, __unum_expr<T, T>& y) __UNUMXX_NOEXCEPT
{ x.swap(y); }

inline int cmpe(const ubnd_c &op1, end_t e1, const ubnd_c &op2, end_t e2)
{ return cmpuQ(op1.__get_mp(), e1, op2.__get_mp(), e2); }

// member operators for ubnd_c

__UNUMUB_DEFINE_COMPOUND_OPERATOR(operator+=, __unum_binary_plus)
__UNUMUB_DEFINE_COMPOUND_OPERATOR(operator-=, __unum_binary_minus)
__UNUMUB_DEFINE_COMPOUND_OPERATOR(operator*=, __unum_binary_multiplies)
__UNUMUB_DEFINE_COMPOUND_OPERATOR(operator/=, __unum_binary_divides)

//__UNUMUB_DEFINE_COMPOUND_OPERATOR_UI(operator<<=, __unum_binary_lshift)
//__UNUMUB_DEFINE_COMPOUND_OPERATOR_UI(operator>>=, __unum_binary_rshift)

__UNUMUB_DEFINE_INCREMENT_OPERATOR(operator++, __unum_unary_increment)
__UNUMUB_DEFINE_INCREMENT_OPERATOR(operator--, __unum_unary_decrement)


/**************** Specialize std::numeric_limits ****************/

namespace std {
  template <> class numeric_limits<ubnd_c>
  {
  public:
    static const bool is_specialized = true;
    static ubnd_c min() { return ubnd_c(smallnormalu); }
    static ubnd_c max() { return ubnd_c(maxrealu); }
    static ubnd_c lowest() { return ubnd_c(minrealu); }
    static const int digits = 0; // TODO:
    static const int digits10 = 0; // TODO:
    static const int max_digits10 = 0; // TODO:
    static const bool is_signed = true;
    static const bool is_integer = false;
    static const bool is_exact = false;
    static const int radix = 2;
    static ubnd_c epsilon() { return ubnd_c(); } // TODO:
    static ubnd_c round_error() { return ubnd_c(); }
    static const int min_exponent = 0; // TODO:
    static const int min_exponent10 = 0; // TODO:
    static const int max_exponent = 0; // TODO:
    static const int max_exponent10 = 0; // TODO:
    static const bool has_infinity = true;
    static const bool has_quiet_NaN = true;
    static const bool has_signaling_NaN = true;
    static const float_denorm_style has_denorm = denorm_present;
    static const bool has_denorm_loss = false;
    static ubnd_c infinity() { return ubnd_c(posinfu); }
    static ubnd_c quiet_NaN() { return ubnd_c(qNaNu); }
    static ubnd_c signaling_NaN() { return ubnd_c(sNaNu); }
    static ubnd_c denorm_min() { return ubnd_c(smallsubnormalu); }
    static const bool is_iec559 = false;
    static const bool is_bounded = true;
    static const bool is_modulo = false;
    static const bool traps = false;
    static const bool tinyness_before = false;
    static const float_round_style round_style = round_indeterminate;
  };
}


/**************** #undef all private macros ****************/

#undef __UNUMP_DECLARE_COMPOUND_OPERATOR
#undef __UNUMN_DECLARE_COMPOUND_OPERATOR
#undef __UNUM_DECLARE_COMPOUND_OPERATOR
#undef __UNUM_DECLARE_COMPOUND_OPERATOR_UI
#undef __UNUM_DECLARE_INCREMENT_OPERATOR
#undef __UNUMXX_DEFINE_ARITHMETIC_CONSTRUCTORS
#undef __UNUMXX_DEFINE_ARITHMETIC_ASSIGNMENTS

#undef __UNUM_DEFINE_UNARY_FUNCTION
#undef __UNUM_DEFINE_UNARY_TYPE_FUNCTION

#undef __UNUMP_DEFINE_BINARY_FUNCTION
#undef __UNUMNN_DEFINE_BINARY_FUNCTION
#undef __UNUMNS_DEFINE_BINARY_FUNCTION
#undef __UNUMNU_DEFINE_BINARY_FUNCTION
#undef __UNUMND_DEFINE_BINARY_FUNCTION
#undef __UNUMNLD_DEFINE_BINARY_FUNCTION
#undef __UNUMN_DEFINE_BINARY_FUNCTION
#undef __UNUM_DEFINE_BINARY_FUNCTION

#undef __UNUM_DEFINE_BINARY_FUNCTION_UI

#undef __UNUMP_DEFINE_BINARY_TYPE_FUNCTION
#undef __UNUMNN_DEFINE_BINARY_TYPE_FUNCTION
#undef __UNUMNS_DEFINE_BINARY_TYPE_FUNCTION
#undef __UNUMNU_DEFINE_BINARY_TYPE_FUNCTION
#undef __UNUMND_DEFINE_BINARY_TYPE_FUNCTION
#undef __UNUMNLD_DEFINE_BINARY_TYPE_FUNCTION
#undef __UNUMN_DEFINE_BINARY_TYPE_FUNCTION
#undef __UNUM_DEFINE_BINARY_TYPE_FUNCTION

#undef __UNUMP_DEFINE_COMPOUND_OPERATOR
#undef __UNUMNN_DEFINE_COMPOUND_OPERATOR
#undef __UNUMNS_DEFINE_COMPOUND_OPERATOR
#undef __UNUMNU_DEFINE_COMPOUND_OPERATOR
#undef __UNUMND_DEFINE_COMPOUND_OPERATOR
#undef __UNUMNLD_DEFINE_COMPOUND_OPERATOR
#undef __UNUMN_DEFINE_COMPOUND_OPERATOR
#undef __UNUM_DEFINE_COMPOUND_OPERATOR

#undef __UNUMUB_DEFINE_COMPOUND_OPERATOR

#undef __UNUM_DEFINE_COMPOUND_OPERATOR_UI
#undef __UNUMUB_DEFINE_COMPOUND_OPERATOR_UI

#undef __UNUM_DEFINE_INCREMENT_OPERATOR
#undef __UNUMUB_DEFINE_INCREMENT_OPERATOR

#undef __UNUMXX_CONSTANT_TRUE
#undef __UNUMXX_CONSTANT

#endif /* __UNUM_PLUSPLUS__ */
