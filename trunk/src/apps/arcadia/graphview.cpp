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
 *  GraphView.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *	Last Documented on 17/04/2008.
 *
 */

#include "graphview.h"

// local
#include "graphcontroller.h"

/**************
* Constructor *
***************
* sets up the Controller
* adds the View to the Controller
* and displays the Model (possibly NULL)
****************************************/
GraphView::GraphView(GraphController * c) : controller(c)
{
	this->controller->addView(this);
}

/***************************************************
* Destructor: removes the View from the Controller *
***************************************************/
GraphView::~GraphView()
{
	if (this->controller) { this->controller->removeView(this); }
}

/*****************************
* display: sets up the Model *
*****************************/
void GraphView::display(GraphModel * gModel)
{
	this->graphModel = gModel;
}

/*************************
* vertexSelectionChanged *
**************************
* Notifies the Controller of the new Vertex selection list
**********************************************************/
void GraphView::vertexSelectionChanged(std::list<BGL_Vertex> vList)
{
	if (this->controller) this->controller->select(vList, this);
}

/****************
* toggleCloning *
*****************
* Notifies the Controller that cloning must be toggled for that Vertex in that layout
*************************************************************************************/
void GraphView::toggleCloning(BGL_Vertex v, GraphLayout * gl, CloneContent * c)
{
	if (this->controller) this->controller->toggleCloning(v, gl, c);
}

void GraphView::updateLayout(GraphLayout * gl, bool edgesOnly, bool fast)
{
	if (this->controller) this->controller->updateLayout(gl, edgesOnly, fast);
}

void GraphView::switchView(std::list<BGL_Vertex> vList)
{
	if (this->controller) this->controller->switchView(vList);
}

void GraphView::createView(std::list<BGL_Vertex> vList)
{
	if (this->controller) this->controller->createView(vList);
}

void GraphView::expandView(std::list<BGL_Vertex> vList, GraphLayout * gl)
{
	if (this->controller) this->controller->expandView(vList, gl);
}