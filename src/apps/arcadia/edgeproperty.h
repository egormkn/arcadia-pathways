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
 *  EdgeProperty.h
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *	Last Documented on 17/04/2008.
 *
 */

#ifndef EDGEPROPERTY_H
#define EDGEPROPERTY_H

// STL
#include <string>
#include <iostream>

/*****************
* EdgeProperty *
******************
* Generic properties for edges
* Defines an interface for a stringVersion (for debugging output),
* Also gives info on the edge type (for styling purposes)
**********************************************************************/
class EdgeProperty
{
public:
	EdgeProperty() { this->isOriented = true; } 
	virtual ~EdgeProperty() { }

	virtual std::string stringVersion();
	virtual std::string getTypeLabel();
	
	virtual std::string getId() { return ""; }
	
	virtual int getSBOTerm() {  return -1; } // should not be here...
	virtual int inherits(int sbo) { return -1; } // [!] no meaning for non pathway vertex

	bool isOriented;
};

#endif