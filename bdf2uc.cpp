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
#include <iomanip>
#include <fstream>

#include "Bdf.hh"

Bdf bdf;

bool verbose = false;
bool compress = false;

unsigned int total_converted = 0;
unsigned int total_compressed = 0;


int
percent(int l, int r)
{
  return ((int)(((l - r) / (float)r) * 100.0));
}

void
describe(std::ostream& out, Glyph::encoding_t i)
{
  if (isprint(i))
    out << "'" << (char)i << "'    ";
  else
    {
      switch (i)
        {
        case '\a':
          out << "'\\a'   ";
          break;
        case '\b':
          out << "'\\b'   ";
          break;
        case '\f':
          out << "'\\f'   ";
          break;
        case '\n':
          out << "'\\n'   ";
          break;
        case '\r':
          out << "'\\r'   ";
          break;
        case '\t':
          out << "'\\t'   ";
          break;
        case '\v':
          out << "'\\v'   ";
          break;

        default:
          out << "'\\";
          if (i < 8)
            out << "0";
          if (i < 64)
            out << "0";
          out << std::oct << (int)i <<  "' " << std::dec;
        }
    }
  out << "(";
  if (i < 10)
    out << " ";
  if (i < 100)
    out << " ";
  out << i;
  out << ", 0x";
  if (i < 16)
    out << "0";
  out << std::hex << i << std::dec;
  out << ")";
}

void
output_hdr(std::ofstream& out, Bdf& bdf)
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

void
convert_glyph(Bdf& bdf, Glyph::encoding_t i)
{
  if (bdf.glyph(i).converted_count())
    return;  // must be invalid glyph

  if(bdf.glyph(i).valid())
    {
      // Cosa Font
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

                  unsigned short in_offset =
                    yb * (BITS_TO_BYTES(bdf.bb_width()) * 8) +
                    BITS_TO_BYTES(x+1) - 1 +
                    bit * BITS_TO_BYTES(bdf.bb_width());
                  //                  out << " in_offset=" << in_offset;

                  unsigned short right_shift = 7 - x % 8;
                  //                  out << " rs=" << right_shift;

                  
                  Bitmap::byte_t data = bdf.glyph(i).input_byte(in_offset);
                  //                  out << " data=0x" << std::hex << (uint16_t)data << std::dec
                  //                      << " (0b" << (std::bitset<8>)data << ")";

                  byte |= ((data >> right_shift) & 0x1) << bit;
                  //                  out << " byte=0b" << (std::bitset<8>)byte << std::endl;
                }
              bdf.glyph(i).append_converted_byte(byte);
            }
        }
    }
  else
    {
      if (verbose)
        std::cerr << "WARNING: Missing glyph for " << "0x" << std::hex << (i>>4) << (i&0xF) << std::dec << std::endl;

      unsigned int amount = bdf.bb_width() * BITS_TO_BYTES(bdf.bb_height());
      for (unsigned int j=0; j<amount; j++)
        bdf.glyph(i).append_converted_byte(0);
    }
  total_converted += bdf.glyph(i).converted_count();
}

void
compress_glyph(Bdf& bdf, Glyph::encoding_t i)
{
  if (!compress)
    return;

  if (bdf.glyph(i).compressed_count())
    return;  // must be invalid glyph

#if 0
  // Pre-compression

  std::cerr << "pre ";
  for (unsigned int j=0; j<bdf.glyph(i).converted_count(); j++)
    std::cerr << "0x"
              << std::hex << (bdf.glyph(i).converted_byte(j)>>4)
              << (bdf.glyph(i).converted_byte(j) & 0xF)
              << std::dec << ",";
  std::cerr << std::endl;
#endif

  unsigned int scan = 0;

  while (scan < bdf.glyph(i).converted_count())
    {
      if (bdf.glyph(i).converted_byte(scan) == 0)
        {
          unsigned int compressor = 0;

          while (scan < bdf.glyph(i).converted_count() &&
                 bdf.glyph(i).converted_byte(scan) == 0 &&
                 compressor < 256)
            {
              compressor++;
              scan++;
            }

          assert(compressor);

          bdf.glyph(i).append_compressed_byte(0);
          compressor--;
          bdf.glyph(i).append_compressed_byte(compressor);
        }
      else
        {
          bdf.glyph(i).append_compressed_byte(bdf.glyph(i).converted_byte(scan));
          scan++;
        }
    }
  total_compressed += bdf.glyph(i).compressed_count();
}

// Atmega2560 interprets 0x21 0x21 0x21 sequence as an escape
static unsigned int hex21 = 0;

void
output_glyph(std::ofstream& out, Bdf& bdf, char* class_name, Glyph::encoding_t i)
{
  static bool seen_blank = false;
  bool duplicate = false;

  out << "/* ";
  describe(out, i);

  if (compress)
    {
      if (!seen_blank)
        seen_blank = bdf.glyph(i).blank();
      else
        if (bdf.glyph(i).blank())
          duplicate = true;
    }

  if (!compress || !duplicate)
    {
      out << " offset=" << bdf.glyph(i).output_offset();
      out << " length=";
      if (compress)
        out << bdf.glyph(i).compressed_count();
      else
        out << bdf.glyph(i).converted_count();

      if (compress)
        out << " compressed="
            << -percent(bdf.glyph(i).compressed_count(), bdf.glyph(i).converted_count())
            << "%";
    }
  else
    out << " duplicate";

  if (!bdf.glyph(i).valid())
    out << " !BDF";
  out << " */" << std::endl;

  if (duplicate)
    return;  // don't output duplicate blank

  if(bdf.glyph(i).valid())
    bdf.glyph(i).print(out, "/* ", " */");

  unsigned int number;
  if (compress)
    number = bdf.glyph(i).compressed_count();
  else
    number = bdf.glyph(i).converted_count();
        
  for (unsigned int j=0; j<number; j++)
    {
      unsigned int byte;
      if (compress)
        byte = bdf.glyph(i).compressed_byte(j);
      else
        byte = bdf.glyph(i).converted_byte(j);

      if (byte == 0x21) // !
        {
          if (++hex21 == 3)
            {
              if (class_name)
                std::cerr << class_name << " ";
              std::cerr << "WARNING: 3rd 0x21 replaced with 0x41 for BOARD_ATMEGA2560" << std::endl;
              out << std::endl;
              out << "#ifndef BOARD_ATMEGA2560" << std::endl;
            }
        }
      else
        hex21 = 0;

      out << "0x" << std::hex;
      if (byte < 16)
        out << "0";
      out << byte;
      out << std::dec << ",";

      if (hex21 == 3)
        {
          out << std::endl;
          out << "#else" << std::endl;
          out << "0x41," << std::endl;
          out << "#endif" << std::endl;
          hex21 = 0;
        }
    }

  out << std::endl;
}

void
generate(std::ofstream& out, Bdf& bdf, char* class_name, Glyph::encoding_t first, Glyph::encoding_t last)
{
  out << "/* Generated by bdf2uc " << VERSION
      << " " << AUTHOR
      << " " << SOURCE
      << " */" << std::endl;

  output_hdr(out, bdf);

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
          << " bitmap_size=" << (bdf.bb_width() * BITS_TO_BYTES(bdf.bb_height())) * (last-first+1)
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
      if (compress)
        out << "const uint8_t " << class_name << "::compression_type = "
            << compress << ";" << std::endl;
      out << std::endl;

      out << "/* glyph_size=" << (bdf.bb_width() * BITS_TO_BYTES(bdf.bb_height())) << " */" << std::endl;
      if (compress)
        {
          //          out << "/* uncompressed_size=" << (bdf.bb_width() * BITS_TO_BYTES(bdf.bb_height())) * (last-first+1) << " */" << std::endl;
          out << "/* offsets_size=" << (2*(last-first+1)) << " */" << std::endl;
          out << "/* bitmap_size=" << total_compressed << " */" << std::endl;
          //          out << "/* offsets_size+bitmap_size=" << (total_compressed + (2*(last-first+1))) << " */" << std::endl;
          out << "/* compression saved " << -percent(total_compressed+(2*(last-first+1)), total_converted) << "% */" << std::endl;
        }
      else
        out << "/* bitmap_size=" << total_converted << " */" << std::endl;

      out << std::endl;

      if (compress)
        {
          // Offset of first "blank" glyph
          bool seen_blank = false;
          Glyph::encoding_t blank_en;

          // Output offsets table
          unsigned int offset = 0;

          out << "const uint16_t " << class_name << "::offsets[] __PROGMEM = {" << std::endl;

          for (Glyph::encoding_t i = first; i <= last; i++)
            {
              out << "/* ";
              describe(out, i);
              out << " */ ";

              if (bdf.glyph(i).blank())
                {
                  if (seen_blank)
                    {
                      // duplicate
                      out << std::setw(5) << bdf.glyph(blank_en).output_offset() << ",";
                      out << " /*blank*/";
                    }
                  else
                    {
                      // first
                      seen_blank = true;
                      blank_en = i;

                      bdf.glyph(i).output_offset(offset);
                      out << std::setw(5) << offset << ",";
                      offset += bdf.glyph(i).compressed_count();

                      out << " /*first blank*/";
                    }
                }
              else
                {
                  bdf.glyph(i).output_offset(offset);
                  out << std::setw(5) << offset << ",";
                  offset += bdf.glyph(i).compressed_count();
                }

              out << std::endl;
            }

          out << "};" << std::endl;
          out << std::endl;
        }

      if (compress)
        out << "const uint8_t " << class_name << "::compressed_bitmap[] __PROGMEM = {" << std::endl;
      else
        out << "const uint8_t " << class_name << "::bitmap[] __PROGMEM = {" << std::endl;
    }

  for (Glyph::encoding_t i = first; i <= last; i++)
    {
      output_glyph(out, bdf, class_name, i);
      out << std::endl;
    }

  if (class_name)
    out << "};" << std::endl;
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
    switch (getopt(argc, argv, "f:l:n:vch?-")) {
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

    case 'v':
      verbose = true;
      continue;

    case 'c':
      compress = true;
      continue;

    case EOF:
      break;

    case '?':
    case 'h':
      std::cout << "bdf2uc [option]... {bdf-input-file} {output-path-name}" << std::endl << std::endl;
      std::cout << "-f {first}          - {first} is character or hex (0xXX)" << std::endl;
      std::cout << "-l {last}           - {last} is character or hex (0xXX)" << std::endl;
      std::cout << "-n {name}           - will package for Cosa using class {name}" << std::endl;
      std::cout << "-c                  - compress" << std::endl;
      std::cout << "-v                  - verbose" << std::endl;
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

  // Check arguments

  if (optind == argc)
    {
      std::cerr << "No bdf file provided" << std::endl;
      exit(-1);
    }
  bdf_name = argv[optind++];

  if (!class_name && compress)
    {
      std::cerr << "WARNING: without name (-n) compression is disabled" << std::endl;
      compress = false;
    }

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

  // Read in BDF

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

  // Convert
  for (Glyph::encoding_t i = first; i <= last; i++)
    convert_glyph(bdf, i);

  // Compress
  if (compress)
    for (Glyph::encoding_t i = first; i <= last; i++)
      compress_glyph(bdf, i);
 
  // Verbose reporting

  if (verbose)
    for (Glyph::encoding_t i = first; i <= last; i++)
      {
        describe(std::cerr, i);
        if (compress)
        {
          if (bdf.glyph(i).converted_count() < bdf.glyph(i).compressed_count())
            std::cerr << " *** GREW ***"
                      << percent(bdf.glyph(i).compressed_count(), bdf.glyph(i).converted_count()) << "%";
          if (bdf.glyph(i).converted_count() > bdf.glyph(i).compressed_count())
            std::cerr << percent(bdf.glyph(i).converted_count(), bdf.glyph(i).compressed_count()) << "%";
        }
        std::cerr << std::endl;
      }

  // Decide if we are keeping compression

  if (compress)
    {
      // Compressing means we need a offsets table too
      unsigned int offsets_size = 2*(last-first+1);
      
      if (class_name)
        std::cerr << class_name << " ";
      std::cerr << "overall compression: ";
      if (total_converted < (total_compressed + offsets_size))
        {
          std::cerr << " *** GREW *** "
                    << -percent(total_compressed + offsets_size, total_converted)
                    << "%, cancelling compression";
          compress = false;
        }
      if (total_converted > (total_compressed + offsets_size))
        std::cerr << -percent(total_compressed + offsets_size, total_converted)
                  << "%";
      if (total_converted == (total_compressed + offsets_size))
        {
          std::cerr << "0%, cancelling compression";
          compress = false;
        }
      std::cerr << std::endl;
    }

  // Generate output

  generate(out, bdf, class_name, first, last);

  out.close();

  return (0);
}
