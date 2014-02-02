############################################################################
#
# Arcadia is a visualisation tool for metabolic pathways
# This file is the Qt project file for the arcadia1.0 application
#
# Copyright (C) 2007-2009 Alice Villeger, University of Manchester
# <alice.villeger@manchester.ac.uk>
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
############################################################################

# When I will properly use webservices
# include(qtextension/qtsoap/src/qtsoap.pri)

TEMPLATE = app
TARGET = arcadia

win32 {
	DESTDIR = ../win32
}
macx {
DESTDIR = ../macosx
}
unix {
DESTDIR = ../macosx
}

QT += svg
QT += xml

CONFIG += release

macx {
#	CONFIG += x86 ppc
	DEFINES += MAC_COMPILATION
	QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.3
}

# VARIABLES ######################################################################################

UTILPATH = ./libs
LIBSPATH = ./apps
ARCADIAPATH = $$LIBSPATH/arcadia
PATHWAYPATH = $$LIBSPATH/pathways

win32 {
	BOOSTPATH = C:/boost/boost_1_37_0
	GRAPHVIZPATH = C:\graphviz\graphviz_2_21\include\graphviz
	LIBSBMLPATH = C:\sbml\libsbml-4.0.0\install\include
	EXTERNALLIBS = -LC:\graphviz\graphviz_2_21\lib -lgvc.dll -lgraph.dll
	EXTERNALLIBS += -LC:\sbml\libsbml-4.0.0\install\lib -lsbml
	EXTERNALLIBS += -LC:\libxml\libxml2-2.7.3\installdir\lib -lxml2
} else {
	BOOSTPATH = /opt/local/include/boost-1_35 /opt/local/include
	GRAPHVIZPATH = /usr/local/include/graphviz
	LIBSBMLPATH = /usr/local/include/sbml
	unix {
		LIBSBMLLIBS = -L/usr/local/lib -lsbml
		GRAPHVIZLIBS = -L/usr/local/lib -lgvc -lgraph
	}
	macx {
		LIBSBMLLIBS= /usr/local/lib/libsbml.a
		LIBSBMLLIBS += /usr/local/lib/libbz2.a /usr/local/lib/libxml2.a /opt/local/lib/libiconv.a
		GRAPHVIZLIBS = /usr/local/lib/libgvc.a
		GRAPHVIZLIBS += /usr/local/lib/libgraph.a
		GRAPHVIZLIBS += /usr/local/lib/libpathplan.a
		GRAPHVIZLIBS += /usr/local/lib/libcdt.a
		GRAPHVIZLIBS += /usr/local/lib/libexpat.a
		GRAPHVIZLIBS += /usr/local/lib/libltdl.a
		GRAPHVIZLIBS += /usr/local/lib/graphviz/libgvplugin_dot_layout.a
		GRAPHVIZLIBS += /usr/local/lib/graphviz/libgvplugin_neato_layout.a
		# somehow required on os 10.4?
		GRAPHVIZLIBS += /usr/local/lib/libgvc_builtins.a
		GRAPHVIZLIBS += /usr/local/lib/graphviz/tcl/libtcldot_builtin.a
	}
	EXTERNALLIBS = $$LIBSBMLLIBS $$GRAPHVIZLIBS
}

# INCLUDEPATH ####################################################################################

INCLUDEPATH += $$UTILPATH
INCLUDEPATH += $$LIBSPATH

INCLUDEPATH += $$BOOSTPATH
INCLUDEPATH += $$GRAPHVIZPATH
INCLUDEPATH += $$LIBSBMLPATH

# LIBS ###########################################################################################

win32 {
	LIBS += -L$$UTILPATH/libavoid -lavoidwin
	LIBS += -L$$UTILPATH/twines/lib -ltwinewin
} else {
	LIBS += -L$$UTILPATH/libavoid -lavoid
	LIBS += -L$$UTILPATH/twines/lib -ltwine
}

LIBS += $$EXTERNALLIBS

# ARCADIA HEADERS ################################################################################

HEADERS += \
		$$ARCADIAPATH/graphwindow.h\
		$$ARCADIAPATH/graphcontroller.h\
		$$ARCADIAPATH/graphmodel.h\
		$$ARCADIAPATH/graphloader.h\
			$$ARCADIAPATH/defaultgraphloader.h\
			$$ARCADIAPATH/graphvizgraphloader.h\
		$$ARCADIAPATH/graphlayout.h\
		$$ARCADIAPATH/content.h\
			$$ARCADIAPATH/containercontent.h\
			$$ARCADIAPATH/clonecontent.h\
		$$ARCADIAPATH/connector.h\
		$$ARCADIAPATH/contentlayoutmanager.h\
			$$ARCADIAPATH/squarecontentlayoutmanager.h\
			$$ARCADIAPATH/graphvizcontentlayoutmanager.h\
		$$ARCADIAPATH/connectorlayoutmanager.h\
		$$ARCADIAPATH/stylesheet.h\
		$$ARCADIAPATH/edgestyle.h\
		$$ARCADIAPATH/vertexstyle.h\
		$$ARCADIAPATH/containerstyle.h\
			$$ARCADIAPATH/branchcontainerstyle.h\
			$$ARCADIAPATH/clonecontainerstyle.h\
			$$ARCADIAPATH/trianglecontainerstyle.h\
		$$ARCADIAPATH/edgeproperty.h\
		$$ARCADIAPATH/vertexproperty.h\
		$$ARCADIAPATH/graphview.h\
			$$ARCADIAPATH/modelgraphview.h\
			$$ARCADIAPATH/listgraphview.h\
			$$ARCADIAPATH/propertygraphview.h\
			$$ARCADIAPATH/layoutgraphview.h\
		$$ARCADIAPATH/edgegraphics.h\
		$$ARCADIAPATH/vertexgraphics.h\
		$$ARCADIAPATH/containergraphics.h\
		$$ARCADIAPATH/graphgraphics.h\

# ARCADIA SOURCES ################################################################################

SOURCES += \
		$$ARCADIAPATH/graphwindow.cpp\
		$$ARCADIAPATH/graphcontroller.cpp\
		$$ARCADIAPATH/graphmodel.cpp\
		$$ARCADIAPATH/graphloader.cpp\
			$$ARCADIAPATH/defaultgraphloader.cpp\
			$$ARCADIAPATH/graphvizgraphloader.cpp\
		$$ARCADIAPATH/graphlayout.cpp\
		$$ARCADIAPATH/content.cpp\
			$$ARCADIAPATH/containercontent.cpp\
			$$ARCADIAPATH/clonecontent.cpp\
		$$ARCADIAPATH/connector.cpp\
		$$ARCADIAPATH/contentlayoutmanager.cpp\
			$$ARCADIAPATH/squarecontentlayoutmanager.cpp\
			$$ARCADIAPATH/graphvizcontentlayoutmanager.cpp\
		$$ARCADIAPATH/connectorlayoutmanager.cpp\
		$$ARCADIAPATH/stylesheet.cpp\
		$$ARCADIAPATH/edgestyle.cpp\
		$$ARCADIAPATH/vertexstyle.cpp\
		$$ARCADIAPATH/containerstyle.cpp\
			$$ARCADIAPATH/branchcontainerstyle.cpp\
			$$ARCADIAPATH/clonecontainerstyle.cpp\
			$$ARCADIAPATH/trianglecontainerstyle.cpp\
		$$ARCADIAPATH/edgeproperty.cpp\
		$$ARCADIAPATH/vertexproperty.cpp\
		$$ARCADIAPATH/graphview.cpp\
			$$ARCADIAPATH/modelgraphview.cpp\
			$$ARCADIAPATH/listgraphview.cpp\
			$$ARCADIAPATH/propertygraphview.cpp\
			$$ARCADIAPATH/layoutgraphview.cpp\
		$$ARCADIAPATH/edgegraphics.cpp\
		$$ARCADIAPATH/vertexgraphics.cpp\
		$$ARCADIAPATH/containergraphics.cpp\

# PATHWAY HEADERS ################################################################################

HEADERS += \
		$$PATHWAYPATH/pathwaygraphwindow.h\
		$$PATHWAYPATH/pathwaygraphcontroller.h\
		$$PATHWAYPATH/pathwaygraphmodel.h\
		$$PATHWAYPATH/sbmlgraphloader.h\
		$$PATHWAYPATH/compartmentcontainer.h\
		$$PATHWAYPATH/pathwaystylesheet.h\
		$$PATHWAYPATH/reactionvertexstyle.h\
		$$PATHWAYPATH/speciesvertexstyle.h\
		$$PATHWAYPATH/modifieredgestyle.h\
		$$PATHWAYPATH/productedgestyle.h\
		$$PATHWAYPATH/reactantedgestyle.h\
		$$PATHWAYPATH/compartmentcontainerstyle.h\
		$$PATHWAYPATH/pathwayedgeproperty.h\
			$$PATHWAYPATH/modifieredgeproperty.h\
			$$PATHWAYPATH/productedgeproperty.h\
			$$PATHWAYPATH/reactantedgeproperty.h\
		$$PATHWAYPATH/pathwayvertexproperty.h\
			$$PATHWAYPATH/speciesvertexproperty.h\
			$$PATHWAYPATH/reactionvertexproperty.h\
		$$PATHWAYPATH/ontologycontainer.h\
# When I will properly use web services
#		$$PATHWAYPATH/webservicehandler.h\
		
# PATHWAY SOURCES ################################################################################

SOURCES += \
		$$PATHWAYPATH/pathwaygraphwindow.cpp\
		$$PATHWAYPATH/pathwaygraphcontroller.cpp\
		$$PATHWAYPATH/pathwaygraphmodel.cpp\
		$$PATHWAYPATH/sbmlgraphloader.cpp\
		$$PATHWAYPATH/compartmentcontainer.cpp\
		$$PATHWAYPATH/pathwaystylesheet.cpp\
		$$PATHWAYPATH/reactionvertexstyle.cpp\
		$$PATHWAYPATH/speciesvertexstyle.cpp\
		$$PATHWAYPATH/modifieredgestyle.cpp\
		$$PATHWAYPATH/productedgestyle.cpp\
		$$PATHWAYPATH/reactantedgestyle.cpp\
		$$PATHWAYPATH/compartmentcontainerstyle.cpp\
		$$PATHWAYPATH/pathwayedgeproperty.cpp\
			$$PATHWAYPATH/modifieredgeproperty.cpp\
			$$PATHWAYPATH/productedgeproperty.cpp\
			$$PATHWAYPATH/reactantedgeproperty.cpp\
		$$PATHWAYPATH/pathwayvertexproperty.cpp\
			$$PATHWAYPATH/speciesvertexproperty.cpp\
			$$PATHWAYPATH/reactionvertexproperty.cpp\
		$$PATHWAYPATH/ontologycontainer.cpp\
# When I will properly use web services
#		$$PATHWAYPATH/webservicehandler.cpp\

# MAIN SOURCES ###################################################################################

SOURCES +=	main.cpp\

# HIDING THE OBJECTS FILES #######################################################################

win32 {
	OBJECTS_DIR = .winobj
	MOC_DIR = .winmoc
	UI_DIR = .winuic
}
macx {
	OBJECTS_DIR = .macobj
	MOC_DIR = .macmoc
	UI_DIR = .macuic
}
unix {
	OBJECTS_DIR = .obj
	MOC_DIR = .moc
	UI_DIR = .uic
}