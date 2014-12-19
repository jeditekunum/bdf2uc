/**
 * @file Glyph.hh
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

#ifndef GLYPH_HH
#define GLYPH_HH

#include <string.h>

#include <iostream>
#include <fstream>

#include "Bitmap.hh"

#define BITS_TO_BYTES(w) ((w + 7) / 8)


class Glyph
{
public:
  typedef unsigned int encoding_t;
  typedef unsigned int dimension_t;
  typedef int offset_t;

#if 0
  enum rotation_t {
    NONE = 0,
    CLOCKWISE_90 = 90,
    CLOCKWISE_180 = 180,
    CLOCKWISE_270 = 270,
    COUNTERCLOCKWISE_90 = -90,
    COUNTERCLOCKWISE_180 = -180,
    COUNTERCLOCKWISE_270 = -270
  };
#endif

  Glyph()
#if 0
    : m_rotation(NONE)
#endif
  {
    invalidate();
  }

  void invalidate() {
    m_name_valid = false;
    m_encoding_valid = false;
    m_width_valid = false;
    m_height_valid = false;
    m_x_offset_valid = false;
    m_y_offset_valid = false;
    m_bitmap_valid = false;
  }

  bool valid() { return(m_name_valid && m_encoding_valid && m_width_valid && m_height_valid &&
                        m_x_offset_valid && m_y_offset_valid && m_bitmap_valid); }
    
  const char* name() { return (m_name); }
  encoding_t encoding() { return (m_encoding); }
  dimension_t width() { return (m_width); }
  dimension_t height() { return (m_height); }
  offset_t x_offset() { return (m_x_offset); }
  offset_t y_offset() { return (m_y_offset); }
  Bitmap::index_t bitmap_index() { return (m_bitmap_index); }

  void name(char* n)            { strncpy(m_name, n, GLYPH_NAME_LEN); m_name_valid = true; }
  void encoding(encoding_t e)   { m_encoding = e; m_encoding_valid = true; }
  void width(dimension_t d)     { m_width = d; m_width_valid = true; }
  void height(dimension_t d)    { m_height = d; m_height_valid = true; }
  void x_offset(offset_t o)     { m_x_offset = o; m_x_offset_valid = true; }
  void y_offset(offset_t o)     { m_y_offset = o; m_y_offset_valid = true; }
  void bitmap(Bitmap* bm, Bitmap::index_t i) { m_bitmap = bm; m_bitmap_index = i; m_bitmap_valid = true; }

#if 0
  rotation_t rotation() { return (m_rotation); }

  void rotate(rotation_t r);
#endif

  void print(std::ostream& outs, const char* leading_ornament = NULL, const char* trailing_ornament = NULL);

  void dump(std::ostream& outs)
  {
    if (valid())
      {
        outs << "glyph"
             << " name=" << m_name
             << " encoding=" << m_encoding << " 0x" << std::hex << m_encoding << std::dec;
        if (isprint(m_encoding))
          outs << " '" << (char)m_encoding << "'";

        outs   << " m_width=" << m_width
               << " m_height=" << m_height
               << " m_x_offset=" << m_x_offset
               << " m_y_offset=" << m_y_offset
               << " m_bitmap_index=" << m_bitmap_index << std::endl;
      }
    else
      outs << "glyph not valid" << std::endl;
  }

  friend std::ostream& operator<<(std::ostream& outs, Glyph& glyph)
  {
    if (glyph.valid())
      glyph.print(outs);
    return (outs);
  }

protected:
#if 0
  rotation_t m_rotation;
#endif

  static const unsigned int GLYPH_NAME_LEN = 32;

  char m_name[GLYPH_NAME_LEN+1];
  bool m_name_valid;

  encoding_t m_encoding;
  bool m_encoding_valid;
  dimension_t m_width;
  bool m_width_valid;
  dimension_t m_height;
  bool m_height_valid;
  offset_t m_x_offset;
  bool m_x_offset_valid;
  offset_t m_y_offset;
  bool m_y_offset_valid;
  Bitmap *m_bitmap;
  Bitmap::index_t m_bitmap_index;
  bool m_bitmap_valid;

private:
#if 0
  void hlshift();
  void hrshift();
#endif
};

#endif
