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
 *  GraphLayout.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 27/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#include "graphlayout.h"

#include <iostream>

// local
#include "clonecontent.h"
#include "connector.h"
#include "connectorlayoutmanager.h"
#include "vertexproperty.h"

#include "edgeproperty.h"

/*************************************************************************
* Constructor: Creates a new root Container with a default LayoutManager *
* [!] can't chose the layout type!                                       *
*************************************************************************/
GraphLayout::GraphLayout(GraphModel * gm, std::string n) : graphModel(gm), visible(false),
	avoiding(false), name(n)
{
	this->root = new ContainerContent(this);
	this->connectorLayoutManager = new ConnectorLayoutManager(gm);
	this->layoutStyleSheet = this->graphModel->getStyleSheet();
}

/*****************************************************************************************************************************************
* Destructor: Deletes the root, and along with it the containers and clones it contains, and along with them the connectors linking them *
* [!] pbe if the clones are NOT part of the root tree, for some odd reason                                                               *
*****************************************************************************************************************************************/
GraphLayout::~GraphLayout() { delete this->root; delete this->connectorLayoutManager; }

/***********************************************************************************
* CloneContent management                                                       *
***********************************************************************************/
		
/*****************************************************************************
* map: adds the clone to the clone list linked to the vertex in the cloneMap *
* [!] assumes the clone belongs to the root tree                             *
*****************************************************************************/
void GraphLayout::map(BGL_Vertex v, CloneContent * cd) {	this->cloneMap[v].push_back(cd); }

/*****************************************************************
* getClone: Returns the 1st clone in the list for a given vertex *
*****************************************************************/
CloneContent * GraphLayout::getClone(BGL_Vertex v)
{
	if (this->cloneMap[v].empty()) return NULL;
	else return this->cloneMap[v].front();
}

/******************************************************
* getClones: Returns the clone list of a given vertex *
******************************************************/
std::list<CloneContent *> GraphLayout::getClones(BGL_Vertex v) { return this->cloneMap[v]; }

/***********************************************************************************
* Root Container management                                                        *
***********************************************************************************/

/**************************************
* getRoot: Returns the Root Container *
**************************************/
ContainerContent * GraphLayout::getRoot() { return this->root; }

/****************************************************
* update: Layouts the content of the Root Container *
****************************************************/
void GraphLayout::update(bool edgesOnly, bool fast)
{
	if (!edgesOnly && !fast)
	{
		this->getRoot()->layoutContent();
	}

	if (!fast)
	{
		// [!] First I need to update the orientation of reactions (pathway only!!)
		BGL_Vertex r;
		////////////////////////////////////////
		// we look up every node in the model //
		////////////////////////////////////////
		std::list<BGL_Vertex> vList = this->graphModel->getVertices();	
		for (std::list<BGL_Vertex>::iterator vit = vList.begin(); vit != vList.end(); ++vit)
		{
			// for every reaction...
			if (this->graphModel->getProperties(*vit)->getTypeLabel() != "Reaction")
				if (this->graphModel->getProperties(*vit)->getSuperTypeLabel() != "Reaction")
					continue;
			r = (*vit); 
			CloneContent * rClone = this->getClone(r); // has always only one clone
			// actually, sometimes, no clone at all! (partial view)
			if (!rClone) continue;
			
			//////////////////////////////////////////////////////////////////////////////////
			// we compute the global position of reactants and products around the reaction
			float x = 0;
			float y = 0;
			// by adding up the reactionToReactant and productToReaction vectors
			// we can see an overall emerging orientation
			// if x is bigger than y, the reaction is horizontal, if not, vertical
			// with this system, the orientation of the reaction gets updated automatically
			// as reactants and products move around it                                    
			//////////////////////////////////////////////////////////////////////////////////
	
			// first we have a look at all the edges around the reaction
			std::list<BGL_Edge> edges = this->graphModel->getEdges(r);
			for (std::list<BGL_Edge>::iterator eit = edges.begin(); eit != edges.end(); ++eit)
			{
				BGL_Edge e = (*eit);
				int factor = 0;

				// we only care about reactants and products
				std::string type = this->graphModel->getProperties(e)->getTypeLabel();
				if (type == "Reactant") factor = -1;
				else if (type == "Product") factor = 1;
				else // we ignore modifiers at the moment
				{
					continue;
				}

				BGL_Vertex s;
				if (factor < 0)	s = this->graphModel->getSource(e);
				else 			s = this->graphModel->getTarget(e);

				// now back at the layout level!
				CloneContent * sClone;
				if (factor < 0) sClone = this->getConnector(s, r)->getSource();
				else			sClone = this->getConnector(r, s)->getTarget();
				
				x += factor*(sClone->x() - rClone->x());
				y += factor*(sClone->y() - rClone->y());
			} // for every reactant and product

			bool rotated = (x*x > y*y)? true: false;
			bool inverted = rotated? (x < 0) : (y < 0) ;

			rClone->rotated = rotated;
			rClone->inverted = inverted;

			// now to deal with modifiers => they must point at the right side of the reaction
			for (std::list<BGL_Edge>::iterator eit = edges.begin(); eit != edges.end(); ++eit)
			{
				BGL_Edge e = (*eit);
				std::string type = this->graphModel->getProperties(e)->getTypeLabel();
				if (type != "Modifier") continue;

				BGL_Vertex s = this->graphModel->getSource(e);
				CloneContent * sClone;
				Connector * c = this->getConnector(s, r);
				sClone = c->getSource();
				
				if (rClone->rotated)
				{
					if (rClone->inverted)
					{
						if (sClone->y() >= rClone->y()) c->setTargetConnection(side1);
						else c->setTargetConnection(side2);
					}
					else
					{
						if (sClone->y() <= rClone->y()) c->setTargetConnection(side1);
						else c->setTargetConnection(side2);
					}
				}
				else
				{
					if (rClone->inverted)
					{
						if (sClone->x() >= rClone->x()) c->setTargetConnection(side1);
						else c->setTargetConnection(side2);
					}
					else
					{
						if (sClone->x() <= rClone->x()) c->setTargetConnection(side1);
						else c->setTargetConnection(side2);
					}				
				}
			} // for every modifier

		} // for every reaction	

		std::list<CloneContent*> cloneList;
		if (this->avoiding) { cloneList = this->getCloneContents(); }
		this->connectorLayoutManager->layout(this->getConnectors(), cloneList);
	}

	if (fast)
	{
// do something with this->inList and this->outList, similar to the quicktranslate and quickupdate of edgegraphics	
		for (std::list<Connector *>::iterator it = this->inList.begin(); it != this->inList.end(); ++it)
		{
			(*it)->quickTranslate();
		}
		for (std::list<Connector *>::iterator it = this->outList.begin(); it != this->outList.end(); ++it)
		{
			(*it)->quickUpdate();
		}
// actually, it may be useless to distinguish in and out connectors at that level (it's only important for the updateshape part of edgegraphics)
	}
}

/* // [!] from the old ModelGraphView... we also must layout the edges and resize the scene!
	this->resizeScene();
*/		

/***********************************************************************************
* Connector management                                                             *
***********************************************************************************/

/**********
* connect *
***********
* Given a pair of BGL_Vertex,
* finds the corresponding clones
* creates a connector between these two clones
* and adds the connector to the list
* [!] is that list useful?
**********************************************/
Connector * GraphLayout::connect(BGL_Vertex u, BGL_Vertex v)
{
	CloneContent * uc = this->findClone(u, v);
	CloneContent * vc = this->findClone(v, u);
	
	if (!uc) return NULL;
	if (!vc) return NULL;
		
	return this->connectClones(uc, vc);
}

Connector * GraphLayout::connectClones(CloneContent * uc, CloneContent * vc)
{	
	if (!this->graphModel->isEdge(uc->getVertex(), vc->getVertex())) return NULL;
	Connector * c =  new Connector (uc, vc);
	this->connectorList.push_back(c);

	// [!] to solve the problem of displaying adequate SBGN reactions in neighbourhood views
	BGL_Edge e = this->graphModel->getEdge(uc->getVertex(), vc->getVertex());
	if (this->graphModel->getProperties(e)->getTypeLabel() == "Reactant") c->setTargetConnection(in);
	if (this->graphModel->getProperties(e)->getTypeLabel() == "Product")  c->setSourceConnection(out);		
	if (this->graphModel->getProperties(e)->getTypeLabel() == "Modifier") c->setTargetConnection(side1);

	return c;
}

/***************
* getConnector *
****************
* Given a pair of BGL_Vertex,
* browses the list of connectors to find the one connecting them
* [!] that looks terribly inefficient. Better indexing?
****************************************************************/
Connector * GraphLayout::getConnector(BGL_Vertex u, BGL_Vertex v)
{
	Connector * c = NULL;
	for (std::list<Connector*>::iterator it = this->connectorList.begin(); it != this->connectorList.end(); ++it)
	{
		c = (*it);
		
		c->getSource()->getVertex();
		c->getTarget()->getVertex();
		
		if (c->getSource()->getVertex() == u) if (c->getTarget()->getVertex() == v) break;

		c = NULL;
	}
	return c;
}

/**********************
* getCloneContents *
***********************
* From the map, returns a list of every Clones
* [!] I should just call it getClones...
**********************************************/
std::list< CloneContent * > GraphLayout::getCloneContents()
{
	std::list< CloneContent *> cList;
	for (std::map< BGL_Vertex, std::list<CloneContent*> >::iterator it = cloneMap.begin(); it != cloneMap.end(); ++it)
	{
		std::list<CloneContent*> v = (*it).second;
		for (std::list<CloneContent *>::iterator ic = v.begin(); ic != v.end(); ++ic)
		{
			cList.push_back(*ic);
		}
	}
	return cList;
}

/****************
* getConnectors *
*****************
* Returns the list of Connectors
* [!] who uses that?
********************************/
std::list< Connector * > GraphLayout::getConnectors() { return this->connectorList; }

/*************************************************************
* unmap: removes the clone from the map                      *
* [!] doesn't check whateve happens to the clone             *
* (we assume it's the clone itself calling, upon destruction *
*************************************************************/
void GraphLayout::unmap(CloneContent * cd) {	this->cloneMap[cd->getVertex()].remove(cd); }

/****************
* toggleCloning *
*****************
* Whether the vertex already has several clones, or not
* we unclone or clone it
*******************************************************/
void GraphLayout::toggleCloning(BGL_Vertex v, CloneContent * c)
{
	if ( (this->getClones(v).size() == 1) && (!this->getClone(v)->getNeighbours().size()) ) this->clone(v);
	else this->unclone(v, c);
}

void GraphLayout::toggleBranching(BGL_Vertex v)
{	
	CloneContent* c = this->getClone(v);
	
	ContainerContent *cloneContainer = NULL;
	ContainerContent *parent = c->getContainer();

	if (parent->getContentLayoutStrategy() == Clone)
	{
		cloneContainer = parent;
		parent = parent->getContainer();
	}

	if ( (parent->getContentLayoutStrategy() == Branch) || (parent->getContentLayoutStrategy() == Triangle) )
	{
		this->unbranch(c, parent, cloneContainer);
	}
	else
	{
		this->branch(c, parent, cloneContainer);
	}
	
	this->update(true);
}

void GraphLayout::branch(CloneContent * c, ContainerContent * parent, ContainerContent * cloneContainer)
{
	CloneContent* r1 = NULL;
	CloneContent* r2 = NULL;
	std::list<Content*> leaves;

	// We look at all the edges connected to the outside of the branching candidate
	std::list<Connector*> connectors = c->getOutterConnectors();	
	for (std::list<Connector*>::iterator it = connectors.begin(); it != connectors.end(); ++it)
	{
		Connector * connect = *it;
		// we look at the neighbour of the branching candidate along that edge
		CloneContent* n = connect->getNeighbour(c);
		// if that neighbour belongs to a cloneContainer, it gets ignored
		if (n->getContainer() == cloneContainer) continue;

		// we count the number of nodes the neighbour is connected to
		int nn = n->getOutterConnectors().size();
		if (nn > 1) // if the neighbour is not a leaf...
		{
			if (r1) // if this is not the first non leafy neighbour
			{
				// if there are already two non leafy neighbours, this is not a triangle
				if (r2) { std::cout << "too many neighbours, can't triangle" << std::endl; return; }
				else { r2 = n; } // else we have a second non leafy neighbour
			}
			else { r1 = n; } // else we have a first non leafy neighbour
		}
		else { leaves.push_back(n); } // else, the neighbour is added to the leaves list
	}
	// if all the neighbours are leaves, there is no main trunk to branch from!
	if (!r1) { std::cout << "no main trunk, can't branch this" << std::endl; return; }

	ContainerContent * branchContainer = NULL;

	// if we have a potential triangle configuration
	if (r2)
	{
		// if there are non-clone leaves, this is not a triangle...
		if (!leaves.empty()) { std::cout << "too many leaves for a triangle layout" << std::endl; return; }

		// Additional test to check if we have a triangle... the two neighbours must have
		// a common ancestor (triangle up) or a common child (triangle down)
		bool found = false;
		std::list<Connector*> edges1 = r1->getOutterConnectors();
		std::list<Connector*> edges2 = r2->getOutterConnectors();
		for (std::list<Connector*>::iterator e1 = edges1.begin(); e1 != edges1.end(); ++e1)
		{
			bool isSource = ((*e1)->getSource() == r1);
			bool isTarget = ((*e1)->getTarget() == r1);
			for (std::list<Connector*>::iterator e2 = edges2.begin(); e2 != edges2.end(); ++e2)
			{
				if (isSource && ((*e2)->getSource() == r2))
					if ((*e1)->getTarget() == (*e2)->getTarget()) { found = true; break; }
				if (isTarget && ((*e2)->getTarget() == r2))
					if ((*e1)->getSource() == (*e2)->getSource()) { found = true; break; }
			}
		}
		if (!found) { std::cout << "no common child or ancestor for triangle" << std::endl; return; }
		
		// Then we can create the new container
		branchContainer = new ContainerContent(this, parent);
		branchContainer->setContentLayoutStrategy(Triangle);
		
		// the middle node of the triangle is the core!
		if (cloneContainer)	branchContainer->add(cloneContainer, true);
		else				branchContainer->add(c, true);

		// the core's neighbours...
		branchContainer->add(r1);
		branchContainer->add(r2);
	}
	else // else we have a branch
	{
		ContainerContent * rParent = r1->getContainer();
		if (rParent->getContentLayoutStrategy() == Branch) branchContainer = rParent;
		else
		{
			branchContainer = new ContainerContent(this, parent);
			branchContainer->setContentLayoutStrategy(Branch);
			branchContainer->add(r1, true);
		}		

		branchContainer->add(leaves);

		if (cloneContainer)	branchContainer->add(cloneContainer);
		else				branchContainer->add(c);
	}

	// the branch gets laid out
	branchContainer->layoutContent();
}

// [!] should remove only one branch...
void GraphLayout::unbranch(CloneContent * c, ContainerContent * parent, ContainerContent * cloneContainer)
{
	ContainerContent * grandad = parent->getContainer();
	grandad->add(parent->getChildren());
	delete parent;
}

/***********************************************************************************
* Private methods                                                                  *
***********************************************************************************/

/************
* findClone *
*************
* Given a pair of BGL_Vertex,
* browses the list of clones of the first vertex
* to find which one as the 2nd vertex as one of its neighbours
* Returns the matching clone, or NULL
* [!] Should I actually keep clones as a map within a map?
* eg mapping a vertex to a map mapping a neighbour to a clone?
* instead of mapping the vertex to a list of clones
* Pbe: quite verbose...
****************************************************************/
CloneContent * GraphLayout::findClone (BGL_Vertex vertex, BGL_Vertex neighbour)
{
	CloneContent * cd = NULL;

	std::list<CloneContent *> clones = this->getClones(vertex);
	for (std::list<CloneContent *>::iterator it = clones.begin(); it != clones.end(); ++it)
	{
		cd = *it;
		
		std::list<BGL_Vertex> nList = cd->getNeighbours();
		if ( nList.empty() ) break; // contains all the neighbours
		else if ( std::find(nList.begin(), nList.end(), neighbour) != nList.end() ) break; // neighbour is in the list
		
		cd = NULL;	
	}

	return cd;
}

/********
* clone *
*********
* Removes the current only clone of the vertex
* [!] assumes the clone is unique
* and creates one clone per neighbour instead
* Makes sure the connectors are properly set up
* and updates the overall layout
* [!] nothing is done placement container wise
***********************************************/
void GraphLayout::clone(BGL_Vertex v)
{	
	CloneContent * cd = this->getClone(v);
	
	std::list<Connector*> connectors = cd->getOutterConnectors();
	for (std::list<Connector*>::iterator it = connectors.begin(); it != connectors.end(); ++it)
	{
		CloneContent * newClone = new CloneContent(v, this);
		newClone->setPosition(cd->x(), cd->y());

		CloneContent * source = (*it)->getSource();
		CloneContent * target = (*it)->getTarget();
		CloneContent * root = NULL;

		if (cd == source) { (*it)->setSource(newClone); root = target; }
		if (cd == target) {	(*it)->setTarget(newClone); root = source; }

		newClone->addNeighbour(root->getVertex());

		// the container bit...
		ContainerContent * parent = root->getContainer();
		// ... should not be done when the overall layout is of neighbourhood type
		if (parent->getContentLayoutStrategy() == Neighbourhood) { continue; }
		
		ContainerContent * cloneContainer = NULL;

		if (parent->getContentLayoutStrategy() == Clone)
		{
			cloneContainer = parent;
						
			// has(cd))  // [!] alternative test?
			if (parent->getCore() == cd) // slightly convoluted case where the cloned vertex is ALREADY in a clone container (probably as a root)
			{
				ContainerContent * grandPa = parent->getContainer();
				if (grandPa)
				{
					grandPa->add(parent->getChildren());
					delete parent;
					cloneContainer = NULL;
					parent = grandPa;
				}
			}
		}
		
		if (cloneContainer == NULL)
		{ 
			cloneContainer = new ContainerContent(this, parent, "CloneContainer");
			cloneContainer->setContentLayoutStrategy(Clone); // [!] what about edges? avoiding?
			cloneContainer->add(root, true);
		}
		cloneContainer->add(newClone);
		cloneContainer->layoutContent();		
	}
	
	delete cd;

	this->update(true);
}

/**********
* unclone *
***********
* Removes the current clones of the vertex
* and creates one clone for all the neighbours instead
* Makes sure the connectors are properly set up
* and updates the overall layout
* [!] nothing is done placement container wise
******************************************************/
void GraphLayout::unclone(BGL_Vertex v, CloneContent * c)
{
	std::list<CloneContent *> cList = this->getClones(v);

	CloneContent * newClone = new CloneContent(v, this);
	if (c) newClone->setPosition(c->x(), c->y());

	ContainerContent * newParent = NULL;

	for (std::list<CloneContent *>::iterator ic = cList.begin(); ic != cList.end(); ++ic)
	{
		CloneContent * cd = *ic;
		if (!c) newClone->setPosition(cd->x(), cd->y());

		std::list<Connector*> connectors = cd->getOutterConnectors();
		for (std::list<Connector*>::iterator it = connectors.begin(); it != connectors.end(); ++it)
		{
			CloneContent * source = (*it)->getSource();
			CloneContent * target = (*it)->getTarget();

			if (cd == source) (*it)->setSource(newClone);
			if (cd == target) (*it)->setTarget(newClone);
		}

		// [!] implement automatic unnesting instead of manually deleting the useless container?
		ContainerContent * cloneContainer = cd->getContainer();	
		delete cd;
		
		ContainerContent * parent = cloneContainer->getContainer();
		if (!parent) { std::cout << "bug" << std::endl; continue; } // [!] very strange case where the root of the layout is a clone container...
		// (actually happens in the local view has the strategy is the same as for a clone)

		if (!newParent)	newParent = parent;
		else newParent = Content::CommonAncestor(newClone, cloneContainer);
		newParent->add(newClone); // places the new clone in the common ancestor of all its previous clones?

		if (cloneContainer->getChildren().size() < 2)
		{
			parent->add(cloneContainer->getChildren().front());
			delete cloneContainer;
		}
	}
	
	this->update(true);
}

std::list<CloneContent*> GraphLayout::recursiveNeighbourhoodBuilding(std::list<CloneContent*> roots, std::list<CloneContent*> neighbourhood, int depth) {
	if (depth >= 0)	
	for (std::list<CloneContent*>::iterator it = roots.begin(); it != roots.end(); ++it)
	{
		CloneContent* clone = *it;
		neighbourhood = this->buildNeighbours(clone, neighbourhood, depth);	
		neighbourhood = this->recursiveNeighbourhoodBuilding(clone->getNeighbourClones(), neighbourhood, depth-1);
	}

	return neighbourhood;
}

std::list<CloneContent*> GraphLayout::buildNeighbours(CloneContent * clone, std::list<CloneContent *> neighbourhood, bool isVisible)
{
	std::list<BGL_Vertex> vertexNeighbours = this->graphModel->getNeighbours(clone->getVertex());
	
	if (isVisible) for (std::list<BGL_Vertex>::iterator vi = vertexNeighbours.begin(); vi != vertexNeighbours.end(); ++vi)
	{
		bool newEdge = true;
		CloneContent * neighbour = NULL;
		BGL_Vertex v = *vi;

		std::list<CloneContent *> existingNeighbours = clone->getNeighbourClones();
		for (std::list<CloneContent *>::iterator ni = existingNeighbours.begin(); ni != existingNeighbours.end(); ++ni)
		{
			if ((*ni)->getVertex() == v) { neighbour = (*ni); newEdge = false; break; }
		}
		
		if (!neighbour) for (std::list<CloneContent*>::iterator li = neighbourhood.begin(); li != neighbourhood.end(); ++li)
		{
			if ((*li)->getVertex() == v) { neighbour = (*li); break; }
		}

		if (!neighbour)
		{
			neighbour = new CloneContent(v, this);
			neighbourhood.push_back(neighbour);
		}

		if (newEdge)
		{
			this->connectClones(clone, neighbour);
			this->connectClones(neighbour, clone);
		}
	}
	else for (std::list<BGL_Vertex>::iterator vi = vertexNeighbours.begin(); vi != vertexNeighbours.end(); ++vi)
	{
		bool newEdge = true;
		CloneContent * neighbour = NULL;			
		BGL_Vertex v = *vi;

		std::list<CloneContent *> existingNeighbours = clone->getNeighbourClones();
		for (std::list<CloneContent *>::iterator ni = existingNeighbours.begin(); ni != existingNeighbours.end(); ++ni)
		{
			if ((*ni)->getVertex() == v) { neighbour = (*ni); newEdge = false; break; }
		}

		if (!neighbour) for (std::list<CloneContent*>::iterator li = neighbourhood.begin(); li != neighbourhood.end(); ++li)
		{
			if ((*li)->getVertex() == v) { neighbour = (*li); break; }
		}

		if (!neighbour)
		{
			//clone->hasMoreEdges = true;
			newEdge = false;
		}

		if (newEdge)
		{
			this->connectClones(clone, neighbour);
			this->connectClones(neighbour, clone);		
		}
	}
	
	return neighbourhood;
}

void GraphLayout::expand(std::list<BGL_Vertex> vList)
{
	std::list<CloneContent*> neighbourhood = this->getCloneContents();
		
	std::list<CloneContent*> roots; // first we add the new clones
	for (std::list<BGL_Vertex>::iterator it = vList.begin(); it != vList.end(); ++it)
	{
		std::list<CloneContent*> clones = this->cloneMap[*it];
		if (clones.empty()) clones.push_back(new CloneContent(*it, this));
		for (std::list<CloneContent*>::iterator ic = clones.begin(); ic != clones.end(); ++ic)
		{
			roots.push_back(*ic);
			neighbourhood.push_back(*ic);
		}
	}
		
	// just like fill gaps now...? except set of existing nodes != set of new ones
	
	std::list<CloneContent*> gaps;
	
	// first joining the species with their reactions
	for (std::list<CloneContent*>::iterator it = roots.begin(); it != roots.end(); ++it)
	{
		CloneContent* clone = *it;

		std::list<BGL_Vertex> vertexNeighbours = this->graphModel->getNeighbours(clone->getVertex());
		for (std::list<BGL_Vertex>::iterator vi = vertexNeighbours.begin(); vi != vertexNeighbours.end(); ++vi)
		{
			bool newEdge = true;
			CloneContent * neighbour = NULL;
			BGL_Vertex v = *vi;

			std::list<CloneContent *> existingNeighbours = clone->getNeighbourClones();
			for (std::list<CloneContent *>::iterator ni = existingNeighbours.begin(); ni != existingNeighbours.end(); ++ni)
			{
				if ((*ni)->getVertex() == v) { neighbour = (*ni); newEdge = false; break; }
			}
		
			if (!neighbour) for (std::list<CloneContent*>::iterator li = neighbourhood.begin(); li != neighbourhood.end(); ++li)
			{
				if ((*li)->getVertex() == v) { neighbour = (*li); break; }
			}

			if (neighbour)
			{
				gaps.push_back(neighbour);
			}
			else
			{
				neighbour = new CloneContent(v, this);
				neighbourhood.push_back(neighbour);
			}

			if (newEdge)
			{
				this->connectClones(clone, neighbour);
				this->connectClones(neighbour, clone);
			}
		}
	}

	// now expanding the gap reactions (between two root species)
	for (std::list<CloneContent*>::iterator it = gaps.begin(); it != gaps.end(); ++it)
	{
		CloneContent* clone = *it;

		std::list<BGL_Vertex> vertexNeighbours = this->graphModel->getNeighbours(clone->getVertex());
		for (std::list<BGL_Vertex>::iterator vi = vertexNeighbours.begin(); vi != vertexNeighbours.end(); ++vi)
		{
			bool newEdge = true;
			CloneContent * neighbour = NULL;
			BGL_Vertex v = *vi;

			std::list<CloneContent *> existingNeighbours = clone->getNeighbourClones();
			for (std::list<CloneContent *>::iterator ni = existingNeighbours.begin(); ni != existingNeighbours.end(); ++ni)
			{
				if ((*ni)->getVertex() == v) { neighbour = (*ni); newEdge = false; break; }
			}
		
			if (!neighbour) for (std::list<CloneContent*>::iterator li = neighbourhood.begin(); li != neighbourhood.end(); ++li)
			{
				if ((*li)->getVertex() == v) { neighbour = (*li); break; }
			}

			if (!neighbour)
			{
				neighbour = new CloneContent(v, this);
				neighbourhood.push_back(neighbour);
			}

			if (newEdge)
			{
				this->connectClones(clone, neighbour);
				this->connectClones(neighbour, clone);
			}
		}		
	}	
	
	this->update();
}

void GraphLayout::fillGaps(std::list<CloneContent*> roots) // neighbourhood at distance one? then expand reactions when another species is present
{
	std::list<CloneContent*> neighbourhood = roots;
	std::list<CloneContent*> gaps;
	
	// first joining the species with their reactions
	for (std::list<CloneContent*>::iterator it = roots.begin(); it != roots.end(); ++it)
	{
		CloneContent* clone = *it;

		std::list<BGL_Vertex> vertexNeighbours = this->graphModel->getNeighbours(clone->getVertex());
		for (std::list<BGL_Vertex>::iterator vi = vertexNeighbours.begin(); vi != vertexNeighbours.end(); ++vi)
		{
			bool newEdge = true;
			CloneContent * neighbour = NULL;
			BGL_Vertex v = *vi;

			std::list<CloneContent *> existingNeighbours = clone->getNeighbourClones();
			for (std::list<CloneContent *>::iterator ni = existingNeighbours.begin(); ni != existingNeighbours.end(); ++ni)
			{
				if ((*ni)->getVertex() == v) { neighbour = (*ni); newEdge = false; break; }
			}
		
			if (!neighbour) for (std::list<CloneContent*>::iterator li = neighbourhood.begin(); li != neighbourhood.end(); ++li)
			{
				if ((*li)->getVertex() == v) { neighbour = (*li); break; }
			}

			if (neighbour)
			{
				gaps.push_back(neighbour);
			}
			else
			{
				neighbour = new CloneContent(v, this);
				neighbourhood.push_back(neighbour);
			}

			if (newEdge)
			{
				this->connectClones(clone, neighbour);
				this->connectClones(neighbour, clone);
			}
		}
	}

	// now expanding the gap reactions (between two root species)
	for (std::list<CloneContent*>::iterator it = gaps.begin(); it != gaps.end(); ++it)
	{
		CloneContent* clone = *it;

		std::list<BGL_Vertex> vertexNeighbours = this->graphModel->getNeighbours(clone->getVertex());
		for (std::list<BGL_Vertex>::iterator vi = vertexNeighbours.begin(); vi != vertexNeighbours.end(); ++vi)
		{
			bool newEdge = true;
			CloneContent * neighbour = NULL;
			BGL_Vertex v = *vi;

			std::list<CloneContent *> existingNeighbours = clone->getNeighbourClones();
			for (std::list<CloneContent *>::iterator ni = existingNeighbours.begin(); ni != existingNeighbours.end(); ++ni)
			{
				if ((*ni)->getVertex() == v) { neighbour = (*ni); newEdge = false; break; }
			}
		
			if (!neighbour) for (std::list<CloneContent*>::iterator li = neighbourhood.begin(); li != neighbourhood.end(); ++li)
			{
				if ((*li)->getVertex() == v) { neighbour = (*li); break; }
			}

			if (!neighbour)
			{
				neighbour = new CloneContent(v, this);
				neighbourhood.push_back(neighbour);
			}

			if (newEdge)
			{
				this->connectClones(clone, neighbour);
				this->connectClones(neighbour, clone);
			}
		}		
	}
	
	this->update();
}

/*
std::list<NodeView*> PathwayScene::buildNeighbours(NodeView *node, std::list<PathwayVertex> vertexNeighbours, bool isSpecies, bool source, std::list<NodeView*> neighbourhood, bool isVisible) {
	if (isVisible)
	// for each vertex in the neighbour list, we may have to create a new neighbour node, and a new edge connecting it to the main node
	for (std::list<PathwayVertex>::iterator vi = vertexNeighbours.begin(); vi != vertexNeighbours.end(); ++vi) {
		bool newEdge = true;
		NodeView* neighbour = NULL;			

		// first exception: there is already an edge connecting the main node to a node holding the vertex we want: no need either for a new node or a new edge
		std::list<NodeView*> existingNeighbours;
		if (source) existingNeighbours = node->getSourceNeighbours();
		else existingNeighbours = node->getTargetNeighbours();
		for (std::list<NodeView*>::iterator ni = existingNeighbours.begin(); ni != existingNeighbours.end(); ++ni) {
			if ((*ni)->getVertex().getLabel() == (*vi).getLabel()) { neighbour = (*ni);	newEdge = false; break; }
		}

		// second exception: among existing nodes in the neighbourhood, one hold the vertex we want. We just need a new edge
		if (!neighbour) for (std::list<NodeView*>::iterator li = neighbourhood.begin(); li != neighbourhood.end(); ++li) {
			if ((*vi).getLabel() == (*li)->getVertex().getLabel()) { neighbour = (*li); break; }
		}

		// in case no existing neighbour holding the vertex was found, we create one and add it to the list
		if (!neighbour) {
			if (isSpecies)	neighbour = this->addReactionView(*vi);
			else			neighbour = this->addSpeciesView(*vi);
			neighbourhood.push_back(neighbour);
		} // rem: maybe we should define a function such as addNodeView(vertex) that automatically check the vertex type and call the right function

		// in case we DO need a new edge between the main node and this neighbour, we create one
		if (newEdge) {
			if (isSpecies) {
				if (source) {
					if (neighbour->getVertex().isModifier(node->getVertex())) this->addModifierView(neighbour, node);
					else this->addProductView(neighbour, node);
				}
				else {
					if (neighbour->getVertex().isModifier(node->getVertex())) this->addModifierView(node, neighbour);
					else this->addReactantView(node, neighbour);
				}
			}
			else {
				if (source) {
					if (neighbour->getVertex().isModifier(node->getVertex())) this->addModifierView(neighbour, node);
					else this->addReactantView(neighbour, node);
				}
				else {
					if (neighbour->getVertex().isModifier(node->getVertex())) this->addModifierView(node, neighbour);
					else this->addProductView(node, neighbour);
				}
			}
		}  // rem: maybe we should define a function such as addEdgeView(node, node) that automatically check their vertex types and call the right function
	}	
	else for (std::list<PathwayVertex>::iterator vi = vertexNeighbours.begin(); vi != vertexNeighbours.end(); ++vi) {
		NodeView* neighbour = NULL;			

		std::list<NodeView*> existingNeighbours;
		if (source) existingNeighbours = node->getSourceNeighbours();
		else existingNeighbours = node->getTargetNeighbours();
		for (std::list<NodeView*>::iterator ni = existingNeighbours.begin(); ni != existingNeighbours.end(); ++ni) {
			if ((*ni)->getVertex().getLabel() == (*vi).getLabel()) { neighbour = (*ni); break; }
		}
		if (!neighbour) for (std::list<NodeView*>::iterator li = neighbourhood.begin(); li != neighbourhood.end(); ++li) {
			if ((*vi).getLabel() == (*li)->getVertex().getLabel()) { neighbour = (*li); break; }
		}

		if (!neighbour) { node->hasMoreEdges = true; }
	}
/*
	else for (std::list<PathwayVertex>::iterator vi = vertexNeighbours.begin(); vi != vertexNeighbours.end(); ++vi) {
		NodeView* neighbour = NULL;			

		std::list<NodeView*> existingNeighbours;
		if (source) existingNeighbours = node->getSourceNeighbours();
		else existingNeighbours = node->getTargetNeighbours();
		for (std::list<NodeView*>::iterator ni = existingNeighbours.begin(); ni != existingNeighbours.end(); ++ni) {
			if ((*ni)->getVertex().getLabel() == (*vi).getLabel()) if ((*ni)->isVisible()) { neighbour = (*ni); break; }
		}
		if (!neighbour) for (std::list<NodeView*>::iterator li = neighbourhood.begin(); li != neighbourhood.end(); ++li) {
			if ((*vi).getLabel() == (*li)->getVertex().getLabel()) if ((*li)->isVisible()) { neighbour = (*li); break; }
		}

		if (!neighbour) {
			if (isSpecies) {	
				neighbour = this->addReactionView(*vi);
				if (source) this->addProductView(neighbour, node);
				else this->addReactantView(node, neighbour);
			}
			else {
				neighbour = this->addSpeciesView(*vi);
				if (source)	this->addReactantView(neighbour, node);
				else		this->addProductView(node, neighbour);			
			}
			neighbourhood.push_back(neighbour);
			neighbour->setVisible(false);
		}
	}	
*/
/*	
	return neighbourhood;
}
*/
/*
std::list<NodeView*> PathwayScene::recursiveNeighbourhoodBuilding(std::list<NodeView*> roots, std::list<NodeView*> neighbourhood, int depth) {
	if (depth >= 0)	
	for (std::list<NodeView*>::iterator ri = roots.begin(); ri != roots.end(); ++ri) {
		NodeView* node = (*ri);

		PathwayVertex v = node->getVertex();

		std::list<PathwayVertex> sourceNeighbours = v.getSourceNeighbours();
		neighbourhood = this->buildNeighbours(node, sourceNeighbours, v.isSpecies(), true, neighbourhood, depth);
	
		std::list<PathwayVertex> targetNeighbours = v.getTargetNeighbours();		
		neighbourhood = this->buildNeighbours(node, targetNeighbours, v.isSpecies(), false, neighbourhood, depth);
	
		std::list<NodeView*> neighbourNodes = node->getNeighbours();
		neighbourhood = this->recursiveNeighbourhoodBuilding(neighbourNodes, neighbourhood, depth-1);
	}

	return neighbourhood;
}
*/

/* // [!] what about clone styles? and label? and if we are in neighbourhood mode? or not? (container)
	NodeStyle *s;
	if (total>1) s = &this->cofactorStyle;
	else s = &this->secondaryStyle;
	
	ostringstream o;
	o << number << "/" << total;
	node->optionalLabel = o.str();
	
	node->setStyle(&this->cofactorStyleRight);
	//or
	node->setStyle(&this->cofactorStyleLeft);
*/