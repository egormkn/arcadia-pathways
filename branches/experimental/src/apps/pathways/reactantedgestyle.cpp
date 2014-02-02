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
 *  ReactantEdgeStyle.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 21/01/2008.
 *  Last documented on 17/04/2008.
 *
 */

#include "reactantedgestyle.h"

/******************************
* DefaultStyle: the Singleton *
******************************/
ReactantEdgeStyle* ReactantEdgeStyle::DefaultStyle = 0;

/*******************************
* GetDefaultStyle: the Factory *
*******************************/
ReactantEdgeStyle* ReactantEdgeStyle::GetDefaultStyle() {
	if (!ReactantEdgeStyle::DefaultStyle) ReactantEdgeStyle::DefaultStyle = new ReactantEdgeStyle();
	return ReactantEdgeStyle::DefaultStyle;
}

/*************************************
* ReactantEdgeStyle: red curvy edge  *
*************************************/
ReactantEdgeStyle::ReactantEdgeStyle() : EdgeStyle(64, 64, 128, true, noDeco, noDeco) { this->penWidth = 2; }






/******************************
* DefaultStyle: the Singleton *
******************************/
ReversibleReactantEdgeStyle* ReversibleReactantEdgeStyle::DefaultStyle = 0;

/*******************************
* GetDefaultStyle: the Factory *
*******************************/
ReversibleReactantEdgeStyle* ReversibleReactantEdgeStyle::GetDefaultStyle() {
	if (!ReversibleReactantEdgeStyle::DefaultStyle) ReversibleReactantEdgeStyle::DefaultStyle = new ReversibleReactantEdgeStyle();
	return ReversibleReactantEdgeStyle::DefaultStyle;
}

/*************************************
* ReactantEdgeStyle: red curvy edge  *
*************************************/
ReversibleReactantEdgeStyle::ReversibleReactantEdgeStyle() : EdgeStyle(64, 64, 128, true, triangleArrow, noDeco) { this->penWidth = 2; }