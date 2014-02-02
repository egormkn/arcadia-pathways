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
 *  CompartmentContainer.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 16/05/2008.
 *  Last documented never
 *
 */

#include "compartmentcontainer.h"
#include "arcadia/graphlayout.h"

#include <sbml/SBMLTypes.h>

CompartmentContainer::CompartmentContainer(GraphLayout *l, ContainerContent * c, Compartment * comp) : ContainerContent(l, c, "CompContainer"), compartment(comp)
{
	this->label = comp->getName();
	if (this->label == "") this->label = comp->getId();
	this->layout->map(this);	
}
	
std::string CompartmentContainer::getReference() { return compartment->getId(); }

CompartmentContainer::~CompartmentContainer()
{	
	this->layout->unMap(this);
}