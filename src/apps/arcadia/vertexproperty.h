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
 *  VertexProperty.h
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#ifndef VERTEXPROPERTY_H
#define VERTEXPROPERTY_H

// STL
#include <string>

// Local (for BGL_Vertex)
#include "graphmodel.h"

/*****************
* VertexProperty *
******************
* Generic properties for vertex (redefined in derived classes)
*
* Defines an interface for an id (core attribute: a number or nothing)
* but also a stringVersion (for debugging output: label + info),
* a label (for every views: by default, the id),
* other infos (for the property view: by default, nothing)
*
* Also gives info on the vertex type (for styling purposes)
* and corresponding label (for the views)
* and on whether it is clonable or not (by default, yes)
*
* [!] that's a lot of different string outputs, maybe to clean up a bit...
**************************************************************************/
class VertexProperty
{
public:
	VertexProperty();
	VertexProperty(int n, bool c = true);
	virtual ~VertexProperty() {}
	
	virtual std::string stringVersion();
	virtual std::string getId();
	virtual std::string getLabel();
	virtual std::string getInfo();	

	virtual std::map<std::string, std::string> getInfoMap();	
	
	virtual std::string getTypeLabel();
	virtual std::string getSuperTypeLabel() { return ""; }

	virtual bool clonable();
	
	virtual int getSBOTerm() { return -1; } // [!] no meaning for non pathway vertex
	virtual std::string getCompartment() { return ""; } // [!] no meaning for non pathway vertex
	
	virtual int inherits(int sbo) { return -1; } // [!] no meaning for non pathway vertex
	
private:
	bool clonableFlag;

	std::string id;
};

#endif