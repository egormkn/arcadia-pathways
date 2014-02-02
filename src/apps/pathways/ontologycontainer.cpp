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
 *  OntologyContainer.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 24/09/2008.
 *  Last documented never, really
 *
 */

#include <iostream>
#include <stdexcept>
#include <QtXml>
#include <QDomDocument>

#include "ontologycontainer.h"

int OntologyContainer::inherits(std::string child, std::string parent)
{
	if (child == "") return -1;
	if (child == parent) return 0;	
	
	int distance = this->inherits(this->idToParent[child], parent);
	if (distance == -1) return -1;
	else return distance + 1;
}

OntologyContainer * OntologyContainer::MyLocalSBO = NULL;

void OntologyContainer::LoadLocalSBO(std::string dirName, bool fullPath)
{
	if (!fullPath)
	{
		if (dirName == "") dirName = ".";
		std::string path = "\\res\\SBO_XML.xml"; // windows, *nix
		#ifdef MAC_COMPILATION
		path = "/../Resources/SBO_XML.xml";
	//	path = "/arcadia.app/Contents/Resources/SBO_XML.xml";
		#endif
		dirName += path;
	}

	OntologyContainer::MyLocalSBO = new OntologyContainer(dirName);
}

OntologyContainer * OntologyContainer::GetMyLocalSBO()
{
	if (!OntologyContainer::MyLocalSBO) OntologyContainer::LoadLocalSBO();
	return OntologyContainer::MyLocalSBO;
}

std::string OntologyContainer::GetTextFromUniqueTag(QDomElement * e, std::string tag)
{
	QDomNodeList nodeList = e->elementsByTagName (tag.c_str());
	if (nodeList.count() != 1) return "";
	QDomElement element = nodeList.at(0).toElement();
	if (element.isNull()) return "";
	return element.text().toStdString();
}

OntologyContainer::OntologyContainer(std::string fileName) // at the moment, only support the XML format used for SBO
{
	if (fileName == "") return; // dummy

	// loading the file into an XML document
	QDomDocument doc("mydocument");
	QFile file(fileName.c_str());
	bool failure = false;

	if (!file.open(QIODevice::ReadOnly)) failure = true;
	else
	{
		if (!doc.setContent(&file)) failure = true;
		file.close();
	}
	
	if (failure)
	{
		throw std::runtime_error("OntologyContainer::OntologyContainer\nCould not load ontology from "+fileName);
		return;
	}	

	// parsing the XML document and loading into my ontology structure
	QDomElement docElem = doc.documentElement();

	// we deal with all the sibling elements which are children of the root
	for (QDomNode n = docElem.firstChild(); !n.isNull(); n = n.nextSibling())
	{
		QDomElement e = n.toElement();
		if(e.isNull() || e.tagName() != "Term") continue;
		
		// getting the id
		std::string id = OntologyContainer::GetTextFromUniqueTag(&e, "id");
		// getting the name
		std::string name = OntologyContainer::GetTextFromUniqueTag(&e, "name");
		
		this->idToName[id] = name;

		std::string is_a = OntologyContainer::GetTextFromUniqueTag(&e, "is_a");
		this->idToParent[id] = is_a;

		if (is_a == "")
		{
			std::string part_of = OntologyContainer::GetTextFromUniqueTag(&e, "part_of");
			this->idToParent[id] = part_of;
		}

/*		
		// getting the is_a relationship(s)
		nodeList = e.elementsByTagName ("is_a");
		for (int i=0; i<nodeList.count(); ++i)
		{
			element = nodeList.at(i).toElement();
			if (element.isNull()) continue;
			std::cout << element.tagName().toStdString() << " : " << element.text().toStdString() << std::endl;				
		}
		
		// getting the part_of relationship
		nodeList = e.elementsByTagName ("relationship");
		for (int i=0; i<nodeList.count(); ++i)
		{
			element = nodeList.at(i).toElement();
			if (element.isNull()) continue;

			QDomNodeList nl1 = element.elementsByTagName("type");
			QDomNodeList nl2 = element.elementsByTagName("to");
			if (nl1.isEmpty() || nl2.isEmpty()) continue;
			
			QDomElement el1 = nl1.at(0).toElement();
			QDomElement el2 = nl2.at(0).toElement();
			if (el1.isNull() || el2.isNull()) continue;
			
			std::cout << el1.text().toStdString() << " : " << el2.text().toStdString() << std::endl;							
		}		
*/
	}
	
	return;
}

std::string OntologyContainer::getName(std::string sboId)
{
	return this->idToName[sboId];
}