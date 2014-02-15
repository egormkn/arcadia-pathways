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
 *  ProductEdgeProperty.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#include "productedgeproperty.h"

// STL
#include <sstream>

// LibSBML
#include <sbml/SBMLTypes.h>

/********************************************
* Constructor: sets up the SpeciesReference *
*********************************************/
ProductEdgeProperty::ProductEdgeProperty(SpeciesReference * sr, bool reversible) : PathwayEdgeProperty(sr, reversible) { }	

/****************
* stringVersion *
*****************
* Returns "Product " + the stoichiometry and the species from the SpeciesReference
**********************************************************************************/
std::string ProductEdgeProperty::stringVersion()
{
	std::ostringstream text;
	text << "Product ";
	if (this->speRef)
	{
		text << this->speRef->getStoichiometry();
		text << " ";
		text << this->speRef->getSpecies();
	}
	else text << "Unknown";

	return text.str();
}

/*******************
* getType: product *
*******************/
std::string ProductEdgeProperty::getTypeLabel() { return "Product"; }