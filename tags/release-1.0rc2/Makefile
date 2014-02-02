###########################################################################
#
# Arcadia is a visualisation tool for metabolic pathways
# This file is the main Makefile for the arcadia1.0 application
#
# This Makefile first compiles auxiliary static library in src/libs
# then creates a makefile for the Qt project in src/apps and compiles it
#
# In install mode, specific packaging steps are performed for MacOSX apps
#
# To compile for Windows, use the target=win32 option
#
###########################################################################
#
# Copyright (C) 2007-2009 Alice Villeger, University of Manchester
# <alice.villeger@manchester.ac.uk>
#
###########################################################################
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
###########################################################################

createmakefile = qmake -spec macx-g++
finishinstall = ./finishinstallosx
ifeq ($(target), win32)
	createmakefile = qmake
	finishinstall = .\finishinstallwin32.bat
endif

all: libs apps

install: all
	cd src/libs && make install
	cd src && make install && $(finishinstall)

libs:
	cd src/libs && make

apps:
	cd src && $(createmakefile) && make
	
clean:
	-cd src/libs && make clean
	-cd src && make clean

distclean: clean
	-cd src/libs && make distclean
	-cd src && make distclean

uninstall: distclean
	-cd src/libs && make uninstall
	-cd src && make uninstall