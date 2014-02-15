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
 *  VertexStyle.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 04/06/2008.
 *	Last Documented never
 *
 */

#include "vertexstyle.h"

#include <math.h>

#include <iostream>

VertexStyle * VertexStyle::DefaultStyle = NULL;

VertexStyle* VertexStyle::GetDefaultStyle()
{
	if (!VertexStyle::DefaultStyle) VertexStyle::DefaultStyle = new VertexStyle();
	return VertexStyle::DefaultStyle;
}

VertexStyle::VertexStyle(int r, int g, int bl, Shape s, float p, float b, float m, bool CSSmodeEnabled)
: rColor(r), gColor(g), bColor(bl), shape(s), padding(p), border(b), margin(m), CSSMode(CSSmodeEnabled), showLabel(true), isMidget(false), fillInside(foreground) { }

float VertexStyle::getFullWidth(const std::string text)
{
	float w = this->showLabel? text.size()*7.5 : 0; // assumed to be font width	
	if (this->getShape() == squareIOHori) w += 10;
	return w + this->getFullOffset();
}

float VertexStyle::getFullHeight(const std::string text)
{
	if (this->shape == circular || this->shape == underlinedCircular)
	{
		return this->getFullWidth(text);
	}

	float h = this->showLabel? 20 : 0; // assumed to be font height
	if (this->getShape() == squareIOVert) h += 10;
	return h + this->getFullOffset();
}

float VertexStyle::getBoundingWidth(const std::string text)
{
	float w = this->showLabel? text.size()*7.5 : 0; // assumed to be font width	
	if (this->getShape() == squareIOHori) w += 10;
	return w + this->getBoundingOffset();
}

float VertexStyle::getBoundingHeight(const std::string text)
{
	if (this->shape == circular || this->shape == underlinedCircular)
	{
		return this->getBoundingWidth(text);
	}

	float h = this->showLabel? 20 : 0; // assumed to be font height
	if (this->getShape() == squareIOVert) h += 10;
	return h + this->getBoundingOffset();
}