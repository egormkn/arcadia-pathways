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
 *  GraphvizGraphLoader.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#include "graphvizgraphloader.h"

// GraphViz
#include <gvc.h>

// STL
#include <sstream>
#include <iostream>

// Local
#include "graphmodel.h"
#include "graphlayout.h"
#include "clonecontent.h"
#include "connector.h"

/***********
* getModel *
************
* [!] Just generates a void model ATM
*************************************/
GraphModel * GraphvizGraphLoader::getModel()
{
	GraphModel * gm = new GraphModel();
	return gm;
}

/*******
* save *
********
* For every layout in the model, a dot file is created
* Every clones are saved at their current position
* [!] The node names are just number, not the clone's label!
* The edges are saved too, of course
************************************************************/
void GraphvizGraphLoader::save(GraphModel *m, std::string fName)
{
	unsigned int layoutNumber = m->layoutNumber();
	for(unsigned int l = 0; l < layoutNumber; ++l) {
		GraphLayout * graphLayout = m->getLayout(l);

		std::ostringstream nstream;
		nstream << GraphLoader::GetFileNameOnly(fName) << "_" << l << ".dot";
		std::string name = nstream.str();
		
		std::map <CloneContent*, Agnode_t *> cloneToGraphVizNode;	

		GVC_t *graphContext = gvContext();
		Agraph_t *graph = agopen("g", AGDIGRAPH);

		int nodeId = 0;

		std::list<CloneContent*> cloneDescriptors = graphLayout->getCloneContents();
		for (std::list<CloneContent*>::iterator it = cloneDescriptors.begin(); it != cloneDescriptors.end(); ++it)
		{
				CloneContent *clone = *it;			

				std::ostringstream nodeName;
				nodeName << nodeId++;

				Agnode_t * gvNode = agnode(graph, (char*)nodeName.str().c_str());
				cloneToGraphVizNode[clone] = gvNode;

				std::ostringstream pos;
				pos <<  clone->x()/100 << ", " <<  -clone->y()/100;

				agsafeset(gvNode, "pin", "true", "true");
				agsafeset(gvNode, "pos", (char*) pos.str().c_str(), "0,0");
		}

		std::list<Connector*> connectors = graphLayout->getConnectors();
		for (std::list<Connector*>::iterator it = connectors.begin(); it != connectors.end(); ++it)
		{
			CloneContent * source = (*it)->getSource();
			CloneContent * target = (*it)->getTarget();
		
			agedge(graph, cloneToGraphVizNode[source], cloneToGraphVizNode[target]);
		}

		FILE *f = fopen(name.c_str(), "w");
		agwrite(graph, f);
		fclose(f);

		agclose(graph);
		gvFreeContext(graphContext);
	}
}