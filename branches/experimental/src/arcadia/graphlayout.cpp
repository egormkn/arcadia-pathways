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
#include <pathways/pathwayvertexproperty.h> // [!]

#include "edgeproperty.h"

/*************************************************************************
* Constructor: Creates a new root Container with a default LayoutManager *
* [!] can't chose the layout type!                                       *
*************************************************************************/
GraphLayout::GraphLayout(GraphModel * gm, std::string n) : graphModel(gm), visible(false),
	avoiding(false), name(n)
{
	this->root = new ContainerContent(this);
	this->connectorLayoutManager = new ConnectorLayoutManager(this);
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
void GraphLayout::map(BGL_Vertex v, CloneContent * cd)
{
	this->cloneMap[v].push_back(cd);
}

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
		this->getRoot()->layoutContent(); // calls graphviz to compute the whole node layout
	}

	// fix the orientation of reactions then computes the path of edges to avoid nodes
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
			if (this->graphModel->getProperties(*vit)->getTypeLabel(true) != "Reaction")
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
				CloneContent * sClone = NULL;
				if (factor < 0) sClone = this->getConnector(s, r, e)->getSource();
				else			sClone = this->getConnector(r, s, e)->getTarget();
				
				x += factor*(sClone->x() - rClone->x());
				y += factor*(sClone->y() - rClone->y());
			} // for every reactant and product

			bool rotated = (x*x > y*y)? true: false;
			bool inverted = rotated? (x < 0) : (y < 0) ;

			rClone->rotated = rotated;
			rClone->inverted = inverted;

			// now to deal with modifiers => they must point at the correct side of the reaction
			for (std::list<BGL_Edge>::iterator eit = edges.begin(); eit != edges.end(); ++eit)
			{
				BGL_Edge e = (*eit);
				std::string type = this->graphModel->getProperties(e)->getTypeLabel();

				if (type != "Modifier") continue;

				BGL_Vertex s = this->graphModel->getSource(e);
				CloneContent * sClone;
				Connector * c = this->getConnector(s, r, e);
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

		// now we update the path of edges
		this->connectorLayoutManager->layout();
	}

	// to do a quick update, we translate the connectors that were selected and moved, and update the others?
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
Connector * GraphLayout::connect(BGL_Edge e)
{
	CloneContent * uc = this->findClone(e, true);
	if (!uc) return NULL;

	CloneContent * vc = this->findClone(e, false);	
	if (!vc) return NULL;

	return this->connectClones(uc, vc, e);
}

Connector * GraphLayout::connectClones(CloneContent * uc, CloneContent * vc, BGL_Edge e)
{	
	if (!this->graphModel->isEdge(uc->getVertex(), vc->getVertex())) return NULL;

	Connector * c =  new Connector (uc, vc, e);
	this->connectorList.push_back(c);

	// [!] to display adequate SBGN reactions in views created on the fly
	if (this->graphModel->getProperties(e)->getTypeLabel() == "Reactant") c->setTargetConnection(in);
	if (this->graphModel->getProperties(e)->getTypeLabel() == "Product")  c->setSourceConnection(out);		
	if (this->graphModel->getProperties(e)->getTypeLabel() == "Modifier") c->setTargetConnection(side1);
	// [!] pbm: this should be in pathways, not arcadia

	return c;
}

/***************
* getConnector *
****************
* Given a pair of BGL_Vertex,
* browses the list of connectors to find the one connecting them
* [!] that looks terribly inefficient. Better indexing?
****************************************************************/
Connector * GraphLayout::getConnector(BGL_Vertex u, BGL_Vertex v, BGL_Edge e)
{
	Connector * c = NULL;
	for (std::list<Connector*>::iterator it = this->connectorList.begin(); it != this->connectorList.end(); ++it)
	{
		c = (*it);
		
		c->getSource()->getVertex();
		c->getTarget()->getVertex();
		
		if (c->getSource()->getVertex() == u) if (c->getTarget()->getVertex() == v) if (c->getEdge() == e) break;

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
void GraphLayout::unmap(CloneContent * cd)
{
	this->cloneMap[cd->getVertex()].remove(cd);
}

/****************
* toggleCloning *
*****************
* Whether the vertex already has several clones, or not
* we unclone or clone it
*******************************************************/
void GraphLayout::toggleCloning(BGL_Vertex v, CloneContent * c)
{
	if ( (this->getClones(v).size() == 1) && (!this->getClone(v)->getNeighbours().size()) )
	{
		this->clone(v);
	}
	else
	{
		this->unclone(v, c);
	}
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

	std::string compartment = this->graphModel->getProperties(c->getVertex())->getCompartment();

	// We look at all the edges connected to the outside of the branching candidate
	std::list<Connector*> connectors = c->getOutterConnectors();	
	for (std::list<Connector*>::iterator it = connectors.begin(); it != connectors.end(); ++it)
	{
		Connector * connect = *it;
		// we look at the neighbour of the branching candidate along that edge
		CloneContent* n = connect->getNeighbour(c);
		// if that neighbour belongs to a cloneContainer, it gets ignored
		if (n->getContainer() == cloneContainer) continue;

		// NO BRANCH OR TRIANGLE CAN SPAN SEVERAL COMPARTMENTS!
		if ( compartment != this->graphModel->getProperties(n->getVertex())->getCompartment() )
		{
//			std::cerr << "branch and triangle can't span several compartments" << std::endl;
			return;			
		}

		// we count the number of nodes the neighbour is connected to
		int nn = n->getOutterConnectors().size();
		if (nn > 1) // if the neighbour is not a leaf...
		{
			if ((n == r1) || (n == r2)) continue; // different connectors but same nodes, counts only once
		
			if (r1) // if this is not the first non leafy neighbour
			{
				// if there are already two non leafy neighbours, this is not a triangle
				if (r2) { /* std::cerr << "too many neighbours, can't triangle" << std::endl; */ return; }
				else { r2 = n; } // else we have a second non leafy neighbour
			}
			else { r1 = n; } // else we have a first non leafy neighbour
		}
		else { leaves.push_back(n); } // else, the neighbour is added to the leaves list
	}
	// if all the neighbours are leaves, there is no main trunk to branch from!
	if (!r1)
	{
//		std::cerr << "no main trunk, can't branch this" << std::endl;
		return;
	}

	ContainerContent * branchContainer = NULL;

	// if we have a potential triangle configuration
	if (r2)
	{
		// if there are non-clone leaves, this is not a triangle...
		if (!leaves.empty()) { /* std::cerr << "too many leaves for a triangle layout" << std::endl; */ return; }

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
		if (!found) { /* std::cerr << "no common child or ancestor for triangle" << std::endl; */ return; }
		
		// additional test: if any of the nodes is already in a triangle container, we stop everything
		if (r1->getContainer()->getContentLayoutStrategy() == Triangle)
		{
//			std::cerr << r1->getLabel() << " already in triangle container, can't triangle further" << std::endl;
			return;
		}
		if (r2->getContainer()->getContentLayoutStrategy() == Triangle)
		{
//			std::cerr << r2->getLabel() << " already in triangle container, can't triangle further" << std::endl;
			return;		
		}
		if (parent->getContentLayoutStrategy() == Triangle)
		{
//			std::cerr << c->getLabel() << " already in triangle container, can't triangle further" << std::endl;
			return;		
		}
		
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
		// additional test: if any of the nodes is already in a triangle container, we stop everything
		if (r1->getContainer()->getContentLayoutStrategy() == Triangle)
		{
//			std::cerr << r1->getLabel() << " already in triangle container, can't branch this" << std::endl;
			return;
		}
		if (parent->getContentLayoutStrategy() == Triangle)
		{
//			std::cerr << c->getLabel() << " already in triangle container, can't branch this" << std::endl;
			return;		
		}

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
CloneContent * GraphLayout::findClone (BGL_Edge edge, bool isSource)
{
	CloneContent * cd = NULL;

	BGL_Vertex vertex; // we consider either the source or target of the edge
	if (isSource)	vertex = this->graphModel->getSource(edge);
	else			vertex = this->graphModel->getTarget(edge);
	
	// we look at each clones of the given vertex
	std::list<CloneContent *> clones = this->getClones(vertex);
	for (std::list<CloneContent *>::iterator it = clones.begin(); it != clones.end(); ++it)
	{
		cd = *it; // for each candidate
		
		// we look at the list of neighbour edges
		std::list<BGL_Edge> eList = cd->getNeighbourEdges();
		// if the list is empty,
		if ( eList.empty() ) break; // the clone is connected to ALL its neighbour edges
		else // can we find the neighbour edge in the list?
			if ( std::find(eList.begin(), eList.end(), edge) != eList.end() ) break; // yes!
			else cd = NULL; // wrong candidate
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
	// Old clone, to destroy
	CloneContent * cd = this->getClone(v);	
		
	// For each connection with another element...
	std::list<Connector*> connectors = cd->getOutterConnectors();
		
	for (std::list<Connector*>::iterator it = connectors.begin(); it != connectors.end(); ++it)
	{
		// we create a new clone, at the same position as the old clone
		CloneContent * newClone = new CloneContent(v, this);
		newClone->setPosition(cd->x(), cd->y());

		// we identify the source and target of the connector
		CloneContent * source = (*it)->getSource();
		CloneContent * target = (*it)->getTarget();
		CloneContent * root = NULL;

		// we set up the new clone as the new source or target of the connector (same as old clone)
		if (cd == source) { (*it)->setSource(newClone); root = target; }
		if (cd == target) {	(*it)->setTarget(newClone); root = source; }
		// root is the other end of the connector

		// the other end of the connector is set as the sole neighbour of the new clone
		newClone->addNeighbour(root->getVertex(), (*it)->getEdge());

		// We determine the current container of the root (= new clone's neighbour)
		ContainerContent * parent = root->getContainer();
		// No clone container should be created when the overall layout is of neighbourhood type
		if (parent->getContentLayoutStrategy() == Neighbourhood) { continue; }
		
		// is the root in a different compartment? In that case, no clone container is created
		std::string compClone = this->getGraphModel()->getProperties( cd->getVertex() )->getCompartment();
		std::string compRoot = this->getGraphModel()->getProperties( root->getVertex() )->getCompartment();		
		if ( compClone != compRoot )
		{
			// [!] We need to look for the compartment container (supposedly in the container "ancestry" of the old clone)
			ContainerContent * newContainer = cd->getContainer();
			while( newContainer )
			{
				if (newContainer->getReference() == compClone) break;
				newContainer = newContainer->getContainer();
			}
			if (newContainer) newContainer->add(newClone);
			else
			{
				std::cout << "That's awkward... " << newClone->getLabel() << " in " << compClone << std::endl;
				throw std::exception();
			}
			continue;
		}
		// we just make sure to put the new clone in the compartment of the old clone
		// [!] the concept of compartment is currently a pathway one, not an arcadia one!!!
		
		ContainerContent * cloneContainer = NULL;

		// if the root/neighbour already belongs to a clone container
		if (parent->getContentLayoutStrategy() == Clone)
		{
			// we use that existing clone container
			cloneContainer = parent;
						
			// has(cd))  // [!] alternative test?
			if (parent->getCore() == cd) // slightly convoluted case where the old clone is ALREADY in a clone container (probably as a root)
			{
				// we consider the container of that container
				ContainerContent * grandPa = parent->getContainer();
				if (grandPa)
				{
					// we transfer all the content of the clone container into its parent
					grandPa->add(parent->getChildren());
					// we delete the clone container of which the old clone is the root
					delete parent;
					// we consider there is no existing clone container
					cloneContainer = NULL;
					// and that the new clone container should be created in the parent of the deleted clone container
					parent = grandPa;
				}
			}
		}
		
		// if there is no existing clone container
		if (cloneContainer == NULL)
		{ 
			// we create a new one, in the current container of the root (or its parent, cf convoluted case)
			cloneContainer = new ContainerContent(this, parent, "CloneContainer");
			// we set that container as a clone container
			cloneContainer->setContentLayoutStrategy(Clone); // [!] what about edges? avoiding?
			// we add the root/neighbour to it, as a root
			cloneContainer->add(root, true);
		}
		// we add the new clone
		cloneContainer->add(newClone);
		// and we lay the container out
		cloneContainer->layoutContent();		
	}

	// deletion of the old clone
	
	// is the old clone belongs to a triangle container, that container makes no sense anymore
	ContainerContent * parent = cd->getContainer();
	if (parent->getContentLayoutStrategy() == Triangle) 
	{
		// we consider the container of that container
		ContainerContent * grandPa = parent->getContainer();
		if (grandPa)
		{
			// we transfer all the content of the clone container into its parent
			grandPa->add(parent->getChildren());
			// we delete the container of which the old clone is the root
			delete parent;
		}	
	}

	// then we delete the old clone	
	delete cd;

	// we update the edges
	this->update(true);
}

void GraphLayout::map(ContainerContent * container)
{
	std::string containerRef = container->getReference();
	if (containerRef == "") return;
	this->refToContainer[containerRef] = container;
}
	
void GraphLayout::unMap(ContainerContent * container)
{
	std::string containerRef = container->getReference();
	if (containerRef == "") return;
	this->refToContainer.erase(containerRef);	
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
	// we look at all the current clones
	std::list<CloneContent *> cList = this->getClones(v);

	// we create a new clone
	CloneContent * newClone = new CloneContent(v, this);

	// if an old clone position is given, we use it for the new clone
	if (c) newClone->setPosition(c->x(), c->y());

	// we prepare a new parent container (undefined at the beginning)
	ContainerContent * newParent = NULL;
	
	// [!] strange case when the user cloned a species that is involved in no reactions...?
	// we need to put it back in its original compartment
	if (cList.empty())
	{
		std::string compClone = this->getGraphModel()->getProperties( v )->getCompartment();
		if ( compClone != "" )
		{
			ContainerContent * newContainer = this->refToContainer[compClone];			
			newContainer->add(newClone);
			newClone->setPosition(0,0);
		}
	}

	// for each existing old clone
	for (std::list<CloneContent *>::iterator ic = cList.begin(); ic != cList.end(); ++ic)
	{
		CloneContent * cd = *ic;
	
		// if no position was offered, the new clone is placed at the position of the old one
		if (!c) newClone->setPosition(cd->x(), cd->y());
		// [!] was of time! shouldn't move the new clone n times, 1 is enough!

		// we look at all the connectors of the old clone
		std::list<Connector*> connectors = cd->getOutterConnectors();
		for (std::list<Connector*>::iterator it = connectors.begin(); it != connectors.end(); ++it)
		{
			// we consider the source and target of the connector
			CloneContent * source = (*it)->getSource();
			CloneContent * target = (*it)->getTarget();

			// we determine if the old clone is source or target, and redirect the appropriate end of the connector to the new clone
			if (cd == source) (*it)->setSource(newClone);
			if (cd == target) (*it)->setTarget(newClone);
		}

		// [!] implement automatic unnesting instead of manually deleting the useless container?
		// we consider the container of the old clone (always a clone container?) : is it still useful?
		ContainerContent * cloneContainer = cd->getContainer();	

		////////////////////////////////////////////////////////////////////////////////////////////

		ContainerContent * parent;
		Content * child = NULL;

		if (cloneContainer->getContentLayoutStrategy() == Clone) // default case
		{
			// we look at the parent of the container of the old clone
			parent = cloneContainer->getContainer();
			// if there is none, there must be a bug somewhere, as clones are supposed to always be in a container inside of the root container
			if (!parent) parent = cloneContainer; // we keep that one, child remains NULL (root == clone container, happens in reaction view) [!]
			else child = cloneContainer;
		}	
		else // if the container of the old clone is NOT a clone container
		{
			child = cd;
			parent = cloneContainer;
		}

		// if newparent has not been initialized yet, we set it to be that parent container
		if (!newParent)	newParent = parent;
		// if there is already a candidate for newparent, we pick the common ancestor of the new clone and the old clone (or clone container)
		else if (child) newParent = Content::CommonAncestor(newClone, child); // normal case
		else newParent = parent; // [!] strange case where parent == clone container == root
		
		// the new clone is added to its new parent
		newParent->add(newClone); // places the new clone in the common ancestor of all its previous clones?
		// [!] this should now work ok with compartments, but implicitly (because clone never leave their compartments)

		// we delete the now useless old clone
		delete cd;

		// if the old clone container now contains 1 or less nodes (and is not root)
		if ( (cloneContainer->getContentLayoutStrategy() == Clone) && (cloneContainer->getChildren().size() < 2) && child )
		{
			// we add the potentially remaining child to its parent container
			parent->add(cloneContainer->getChildren().front());
			// and delete the useless old clone container
			delete cloneContainer;
		}
		
		// check how many things the modified containers now contain (if they are triangle containers)
		if ( (parent->getContentLayoutStrategy() == Triangle) && (parent->getChildren().size() != 3) )
		{
			ContainerContent * gramps = parent->getContainer();

			gramps->add(parent->getChildren());
			delete parent;
			if (newParent == parent) newParent = gramps;
		}
	}

	this->update(true);
}

// roots is the core list of clones from a distance of which we wish to expand the neighbourhood
// neighbourhood is the result of the expansion
// depth is the remaining distance to cover to finish the expansion
std::list<CloneContent*> GraphLayout::recursiveNeighbourhoodBuilding(std::list<CloneContent*> roots, std::list<CloneContent*> neighbourhood, int depth)
{
	if (depth >= 0)	// when the depth reaches less than 0, we stop the recursion
	for (std::list<CloneContent*>::iterator it = roots.begin(); it != roots.end(); ++it) // we look at every root
	{
		CloneContent* clone = *it;
		// we build the neighbours of the root at the required depth, and add these to the existing neighbourhood
		neighbourhood = this->buildNeighbours(clone, neighbourhood, depth != 0);
		// we also build up the neighbourhood around the neighbours of the current clone, but at at a decreased distance
		neighbourhood = this->recursiveNeighbourhoodBuilding(clone->getNeighbourClones(), neighbourhood, depth-1);
	}

	return neighbourhood;
}

// adds the connector and neighbour corresponding to the edge from the clone
// the neighbour may already exist in the neighbourhood (if not, we add it)
// incoming tells us which side of the edge the neighbour is
// isVisible is false when we are reaching the limit of the neighbourhood view: clone is at depth 0, and its neighbour at depth -1 (if not already drawn)
std::list<CloneContent*> GraphLayout::addNeighbourFromEdge(BGL_Edge edge, CloneContent * clone, std::list<CloneContent *> neighbourhood, bool incoming, bool isVisible)
{
	// if the corresponding connector already exists, do nothing
	if (clone->hasConnector(edge)) return neighbourhood;

	// we check that the clone we want the neighbour of is at the end of the edge we expect
	if (incoming && this->graphModel->getTarget(edge) != clone->getVertex()) throw std::runtime_error("GraphLayout::addNeighbourFromEdge\nClone and edge mismatch");
	if (!incoming && this->graphModel->getSource(edge) != clone->getVertex()) throw std::runtime_error("GraphLayout::addNeighbourFromEdge\nClone and edge mismatch");

	// we identify the node at the other end of the edge
	BGL_Vertex v = incoming? this->graphModel->getSource(edge) : this->graphModel->getTarget(edge);
	// we look for the corresponding clone...
	CloneContent * neighbour = NULL;

	// ... is it in the existing neighbourhood?
	for (std::list<CloneContent*>::iterator li = neighbourhood.begin(); li != neighbourhood.end(); ++li)
	{ // we look at each clone in the neighbourhood
		if ((*li)->getVertex() != v) continue; // we check we have the right vertex first
		std::list<BGL_Edge> eList = (*li)->getNeighbourEdges(); // we look at their neighbour edge list
		if (eList.empty() ) { neighbour = *li; break; } // if it's empty, then ALL edges of the node point to this clone => bingo
		else // if there is a neighbour edge list for the current clone
		{
			// we check if the current edge is in that list
			if ( std::find(eList.begin(), eList.end(), edge) != eList.end() ) { neighbour = *li; break; }
			// if yes, that clone is the one we are looking for
		} // but to be honest, I don't see why eList should be anything else than empty! [!] (no clones in neighbourhood view)
	}

	if (!neighbour) // if the clone at the other end of the edge doesn't exist yet
	{
		if (isVisible)  // if it should be visible, we create it and add it to the neighbourhood
		{
			neighbour = new CloneContent(v, this);
			neighbourhood.push_back(neighbour);
		}
//		else clone->hasMoreEdges = true; // if it should remain invisible, we mark the clone has having undisplayed edges
	}

	// if there is a visible neighbour, we need to create a new connector for the current edge
	if (neighbour)
	{
		if (incoming)	this->connectClones(neighbour, clone, edge);
		else			this->connectClones(clone, neighbour, edge);
	}

	return neighbourhood;
}

std::list<CloneContent*> GraphLayout::buildNeighbours(CloneContent * clone, std::list<CloneContent *> neighbourhood, bool isVisible)
{
	std::list<BGL_Edge> inEdges = this->graphModel->getInEdges(clone->getVertex());
	for (std::list<BGL_Edge>::iterator ei = inEdges.begin(); ei != inEdges.end(); ++ei)
		neighbourhood = this->addNeighbourFromEdge(*ei, clone, neighbourhood, true, isVisible);

	std::list<BGL_Edge> outEdges = this->graphModel->getOutEdges(clone->getVertex());
	for (std::list<BGL_Edge>::iterator ei = outEdges.begin(); ei != outEdges.end(); ++ei)
		neighbourhood = this->addNeighbourFromEdge(*ei, clone, neighbourhood, false, isVisible);

	return neighbourhood;			
}

/*
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
*/
/*
// neighbourhood at distance one? then expand reactions when another species is present
void GraphLayout::fillGaps(std::list<CloneContent*> roots)
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
*/

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