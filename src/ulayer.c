/*
 * Copyright (c) 2016, Lawrence Livermore National Security, LLC. 
 * Produced at the Lawrence Livermore National Laboratory. Written by
 * G. Scott Lloyd, lloyd23@llnl.gov. LLNL-CODE-704762. All rights reserved.
 * 
 * This file is part of Unum. For details, see
 * http://github.com/LLNL/unum
 * 
 * Please also read COPYING – Our Notice and GNU Lesser General Public 
 * License. 
 * 
 * This program is free software; you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License (as published by the 
 * Free Software Foundation) version 2.1 dated February 1999. 
 * 
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the IMPLIED WARRANTY OF 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the terms and 
 * conditions of the GNU General Public License for more details. 
 * 
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this program; if not, write to the Free Software Foundation, 
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 */

#include "ulayer.h"
#include "uenv.h" /* UBITS */

void unum_init(unum_s *un)
{
	mpx_init2(un, UBITS);
}

void unum_clear(unum_s *un)
{
	mpx_clear(un);
}

void unum_copy(unum_s *dst, const unum_s *src)
{
	mpx_set(dst, src);
}

void ubnd_init(ubnd_s *ub)
{
	/* TODO: change type of ub->l & r to unum_t? */
#ifndef USE_MPN
	ub->l = (unum_s *)MP_ALLOC(sizeof(unum_s));
	ub->r = (unum_s *)MP_ALLOC(sizeof(unum_s));
#endif
	ub->p = 0;
	mpx_init2(ub->l, UBITS);
	mpx_init2(ub->r, UBITS);
}

void ubnd_clear(ubnd_s *ub)
{
	/* TODO: change type of ub->l & r to unum_t? */
	mpx_clear(ub->l);
	mpx_clear(ub->r);
#ifndef USE_MPN
	MP_FREE(ub->l, sizeof(unum_s));
	MP_FREE(ub->r, sizeof(unum_s));
#endif
}

void ubnd_copy(ubnd_s *dst, const ubnd_s *src)
{
	dst->p = src->p;
	mpx_set(dst->l, src->l);
	mpx_set(dst->r, src->r);
}
