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
 *  GraphController.h
 *  arcadia
 *
 *  Created by Alice Villeger on 08/04/2008.
 *	Last Documented on 17/04/2008.
 *
 */

#ifndef GRAPHCONTROLLER_H
#define GRAPHCONTROLLER_H

#include <iostream>

// local MVC club
#include "graphmodel.h"
class GraphView;

class GraphLoader;
 
/******************
* GraphController *
*******************
* The GraphController manages a Model and a list of Views
* It can perform general actions such as load, save and close the Model
* It can also provide basic information on the model such as its Title and FileName
*
* Concerning Views, it can add and remove them (and destroy them upon its own destruction)
* Upon big actions, eg when a model gets loaded, the view get told to display it
* The view can also notify the model of specific interactions such as select or toggleCloning
* When these occur, the model temporarilly switches to busy mode (won't listen to new signals)
* , performs the given change on the model, and notifies every views about the new situation
**********************************************************************************************/
class GraphController
{
public:
	GraphController();
	virtual ~GraphController();
	
	void addView(GraphView * view);
	void removeView(GraphView * view);

	std::string getFileName();
	std::string getModelTitle();	
	
	void load(std::string filename);
	void save(std::string filename);
	void close();
	
	void select(std::list<BGL_Vertex> vList, GraphView * caller);
	void toggleCloning(BGL_Vertex v, GraphLayout * gl, CloneContent * c);
	void updateLayout(GraphLayout * gl, bool edgesOnly, bool fast);
	void switchView(std::list<BGL_Vertex> vList);
	void createView(std::list<BGL_Vertex> vList);
	void expandView(std::list<BGL_Vertex> vList, GraphLayout * gl);
	void selectLayout(GraphLayout * gl, GraphView * caller);
	void moveToLayout(GraphLayout * gl, int i);

	void toggleContainerVisibility();
	virtual void toggleModifiersCloning() {} //[!] shouldn't be defined here!
	virtual void toggleReactionsFusing() {} //[!] shouldn't be defined here!
	void toggleAvoidingEdges();
	void destroyLayout();
	void arrangeSelection();
	void newLayout();

	std::list< std::string > getActions();
	void executeAction(std::string a);

protected:
	GraphModel * _graphModel;
	std::list<GraphView*> _graphViews;
	bool busy;	
	virtual void getGraphModel(std::string filename);

	std::list<BGL_Vertex> currentSelection;

public:
	virtual std::string getImportFileTypes();
	virtual std::string getExportFileTypes();
};

#endif