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
 *  SpeciesVertexProperty.h
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#ifndef SPECIESVERTEXPROPERTY_H
#define SPECIESVERTEXPROPERTY_H

// local base class
#include "pathwayvertexproperty.h"

class Species;

/******************
* SpeciesVertexProperty *
*******************
* This subclass of VertexProperty
* is associated to a given SBML Species
* It provides the SBML Id of the Species
* and information from the SBML Model
* The local Type is specType
****************************************/
class SpeciesVertexProperty : public PathwayVertexProperty
{
public:
	SpeciesVertexProperty(Species * s);
	std::string getInfo();
	std::string getCompartment();

	std::string getLabel();
	std::string getTypeLabel();
	std::string getSuperTypeLabel();

	std::map<std::string, std::string> getInfoMap();
};

#endif