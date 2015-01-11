/**
 * @file Glyphs.hh
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014, jediunix
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 */

#ifndef GLYPHS_HH
#define GLYPHS_HH

#include <stdio.h>
#include <iostream>
#include <assert.h>

#include "Glyph.hh"

class Glyphs
{
public:
  typedef unsigned int index_t;

  Glyphs() :
    m_count(0)
  {
  }

  void count(index_t count)
  {
    if (m_count == 0)
      {
        m_glyphs = new Glyph [count];
        m_count = count;
      }
  }

  Glyph& glyph(index_t index)
  {
    assert(index < m_count);
    return (m_glyphs[index]);
  }

  void dump(std::ostream& outs)
  {
    for (unsigned int i = 0; i < m_count; i++)
      m_glyphs[i].dump(outs);
  }

  friend std::ostream& operator<<(std::ostream& outs, Glyphs& glyphs)
  {
    for (unsigned int i = 0; i < glyphs.m_count; i++)
      outs << glyphs.m_glyphs[i];

    return (outs);
  }

protected:
  index_t m_count;
  Glyph* m_glyphs;
};

#endif
