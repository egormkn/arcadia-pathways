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
 *  VertexProperty.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#include "vertexproperty.h"

// STL for "i to a" conversion
#include <stdlib.h>

#include <iostream>

/**************************************
* Default Constructor: The id is void *
***************************************/
VertexProperty::VertexProperty() : id(""), clonableFlag(true) { }

/************************
* Numbering Constructor *
*************************
* The id is an integer
**********************/
VertexProperty::VertexProperty(int n, bool c) : clonableFlag(c)
{
	char buff[16];
	sprintf(buff, "%d", n);
	this->id = buff;
}

/**************************************
* stringVersion: the label + the info *
***************************************/
std::string VertexProperty::stringVersion() { return this->getLabel() + ": " + this->getInfo(); }

/************************************
* getId: read only access to the id *
*************************************/
std::string VertexProperty::getId() { return this->id; }

/****************************
* getLabel: in fact, the id *
*****************************/
std::string VertexProperty::getLabel() { return this->getId(); }

/***************************
* getInfo: not much to say *
****************************/
std::string VertexProperty::getInfo() { return "Vertex Unknown"; }	
	
/***************
* getTypeLabel *
****************
* Returns a label that depends on the vertex type
* [!] a bit messy? should it be a static method?
*************************************************/
std::string VertexProperty::getTypeLabel()
{
	std::cout << "vertex" << std::endl;
	return "Vertex";
}
	
/***************************
* clonable: yes by default *
****************************/
bool VertexProperty::clonable() { return clonableFlag; }

std::map<std::string, std::string> VertexProperty::getInfoMap()
{
	std::map<std::string, std::string> infoMap;
	if (this->getId() != "")infoMap["Id"] = this->getId();
	if (this->getLabel() != "") infoMap["Label"] = this->getLabel();
	if (this->getSuperTypeLabel() != "")
	{
		infoMap["Type"] = this->getSuperTypeLabel();
		if (this->getTypeLabel() != "") infoMap["Sub-type"] = this->getTypeLabel();
	}
	else if (this->getTypeLabel() != "") infoMap["Type"] = this->getTypeLabel();
	return infoMap;
}