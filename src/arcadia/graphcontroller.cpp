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
#include "graphwindow.h"
#include "graphview.h"
#include "graphmodel.h"
#include "graphloader.h"
#include "graphlayout.h"
#include "stylesheet.h"

/********************************************************************//**
* Constructor: sets the graphModel to NULL and the busy status to false *
*************************************************************************/
GraphController::GraphController(std::string fileName) : _graphModel(NULL), busy(false), undoFlag(false), moving(false), layoutNumber(0), window(NULL)
{
	if (fileName != "") this->load(fileName);
}

void GraphController::setGraphWindow(GraphWindow * w)
{
	this->window = w;
	if (this->window) this->window->enableUndo(this->undoFlag);
}

/*********//**
* Destructor *
**************
* Closes any existing graph
* Then deletes all the Views in the list
****************************************/
GraphController::~GraphController()
{
	this->save("ArcadiaLastDocument.xml");

	this->close();
	 // the views get removed from their Controller when deleted, so we need a copy of the list to process through
	std::list<GraphView*> views = this->_graphViews;
	for (std::list<GraphView*>::iterator it = views.begin(); it != views.end(); ++it) { if (*it) delete (*it); }
}

/************************************************//**
* getFileName: returns the name of the Model's file *
*****************************************************/
std::string GraphController::getFileName() { if (this->_graphModel) return this->_graphModel->getFileName(); else return ""; }

/***************************************//**
* getModelTitle: returns the Model's title *
********************************************/
std::string GraphController::getModelTitle() { if (this->_graphModel) return this->_graphModel->getTitle(); else return ""; }

/*****************************************************************************************//**
* addView: adds the view to the list [!] doesn't check if the View points at that controller *
**********************************************************************************************/
void GraphController::addView(GraphView * view)
{
	this->_graphViews.push_back(view);
	if (this->_graphModel) view->display(this->_graphModel);
}

/*************************************************************************************************//**
* removeView: removes the view from the list [!] doesn't check if the View points at that controller *
******************************************************************************************************/
void GraphController::removeView(GraphView * view)
{
	this->_graphViews.remove(view);
	view->resetController();
}

#include <fstream>

/***//**
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

/***//**
* save *
********
* Calls the GraphModel's save method
* [!] no format specified! Shouldn't we have the equivalent of a Loader for handling exports?
* Also triggers the exportGraphics method of every Views (screengrabs)
* [!] not very clean for a public release...
*********************************************************************************************/
void GraphController::save(std::string filename)
{
	if (!this->_graphModel) return;

	this->_graphModel->save(filename);

	// + mass screengrab export
	for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
	{
		(*it)->exportGraphics(filename);
	}
}

/****//**
* close *
*********
* deletes the existing GraphModel and sets it to NULL
* [!] add an option for saving first? (bool input)
* [!] what about the views??!!
*****************************************************/
void GraphController::close()
{
	// Because deleting the graphmodel requires waiting for its connector layout manager threads
	// and in the mean time the view can't display half deleted data, obviously
	for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
	{
		(*it)->display(NULL);
	}

	if (this->_graphModel) { delete this->_graphModel; }
	this->_graphModel = NULL;
}

/*****//**
* select *
**********
* The list of vertex gets selected in every views but the one who called
* While the operation is being performed, no other signal is processed
************************************************************************/
void GraphController::select(std::list<BGL_Vertex> vList, GraphView * caller)
{
	if (this->busy) return;
	this->busy = true;
	this->selfSelect(vList, caller);
	this->busy = false;
}

void GraphController::selfSelect(std::list<BGL_Vertex> vList, GraphView * caller)
{	
	this->currentSelection = vList;
	for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
	{
		if (*it != caller) (*it)->select(vList);
	}
}

void GraphController::selectLayout(GraphLayout * gl, GraphView * caller)
{
	this->layoutNumber = this->graphModel()->getNumber(gl);

	for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
	{
		if (*it != caller) (*it)->selectLayout(gl);
	}
}

void GraphController::moveToLayout(GraphLayout *gl, int i)
{
	if (this->_graphModel) this->selectLayout(this->_graphModel->getNextLayout(gl, i), NULL);
}

/************//**
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

	this->saveUndo();
	this->selfToggleCloning(v, gl, c);

	this->busy = false;	
}

void GraphController::selfToggleCloning(BGL_Vertex v, GraphLayout * gl, CloneContent * c)
{
	if (!this->_graphModel) return;
	
	this->_graphModel->toggleCloning(v, gl, c);

	for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
	{
		(*it)->cloningGotToggled(v, gl);
	}

	std::list<BGL_Vertex> vList;
	vList.push_back(v);
	this->selfSelect(vList, NULL);
}

void GraphController::updateLayout(GraphLayout * gl, bool edgesOnly, bool fast)
{
	if (this->busy) return;
	this->busy = true;

	// Before a full update
	if (!edgesOnly) this->saveUndo();
// Too time consuming, need smarter Undo (just save what changed... = nodes & coords)
/*
	else // manual move
	{
		// At the start of a move, we save the initial position
		if (fast && !this->moving) { this->saveUndo(); this->moving = true; }
		// after that, we don't save intermediate moves
		
		if (!fast) this->moving = false; // end of move, liberate the flag for next move
	}
*/

	this->selfUpdateLayout(gl, edgesOnly, fast);
	this->busy = false;
}

void GraphController::selfUpdateLayout(GraphLayout * gl, bool edgesOnly, bool fast)
{
	if (!this->_graphModel) return;

	this->_graphModel->updateLayout(gl, edgesOnly, fast);

	for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
	{
		(*it)->layoutGotUpdated(gl, edgesOnly, fast);
	}
}

void GraphController::switchView(std::list<BGL_Vertex> vList)
{
	if (this->busy) return;
	this->busy = true;
	this->saveUndo();
	this->selfSwitchView(vList);
	this->busy = false;
}

void GraphController::selfSwitchView(std::list<BGL_Vertex> vList)
{
	if (!this->_graphModel) return;
	GraphLayout * layout = this->_graphModel->addLayout(vList);
		
	// created a new layout
	if (!layout) for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
	{
		(*it)->layoutGotAdded();
	}
	// selecting the existing layout
	else this->selectLayout(layout, NULL);
}

/*
void GraphController::createView(std::list<BGL_Vertex> vList)
{
	if (this->busy) return;
	if (!this->_graphModel) return;

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
*/
/*
void GraphController::expandView(std::list<BGL_Vertex> vList, GraphLayout * gl)
{
	if (this->busy) return;
	if (!this->_graphModel) return;

	this->busy = true;

	this->_graphModel->expandLayout(vList, gl);

	// actually that's because I want to update the display (take into account the new nodes)
	if (vList.size()) for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
	{
		(*it)->cloningGotToggled(vList.front(), gl);
	}

	this->busy = false;
}
*/

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
	if (!this->_graphModel) return;

	this->_graphModel->getStyleSheet()->toggleContainerVisibility();
	for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
	{
		(*it)->containerVisibilityGotToggled();
	}	
}

void GraphController::toggleAvoidingEdges()
{
	if (this->busy) return;
	if (!this->_graphModel) return;

	this->busy = true;
	this->_graphModel->toggleAvoidingEdges();
	this->selfUpdateLayout(NULL, true, false);	
	this->busy = false;
}

void GraphController::destroyLayout()
{
	if (this->busy) return;

	this->saveUndo();

	if (!this->_graphModel) return;

	this->busy = true;

	GraphLayout * gl = this->_graphModel->getCurrentLayout();

	if (gl && (this->_graphModel->layoutNumber() > 1) ) // the visible layout will be destroyed
	{
		for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
		{
			(*it)->layoutGotRemoved(gl);
		}

		this->_graphModel->destroyLayout(gl);
	}

// old version - we need the views to relinquish control over the layout BEFORE it gets destroyed,
// as layout destruction can take a non negligeable time if edge routing is on
/*
	GraphLayout * gl = this->_graphModel->destroyLayout();

	if (gl)
	{
		for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
		{
			(*it)->layoutGotRemoved(gl);
		}
	}
*/
	this->busy = false;
}

void GraphController::arrangeSelection()
{
	if (this->busy) return;
	this->busy = true;

	this->saveUndo();

	std::list<BGL_Vertex> vList = this->currentSelection;
	for (std::list<BGL_Vertex>::iterator it = vList.begin(); it != vList.end(); ++it)
	{
		this->selfToggleCloning(*it, NULL, NULL);
	}

	this->busy = false;
}

void GraphController::newLayout()
{
	this->saveUndo();
	std::list<BGL_Vertex> vList = this->currentSelection;
	this->selfSwitchView(vList);
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

void GraphController::undo()
{
	if (!this->graphModel()) return;

	if(this->undoFlag)
	{
		int n = this->layoutNumber;

		std::string saveName = this->graphModel()->getFileName();
		this->load("ArcadiaLastDocument.xml");
		this->graphModel()->setFileName(saveName);

		if (GraphLayout * gl = this->graphModel()->getLayout(n))
		{
			this->selectLayout(gl, NULL);
		}

		this->undoFlag = false;
		if (this->window) this->window->enableUndo(this->undoFlag);
	}
}

void GraphController::saveUndo()
{
	if (!this->graphModel()) return;

	std::string saveName = this->graphModel()->getFileName();
	this->save("ArcadiaLastDocument.xml");
	this->graphModel()->setFileName(saveName);

	this->undoFlag = true;
	if (this->window) this->window->enableUndo(this->undoFlag);
}
