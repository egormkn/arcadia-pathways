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
 *  TriangleContainerStyle.h
 *  arcadia
 *
 *  Created by Alice Villeger on 04/06/2008.
 *	Last Documented never, rly.
 *
 */

#ifndef TRIANGLECONTAINERSTYLE_H
#define TRIANGLECONTAINERSTYLE_H

#include "containerstyle.h"

/***********************
* TriangleContainerStyle *
************************/
class TriangleContainerStyle: public ContainerStyle
{
public:
	static TriangleContainerStyle * GetDefaultStyle();
	TriangleContainerStyle();
private:
	static TriangleContainerStyle * DefaultStyle;
};

#endif