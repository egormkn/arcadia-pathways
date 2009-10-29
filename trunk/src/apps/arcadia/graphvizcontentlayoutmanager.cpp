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
*  Additional permission under GNU GPL version 3 section 7
*
*  If you modify this Program, or any covered work, by linking or
*  combining it with graphviz (or a modified version of that library),
*  containing parts covered by the terms of the Common Public License 1.0,
*  the licensors of this Program grant you additional permission to convey
*  the resulting work.
*
*************************************************************************/

/*
 *  GraphvizContentLayoutManager.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 06/03/2008.
 *  Last documented on 17/04/2008.
 *
 */

#include "graphvizcontentlayoutmanager.h"

#ifdef MAC_COMPILATION // [!] Hack to preload plugins on Mac

#ifndef LTPRELOADEDSYMBOLS
#define LTPRELOADEDSYMBOLS
#include "graphviz/gvplugin.h"

extern gvplugin_library_t *gvplugin_dot_layout_LTX_library;
extern gvplugin_library_t *gvplugin_neato_layout_LTX_library;

const lt_symlist_t lt_preloaded_symbols[] = {
{ "gvplugin_dot_layout_LTX_library", (void*)(&gvplugin_dot_layout_LTX_library) },
{ "gvplugin_neato_layout_LTX_library", (void*)(&gvplugin_neato_layout_LTX_library) },
{ 0, 0 }
};
#endif

#endif // end of hack [!]

#include <iostream>

// STL
#include <sstream>
#include <list>
#include <stack>

// local
#include "containercontent.h"
#include "connector.h"

#include "clonecontent.h" // [!] for label display

/**************************************************************************************************************************
* Constructor: Initialises the graph to NULL, and the graphContext to a new one. Also sets up the method (dot by default) *
**************************************************************************************************************************/
GraphvizContentLayoutManager::GraphvizContentLayoutManager(std::string m) : method (m), graph(NULL)
{
	this->graphContext = gvContext();
}

/*************
* Destructor *
**************
* closes the graph (if any)
* and frees the context
***************************/
GraphvizContentLayoutManager::~GraphvizContentLayoutManager()
{
	if (this->graph) agclose(this->graph);
	gvFreeContext(this->graphContext);
}

/*********
* layout *
**********
* First resets the graph and the map
* Loads the container's content as nodes of the graph
* (sets their width and height, depending on rotation)
* [!] ATM the clone dimensions are arbitrarily fixed
* Then loads the container's inner connectors as edges
* [!] should check for oddly "missing" edges
* Computes the layout with the proper method
* Converts the graphviz coordinates in the local system
* (depends on the rotation factor again)
* and sets the new value of each content's position
* [!] the zoom value is pretty arbitrary
* [!] the logical center of the content is not used...
*******************************************************/
void GraphvizContentLayoutManager::layout(ContainerContent * container)
{
/*
Agraph_t* G;
GVC_t* gvc;
gvc = gvContext();
// create graph
gvLayout (gvc, G, "dot");
// Draw graph
gvFreeLayout(gvc, g);
agclose (G);
gvFreeContext(gvc);
*/

	// [!] hack! Let's totally forget about Triangle layout: it gets dealt with in its parent container
	if (this->strategy == Triangle) return;
	// [!] end of hack!
		
	////////////////////////////////////////////////////////////////////////////////////////////////

	// reset graphviz objects
	this->contentToGraphVizNode.clear();
	if (this->graph) agclose(this->graph);

	////////////////////////////////////////////////////////////////////////////////////////////////

	// security for unmanageable graphs : uses a simpler layout method instead
	if (container->getChildren().size() > 280)
	{
		std::cout << "security" << std::endl;
		ContentLayoutManager * layoutManager = ContentLayoutManager::GetLayoutManager(Automatic);
		layoutManager->layout(container);
		delete layoutManager;
		return;
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	// beginning of the normal process
	this->graph = agopen("g", AGDIGRAPH);

	////////////////////////////////////////////////////////////////////////////////////////////////

	std::list<Content*> cList = container->getChildren();

	// [!] hack! for triangle containers children of the main container...
	std::list<ContainerContent*> tList = this->initTriangleContainerNodes(cList);
	// [!] end of hack!

	// We turn the children of the main container into graphviz nodes of similar-ish(?) dimensions
	int nodeId = 0;
	for (std::list<Content*>::iterator it= cList.begin(); it != cList.end(); ++it)
	{
		Content * c = (*it);
				
		std::ostringstream nodeName; nodeName << nodeId++;
		Agnode_t * gvNode = agnode(graph, (char*)nodeName.str().c_str());
		this->contentToGraphVizNode[c] = gvNode;

		std::ostringstream w;	w <<  c->width(true)/50; std::ostringstream h;	h <<  c->height(true)/50;
		if (this->rotation)	{ agsafeset(gvNode, "width", (char*)h.str().c_str(), "");	agsafeset(gvNode, "height", (char*)w.str().c_str(), ""); }
		else				{ agsafeset(gvNode, "width", (char*)w.str().c_str(), "");	agsafeset(gvNode, "height", (char*)h.str().c_str(), ""); }
	}
	if (nodeId == 0) return; // if there is none, no need to layout any further...

	////////////////////////////////////////////////////////////////////////////////////////////////

	// we obtain the container's connectors between its first generation of children
	std::list<Connector*> connectors = container->getInnerConnectors();
	// we also obtain connectors that may need be inverted (cf crossroads of branch containers)
	std::list<Connector *> inverseList = GraphvizContentLayoutManager::dealWithBranches(container, connectors);

	// We turn the inner connectors into graphviz edges (inverting the ones that need be)		
	for (std::list<Connector*>::iterator it = connectors.begin(); it != connectors.end(); ++it)
	{	
		Content * source = (*it)->getSourceContent(container);
		Content * target = (*it)->getTargetContent(container);
	
		if (this->contentToGraphVizNode.find(source) == this->contentToGraphVizNode.end()) continue;
		if (this->contentToGraphVizNode.find(target) == this->contentToGraphVizNode.end()) continue;

		if ( (!inverseList.empty()) && (std::find(inverseList.begin(), inverseList.end(), *it) != inverseList.end() ) )
		{
			agedge(this->graph, this->contentToGraphVizNode[target], this->contentToGraphVizNode[source]);		
		}
		else
		{
			agedge(this->graph, this->contentToGraphVizNode[source], this->contentToGraphVizNode[target]);
		}
	}


	// [!] hack! For triangle containers
	this->initTriangleContainerEdges(tList, container);
	// [!] hack!

	////////////////////////////////////////////////////////////////////////////////////////////////

	// layout						
	gvLayout(this->graphContext, this->graph, (char*)this->method.c_str());
	
	////////////////////////////////////////////////////////////////////////////////////////////////
	
	// setting the local zoom
	float zoom = (this->method == "twopi")? 1.25: 0.75;

	// getting the bounding box of the graph
	box bb = GD_bb(this->graph); // boxf in latest version
	float right = bb.UR.x;	float top    = bb.UR.y;
	float left  = bb.LL.x;	float bottom = bb.LL.y;

	////////////////////////////////////////////////////////////////////////////////////////////////

	// old and new coordinates of the core of the main container in our coordinates system
	float x0, y0, X0, Y0;
	
	Content * core = container->getCore();

	// if there is a core, we get the old and new coordinates in our coordinates system
	if (core)
	{
		// we get it's current coordinates in our layout
		x0 = core->x(); y0 = core->y();
		// we get the new coordinates in graphviz layout
		Agnode_t * coreNode = this->contentToGraphVizNode[core];
		point p0 = ND_coord_i(coreNode); // ND_coord and pointf in latest version
		// we translate the graphviz coordinates in our coordinates system
		X0 = p0.x*zoom; Y0 = (top - p0.y)*zoom;
	}
	else // if not, we consider the center of the container as the core coordinates
	{
		x0 = container->x(); y0 = container->y();
		X0 = zoom*(right+left)/2; Y0 = zoom*(top-bottom)/2;
	}

	// all the children nodes of the main container are placed according to the graphviz coords
	// then they are translated so that the core does not appear to have moved
	for (std::list<Content*>::iterator it = cList.begin(); it != cList.end(); ++it)
	{
		Content * c = (*it);
		// graphviz coordinates
		Agnode_t * gvNode = this->contentToGraphVizNode[c];
		point p = ND_coord_i(gvNode); // ND_coord and pointf in latest version
	
		// translated coordinates so that the core of the main container doesn't move
		float X = p.x*zoom - X0;
		float Y = (top - p.y)*zoom - Y0;

		// clone translation (because of the reaction's in an out ports)
		float dx = 0; // for clones
		float dy = 0; // for clones

		// the node gets aligned to the in and out ports instead of the centre of the core
		if (c != core)
		{
			Connector * e = NULL; bool isSource = true;
			std::list<Connector*> eList = c->getOutterConnectors();
			for (std::list<Connector*>::iterator et = eList.begin(); et != eList.end(); ++et)
			{
				e = (*et);
				if (e->getTarget() == c) { if (e->getSource() == core) break; }
				else if (e->getTarget() == core) { isSource = false; break; }
				e = NULL;
			}
			if (e) 
			{		
				dx = e->getPoint(isSource).first - core->x();
				dy = e->getPoint(isSource).second - core->y();
			}
		}

		// broken attempt to make the graph more compact
		float xCompactFactor = 1;
		float yCompactFactor = 1;
//		float yCompactFactor = 0.75;

		// Translation + rotation if need be, and scaling too...
		if (this->rotation)	c->setPosition((x0 + Y + dx) * xCompactFactor, (y0 + X + dy) * yCompactFactor);
		else				c->setPosition((x0 + X + dx) * xCompactFactor, (y0 + Y + dy) * yCompactFactor);		
	}

	// [!] fixing the position of compartments
	for (std::list<Content*>::iterator it = cList.begin(); it != cList.end(); ++it)
	{
		Content * c = (*it);
		if ( c->getId() == "" ) continue;
		if ( ( (ContainerContent *) (c) )->getTypeLabel() != "CompContainer" ) continue;
		ContainerContent * comp = (ContainerContent*)c;
		
		std::list<Connector*> eList = comp->getOutterConnectors();
		if (eList.size() != 1) continue; // can't deal with compartements with more than one core...
		Connector * e = eList.front();
		Content * reaction = e->getSourceContent(container);
		if (reaction == c) reaction = e->getTargetContent(container);
		float xr = reaction->x();
		float yr = reaction->y();
		float xc = comp->x();
		float yc = comp->y();
		float x = comp->getCore()->x();
		float y = comp->getCore()->y();

		// aligns the core to where the centre currently is, relatively to the reaction
		if ((xr == xc) && (yr != yc))
		{
			comp->setPosition(2 * xc - x, yc);
		}
		else if ((yr == yc) && (xr != xc))
		{
			comp->setPosition(xc, 2 * yc - y);		
		}
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////

	this->layoutTriangleContainers(tList);

}

// in container and connectors, out inverselist and crossroad
std::list<Connector *> GraphvizContentLayoutManager::dealWithBranches(ContainerContent * container, std::list<Connector*> &connectors)
{
	std::list<Connector *> inverseList;

	// [!] temporary hack
	if (this->strategy == Triangle)
	{
		for (std::list<Connector *>::iterator it = connectors.begin(); it != connectors.end(); ++it)
		{
			inverseList.push_back(*it);
		}
	}
	
	if (this->strategy != Branch) return inverseList; // empty list

	Content * root = container->getCore();
	int in = 0;	int out = 0;	Connector * connect = NULL;
	for (std::list<Connector *>::iterator it = connectors.begin(); it != connectors.end(); ++it)
	{
		if ( (*it)->getSourceContent(container) == root ) { out++; connect = *it; }
		if ( (*it)->getTargetContent(container) == root ) { in++; connect = *it; }
	}

	if ( (in + out != 2) || (in * out != 0) ) return inverseList; // empty list
	
	std::stack<Connector *> moreEdges;
	moreEdges.push(connect);	
	while (!moreEdges.empty())
	{
		Connector * current = moreEdges.top();
		moreEdges.pop();
		inverseList.push_back(current);

		if (in)
		{
			Content * content = current->getSourceContent(container);
			for (std::list<Connector *>::iterator it = connectors.begin(); it != connectors.end(); ++it)
			{
				if (content == (*it)->getTargetContent(container))	moreEdges.push(*it);
			}
		}
		else
		{
			Content * content = current->getTargetContent(container);
			for (std::list<Connector *>::iterator it = connectors.begin(); it != connectors.end(); ++it)
			{
				if (content == (*it)->getSourceContent(container))	moreEdges.push(*it);
			}
		}
	}
	
	return inverseList;
}

// modifies cList (reference)
std::list<ContainerContent*> GraphvizContentLayoutManager::initTriangleContainerNodes(std::list<Content*> & cList)
{
	// We get a list called tList, of all the triangle containers in the main container
	std::list<ContainerContent*> tList;	
	for (std::list<Content*>::iterator it= cList.begin(); it != cList.end(); ++it)
	{
		Content * c = (*it);
		if (c->getId() == "") continue;
		ContainerContent * cc = (ContainerContent *) c;
		if (cc->getContentLayoutStrategy() != Triangle) continue;
		tList.push_back(cc);
	}
	// Then we remove these containers from the local copy of the main container's children list
	// and instead we put in the children list, the non-core children of the triangle container
	for (std::list<ContainerContent*>::iterator it= tList.begin(); it != tList.end(); ++it)
	{
		cList.remove(*it);
		std::list<Content*> ccList = (*it)->getChildren();
		for (std::list<Content*>::iterator ct= ccList.begin(); ct != ccList.end(); ++ct)
		{
			if ((*it)->getCore() == (*ct)) continue;
			cList.push_back(*ct);
		}
	}
	return tList;
}

void GraphvizContentLayoutManager::initTriangleContainerEdges(std::list<ContainerContent*> tList, ContainerContent * container)
{
	// Finds all the outter connectors of all the Triangle containers
	std::list<Connector*> oCon;
	for (std::list<ContainerContent*>::iterator it= tList.begin(); it != tList.end(); ++it)
	{
		std::list<Connector*> eList = (*it)->getOutterConnectors();
		for (std::list<Connector*>::iterator et= eList.begin(); et != eList.end(); ++et)
		{
			oCon.push_back(*et);
		}
	}
	// Then we create graphviz edges for the connector which have graphviz sources and targets
	for (std::list<Connector*>::iterator it = oCon.begin(); it != oCon.end(); ++it)
	{
		Content * source = (*it)->getSourceContent(container);
		if (this->contentToGraphVizNode.find(source) == this->contentToGraphVizNode.end())
		{
			source = (*it)->getSource();			
			if (this->contentToGraphVizNode.find(source) == this->contentToGraphVizNode.end()) continue;
		}

		Content * target = (*it)->getTargetContent(container);
		if (this->contentToGraphVizNode.find(target) == this->contentToGraphVizNode.end())
		{
			target = (*it)->getTarget();
			if (this->contentToGraphVizNode.find(target) == this->contentToGraphVizNode.end()) continue;
		}

		agedge(this->graph, this->contentToGraphVizNode[source], this->contentToGraphVizNode[target]);
	}
	// this means graphviz will lay out a graph that only has the non-core nodes of the triangle
	// and the container triangle itself is pretty much ignored (and so is its core)
}

void GraphvizContentLayoutManager::layoutTriangleContainers(std::list<ContainerContent*> tList)
{
	// [!] new hack!	
	// new condition on triangles is that they have either a common child or a common ancestor
	// We want to align the non core nodes to their respective non common ancestors or children
	// (and then place the core node in the middle in between)
	for (std::list<ContainerContent*>::iterator it= tList.begin(); it != tList.end(); ++it)
	{
		// first, we find r1 and r2
		CloneContent * r1 = NULL;
		CloneContent * r2 = NULL;		
		bool OK = false; // not enough
		std::list<Content*> ccList = (*it)->getChildren();
		for (std::list<Content*>::iterator ct= ccList.begin(); ct != ccList.end(); ++ct)
		{
			if ((*it)->getCore() == (*ct)) continue;
			if (!r1) r1 = (CloneContent*)(*ct);
			else if (!r2) { r2 = (CloneContent*)(*ct); OK = true; } // just right
			else OK = false; // too many
		}
		if (!OK) { std::cout << "Not OK" << std::endl; continue; } // something wrong...
		
		// finding the common ancestor or child
		int direction = 0; // 1 = child, -1 = ancestor;		
		std::list<Connector*> edges1 = r1->getOutterConnectors();
		std::list<Connector*> edges2 = r2->getOutterConnectors();
		for (std::list<Connector*>::iterator e1 = edges1.begin(); e1 != edges1.end(); ++e1)
		{
			for (std::list<Connector*>::iterator e2 = edges2.begin(); e2 != edges2.end(); ++e2)
			{
				if ((*e1)->getTarget() == (*e2)->getTarget()) { direction = 1; break; }
				if ((*e1)->getSource() == (*e2)->getSource()) { direction = -1; break; }
			}
		}
		if (!direction) { std::cout << "No direction" << std::endl; continue; } // couldn't find...
			
		// we then look at all the neighbours in the opposite direction of the common node
		float x1 = 0;
		int n1 = 0;
		float x2 = 0;
		int n2 = 0;
		if (direction == 1)
		{
			for (std::list<Connector*>::iterator e1 = edges1.begin(); e1 != edges1.end(); ++e1)
			{
				if ((*e1)->getSource() != r1) continue;
				CloneContent *cc = (*e1)->getTarget();
				if (cc == (*it)->getCore()) continue;
				n1++;
				x1 += cc->x();
			}
			for (std::list<Connector*>::iterator e2 = edges2.begin(); e2 != edges2.end(); ++e2)
			{
				if ((*e2)->getSource() != r2) continue;
				CloneContent *cc = (*e2)->getTarget();
				if (cc == (*it)->getCore()) continue;
				n2++;
				x2 += cc->x();		
			}
		}
		else
		{
			for (std::list<Connector*>::iterator e1 = edges1.begin(); e1 != edges1.end(); ++e1)
			{
				if ((*e1)->getSource() != r1) continue;
				CloneContent *cc = (*e1)->getTarget();
				if (cc == (*it)->getCore()) continue;
				n1++;
				x1 += cc->x();
			}
			for (std::list<Connector*>::iterator e2 = edges2.begin(); e2 != edges2.end(); ++e2)
			{
				if ((*e2)->getSource() != r2) continue;
				CloneContent *cc = (*e2)->getTarget();
				if (cc == (*it)->getCore()) continue;
				n2++;
				x2 += cc->x();		
			}		
		}
		if (n1) r1->setPosition(x1/n1, r1->y());
		if (n2) r2->setPosition(x2/n2, r2->y());
		// and we position both core's neighbours at the average x coordinate for these nodes
	}

	// [!] new hack!	
	
	// [!] hack!	
	// We still need to place the cores of the triangle containers
	// => at the barycentre of all the other nodes in the container
	///*
	for (std::list<ContainerContent*>::iterator it= tList.begin(); it != tList.end(); ++it)
	{
		std::list<Content*> ccList = (*it)->getChildren();
		float x = 0;
		float y = 0;
		int n = 0;
		for (std::list<Content*>::iterator ct= ccList.begin(); ct != ccList.end(); ++ct)
		{
			if ((*it)->getCore() == (*ct)) continue;
			++n;
			x += (*ct)->x();
			y += (*ct)->y();
		}
		if (n) (*it)->getCore()->setPosition(x/n, y/n);
	}
	//*/

	// Smarter alternative: roughly equal distance to the border of each other node?
	// => Barycentre of the nearest corner to G of the bounding box of each node
	for (std::list<ContainerContent*>::iterator it= tList.begin(); it != tList.end(); ++it)
	{
		float x0 = (*it)->getCore()->x();
		float y0 = (*it)->getCore()->y();

		std::list<Content*> ccList = (*it)->getChildren();
		float x = 0;
		float y = 0;
		int n = 0;
		for (std::list<Content*>::iterator ct= ccList.begin(); ct != ccList.end(); ++ct)
		{
			if ((*it)->getCore() == (*ct)) continue;
			++n;
			
			float xi = (*ct)->x();		
			if  (xi > x0) xi = (*ct)->left(false);
			else if (xi < x0) xi = (*ct)->right(false);
			
			float yi = (*ct)->y();
			if (yi > y0) yi = (*ct)->top(false);
			else if (yi < y0) yi = (*ct)->bottom(false);
			
			x += xi;
			y += yi;
		}
		if (n) (*it)->getCore()->setPosition(x/n, y/n);
	}
	// [!] hack!	
}
