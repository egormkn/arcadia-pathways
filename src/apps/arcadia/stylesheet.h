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
 *  StyleSheet.h
 *  arcadia
 *
 *  Created by Alice Villeger on 04/06/2008.
 *	Last Documented never.
 *
 */

#ifndef STYLESHEET_H
#define STYLESHEET_H

#include <string>

class VertexProperty;
class VertexStyle;
class EdgeProperty;
class EdgeStyle;
class ContainerStyle;

// [!] dirty hack...
typedef enum CloneProperty { notClone, isClone, isMidget, isRotated, isInverted };

/*******************
* StyleSheet *
********************
* Links properties to appropriate styles
****************************************/
class StyleSheet
{
public:
	virtual VertexStyle * getVertexStyle(VertexProperty * vp, CloneProperty cp);
	virtual EdgeStyle * getEdgeStyle(EdgeProperty *ep);
	virtual ContainerStyle * getContainerStyle(std::string type);
	
	void toggleContainerVisibility();
};

#endif