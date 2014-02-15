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
 *  GraphLoader.h
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *	Last Documented on 17/04/2008.
 *
 */

#ifndef GRAPHLOADER_H
#define GRAPHLOADER_H

// STL
#include <string>

// local
class GraphModel;

/**************
* GraphLoader *
***************
* Fetches a graphModel from a given fileName (GetGraphModel)
* by identifying its type (getFileType)
* and calling the appropriate specific loader
*
* Reciprocally, saves a model in a given type (SaveGraphModel)
* by finding the proper extension (GetFileExt)
* and calling the appropriate specific loader
* [!] so loaders also save... isn't that a bit messy? or badly named
*
* Also defines the generic interface of a GraphLoader;
* getMode for loading, and save
* [!] that's a bit messy again? Should a factory do that?
********************************************************************/
class GraphLoader
{
public:
	virtual ~GraphLoader() {}

	static GraphModel * GetModel(std::string fName);
	static bool Save(GraphModel *m, std::string fName);

private:
	virtual GraphModel * getModel() = 0;
	virtual void save( GraphModel *m, std::string fName ) {}
	
protected:
	static GraphModel * GetModel(GraphLoader * gl);

public: // [!] for the graph scene export...
	static std::string GetFileExtension(std::string fName);	
	static std::string GetFileNameOnly(std::string fName);	
};

#endif