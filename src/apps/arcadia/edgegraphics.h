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
 *  EdgeGraphics.h
 *  arcadia
 *
 *  Created by Alice Villeger on 25/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#ifndef EDGEGRAPHICS_H
#define EDGEGRAPHICS_H

// base class
#include "graphgraphics.h"

// local
#include "edgestyle.h";

class Connector;
class VertexGraphics; // needed to compute proper edge decorations
class LayoutGraphView; // needed to get access to the vertex graphics

/***************
* EdgeGraphics *
****************
* This QGraphicsItem represents a Connector with a given EdgeStyle
* [!] could it be possible to just give the Connector and guess the style for here???
*
* It overloads the usual QGraphicsItem painting methods: boundingRect, shape, paint
*
* Methods are provided to set up the style and update the various edge shapes
* [!] should these be public?
*
* These shapes are stored as attributes, as well as the source and target points
* These are obtained directly from the Connector, through getPoint
* [!] not properly updated ATM, though!
*************************************************************************************/
class EdgeGraphics : public GraphGraphics {
public:
	bool isEdge() { return true; }

	EdgeGraphics(LayoutGraphView *gs, Connector * c, EdgeStyle *s = NULL, bool h = false);
	
	// painting related methods
    QRectF boundingRect() const;
    QPainterPath shape() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

	void updatePos();

	void setHidden(bool h) { this->hidden = h; }

	void quickTranslate();
	void quickUpdate();
	
private:
	bool hidden;

	void setPosition();
	// style related methods
	void updateShapes(bool lineUpdate = true, bool sourceUpdate = true, bool targetUpdate = true);
	void setStyle(EdgeStyle *s);

	Connector * connector;
	EdgeStyle *style;
	
	QPainterPath lineShape;
	QPainterPath sourceDecoration;
	QPainterPath targetDecoration;	
	
	QPointF source;
	QPointF target;
	std::list<QPointF> points;

	QPointF getPoint(bool isSource) const;

	VertexGraphics * getVertexGraphics(bool isSource) const;
	LayoutGraphView * graphScene;
	
	// styling stuff
	QRectF getBoundingRect(QPainterPath path, EdgePart type) const;
	void configurePainting(QPainter *painter, EdgePart type, bool isSelected);

	QPainterPath getSplinePath(QGraphicsItem * sourceShape, QGraphicsItem * targetShape);
	QPainterPath getStraightPath(QGraphicsItem * sourceShape, QGraphicsItem * targetShape);

	QPainterPath getDecorationPath(EdgeDecoration decoStyle, QPointF pos, float angle);
	QPointF getDecorationPos(QPointF src, QPointF tar, QGraphicsItem * endItem, bool isSource, float * angle = NULL);
	QPointF getDecorationPosOnSpline(QGraphicsItem * endItem, bool isSource, float * angle = NULL);
	bool isInShape(QPointF p, QGraphicsItem * endItem, EdgeDecoration deco, float * angle = NULL);
	QPointF addDecorationOffset(QPointF p, EdgeDecoration deco, bool isSource);
	
	QPainterPath getSourceDecoration(QGraphicsItem * sourceShape);
	QPainterPath getTargetDecoration(QGraphicsItem * targetShape);
};

#endif