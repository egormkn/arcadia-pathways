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
 *  SBMLGraphLoader.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#include "sbmlgraphloader.h"

#include <fstream>

// LibSBML
#include <sbml/SBMLTypes.h>

// local
#include "pathwaygraphmodel.h"

/*********************************************************************//**
* Constructor: just initializes the doc to NULL and loads the given file *
*************************************************************************/
SBMLGraphLoader::SBMLGraphLoader(std::string fName) : document(NULL) { this->load(fName); }

/*********************************//**
* Destructor: calls the clear method *
*************************************/
SBMLGraphLoader::~SBMLGraphLoader() { this->clear(); }

/**********************************//**
* clear: deletes the current document *
**************************************/
void  SBMLGraphLoader::clear() { if (this->document) delete this->document; }

/***//**
* load *
********
* sets up the fileName,
* clears the current doc
* reads the doc from the file
* filters out reading errors
*****************************/
void SBMLGraphLoader::load(std::string fName)
{
	this->filename = fName;
	
	this->clear();

	#ifdef MAC_COMPILATION
	this->document = readSBML(this->filename.c_str());
	#endif

	#ifndef MAC_COMPILATION // for windows... and Unix? (just because I was too lazy to define a Windows only case)
	std::string xmlString;

	std::ifstream myFile(this->filename.c_str());
	if (!myFile.is_open())
	{
		std::cerr << "Could not open file" << std::endl;
		throw std::exception();
	}
	
	while (! myFile.eof())
	{
		int c = myFile.get();
		if ( c != 0x0D ) xmlString += (char)c;
	}

	myFile.close();
	
	this->document = readSBMLFromString(xmlString.c_str());	
	#endif

	if (this->document->getNumErrors() > 0)
	{
		std::cerr << "LibSBML encountered the following SBML errors:" << std::endl;
		this->document->printErrors(std::cerr);
		
		std::ostringstream stream;
		stream << "LibSBML encountered the following SBML errors:" << std::endl;
		stream << std::endl;
		stream << "----------------------------------------------" << std::endl;
		stream << std::endl;
		this->document->printErrors(stream);
		stream << "----------------------------------------------" << std::endl;

		SBMLErrorLog * errors = document->getErrorLog();
		for (int i = 0; i < errors->getNumErrors(); ++i)
		{
			if (errors->getError(i)->getErrorId() == InvalidIdSyntax)
			{
				stream << "\nSBML identifiers can only contain letters, digits and/or underscore symbols. ";
				stream << "They must start with either a letter or an underscore symbol." << std::endl;				
			}
		}

		throw std::runtime_error(stream.str().c_str());
	}
}

/*******//**
* getModel *
************
* If there is an SBMLDocument,
* returns a PathwayGraphModel created from it
* (with the current fileName too)
* and sets up the current document to NULL
* (this basically delegates the deletion
* of the document to the PathwayGraphModel)
******************************************/
GraphModel * SBMLGraphLoader::getModel()
{
	GraphModel * graphModel = NULL;
	
	if (this->document)
	{
		graphModel = new PathwayGraphModel(this->document, this->filename);

		this->document = NULL; // delegates the deletion of the document to the model
	}
	
	return graphModel;
}

GraphModel * SBMLGraphLoader::GetModel(std::string fName)
{
	return GraphLoader::GetModel(new SBMLGraphLoader(fName));
}

bool SBMLGraphLoader::Save(PathwayGraphModel *m, std::string fName)
{
	GraphLoader * graphLoader = NULL;

	std::string ext = GraphLoader::GetFileExtension(fName);	

	if ( (ext == "sbml") || (ext == "xml") ) return true;
	else { GraphLoader::Save(m, fName); return false; }
}
