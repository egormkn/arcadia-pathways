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
 *  PathwayVertexProperty.h
 *  arcadia
 *
 *  Created by Alice Villeger on 09/05/2008.
 *  Last documented never.
 *
 */

#ifndef PATHWAYVERTEXPROPERTY_H
#define PATHWAYVERTEXPROPERTY_H

// local base class
#include <arcadia/vertexproperty.h>

// LibSBML
#include <sbml/SBMLTypes.h> // [!] because I didn't encapsulate the BiolQualifierType_t bit

class OntologyContainer;
class PathwayGraphModel;

//class SBase;

/******************
* SpeciesVertexProperty *
*******************/
class PathwayVertexProperty : public VertexProperty
{
public:
	PathwayVertexProperty(SBase * b, PathwayGraphModel * m);

	virtual std::string getLabel();
	std::string getId();

	int getSBOTerm();
	std::string getSBOTermId();
	std::list< std::string > getIsURIs();
	
	virtual std::map<std::string, std::string> getInfoMap();	

	int inherits(int sbo);

protected:
	SBase * base;
	PathwayGraphModel * model;
	
	std::list< std::pair< BiolQualifierType_t, std::string > > getBioURIs();
	std::list< std::string > getBioURIs(BiolQualifierType_t bt, bool includeURLs = false);
};

#endif