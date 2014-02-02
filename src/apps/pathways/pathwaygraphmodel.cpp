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
 *  PathwayGraphModel.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#include "pathwaygraphmodel.h"

// STL, for the I/O bit
#include <sstream>
#include <fstream>
#include <stack>

#include <exception>

// LibSBML
#include <sbml/SBMLTypes.h>
#include <sbml/layout/LineSegment.h>

// Local
#include <arcadia/graphlayout.h>
#include <arcadia/clonecontent.h>
#include <arcadia/connector.h>
#include <arcadia/containercontent.h>

#include "sbmlgraphloader.h"
#include "speciesvertexproperty.h"
#include "reactionvertexproperty.h"
#include "reactantedgeproperty.h"
#include "productedgeproperty.h"
#include "modifieredgeproperty.h"
#include "compartmentcontainer.h"
#include "pathwaystylesheet.h"

FunctionDefinition * PathwayGraphModel::getFunctionDefinition(std::string id)
{ return this->model()->getFunctionDefinition(id); }

std::pair< std::string, std::list<std::string> > PathwayGraphModel::getFunctionDefinitionAsStrings(std::string id)
{
	std::string body = "";
	std::list<std::string> arguments;
	
	FunctionDefinition * fd = this->getFunctionDefinition(id);
	if (!fd) return std::pair< std::string, std::list<std::string> > (body, arguments);

	body = SBML_formulaToString( fd->getBody() );
	for (int i=0; i<fd->getNumArguments(); ++i)
		arguments.push_back( SBML_formulaToString( fd->getArgument(i) ) );

	return std::pair< std::string, std::list<std::string> > (body, arguments);
}

std::string PathwayGraphModel::getFunctionName(std::string id)
{
	std::string label = "";
	FunctionDefinition * fd = this->getFunctionDefinition(id);
	if (fd) { label = fd->getName(); if (label == "") label = id; }
	return label;
}

std::string PathwayGraphModel::getCompartmentLabel(std::string id)
{
	std::string label = "";
	Compartment * c = this->model()->getCompartment(id);
	if (c) { label = c->getName(); if (label == "") label = id; }
	return label;
}

std::string PathwayGraphModel::getLabelFromId(std::string id)
{
	std::string label = "";

	if (this->idToVertex.find(id) == this->idToVertex.end())
	{ // we are dealing with something that is neither a reaction or a species
		// could it be a compartment?
		label = this->getCompartmentLabel(id);
		if (label == "")
		{ // can be either an event, a functiondefinition, a parameter, a speciestype or a unitdefinition
		// We are only interested in Parameter at the moment (KineticLaw labels)
			Parameter * p = this->model()->getParameter(id);
			if (p) { label = p->getName(); if (label == "") label = id; } // rem: could be param of a kinetic law...
		}
	} // Species and Reactions
	else label = this->getProperties( this->idToVertex[id] )->getLabel();
		
	return label;
}

/**************
* Constructor *
***************
* Builds a PathwayGraphModel from an SBMLDocument
* The GraphModel is initialized with the given filename
* The Document is set up (new Document if none is given)
* 
* First, we add one vertex per species,
* with corresponding properties
*
* Then we browse the reactions
* and add one vertex per reaction,
* with corresponding properties
* Also we build the edges corresponding to
* reactants, products, and modifiers
* with corresponding properties again
* (each species vertex is found with the idToVertex map)
*
* Finally we load layout information
* [!] we should load Compartments from the model!
* [!] also the format used for layout information is dodgy
* [!] finally, shouldn't all that stuff be defined in the loader?
*****************************************************************/
PathwayGraphModel::PathwayGraphModel(SBMLDocument *doc, std::string fName) : GraphModel(fName, false), document(doc)
{
	if (this->layoutStyleSheet) delete this->layoutStyleSheet;
	this->layoutStyleSheet = new PathwayStyleSheet();

	if (!this->document) this->document = new SBMLDocument();

	// First, we add the species
	const unsigned int numSpecies = this->speciesNumber();
	for (unsigned int i=0; i<numSpecies; ++i) { this->addPathwayVertex( new SpeciesVertexProperty( this->getSpecies(i), this ) ); }

	// Then the reactions
	const unsigned int numReactions = this->reactionsNumber();
	for (unsigned int i=0; i<numReactions; ++i)
	{
		Reaction * r = this->getReaction(i);
		ReactionVertexProperty * rp = new ReactionVertexProperty( this->getReaction(i), this );
		BGL_Vertex reaction = this->addPathwayVertex( rp );
			
		bool reversible = rp->isReversible();
			
		// Also builds the edges between the reaction and the involved species...
		// The reactants
        int numReactants = r->getNumReactants();
		for (int reactantIndex=0; reactantIndex<numReactants; reactantIndex++)
		{	
			SpeciesReference * sr = r->getReactant(reactantIndex);
			this->addEdge( idToVertex[ sr->getSpecies() ], reaction, new ReactantEdgeProperty( sr, reversible ));
		}
		if (!numReactants) this->addEdge( this->addPathwayVertex( new SourceOrSinkProperty(true, rp) ), reaction, new ReactantEdgeProperty(NULL, reversible) );

		// The products
		int numProducts = r->getNumProducts();
		for (int productIndex=0; productIndex<numProducts; productIndex++)
		{
			SpeciesReference * sr = r->getProduct(productIndex);
			this->addEdge( reaction, idToVertex[ sr->getSpecies() ], new ProductEdgeProperty( sr, reversible ) );
		}
		if (!numProducts) this->addEdge( reaction, this->addPathwayVertex( new SourceOrSinkProperty(false, rp) ), new ProductEdgeProperty(NULL, reversible) );
		
		// The modifiers
		int numModifiers = r->getNumModifiers();
		for (int modifierIndex=0; modifierIndex<numModifiers; modifierIndex++)
		{
			ModifierSpeciesReference * mr = r->getModifier(modifierIndex);
			this->addEdge( idToVertex[ mr->getSpecies() ], reaction, new ModifierEdgeProperty( mr ) );
		}		
	}

	this->loadLayoutInfo();	
}

/*******
* save *
********
* For a given filename (possibly the default one)
* we save the layout information first
* [!] custom format!
* then write the SBML document in the current file
* as well as a default file "lastDocument.xml"
**************************************************/
void PathwayGraphModel::save(std::string fName)
{
	if (fName == "") fName = this->getFileName();

	if (SBMLGraphLoader::Save(this, fName))
	{
		this->setFileName(fName);

		this->saveLayoutInfo();		

		writeSBML(this->document, this->fileName.c_str());

//		writeSBML(this->document,"lastDocument.xml");
	}
}

/*************************************************
* Destructor: destroys the existing SBMLDocument *
*************************************************/
PathwayGraphModel::~PathwayGraphModel()
{
//	if (this->document) delete this->document; [!] problem when the same Utopia node gets given to two different controllers...
}

/***********
* getTitle *
************
* If there is no model, returns the usual GraphModel title
* If there is one, returns the name of the model (possibly empty)
* followed by the id of the model
* [!] what if there is no Id?
* [!] info on the document's Level?
*****************************************************************/
std::string PathwayGraphModel::getTitle()
{
	std::string title;

	if (this->model())
	{
		if (this->model()->getName() != "") title = this->model()->getName();
		else title = "[NoName]";
		title.append(" : ");
		title.append(this->model()->getId());
	}
	else title = GraphModel::getTitle();

	return title;
}

/*******************
* addPathwayVertex *
********************
* Creates a new vertex with the given properties
* Also maps the id to the vertex, and returns it
************************************************/
BGL_Vertex PathwayGraphModel::addPathwayVertex(VertexProperty * properties)
{
	BGL_Vertex v = this->addVertex(properties);

	VertexProperty * vp = this->vertexProperties[v];
	std::string id = vp->getId();
	this->idToVertex[id] = v;
	
	return v;
}

/**********************
* removePathwayVertex *
***********************
* Removes the vertex from the idToVertex map
* then removes the vertex from the graph
* [!] is that method actually ever used?
********************************************/
void PathwayGraphModel::removePathwayVertex(BGL_Vertex v)
{
	VertexProperty * vp = this->vertexProperties[v];
	std::string id = vp->getId();
	this->idToVertex.erase(id);
	
	this->removeVertex(v);
}

/**************************************
* model: returns the Document's Model *
**************************************/
Model * PathwayGraphModel::model() { return this->document->getModel(); }

/****************************************************************************
* speciesNumber: returns the number of species in the Model (0 if no Model) *
****************************************************************************/
const unsigned int PathwayGraphModel::speciesNumber() { return this->model()? this->model()->getListOfSpecies()->size(): 0; }
/********************************************************************************
* reactionsNumber: returns the number of reactions in the Model (0 if no Model) *
********************************************************************************/
const unsigned int PathwayGraphModel::reactionsNumber() { return this->model()? this->model()->getListOfReactions()->size(): 0; }
/**************************************************************************************
* compartmentsNumber: returns the number of compartments in the Model (0 if no Model) *
**************************************************************************************/
const unsigned int PathwayGraphModel::compartmentsNumber() { return this->model()? this->model()->getListOfCompartments()->size(): 0; }

/************************************************************************************************************************
* getSpecies: returns the species number i in the Model (NULL if no Model) [!] doesn't check whether i is in the range! *
************************************************************************************************************************/
Species * PathwayGraphModel::getSpecies(unsigned int i) { return this->model()? (Species *) this->model()->getListOfSpecies()->get(i): NULL; }
/**************************************************************************************************************************
* getReaction: returns the reaction number i in the Model (NULL if no Model) [!] doesn't check whether i is in the range! *
**************************************************************************************************************************/
Reaction* PathwayGraphModel::getReaction(unsigned int i) { return this->model()? (Reaction *) this->model()->getListOfReactions()->get(i): NULL; }
/********************************************************************************************************************************
* getCompartment: returns the compartment number i in the Model (NULL if no Model) [!] doesn't check whether i is in the range! *
********************************************************************************************************************************/
Compartment * PathwayGraphModel::getCompartment(unsigned int i) { return this->model()? (Compartment *) this->model()->getListOfCompartments()->get(i): NULL; }

Compartment* PathwayGraphModel::getCompartment(std::string s)
{
	Model *m = this->model();
	if (m) return (Compartment*) m->getCompartment(s);
	else return NULL;
}

void PathwayGraphModel::newLayout(bool update)
{
	this->deleteLayouts();	

	this->defaultLayout(update);
}

void PathwayGraphModel::defaultLayout(bool update)
{
	// Generates a layout containing all the nodes and edges in the graph (no cloning)
	GraphModel::defaultLayout(false); // "false" argument: doesn't compute nodes location
	
	// The one and only generated layout
	GraphLayout * graphLayout = this->layoutInformation[this->layoutInformation.size()-1];

	// We create one container per compartment
	const unsigned int numCompartments = this->compartmentsNumber();
	for (unsigned int i=0; i<numCompartments; ++i)
	{
		Compartment * comp = this->getCompartment(i);		
		// The new compartment container is linked to its sbml id, and placed at the root of the layout
		ContainerContent * cont = new CompartmentContainer(graphLayout, graphLayout->getRoot(), comp);
		// Linking compartment id to container (temp storage)
		this->compartmentToContainer[graphLayout][comp->getId()] = cont;
	}
	
	// We look at all inside/outside relationships and nest compartment containers accordingly
	for (unsigned int i=0; i<numCompartments; ++i)
	{
		Compartment* comp = this->getCompartment(i);
		const std::string out = comp->getOutside();
		if (out != "") // if there is a compartment containing the current compartment ("outside")
		{
			// We find the container of the current compartment
			ContainerContent * child = this->compartmentToContainer[graphLayout][comp->getId()];
			// We find the container of the "outside" compartment
			ContainerContent * parent = this->compartmentToContainer[graphLayout][out];
			parent->add( child ); // We put the former inside of the latter
		}
	} // We now have a container tree reflecting the containment relationship of sbml compartments

	// Now we just need to place each node in the appropriate compartment container
	std::list<BGL_Vertex> vList = this->getVertices();	
	// First pass: put all vertex with a compartment in their compartment container
	for (std::list<BGL_Vertex>::iterator it = vList.begin(); it != vList.end(); ++it)
	{
		BGL_Vertex v = *it; // for each node in the graph
		VertexProperty * vp = this->vertexProperties[v];

		std::string comp = vp->getCompartment();
		if (comp == "")
		{
			// Species should always have a compartment!
			if ( (vp->getTypeLabel(true) == "Species") && (vp->getTypeLabel() != "empty set") )
				throw std::runtime_error("PathwayGraphModel::defaultLayout()\nSpecies with no compartment");
			else continue; // will be dealt with at second pass
		}

		// We get the container corresponding to their compartment id
		ContainerContent * cont = this->compartmentToContainer[graphLayout][comp];
		if (!cont) throw std::runtime_error("PathwayGraphModel::defaultLayout()\nCompartment to container mismatch");
		// We add the clone of the species to the compartment container
		cont->add( graphLayout->getClone(v) );
	}

	// Second pass: assign reaction with no compartment to the right compartment (and container)
	// Also deal with their source or sink while we're at it
	for (std::list<BGL_Vertex>::iterator it = vList.begin(); it != vList.end(); ++it)
	{
		BGL_Vertex v = *it; // for each node in the graph
		VertexProperty * vp = this->vertexProperties[v];

		std::string comp = vp->getCompartment();
		if (comp != "") continue; // We are only interested in vertices with no compartments
		if (vp->getTypeLabel(true) != "Reaction") // Only reactions, actually
		{
			// only source or sink are also allowed to have no compartment
			if (vp->getTypeLabel() != "empty set")
				throw std::runtime_error("PathwayGraphModel::defaultLayout()\nVertex with no compartment can only be empty sets or reactions, not " + vp->getTypeLabel() + " or " + vp->getTypeLabel(true));
			continue;
		}

		// The clone of the reaction in the default layout
		CloneContent * rClone = graphLayout->getClone(v);
		// The compartment container of the reaction = ancestor container of all species
		ContainerContent * compartmentContainer = NULL;
		// List of source or sink attached to the reaction, to be put in same container
		std::list<BGL_Vertex> sourceOrSinkList;

		// We look at all the neighbours of that reaction, to find the ancestor container
		std::list<BGL_Vertex> neighbours = this->getNeighbours(v);
		for (std::list<BGL_Vertex>::iterator it = neighbours.begin(); it != neighbours.end(); ++it)
		{
			// Because it links to an outside reaction, the current species is a core of its compartment
			BGL_Vertex coreSpecies = *it;

			// If the neighbour is a source or sink
			if (this->vertexProperties[ coreSpecies ]->getTypeLabel() == "empty set")
			{
				// we put it in the list of source or sink, to be put it in the reaction's container
				sourceOrSinkList.push_back(coreSpecies);
				continue; // not used to find the ancestor compartment container
			}

			// We consider the clone of that species in the default layout
			CloneContent * sClone = graphLayout->getClone(coreSpecies);

			// We make that species' clone the core of its own container (because it links to an outside reaction)
			if ( !sClone->setAsCore() ) throw std::runtime_error("PathwayGraphModel::defaultLayout()\nCould not set reaction's neighbour species as core of its own container");

			// We look for the container of that species
			ContainerContent * c = sClone->getContainer();

			// The new compartment container of the reaction is the common ancestor of the old one with the new candidate
			if (compartmentContainer) compartmentContainer = ContainerContent::CommonAncestor(c, compartmentContainer);
			else compartmentContainer = c; // The compartment container is initialized with the first candidate
			// This process ensures the final compartmentContainer is the common ancestor of all species involved in the reaction
		}
		
		// We should have found a common ancestor of some sort at that stage
		if (!compartmentContainer) throw std::runtime_error("PathwayGraphModel::defaultLayout()\nNo common ancestor for reaction's neighbour species??");

		// Sometimes that common ancestor is not a compartment but the layout's root container
		if (compartmentContainer == graphLayout->getRoot()) continue;

		// We put the reaction and its source or sink in the compartment container
		compartmentContainer->add( graphLayout->getClone(v) );
		for (std::list<BGL_Vertex>::iterator it = sourceOrSinkList.begin();
			it != sourceOrSinkList.end(); ++it) compartmentContainer->add( graphLayout->getClone(*it) );
		
		// We get the id of the compartment (it should be non empty at that stage)
		std::string reactionCompartment = compartmentContainer->getReference();
		if (reactionCompartment == "") throw std::runtime_error("PathwayGraphModel::defaultLayout()\nReaction's container is not a compartment??");

		// We set the id of the reaction's compartment (it will automatically also be the source or sink's)
		((ReactionVertexProperty*)vp)->setCompartment(reactionCompartment);
	}

	// we compute the layout
	graphLayout->update();
}

/***************************************************************************************************
*
* Loading
*
***************************************************************************************************/

/*****************
* loadLayoutInfo *
******************
* First we make sure to delete all the existing layout information and clear the list
*
* Then we start looking for layout information files, starting with zero
* until no corresponding file is found
* [!] the layout information files are supposed to have a very specific naming scheme
*
* For each file found, we create a new GraphLayout
* The first line is supposed to deal with Container information
* It follows a recursive form: id ( c1 c2 ... cn ), where each ci has the same form
* <=> id + brackets containing the list of id of Container children
* In short, this line describes the whole container tree
* It is parsed accordingly, and a Container is created for every id found,
* and set to belong to its corresponding parent Container
* (the first container has no parent, but is set as the root of the current layout)
* Also, we make sure to map each id to the corresponding Container
* [!] but the real Container id is NOT the one found in the file!
* [!] it's an integer incremented automatically upon each Container's creation...
* 
* After that step, we look for clone information
* We first read a line containing the id of a clone, followed by the id of its neighbours
* The ids are tab separated, and corresponding vertices are found with idToVertex
* If no neighbours are found, we assume the clone to neighbour all the connected vertices
* A clone is created for each one of these lines we read
* The following line always contains position information on the clone, as a x \t y pair
* as well as the id of the clone's parent container
* The clone's position and parent Container (found with the map) are set accordingly 
*
* After that, we simply define Connectors in the layout for every edges in the model
* 
* The layout information gets added to the model,
* we close the file, and try to open the next one...
*
* [!] we don't load layouts stored with the SBML layout extension...
*****************************************************************************************/
void PathwayGraphModel::loadLayoutInfo()
{
	// the reset bit
	for (std::vector<GraphLayout*>::iterator it = this->layoutInformation.begin(); it != this->layoutInformation.end(); ++it) { delete (*it); }
	this->layoutInformation.clear();

// deprecated since the Pedro bug fix 2010/02/10
/*
	// loading existing layout in the native format
	for(unsigned int layoutNumber = 0; true; layoutNumber++) if (!this->loadLegacyLayoutInfo(layoutNumber)) break;
*/

	// loading info from annotations
	this->loadXMLLayoutInfo();	

	// if no layout is found, we create a default one
	if (!this->layoutNumber()) this->newLayout(true);
}

/***************************************************************************************************
* Loading: my XML annotation in SBML (preferred format)
***************************************************************************************************/

void PathwayGraphModel::loadXMLLayoutInfo()
{
	XMLNode * annotationNode = this->model()->getAnnotation();
	if (!annotationNode) return;

	XMLNode arcadiaNode; bool found = false;	
	int nc = annotationNode->getNumChildren();
	for (int i=0; i<nc; ++i)
	{
		arcadiaNode = annotationNode->getChild(i);
		if (arcadiaNode.getPrefix() == "arcadia") { found = true; break; }
	}
	if (!found) return;

	int nl = arcadiaNode.getNumChildren();
	for (int i=0; i<nl; ++i)
	{
		XMLNode * n = (XMLNode*)(&arcadiaNode.getChild(i)); // force the conversion from const XMLNode * for compatibility with libsbml3.1.1
		this->loadXMLLayoutInfo(n);
	}
}

void PathwayGraphModel::loadXMLLayoutInfo(XMLNode * layoutNode)
{
	XMLAttributes att = layoutNode->getAttributes();
	std::string name = att.getValue("name");
	if (name == "") name = "No Name";

	GraphLayout * graphLayout = new GraphLayout(this, name);

	int nc = layoutNode->getNumChildren();
	for (int i=0; i<nc; ++i)
	{
		XMLNode * n = (XMLNode*)(&layoutNode->getChild(i)); // force the conversion from const XMLNode * for compatibility with libsbml3.1.1
		this->loadXMLContentInfo(n, graphLayout, NULL);
	}

	// Connectors (standard procedure, the same as for the classic txt file format)
	std::list<BGL_Edge> eList = this->getEdges();
	for (std::list<BGL_Edge>::iterator it = eList.begin(); it != eList.end(); ++it)
	{
		Connector * c = graphLayout->connect(*it);
		if (!c) continue; // I can have a partial layout
	}
	
	this->layoutInformation.push_back(graphLayout);
}

void PathwayGraphModel::loadXMLContentInfo(XMLNode * contentNode, GraphLayout * graphLayout, ContainerContent * parent)
{
	XMLAttributes att = contentNode->getAttributes();
	std::string sbmlid = att.getValue("sbmlid"); 

	if (contentNode->getName() == "container")
	{
		// first we must create the container and place it inside of its parent (or fetch the root if there is no parent)
		ContainerContent* c;
		if (!parent) c = graphLayout->getRoot();
		else
		{
			if (sbmlid == "")
			{ // the normal case, a standard container
				c = new ContainerContent(graphLayout, parent);		
			}
			else
			{ // in case we are dealing with a compartment container
				c = new CompartmentContainer(graphLayout, parent, this->model()->getCompartment(sbmlid));
				this->compartmentToContainer[graphLayout][sbmlid] = c;			
			}
		}
		
		// then we must set its strategy properly
		std::string strategy = att.getValue("strategy");

		if (strategy == "NoStrategy") c->setContentLayoutStrategy(NoStrategy);
		if (strategy == "Automatic") c->setContentLayoutStrategy(Automatic);
		if (strategy == "Hierarchy") c->setContentLayoutStrategy(Hierarchy);
		if (strategy == "Clone") c->setContentLayoutStrategy(Clone);
		if (strategy == "Neighbourhood") c->setContentLayoutStrategy(Neighbourhood);
		if (strategy == "Branch") c->setContentLayoutStrategy(Branch);
		if (strategy == "Triangle") c->setContentLayoutStrategy(Triangle);

		// finally we must have a look at the children of the container (the parent parameter is the current container)
		int nc = contentNode->getNumChildren();
		for (int i=0; i<nc; ++i)
		{
			XMLNode * n = (XMLNode*)(&contentNode->getChild(i)); // force the conversion from const XMLNode * for compatibility with libsbml3.1.1
			this->loadXMLContentInfo(n, graphLayout, c);
		}
	}
	else // clone
	{
		CloneContent * clone = NULL;
		if (sbmlid != "") // normal sbml vertex
		{
			// we must create the clone with the relevant sbml id
			clone = new CloneContent( this->idToVertex[sbmlid], graphLayout );		
		}
		else // probably a source or sink
		{
			bool isSource;
			std::string role = att.getValue("role");
			if (role ==  "Source") isSource = true;
			else if (role ==  "Sink") isSource = false;
			else throw std::runtime_error("In PathwayGraphModel::loadXMLContent\nNo sbmlid but neither source or sink"); // not actually a source or sink? [!]

			std::string rId = att.getValue("sbmlref");
			BGL_Vertex r = this->idToVertex[ rId ];
			BGL_Vertex s;
			if (isSource)	s = this->getSource( this->getInEdges(r).front() );
			else			s = this->getTarget( this->getOutEdges(r).front() );
			clone = new CloneContent(s, graphLayout);
		}		
		
		// inside of the proper parent container, as a core if necessary
		parent->add(clone, att.getValue("iscore") != "");
		
		// its x and y coordinates
		std::string x = att.getValue("x"); 
		std::string y = att.getValue("y");
		if (x!="" && y!="") clone->setPosition( atoi(x.c_str()), atoi(y.c_str()) );

		// its neighbours => what is their sbml id?
		int nc = contentNode->getNumChildren();
		for (int i=0; i<nc; ++i)
		{
			XMLNode neighbour = contentNode->getChild(i);
			XMLAttributes a = neighbour.getAttributes();
			BGL_Vertex node = this->idToVertex[ a.getValue("sbmlid") ];
			BGL_Edge edge = this->findEdgeFromNeighbourRelationship( clone, node, a.getValue("relationship") );
			clone->addNeighbour(node, edge);
		}
	}
}

// given a clone, its neighbour node, and their relationship, finds the corresponding edge
BGL_Edge PathwayGraphModel::findEdgeFromNeighbourRelationship(CloneContent * clone, BGL_Vertex neighbour, std::string relationship)
{ // the relationship is that of a reaction (the neighbour) to a species (the clone)
// if the relationship is unspecified (legacy) we must try all possibilities
	BGL_Edge e;
	bool found = false;

	if ((relationship == "Reactant") || (relationship == "Modifier") || (relationship == ""))
	{
		std::list<BGL_Edge> eList = this->getInEdges(neighbour);
		for (std::list<BGL_Edge>::iterator it = eList.begin(); it != eList.end(); ++it)
		{
			BGL_Vertex v = this->getSource(*it);
			EdgeProperty * ep = this->getProperties(*it);
			if ( (relationship != "") && (relationship != ep->getTypeLabel())) continue;
			if (v == clone->getVertex()) { e = *it; found = true; break; }
		}
	}
	
	if (!found) if ((relationship == "Product") || (relationship == ""))
	{
		std::list<BGL_Edge> eList = this->getOutEdges(neighbour);
		for (std::list<BGL_Edge>::iterator it = eList.begin(); it != eList.end(); ++it)
		{
			BGL_Vertex v = this->getTarget(*it);
			EdgeProperty * ep = this->getProperties(*it);
			if ( (relationship != "") && (relationship != ep->getTypeLabel())) continue;
			if (v == clone->getVertex()) { e = *it; found = true; break; }
		}
	}
	
	if (!found) throw std::runtime_error("In PathwayGraphModel::findEdgeFromNeighbourRelationship\nCan't find edge");
	
	return e;
}


/* [!] old draft of loading layout data from the extension
	// using the extensions, but doesn't handle cloning
	
	if (this->model())
	{
		if (!this->model()->getListOfLayouts()->size()) return;

		// get the Layout
		Layout* layout = this->model()->getLayout(0);

		// get the SpeciesGlyphs
		int numSG = layout->getNumSpeciesGlyphs();
		for (unsigned int i=0; i<numSG; ++i)
		{
			SpeciesGlyph* sg = layout->getSpeciesGlyph(i);	
			BGL_Vertex v = this->idToVertex[sg->getSpeciesId()];
			VertexProperty *vp = this->vertexProperties[v];
			BoundingBox *bb = sg->getBoundingBox();
			vp->setX(bb->x());
			vp->setY(bb->y());
		}
		
		// get the ReactionGlyphs
		int numRG = layout->getNumReactionGlyphs();
		for (unsigned int i=0; i<numRG; ++i)
		{		
			ReactionGlyph* rg = layout->getReactionGlyph(i);
			BGL_Vertex v = this->idToVertex[rg->getReactionId()];
			VertexProperty *vp = this->vertexProperties[v];
			LineSegment* ls = rg->getCurve()->getCurveSegment(0);
			vp->setX(ls->getStart()->x());
			vp->setY(ls->getStart()->y());
		}
	}
*/
/*
	As far as I can tell, I/O with the layout data need to be done independently because there is no direct link between a Species/Reaction and its Glyph
	(I should probably save a link to the Glyph in my properties, not just the SBase object?? Although that would rather be a list of Glyphs
	since an SBML file can contain several layouts...?)
*/

/***************************************************************************************************
*
* Saving
*
***************************************************************************************************/

/*****************
* saveLayoutInfo *
******************
* First, for every layout in the model's list,
* we export a file in our custom layout format described in loadLayoutInfo
* [!] I should have a separate class for handling this import/export
* First we call the saveContainerInfo method
* then the saveLayoutInfo method on every species and reactions
*
* Then we export the layout information in SBML
* [!] no support of the rendering extension yet
*  We create a copy of the current document
* then for every layout in the list we create a new SBML layout
* For every species, we find the corresponding clones
* and create one species glyph per clone,
* set up at the clone's position with the clone's dimensions
* [!] warning! my pos is the center, but maybe it's supposed to be a corner?
* For every reaction, we create one reaction glyph
* (reactions are not supposed to be clonable)
* described by a set of line segments starting at the reaction's pos
* and ending at the pos of each clone of species involved in the reaction
* [!] that's pretty nice, but ignores the reaction's rectangle shape
* The resulting SBMLDocument is saved under a name
* inspired from the original document's file name
* [!] these exporting options are chosen arbitrarily, should be more customizable
* [!] also it's very messy to have these load/save I/O method in this class?
*********************************************************************************/
void PathwayGraphModel::saveLayoutInfo()
{
	if (!this->model()) return;

	//////////////////////////////////////////////////////////////
	// Saving my layout info as custom sbml annotations
	this->saveXMLLayoutInfo();

	//////////////////////////////////////////////////////////////
	// the SBML extension method
	this->saveExtensionLayoutInfo();
}

/***************************************************************************************************
* Save as annotations
***************************************************************************************************/

void PathwayGraphModel::saveXMLLayoutInfo()
{
	// removes my preexisting proprietary annotations
/*
	std::string annotationString = 	this->model()->getAnnotationString(); // get a copy of annotations
	XMLNode * annotationNode = XMLNode::convertStringToXMLNode(annotationString);
	this->model()->unsetAnnotation(); // remove annotations
	int nc = 0; if (annotationNode) nc = annotationNode->getNumChildren();
	for (int i=0; i<nc; ++i)
	{ // add back all annotations except for the arcadia ones
		XMLNode node = annotationNode->getChild(i);
		if (node.getPrefix() != "arcadia") this->model()->appendAnnotation(&node);
	}
*/
	XMLNode * annotationNode = this->model()->getAnnotation(); 
	int nc = 0; if (annotationNode) nc = annotationNode->getNumChildren();
	for (int i=0; i<nc; ++i)
	{ // remove the existing arcadia annotation node if it exists
		XMLNode node = annotationNode->getChild(i);
		if (node.getPrefix() == "arcadia") { annotationNode->removeChild(i); break; }
	}
	// set the edited node as the model annotation
	this->model()->setAnnotation(annotationNode);

	// add my proprieatary annotations
	std::string arcadiaString =	"<arcadia:listoflayouts xmlns:arcadia=\"http://www.arcadia.org/ns\">";

	unsigned int layoutNumber = this->layoutInformation.size();
	for(unsigned int l = 0; l < layoutNumber; ++l) arcadiaString += this->saveXMLLayoutInfo(l);

	if (layoutNumber)
	{
		arcadiaString += "</arcadia:listoflayouts>";
				
		XMLNode * arcadiaLOL = XMLNode::convertStringToXMLNode(arcadiaString);
		this->model()->appendAnnotation(arcadiaLOL);
	}
}

std::string PathwayGraphModel::saveXMLLayoutInfo(unsigned int l)
{
	GraphLayout * graphLayout = this->layoutInformation[l];
	std::string layoutString = "<arcadia:layout arcadia:name=\"" + graphLayout->name + "\">";
	layoutString += this->saveXMLContainerInfo(graphLayout->getRoot());	
	layoutString += "</arcadia:layout>";
	return layoutString;	
}

std::string PathwayGraphModel::saveXMLContainerInfo(ContainerContent * c)
{
	std::string containerString = "<arcadia:container arcadia:strategy=\"";
	
	std::string moreInfo = "";
	switch (c->getContentLayoutStrategy())
	{
	case NoStrategy:
		containerString += "NoStrategy";
		break;
	case Automatic:
		containerString += "Automatic";
		break;
	case Hierarchy:
		containerString += "Hierarchy";
		if (c->getTypeLabel() == "CompContainer")
		{
			moreInfo += "arcadia:sbmlid=\"" + c->getReference() + "\"";
		}
		break;
	case Clone:
		containerString += "Clone";
		break;
	case Neighbourhood:
		containerString += "Neighbourhood";
		break;
	case Branch:
		containerString += "Branch";
		break;
	case Triangle:
		containerString += "Triangle";
		break;
	}
	containerString += "\"";
	if (moreInfo != "") containerString += " " + moreInfo;
	containerString += ">";

	std::list<Content*> children = c->getChildren();
	for (std::list<Content*>::iterator it = children.begin(); it != children.end(); ++it)
	{
		if ((*it)->getId() == "") // it's not a container
		{
			containerString += this->saveXMLCloneInfo((CloneContent*)(*it));
		}
		else containerString += this->saveXMLContainerInfo((ContainerContent*)(*it));
	}

	containerString += "</arcadia:container>";
		
	return containerString;
}

std::string PathwayGraphModel::saveXMLCloneInfo(CloneContent * c)
{
	std::string cloneString = "<arcadia:clone";

	VertexProperty * vp = this->getProperties(c->getVertex());
	if (vp->getId() != "")
	{
		cloneString += " arcadia:sbmlid=\"" + vp->getId() + "\"";
	}
	else // there's no id...
	{
		if (vp->getTypeLabel() == "empty set") // it's a source or sink
		{
			cloneString += " arcadia:role=\"" + vp->getLabel() + "\"";
			BGL_Vertex reaction = this->getNeighbours( c->getVertex() ).front();
			cloneString += " arcadia:sbmlref=\"" + this->getProperties(reaction)->getId() + "\"";
		}
		else throw std::runtime_error("In PathwayGraphModel::saveXMLCloneInfo\nNo id but not empty set"); // if not, we have a problem [!]
	}
	
	std::ostringstream oFile;
	oFile << " arcadia:x=\"" << c->x() << "\"";
	oFile << " arcadia:y=\"" << c->y() << "\"";
	cloneString += oFile.str();

	if (c->getContainer()->getCore() == c) cloneString += " arcadia:iscore=\"iscore\"";

	cloneString += ">";

	// now for the neighbours
	std::list<BGL_Edge> eList = c->getNeighbourEdges();
	for (std::list<BGL_Edge>::iterator it = eList.begin(); it != eList.end(); ++it)
	{	
		BGL_Edge e = *it;
		BGL_Vertex neighbour = this->getSource(e);
		if (neighbour == c->getVertex()) neighbour = this->getTarget(e);

		cloneString += "<arcadia:neighbour";
		VertexProperty * nvp = this->vertexProperties[neighbour];
		if (nvp->getId() != "") cloneString += " arcadia:sbmlid=\"" + nvp->getId() + "\"";
		else // source or sink? (normally, shouldn't happen, as reactions can't be cloned)
			throw std::runtime_error("In PathwayGraphModel::saveXMLCloneInfo\nNeighbour should have sbml id (can't be source or sink as reactions can't be cloned)");

		cloneString += " relationship=\"" + this->getProperties(e)->getTypeLabel() + "\"/>";
	}

	cloneString += "</arcadia:clone>";
	return cloneString;
}

/***************************************************************************************************
* Save through SBML layout extension
***************************************************************************************************/

void PathwayGraphModel::saveExtensionLayoutInfo()
{
	Model *m = this->model();

	// delete existing arcadia layouts
	ListOfLayouts * listOfLayouts = m->getListOfLayouts();
	long numLayouts = listOfLayouts->size();
	for ( long i = numLayouts-1; i >= 0; --i )
	{
		Layout * l = m->getLayout(i);
		if (l->getId().substr(0,15) != "Arcadia_Layout_") continue;
		listOfLayouts->remove(i);
		delete l;
	}

	unsigned int layoutNumber = this->layoutInformation.size();
	for(unsigned int l = 0; l < layoutNumber; ++l)
	{
		this->saveExtensionLayoutInfo(m, this->layoutInformation[l], l);
	}
}


void PathwayGraphModel::saveExtensionLayoutInfo(Model * m, GraphLayout * graphLayout, unsigned int l)
{
	// layout creation with id
	Layout* layout = m->createLayout();
	std::ostringstream osstream; osstream << "Arcadia_Layout_" << l;
	layout->setId( osstream.str().c_str() );

	// dimensions
	Dimensions dim(graphLayout->getRoot()->width(true), graphLayout->getRoot()->height(true));
	layout->setDimensions(&dim);
	int X0 = graphLayout->getRoot()->left(true);
	int Y0 = graphLayout->getRoot()->top(true);

	// species glyph
	std::map<CloneContent *, std::string> cloneToGlyph;
	const unsigned int numSpecies = this->speciesNumber();
	for (unsigned int i=0; i<numSpecies; ++i)
	{
		int c = 0;	std::list<CloneContent *> clones = graphLayout->getClones( this->idToVertex[ this->getSpecies(i)->getId() ] );
		for (std::list<CloneContent *>::iterator it = clones.begin(); it != clones.end(); ++it)
			cloneToGlyph[*it] = this->saveExtensionSpeciesLayoutInfo(*it, layout, i, c++, X0, Y0);
	}

	// reactions glyph
	const unsigned int numReactions = this->reactionsNumber();
	for (unsigned int i=0; i<numReactions; ++i)
	{
		CloneContent * c = graphLayout->getClone( this->idToVertex[ this->getReaction(i)->getId() ] );
		this->saveExtensionReactionLayoutInfo(c, layout, i, cloneToGlyph, X0, Y0);
	}

	// compartment glyph
	const unsigned int numCompartment = this->compartmentsNumber();
	for (unsigned int i=0; i<numCompartment; ++i)
	{
		Compartment * c = this->getCompartment(i);
		ContainerContent * cont = this->compartmentToContainer[graphLayout][c->getId()];
	// [!] compartmentToContainer could lead to issues if I remove a layout or a compartment???
		if (!cont) continue;
		
		this->saveExtensionCompartmentLayoutInfo(cont, layout, i, c, X0, Y0);
	}

	// don't forget source or sink!
	int i = 0;
	std::list<BGL_Vertex> vList = this->getVertices();
	for (std::list<BGL_Vertex>::iterator it = vList.begin(); it != vList.end(); ++it)
	{
		BGL_Vertex v = *it;
		VertexProperty * vp = this->getProperties(v);

		if (vp->getTypeLabel() != "empty set") continue;

		CloneContent * clone = graphLayout->getClone(v);
		if (!clone) continue; // e.g neighbourhood layout

		// These ARE species, in a way, even if they're missing from the sbml model = no species id
		SpeciesGlyph* sg = layout->createSpeciesGlyph();
		std::ostringstream sgId; sgId << "EmptySetGlyph_" << i;
		sg->setId( sgId.str().c_str() );
		std::ostringstream bbId; bbId << "bbesg_" << i;
		BoundingBox bb( bbId.str().c_str(), clone->left(false)-X0, clone->top(false)-Y0, clone->width(false), clone->height(false));
		sg->setBoundingBox(&bb);

		TextGlyph * tg = layout->createTextGlyph();
		std::ostringstream tgId; tgId << "EmptySetLabel_" << i;
		tg->setId( tgId.str().c_str() );
		tg->setBoundingBox(&bb);
		tg->setText("(/)");
		tg->setGraphicalObjectId( sgId.str().c_str() );

/* // Additional graphical objects are not dealt with properly by any other program... no interoperability here
		// saving the graphical object
		GraphicalObject * go = layout->createAdditionalGraphicalObject();
		char goId[16]; sprintf(goId, "GO_%d", i);
		go->setId(goId);
		char bbId[16]; sprintf(bbId, "bbg_%d", i);
		BoundingBox bb(bbId, clone->left(false)-X0, clone->top(false)-Y0, clone->width(false), clone->height(false));
		go->setBoundingBox(&bb);
*/		
		++i; // counter of graphical objects
	}
}

std::string PathwayGraphModel::saveExtensionSpeciesLayoutInfo(CloneContent * clone, Layout * layout, int i, int c, int X0, int Y0)
{
	std::string sId = this->getProperties(clone->getVertex())->getId();

	SpeciesGlyph* sg = layout->createSpeciesGlyph();

	std::ostringstream sgId; sgId << "SG_" << i << "_" << c;
	sg->setId( sgId.str().c_str() );
	sg->setSpeciesId( sId.c_str() );

	std::ostringstream bbId; bbId << "bbs_" << i << "_" << c;
	BoundingBox bb( bbId.str().c_str(), clone->left(false)-X0, clone->top(false)-Y0, clone->width(false), clone->height(false));
	sg->setBoundingBox(&bb);
	
	TextGlyph * tg = layout->createTextGlyph();
	std::ostringstream tgId; tgId << "TGS_" << i << "_" << c;
	tg->setId( tgId.str().c_str() );
	tg->setBoundingBox(&bb);
	tg->setOriginOfTextId( sId.c_str() );
	tg->setGraphicalObjectId( sgId.str().c_str() );

	return sgId.str();
}

// [!] to manage these properly I need an easy way to access the limit between a vertexGraphics and the edgeGraphics pointing to/from it
void PathwayGraphModel::saveExtensionReactionLayoutInfo(CloneContent * clone, Layout * layout, int i, std::map<CloneContent *, std::string> cloneToGlyph, int X0, int Y0)
{		
	if (!clone) return;
	
	// Create the reaction glyph and links it to the corresponding SBML reaction
	ReactionGlyph* rg = layout->createReactionGlyph();
	std::ostringstream rgId; rgId << "RG_" << i;
	rg->setId( rgId.str().c_str() );
	rg->setReactionId( this->getProperties(clone->getVertex())->getId().c_str() );

	// Defines the glyph's bounding box	from the clone's dimension and position
	std::ostringstream bbId; bbId << "bbr_" << i;
	BoundingBox bb( bbId.str().c_str(), clone->left(false)-X0, clone->top(false)-Y0, clone->width(false), clone->height(false));
	rg->setBoundingBox(&bb);

	// Creates a curve for the reaction (overrides the bounding box definition!)
	Curve* rc = rg->getCurve();
	std::ostringstream rcId; rcId << "RC_" << i;
	rc->setId(rcId.str().c_str());	

	// SBW/Athena compatibility: reaction is a dot
	LineSegment* ls = rc->createLineSegment();
	Point p(clone->x() - X0, clone->y() - Y0);
	ls->setStart(&p);
	ls->setEnd(&p);

/*
	// Defines the reaction as a single line segment connecting the clone's in with its out
	LineSegment* ls = rc->createLineSegment();
	Point p1(clone->x(in) - X0, clone->y(in) - Y0);
	Point p2(clone->x(out) - X0, clone->y(out) - Y0);
	ls->setStart(&p1);
	ls->setEnd(&p2);
*/

	// For each connector to the reaction...
	int j = 0;
	std::list<Connector*> connectors = clone->getOutterConnectors();
	for (std::list<Connector *>::iterator it = connectors.begin(); it != connectors.end(); ++it)
	{
		// We find the species at the other end, and the properties of the connector's edge
		Connector * connect = *it;
		bool speciesIsSource = (connect->getSource() != clone);
		CloneContent * specClone = speciesIsSource? connect->getSource(): connect->getTarget();
		EdgeProperty * ep = this->getProperties( connect->getEdge() );

		// We create a species reference glyph, and link it to the glyph of the species
		SpeciesReferenceGlyph * specRef = rg->createSpeciesReferenceGlyph();
		std::ostringstream srId; srId << "SR_" << i << "_" << j++;
		specRef->setId( srId.str().c_str() );
		specRef->setSpeciesGlyphId( cloneToGlyph[specClone] );
		// [!] Edges don't normally have sbml id. It's a special layout annotation which I currently don't parse
//		specRef->setSpeciesReferenceId( ep->getId() ); // this is the preferred way of defining species reference glyphs, though...

		// We set the role of the glyph based on the edge type
		if (ep->getTypeLabel() == "Modifier") specRef->setRole(SPECIES_ROLE_MODIFIER);
		if (ep->getTypeLabel() == "Product") specRef->setRole(SPECIES_ROLE_PRODUCT);
		if (ep->getTypeLabel() == "Reactant") specRef->setRole(SPECIES_ROLE_SUBSTRATE);

		// We get the connector end coordinates: p1 for the reaction, p2 for the species
		std::pair<int, int> p1 = connect->getPoint(!speciesIsSource); // no intersection problem as connectors point at either in, out, side1 or side2 for reactions
		std::pair<int, int> p2 = connect->getPoint(speciesIsSource, true); // detects intersection to avoid overlap between edge and species
				
		// We create the corresponding points
		Point reacPoint(p1.first - X0, p1.second - Y0);
		Point specPoint(p2.first - X0, p2.second - Y0);

		// And the corresponding line segment
		LineSegment* ls = specRef->createLineSegment();
		
		// We make sure the edge goes in the right direction
		if (ep->getTypeLabel() == "Product") // from reaction to species for products
		{
			// SBW compatibility start
			int x3 = connect->getSource()->x();
			int y3 = connect->getSource()->y();
			Point firstPoint(x3 - X0, y3 - Y0);
			ls->setStart(&firstPoint);
			ls->setEnd(&reacPoint);		
			ls = specRef->createLineSegment();
			// SBW compatibility end

			ls->setStart(&reacPoint);
			ls->setEnd(&specPoint);
		}
		else if (ep->getTypeLabel() == "Reactant") // from species to reaction for reactant
		{
			ls->setStart(&specPoint);
			ls->setEnd(&reacPoint);		

			// SBW compatibility start
			ls = specRef->createLineSegment();
			ls->setStart(&reacPoint);
			int x3 = connect->getTarget()->x();
			int y3 = connect->getTarget()->y();
			Point lastPoint(x3 - X0, y3 - Y0);
			ls->setEnd(&lastPoint);		
			// SBW compatibility end
		}
		else if (ep->getTypeLabel() == "Modifier") // from species to reaction for modifier too
		{
			ls->setStart(&specPoint);
			ls->setEnd(&reacPoint);		

			// we need to add the end bit connecting to the center of the reaction square (since it's a flat line in the layout extension)
			ls = specRef->createLineSegment();
			ls->setStart(&reacPoint);
			int x3 = connect->getTarget()->x();
			int y3 = connect->getTarget()->y();
			// SBW compatibility start
			int margin = 10;
			if ((p1.first - x3)*(p1.first - x3) > (p1.second - y3)*(p1.second - y3)) // is the link horizontal or vertical
			{ // horizontal
				if (p1.first < x3) // from left to right
				{
					x3 -= margin;
				}
				else // from right to left
				{
					x3 += margin;
				}
			}
			else // vertical
			{
				if (p1.second < y3) // from top to bottom
				{
					y3 -= margin;
				}
				else // from bottom to top
				{
					y3 += margin;
				}
			}
			// SBW compatibility end
			Point lastPoint(x3 - X0, y3 - Y0);
			ls->setEnd(&lastPoint);
		}
		else throw std::runtime_error("PathwayGraphModel::saveExtensionReactionLayoutInfo()\nCan't recognize edge type");
	}
}

void PathwayGraphModel::saveExtensionCompartmentLayoutInfo(ContainerContent * cont, Layout * layout, int i, Compartment * c, int X0, int Y0)
{
	CompartmentGlyph * cg = layout->createCompartmentGlyph();
	std::ostringstream cgId; cgId << "CG_" << i;
	cg->setId( cgId.str().c_str() );
	cg->setCompartmentId(c->getId());
	std::ostringstream bbId; bbId << "bbc_" << i;
	BoundingBox bb( bbId.str().c_str(), cont->left(false)-X0, cont->top(false)-Y0, cont->width(false), cont->height(false));
	cg->setBoundingBox(&bb);
	
	// needs the label too
	std::string label = cont->getLabel();
	if (label == "") { return; }
	
	TextGlyph * tg = layout->createTextGlyph();
	std::ostringstream tgId; tgId << "TGC_" << i;
	tg->setId( tgId.str().c_str() );

	std::ostringstream bblId; bblId << "bbl_" << i;
	BoundingBox bbl( bblId.str().c_str(), cont->left(false)+12-X0, cont->top(false)+12-Y0, cont->labelWidth(), cont->labelHeight());
	tg->setBoundingBox(&bbl); // [!] I add the "usual" values of the compartment border and padding
	
	tg->setOriginOfTextId( c->getId() );
	tg->setGraphicalObjectId( cgId.str().c_str() );
}

////////////////////////////////////////////////////////////////////////////////////////////////////

// [!] the method for fusing similar reactions
void PathwayGraphModel::toggleFusing(std::list<BGL_Vertex> vList)
{
	ListOfReactions * reactions = this->model()->getListOfReactions();
	
	// Flux is a string uniquely identifying a reaction's list of reactants and products
	// It's a concatenation of the reactants and products id separated by \t
	// the reactant list comes first, then \n, then the product list
	// the reactant and product lists are alphebetically ordered
	std::map<std::string, std::list<std::string> > fluxToReactions;
	// reactions sharing the same flux must be fused

	// we go through all reactions to populate the fluxToReactions map
	for (int i=0; i<reactions->size(); ++i)
	{
		Reaction * r = (Reaction*) reactions->get(i);
		std::string flux;
		std::list<std::string> sList;
		ListOfSpeciesReferences * specRefs = r->getListOfReactants();

		for (int j=0; j<specRefs->size(); ++j)
		{
			std::string s = ((SpeciesReference*)specRefs->get(j))->getSpecies();
			sList.push_back(s);
		}
		sList.sort();
		for (std::list<std::string>::iterator it = sList.begin(); it != sList.end(); ++it)
		{
			flux += (*it) + "\t";
		}
		
		flux += "\n";
		sList.clear();
		specRefs = r->getListOfProducts();
		
		for (int j=0; j<specRefs->size(); ++j)
		{
			std::string s = ((SpeciesReference*)specRefs->get(j))->getSpecies();
			sList.push_back(s);
		}
		sList.sort();
		for (std::list<std::string>::iterator it = sList.begin(); it != sList.end(); ++it)
		{
			flux += (*it) + "\t";
		}
		
		fluxToReactions[flux].push_back(r->getId());
	}

	// we then go through the map, and fuse reactions that need be (>1)
	for (std::map<std::string, std::list<std::string> >::iterator it = fluxToReactions.begin();
		it != fluxToReactions.end(); ++it)
	{
		std::list<std::string> rList = (*it).second;
		if (rList.size() < 2) continue;
		else
		{
			Reaction * first = NULL;
			for (std::list<std::string>::iterator ir = rList.begin(); ir != rList.end(); ++ir)
			{
				Reaction * r =  (Reaction*)reactions->get(*ir);

/*
				if (!first) first = r;
				else
				{
					ListOfSpeciesReferences * mods = r->getListOfModifiers();
					for (int i=0; i<mods->size(); ++i)
					{
						first->addModifier((ModifierSpeciesReference*)mods->get(i));
					}
					reactions->remove(*ir);
				}
*/
			}
		}
	}
}