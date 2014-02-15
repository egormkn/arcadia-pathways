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
 *  ContentLayoutManager.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 04/03/2008.
 *  Last documented on 17/04/2008.
 *
 */

#include "contentlayoutmanager.h"

// local subclasses to be instanciated by tghe factory
#include "squarecontentlayoutmanager.h"
#include "graphvizcontentlayoutmanager.h"

/***************************************************
* DefaultStrategy: Set to Hierarchy (graphviz dot) *
***************************************************/
ContentLayoutStrategy ContentLayoutManager::DefaultStrategy = Hierarchy;

/*******************
* GetLayoutManager *
********************
* Creates and returns an appropriate subclass
* (set up with the corresponding strategy type)
* depending on the ContentLayoutStrategy parameter
**************************************************/
ContentLayoutManager* ContentLayoutManager::GetLayoutManager(ContentLayoutStrategy s)
{
	ContentLayoutManager *layoutManager = NULL;

	switch(s)
	{
	case Automatic:
		layoutManager = new SquareContentLayoutManager();
		break;
	case Hierarchy:
		layoutManager = new GraphvizContentLayoutManager();
		break;
	case Clone:
	case Branch:
	case Triangle:
		layoutManager = new GraphvizContentLayoutManager();
		layoutManager->setRotation(true); // by default, to be overloaded by the calling container if necessary
		break;
	case Neighbourhood:
		layoutManager = new GraphvizContentLayoutManager("twopi");
		break;
	case NoStrategy:
		layoutManager = new ContentLayoutManager();
		break;
	}
	
	layoutManager->strategy = s; // not done in the class itself because GraphViz, for instance, can be used for cloning, branching, etc.
	
	return layoutManager;
}

/*****************************************************
* Constructor: Sets the rotation to false by default *
*****************************************************/
ContentLayoutManager::ContentLayoutManager() { this->rotation = false; }

/**************************************
* getType: Returns the local strategy *
**************************************/
ContentLayoutStrategy ContentLayoutManager::getType() const { return this->strategy; }