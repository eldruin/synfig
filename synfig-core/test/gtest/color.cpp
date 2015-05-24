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
  #include <boost/type_traits/is_floating_point.hpp>
  #include <boost/type_traits/is_integral.hpp>
  #include <boost/utility/enable_if.hpp>
#endif

using synfig::Color;
typedef Color::value_type color_type;

class ColorTest : public testing::Test
{
protected:
  ColorTest()
    : r(1.3),  g(1.4),  b(1.5),  a(1.6),
      r2(2.3), g2(2.4), b2(2.5), a2(2.6)
  { }

  static void RGBA_EQ(const Color& current, const float r, const float g,
                                            const float b, const float a)
  {
    ASSERT_FLOAT_EQ(r, current.get_r());
    ASSERT_FLOAT_EQ(g, current.get_g());
    ASSERT_FLOAT_EQ(b, current.get_b());
    ASSERT_FLOAT_EQ(a, current.get_a());
  }

  const color_type r,  g,  b,  a,
                   r2, g2, b2, a2;
};



TEST_F(ColorTest, ByDefaultAll0)
{
  RGBA_EQ(Color(), 0, 0, 0, 0);
}

TEST_F(ColorTest, 1ValueConstructor)
{
  const color_type c(1.3);
  RGBA_EQ(Color(c), c, c, c, c);
}

TEST_F(ColorTest, 4ValueConstructor)
{
  RGBA_EQ(Color(r, g, b, a), r, g, b, a);
}

TEST_F(ColorTest, 3ValueConstructorAlphaIsDefault)
{
  RGBA_EQ(Color(r, g, b), r, g, b, 1.0f);
}

TEST_F(ColorTest, CopyConstructor)
{
  Color c(r, g, b, a);
  RGBA_EQ(Color(c), r, g, b, a);
}

TEST_F(ColorTest, CopyColorComponentsAndSeparateAlpha)
{
  Color c(r, g, b, a);
  RGBA_EQ(Color(c, a2), r, g, b, a2);
}

TEST_F(ColorTest, SetRGBA)
{
  Color c(r, g, b, a);
  c.set_r(r2);
  c.set_g(g2);
  c.set_b(b2);
  c.set_a(a2);
  RGBA_EQ(c, r2, g2, b2, a2);
}


