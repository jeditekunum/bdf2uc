/**
 * @file Bitmap.hh
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

#ifndef BITMAP_HH
#define BITMAP_HH

#include <string.h>

class Bitmap
{
public:
  typedef unsigned int  index_t;
  typedef unsigned char byte_t;

  static const index_t ALLOC_SIZE = 256;

  Bitmap() :
    m_size(0),
    m_count(0)
  {
  }

  ~Bitmap()
  {
    delete m_bytes;
  }

  index_t count()
  {
    return (m_count);
  }

  void grow(index_t size)
  {
    if (m_count > 0)
      {
        if (size > m_count)
          {
            byte_t* new_bytes = new byte_t [size];

            memset(new_bytes, 0, sizeof(*new_bytes) * size);
            memcpy(new_bytes, m_bytes, sizeof(*new_bytes) * m_size);

            delete m_bytes;
            m_bytes = new_bytes;

            m_size = size;
          }
      }
    else
      {
        m_bytes = new byte_t [size];
        memset(m_bytes, 0, sizeof(*m_bytes) * size);
        m_count = size;
      }
  }

  byte_t byte(index_t index)
  {
    if (index <= m_count)
      return (m_bytes[index]);
    else
      return (0);
  }

  void byte(index_t index, byte_t byte)
  {
    if (index <= m_count)
      m_bytes[index] = byte;
  }

  void append(byte_t byte)
  {
    if (m_count == m_size)
      grow(m_size + ALLOC_SIZE);
    m_bytes[m_count++] = byte;
  }

protected:
  index_t m_size;
  index_t m_count;
  byte_t* m_bytes;
};

#endif
