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
 *  ContainerGraphics.h
 *  arcadia
 *
 *  Created by Alice Villeger on 10/04/2008.
 *  Last documented on 17/04/2008.
 *
 */

#ifndef CONTAINERGRAPHICS_H
#define CONTAINERGRAPHICS_H

// Qt base class
#include "graphgraphics.h"

// local, the object to represent
class ContainerContent;
class ContainerStyle;

/********************
* ContainerGraphics *
*********************
* This GraphicsItem represents a Container
* It overloads the usual painting related methods:
* boundingRect, shape and paint
*
* Like VertexGraphics, it does so by defining a
* boundingRectangle, a boundingShape and a PaintingShape
* [!] doesn't support styles, ATM!
* [!] update of the Container's geometry not supported
* [!] should we have a ContentGraphics superclass?
*/
class ContainerGraphics : public GraphGraphics {
public:
	bool isContainer() { return true; }

	ContainerGraphics(ContainerContent * c, ContainerStyle * s = NULL);

	// painting related methods
    QRectF boundingRect() const;
    QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

	void updatePos();
	void setUpdateFlag(); // to optimize the update pos process

private:
	void updateShapesAndRect();
	void setPosition();

	ContainerStyle * style;
	ContainerContent * container;

	QRectF boundingRectangle;
	QPainterPath boundingShape;
	QPainterPath paintingShape;
	QRectF fullRect;
	
	void setStyle(ContainerStyle * s = NULL);
	
	QRectF getRect(const QRectF r, const int offset) const;
	QPainterPath getShape(const QRectF r) const;

	QPainterPath getShape() const;
};

#endif