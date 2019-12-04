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
 *  GraphLoader.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#include "graphloader.h"

#include <iostream>

// STL functions for analysing the file extension
#include <cctype>
#include <algorithm>

// Local loaders
#include "defaultgraphloader.h"
#include "graphvizgraphloader.h"

GraphModel * GraphLoader::GetModel(std::string fName)
{
	GraphLoader * graphLoader = NULL;

	std::string ext = GraphLoader::GetFileExtension(fName);
	if (ext == "") graphLoader = new DefaultGraphLoader();
	if (ext == "dot") graphLoader = new GraphvizGraphLoader(fName);
	
	return GraphLoader::GetModel(graphLoader);
}

/*****************
* SaveGraphModel *
******************
* Depending on the file type
* creates the appropriate loader
* and saves the model with it
* [!] a LOADER that SAVES?
********************************/
bool GraphLoader::Save(GraphModel *m, std::string fName)
{
	GraphLoader * graphLoader = NULL;

	std::string ext = GraphLoader::GetFileExtension(fName);	

	if (ext == "txt") graphLoader = new DefaultGraphLoader();
	if (ext == "dot") graphLoader = new GraphvizGraphLoader();

	if (graphLoader)
	{
		graphLoader->save(m, fName);
		delete graphLoader;
		return true;
	}
	
	return false;
}

std::string GraphLoader::GetFileExtension(std::string fName)
{
	std::transform(fName.begin(), fName.end(), fName.begin(), tolower);
	if (fName == "") return "";
	std::string::size_type point = fName.find_last_of('.');
	return fName.substr(++point);
}

std::string GraphLoader::GetFileNameOnly(std::string fName)
{
	if (fName == "") return "";
	std::string::size_type point = fName.find_last_of('.');
	return fName.substr(0, point);
}

GraphModel * GraphLoader::GetModel(GraphLoader * graphLoader)
{
	GraphModel * graphModel = NULL;
	if (graphLoader)
	{
		graphModel = graphLoader->getModel();
		delete graphLoader;
	}
	return graphModel;
}