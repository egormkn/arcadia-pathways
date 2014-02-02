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
 *  PathwayGraphController.h
 *  arcadia
 *
 *  Created by Alice Villeger on 27/05/2008.
 *	Last Documented never.
 *
 */

#ifndef PATHWAYGRAPHCONTROLLER_H
#define PATHWAYGRAPHCONTROLLER_H

#include <arcadia/graphcontroller.h>

/*
#if utopia
	#include <utopia2/utopia2.h> // [!] utopia node
#endif
*/

/*************************
* PathwayGraphController *
**************************
*************************/
class PathwayGraphController : public GraphController
{
protected:
	void getGraphModel(std::string filename);
public:
	std::string getExportFileTypes();
	std::string getImportFileTypes();

	void toggleModifiersCloning();
	void toggleReactionsFusing();
	
	void lookUpResources(BGL_Vertex v); // do something with these nice URIs

/*
#if utopia	
	void load(UTOPIA2::Node * model, std::string fileName);  // [!] utopia node
	PathwayGraphController(UTOPIA2::Node * model, std::string fileName);  // [!] utopia node
#endif
*/
	void setSBO(std::string dirName = "", bool fullPath = false);

	PathwayGraphController(std::string dirName="", std::string fileName="");  // [!] utopia node
};

#endif