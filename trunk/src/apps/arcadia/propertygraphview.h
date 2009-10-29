/***********************************************************************
*
*  Arcadia is a visualisation tool for metabolic pathways
*
*  This file is part of the arcadia1.0 application distribution
*  Copyright (C) 2007-2009 Alice Villeger, University of Manchester
*  <alice.villeger@manchester.ac.uk>
* 
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
* 
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
* 
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*************************************************************************/

/*
 *  PropertyGraphView.h
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#ifndef PROPERTYGRAPHVIEW_H
#define PROPERTYGRAPHVIEW_H

#define tabletest

// local base class
#include "graphview.h"

#ifdef tabletest
#include <QTabWidget>

/***************
* PropertyGraphView *
****************
* This subclass of QLabel and GraphView displays
* detailled information on the currently selected Vertices
* As a GraphView, it refers to a GraphController
* [!] it being merely a subclass of QLabel makes it a bit ugly
**************************************************************/
class PropertyGraphView : public QTabWidget, public GraphView
{
public:
	PropertyGraphView(GraphController * c, QWidget * parent = NULL);
	void select(std::list<BGL_Vertex> vList);
	void display(GraphModel *gModel);
};
#endif

// Qt base class
#ifndef tabletest
#include <QTextEdit>

class PropertyGraphView : public QTextEdit, public GraphView
{
public:
	PropertyGraphView(GraphController * c, QWidget * parent = NULL);
	void select(std::list<BGL_Vertex> vList);
	void display(GraphModel *gModel);
};
#endif

#endif