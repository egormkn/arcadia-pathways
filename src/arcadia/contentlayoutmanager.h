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
 *  ContentLayoutManager.h
 *  arcadia
 *
 *  Created by Alice Villeger on 04/03/2008.
 *  Last documented on 17/04/2008.
 *
 */

#ifndef CONTENTLAYOUTMANAGER_H
#define CONTENTLAYOUTMANAGER_H

// local, to be laid out
class ContainerContent;

/************************
* ContentLayoutStrategy *
*************************
* List of all the possible Layout Strategies (3 ATM, but should grow)
* [!] to store independently, somewhere else? (make extensible??)
*********************************************************************/
enum ContentLayoutStrategy { NoStrategy, Automatic, Hierarchy, Clone, Neighbourhood, Branch, Triangle };

/***********************
* ContentLayoutManager *
************************
* Provides a static method (factory) for creating layout managers
* [!] Should make constructors protected or something?
* Also defines a static DefaultLayoutStrategy
*
* As an instance, this class sets up a few parameters:
* the rotation, set to false by default
* Also, the strategy, accessible through the getType method
*
* The main interface, that is the method to layout a Container, does NOTHING
* (see sub-classes for different implementations)
****************************************************************************/
class ContentLayoutManager {
private:
	static ContentLayoutStrategy DefaultStrategy;
public:
	static ContentLayoutManager * GetLayoutManager(ContentLayoutStrategy s = ContentLayoutManager::DefaultStrategy);

	ContentLayoutManager();
	virtual ~ContentLayoutManager() { }
	
	ContentLayoutStrategy getType() const;
	virtual void layout(ContainerContent * container) { }
	
	void setRotation(bool r) { this->rotation = r; }
	bool getRotation() { return this->rotation; }


protected:
	bool rotation;
	ContentLayoutStrategy strategy;
};

#endif