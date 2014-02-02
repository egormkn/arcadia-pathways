############################################################################
#
# Makefile for the libavoid and twine libraries
# - largely inspired from their original respective Makefile instructions
# - to be statically linked to by the arcadia1.0 application
#
# Copyright (C) 2007-2009 Alice Villeger, University of Manchester
# <alice.villeger@manchester.ac.uk>
#
# This library is free software; you can redistribute it and/or# modify it under the terms of the GNU Lesser General Public# License as published by the Free Software Foundation; either# version 2.1 of the License, or (at your option) any later version.## This library is distributed in the hope that it will be useful,# but WITHOUT ANY WARRANTY; without even the implied warranty of# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU# Lesser General Public License for more details.## You should have received a copy of the GNU Lesser General Public# License along with this library; if not, write to the Free Software# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
############################################################################


############################################################################
# OPTIONS
############################################################################

all: libavoid twine
install: all clean
clean: clean_libavoid clean_twine
distclean: distclean_libavoid distclean_twine
uninstall: distclean

############################################################################
# PATH
############################################################################

UTIL_DIR = .

############################################################################
# SYSTEM VARIABLE
############################################################################

copy = cp
delete = rm

ifeq ($(target), win32)
	copy = copy
endif

ifeq ($(target), win32)
	delete = del
endif

############################################################################
# LIBAVOID
############################################################################

LIBAV_DIR = $(UTIL_DIR)/libavoid
ifeq ($(target), win32)
	LIBAV_DIR = $(UTIL_DIR)\libavoid
endif

LIBAV_INC = -I$(LIBAV_DIR) -I$(LIBAV_DIR)/..
CPPFLAGS = -Wall -Wno-unknown-pragmas $(LIBAV_INC)

LIBAV_OBJ = \
	$(LIBAV_DIR)/connector.o \
	$(LIBAV_DIR)/geometry.o \
	$(LIBAV_DIR)/graph.o \
	$(LIBAV_DIR)/router.o \
	$(LIBAV_DIR)/makepath.o \
	$(LIBAV_DIR)/polyutil.o \
	$(LIBAV_DIR)/region.o \
	$(LIBAV_DIR)/shape.o \
	$(LIBAV_DIR)/static.o \
	$(LIBAV_DIR)/timer.o \
	$(LIBAV_DIR)/vertices.o \
	$(LIBAV_DIR)/visibility.o
	
ifeq ($(target), win32)
	LIBAV_OBJ = \
	$(LIBAV_DIR)\connector.o \
	$(LIBAV_DIR)\geometry.o \
	$(LIBAV_DIR)\graph.o \
	$(LIBAV_DIR)\router.o \
	$(LIBAV_DIR)\makepath.o \
	$(LIBAV_DIR)\polyutil.o \
	$(LIBAV_DIR)\region.o \
	$(LIBAV_DIR)\shape.o \
	$(LIBAV_DIR)\static.o \
	$(LIBAV_DIR)\timer.o \
	$(LIBAV_DIR)\vertices.o \
	$(LIBAV_DIR)\visibility.o
endif
	
LIBAV_LIB = $(LIBAV_DIR)/libavoid.a
ifeq ($(target), win32)
	LIBAV_LIB = $(LIBAV_DIR)\libavoidwin.a
endif

libavoid: $(LIBAV_LIB)

$(LIBAV_LIB): $(LIBAV_OBJ)
	ar cr $(LIBAV_LIB) $(LIBAV_OBJ)
	ranlib $(LIBAV_LIB)

clean_libavoid:
	-$(delete) $(LIBAV_OBJ)

distclean_libavoid: clean_libavoid
	-$(delete) $(LIBAV_LIB)

############################################################################
# TWINE
############################################################################

TWINE_DIR = $(UTIL_DIR)/twines
ifeq ($(target), win32)
	TWINE_DIR = $(UTIL_DIR)\twines
endif

TWINE_INC = -I$(TWINE_DIR)/include/ -I/usr/include 
ifeq ($(target), win32)
	TWINE_INC = -I$(TWINE_DIR)\include
endif

TWINE_CFL = -O3 -pedantic -ffast-math $(TWINE_INC) -Wall
ifeq ($(target), win32)
	TWINE_CFL = -O3 -pedantic -ffast-math $(TWINE_INC) -Wall
endif

TWINE_OBJ = $(TWINE_DIR)/rb.o \
	    $(TWINE_DIR)/twine.o

ifeq ($(target), win32)
	TWINE_OBJ = $(TWINE_DIR)\rb.o \
		$(TWINE_DIR)\twine.o
endif

TWINE_LIB = $(TWINE_DIR)/lib/libtwine.a
ifeq ($(target), win32)
	TWINE_LIB = $(TWINE_DIR)\lib\libtwinewin.a
endif

twine:	$(TWINE_LIB)

$(TWINE_LIB): ${TWINE_OBJ}
	$(copy) *.o $(TWINE_DIR)
	$(delete) *.o
	ar ruvs $(TWINE_LIB) ${TWINE_OBJ}
	
.SUFFIXES:.c

CC = gcc

.c.o:
	${CC} ${TWINE_CFL} -c $<

clean_twine:
	-$(delete) ${TWINE_OBJ}

distclean_twine: clean_twine
	-$(delete) $(TWINE_LIB)