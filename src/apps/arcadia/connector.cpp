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
 *  Connector.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 01/04/2008.
 *  Last documented on 17/04/2008.
 *
 */

#include "connector.h"

#include <iostream>

#include "clonecontent.h"
#include "containercontent.h"

// [!] useless? (cf quickUpdate)
void Connector::quickTranslate()
{
	std::list< std::pair <int, int> > controlPoints;
	controlPoints.push_back(this->getPoint(true));
	controlPoints.push_back(this->getPoint(false));
	this->setPoints(controlPoints);
	// the shape didn't change: target - source = the same
}

void Connector::quickUpdate()
{
	std::list< std::pair <int, int> > controlPoints;
	controlPoints.push_back(this->getPoint(true));
	controlPoints.push_back(this->getPoint(false));
	this->setPoints(controlPoints);
	// the shape changed...
}

/**************
* Constructor *
***************
* sets up the source and target
* and notify them of their new Connector
****************************************/
Connector::Connector(CloneContent *s, CloneContent *t) : source(NULL), target(NULL), sourceConnection(neutral), targetConnection(neutral)
{
	this->setSource(s);
	this->setTarget(t);

	// this->quickUpdate(); // [!] ?
	std::list< std::pair <int, int> > controlPoints;
	controlPoints.push_back(this->getPoint(true));
	controlPoints.push_back(this->getPoint(false));
	this->setPoints(controlPoints);	
}

CloneContent * Connector::getNeighbour(CloneContent * c)
{
	CloneContent * n = NULL;
	if (c == this->getSource()) n = this->getTarget();
	if (c == this->getTarget()) n = this->getSource();
	return n;
}

/*************
* Destructor *
**************
* sets up the source and target to NULL
* notify the old ones of their loss
***************************************/
Connector::~Connector()
{
	this->setSource(NULL);
	this->setTarget(NULL);
}

/*************************************************
* getSource: returns the source clone descriptor *
*************************************************/
CloneContent* Connector::getSource() { return this->source; }

/*************************************************
* getTarget: returns the target clone descriptor *
*************************************************/
CloneContent* Connector::getTarget() { return this->target; }

/************
* setSource *
*************
* notifies the old source of the change
* and links the connector to the new source
*******************************************/
void Connector::setSource(CloneContent* c)
{
	if (this->getSource() == c) return;
	if (this->getSource()) this->getSource()->removeConnector(this);
	this->source = c;
	if (this->getSource()) this->getSource()->addConnector(this);
}

/************
* setTarget *
*************
* notifies the old target of the change
* and links the connector to the new target
* [!] very similar to getSource...
*******************************************/
void Connector::setTarget(CloneContent* c)
{
	if (this->getTarget() == c) return;
	if (this->getTarget()) this->getTarget()->removeConnector(this);	
	this->target = c;
	if (this->getTarget()) this->getTarget()->addConnector(this);
}

/*******************
* getSourceContent *
********************
* returns the child of the caller container that contains the source
* Actually, we start at the source and go up one Container
* until the caller Container is found as the current parent
********************************************************************/
Content* Connector::getSourceContent(ContainerContent *caller)
{
	Content *c = this->getSource();
	while (c && !c->hasContainer(caller)) c = c->getContainer();
	return c;
}
	
/*******************
* getTargetContent *
********************
* returns the child of the caller container that contains the target
* Actually, we start at the target and go up one Container
* until the caller Container is found as the current parent
* [!] very similar to getSourceContent...
********************************************************************/
Content* Connector::getTargetContent(ContainerContent *caller)
{
	Content *c = this->getTarget();	
	while (c && !c->hasContainer(caller)) c = c->getContainer();
	return c;
}

/***********
* getPoint *
************
* returns the pair of coordinates of the source or target
* [!] keep that method private and add a getSourcePoint, getTargetPoint interface?
**********************************************************************************/
// method changed to accomodate for pointing at an input/output point, not the center of a cloneContent (e.g. reactions)
std::pair<int, int> Connector::getPoint (bool isSource, bool intersectionDetection) 
{
	std::pair<int, int> p;

	CloneContent *cd1 = (isSource)? this->getSource(): this->getTarget();
	ConnectionType ct1 = (isSource)? this->sourceConnection: this->targetConnection;
	std::pair<int, int> p1; p1.first = cd1->x(ct1); p1.second = cd1->y(ct1);

	if (!intersectionDetection) return p1;

	// I'd like the crossing point between the connector and the shape, actually
	// useful for saving layout in sbml layout extension
	// first I need the other end of my connector
	CloneContent *cd2 = (!isSource)? this->getSource(): this->getTarget();
	ConnectionType ct2 = (!isSource)? this->sourceConnection: this->targetConnection;
	std::pair<int, int> p2;	p2.first = cd2->x(ct2); p2.second = cd2->y(ct2);

	// then I need to consider a straight line between these two points
	float d = sqrt((p1.first-p2.first)*(p1.first-p2.first) + (p1.second-p2.second)*(p1.second-p2.second));
	float interval = (float) 1 / d;
	for (float f=0; f<=1; f+=interval)
	{
		p.first = f*p2.first + (1-f)*p1.first;
		p.second = f*p2.second + (1-f)*p1.second;

		// and see where the lines crosses the shape of my node
		// to make things simpler, I'll just consider the bounding rect...
		if (p.first <= cd1->left(false)) { break; }
		if (p.first >= cd1->right(false)) { break; }
		if (p.second <= cd1->top(false)) { break; }
		if (p.second >= cd1->bottom(false)) { break; }
	}			

	return p;
}