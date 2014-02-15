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
 *  PathwayGraphModel.h
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#ifndef PATHWAYGRAPHMODEL_H
#define PATHWAYGRAPHMODEL_H

// local base class
#include <arcadia/graphmodel.h>

// LibSBML
class SBMLDocument;
class Model;
class Species;
class Reaction;
class Compartment;
class SBase;
class Layout;
class XMLNode;
class FunctionDefinition;

// local, because I do a lot of saving and loading here
class ContainerContent;


/***************
* PathwayGraphModel *
****************
* This subclass of GraphModel manages an SBMLDocument
* It provides access to the document's title
* and lets it be saved as an SBML file (with layout too)
*
* Internally, access to the document's model is facilitated
* by a map between SBML id and BGL_Vertex
*
* Consequently there are specific method to add and remove Vertices
*
* There are also method to access Species, Reactions and Compartments
*
* Finally, specific method are provided to load and save layout info
* (including Container information)
* [!] that's a bit out of place now? what about loaders?
*/
class PathwayGraphModel : public GraphModel
{
public:
	PathwayGraphModel(SBMLDocument * doc = NULL, std::string fName = "Untitled.SBML");
	~PathwayGraphModel();

	std::string getTitle();
	
	void save(std::string fName = "");
	
	void toggleFusing(std::list<BGL_Vertex> vList);
	
	std::string getLabelFromId(std::string id);
	std::string getCompartmentLabel(std::string id);
	// Returns: pair.first = function body, pair.second = function arguments
	std::pair< std::string, std::list<std::string> > getFunctionDefinitionAsStrings(std::string id);
	std::string getFunctionName(std::string id);
	FunctionDefinition * getFunctionDefinition(std::string id);
		
private:
	BGL_Edge findEdgeFromNeighbourRelationship(CloneContent * clone, BGL_Vertex neighbour, std::string relationship);

	SBMLDocument * document;
	std::map<std::string, BGL_Vertex> idToVertex;

	Model * model();

	// modifying the graph model
	BGL_Vertex addPathwayVertex(VertexProperty * properties = NULL);
	void removePathwayVertex(BGL_Vertex v);

	// accessing the sbml elements
	const unsigned int speciesNumber();
	const unsigned int reactionsNumber();
	const unsigned int compartmentsNumber();

	Species * getSpecies(unsigned int i);
	Reaction * getReaction(unsigned int i);
	Compartment * getCompartment(unsigned int i);
	Compartment* getCompartment(std::string s);

	// dealing with layout... [!] is that the right class to put that stuff in???
	// this is very much a file interface thing... => "Loader" class? (for the load/save bit)

	// creating a new, default layout from scratch
	void newLayout(bool update);	
	// this is useful, but the implementation looks dodgy [!]... Shouldn't the mapping be stored at layout level to begin with??
	// (that would solve the problem of updating the map when a layout is deleted)
	// The problem is that compartment is a pathway only concept... Maybe I should create a generic subgraph concept in my graph model
	// and link it to compartments just like vertices are linked to species and reactions (cf. properties)?
	void defaultLayout(bool update);

	std::map<GraphLayout *, std::map<std::string, ContainerContent *> > compartmentToContainer;

	// loading layout info [!] a loader thing
	void loadLayoutInfo();

/*
	bool loadLegacyLayoutInfo(unsigned int layoutNumber);
	std::map<std::string, ContainerContent*> loadLegacyContainerInfo(GraphLayout * graphLayout, char * containerLine);
	void loadLegacyCloneInfo(GraphLayout * graphLayout, std::map<std::string, ContainerContent*> mapContainer, char * id, char * coords);
*/

	void loadXMLLayoutInfo();
	void loadXMLLayoutInfo(XMLNode * layoutNode);
	void loadXMLContentInfo(XMLNode * contentNode, GraphLayout * graphLayout, ContainerContent * parent);

	// saving layout info [!] a loader thing
	void saveLayoutInfo();
	
	void saveXMLLayoutInfo();
	std::string saveXMLLayoutInfo(unsigned int l);
	std::string saveXMLContainerInfo(ContainerContent * c);
	std::string saveXMLCloneInfo(CloneContent * c);

	void saveExtensionLayoutInfo();
	void saveExtensionLayoutInfo(Model * m, GraphLayout * graphLayout, unsigned int l);
	std::string saveExtensionSpeciesLayoutInfo(CloneContent * clone, Layout * layout, int i, int c, int X0, int Y0);
	void saveExtensionReactionLayoutInfo(CloneContent * clone, Layout * layout, int i, std::map<CloneContent *, std::string> cloneToGlyph, int X0, int Y0);
	void saveExtensionCompartmentLayoutInfo(ContainerContent * cont, Layout * layout, int i, Compartment * c, int X0, int Y0);
};

#endif