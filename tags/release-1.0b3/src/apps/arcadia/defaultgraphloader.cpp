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
 *  DefaultGraphLoader.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *	Last Documented on 17/04/2008.
 *
 */

#include "defaultgraphloader.h"

#include <fstream>

// local
#include "graphmodel.h"
#include "vertexproperty.h"
// for tests...
#include "graphlayout.h"
#include "clonecontent.h"
#include "containercontent.h"
#include "connector.h"

/***********
* getModel *
************
* creates a generic empty graph
* then fills it with a few named Vertex and Edges
* and creates a default layout for it
*************************************************/
GraphModel * DefaultGraphLoader::getModel()
{
	GraphModel * gm = new GraphModel();
/*
	BGL_Vertex v1 = gm->addVertex(new VertexProperty(1));
	BGL_Vertex v2 = gm->addVertex(new VertexProperty(2, false));
	BGL_Vertex v3 = gm->addVertex(new VertexProperty(3));
	BGL_Vertex v4 = gm->addVertex(new VertexProperty(4, false));
	BGL_Vertex v5 = gm->addVertex(new VertexProperty(5));

	gm->addEdge(v1, v2);
	gm->addEdge(v1, v3);
	gm->addEdge(v2, v3);

	gm->addEdge(v3, v4);
	gm->addEdge(v3, v5);
	gm->addEdge(v4, v5);

	gm->newLayout();

/*
//	gm->toggleCloning(v3, -1);
		
	GraphLayout * layout = gm->getLayout(0);

	Container * root = layout->getRoot();
	Container * c1 = new Container(root);
	c1->setContentLayoutStrategy(Clone);
	Container * c2 = new Container(root);
	c2->setContentLayoutStrategy(Clone);

	c1->add(layout->getClone(v1));
	c1->add(layout->getClone(v2));

	c2->add(layout->getClone(v4));
	c2->add(layout->getClone(v5));

	layout->update();
*/

	return gm;
}

/*******
* save *
********
* Outputs the model's stringVersion in a text file
**************************************************/
void DefaultGraphLoader::save(GraphModel *m, std::string fName)
{
	std::ofstream oFile(fName.c_str());
	oFile << m->stringVersion();
	oFile.close();
}