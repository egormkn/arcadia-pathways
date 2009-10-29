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
 *  CloneContent.h
 *  arcadia
 *
 *  Created by Alice Villeger on 27/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#ifndef CLONECONTENT_H
#define CLONECONTENT_H

// Local base class
#include "content.h"

// local, for BGL_Vertex
#include "graphmodel.h"

typedef enum ConnectionType { in, out, side1, side2, neutral };

/******************
* CloneContent *
*******************
* Inherits from the abstract class Content
*
* A CloneContent is always associated to a single BGL_Vertex
* through the constructor and getVertex methods
* 
* It also belongs to a specific Graph Layout (that maps it to the Vertex)
* and (as a Content) to a Container of that Layout (by default, the root)
* [!] I never check that the Container belongs to the Layout, actually
*
* Apart from that, a Clone is characterized by:
* 
* A 2D centre position that can be read and written
* 
* Some 2D dimensions taken into account by layout managers
* (depends on the style => depends on the Vertex type, cf. properties)
* [!] at the moment, no use of the style, default values instead
*
* A bounding rectangle defined by both the position and dimensions
*
* A list of Vertex neighbours it is linked to
* which can be accessed, added, removed
*
* Slightly redundantly maybe, a list of Connectors
* which can be accessed, added, removed
* and are destroyed upon the Clone's destruction
*********************************************************************************/
class CloneContent : public Content
{
public:
	CloneContent(BGL_Vertex v, GraphLayout * l, ContainerContent * c = NULL);
	~CloneContent();

	BGL_Vertex getVertex();

	std::list<BGL_Vertex> getNeighbours();		
	void addNeighbour(BGL_Vertex v);
	void removeNeighbour(BGL_Vertex v);

	std::list<CloneContent *> getNeighbourClones();
		
	std::list<Connector*> getOutterConnectors();	
	void addConnector(Connector *c);
	void removeConnector(Connector *c);

	void setPosition(int x, int y);
	int x(ConnectionType ct);
	int y(ConnectionType ct);

	int x();
	int y();
		
	int left(bool withMargin);
	int right(bool withMargin);
	int top(bool withMargin);
	int bottom(bool withMargin);
	int width(bool withMargin);
	int height(bool withMargin);		
	
	std::string getLabel() { return this->label; }
	
	// especially useful for reactions in pathways...
	
	bool rotated;
	bool inverted;

private:
	BGL_Vertex vertex;
	std::list<BGL_Vertex> neighbours;
	std::list<Connector*> cList;
	
	int _x;
	int _y;
	std::string label;
};

#endif

/* [!] the responsibility now belongs to the CloneContent: Connector management

// Edge management
void addEdge(EdgeGraphics *edge, bool incident);
void removeEdge(EdgeGraphics *edge, bool incident);
std::list<EdgeGraphics*> getEdges(bool incoming = true, bool outgoing = true);
void destroyEdges();
bool hasEdge(EdgeGraphics *edge, bool incident);

std::list<EdgeGraphics *> inEdges;
std::list<EdgeGraphics *> outEdges;

VertexGraphics::~VertexGraphics()
{
	this->destroyEdges();
}

void VertexGraphics::addEdge(EdgeGraphics *edge, bool incident)
{
	if (!hasEdge(edge, incident))
	{
		if (incident)	this->inEdges.push_back(edge);
		else			this->outEdges.push_back(edge);
	}
}

void VertexGraphics::removeEdge(EdgeGraphics *edge, bool incident)
{
	if (incident)	this->inEdges.remove(edge);
	else			this->outEdges.remove(edge);
}

std::list<EdgeGraphics*> VertexGraphics::getEdges(bool incoming, bool outgoing)
{
	std::list<EdgeGraphics*> eList;
	
	if (incoming) eList = this->inEdges;

	if (outgoing)
	{
		if (incoming)
		{
			for (std::list<EdgeGraphics *>::iterator it = this->outEdges.begin(); it != this->outEdges.end(); ++it)
			{
				eList.push_back(*it);
			}
		}
		else eList = this->outEdges;
	}
	
	return eList;
}

void VertexGraphics::destroyEdges()
{
	std::list<EdgeGraphics *> deleteList = this->getEdges();
	for (std::list<EdgeGraphics *>::iterator it = deleteList.begin(); it != deleteList.end(); ++it) { delete (*it); }
	// when getting deleted, the edges automatically remove themselves from their source and target's list
}

bool VertexGraphics::hasEdge(EdgeGraphics *edge, bool incident)
{
	if (incident)	for (std::list<EdgeGraphics *>::iterator it = this->inEdges.begin(); it != this->inEdges.end(); ++it) {	if ((*it) == edge) return true; }
	else			for (std::list<EdgeGraphics *>::iterator it = this->outEdges.begin(); it != this->outEdges.end(); ++it) { if ((*it) == edge) return true; }
	return false;
}
*/