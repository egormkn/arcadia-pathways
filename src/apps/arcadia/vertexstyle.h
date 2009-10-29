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
 *  VertexStyle.h
 *  arcadia
 *
 *  Created by Alice Villeger on 04/06/2008.
 *	Last Documented never.
 *
 */

#ifndef VERTEXSTYLE_H
#define VERTEXSTYLE_H

#include <string>

// local: for picking the right default style
class VertexProperty;

/********
* Shape *
*********
* Only two shapes here: ellipse or rectangle
********************************************/
enum Shape { ellipse, underlinedEllipse,
			curvyRectangle, underlinedCurvyRectangle,
			cutRectangle, underlinedCutRectangle,
			nullSign,
			rectangle,
			squareIOHori, squareIOVert,
			circular, underlinedCircular,
			circleIOHori, circleIOVert,
			donutIOHori, donutIOVert,
			doubleEllipse, underlinedDoubleEllipse,
			halfCurvyRectangle, underlinedHalfCurvyRectangle,
			hexagon, underlinedHexagon,
			concaveHex, underlinedConcaveHex,
			flatLine
};

enum Filling { foreground, background, blank };

/********************
* VertexStyle *
*********************
********************************************************************/
class VertexStyle
{
public:
	VertexStyle(int r=160, int g=160, int bl=160, Shape s = rectangle, float p=10, float b=2, float m=10, bool CSSmodeEnabled=true);

	float getBoundingOffset()	{ return (this->isMidget? 0.5: 1) * (this->padding*2 + this->border*(this->CSSMode? 2: 1)); }
	float getPaintingOffset()	{ return (this->isMidget? 0.5: 1) * (this->padding*2 + this->border*(this->CSSMode? 1: 0)); } 
	float getFullOffset()		{ return (this->isMidget? 0.5: 1) * (this->padding*2 + this->border*(this->CSSMode? 2: 0) + this->margin*2); }

	float getFullWidth(const std::string text = "");
	float getFullHeight(const std::string text = "");
	float getBoundingWidth(const std::string text = "");
	float getBoundingHeight(const std::string text = "");
	
	int getZValue(bool isSelected = false) { return (isSelected? 3: 1); }

	int red() { return this->rColor; }
	int green() { return this->gColor; }
	int blue() { return this->bColor; }

	Filling getFillInside() { return this->fillInside; }
	float getBorder() { return this->border; }
	bool getIsMidget() { return this->isMidget; }
	bool getShowLabel() { return this->showLabel; }
	
	Shape getShape() { return this->shape; }

	float getMargin() { return this->margin; }
	
protected:	
	bool showLabel;
	bool isMidget;
	Filling fillInside;

	int rColor;
	int gColor;
	int bColor;

private:	
	Shape shape;

	float padding;
	float border;
	float margin;

	bool CSSMode;
	
	// some Qt independent font family information???
	
public:
	static VertexStyle* GetDefaultStyle();
private:
	static VertexStyle* DefaultStyle;
};

#endif