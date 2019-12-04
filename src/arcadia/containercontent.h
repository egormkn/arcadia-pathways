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
 *  ContainerContent.h
 *  arcadia
 *
 *  Created by Alice Villeger on 27/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#ifndef CONTAINERCONTENT_H
#define CONTAINERCONTENT_H

// Local base class
#include "content.h"

// STL
#include <list>
#include <map>

// local, for laying out children
#include "contentlayoutmanager.h"

class CloneContent;

/************
* ContainerContent *
*************
* Inherits from the abstract class Content (hence can belong to a Container)
*
* The id of a Container is determined by the static variable Count (instance number)
* 
* The Container role is to manage its Content list, aka children (that get deleted upon its deletion)
* For doing so, there are methods such as add, remove, has, and getChildren
*
* The Container also handles the layout of its content,
* thanks to the layoutContent method and an internal layoutManager 
* [!] no method to set that one!
*
* Being a Content itself, the Container has a geometry (for layout purposes)
* When setting its own position, it's actually the children that are translated
* Indeed, the position of the Container is the center of the rectangle bounding its content
* Its width and height depend on its children's dimension and internal position
* as they are computed from the difference between minimal/maximal left-right and top-bottom values
*
* Finally, although having no Connectors of its own,
* a Container is affected by its clone children's (in term of layout)
* GetInnerConnectors finds the Connectors spanning between its direct children
* GetOutterConnector finds the Connectors linking some of its clone descendants to outsiders
* [!] seems to work, but is a bit non trivial, so should double-check
*****************************************************************************************************/
class ContainerContent: public Content
{
public:
	ContainerContent(GraphLayout *gl, ContainerContent *c = NULL, std::string t = "GenericContainer", std::string l = "");
	virtual ~ContainerContent();

	bool setCore(Content * c);

	int labelWidth();
	int labelHeight();

	void setPosition(int x, int y);	
	int x();
	int y();

	int left(bool withMargin);
	int right(bool withMargin);
	int top(bool withMargin);
	int bottom(bool withMargin);
	// getFullRect() => returns the boundingRect
	int width(bool withMargin);
	int height(bool withMargin);
	
	void add(Content *c, bool asCore = false);
	void add(std::list< Content* > cList);
	void add(std::list< CloneContent* > cList);
 	void remove(Content *c);	
	bool has(Content *c);
	std::list< Content* > getChildren();
	// moveContentTo(ContainerContent *c) { c->add(this->getChildren()); }

	void layoutContent();
		
	std::list<Connector*> getInnerConnectors();	
	std::list<Connector*> getOutterConnectors();
	
	ContentLayoutStrategy getContentLayoutStrategy();
	void setContentLayoutStrategy(ContentLayoutStrategy s);

	void setContainer(ContainerContent * c);
		
	std::string getTypeLabel() { return this->containerType; }
		
	std::string getLabel() { return this->label; }
	virtual std::string getReference() { return ""; }

	Content * getCore() { return this->core; }
	
	void setUpdateFlag();

	std::string stringVersion();

	static ContainerContent * CommonAncestor(ContainerContent * c1, ContainerContent * c2);

protected:		
	std::string label;

private:
	static unsigned int Count;

	Content * core;

	std::list< Content* > children;	
	ContentLayoutManager *layoutManager;
	std::string containerType;

	bool updateFlagHori;
	bool updateFlagTop;
	bool updateFlagBottom;

	int myL;
	int myR;
	int myT;
	int myB;
	
	void updateHori(bool b);
	void updateTop(bool b);
	void updateBottom(bool b);
};

#endif
