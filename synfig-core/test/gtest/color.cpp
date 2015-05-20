/* === S Y N F I G ========================================================= */
/*!	\file
**	\brief Tests for the color class
**
**	$Id$
**
**	\legal
**	Copyright (c) 2015 Diego Barrios Romero
**
**	This package is free software; you can redistribute it and/or
**	modify it under the terms of the GNU General Public License as
**	published by the Free Software Foundation; either version 2 of
**	the License, or (at your option) any later version.
**
**	This package is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
**	General Public License for more details.
**	\endlegal
*/
/* ========================================================================= */

#ifdef USING_PCH
  #include "pch.h"
#else
  #ifdef HAVE_CONFIG_H
    #include <config.h>
  #endif

  #include <gtest/gtest.h>
  #include <synfig/color/color.h>
#endif

TEST(Color, CanCreateColor)
{
  synfig::Color c;
}

