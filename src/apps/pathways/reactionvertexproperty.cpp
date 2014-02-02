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
#include "pathwaygraphmodel.h"

// LibSBML
#include <sbml/SBMLTypes.h>

bool ReactionVertexProperty::isReversible()
{
	return ((Reaction*) this->base)->getReversible();
}

/************************************
* Constructor: sets up the Reaction *
*************************************/
ReactionVertexProperty::ReactionVertexProperty(Reaction * r, PathwayGraphModel * m)
	: PathwayVertexProperty(r, m)
{
	// initialize compartment info
	this->compartment = "";

	Reaction * reaction = (Reaction*) this->base;
	
	// For each modifier
	int numModifiers = reaction->getNumModifiers();
	for (int modifierIndex=0; modifierIndex<numModifiers; modifierIndex++)
	{
		// what is the compartment?
		std::string ss = reaction->getModel()->getSpecies(reaction->getModifier(modifierIndex)->getSpecies())->getCompartment();
		// if it's the first compartment we find, we set that as tentative compartment value
		if (this->compartment == "") this->compartment = ss;
		// else if this new compartment is different from all previous compartments, the compartment value is set to ""
		else if (this->compartment != ss) { this->compartment = ""; return; }
	}

	// For each reactant
	int numReactants = reaction->getNumReactants();
	for (int reactantIndex=0; reactantIndex<numReactants; reactantIndex++)
	{	
		// what is the compartment?
		std::string ss = reaction->getModel()->getSpecies(reaction->getReactant(reactantIndex)->getSpecies())->getCompartment();
		// if it's the first compartment we find, we set that as tentative compartment value
		if (this->compartment == "") this->compartment = ss;
		// else if this new compartment is different from all previous compartments, the compartment value is set to ""
		else if (this->compartment != ss) { this->compartment = ""; return; }
	}

	// For each product
	int numProducts = reaction->getNumProducts();
	for (int productIndex=0; productIndex<numProducts; productIndex++)
	{
		// what is the compartment?
		std::string ss = reaction->getModel()->getSpecies(reaction->getProduct(productIndex)->getSpecies())->getCompartment();
		// if it's the first compartment we find, we set that as tentative compartment value
		if (this->compartment == "") this->compartment = ss;
		// else if this new compartment is different from all previous compartments, the compartment value is set to ""
		else if (this->compartment != ss) { this->compartment = ""; return;	}
	}
	
	// An empty string for the reaction compartment means that all the species involved are not in the same compartment
}

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

std::string ReactionVertexProperty::getTypeLabel(bool highest)
{
	if (highest && this->getSuperTypeLabel() != "") return this->getSuperTypeLabel();

	std::string type = "";

	OntologyContainer * sbo = OntologyContainer::GetMyLocalSBO();
	if (sbo) type = sbo->getName(this->getSBOTermId());
	if (type == "") type = "Reaction";
		
	return type;
}

std::string ReactionVertexProperty::getSuperTypeLabel()
{
	std::string type = "";
	
	if (this->getSBOTerm() > 0) if (this->getTypeLabel() != "Reaction") type = "Reaction";
	
	return type;
}
	
/******************
* clonable: false *
*******************/
bool ReactionVertexProperty::clonable() { return false; }

std::string ReactionVertexProperty::getCompartment() { return this->compartment; }

std::map<std::string, std::string> ReactionVertexProperty::getInfoMap()
{
	std::map<std::string, std::string> infoMap = PathwayVertexProperty::getInfoMap();
	
	infoMap["Compartment"] = this->model->getCompartmentLabel( this->getCompartment() );
	
	Reaction * reaction = (Reaction*) this->base;	
	if (!reaction) return infoMap;
	
	if (reaction->getName() != "")
	{
		infoMap["ReactionName"] = reaction->getName();
	}

	std::string formula = "";

	int numModifiers = reaction->getNumModifiers();
	std::string mod = " (";
	for (int modifierIndex=0; modifierIndex<numModifiers; modifierIndex++)
	{
		std::string speciesId = reaction->getModifier(modifierIndex)->getSpecies();	
		std::string speciesLabel = this->model->getLabelFromId( speciesId );
		mod.append(" + ");
		mod.append(speciesLabel);
	}		
	mod.append(" )");

	int numReactants = reaction->getNumReactants();
	std::string rea = "";
	for (int reactantIndex=0; reactantIndex<numReactants; reactantIndex++)
	{	
		std::string speciesId = reaction->getReactant(reactantIndex)->getSpecies();	
		std::string speciesLabel = this->model->getLabelFromId( speciesId );
		if (reactantIndex) rea.append(" + ");
		rea.append(speciesLabel);
	}

	int numProducts = reaction->getNumProducts();
	std::string pro = "";
	for (int productIndex=0; productIndex<numProducts; productIndex++)
	{
		std::string speciesId = reaction->getProduct(productIndex)->getSpecies();	
		std::string speciesLabel = this->model->getLabelFromId( speciesId );
		if (productIndex) pro.append(" + ");
		pro.append(speciesLabel);
	}

	formula += rea;
	if (numModifiers) formula += mod;
	formula += " -> ";
	formula += pro;		
	if (numModifiers) formula += mod;
	
	infoMap["Formula"] = formula;
	
	if (reaction->getKineticLaw()) if (reaction->getKineticLaw()->getFormula() != "")
	{
		const ASTNode * tree = reaction->getKineticLaw()->getMath(); // getFormula() doesn't resolve function names
		std::string formula = this->getFormula(tree);
		infoMap["Kinetic Law"] = formula;
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

int isNodeName(const ASTNode * tree) { return tree->isName(); }
int isNodeFunction(const ASTNode * tree) { return tree->isFunction(); }

/*
bool replaceName(ASTNode * expression, std::string argument, ASTNode * value)
{
	if ((!expression->isOperator()) && (!expression->isNumber()) && (expression->getName() == argument))
	{
		delete expression; expression = new ASTNode( *value );
		// expression->setName(value.c_str()); // value can be a formula itself, e.g. x + y
	}
	else for (int i = 0; i < expression->getNumChildren(); ++i)
		replaceName(expression->getChild(i), argument, value);
}
*/

// Parses the tree to generate a formula (resolves function names)
std::string ReactionVertexProperty::getFormula(const ASTNode_t * treeOriginal)
{
	std::string formula = "";
	
	ASTNode treeCopy(*treeOriginal); // we are going to modify the node
		
	// we look for the nodes which represent user defined functions
	List * fList = treeCopy.getListOfNodes(isNodeFunction);
	for (int i = 0; i<fList->getSize(); ++i)
	{
		ASTNode * functionCall = (ASTNode*)fList->get(i);
		std::string id = functionCall->getName();

/*
		std::cout << "Function " << id << std::endl;
		std::string label = this->model->getFunctionName(id);
		std::pair< std::string, std::list<std::string> > fd;
		fd = this->model->getFunctionDefinitionAsStrings(id);
		std::cout << label << " " << fd.first << std::endl;
		for(std::list<std::string>::iterator it = fd.second.begin(); it != fd.second.end(); ++it)
			std::cout << "param " << *it << std::endl;
*/
		FunctionDefinition * def = this->model->getFunctionDefinition(id);
		if (!def) continue; // this is a standard function, not a user defined one

		// substitution process

		ASTNode * functionResult = new ASTNode (*(def->getBody())); // copy of the function's body

		for (int i=0; i<def->getNumArguments(); ++i) // for each argument in the function
		{
			const ASTNode * argument = def->getArgument(i);
			// the number of arguments and function call children should match...
			if (i >= functionCall->getNumChildren()) break; 
			// we replace the argument in the function body by the corresponding child in the function call
			functionResult->replaceArgument(argument->getName(), functionCall->getChild(i));
		}

// For some reason the bottom code fails on some files! (e.g. Biomodel56 3rd reaction)
// [!] A pointer problem apparently, which I can't delete properly, and causes a bus error later on
// Problem is, the pointer is returned by a LibSBML method I can't control...
//		delete functionCall;
//		functionCall = functionResult;
// So we swap the nodes names, types and children and delete the node I allocated myself
		functionCall->swapChildren(functionResult);
		functionCall->setType(functionResult->getType());
		functionCall->setName(functionResult->getName());
		delete functionResult;

/*
		ASTNode * functionCallCopy = new ASTNode ( *functionCall );

		delete functionCall; // we're going to replace it with the body of the function
		functionCall = new ASTNode ( *(def->getBody()) ); // copy of the function's body

		for (int i=0; i<def->getNumArguments(); ++i)
		{
			const ASTNode * argument = def->getArgument(i); // for each argument		
			if (i >= functionCallCopy->getNumChildren()) break; // the number of arguments and children should match...
			// we replace the argument in the function's body by the corresponding child in the function call
			functionCall->replaceArgument(argument->getName(), functionCallCopy->getChild(i));
			// replaceName(functionCall, argument->getName(), functionCallCopy->getChild(i));
		}
*/
	}

	// we look for the nodes which represent user defined variables
	List * nList = treeCopy.getListOfNodes(isNodeName);

	for (int i = 0; i<nList->getSize(); ++i)
	{
		ASTNode * n = (ASTNode*)nList->get(i);
		std::string id = n->getName();
		std::string label = this->model->getLabelFromId( id );
		// Also try the kinetic law parameters (trumps global value)
		Parameter * p = ((Reaction*)this->base)->getKineticLaw()->getParameter(id);
		if (p) { label = p->getName(); if (label == "") label = id; }
		if (label != "") n->setName(label.c_str());
	}

	formula = SBML_formulaToString(&treeCopy);
		
	return formula;
}