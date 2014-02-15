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
 *  PathwayVertexProperty.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#include "pathwayvertexproperty.h"
#include "ontologycontainer.h"
// #include "webservicehandler.h"

#include <sstream>

PathwayVertexProperty::PathwayVertexProperty(SBase * b, PathwayGraphModel * m)
	: base(b), model(m) { }

std::string PathwayVertexProperty::getLabel()
{
	std::string l = this->base->getName();
	if (l == "") l = VertexProperty::getLabel();
	return l;
}

std::string PathwayVertexProperty::getId() { return this->base->getId(); }

int PathwayVertexProperty::getSBOTerm() { return this->base->getSBOTerm(); }

std::string PathwayVertexProperty::getSBOTermId()
{
	if (this->base->getSBOTerm() > -1) return this->base->getSBOTermID();
	else return "";
}

int PathwayVertexProperty::inherits(int sboTerm)
{
	int distance = -1;
	
	OntologyContainer * sbo = OntologyContainer::GetMyLocalSBO();

	std::ostringstream o;
	if (sboTerm<1000) o << "SBO:0000";
	if (sboTerm<100) o << "0";
	if (sboTerm<10) o << "0";
	o << sboTerm;
	
	if (sbo) distance = sbo->inherits(this->getSBOTermId(), o.str());

	return distance;
}

std::list< std::string > PathwayVertexProperty::getIsURIs()
{
	return this->getBioURIs(BQB_IS, true);
}

// Parsing all MIRIAM annotations
std::list< std::pair< BiolQualifierType_t, std::string > > PathwayVertexProperty::getBioURIs()
{
	std::list< std::pair< BiolQualifierType_t, std::string > > bList;

	XMLNode* annotation = this->base->getAnnotation();
	if (!annotation) return bList;

	List *listCVTerms = new List();
	RDFAnnotationParser::parseRDFAnnotation (annotation, listCVTerms);

	unsigned int listSize = listCVTerms->getSize();
	for (unsigned int i=0; i<listSize; ++i)
	{
		CVTerm *t = (CVTerm*) listCVTerms->get(i);
		
		QualifierType_t qType = t->getQualifierType();
		if (qType != BIOLOGICAL_QUALIFIER) continue;
		
		BiolQualifierType_t bType = t->getBiologicalQualifierType();

		XMLAttributes *attributes = t->getResources();
		if (!attributes) continue;

		unsigned int nAtt = attributes->getLength();				
		for (unsigned int j=0; j<nAtt; ++j)
		{
			if (attributes->getName(j) == "rdf:resource")
			{
				std::pair<BiolQualifierType_t, std::string> p;
				p.first = bType;
				p.second = attributes->getValue(j);		
				bList.push_back(p);
			}
		}
	}
	
	delete listCVTerms;
	
	return bList;
}

// Parsing specific MIRIAM annotations
std::list< std::string > PathwayVertexProperty::getBioURIs(BiolQualifierType_t bt, bool includeURLs)
{
	std::list< std::string > bList;

	XMLNode* annotation = this->base->getAnnotation();
	if (!annotation) return bList;
	
	/* // no necessary at all, as I'm only dealing with resources, which can't be metaids
	// parsing the annotation for metaids
	std::map<std::string, XMLNode> metaidToNode;
	for (int i = 0; i < annotation->getNumChildren(); ++i)
	{
		XMLNode c = annotation->getChild(i);
		XMLAttributes att = c.getAttributes();
		std::string metaid = att.getValue("metaid");
		if (metaid == "") continue; // no metaid attribute in the annotation node
		metaidToNode["#" + metaid] = c; // storing the metaid to node information
	}
	*/

	// Parsing the RDF data as controlled vocabulary terms
	List listCVTerms;
	RDFAnnotationParser::parseRDFAnnotation (annotation, &listCVTerms);
	
	// For each controlled vocabulary term
	unsigned int listSize = listCVTerms.getSize();
	for (unsigned int i=0; i<listSize; ++i)
	{
		CVTerm *t = (CVTerm*) listCVTerms.get(i);
		
		// We are only interested in biological qualifiers
		QualifierType_t qType = t->getQualifierType();
		if (qType != BIOLOGICAL_QUALIFIER) continue;
		
		// We are only interested in the requested bio qualifier
		BiolQualifierType_t bType = t->getBiologicalQualifierType();
		if ( bType != bt ) continue;

		// We need some attributes
		XMLAttributes *attributes = t->getResources();
		if (!attributes) continue;

		// Among these attribute
		unsigned int nAtt = attributes->getLength();				
		for (unsigned int j=0; j<nAtt; ++j)
		{
			// we are only interested in resources
			if (attributes->getName(j) != "rdf:resource") continue;

			// We look at its value
			std::string value = attributes->getValue(j);
				
			bList.push_back(value);
	
			/* // don't know where that bullshit comes from???
			// Maybe some old file type, e.g. jamboree?
			// but inconsistent with recommendations for annotations in SBML2...

			// Resources that don't start with a # are directly returned
			if (value[0] != '#') { bList.push_back(value); continue; }

			// metaid, should be in the same annotation bag, parsed above
			if (metaidToNode.find(value) == metaidToNode.end()) { bList.push_back(value); continue; } // could not find

			XMLNode c = metaidToNode[value];
			
			if ((c.getName() != "inchi") || (c.getPrefix() != "in") || (c.getURI() != "http://biomodels.net/inchi"))
				{ bList.push_back(value); continue; }
		
			// special case: inchi
			if (c.getNumChildren() != 1) { bList.push_back(value); continue; }
			
			XMLNode text = c.getChild(0);
			if (!text.isText()) { bList.push_back(value); continue; }

			value = text.getCharacters();	
			bList.push_back(value);
			*/
		}
	}
			
	return bList;
}

// web service bit, doesn't quite fit yet (asynch)
/*
			// Query MIRIAM Web service to get URL from URI (handler created on the fly)
			MiriamWebServiceHandler * ws = new MiriamWebServiceHandler()
			this->activeHandlers.push_back(ws);
			ws->getLocations(value);
			// Add URLs to returned value
*/
/*	
	~PathwayVertexProperty() // gets rid of any remaining handler
	{
		for (std::list<MiriamWebServiceHandler *>::iterator it = this->activeHandlers.begin();
			it != this->activeHandlers.end(); ++it) delete *it;
	}
	std::list<MiriamWebServiceHandler *> activeHandlers;
*/

std::map<std::string, std::string> PathwayVertexProperty::getInfoMap()
{
	std::map<std::string, std::string> infoMap = VertexProperty::getInfoMap();
	if (this->getSBOTerm()>0)
	{
		infoMap["SBO"] = this->getSBOTermId();
	}

	int i = 1;	
	std::list<std::string> isList = this->getIsURIs();
	if (isList.size() == 1) infoMap["is"] = isList.front();
	else
	for (std::list<std::string>::iterator it = isList.begin(); it != isList.end(); ++it)
	{
		char name[8]; sprintf(name, "is [%d]", i++);
		infoMap[name] = (*it);
	}
		
	return infoMap;
}