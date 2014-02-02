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
 *  GraphView.h
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *	Last Documented on 17/04/2008.
 *
 */

#ifndef GRAPHVIEW_H
#define GRAPHVIEW_H

// local (included here for the BGL_Vertex)
#include "graphmodel.h"

// local
class GraphController;

/************
* GraphView *
*************
* This generic class controlled by a Controller
* defines the interface for viewing a given Model
* 
* The display method updates the view of a Model
* The exportGraphics method saves the view in a file
*
* The select method selects a list of Vertices
* [!] should the selected state be stored in the model?
* The cloningToggled method updates the view
* to take into account the new cloning state of a vertex
*
* vertexSelectionChanged and toggleCloning notify the
* Controller that these actions must be performed
********************************************************/
class GraphView
{
public:
	GraphView(GraphController * c);
	virtual ~GraphView();
	virtual void display(GraphModel * gModel);

	virtual void exportGraphics(std::string filename) {}

	virtual void select(std::list<BGL_Vertex> vList) {}
	virtual void cloningGotToggled(BGL_Vertex v, GraphLayout * gl) {}
	virtual void layoutGotUpdated(GraphLayout * gl, bool edgesOnly, bool fast) {}
	virtual void layoutGotAdded() {}
	virtual void containerVisibilityGotToggled() {}
	virtual void layoutGotRemoved(GraphLayout * gl) {}
	virtual void selectLayout(GraphLayout * gl) {}

	void resetController() { this->controller = NULL; }
	
	GraphController * getController() { return this->controller; }
	
protected:
	GraphModel * graphModel;
	GraphController * controller;

	void vertexSelectionChanged(std::list<BGL_Vertex> vList);
	void toggleCloning(BGL_Vertex v, GraphLayout * gl = NULL, CloneContent * c = NULL);
	void updateLayout(GraphLayout * gl = NULL, bool edgesOnly = false, bool fast = false);
	void switchView(std::list<BGL_Vertex> vList);
/*
	void createView(std::list<BGL_Vertex> vList);
	void expandView(std::list<BGL_Vertex> vList, GraphLayout * gl);
*/
};

#endif