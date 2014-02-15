################################################################################
# Twines is a fast and simple method for drawing a curve through control points
# Copyright (C) 1998  James Marsh
#  
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
################################################################################

# Makefile for twine library demo program
#
# Ver 1.0 - Initial version. 			James Marsh 05 May 1998

##### MACROS #####

PROG 	= graph
OBJS 	= graph.o
INCDIR 	= -I/usr/include -I../include/

CC 	= gcc
LIBS 	= -L/usr/X11/lib -L/usr/X11R6/lib -L/usr/lib -L../lib \
	-lX11 -lXext -lXmu -lXt -lXi -lSM -lICE \
	-ltwine  -lglut -lGLU -lGL -lm 
CFLAGS 	= -fPIC -O3 -pedantic -ffast-math $(INCDIR) -DSHM

##### TARGETS #####

$(PROG): 	$(OBJS)
		$(CC) $(LIBS) $(OBJS) -o $(PROG)

%.o:
		$(CC) $(CFLAGS) -c $*.c

