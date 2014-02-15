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
 *  VertexGraphics.h
 *  arcadia
 *
 *  Created by Alice Villeger on 25/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#ifndef VERTEXGRAPHICS_H
#define VERTEXGRAPHICS_H

// base class
#include "graphgraphics.h"

#include <QFont>

// local
class VertexStyle;
class CloneContent;
//class EdgeGraphics;
class LayoutGraphView;

/*****************
* VertexGraphics *
******************
* A QGraphicsItem representing a CloneContent with a certain VertexStyle
* (if no Style is given a default Style is used)
*
* The specified label (empty, by default) and style are used
* to determine the Item's exact dimensions, and from there update
* the paintingShape, a boundingRect, and a boundingShape
* (respectively used in the paint, boundingRect and shape methods)
*
* The Clone's position is used to update the Item's pos
* [!] only upon creation, ATM! could be done in the paint method too? 
***************************************************************************/
class VertexGraphics : public GraphGraphics {
public:
	bool isVertex() { return true; }

	VertexGraphics(LayoutGraphView * gs, CloneContent * c, std::string l="", VertexStyle *s = NULL);

	// read-only access to the represented Clone
	CloneContent * getCloneContent();

	// painting related methods
    QRectF boundingRect() const;
    QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

	void updateClonePosition();
	void updatePos();
	void updateStyle(VertexStyle *s);
	
private:
//	std::list<EdgeGraphics *> edgeList;
	LayoutGraphView * graphScene;

	// updating dimensions and position
	void setPosition();
	void updateShapesAndRect();

	VertexStyle * style;
	CloneContent *cloneDescriptor;	
	std::string label;
	
	QRectF boundingRectangle;
	QPainterPath boundingShape;
	QPainterPath paintingShape;

	// style related methods
	void setStyle(VertexStyle *s);
	void setLabel(std::string l);

	QRectF getBoundingRect(const std::string text = "") const;
	QPainterPath getBoundingShape(const std::string text = "") const;
	QPainterPath getPaintingShape(const std::string text = "") const;
	
	QRectF getRect(const std::string text, int offset) const;
	QPainterPath getShape(const QRectF r) const;

	QFont font;
protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
};

#endif