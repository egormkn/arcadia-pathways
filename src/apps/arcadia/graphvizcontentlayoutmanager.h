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
 *  GraphvizContentLayoutManager.h
 *  arcadia
 *
 *  Created by Alice Villeger on 06/03/2008.
 *  Last documented on 17/04/2008.
 *
 */

#ifndef GRAPHVIZCONTENTLAYOUTMANAGER_H
#define GRAPHVIZCONTENTLAYOUTMANAGER_H

// local base class
#include "contentlayoutmanager.h"

#include "connector.h"

// STL
#include <map>
#include <string>

// GraphViz
#include <gvc.h>

// local
class Content;

/************************
* GraphvizContentLayoutManager *
*************************
* Uses a graphviz library method to layout some content (dot, by default)
* Internally, this requires graphviz objects (graph and graphContext)
* and a map between content and graphviz nodes
*
* The graphviz graph and graphContext are created and destroyed along with the class
* However the graph content (nodes, edges) is reset every time a layout is computed
************************************************************************************/
class GraphvizContentLayoutManager: public ContentLayoutManager {
public:
	GraphvizContentLayoutManager(std::string m = "dot");
	~GraphvizContentLayoutManager();
	void layout(ContainerContent * container);

private:
    Agraph_t *graph;
    GVC_t *graphContext;
	std::map <Content*, Agnode_t *> contentToGraphVizNode;
	std::string method;
	
	std::list<Connector*> dealWithBranches(ContainerContent * container, std::list<Connector*> &connectors);
	
	std::list<ContainerContent*> initTriangleContainerNodes(std::list<Content*> & cList);
	void initTriangleContainerEdges(std::list<ContainerContent*> tList, ContainerContent * container);
	void layoutTriangleContainers(std::list<ContainerContent*> tList);
};

#endif