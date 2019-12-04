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
 *  ProductEdgeStyle.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 21/01/2008.
 *  Last documented on 17/04/2008.
 *
 */

#include "productedgestyle.h"

/******************************
* DefaultStyle: the Singleton *
******************************/
ProductEdgeStyle* ProductEdgeStyle::DefaultStyle = 0;

/*******************************
* GetDefaultStyle: the Factory *
*******************************/
ProductEdgeStyle* ProductEdgeStyle::GetDefaultStyle() {
	if (!ProductEdgeStyle::DefaultStyle) ProductEdgeStyle::DefaultStyle = new ProductEdgeStyle();
	return ProductEdgeStyle::DefaultStyle;
}

/*************************************
* ProductEdgeStyle: blue curvy edge  *
*************************************/
ProductEdgeStyle::ProductEdgeStyle() : EdgeStyle(128, 64, 64, true, noDeco, triangleArrow) { this->penWidth = 2; }