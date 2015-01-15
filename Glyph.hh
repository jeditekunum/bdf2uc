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
  }

  bool valid() { return(m_name_valid && m_encoding_valid && m_width_valid && m_height_valid &&
                        m_x_offset_valid && m_y_offset_valid); }
    
  const char* name() { return (m_name); }
  encoding_t encoding() { return (m_encoding); }
  dimension_t width() { return (m_width); }
  dimension_t height() { return (m_height); }
  offset_t x_offset() { return (m_x_offset); }
  offset_t y_offset() { return (m_y_offset); }

  unsigned int input_count() { return (m_input_bitmap.count()); }
  unsigned int converted_count() { return (m_converted_bitmap.count()); }
  unsigned int compressed_count() { return (m_compressed_bitmap.count()); }

  Bitmap::byte_t input_byte(Bitmap::index_t i) { return(m_input_bitmap.byte(i)); }
  Bitmap::byte_t converted_byte(Bitmap::index_t i) { return(m_converted_bitmap.byte(i)); }
  Bitmap::byte_t compressed_byte(Bitmap::index_t i) { return(m_compressed_bitmap.byte(i)); }

  void name(char* n)            { strncpy(m_name, n, GLYPH_NAME_LEN); m_name_valid = true; }
  void encoding(encoding_t e)   { m_encoding = e; m_encoding_valid = true; }
  void width(dimension_t d)     { m_width = d; m_width_valid = true; }
  void height(dimension_t d)    { m_height = d; m_height_valid = true; }
  void x_offset(offset_t o)     { m_x_offset = o; m_x_offset_valid = true; }
  void y_offset(offset_t o)     { m_y_offset = o; m_y_offset_valid = true; }

  void append_input_byte(Bitmap::byte_t byte) { m_input_bitmap.append(byte); }
  void append_converted_byte(Bitmap::byte_t byte) { m_converted_bitmap.append(byte); }
  void append_compressed_byte(Bitmap::byte_t byte) { m_compressed_bitmap.append(byte); }

  void finish_input();

  bool blank() { return (m_blank); }

  void escaped_bitset(bool b) { m_escaped_bitset = b; }
  bool escaped_bitset() { return (m_escaped_bitset); }

  void output_offset(unsigned int offset) { m_output_offset = offset; }
  unsigned int output_offset() { return (m_output_offset); }

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
               << " m_input_bitmap.count()=" << m_input_bitmap.count()
               << " m_converted_bitmap.count()=" << m_converted_bitmap.count()
               << " m_compressed_bitmap.count()=" << m_compressed_bitmap.count()
               << std::endl;
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

  Bitmap m_input_bitmap;
  Bitmap m_converted_bitmap;
  Bitmap m_compressed_bitmap;

  bool m_blank;
  bool m_escaped_bitset;
  unsigned int m_output_offset;

private:
#if 0
  void hlshift();
  void hrshift();
#endif
};

#endif
