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
 *  EdgeStyle.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 05/06/2008.
 *	Last Documented never.
 *
 */

#include "edgestyle.h"

/************************************
* DefaultStyle: the local Singleton *
************************************/
EdgeStyle* EdgeStyle::DefaultStyle = 0;

/******************
* GetDefaultStyle *
*******************
* The local Factory
*******************/
EdgeStyle* EdgeStyle::GetDefaultStyle() {
	if (!EdgeStyle::DefaultStyle) EdgeStyle::DefaultStyle = new EdgeStyle();
	return EdgeStyle::DefaultStyle;
}

/**************************************************************************************
* Constructor: Sets up the base color and curviness parameter, + an arbitray penWidth *
* [!] the penWidth is arbitrary                                                       *
**************************************************************************************/
EdgeStyle::EdgeStyle(int r, int g, int b, bool curve, EdgeDecoration sDeco, EdgeDecoration tDeco) : rColor(r), gColor(g), bColor(b), isCurvy(curve), sourceDecoration(sDeco), targetDecoration(tDeco), penWidth(1) { }