#  @file Makefile
#  @version 1.0
#
#  @section License
#  Copyright (C) 2014-2015, jeditekunum
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.

CC = g++
CFLAGS	=	-Wall -g
LDFLAGS	=
LIBS	=

SRCS=bdf2uc.cpp Bdf.cpp Glyph.cpp
OBJS=$(SRCS:.cpp=.o)
EXEC=bdf2uc

HDRS	=	Bdf.hh Glyphs.hh Glyph.hh Bitmap.hh
FILES	=	Makefile

all: $(SRCS) $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LIBS)

$(OBJS): $(SRCS) $(HDRS) Makefile

.cpp.o:
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	-rm -f $(EXEC) *.o *~
