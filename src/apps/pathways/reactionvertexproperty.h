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
 *  ReactionVertexProperty.h
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#ifndef REACTIONVERTEXPROPERTY_H
#define REACTIONVERTEXPROPERTY_H

// local base class
#include "pathwayvertexproperty.h"

class Reaction;

/******************
* ReactionVertexProperty *
*******************
* This subclass of VertexProperty
* is associated to a given SBML Reaction
* It provides the SBML Id of the Reaction
* and information from the SBML Model
* The local Type is reacType
* The vertex is not clonable
*****************************************/
class ReactionVertexProperty : public PathwayVertexProperty
{
public:
	ReactionVertexProperty(Reaction * r);
	std::string getInfo();
	bool clonable();

	std::string getTypeLabel();
	std::string getSuperTypeLabel();
	
	std::string getCompartment();
	
	bool isReversible();
	
	std::map<std::string, std::string> getInfoMap();
};

class SourceOrSinkProperty : public VertexProperty
{
public:
	SourceOrSinkProperty(bool iS, ReactionVertexProperty * r) : isSource(iS), reaction(r) {}
	std::string getId() { return this->isSource? "Source" : "Sink"; }
	std::string getCompartment() { return this->reaction->getCompartment();}

	std::string getTypeLabel() { return "empty set"; }
	std::string getSuperTypeLabel() { return "Species"; }
	
private:
	bool isSource;
	ReactionVertexProperty * reaction;
};

#endif