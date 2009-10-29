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
 *  ReactionVertexProperty.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#include "reactionvertexproperty.h"

#include "ontologycontainer.h"

// LibSBML
#include <sbml/SBMLTypes.h>

bool ReactionVertexProperty::isReversible()
{
	return ((Reaction*) this->base)->getReversible();
}

/************************************
* Constructor: sets up the Reaction *
*************************************/
ReactionVertexProperty::ReactionVertexProperty(Reaction * r) : PathwayVertexProperty(r) { }

/**********
* getInfo *
***********
* Returns "Reaction " + the Id
* then the modifiers, reactant and products
* [!] we don't return the stoichiometries
*******************************************/
std::string ReactionVertexProperty::getInfo()
{
	std::string text = "Reaction ";
	
	Reaction * reaction = (Reaction*) this->base;
	
	if (reaction)
	{	
		text.append(reaction->getId());
		
		if (reaction->getName() != "")
		{
			text.append("\nName: ");		
			text.append(reaction->getName());
		}

		int numModifiers = reaction->getNumModifiers();
		std::string mod = "(";
		for (int modifierIndex=0; modifierIndex<numModifiers; modifierIndex++)
		{
			mod.append(" + ");
			mod.append(reaction->getModifier(modifierIndex)->getSpecies());
		}		
		mod.append(" )");

        int numReactants = reaction->getNumReactants();
		std::string rea = "";
		for (int reactantIndex=0; reactantIndex<numReactants; reactantIndex++)
		{	
			if (reactantIndex) rea.append(" + ");
			rea.append(reaction->getReactant(reactantIndex)->getSpecies());
		}

		int numProducts = reaction->getNumProducts();
		std::string pro = "";
		for (int productIndex=0; productIndex<numProducts; productIndex++)
		{
			if (productIndex) pro.append(" + ");
			pro.append(reaction->getProduct(productIndex)->getSpecies());
		}

		text.append("\n");
		text.append(rea);
		if (numModifiers) text.append(mod);
		text.append(" -> ");
		text.append(pro);		
		if (numModifiers) text.append(mod);
		
		if (reaction->getKineticLaw()) if (reaction->getKineticLaw()->getFormula() != "")
		{
			text.append("\nKinetic Law: ");
			text.append(reaction->getKineticLaw()->getFormula());
		}
		
		if (reaction->getReversible())
		{
			text.append("\nReversible");
		}
		
		if (reaction->getFast())
		{
			text.append("\nFast");
		}
		
		text.append("\n");
	}
	else text.append("Unknown");

	return text;
}

std::string ReactionVertexProperty::getTypeLabel()
{
	std::string type = "";

	OntologyContainer * sbo = OntologyContainer::GetMyLocalSBO();
	if (sbo) type = sbo->getName(this->getSBOTermId());

	if (type == "") type = "Reaction";
		
	return type;
}

std::string ReactionVertexProperty::getSuperTypeLabel()
{
	std::string type = "";
	
	if (this->getSBOTerm() > 0) type = "Reaction";
	
	return type;
}
	
/******************
* clonable: false *
*******************/
bool ReactionVertexProperty::clonable() { return false; }

std::string ReactionVertexProperty::getCompartment() // if all species involved are not in the same compartment, returns nothing
{
	std::string s = "";

	Reaction * reaction = (Reaction*) this->base;
	
	int numModifiers = reaction->getNumModifiers();
	for (int modifierIndex=0; modifierIndex<numModifiers; modifierIndex++)
	{
		std::string ss = reaction->getModel()->getSpecies(reaction->getModifier(modifierIndex)->getSpecies())->getCompartment();
		if (s == "") { s = ss; continue; }
		if (s != ss) { s = ""; break; }
	}		

	int numReactants = reaction->getNumReactants();
	for (int reactantIndex=0; reactantIndex<numReactants; reactantIndex++)
	{	
		std::string ss = reaction->getModel()->getSpecies(reaction->getReactant(reactantIndex)->getSpecies())->getCompartment();
		if (s == "") { s = ss; continue; }
		if (s != ss) { s = ""; break; }
	}

	int numProducts = reaction->getNumProducts();
	for (int productIndex=0; productIndex<numProducts; productIndex++)
	{
		std::string ss = reaction->getModel()->getSpecies(reaction->getProduct(productIndex)->getSpecies())->getCompartment();
		if (s == "") { s = ss; continue; }
		if (s != ss) { s = ""; break; }
	}

	return s;
}

std::map<std::string, std::string> ReactionVertexProperty::getInfoMap()
{
	std::map<std::string, std::string> infoMap = PathwayVertexProperty::getInfoMap();
	
	Reaction * reaction = (Reaction*) this->base;	
	if (!reaction) return infoMap;
	
	if (reaction->getName() != "")
	{
		infoMap["ReactionName"] = reaction->getName();
	}

	std::string formula = "";

	int numModifiers = reaction->getNumModifiers();
	std::string mod = "(";
	for (int modifierIndex=0; modifierIndex<numModifiers; modifierIndex++)
	{
		mod.append(" + ");
		mod.append(reaction->getModifier(modifierIndex)->getSpecies());
	}		
	mod.append(" )");

	int numReactants = reaction->getNumReactants();
	std::string rea = "";
	for (int reactantIndex=0; reactantIndex<numReactants; reactantIndex++)
	{	
		if (reactantIndex) rea.append(" + ");
		rea.append(reaction->getReactant(reactantIndex)->getSpecies());
	}

	int numProducts = reaction->getNumProducts();
	std::string pro = "";
	for (int productIndex=0; productIndex<numProducts; productIndex++)
	{
		if (productIndex) pro.append(" + ");
		pro.append(reaction->getProduct(productIndex)->getSpecies());
	}

	formula += rea;
	if (numModifiers) formula += mod;
	formula += " -> ";
	formula += pro;		
	if (numModifiers) formula += mod;
	
	infoMap["Formula"] = formula;
	
	if (reaction->getKineticLaw()) if (reaction->getKineticLaw()->getFormula() != "")
	{
		infoMap["Kinetic Law"] = reaction->getKineticLaw()->getFormula();
	}
		
	if (reaction->getReversible())
	{
		infoMap["Reversible"] = "Yes";
	}
		
	if (reaction->getFast())
	{
		infoMap["Fast"] = "Yes";
	}
	
	return infoMap;
}