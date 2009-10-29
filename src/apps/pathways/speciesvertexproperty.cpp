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
 *  SpeciesVertexProperty.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#include "speciesvertexproperty.h"

#include "ontologycontainer.h"

// LibSBML
#include <sbml/SBMLTypes.h>

/***********************************
* Constructor: sets up the Species *
************************************/
SpeciesVertexProperty::SpeciesVertexProperty(Species * s) : PathwayVertexProperty(s) { }
		
std::string SpeciesVertexProperty::getLabel()
{
	std::string l;
	Species * species = (Species*) this->base;
	l = species->getSpeciesType();
	if (l != "") l = species->getModel()->getSpeciesType(l)->getName();
	if (l == "") l = PathwayVertexProperty::getLabel();
	return l;
}

/**********
* getInfo *
***********
* Returns "Species " + the Id
* [!] do we have any more relevant info from the model?
*******************************************************/
std::string SpeciesVertexProperty::getInfo()
{
	std::string text = "Species ";
	
	Species * species = (Species*) this->base;
	
	if (species)
	{
		std::ostringstream o;

		o << species->getId() << "\n";
		
		if (species->getName() != "")
		{
			o << "Name: " << species->getName() << "\n";
		}
		
		if (species->getSpeciesType() != "")
		{
			o << "Species Type: " << species->getSpeciesType() << "\n";
		}
		if (species->getInitialAmount())
		{
			o << "Initial Amount: " << species->getInitialAmount() << "\n";
		}
		if (species->getInitialConcentration())
		{
			o << "Initial Concentration: " << species->getInitialConcentration() << "\n";
		}
		if (species->getSubstanceUnits() != "")
		{
			o << "Substance Units: " << species->getSubstanceUnits() << "\n";
		}
		if (species->getSpatialSizeUnits() != "")
		{
			o << "Spatial Size Units: " << species->getSpatialSizeUnits() << "\n";
		}
		if (species->getUnits() != "")
		{
			o << "Units: " << species->getUnits() << "\n";
		}
		if (species->getHasOnlySubstanceUnits())
		{
			o << "Has Only Substance Units\n";
		}
		if (species->getBoundaryCondition())
		{
			o << "Boundary Condition\n";
		}
		if (species->getCharge())
		{
			o << "Charge: " << species->getCharge() << "\n";
		}
		if (species->getConstant())
		{
			o << "Constant\n";
		}
		
		if (this->getSBOTerm()>0)
		{
			o << "SBO Term: " << this->getTypeLabel() << " (" << this->getSBOTerm() << ")\n";		
		}
		
		text.append(o.str());
	}
	else	text.append("Unknown\n");
	return text;
}

std::string SpeciesVertexProperty::getTypeLabel()
{
	std::string type = "";
	OntologyContainer * sbo = OntologyContainer::GetMyLocalSBO();
	if (sbo) type = sbo->getName(this->getSBOTermId());
	if (type == "") type = "Species";
	return type;
}

std::string SpeciesVertexProperty::getSuperTypeLabel()
{
	std::string type = "";
	
	if (this->getSBOTerm() > 0) type = "Species";
	
	return type;
}

std::string SpeciesVertexProperty::getCompartment()
{
	Species* species = (Species*) this->base;
	return species->getCompartment();
}

std::map<std::string, std::string> SpeciesVertexProperty::getInfoMap()
{
	std::map<std::string, std::string> infoMap = PathwayVertexProperty::getInfoMap();
	infoMap["Compartment"] = this->getCompartment();

	Species * species = (Species*) this->base;
	if (!species) return infoMap;

	if (species->getName() != "")
	{
		infoMap["Name"] = species->getName();
	}
		
	if (species->getSpeciesType() != "")
	{
		infoMap["Species Type"] = species->getSpeciesType();
	}
	if (species->getInitialAmount() != 0)
	{
		char text[32]; sprintf(text, "%f", species->getInitialAmount());
		infoMap["Initial Amount"] = text;
	}
	if (species->getInitialConcentration() != 0)
	{
		char text[32]; sprintf(text, "%f", species->getInitialConcentration());
		infoMap["Initial Concentration"] = text;
	}
	if (species->getSubstanceUnits() != "")
	{
		infoMap["Substance Units"] = species->getSubstanceUnits();
	}
	if (species->getSpatialSizeUnits() != "")
	{
		infoMap["Spatial Size Units"] = species->getSpatialSizeUnits();
	}
	if (species->getUnits() != "")
	{
		infoMap["Units"] = species->getUnits();
	}
	if (species->getHasOnlySubstanceUnits())
	{
		infoMap["Has Only Substance Units"] = "Yes";
	}
	if (species->getBoundaryCondition())
	{
		infoMap["Boundary Condition"] = "Yes";
	}
	if (species->getCharge() != 0)
	{
		char text[32]; sprintf(text, "%d", species->getCharge());
		infoMap["Charge"] = text;
	}
	if (species->getConstant())
	{
		infoMap["Constant"] = "Yes";
	}
	
	return infoMap;
}