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
 *  ContainerStyle.h
 *  arcadia
 *
 *  Created by Alice Villeger on 04/06/2008.
 *	Last Documented never, rly.
 *
 */

#ifndef CONTAINERSTYLE_H
#define CONTAINERSTYLE_H

// for ContainerType
#include "containercontent.h"

enum ContainerShape { concaveHullContainer, rectangleContainer };

/*****************
* ContainerStyle *
******************/
class ContainerStyle
{
public:
	ContainerStyle();
	float getFullOffset() { return 2*this->padding + 2*this->border + 2*this->margin; }
	float getBoundingOffset() { return 2*this->padding + 2*this->border; }
	float getPaintingOffset() { return 2*this->padding + this->border; }

	bool isVisible() { return this->visible; }
	float getBorder() { return this->border; }
	bool getContinuousLine() { return this->continuousLine; }
	int red() { return this->rColor; }
	int green() { return this->gColor; }
	int blue() { return this->bColor; }
	
	void toggleVisibility() { this->visible = !this->visible; }

	ContainerShape shape;

protected:
	int rColor;
	int gColor;
	int bColor;

	bool visible;
	
	bool continuousLine;
	
	float border;
	
private:
	float padding;
	float margin;

public:
	static ContainerStyle * GetDefaultStyle();
private:
	static ContainerStyle * DefaultStyle;
};


#endif