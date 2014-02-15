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
 *  PathwayEdgeProperty.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#include "pathwayedgeproperty.h"

#include "ontologycontainer.h"

// LibSBML
#include <sbml/SBMLTypes.h>

/********************************************
* Constructor: sets up the SpeciesReference *
*********************************************/
PathwayEdgeProperty::PathwayEdgeProperty(SpeciesReference * sr, bool reversible) : speRef(sr) { if (reversible) this->isOriented = false; }

std::string PathwayEdgeProperty::getId()
{
	std::string v = "";
	if (this->speRef) v = this->speRef->getId();
	return v;
}

int PathwayEdgeProperty::getSBOTerm()
{
	if (this->speRef)
	{
		return this->speRef->getSBOTerm();
	}
	else
	{
		return -1;
	}
}

int PathwayEdgeProperty::inherits(int sboTerm)
{
	int distance = -1;
	
	OntologyContainer * sbo = OntologyContainer::GetMyLocalSBO();

	std::ostringstream o;
	if (sboTerm<1000) o << "SBO:0000";
	if (sboTerm<100) o << "0";
	if (sboTerm<10) o << "0";
	o << sboTerm;
	
	if (sbo) distance = sbo->inherits(this->getSBOTermId(), o.str());

	return distance;
}

std::string PathwayEdgeProperty::getSBOTermId()
{
	int sbo = this->getSBOTerm();
	std::ostringstream o;
	if (sbo<1000) o << "SBO:0000";
	if (sbo<100) o << "0";
	if (sbo<10) o << "0";
	o << sbo;
	return o.str();

//	return "";
	/*this->base->getSBOTermID();*/
}