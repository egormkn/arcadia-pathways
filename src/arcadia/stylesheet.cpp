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
 *  StyleSheet.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 04/06/2008.
 *	Last Documented never.
 *
 */

#include "stylesheet.h"

#include "vertexstyle.h"
#include "edgestyle.h"
#include "clonecontainerstyle.h"
#include "branchcontainerstyle.h"
#include "trianglecontainerstyle.h"

VertexStyle * StyleSheet::getVertexStyle(VertexProperty * vp, CloneProperty cp) { return VertexStyle::GetDefaultStyle(); }

EdgeStyle * StyleSheet::getEdgeStyle(EdgeProperty * ep) { return EdgeStyle::GetDefaultStyle(); }

ContainerStyle * StyleSheet::getContainerStyle(std::string type)
{
	ContainerStyle *s = NULL;

	if (type == "CloneContainer")			s = CloneContainerStyle::GetDefaultStyle();
	if (type == "BranchContainer")			s = BranchContainerStyle::GetDefaultStyle();
	if (type == "TriangleContainer")		s = TriangleContainerStyle::GetDefaultStyle();

	if (type == "GenericContainer" || !s)	s = ContainerStyle::GetDefaultStyle();

	return s;
}

void StyleSheet::toggleContainerVisibility()
{
	CloneContainerStyle::GetDefaultStyle()->toggleVisibility();
	BranchContainerStyle::GetDefaultStyle()->toggleVisibility();
	TriangleContainerStyle::GetDefaultStyle()->toggleVisibility();
}