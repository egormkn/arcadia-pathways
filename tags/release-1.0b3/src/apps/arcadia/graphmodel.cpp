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
 *  GraphModel.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#include "graphmodel.h"

#include <iostream>

// STL
#include <sstream>
#include <math.h>
 
// local
#include "vertexproperty.h"
#include "edgeproperty.h"
#include "graphlayout.h"
#include "clonecontent.h"
#include "graphloader.h"
#include "stylesheet.h"

// [!] should I add a copy constructor for this class and subclasses?
/*
PathwayGraph* PathwayGraph::getProjection()
{
	PathwayGraph* projection = new PathwayGraph();
	
	projection->document = this->document;

	// first, adds every vertices
	vertex_iter v, v_end;
	for (tie(v, v_end) = vertices(this->graph); v != v_end; ++v) { projection->addVertex(this->vertexToNode[*v]); }	

	// then adds every edges
	edge_iter e, e_end;
	for (tie(e, e_end) = edges(this->graph); e != e_end; ++e)
	{
		Vertex uG = this->getSource(*e);
		Vertex vG = this->getTarget(*e);

		Vertex uP = projection->idToVertex[ this->vertexToNode[uG]->getId() ];
		Vertex vP = projection->idToVertex[ this->vertexToNode[vG]->getId() ];

		projection->addEdge(uP, vP, this->edgeToReference[*e]);
	}
					
	return projection;
}
*/

/**************
* Constructor *
***************
* Creates the two property maps
* and sets the fileName (by default, Untitled.graph)
****************************************************/
GraphModel::GraphModel(std::string fName, bool createStyleSheet) : fileName(fName), layoutStyleSheet(NULL)
{
	this->vertexProperties = get(vertex_name, this->graph);
	this->edgeProperties = get(edge_name, this->graph);
	
	if (createStyleSheet) {	this->layoutStyleSheet = new StyleSheet(); }
}

/*************
* Destructor *
**************
* Removes every edges and vertices
* [!] is it actually useful?
* And deletes every layouts
**********************************/
GraphModel::~GraphModel()
{
	this->deleteLayouts();
	this->removeEdges();
	this->removeVertices();
	if (this->layoutStyleSheet) delete this->layoutStyleSheet;
}

/****************************************************************************************
* Meta data and other text inputs and outputs                                           *
****************************************************************************************/

/************************************
* getFileName: Returns the fileName *
************************************/
std::string GraphModel::getFileName() {	return this->fileName; }

/************************************
* setFileName: Changes the fileName *
************************************/
void GraphModel::setFileName(std::string fName) { this->fileName = fName; }

/*************************************
* getTitle: By default, the fileName *
*************************************/
std::string GraphModel::getTitle()  { return this->getFileName(); }

/*******
* save *
********
* With the given fileName,
* saves the model as a GraphViz dot file
* and a string version text file
****************************************/
void GraphModel::save(std::string fName)
{
	if (fName == "") fName = this->getFileName();
	if (GraphLoader::Save(this, fName)) this->setFileName(fName);
}

/****************
* stringVersion *
*****************
* Lists every vertices and their properties
* And every edges and their properties
* [!] no info on which vertices the edges point at and from!
************************************************************/
std::string GraphModel::stringVersion()
{
	std::ostringstream text;

	text << "Vertices: " << num_vertices(this->graph) << std::endl;

	std::list<BGL_Vertex> vList = this->getVertices();
	unsigned int i = 0;
	for (std::list<BGL_Vertex>::iterator it = vList.begin(); it != vList.end(); ++it)
	{
		BGL_Vertex v = *it;
		VertexProperty * vp = this->vertexProperties[v];	
		text << i;
		if (vp) text << " : " << vp->stringVersion();
		text << std::endl;
		++i;
	}	

	text << "Edges: " << num_edges(this->graph) << std::endl;

	std::list<BGL_Edge> eList = this->getEdges();
	unsigned int j = 0;
	for (std::list<BGL_Edge>::iterator it = eList.begin(); it != eList.end(); ++it)
	{
		BGL_Edge e = *it;
		EdgeProperty * ep = this->edgeProperties[e];
		text << j;
		if (ep) text << " : " << ep->stringVersion();
		text << std::endl;
		++j;
	}	
		
	return text.str();
}

/****************************************************************************************
* Adding vertices and edges                                                             *
****************************************************************************************/

/************
* addVertex *
*************
* adds a new vertex, with some properties
* (created by default, or input provided)
*****************************************/
BGL_Vertex GraphModel::addVertex(VertexProperty * properties)
{
	BGL_Vertex v = add_vertex(this->graph);
	if (!properties) properties = new VertexProperty();
	this->vertexProperties[v] = properties;
	
	return v;
}

/**********
* addEdge *
***********
* adds a new edge between two vertices, with some properties
* (created by default, or input provided)
************************************************************/
BGL_Edge GraphModel::addEdge(BGL_Vertex source, BGL_Vertex target, EdgeProperty * properties)
{
	BGL_Edge e; bool inserted;

	// [!] checking for useless duplicates! 2008-05-19
	if (this->isEdge(source, target)) return this->getEdge(source, target);
	// I don't allow several edges to link the same nodes together in the same direction

	tie (e, inserted) = add_edge(source, target, this->graph);
	if (!properties) properties = new EdgeProperty();
	this->edgeProperties[e] = properties;

	return e;
}

/****************************************************************************************
* Removing vertices and edges                                                           *
****************************************************************************************/

/***************
* removeVertex *
****************
* removes all the edges first
* deletes the properties
* then removes the vertex
* [!] should also remove the cloneDescriptor from the layouts!
**************************************************************/
void GraphModel::removeVertex(BGL_Vertex v)
{
	this->removeEdges(v);
	if (this->vertexProperties[v]) delete this->vertexProperties[v];
	remove_vertex(v, this->graph);
}

/*************
* removeEdge *
**************
* deletes the properties
* then removes the edge
* [!] should also remove the connector from the layouts!
********************************************************/
void GraphModel::removeEdge(BGL_Edge e)
{
	if (this->edgeProperties[e]) delete this->edgeProperties[e];
	remove_edge(e, this->graph);
}

/*****************
* removeVertices *
******************
* removes a list of vertices
****************************/
void GraphModel::removeVertices(std::list<BGL_Vertex> vList)
{
	for (std::list<BGL_Vertex>::iterator it = vList.begin(); it != vList.end(); ++it)
	{
		this->removeVertex(*it);
	}
}

/**************************************************
* removeVertices: removes every existing vertices *
**************************************************/
void GraphModel::removeVertices() { this->removeVertices(this->getVertices()); }

/**************
* removeEdges *
***************
* removes a list of edges
*************************/
void GraphModel::removeEdges(std::list<BGL_Edge> eList)
{
	for (std::list<BGL_Edge>::iterator it = eList.begin(); it != eList.end(); ++it)
	{
		this->removeEdge(*it);
	}
}

/********************************************
* removeEdges: removes every existing edges *
********************************************/
void GraphModel::removeEdges() { this->removeEdges(this->getEdges()); }

/*****************************************************************
* removeEdges: removes all the edges connected to a given vertex *
*****************************************************************/
void GraphModel::removeEdges(BGL_Vertex v) { this->removeEdges(this->getEdges(v)); }
	
/****************************************************************************************
* Traversing vertices and edges                                                         *
****************************************************************************************/
	
/**************************************************
* getSource: obtains the source vertex of an edge *
**************************************************/
BGL_Vertex GraphModel::getSource(BGL_Edge e) { return source(e, this->graph); }

/**************************************************
* getTarget: obtains the target vertex of an edge *
**************************************************/
BGL_Vertex GraphModel::getTarget(BGL_Edge e) { return target(e, this->graph); }

/**************
* getVertices *
***************
* obtains every vertices in the graph
*************************************/	
std::list<BGL_Vertex> GraphModel::getVertices()
{
	std::list<BGL_Vertex> vertexList;

	BGL_Vertex_iter v, v_end;
	for (tie(v, v_end) = vertices(this->graph); v != v_end; ++v)
	{
		vertexList.push_back(*v);
	}
	
	return vertexList;
}

/***********
* getEdges *
************
* obtains every edges in the graph
*************************************/	
std::list<BGL_Edge> GraphModel::getEdges()
{
	std::list<BGL_Edge> edgeList;

	BGL_Edge_iter e, e_end;
	for (tie(e, e_end) = edges(this->graph); e != e_end; ++e)
	{
		edgeList.push_back(*e);
	}
	
	return edgeList;
}

/****************
* getNeighbours *
*****************
* obtains every vertices adjacent to a given vertex
***************************************************/
std::list<BGL_Vertex> GraphModel::getNeighbours(BGL_Vertex v)
{
	std::list<BGL_Vertex> neighbours;

	BGL_In_iter i, i_end;
    for (tie(i, i_end) = in_edges(v, this->graph); i != i_end; ++i)
	{
		BGL_Vertex u = source(*i, this->graph);
		neighbours.push_back(u);
	}

	BGL_Out_iter o, o_end;
    for (tie(o, o_end) = out_edges(v, this->graph); o != o_end; ++o)
	{
		BGL_Vertex u = target(*o, this->graph);
		neighbours.push_back(u);
	}

	return neighbours;
}

/***********
* getEdges *
************
* obtains every edges connected to a given vertex
*************************************************/
std::list<BGL_Edge> GraphModel::getEdges(BGL_Vertex v)
{
	std::list<BGL_Edge> edgeList;

	BGL_In_iter i, i_end;
    for (tie(i, i_end) = in_edges(v, this->graph); i != i_end; ++i)
	{
		edgeList.push_back(*i);
	}

	BGL_Out_iter o, o_end;
    for (tie(o, o_end) = out_edges(v, this->graph); o != o_end; ++o)
	{
		edgeList.push_back(*o);
	}

	return edgeList;
}

/*************
* getInEdges *
**************
* obtains every edges pointing at a given vertex
************************************************/
std::list<BGL_Edge> GraphModel::getInEdges(BGL_Vertex v)
{
	std::list<BGL_Edge> edgeList;

	BGL_In_iter i, i_end;
    for (tie(i, i_end) = in_edges(v, this->graph); i != i_end; ++i)
	{
		edgeList.push_back(*i);
	}

	return edgeList;
}

/**************
* getOutEdges *
***************
* obtains every edges pointing from a given vertex
**************************************************/
std::list<BGL_Edge> GraphModel::getOutEdges(BGL_Vertex v)
{
	std::list<BGL_Edge> edgeList;

	BGL_Out_iter o, o_end;
    for (tie(o, o_end) = out_edges(v, this->graph); o != o_end; ++o)
	{
		edgeList.push_back(*o);
	}

	return edgeList;
}

bool GraphModel::isEdge(BGL_Vertex u, BGL_Vertex v)
{
	return edge(u, v, this->graph).second;
}

BGL_Edge GraphModel::getEdge(BGL_Vertex u, BGL_Vertex v)
{
	return edge(u, v, this->graph).first;
}

/****************************************************************************************
* Obtaining properties                                                                  *
****************************************************************************************/

/***************************************************
* getProperties: return the properties of a vertex *
* [!] should probably encapsulate BGL_Vertex       *
* in a Vertex class with a getProperty method...   *
***************************************************/
VertexProperty * GraphModel::getProperties(BGL_Vertex v) { return this->vertexProperties[v]; }

/**************************************************
* getProperties: return the properties of an edge *
* [!] should probably encapsulate BGL_Edge        *
* in an Edge class with a getProperty method...   *
**************************************************/
EdgeProperty * GraphModel::getProperties(BGL_Edge e) { return this->edgeProperties[e]; }

/****************************************************************************************
* Layout information                                                                    *
****************************************************************************************/

/************
* newLayout *
*************
* deletes existing layouts
* then creates a single layout for existing edges and vertices
* The vertex layout is the default one
**************************************************************/
void GraphModel::newLayout(bool update)
{
	// reset layouts
	this->deleteLayouts();	
	
	this->defaultLayout(update);
}

void GraphModel::defaultLayout(bool update)
{
	// create one new layout
	GraphLayout *graphLayout = new GraphLayout(this, "Default Layout");
	this->layoutInformation.push_back(graphLayout);

	// adds vertices as CloneContents (1-1)
	std::list<BGL_Vertex> vList = this->getVertices();
	for (std::list<BGL_Vertex>::iterator it = vList.begin(); it != vList.end(); ++it) {
		new CloneContent(*it, graphLayout);
	}

	// adds edges as Connectors (1-1)
	std::list<BGL_Edge> eList = this->getEdges();
	for (std::list<BGL_Edge>::iterator it = eList.begin(); it != eList.end(); ++it) {
		graphLayout->connect(this->getSource(*it), this->getTarget(*it));
	}	

	if (update) graphLayout->update();
}

/****************
* deleteLayouts *
*****************
* deletes existing layouts
* and clears the layout list
****************************/
void GraphModel::deleteLayouts()
{
	for (std::vector<GraphLayout*>::iterator it = this->layoutInformation.begin(); it != this->layoutInformation.end(); ++it) { delete (*it); }
	this->layoutInformation.clear();
}

/****************************************************
* layoutNumber: returns the total number of layouts *
****************************************************/
int GraphModel::layoutNumber() { return this->layoutInformation.size(); }

/*******************************************
* getLayout: returns the "numberth" layout *
*******************************************/
GraphLayout * GraphModel::getLayout(int number) { return this->layoutInformation[number]; }

int GraphModel::getNumber(GraphLayout * gl)
{
	int n = -1;
	for (int i = 0; i < this->layoutNumber(); ++i) if (this->getLayout(i) == gl) { n = i; break; }
	return n;
}

void GraphModel::updateLayout(GraphLayout * gl, bool edgesOnly, bool fast)
{
	if (gl) gl->update(edgesOnly, fast);
	else for (int i = 0; i < this->layoutNumber(); ++i) this->getLayout(i)->update(edgesOnly, fast);
}

/****************
* toggleCloning *
*****************
* For a given vertex, for a given layout
* (or every layouts if the number given is -1)
* cloning gets toggled
**********************************************/
void GraphModel::toggleCloning(BGL_Vertex v, GraphLayout * gl, CloneContent * c)
{
	if (this->vertexProperties[v]->clonable()) 
	{
		if (gl) { if (gl->isVisible()) gl->toggleCloning(v, c); }
		else for (int i = 0; i < this->layoutNumber(); ++i) { if (this->getLayout(i)->isVisible()) this->getLayout(i)->toggleCloning(v, c); }
	}
	else
	{
		if (gl)  { if (gl->isVisible()) gl->toggleBranching(v); }
		else for (int i = 0; i < this->layoutNumber(); ++i) { if (this->getLayout(i)->isVisible()) this->getLayout(i)->toggleBranching(v); }
	}
}

GraphLayout * GraphModel::getNextLayout(GraphLayout * gl, int i)
{
	std::vector<GraphLayout*> lList = this->layoutInformation;

	if (i == -2) return lList.front();
	if (i == +2) return lList.back();	

	if (i == -1)
	{
		GraphLayout * left = lList.back();
		for (std::vector<GraphLayout*>::iterator it = lList.begin(); it != lList.end(); ++it)
		{
			if (*it == gl) break;
			left = *it;
		}
		return left;
	}
	
	if (i == +1)
	{
		GraphLayout * right = lList.front();
		bool found = false;
		for (std::vector<GraphLayout*>::iterator it = lList.begin(); it != lList.end(); ++it)
		{
			if (found) { right = *it; break; }
			if (*it == gl) found = true;
		}
		return right;
	}

	return NULL;
}

GraphLayout * GraphModel::addLayout(std::list<BGL_Vertex> vList, bool neighbourhood)
{
	// finding what would be the name of the new layout
	std::string layoutId;
	if (vList.empty()) layoutId = "Default Layout";
	else
	{
		layoutId = "Around";
		for (std::list<BGL_Vertex>::iterator it = vList.begin(); it != vList.end(); ++it)
		{
			layoutId += " " + this->vertexProperties[*it]->getLabel();
		}		
	}

	// checking if it exists already
	GraphLayout * layout = NULL;
	std::vector<GraphLayout*> lList = this->layoutInformation;
	for (std::vector<GraphLayout*>::iterator it = lList.begin(); it != lList.end(); ++it)
	{
		std::cout << (*it)->name << " VS " << layoutId << std::endl;
		if ((*it)->name == layoutId) { layout = *it; break; }
	}

	if (layout) return layout;


	if (vList.empty())
	{
		this->defaultLayout(true);
	}
	else
	{
		GraphLayout *graphLayout = new GraphLayout(this, layoutId);
		this->layoutInformation.push_back(graphLayout);

		int d = 0;
		std::list<CloneContent*> roots;
		for (std::list<BGL_Vertex>::iterator it = vList.begin(); it != vList.end(); ++it)
		{
			BGL_Vertex v = *it;
			CloneContent * clone = new CloneContent(v, graphLayout);
			// clone->setSelected(true);
			roots.push_back(clone);
		
			if (this->vertexProperties[v]->clonable()) d = 2;
			else if (!d) d = 1;
		}

		if (neighbourhood)
		{
			graphLayout->recursiveNeighbourhoodBuilding(roots, roots, d);
			graphLayout->setLayoutStrategy(d == 2? Neighbourhood: Clone);
		}
		else
		{
			graphLayout->fillGaps(roots);
		}

		graphLayout->update();
	}

	return NULL;
}

void GraphModel::expandLayout(std::list<BGL_Vertex> vList, GraphLayout * gl)
{
	if (gl) { if (gl->isVisible()) gl->expand(vList); }
	else for (int i = 0; i < this->layoutNumber(); ++i)
	{
		if (this->getLayout(i)->isVisible())
		{
			this->getLayout(i)->expand(vList);
		}
	}
}

void GraphModel::toggleAvoidingEdges()
{
	for (int i = 0; i < this->layoutNumber(); ++i)
	{
		if (this->getLayout(i)->isVisible()) this->getLayout(i)->toggleAvoiding();
	}
}

GraphLayout * GraphModel::destroyLayout()
{
	GraphLayout* del = NULL;
	if (this->layoutNumber() < 2) return del;

	std::vector<GraphLayout*> newList;
	for (int i = 0; i < this->layoutNumber(); ++i)
	{
		GraphLayout *gl = this->getLayout(i);
		if (gl->isVisible()) { del =  gl; }
		else { newList.push_back(gl); }
	}

	if (del)
	{
		if (newList.size() != this->layoutInformation.size() - 1) std::cout << "Problem, more than one visible layout!" << std::endl;
		this->layoutInformation = newList;
		delete del;
	}
	
	return del;
}

/*
void PathwayScene::rightClickNodes( std::list<NodeView*> nodes ) {
	PathwayScene *mainScene = NULL;
	PathwayScene *oldBackstage = NULL;
	PathwayScene *newBackstage = NULL;
	PathwayScene *newFocus = NULL;

	if (!this->displayNeighbourhood) { // we are in the main scene, we want to display a neighbourhood now
		if (nodes.empty()) return; // but we need something to display...

		mainScene = this;
		oldBackstage = this->backstage;
	}
	else { // we are currently displaying a neighbourhood
		mainScene = this->backstage;
		oldBackstage = this;

		if (nodes.empty()) newFocus = mainScene; // by default, we switch back to the main scene			
		// But if some nodes are selected, we display their neighbourhood instead
	}

	if (!newFocus) {
		mainScene->clearSelection();
	
		// we create a new backstage that is linked to the current main scene
		newBackstage = new PathwayScene(mainScene);

		bool species = nodes.front()->getVertex().isSpecies();

		// building the neighbourhood
		std::list<NodeView*> roots;
		for (std::list<NodeView*>::iterator it = nodes.begin(); it != nodes.end(); ++it) {
			PathwayVertex v = (*it)->getVertex();
			// if the vertex is already here, skip (clone case)
			if (newBackstage->vertexIdToNodeViews.find(v.getLabel()) != newBackstage->vertexIdToNodeViews.end()) continue;
						
			NodeView* newNode = NULL;
			if (species && v.isSpecies())	newNode = newBackstage->addSpeciesView(v);
			if (!species && v.isReaction())	newNode = newBackstage->addReactionView(v);
			if (newNode) {
				newNode->setSelected(true); roots.push_back(newNode);
				std::list<NodeView*> mainSceneNodes = mainScene->vertexIdToNodeViews[v.getLabel()];
				for (std::list<NodeView*>::iterator ni = mainSceneNodes.begin(); ni != mainSceneNodes.end(); ++ni) { (*ni)->setSelected(true); }
			}
		}
		int d = species? 2: 1;
		std::list<NodeView*> neighbourHood = newBackstage->recursiveNeighbourhoodBuilding(roots, roots, d);

		newBackstage->mainContainer->changeNodePlacement(species? Neighbourhood: Cofactors);
		newBackstage->layout();
		newFocus = newBackstage;
	}
		
	emit(sceneSwitch(newFocus));
	mainScene->backstage = newBackstage;
	if (oldBackstage) delete oldBackstage;
}
*/