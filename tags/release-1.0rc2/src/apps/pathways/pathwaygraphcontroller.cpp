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
 *  PathwayGraphController.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 27/05/2008.
 *	Last Documented never.
 *
 */

#include "pathwaygraphcontroller.h"

#include "ontologycontainer.h"

#include "sbmlgraphloader.h"

#include <arcadia/edgeproperty.h> // [!] the toggle modifier bit should be put in the model
#include <arcadia/vertexproperty.h> // [!] the toggle modifier bit should be put in the model

#include <iostream>

#include <arcadia/graphview.h>

/*
#include "pathwaygraphmodel.h" // [!] utopia node
#include "sbmldocument.h" // [!] utopia node
*/

#include "pathwayvertexproperty.h"

#include <arcadia/graphlayout.h> // [!] updateReactionOrientation
#include <arcadia/connector.h> // [!] updateReactionOrientation
#include <arcadia/clonecontent.h> // [!] updateReactionOrientation

void PathwayGraphController::getGraphModel(std::string filename)
{
	GraphController::getGraphModel(filename);
	if (!this->_graphModel) this->_graphModel = SBMLGraphLoader::GetModel(filename); // pathway graph loader
}

std::string PathwayGraphController::getExportFileTypes()
{
	std::string sbmlFilter = "SBML Files (*.sbml *.xml)";
	return sbmlFilter + ";;" + GraphController::getExportFileTypes();
}

std::string PathwayGraphController::getImportFileTypes()
{
	std::string sbmlFilter = "SBML Files (*.sbml *.xml)";
	return sbmlFilter + ";;" + GraphController::getImportFileTypes();
}

void PathwayGraphController::toggleModifiersCloning()
{
	if (this->busy) return;
	this->busy = true;
	
	this->saveUndo();
	
	std::list<BGL_Vertex> vList = this->_graphModel->getVertices();	
	for (std::list<BGL_Vertex>::iterator vit = vList.begin(); vit != vList.end(); ++vit)
	{
		std::list<BGL_Edge> eList = this->_graphModel->getOutEdges(*vit);
		for (std::list<BGL_Edge>::iterator eit = eList.begin(); eit != eList.end(); ++eit)
		{
			if (this->_graphModel->getProperties(*eit)->getTypeLabel() == "Modifier")
			{
				this->selfToggleCloning(*vit, NULL, NULL);
			}
		}
	}	
	
	this->busy = false;
}

// [!] not implemented yet!!
void PathwayGraphController::toggleReactionsFusing()
{
	if (this->busy) return;
	this->busy = true;

	this->saveUndo();

	std::list<BGL_Vertex> rList;	
	std::list<BGL_Vertex> vList = this->_graphModel->getVertices();	
	for (std::list<BGL_Vertex>::iterator vit = vList.begin(); vit != vList.end(); ++vit)
	{
		if (this->_graphModel->getProperties(*vit)->getTypeLabel(true) == "Reaction") rList.push_back(*vit);
	}

	this->_graphModel->toggleFusing(rList);
	for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
	{
		(*it)->cloningGotToggled(rList.front(), NULL); // [!] that's not really the right method to use, but hey
	}

	this->busy = false;	
	this->select(rList, NULL);
}

/*
#if utopia
	// [!] utopia node
	void PathwayGraphController::load(UTOPIA2::Node * model, std::string fileName)
	{
		this->close();

		SBMLDocument * doc = NULL;

		if (model && model->attributes.exists("SBMLDocument"))
		{
			doc = model->attributes.get("SBMLDocument").value< SBMLDocument * >();
		}
	
		if (fileName != "")
			this->_graphModel = new PathwayGraphModel(doc, fileName);
		else
			this->_graphModel = new PathwayGraphModel(doc);

		for (std::list<GraphView*>::iterator it = this->_graphViews.begin(); it != this->_graphViews.end(); ++it)
		{
			(*it)->display(this->_graphModel);
		}	
	}
#endif
*/

/*
#if utopia
	// [!] utopia node
	PathwayGraphController::PathwayGraphController(UTOPIA2::Node * model, std::string fileName) : GraphController()
	{
		this->load(model, fileName);
	}
#endif
*/

// [!] utopia node
PathwayGraphController::PathwayGraphController(std::string dirName, std::string fileName) : GraphController("")
{
	if (dirName != "") this->setSBO(dirName, false);
	if (fileName != "") this->load(fileName);
}

void PathwayGraphController::setSBO(std::string dirName, bool fullPath)
{
	OntologyContainer::LoadLocalSBO(dirName, fullPath);
}

void PathwayGraphController::lookUpResources(BGL_Vertex v)
{
	PathwayVertexProperty * p = (PathwayVertexProperty*)(this->_graphModel->getProperties(v)); // [!] oh le vilain cast!! bouh :(
	std::list<std::string> isList = p->getIsURIs();
	for (std::list<std::string>::iterator it = isList.begin(); it != isList.end(); ++it)
	{
		std::cout << (*it) << std::endl;
	}
}