/**
 * @file bdf2uc.cpp
 * @version 1.2
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

#define VERSION "1.3"
#define AUTHOR "jediunix"
#define SOURCE "https://github.com/jediunix/bdf2uc"

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <fstream>

#include "Bdf.hh"

//#define ENABLE_COMPRESSION // experimental

Bdf bdf;

#ifdef ENABLE_COMPRESSION
bool compress = false;
#endif


void
generate_hdr(std::ofstream& out, Bdf& bdf)
{
  out << "/* BDF " << std::endl;
  
#if 0
  out << bdf.comments();
#endif
  out << "Font:\t\t" << bdf.font() << std::endl;
  out << "Foundry:\t" << bdf.foundry() << std::endl;
  out << "FamilyName:\t" << bdf.family_name() << std::endl;
  out << "WeightName:\t" << bdf.weight_name() << std::endl;
  out << "Slant:\t\t";
  switch (bdf.slant())
      {
      case Bdf::ROMAN:
        out << "R";
        break;
      case Bdf::ITALIC:
        out << "I";
        break;
      case Bdf::OBLIQUE:
        out << "O";
        break;
      }
  out << std::endl;
  out << "WidthName:\t" << bdf.setwidth_name() << std::endl;
  out << "Spacing:\t";
  switch (bdf.spacing())
    {
    case Bdf::CELL:
      out << "C";
      break;
    case Bdf::MONOSPACE:
      out << "M";
      break;
    case Bdf::PROPORTIONAL:
      out << "P";
      break;
    }
  out << std::endl;
  out << "Copyright:\t" << bdf.copyright() << std::endl;
  out << "*/" << std::endl << std::endl;

}


static unsigned int out_offset = 0;

void
generate_glyph(std::ofstream& out, Bdf& bdf, Glyph::encoding_t i)
{
  Bitmap::byte_t bytes[bdf.bb_width() * BITS_TO_BYTES(bdf.bb_height())];
  unsigned int byte_count = 0;

  out << std::endl;
  out << "/* ";
  if (isprint(i))
    out << "'" << (char)i << "' ";
  out << "(" << i << ", 0x" << std::hex << i << std::dec << ")";

  if(bdf.glyph(i).valid())
    {
      Bitmap::index_t bi_begin = bdf.glyph(i).bitmap_index();

      out << " offset " << out_offset << " */" << std::endl;

      bdf.glyph(i).print(out, "/* ", " */");

      // Cosa native font
      //      for (unsigned int x = 0; x < bdf.bb_width(); x++)
      //        {
      //          //              out << "x=" << x << std::endl;
      //
      //          for (unsigned int yb = 0; yb < BITS_TO_BYTES(bdf.bb_height()); yb++)
      //            {
      //              //                out << " yb=" << yb << std::endl;

      // Cosa GLCDFont
      for (unsigned int yb = 0; yb < BITS_TO_BYTES(bdf.bb_height()); yb++)
        {
          //          out << "yb=" << yb << std::endl;
          for (unsigned int x = 0; x < bdf.bb_width(); x++)
            {
              //              out << " x=" << x;

              unsigned int bits = bdf.bb_height() - (yb*8);
              if (bits > 8)
                bits = 8;
              //              out << ", bits=" << bits << std::endl;

              Bitmap::byte_t byte = 0;

              for (unsigned int bit = 0; bit < bits; bit++)
                {
                  //                  out << "  bit=" << bit;

                  uint16_t in_offset =
                    yb * (BITS_TO_BYTES(bdf.bb_width()) * 8) +
                    BITS_TO_BYTES(x+1) - 1 +
                    bit * BITS_TO_BYTES(bdf.bb_width());
                  //                  out << " in_offset=" << in_offset;

                  uint16_t right_shift = 7 - x % 8;
                  //                  out << " rs=" << right_shift;

                  uint8_t data = bdf.bitmap().byte(bi_begin + in_offset);
                  //                  out << " data=0x" << std::hex << (uint16_t)data << std::dec
                  //                      << " (0b" << (std::bitset<8>)data << ")";

                  byte |= ((data >> right_shift) & 0x1) << bit;
                  //                  out << " byte=0b" << (std::bitset<8>)byte << std::endl;
                }
              bytes[byte_count++] = byte;
            }
        }
    }
  else
    {
      out << " offset " << out_offset << ", is not present in BDF; blank glyph generated */" << std::endl;

      for (byte_count=0; byte_count<sizeof(bytes); byte_count++)
        bytes[byte_count] = 0;

      std::cerr << "WARNING: Missing glyph for " << "0x" << std::hex << (i>>4) << (i&0xF) << std::dec << std::endl;
    }

#ifdef ENABLE_COMPRESSION
#if 0
  if (compress)
    {
      // Pre-compression

      std::cerr << "pre ";
      for (unsigned int i=0; i<byte_count; i++)
        std::cerr << "0x" << std::hex << (bytes[i]>>4) << (bytes[i] & 0xF) << std::dec << ",";
      std::cerr << std::endl;
    }
#endif
#endif

  unsigned int scan = 0;

  while (scan < byte_count)
    {
      out << "0x" << std::hex << (bytes[scan]>>4) << (bytes[scan] & 0xF) << std::dec << ",";
      out_offset++;

#ifdef ENABLE_COMPRESSION
      if (compress && bytes[scan] == 0)
        {
          unsigned int compressor = 0;

          while (scan < byte_count && bytes[scan] == 0 && compressor < 256)
            {
              //              std::cerr << " [" << scan << "]#" << compressor << std::endl;
              compressor++;
              scan++;
            }

          if (compressor)
            {
              compressor--;

              out << compressor << ",";
              out_offset++;
            }
        }
      else
#endif
        scan++;
    }
  out << std::endl;

#ifdef ENABLE_COMPRESSION
#if 0
  if (compress)
    {
      int expected = bdf.bb_width() * BITS_TO_BYTES(bdf.bb_height());
      int got = ((int)out_offset - begin_offset);

      std::cerr << (int)(((got - expected ) / (float)expected) * 100.0) << "%" << std::endl;
    }
#endif
#endif
}

void
generate(std::ofstream& out, Bdf& bdf, char* class_name, Glyph::encoding_t first, Glyph::encoding_t last)
{
  int expected_total = (bdf.bb_width() * BITS_TO_BYTES(bdf.bb_height())) * (last-first+1);

  out << "/* Generated by bdf2uc " << VERSION
      << " " << AUTHOR
      << " " << SOURCE
      << " */" << std::endl;

  generate_hdr(out, bdf);

  out << "/* encoding format is 8 rows at a time (byte) sweeping across columns */" << std::endl;
  out << std::endl;

  if (!class_name)
    {
      out << "/*"
          << " width=" << bdf.bb_width()
          << " height=" << bdf.bb_height()
          << " first=0x" << std::hex << first << std::dec
          << " last=0x" << std::hex << last << std::dec
          << " glyph_size=" << (bdf.bb_width() * BITS_TO_BYTES(bdf.bb_height()))
          << " total_size=" << expected_total
          << " */"
          << std::endl;

      out << bdf.bb_width() << ",";
      out << bdf.bb_height() << ",";
      out << first << ",";
      out << last << ",";
      out << std::endl;
    }
  else
    {
      out << "const uint8_t " << class_name << "::width = " << bdf.bb_width() << ";" << std::endl;
      out << "const uint8_t " << class_name << "::height = " << bdf.bb_height() << ";" << std::endl;
      out << "const uint8_t " << class_name << "::first = 0x" << std::hex << first << std:: dec << ";" << std::endl;
      out << "const uint8_t " << class_name << "::last = 0x" << std::hex << last << std::dec << ";" << std::endl;
      out << std::endl;
      out << "/* glyph_size=" << (bdf.bb_width() * BITS_TO_BYTES(bdf.bb_height()))
          << " total_size=" << expected_total
          << " */"
          << std::endl;
      out << std::endl;

      out << "const uint8_t " << class_name << "::bitmap[] __PROGMEM = {" << std::endl;
    }

  for (Glyph::encoding_t i = first; i <= last; i++)
    generate_glyph(out, bdf, i);

  if (class_name)
    out << "};" << std::endl;

#ifdef ENABLE_COMPRESSION
  if (compress)
    {
      out << "/* compression "
          << (int)((((int)out_offset - expected_total) / (float) expected_total) * 100.0) << "%,"
          << " with lookup table "
          << (int)(((((int)out_offset + 128) - expected_total) / (float) expected_total) * 100.0) << "% */" << std::endl;
    }
#endif
}

int
main(int argc, char *const argv[])
{
  Glyph::encoding_t first = 0;
  Glyph::encoding_t last = 255;
  char * bdf_name;
  char *out_name;
  char *class_name = NULL;
  std::ofstream out;


  for (;;) {
    switch (getopt(argc, argv, "f:l:n:ch?-")) {
    case 'f':
      if (optarg[0] == '0' &&
          (optarg[1] == 'x' || optarg[1] == 'X'))
        first = strtoul(&optarg[2], NULL, 16);
      else
        {
          if (strlen(optarg) > 1)
            {
              std::cerr << "Argument to -f more than 1 character" << std::endl;
              exit(-1);
            }
          first = optarg[0];
        }
      continue;

    case 'l':
      if (optarg[0] == '0' &&
          (optarg[1] == 'x' || optarg[1] == 'X'))
        last = strtoul(&optarg[2], NULL, 16);
      else
        {
          if (strlen(optarg) > 1)
            {
              std::cerr << "Argument to -l more than 1 character" << std::endl;
              exit(-1);
            }
          last = optarg[0];
        }
      continue;

    case 'n':
      if (optind == argc)
        {
          std::cerr << "No class name provided" << std::endl;
          exit(-1);
        }
      class_name = optarg;
      continue;

    case 'c':
#ifdef ENABLE_COMPRESSION
      compress = true;
#else
      std::cerr << "Warning: compression is disabled" << std::endl;
#endif
      continue;

    case EOF:
      break;

    case '?':
    case 'h':
      std::cout << "bdf2uc [option]... {bdf-input-file} {output-path-name}" << std::endl << std::endl;
      std::cout << "-f {first}          - {first} is character or hex (0xXX)" << std::endl;
      std::cout << "-l {last}           - {last} is character or hex (0xXX)" << std::endl;
      std::cout << "-n {name}           - will package for Cosa using class {name}" << std::endl;
#ifdef ENABLE_COMPRESSION
      std::cout << "-c                  - compress" << std::endl;
#endif
      exit(0);

    case '-':
      std::cerr << "Long options are invalid" << std::endl;
      exit(-1);

    case ':':
      std::cerr << "Missing argument for option '" << optopt << "'" << std::endl;
      exit(-1);

    default:
      std::cerr << "Unkown option '" << optopt << "'" << std::endl;
      exit(-1);
    }

    break;
  }
  if (optind == argc)
    {
      std::cerr << "No bdf file provided" << std::endl;
      exit(-1);
    }
  bdf_name = argv[optind++];

  if (optind == argc)
    out.open("/dev/stdout");
  else
    {
      out_name = argv[optind++];
      out.open(out_name);
      if (!out.is_open())
        {
          std::cerr << "Can't open file '" << out_name << "' "
                    << strerror(errno) << std::endl;
          exit(-1);
        }
    }

  while (optind < argc) {
    std::cerr << "Ignored argument '" << argv[optind++] << "'" << std::endl;
  }

  if (first > last)
    {
      std::cerr << "-f greater than -l, swapped" << std::endl;
      unsigned int t = first;
      first = last;
      last = t;
    }

  std::ifstream input;
  input.open(bdf_name);
  if (!input.is_open())
    {
      std::cerr << "Can't open input file '" << bdf_name << "' "
                << strerror(errno) << std::endl;
      exit(-1);
    }
  bdf.read(input);
  input.close();

  generate(out, bdf, class_name, first, last);

  out.close();

  return (0);
}
