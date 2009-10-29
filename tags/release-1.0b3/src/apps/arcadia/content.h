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
 *  Content.h
 *  arcadia
 *
 *  Created by Alice Villeger on 27/02/2008.
 *	Last Documented on 17/04/2008.
 *
 */

#ifndef CONTENT_H
#define CONTENT_H

// STL
#include <string>
#include <list>

// local
class Connector;
class ContainerContent;
class GraphLayout;

/**********
* Content *
***********
* Abstract class describing the Content of a Container
* Concretely, it can be instanciated either as a CloneContent or another Container
*
* For debugging purposes mostly (+serialization) each Content has a string id
* accessible read only through getId (it differentiates Clones="" from Containers="n")
* [!] isn't that a bit dodgy?
*
* A Content is also normally held in a Container (NULL only briefly when moving)
* [!] should we allow that NULL situation at all?
* This is managed in a read/write ways through hasContainer, getContainer and setContainer
*
* A Content has a (purely abstract) x, y position, that can be set through setPosition
* It also has width, height dimensions (defined in subclasses too)
* and left, right, top, and bottom borders (rectangle centered around the position)
*
* This (purely abstract) geometrical information is provided for layout purposes.
* Moreover, for that purpose, there is also an (empty by default) layoutContent method
* (redefined by the Container class)
*
* Finally, a getOutterConnector function is declared, but not defined at that level
*****************************************************************************************************/
class Content
{
public:
	Content(GraphLayout * l, ContainerContent * c = NULL);
	virtual ~Content();

	std::string getId();

	bool hasContainer(ContainerContent * c);
	ContainerContent * getContainer();
	virtual void setContainer(ContainerContent * c);
		
	virtual void setPosition(int x, int y);
	virtual int x() = 0;
	virtual int y() = 0;
	
	virtual int left(bool withMargin) = 0;
	virtual int right(bool withMargin) = 0;
	virtual int top(bool withMargin) = 0;
	virtual int bottom(bool withMargin) = 0;
	virtual int width(bool withMargin) = 0;
	virtual int height(bool withMargin) = 0;	

	virtual void layoutContent() {}

	virtual std::list<Connector*> getOutterConnectors() = 0;
	
	int getLevel();		
	static ContainerContent * CommonAncestor(Content *c1, Content *c2);

	void hide() { this->hidden = true; }
	void show() { this->hidden = false; }
	bool isHidden() { return this->hidden; }

protected:
	GraphLayout * layout;

	std::string cId;
	ContainerContent * container;
	
	bool hidden; // [!] really only used for compartment containers, to hide the border...
};

#endif