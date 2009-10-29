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
 *  Container.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 27/02/2008.
 *	Last Documented on 17/04/2008.
 *
 */

#include "containercontent.h"

#include "clonecontent.h"
#include "containerstyle.h"
#include "graphlayout.h"
#include "stylesheet.h"

// STL
#include <sstream>
#include <iostream>

// [!] wild guesses on the font size!!
int ContainerContent::labelWidth() { return this->label.size()*7.5; }
int ContainerContent::labelHeight() { return (label!="")? 20: 0; }

/**************************************
* Count: To determine the instance Id *
**************************************/
unsigned int ContainerContent::Count = 0;

void ContainerContent::setUpdateFlag()
{
	this->updateFlagHori = true;
	this->updateFlagTop = true;	
	this->updateFlagBottom = true;	

	this->myL = 0;
	this->myR = 0;
	this->myT = 0;
	this->myB = 0;
	
	if (this->container) this->container->setUpdateFlag();
}

/**************
* Constructor *
***************
* classic Content initialization
* + sets up the id (instance number)
* and creates a default layoutManager
* with a rotation opposite of that of its Container
***************************************************/
ContainerContent::ContainerContent(GraphLayout *gl, ContainerContent *c, std::string t, std::string l) : Content(gl, c), containerType(t), label(l), core(NULL)
{
	this->setUpdateFlag(); // the dimensions have not been computed yet

	std::ostringstream oss;
	oss << ContainerContent::Count++;
	this->cId = oss.str();

	this->layoutManager = ContentLayoutManager::GetLayoutManager();
}

void ContainerContent::setContainer(ContainerContent *c)
{
	Content::setContainer(c);
	if (this->container && this->layoutManager && this->layoutManager->getType() != Hierarchy)
	{
		this->layoutManager->setRotation(!this->container->layoutManager->getRotation());
	}
}

/*************
* Destructor *
**************
* Deletes all the children
* and the layout manager
**************************/
ContainerContent::~ContainerContent()
{
	std::list<Content*> cList = this->getChildren(); // we will destroy the children, hence remove them, so we need to browse thoug a copy of the list
	for (std::list<Content*>::iterator it = cList.begin(); it != cList.end(); ++it) delete *it;

	delete this->layoutManager;
}

/***********************************************************************
* Position                                                             *
***********************************************************************/

/**************
* setPosition *
***************
* Computes the corresponding translation from the current position
* Translates all the children to their new position
* (this will naturally update the Container's own position)
******************************************************************/
void ContainerContent::setPosition(int x, int y)
{
	Content::setPosition(x, y);

	int dX = x - this->x();
	int dY = y - this->y();

	for (std::list<Content*>::iterator it = this->children.begin(); it != this->children.end(); ++it)
	{
		(*it)->setPosition((*it)->x() + dX, (*it)->y() + dY);
	}
}


std::string ContainerContent::stringVersion()
{
	std::string s = "";
	for (std::list<Content*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		s += (*it)->getId() + "; ";
	}
	return s;
}

/*****************************************************
* x: returns the middle point between left and right *
*****************************************************/
int ContainerContent::x()
{
	int value;
	
	// the condition on compartments means their core gets ignored layout wise
	if (this->core && this->getTypeLabel() != "CompContainer")
	{
		value = this->core->x();
	}
	else // the center is used instead
	{
		value = (this->left(true)+this->right(true)) / 2;
	}
	return value;
}
/*****************************************************
* y: returns the middle point between top and bottom *
*****************************************************/
int ContainerContent::y()
{
	int value;

	// the condition on compartments means their core gets ignored layout wise
	if (this->core && this->getTypeLabel() != "CompContainer")
 	{
		value = this->core->y();
	}
	else // the center is used instead
	{
		value = (this->top(true)+this->bottom(true)) / 2;
	}
	return value;
}

/***********************************************************************
* Position and dimension                                               *
***********************************************************************/

/*******
* left *
********
* Finds the minimal left border among children
**********************************************/
int ContainerContent::left(bool withMargin)
{
	if (this->children.empty()) return 0;
	this->updateHori(withMargin);
	return this->myL;
}

/********
* right *
*********
* Finds the maximal right border among children
***********************************************/
int ContainerContent::right(bool withMargin)
{
	if (this->children.empty()) return 0;
	this->updateHori(withMargin);	
	return this->myR;
}

void ContainerContent::updateHori(bool withMargin)
{
	if (!this->updateFlagHori) return;

	std::list<Content*>::iterator it = this->children.begin();
	int r = (*it)->right(true);
	int l = (*it)->left(true);
	for (++it; it != this->children.end(); ++it)
	{
		if ((*it)->right(true)  > r) r = (*it)->right(true);			
		if ((*it)->left(true) < l) l = (*it)->left(true);
	}
	
	if (r-l < this->labelWidth()) { l = (r+l)/2 - this->labelWidth()/2; }
	if (r-l < this->labelWidth()) { r = (r+l)/2 + this->labelWidth()/2; }

	int offset;
	ContainerStyle * cs = this->layout->getStyleSheet()->getContainerStyle(this->getLabel());
	if (withMargin)  offset= cs->getFullOffset()/2; // padding + border + margin
	else offset = cs->getBoundingOffset()/2; // padding + border
	l -= offset;
	r += offset;
	
	this->myL = l;	
	this->myR = r;	

	this->updateFlagHori = false;
}

/******
* top *
*******
* Finds the minimal top border among children
*********************************************/
int ContainerContent::top(bool withMargin)
{
	if (this->children.empty()) return 0;
	this->updateTop(withMargin);
	return this->myT;
}

/*********
* bottom *
**********
* Finds the maximal bottom border among children
*************************************************/
int ContainerContent::bottom(bool withMargin)
{
	if (this->children.empty()) return 0;
	this->updateBottom(withMargin);
	return this->myB;
}

void ContainerContent::updateTop(bool withMargin)
{
	if (!this->updateFlagTop) return;

	std::list<Content*>::iterator it = this->children.begin();
	int t = (*it)->top(true);
	for (++it; it != this->children.end(); ++it)
	{
		if ((*it)->top(true) < t) t = (*it)->top(true);
	}

	int offset;
	ContainerStyle * cs = this->layout->getStyleSheet()->getContainerStyle(this->getLabel());
	if (withMargin)  offset= cs->getFullOffset()/2; // padding + border + margin
	else offset = cs->getBoundingOffset()/2; // padding + border
	t -= offset;

	// extra space for the label
	t -= this->labelHeight();

	this->myT = t;

	this->updateFlagTop = false;
}

void ContainerContent::updateBottom(bool withMargin)
{
	if (!this->updateFlagBottom) return;

	std::list<Content*>::iterator it = this->children.begin();
	int b = (*it)->bottom(true);
	for (++it; it != this->children.end(); ++it)
	{
		if ((*it)->bottom(true) > b) b = (*it)->bottom(true);			
	}

	int offset;
	ContainerStyle * cs = this->layout->getStyleSheet()->getContainerStyle(this->getLabel());
	if (withMargin)  offset= cs->getFullOffset()/2; // padding + border + margin
	else offset = cs->getBoundingOffset()/2; // padding + border
	b += offset;

	this->myB = b;

	this->updateFlagBottom = false;
}

/***********************************************************************
* Dimension                                                            *
***********************************************************************/

/*******************************
* width: right left difference *
*******************************/
int ContainerContent::width(bool withMargin) { return this->right(withMargin) - this->left(withMargin); }

/********************************
* height: top bottom difference *
********************************/
int ContainerContent::height(bool withMargin) { return this->bottom(withMargin) - this->top(withMargin); }

/***********************************************************************
* Content management                                                   *
***********************************************************************/

/******
* add *
*******
* adds the new child to the list
* and sets itself up as its container
*************************************/
void ContainerContent::add(Content *c, bool asCore)
{
	if (asCore) this->core = c;
	if (!this->has(c)) this->children.push_back(c);
	if (!c->hasContainer(this)) c->setContainer(this);
}

void ContainerContent::add(std::list<Content *> cList)
{
	for (std::list<Content *>::iterator it = cList.begin(); it != cList.end(); ++it) { this->add(*it); }
}

void ContainerContent::add(std::list<CloneContent *> cList)
{
	for (std::list<CloneContent *>::iterator it = cList.begin(); it != cList.end(); ++it) { this->add(*it); }
}
	
/*********
* remove *
**********
* removes the child from the list
* and sets its container to nothing
***********************************/
void ContainerContent::remove(Content *c)
{
	this->children.remove(c);
	c->setContainer(NULL);
	if (c == this->core) this->core = NULL; // what is the new core, though???
}
	
/******
* has *
*******
* checks whether a Content is in the children list
**************************************************/
bool ContainerContent::has(Content *c)
{
	return ( std::find(this->children.begin(), this->children.end(), c) != this->children.end() );
}

/****************************************
* getChildren: returns all the children *
****************************************/
std::list<Content*> ContainerContent::getChildren() { return this->children; }

/***********************************************************************
* Content layout                                                       *
***********************************************************************/

/****************
* layoutContent *
*****************
* Asks all the children to perform their own layout
* then performs its own layout
***************************************************/
void ContainerContent::layoutContent()
{
	for (std::list<Content*>::iterator it = this->children.begin(); it != this->children.end(); ++it)
	{
		(*it)->layoutContent();
	}
	this->layoutManager->layout(this);
}

/***********************************************************************
* Connector management                                                 *
***********************************************************************/

/********************
* getInnerConnector *
*********************
* Browses the children's OutterConnector lists
* Returns every connector that appears twice:
* it means they begin and they end inside an offspring of the Container
* + they spring out of the Container's direct children
* [!] it works, but what about efficiency?
***********************************************************************/
std::list<Connector*> ContainerContent::getInnerConnectors()
{
	std::list<Connector*> iList;

	std::map<Connector*, int> cMap;		
	for (std::list< Content* >::iterator it = this->children.begin(); it != this->children.end(); ++it)
	{
		std::list<Connector*> oList = (*it)->getOutterConnectors();
		for (std::list<Connector*>::iterator ot = oList.begin(); ot != oList.end(); ++ot)
		{
			if (cMap.find(*ot) == cMap.end()) { cMap[*ot] = 1; }
			else { cMap[*ot]++; }
		}
	}
	for (std::map<Connector*, int>::iterator it = cMap.begin(); it != cMap.end(); ++it)
	{
		if ((*it).second == 2) iList.push_back((*it).first);
	}
		
	return iList;
}
	
/********************
* getOutterConnector *
*********************
* Browses the children's OutterConnector lists
* Returns every connector that appears only once:
* it means they begin or end inside an offspring of the Container
* but end or begin at a point that is outside of the Container
* [!] it works, but what about efficiency?
***********************************************************************/
std::list<Connector*> ContainerContent::getOutterConnectors()
{
	std::list<Connector*> iList;

	std::map<Connector*, bool> cMap;		
	for (std::list< Content* >::iterator it = this->children.begin(); it != this->children.end(); ++it)
	{
		std::list<Connector*> oList = (*it)->getOutterConnectors();
		for (std::list<Connector*>::iterator ot = oList.begin(); ot != oList.end(); ++ot)
		{
			if (cMap.find(*ot) == cMap.end()) { cMap[*ot] = true; }
			else { cMap.erase(*ot); }
		}
	}
	for (std::map<Connector*, bool>::iterator it = cMap.begin(); it != cMap.end(); ++it)
	{
		iList.push_back((*it).first);
	}
		
	return iList;	
}

ContentLayoutStrategy ContainerContent::getContentLayoutStrategy() { return this->layoutManager->getType(); }

void ContainerContent::setContentLayoutStrategy(ContentLayoutStrategy s)
{
	if (this->getContentLayoutStrategy() == s) return;
	if (this->layoutManager) delete this->layoutManager;
	this->layoutManager = ContentLayoutManager::GetLayoutManager(s);
	if ((s == Clone) && this->container)
	{
		this->layoutManager->setRotation(!this->container->layoutManager->getRotation());	
	}
	
	if (s == Clone) this->containerType = "CloneContainer";
	if (s == Branch) this->containerType = "BranchContainer";
	if (s == Triangle) this->containerType = "TriangleContainer";
	if (s == Neighbourhood) this->containerType = "TriangleContainer";
}