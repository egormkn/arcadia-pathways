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
 *  Content.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 27/02/2008.
 *	Last Documented on 17/04/2008.
 *
 */

#include "content.h"

// local
#include "containercontent.h"

#include <iostream>

void Content::setPosition(int x, int y)
{
	if (container) this->container->setUpdateFlag();
}

/*****************************************************************
* Constructor: sets the id to nothing then sets up the container *
*****************************************************************/
Content::Content(GraphLayout *l, ContainerContent * c) : layout(l), cId(""), container(NULL), hidden(false) { this->setContainer(c); }

/*****************************************
* Destructor: sets the Container to NULL *
*****************************************/
Content::~Content() { this->setContainer(NULL); }

/***********************************************************************
* Id access                                                            *
***********************************************************************/

/************************
* getId: returns the id *
************************/
std::string Content::getId() { return this->cId; }

/***********************************************************************
* Container management                                                 *
***********************************************************************/

/******************************************************************************************************
* hasContainerContent: checks whether this is the Content's Container [!] doesn't check the Container's side *
******************************************************************************************************/
bool Content::hasContainer(ContainerContent * c) {	return (this->getContainer() == c); }

/************************************************
* getContainerContent: returns the Content's Container *
************************************************/
ContainerContent * Content::getContainer() { return this->container; }

/***************
* setContainer *
****************
* sets the Container to contain the Content
* First, remove the Content from its old Container
* then sets the new Container as the current one
* and adds the Content to it
**************************************************/
void Content::setContainer(ContainerContent * c)
{
	if (!this->hasContainer(c))
	{
		if (ContainerContent *old = this->getContainer())
		{
			this->container = NULL;
			old->remove(this);
		}
		this->container = c;
	}

	if (c) if (!c->has(this)) c->add(this);
}

int Content::getLevel()
{
	ContainerContent * c = this->getContainer();
	if (c) return c->getLevel() + 1;
	else return 0;
}

ContainerContent * Content::CommonAncestor(Content * c1, Content * c2)
{
	ContainerContent * p1 = c1->getContainer();	
	ContainerContent * p2 = c2->getContainer();
		
	if (p1 == p2) return p1;
	else
	{
		int diff = c1->getLevel() - c2->getLevel();
		if (diff > 0)	return Content::CommonAncestor(p1, c2);
		else			return Content::CommonAncestor(c1, p2);
	}
}