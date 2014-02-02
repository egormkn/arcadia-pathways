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
	ReactionVertexProperty(Reaction * r, PathwayGraphModel * m);
	std::string getInfo();
	bool clonable();

	std::string getTypeLabel(bool highest=false);
	std::string getSuperTypeLabel();
	
	std::string getCompartment();
	void setCompartment(std::string c) { this->compartment = c; }
	
	bool isReversible();
	
	std::map<std::string, std::string> getInfoMap();

	std::string getFormula(const ASTNode * tree);

private:
	std::string compartment;
};

class SourceOrSinkProperty : public VertexProperty
{
public:
	SourceOrSinkProperty(bool iS, ReactionVertexProperty * r) : isSource(iS), reaction(r) {}
	std::string getLabel() { return this->isSource? "Source" : "Sink"; } // the id, however, is ""
	std::string getCompartment() { return this->reaction->getCompartment();}

	std::string getTypeLabel(bool highest=false) { if (highest) return this->getSuperTypeLabel(); else return "empty set"; }
	std::string getSuperTypeLabel() { return "Species"; }
	
private:
	bool isSource;
	ReactionVertexProperty * reaction;
};

#endif