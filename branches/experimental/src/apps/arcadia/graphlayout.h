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
 *  GraphLayout.h
 *  arcadia
 *
 *  Created by Alice Villeger on 27/02/2008.
 *	Last Documented on 17/04/2008.
 *
 */

#ifndef GRAPHLAYOUT_H
#define GRAPHLAYOUT_H

// local (for BGL_Vertex)
#include "graphmodel.h"
#include "containercontent.h"

// local (managed by the GraphLayout)
class CloneContent;
class Connector;
class ConnectorLayoutManager;
//class StyleSheet;
class StyleSheet;

/**************
* GraphLayout *
***************
* This class manages a set of objects describing a layout for a given graph
* [!] maybe I should keep a reference to the graph here? (a pointer)
* These objects are usually created by, and ALWAYS destroyed along with the layout
* Rem: in many ways, this replaces some of the old LayoutGraphView roles
*
* CloneContent objects
* A CloneContent always refer to a BGL_Vertex
* The GraphLayout class provides a mapping from each BGL_Vertex to a list of its clones
* The map can be written with the map and unmap methods
* For a given vertex, the map is read through the getClone/s methods
* A list of all clones can also be obtained with getCloneContents
*
* Root Container
* Every Content type objects (CloneContent and Container) are listed in a tree
* The root of that tree is accessible through the getRoot method
* By default, any newly mapped CloneContent gets placed at the root
* The Root can be laid out automatically thanks to the update method
* The Root is created automatically with a default LayoutManager
* [!] should let configure the root layout method
* When the root is destroyed, so are every objects it contains
* [!] but the mapping methods doesn't check a clone given with a Container belongs to the tree
*
* Connector objects
* A Connector is equivalent to a BGL_Edge (1-1 relationship)
* But instead of linking 2 BGL_Vertex, they link the 2 corresponding clones
* Connectors are created with the connect method stating two BGL_Vertex
* Connectors are accessed with the getConnector method stating two BGL_Vertex
* or through a method returning the global connector list
* Internally, Connectors are kept as a list, and refer to CloneContents
* The GraphLayout class has a private method for finding the clone
* corresponding to a given (vertex, neighbour) pair
* Connectors are normally destroyed along with the Clone they point at
*
* Specific actions can be performed on the GraphLayout
* in particular cloning related action on a given vertex
***********************************************************************************************/
class GraphLayout
{
public:
	GraphLayout(GraphModel * gm, std::string n);
	~GraphLayout();

	// Root Container
	ContainerContent *getRoot();
	void update(bool edgesOnly = false, bool fast = false);

	// CloneContent
	void map(BGL_Vertex v, CloneContent * cd);
	void unmap(CloneContent * cd);	
	CloneContent * getClone(BGL_Vertex v); // the first one
	std::list<CloneContent*> getClones(BGL_Vertex v);
	std::list< CloneContent * > getCloneContents();

	// Connector
	Connector * connect(BGL_Edge e);
	Connector * connectClones(CloneContent * u, CloneContent * v, BGL_Edge e);
	Connector * getConnector(BGL_Vertex u, BGL_Vertex v, BGL_Edge e);	
	std::list< Connector * > getConnectors();

	// cloning/branching actions
	void toggleCloning(BGL_Vertex v, CloneContent * c);
	void toggleBranching(BGL_Vertex v);

	GraphModel * getGraphModel () { return this->graphModel; }
				
	std::list<CloneContent*> recursiveNeighbourhoodBuilding(std::list<CloneContent*> roots, std::list<CloneContent*> neighbourhood, int depth);
	void setLayoutStrategy(ContentLayoutStrategy s) { this->root->setContentLayoutStrategy(s); }

//	StyleSheet * getStyleSheet() { return this->styleSheet; }
	StyleSheet * getStyleSheet() { return this->layoutStyleSheet; }
	
	bool isVisible() { return this->visible; }
	void setVisible(bool v) { this->visible = v; }

	bool isAvoiding() { return this->avoiding; }
	void toggleAvoiding() { this->avoiding = !this->avoiding; }
	void setAvoiding(bool v) { this->avoiding = v; }

/*
	void expand(std::list<BGL_Vertex> vList);
*/
/*
	void fillGaps(std::list<CloneContent*> roots);
*/

	std::string name;
	
	std::list<Connector *> inList;
	std::list<Connector *> outList;

	void map(ContainerContent * container);
	void unMap(ContainerContent * container);

private:
	bool visible;
	bool avoiding;

	std::list<CloneContent*> buildNeighbours(CloneContent * clone, std::list<CloneContent *> neighbourhood, bool isVisible);
	std::list<CloneContent*> addNeighbourFromEdge(BGL_Edge edge, CloneContent * clone,
		std::list<CloneContent *> neighbourhood, bool incoming, bool isVisible);

	GraphModel * graphModel;

	void clone(BGL_Vertex v);
	void unclone(BGL_Vertex v, CloneContent * c);

	void branch(CloneContent * c, ContainerContent * parent, ContainerContent * cloneContainer);
	void unbranch(CloneContent * c, ContainerContent * parent, ContainerContent * cloneContainer);

	ContainerContent *root;
	std::map< BGL_Vertex, std::list<CloneContent*> > cloneMap;
	std::list< Connector * > connectorList;
	
	CloneContent * findClone (BGL_Edge edge, bool isSource);
	
	ConnectorLayoutManager * connectorLayoutManager;
	
//	StyleSheet * styleSheet;
	StyleSheet * layoutStyleSheet;
	
	std::map< std::string, ContainerContent* > refToContainer;
};

#endif