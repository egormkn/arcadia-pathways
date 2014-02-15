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
 *  Connector.h
 *  arcadia
 *
 *  Created by Alice Villeger on 01/04/2008.
 *	Last Documented on 17/04/2008.
 *
 */

#ifndef CONNECTOR_H
#define CONNECTOR_H

// STL for std::pair
#include <utility>
#include <list>
#include <iostream>

// local
#include "clonecontent.h"
class Content;
class ContainerContent;

/************
* Connector *
*************
* Ultimately connects a pair of CloneContent
* In the process, may go through a series of Container
*
* The constructor defines the source and target
* The destructor notifies source and target that the connector died
* The final source and target can be accessed through getSource and getTarget
* and modified through setSource and setTarget
*
* The local source and target (from a Container's perspective)
* are accessed through getSourceContent and getTargetContent
*
* The position of the source or target are obtained with getPoint
*****************************************************************************/
class Connector
{
public:
	Connector(CloneContent * s, CloneContent * t, BGL_Edge e);
	~Connector();

	CloneContent * getNeighbour(CloneContent * c);
	
	CloneContent * getSource();
	CloneContent * getTarget();

	Content * getSourceContent(ContainerContent * caller);
	Content * getTargetContent(ContainerContent * caller);
	
	std::pair<int, int> getPoint (bool isSource, bool intersectionDetection = false);

	void setSource(CloneContent * c);
	void setTarget(CloneContent * c);

	void setPoints(std::list< std::pair <int, int> > p) { this->points = p; }
	std::list< std::pair <int, int> > getPoints() { return this->points; }

	void quickTranslate();
	void quickUpdate();

	ConnectionType getSourceConnection() { return this->sourceConnection; }
	ConnectionType getTargetConnection() { return this->targetConnection; }

	void setSourceConnection(ConnectionType ct) { this->sourceConnection = ct; this->quickUpdate(); }
	void setTargetConnection(ConnectionType ct) { this->targetConnection = ct; this->quickUpdate(); }

	BGL_Edge getEdge() { return this->edge; }
	
private:	
	BGL_Edge edge;

	CloneContent * source;
	CloneContent * target;
	
	std::list< std::pair <int, int> > points;

	ConnectionType sourceConnection;
	ConnectionType targetConnection;
};

#endif

/*
// [!] from EdgeGraphics...
bool EdgeGraphics::pointsAt(VertexGraphics *v, bool isSource) const
{
	return ( v == this->getVertex(isSource) );
}

void EdgeGraphics::setVertex(VertexGraphics *v, bool isSource)
{
	VertexGraphics *oldVertex = this->getVertex(isSource);
	if (oldVertex == v) return;
	
	if (isSource)	this->sourceVertex = v;
	else			this->targetVertex = v;

	if (oldVertex)	oldVertex->removeEdge(this, !isSource);
	if (v)			v->addEdge(this, !isSource);
}

VertexGraphics* EdgeGraphics::getVertex(bool isSource) const
{
	if (isSource)	return this->sourceVertex;
	else			return this->targetVertex;
}
*/
