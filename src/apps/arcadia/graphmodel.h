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
 *  GraphModel.h
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *	Last Documented on 17/04/2008.
 *
 */

#ifndef GRAPHMODEL_H
#define GRAPHMODEL_H

// STL
#include <string>

// BGL (for all the template graph-related types)
#include <boost/graph/adjacency_list.hpp>
using namespace boost;

// local
class VertexProperty;
class EdgeProperty;
class GraphLayout;
class CloneContent;
//class StyleSheet;
class StyleSheet;

/**************************************
* Type declarations for using the BGL *
* [!] should be in a separate header? *
***************************************/

// The Graph type
typedef adjacency_list<	listS, // out edges
						listS, // vertices
						bidirectionalS,
						property<vertex_name_t, VertexProperty*>, // each vertex can have properties
						property<edge_name_t, EdgeProperty*> // and so does each edge
					> BGL_Graph;

// The Vertex and Edge types
typedef graph_traits<BGL_Graph>::vertex_descriptor BGL_Vertex;
typedef graph_traits<BGL_Graph>::edge_descriptor BGL_Edge;

// The Iterator types
typedef graph_traits<BGL_Graph>::vertex_iterator BGL_Vertex_iter;
typedef graph_traits<BGL_Graph>::edge_iterator BGL_Edge_iter;
typedef graph_traits<BGL_Graph>::in_edge_iterator BGL_In_iter;
typedef graph_traits<BGL_Graph>::out_edge_iterator BGL_Out_iter;
typedef graph_traits<BGL_Graph>::adjacency_iterator BGL_Adj_iter;

/*************
* GraphModel *
**************
* The model refers directly to a file by its name
* The title of the graph is the name of the file, by default 
*
* There is an interface for a generic save method that, by default
* saves the graph in dot format, and as a text version
* The stringVersion method is used for the text save (+ debugging)
*
* The graph structure is provided by a BGL_Graph,
* which is accessed through a public interface that lets
* adding and removing BGL_Vertex and BGL_Edge objects
* (individually or in bulk for removal)
* Also we can obtain lists of BGL_Vertex and BGL_Edge objects
* for the whole graph, or adjacent to a given graph object
*
* BGL_Vertex and BGL_Edge are also associated to individual properties
* through a protected property map (read only public access)
*
* Finally, the model hold a list of GraphLayout objects
* Default layout information can be generated with the newLayout method
* and all the layout destroyed with the deleteLayouts method
*
* Also, cloning can be toggled for a given vertex in a given layout
***********************************************************************/
class GraphModel
{
public:
	GraphModel(std::string fName = "No Document", bool createStyleSheet = true);
	virtual ~GraphModel();

	// file name and title
	std::string getFileName();
	void setFileName(std::string fName);
	virtual std::string getTitle();
	
	// save and output
	virtual void save(std::string fName = "");
	std::string stringVersion();
	
	// read vertices
	std::list<BGL_Vertex> getVertices();
	std::list<BGL_Vertex> getNeighbours(BGL_Vertex v);
	std::list<BGL_Edge> getEdges(BGL_Vertex v);
	std::list<BGL_Edge> getInEdges(BGL_Vertex v);
	std::list<BGL_Edge> getOutEdges(BGL_Vertex v);
	bool isEdge(BGL_Vertex u, BGL_Vertex v);

	// read edges
	std::list<BGL_Edge> getEdges();
	BGL_Vertex getSource(BGL_Edge e);
	BGL_Vertex getTarget(BGL_Edge e);
	BGL_Edge getEdge(BGL_Vertex u, BGL_Vertex v);

	// write vertices
	BGL_Vertex addVertex(VertexProperty * properties = NULL);
	void removeVertex(BGL_Vertex v);
	void removeVertices(std::list<BGL_Vertex> vList);
	void removeVertices();

	// write edges
	BGL_Edge addEdge(BGL_Vertex source, BGL_Vertex target, EdgeProperty * properties = NULL);
	void removeEdge(BGL_Edge e);
	void removeEdges(std::list<BGL_Edge> eList);
	void removeEdges();
	void removeEdges(BGL_Vertex v);		

	// properties
	VertexProperty * getProperties(BGL_Vertex v);
	EdgeProperty * getProperties(BGL_Edge e);
	
	// read layout
	int layoutNumber();
	GraphLayout * getLayout(int number);
	int getNumber(GraphLayout *);

	// write layout
	virtual void newLayout(bool update);
	virtual void defaultLayout(bool update);
	void deleteLayouts();
	void toggleCloning(BGL_Vertex v, GraphLayout * gl, CloneContent * c);
	void updateLayout(GraphLayout * gl, bool edgesOnly, bool fast);
	GraphLayout * addLayout(std::list<BGL_Vertex> vList, bool neighbourhood = true);
	void expandLayout(std::list<BGL_Vertex> vList, GraphLayout * gl);
	
	GraphLayout * getNextLayout(GraphLayout * gl, int i);
	
	StyleSheet * getStyleSheet() { return this->layoutStyleSheet; }

	void toggleAvoidingEdges();
	GraphLayout * destroyLayout();
	
	virtual void toggleFusing(std::list<BGL_Vertex> vList) {}

private:
	BGL_Graph graph;
	
protected:
	StyleSheet * layoutStyleSheet;

	std::string fileName;

	property_map<BGL_Graph, vertex_name_t >::type vertexProperties;
	property_map<BGL_Graph, edge_name_t >::type edgeProperties;

	std::vector< GraphLayout* > layoutInformation;	
};

#endif