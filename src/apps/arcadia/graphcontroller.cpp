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
 *  GraphController.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 08/04/2008.
 *	Last Documented on 17/04/2008.
 *
 */

#include "graphcontroller.h"

#include <iostream>

// local
#include "graphview.h"
#include "graphmodel.h"
#include "graphloader.h"
#include "graphlayout.h"
#include "stylesheet.h"

/************************************************************************
* Constructor: sets the graphModel to NULL and the busy status to false *
*************************************************************************/
GraphController::GraphController() : _graphModel(NULL), busy(false) {}

/*************
* Destructor *
**************
* Closes any existing graph
* Then deletes all the Views in the list
****************************************/
GraphController::~GraphController()
{
	this->close();
	 // the views get removed from their Controller when deleted, so we need a copy of the list to process through
	std::list<GraphView*> views = this->_graphViews;
	for (std::list<GraphView*>::iterator it = views.begin(); it != views.end(); ++it) { if (*it) delete (*it); }
}

/****************************************************
* getFileName: returns the name of the Model's file *
*****************************************************/
std::string GraphController::getFileName() { return this->_graphModel->getFileName(); }

/*******************************************
* getModelTitle: returns the Model's title *
********************************************/
std::string GraphController::getModelTitle() { return this->_graphModel->getTitle(); }

/*********************************************************************************************
* addView: adds the view to the list [!] doesn't check if the View points at that controller *
**********************************************************************************************/
void GraphController::addView(GraphView * view)
{
	this->_graphViews.push_back(view);
	view->display(this->_graphModel);
}

/*****************************************************************************************************
* removeView: removes the view from the list [!] doesn't check if the View points at that controller *
******************************************************************************************************/
void GraphController::removeView(GraphView * view)
{
	this->_graphViews.remove(view);
	view->resetController();
}

#include <fstream>

/*******
* load *
********
* Closes any existing graph
* Uses a GraphModelLoader to get
* a new graph model from a filename (possibly empty)
* The graph model is then displayed by every views
****************************************************/
void GraphController::load(std::string filename)
{
	this->close();
	
	this->getGraphModel(filename);

	for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
	{	
		(*it)->display(this->_graphModel);
	}	
}

/*******
* save *
********
* Calls the GraphModel's save method
* [!] no format specified! Shouldn't we have the equivalent of a Loader for handling exports?
* Also triggers the exportGraphics method of every Views (screengrabs)
* [!] not very clean for a public release...
*********************************************************************************************/
void GraphController::save(std::string filename)
{
	this->_graphModel->save(filename);

	// + mass screengrab export
	for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
	{
		(*it)->exportGraphics(filename);
	}	
}

/********
* close *
*********
* deletes the existing GraphModel and sets it to NULL
* [!] add an option for saving first? (bool input)
* [!] what about the views??!!
*****************************************************/
void GraphController::close()
{
	if (this->_graphModel) { delete this->_graphModel; }
	this->_graphModel = NULL;
}

/*********
* select *
**********
* The list of vertex gets selected in every views but the one who called
* While the operation is being performed, no other signal is processed
************************************************************************/
void GraphController::select(std::list<BGL_Vertex> vList, GraphView * caller)
{
	if (this->busy) return;
	this->busy = true;
		
	this->currentSelection = vList;

	for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
	{
		if (*it != caller) (*it)->select(vList);
	}

	this->busy = false;
}

void GraphController::selectLayout(GraphLayout * gl, GraphView * caller)
{
	for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
	{
		if (*it != caller) (*it)->selectLayout(gl);
	}
}

void GraphController::moveToLayout(GraphLayout *gl, int i)
{
	this->selectLayout(this->_graphModel->getNextLayout(gl, i), NULL);
}

/****************
* toggleCloning *
*****************
* The vertex cloning status gets toggled in some Layout(s) of the Model
* then every view gets notified of the corresponding change
* While the operation is being performed, no other signal is processed
* After that, the cloned/uncloned vertex gets automatically selected
************************************************************************/
void GraphController::toggleCloning(BGL_Vertex v, GraphLayout * gl, CloneContent * c)
{
	if (this->busy) return;
	this->busy = true;
	
	this->_graphModel->toggleCloning(v, gl, c);

	for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
	{
		(*it)->cloningGotToggled(v, gl);
	}

	this->busy = false;
	
	std::list<BGL_Vertex> vList;
	vList.push_back(v);
	this->select(vList, NULL);
}

void GraphController::updateLayout(GraphLayout * gl, bool edgesOnly, bool fast)
{
	if (this->busy) return;
	this->busy = true;
	
	this->_graphModel->updateLayout(gl, edgesOnly, fast);

	for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
	{
		(*it)->layoutGotUpdated(gl, edgesOnly, fast);
	}

	this->busy = false;
}

void GraphController::switchView(std::list<BGL_Vertex> vList)
{
	if (this->busy) return;
	this->busy = true;

	GraphLayout * layout = this->_graphModel->addLayout(vList);
		
	// created a new layout
	if (!layout)
	{
	
		for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
		{
			(*it)->layoutGotAdded();
		}
	}
	// selecting the existing layout
	else
	{
		this->selectLayout(layout, NULL);
	}

	this->busy = false;
}



void GraphController::createView(std::list<BGL_Vertex> vList)
{
	if (this->busy) return;
	this->busy = true;
	
	if (this->_graphModel->addLayout(vList, false))
	{
		for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
		{
			(*it)->layoutGotAdded();
		}
	}

	this->busy = false;
}

void GraphController::expandView(std::list<BGL_Vertex> vList, GraphLayout * gl)
{
	if (this->busy) return;
	this->busy = true;

	this->_graphModel->expandLayout(vList, gl);

	// actually that's because I want to update the display (take into account the new nodes)
	if (vList.size()) for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
	{
		(*it)->cloningGotToggled(vList.front(), gl);
	}

	this->busy = false;
}

void GraphController::getGraphModel(std::string filename)
{
	this->_graphModel = GraphLoader::GetModel(filename); // generic graph loader
}

std::string GraphController::getImportFileTypes()
{
	return ""; // more options should be separated by ;;
}

std::string GraphController::getExportFileTypes()
{
	return "Text files (*.txt);;Graphviz files (*.dot);;PDF files (*.pdf);;PS files (*.ps);;SVG files (*.svg)"; // more options should be separated by ;;
}

void GraphController::toggleContainerVisibility()
{
	this->_graphModel->getStyleSheet()->toggleContainerVisibility();
	for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
	{
		(*it)->containerVisibilityGotToggled();
	}	
}

void GraphController::toggleAvoidingEdges()
{
	this->_graphModel->toggleAvoidingEdges();
	
	this->updateLayout(NULL, true, false);	
}

void GraphController::destroyLayout()
{
	if (this->busy) return;
	this->busy = true;

	GraphLayout * gl = this->_graphModel->destroyLayout();

	if (gl)
	{
		for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
		{
			(*it)->layoutGotRemoved(gl);
		}
	}
	
	this->busy = false;
}

void GraphController::arrangeSelection()
{
	std::list<BGL_Vertex> vList = this->currentSelection;
	for (std::list<BGL_Vertex>::iterator it = vList.begin(); it != vList.end(); ++it)
	{
		this->toggleCloning(*it, NULL, NULL);
	}
}

void GraphController::newLayout()
{
	std::list<BGL_Vertex> vList = this->currentSelection;
	this->switchView(vList);
}

std::list< std::string > GraphController::getActions()
{
	std::list< std::string > aList;
	aList.push_back("A");
	aList.push_back("B");
	aList.push_back("C");
	return aList;
}

void GraphController::executeAction(std::string a)
{
	if (a == "A")
		std::cout << "tata" << std::endl;	

	if (a == "B")
		std::cout << "tbtb" << std::endl;

	if (a == "C")
		std::cout << "tctc" << std::endl;		
}