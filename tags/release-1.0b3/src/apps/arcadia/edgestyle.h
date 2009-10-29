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
 *  EdgeStyle.h
 *  arcadia
 *
 *  Created by Alice Villeger on 05/06/2008.
 *	Last Documented never.
 *
 */

#ifndef EDGESTYLE_H
#define EDGESTYLE_H

// local
class EdgeProperty;

typedef enum { noDeco, triangleArrow, circle, diamond, emptyTriangle, verticalline, crossedArrow } EdgeDecoration;

/**********************************************
* EdgePart: 3 styles, line, source and target *
**********************************************/
typedef enum { linePart, sourcePart, targetPart } EdgePart;

/************
* EdgeStyle *
*************
**********************************************************/
class EdgeStyle {
public:
	static EdgeStyle* GetDefaultStyle();
	EdgeStyle(int r=0, int g=0, int b=0, bool curve = false, EdgeDecoration sDeco = noDeco, EdgeDecoration tDeco = noDeco);

	int getPenWidth() { return this->penWidth; }
	int red() { return this->rColor; }
	int green() { return this->gColor; }
	int blue() { return this->bColor; }
	
	EdgeDecoration getTargetDecoration() { return this->targetDecoration; }
	EdgeDecoration getSourceDecoration() { return this->sourceDecoration; }
	bool getIsCurvy() { return this->isCurvy; }

protected:
	int penWidth;

	EdgeDecoration targetDecoration;
	EdgeDecoration sourceDecoration;

private:
	static EdgeStyle* DefaultStyle;

	int rColor;
	int gColor;
	int bColor;
	bool isCurvy;
};

#endif