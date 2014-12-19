/**
 * @file Bdf.cpp
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

#include "Bdf.hh"

#define STRSEP "\"\n"
#define SEP " \t\n\r"

static
unsigned char hexi(char c)
{
  if (c <= '9')
    return (c - '0');
  else
    if (c <= 'F')
      return (c - 'A' + 10);
    else
      return (c - 'a' + 10);
}

void
Bdf::read(std::ifstream& input)
{
    char linebuf[256];
    char *s;
    Glyphs::index_t glyph_count;
    unsigned int in_bitmap;
    unsigned int bitmap_line;

    
    for (;;)
      {
        if (input.eof())
          break;

        input.getline(linebuf, sizeof(linebuf));

        //        std::cout << "LINE " << linebuf << std::endl;
 
        if (!(s = strtok(linebuf, SEP)))
          {
            /* Empty Line */
            continue;
          }

        if (!strcasecmp(s, "COMMENT"))
          {
#if 0
            char* p = strtok(NULL, "\"");
            if (p)
              {
                m_COMMENTS += p;
                m_COMMENTS += "\n";
              }
#endif
          }
        else if (!strcasecmp(s, "FONT"))
          strncpy(m_FONT, strtok(NULL, STRSEP), FONT_LEN);
        else if (!strcasecmp(s, "FONTBOUNDINGBOX"))
          {
            m_bb_width = atoi(strtok(NULL, SEP));
            m_bb_height = atoi(strtok(NULL, SEP));
          }
        else if (!strcasecmp(s, "FOUNDRY"))
          strncpy(m_FOUNDRY, strtok(NULL, STRSEP), FOUNDRY_LEN);
        else if (!strcasecmp(s, "FAMILY_NAME"))
          strncpy(m_FAMILY_NAME, strtok(NULL, STRSEP), FAMILY_NAME_LEN);
        else if (!strcasecmp(s, "WEIGHT_NAME"))
          strncpy(m_WEIGHT_NAME, strtok(NULL, STRSEP), WEIGHT_NAME_LEN);
        else if (!strcasecmp(s, "SLANT"))
          {
            char *p = strtok(NULL, SEP);
            if (p)
              switch (*p)
                {
                  case 'r':
                  case 'R':
                    m_SLANT = ROMAN;
                    break;

                  case 'i':
                  case 'I':
                    m_SLANT = ITALIC;
                    break;

                  case 'o':
                  case 'O':
                    m_SLANT = OBLIQUE;
                    break;
                }
            else
              m_SLANT = ROMAN;
          }
        else if (!strcasecmp(s, "SETWIDTH_NAME"))
          strncpy(m_SETWIDTH_NAME, strtok(NULL, STRSEP), SETWIDTH_NAME_LEN);
        else if (!strcasecmp(s, "SPACING"))
          {
            char *p = strtok(NULL, SEP);
            if (p)
              switch (*p)
                {
                  case 'c':
                  case 'C':
                    m_SPACING = CELL;
                    break;

                  case 'm':
                  case 'M':
                    m_SPACING = MONOSPACE;
                    break;

                  case 'p':
                  case 'P':
                    m_SPACING = PROPORTIONAL;
                    break;
                }
            else
              m_SPACING = CELL;
          }
        else if (!strcasecmp(s, "COPYRIGHT"))
          strncpy(m_COPYRIGHT, strtok(NULL, STRSEP), COPYRIGHT_LEN);
        else if (!strcasecmp(s, "CHARS"))
          {
            m_CHARS = atoi(strtok(NULL, SEP));
            break;
          }
      }

    if (!m_bb_width || !m_bb_height || !m_CHARS)
      {
        std::cerr << "Bdf file doesn't contain FONTBOUNDINGBOX or CHARS" << std::endl;
        exit(-1);
      }

    if (m_SPACING == PROPORTIONAL)
      {
        std::cerr << "Proportional spacing not supported" << std::endl;
        exit(-1);
      }

    m_glyphs.count(m_CHARS);
 
    // bitmap bytes are in row order, width rounded to next byte

    glyph_count = 0;
    in_bitmap = 0;
    bitmap_line = 0;

    for (;;)
      {
        if (input.eof())
          break;

        input.getline(linebuf, sizeof(linebuf));

        //        std::cout << "count " << glyph_count << " LINE " << linebuf << std::endl;

        if (!(s = strtok(linebuf, SEP)))
          {
            /* Empty Line */
            continue;
          }

        if (glyph_count > m_CHARS+1)
          {
            std::cerr << "Too many bitmaps!" << glyph_count << " " <<  m_CHARS << std::endl;
            exit(-1);
          }

        if (!strcasecmp(s, "STARTCHAR"))
          m_glyphs.glyph(glyph_count).name(strtok(NULL, SEP));
        else if (!strcasecmp(s, "ENCODING"))
          {
            m_glyphs.glyph(glyph_count).encoding(atoi(strtok(NULL, SEP)));
            if (m_glyphs.glyph(glyph_count).encoding() > MAX_ENCODINGS)
              {
                std::cerr << "Encoding" << m_glyphs.glyph(glyph_count).encoding()
                     << " does not fit. " << MAX_ENCODINGS << std::endl;
                exit(-1);
              }
            m_encodings[m_glyphs.glyph(glyph_count).encoding()] = glyph_count;
          }
        else if (!strcasecmp(s, "BBX"))
          {
            m_glyphs.glyph(glyph_count).width(atoi(strtok(NULL, SEP)));
            m_glyphs.glyph(glyph_count).height(atoi(strtok(NULL, SEP)));
            m_glyphs.glyph(glyph_count).x_offset(atoi(strtok(NULL, SEP)));
            m_glyphs.glyph(glyph_count).y_offset(atoi(strtok(NULL, SEP)));

            if (m_glyphs.glyph(glyph_count).width() > m_max_glyph_width)
              m_max_glyph_width = m_glyphs.glyph(glyph_count).width();
            if (m_glyphs.glyph(glyph_count).height() > m_max_glyph_height)
              m_max_glyph_height = m_glyphs.glyph(glyph_count).height();
          }
        else if (!strcasecmp(s, "BITMAP"))
          {
            if (!m_glyphs.glyph(glyph_count).width())
              {
                std::cerr << "Character width for \"" << m_glyphs.glyph(glyph_count).name()
                          << "\" not specified." << std::endl;
                exit(-1);
              }

            if (m_glyphs.glyph(glyph_count).x_offset() < 0)
              {
                m_glyphs.glyph(glyph_count).width(m_glyphs.glyph(glyph_count).width()-m_glyphs.glyph(glyph_count).x_offset());
                m_glyphs.glyph(glyph_count).x_offset(0);
              }
            if (m_glyphs.glyph(glyph_count).x_offset() + m_glyphs.glyph(glyph_count).width() > m_bb_width)
              m_bb_width = m_glyphs.glyph(glyph_count).x_offset() + m_glyphs.glyph(glyph_count).width();

            m_glyphs.glyph(glyph_count).bitmap(&m_bitmap, m_bitmap.count());

            in_bitmap = 1;
          }
        else if (!strcasecmp(s, "ENDCHAR"))
          {
            in_bitmap = 0;
            bitmap_line = 0;

            if (glyph_count == 0)
              m_first = m_glyphs.glyph(glyph_count).encoding();

            glyph_count++;
          }
        else if (in_bitmap)
          {
            char *p = s;
            unsigned char i;

            s[BITS_TO_BYTES(m_bb_width)*2] = 0;  // clamp

            while (*p) {
              i = hexi(*(p++));
              if (*p)
                i = (i << 4) | hexi(*(p++));
              m_bitmap.append(i);
            }

            bitmap_line++;
          }
      }

    m_last = m_glyphs.glyph(glyph_count - 1).encoding();

    if (glyph_count != m_CHARS)
      {
        std::cerr << "Not enough bitmaps!" << std::endl;
        exit(-1);
      }
}
