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
 *  OntologyContainer.h
 *  arcadia
 *
 *  Created by Alice Villeger on 24/09/2008.
 *  Last documented never properly
 *
 */

#ifndef ONTOLOGY_CONTAINER_H
#define ONTOLOGY_CONTAINER_H

#include <string>
#include <map>

class QDomElement;

/******************
* OntologyContainer *
*******************
* I need a place to store information such as a local copy of SBO loaded from a XML file
* and a generic way to access this data, regardless of implementation
* (I'll look into proper classic ontology storage options when I get more time
* I just need quick read-only access right now)
*****************************************/
class OntologyContainer
{
public:
	OntologyContainer(std::string fileName = ""); // at the moment, only support the XML format used for SBO
	std::string getName(std::string sboId);
	int inherits(std::string child, std::string parent);
	static OntologyContainer * GetMyLocalSBO();
	
	static void LoadLocalSBO(std::string dirName = "", bool fullPath = false);
	
private:
	static OntologyContainer * MyLocalSBO;
	std::map<std::string, std::string> idToParent;
	std::map<std::string, std::string> idToName;
	static std::string GetTextFromUniqueTag(QDomElement * e, std::string tag);
};

#endif