/**
 * @file Bdf.hh
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

#ifndef BDF_HH
#define BDF_HH

#include <iostream>
#include <fstream>

#include "Glyphs.hh"
#include "Bitmap.hh"

#define INVALID_ENCODING 999999

class Bdf
{
public:
  enum slant_t
    {
      ROMAN = 0,
      ITALIC,
      OBLIQUE
    };

  enum spacing_t
    {
      CELL = 0,
      MONOSPACE,
      PROPORTIONAL
    };

  Bdf()
  {
    for (unsigned int i = 0; i< MAX_ENCODINGS; i++)
      m_encodings[i] = INVALID_ENCODING;
    m_COMMENTS = "";
  }

  void read(std::ifstream& input);

  std::string& comments() { return (m_COMMENTS); }
  const char* font() { return (m_FONT); }
  Glyph::dimension_t bb_width() { return (m_bb_width); }
  Glyph::dimension_t bb_height() { return (m_bb_height); }
  const char* foundry() { return (m_FOUNDRY); }
  const char* family_name() { return (m_FAMILY_NAME); }
  const char* weight_name() { return (m_WEIGHT_NAME); }
  slant_t slant() { return (m_SLANT); }
  const char* setwidth_name() { return (m_SETWIDTH_NAME); }
  spacing_t spacing() { return (m_SPACING); }
  const char* copyright() { return (m_COPYRIGHT); }
  unsigned int chars() { return (m_CHARS); }
  Glyph::dimension_t max_glyph_width() { return (m_max_glyph_width); }
  Glyph::dimension_t max_glyph_height() { return (m_max_glyph_height); }

  void font(char* s) { strncpy(m_FONT, s, FONT_LEN); }
  void bb_width(Glyph::dimension_t d) { m_bb_width = d; }
  void bb_height(Glyph::dimension_t d) { m_bb_height = d; }
  void foundry(char* s) { strncpy(m_FOUNDRY, s, FOUNDRY_LEN); }
  void family_name(char* s) { strncpy(m_FAMILY_NAME, s, FAMILY_NAME_LEN); }
  void weight_name(char* s) { strncpy(m_WEIGHT_NAME, s, WEIGHT_NAME_LEN); }
  void slant(slant_t s) { m_SLANT = s; }
  void setwidth_name(char* s) { strncpy(m_SETWIDTH_NAME, s, SETWIDTH_NAME_LEN); }
  void spacing(spacing_t s) { m_SPACING = s; }
  void copyright(char* s) { strncpy(m_COPYRIGHT, s, COPYRIGHT_LEN); }
  void chars(unsigned int c) { m_CHARS = c; }
  void max_glyph_width(Glyph::dimension_t d) { m_max_glyph_width = d; }
  void max_glyph_height(Glyph::dimension_t d) { m_max_glyph_height = d; }

  Bitmap& bitmap() { return (m_bitmap); }

  Glyph::encoding_t first() { return (m_first); }
  Glyph::encoding_t last() { return (m_last); }

  Glyph& glyph(Glyph::encoding_t en)
  {
    if (m_encodings[en] != INVALID_ENCODING)
      return (m_glyphs.glyph(m_encodings[en]));
    else
      return (m_invalid_glyph);
  }

  Glyphs& glyphs() { return (m_glyphs); }

  void dump(std::ostream& outs)
  {
    outs << "FONT=" << m_FONT << std::endl
         << "bb_width=" << m_bb_width << " (" << BITS_TO_BYTES(m_bb_width) << ")" << std::endl
         << "bb_height=" << m_bb_height << std::endl
         << "FOUNDRY=" << m_FOUNDRY << std::endl
         << "FAMILY_NAME=" << m_FAMILY_NAME << std::endl
         << "WEIGHT_NAME=" << m_WEIGHT_NAME << std::endl
         << "SLANT=" ;
    switch (m_SLANT)
      {
      case ROMAN:
        outs << "ROMAIN";
        break;
      case ITALIC:
        outs << "ITALIC";
        break;
      case OBLIQUE:
        outs << "OBLIQUE";
        break;
    }
    outs << std::endl;
    outs << "SETWIDTH_NAME=" << m_SETWIDTH_NAME << std::endl
         << "SPACING=" ;
    switch (m_SPACING)
      {
      case CELL:
        outs << "CELL";
        break;
      case MONOSPACE:
        outs << "MONOSPACE";
        break;
      case PROPORTIONAL:
        outs << "PROPORTIONAL";
        break;
    }
    outs << std::endl;
    outs << "COPYRIGHT=" << m_COPYRIGHT << std::endl
         << "CHARS=" << m_CHARS << std::endl
         << "max_glyph_width=" << m_max_glyph_width << std::endl
         << "max_glyph_height=" << m_max_glyph_height << std::endl
      ;

    //    outs << "bitmaps=" << static_cast<void*>(m_bitmaps) << std::endl;
  }

  friend std::ostream& operator<<(std::ostream& outs, Bdf& bdf)
  {
    outs << bdf.m_glyphs;

    return (outs);
  }

protected:
  static const unsigned int MAX_ENCODINGS = 256;

  static const unsigned int FONT_LEN = 96;
  static const unsigned int FOUNDRY_LEN = 16;
  static const unsigned int FAMILY_NAME_LEN = 16;
  static const unsigned int WEIGHT_NAME_LEN = 16;
  static const unsigned int SETWIDTH_NAME_LEN = 16;
  static const unsigned int COPYRIGHT_LEN = 96;

  std::string m_COMMENTS;
  char m_FONT[FONT_LEN+1];
  Glyph::dimension_t m_bb_width;
  Glyph::dimension_t m_bb_height;
  char m_FOUNDRY[FOUNDRY_LEN+1];
  char m_FAMILY_NAME[FAMILY_NAME_LEN+1];
  char m_WEIGHT_NAME[WEIGHT_NAME_LEN+1];
  slant_t m_SLANT;
  char m_SETWIDTH_NAME[SETWIDTH_NAME_LEN+1];
  spacing_t m_SPACING;
  char m_COPYRIGHT[COPYRIGHT_LEN+1];
  unsigned int m_CHARS;

  Glyph::dimension_t m_max_glyph_width;
  Glyph::dimension_t m_max_glyph_height;

  Glyph::encoding_t m_first;
  Glyph::encoding_t m_last;

  Glyphs m_glyphs;
  int m_encodings[MAX_ENCODINGS];

  Glyph m_invalid_glyph;

  Bitmap m_bitmap;
};
  
#endif
