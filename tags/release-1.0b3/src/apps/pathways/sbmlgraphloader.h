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
 *  SBMLGraphLoader.h
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#ifndef SBMLGRAPHLOADER_H
#define SBMLGRAPHLOADER_H

// local base class
#include <arcadia/graphloader.h>

// LibSBML
class SBMLDocument;

class PathwayGraphModel;

/*************
* SBMLGraphLoader *
**************
* This subclass of GraphLoader handles an SBMLDocument
* Upon construction, the document gets loaded using LibSBML
* from an SBML file with a given fileName
* Upon destruction the SBMLDocument gets destroyed too
*
* However, if the getModel method is called,
* a GraphModel is created that encapsulate the Document,
* and the destruction of the Document is delegated to it
* The current Document of the Loader is set to NULL
***********************************************************/
class SBMLGraphLoader : public GraphLoader
{
public:
	static GraphModel * GetModel(std::string fName);
	static bool Save(PathwayGraphModel *m, std::string fName);

public:
	SBMLGraphLoader(std::string fName="");
	~SBMLGraphLoader();

private:
	GraphModel * getModel();

	void load(std::string fName);
	void clear();
	
	SBMLDocument * document;
	std::string filename;	
};

#endif