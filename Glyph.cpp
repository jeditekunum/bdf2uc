/**
 * @file Glyph.cpp
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2014-2015, jeditekunum
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

#include "Glyph.hh"

#if 0
static
Bitmap::byte_t reverse(Bitmap::byte_t b)
{
  Bitmap::byte_t t = 0;

  if (!b)
    return (b);

  for (unsigned int i=0; i<sizeof(Bitmap::byte_t)*8; i++)
    {
      //      std::cout << "b=" << (std::bitset<8>)b << "  t=" << (std::bitset<8>)t
      //                << "  i=" << i << "  b>>i=" << ((b >> i) & 0x1) << std::endl;
      t <<= 1;
      t |= (b >> i) & 0x1;
    }

  return (t);
}

void
Glyph::hlshift()
{
  unsigned int bits = BITS_TO_BYTES(m_width)*8 - m_width;
  Bitmap::index_t bi;
  Bitmap::byte_t work;
  Bitmap::byte_t save, next_save;
  unsigned int rb;

  //  std::cout << "bits=" << bits << std::endl;

  if (!bits)
    return;

  save = 0;

  for (bi = (m_bitmap_index + BITS_TO_BYTES(m_width)*m_height)-1, rb = BITS_TO_BYTES(m_width)-1;
       bi >= m_bitmap_index;
       bi--, rb = (rb == 0) ? (BITS_TO_BYTES(m_width)-1) : (rb-1))
    {
      work = m_bitmap->byte(bi);
      //      std::cout << "bi=" << bi << "  rb=" << rb << "  " << (std::bitset<8>)work;
      next_save = work >> (8-bits);
      //      std::cout << "  |=" << (std::bitset<8>)save;
      work = (work << bits) | save ;
      m_bitmap->byte(bi, work);
      //      std::cout << "  ->  " << (std::bitset<8>)work;
      //      std::cout << "  next|=" << (std::bitset<8>)next_save << std::endl;
      if (rb == 0)
        {
          //          std::cout << std::endl;
          save = 0;
        }
      else
        save = next_save;
    }
}

void
Glyph::hrshift()
{
  unsigned int bits = BITS_TO_BYTES(m_width)*8 - m_width;
  Bitmap::index_t bi;
  Bitmap::byte_t work;
  Bitmap::byte_t save, next_save;
  unsigned int rb;

  //  std::cout << "bits=" << bits << std::endl;

  if (!bits)
    return;

  save = 0;

  for (bi = m_bitmap_index, rb = 0;
       bi < (m_bitmap_index + BITS_TO_BYTES(m_width)*m_height);
       bi++, rb = (rb+1) % BITS_TO_BYTES(m_width))
    {
      work = m_bitmap->byte(bi);
      //      std::cout << "bi=" << bi << "  rb=" << rb << "  " << (std::bitset<8>)work;
      next_save = work << bits;
      //      std::cout << "  |=" << (std::bitset<8>)save;
      work = (work >> (8-bits)) | save ;
      m_bitmap->byte(bi, work);
      //      std::cout << "  ->  " << (std::bitset<8>)work;
      //      std::cout << "  next|=" << (std::bitset<8>)next_save << std::endl;
      if (rb == BITS_TO_BYTES(m_width)-1)
        save = 0;
      else
        save = next_save;
    }
}

void
Glyph::rotate(rotation_t r)
{
  int before = (int)m_rotation;
  int after = ((int)m_rotation + (int)r) % 360;
  int difference = after - before;

  dimension_t w = m_width;
  dimension_t h = m_height;
  Bitmap::byte_t* nbm = new Bitmap::byte_t [ w * h ];

  Bitmap::index_t bi;
  int i;


  if (!difference)
    return;

  std::cout << "before=" << before
            << " after=" << after
            << " difference=" << difference
            << std::endl;

  if (abs(difference) == 180)
    {
      // up/down flip

      for (bi = m_bitmap_index, i = (BITS_TO_BYTES(w)*h)-1; i>=0; bi++, i--)
        {
          //          std::cout << "bi=" << bi << " i=" << i << std::endl;
          nbm[i] = reverse(m_bitmap->byte(bi));
        }
    }
  else
    {
      if (difference < 0)
        {
          // left 90
          m_width = h;
          m_height = w;
        }

      if (difference > 0)
        {
          // right 90
          m_width = h;
          m_height = w;
        }
    }

  //  std::cout << "--------------------------" << std::endl;

  for (bi = m_bitmap_index, i = 0; i<(BITS_TO_BYTES(w)*h); bi++, i++)
    {
      //      std::cout << "bi=" << bi << " i=" << i << std::endl;
      m_bitmap->byte(bi, nbm[i]);
    }

  //  delete nbm;

  std::cout << *this;
  if (abs(difference) == 180)
    hlshift();

  m_rotation = (rotation_t)after;
}
#endif

void
Glyph::print(std::ostream& outs, const char* leading_ornament, const char* trailing_ornament)
{
  unsigned int bitmap_offset = 0; /* within glyph */


  if (leading_ornament)
    outs << leading_ornament;

  outs.put('+');
  for (dimension_t i = 0; i < m_width; i++)
    outs.put('-');
  outs.put('+');
  if (trailing_ornament)
    outs << trailing_ornament;
  outs.put('\n');

  for (dimension_t r = 0; r < m_height; r++)
    {
      dimension_t columns = m_width;

      if (leading_ornament)
        outs << leading_ornament;
      outs.put('|');
      while (columns)
        {
          unsigned int nbits = (columns < 8) ? columns : 8;
          Bitmap::byte_t byte = m_input_bitmap.byte(bitmap_offset);

          for (unsigned int bits = 0; bits < nbits; bits++, columns--)
            {
              if ((byte >> (7-bits)) & 0x1)
                outs.put('@');
              else
                outs.put('.');
            }
          bitmap_offset++;
        }
      outs.put('|');
      if (trailing_ornament)
        outs << trailing_ornament;
      outs.put('\n');
    }

  if (leading_ornament)
    outs << leading_ornament;
  outs.put('+');
  for (dimension_t i = 0; i < m_width; i++)
    outs.put('-');
  outs.put('+');
  if (trailing_ornament)
    outs << trailing_ornament;
  outs.put('\n');
}

void
Glyph::finish_input()
{
  m_blank = false;
  for (unsigned int i = 0; i < m_input_bitmap.count(); i++)
    if (m_input_bitmap.byte(i) != 0)
      return;
  m_blank = true;
}
