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
 *  EdgeGraphics.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 10/01/2008.
 *  Last documented on 17/04/2008.
 *
 */

#include "edgegraphics.h"

#include <iostream>

// Qt
#include <QPainter>
#include <QFrame> // for computing the visible area
#include <QGraphicsView> // for computing the visible area

#include <twines/twine.h>
#include <math.h>

// local
#include "edgestyle.h"
#include "connector.h"
#include "vertexgraphics.h"
#include "layoutgraphview.h"

// nice try, unfortunately doesn't work in SBGN because the position of reaction nodes
// is not the same as the possition of their input and output nodes
void EdgeGraphics::quickTranslate()
{
/*
	float dx = this->getVertexGraphics(true)->pos().x() - this->source.x();
	float dy = this->getVertexGraphics(true)->pos().y() - this->source.y();

	this->source = this->getVertexGraphics(true)->pos();
	this->setPos(this->pos().x() + dx,  this->pos().y() + dy);
*/
	float dx = this->getPoint(true).x() - this->source.x();
	float dy = this->getPoint(true).y() - this->source.y();

	this->source = this->getPoint(true);	
	this->setPos(this->pos().x() + dx,  this->pos().y() + dy);
}

void EdgeGraphics::quickUpdate()
{
/*
	this->source = this->getVertexGraphics(true)->pos();
	this->target = this->getVertexGraphics(false)->pos();

	this->points.clear();
	this->points.push_back(this->source);
	this->points.push_back(this->target);

	this->updateShapes();
*/
	this->source = this->getPoint(true);
	this->target = this->getPoint(false);

	this->points.clear();
	this->points.push_back(this->source);
	this->points.push_back(this->target);

	this->updateShapes();
}
	
/**************
* Constructor *
***************
* Sets up the connector, then the style
* Also saves the current source and target points
* and updates the shapes accordingly
*************************************************/
EdgeGraphics::EdgeGraphics(LayoutGraphView * gs, Connector * c, EdgeStyle *s, bool h) : graphScene(gs), connector(c)
{
//	this->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable);
	
	this->hidden = h;
	this->setStyle(s);
	this->setPosition();
}

void EdgeGraphics::updatePos()
{
	if (this->hidden) return;
	if (this->source != this->getPoint(true) || this->target != this->getPoint(false) || this->points.size() != this->connector->getPoints().size() )
	{
// now that edges are selectable, the translation bit is done automatically by Qt
/*
		//dealing with simple translation issues (when the whole edge is being moved because both ends are selected)
		float dx1 = this->getPoint(true).x() - this->source.x();
		float dy1 = this->getPoint(true).y() - this->source.y();
		float dx2 = this->getPoint(false).x() - this->target.x();
		float dy2 = this->getPoint(false).y() - this->target.y();
		if ( (dx1 == dx2) && (dy1 == dy2) && (this->points.size() == this->connector->getPoints().size()) )
		{
			this->source = this->getPoint(true);
			this->target = this->getPoint(false);	
			
			this->points.clear();
			std::list< std::pair <int, int> > cPoints = this->connector->getPoints();
			for (std::list< std::pair <int, int> >::iterator it = cPoints.begin(); it != cPoints.end(); ++it)
			{
				this->points.push_back(QPointF((*it).first, (*it).second));
			}
			
			this->setPos(this->pos().x() + dx1,  this->pos().y() + dy1);
		}
		else
*/
		this->setPosition();
	}
}

void EdgeGraphics::setPosition()
{
	this->source = this->getPoint(true);
	this->target = this->getPoint(false);

	this->points.clear();
	std::list< std::pair <int, int> > cPoints = this->connector->getPoints();
	for (std::list< std::pair <int, int> >::iterator it = cPoints.begin(); it != cPoints.end(); ++it)
	{
		this->points.push_back(QPointF((*it).first, (*it).second));		
	}

	this->updateShapes();
}

/***********
* getPoint *
************
* Returns the source or target point of the connector
*****************************************************/
QPointF EdgeGraphics::getPoint(bool isSource) const
{
	std::pair <int, int> p = this->connector->getPoint(isSource);
	return QPointF(p.first, p.second);
}

/***************
* boundingRect *
****************
* returns the boundingRect computed by the style
* of the lineShape, sourceDecoration and targetDecoration
*********************************************************/
QRectF EdgeGraphics::boundingRect() const
{
	if (this->hidden) { QRectF r; return r; }

	QPainterPath p;

	p.addRect(this->getBoundingRect(this->lineShape, linePart));

/* [!] this gives back totally broken values!!! to investigate at some point
	if (this->style->getSourceDecoration() != noDeco)
		p.addRect(this->getBoundingRect(this->sourceDecoration, sourcePart));
	if (this->style->getTargetDecoration() != noDeco)
		p.addRect(this->getBoundingRect(this->targetDecoration, targetPart));
*/	

	

	return p.boundingRect();
}

QRectF EdgeGraphics::getBoundingRect(QPainterPath path, EdgePart type) const
{
	QRectF r = path.boundingRect();
	int width = this->style->getPenWidth() + 10; // [!] could differ depending on the type (so we add 10 as security margin
	r.setTop( r.top() - width/2 );
	r.setLeft( r.left() - width/2 );
	r.setBottom( r.bottom() + width/2 );
	r.setRight( r.right() + width/2 );	
	return r;
}

/********
* shape *
*********
* Simply the shape of the boundingRect!
* [!] could be more precise?
* [!] the problem is adding margins to an arbitrary shape...
************************************************************/
QPainterPath EdgeGraphics::shape() const
{
	QPainterPath edge;
	edge.addRect(this->boundingRect());	
    return edge;
}
	
/********
* paint *
*********
* With antialiasing, at depth zero,
* paints the various shapes (line, source, target)
* with the setting provided by the EdgeStyle 
* [!] what about selection? change depth, width?
* [!] shapes are NOT updated real time
**************************************************/
void EdgeGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
/*
	QRectF r = widget->rect();
	std::cout << r.left() << ", " << r.top() << " ; " << r.right() << ", " << r.bottom() << std::endl;
	QGraphicsView *view = (QGraphicsView*)widget->parent();
	QPointF p1 = view->mapToScene(r.left(), r.top());	
	QPointF p2 = view->mapToScene(r.right(), r.bottom());	
	r = QRectF(p1, p2);	
	std::cout << r.left() << ", " << r.top() << " ; " << r.right() << ", " << r.bottom() << std::endl;
	this->hidden = (!r.contains(this->source)) || (!r.contains(this->target));
*/
	if (this->hidden) return;

	painter->setRenderHint(QPainter::Antialiasing, true);

	bool isSelected = false;
	if (this->getVertexGraphics(true))
	{
		isSelected |= this->getVertexGraphics(true)->isSelected();
	}
	if (this->getVertexGraphics(false))
	{
		isSelected |= this->getVertexGraphics(false)->isSelected();
	}

//	this->setZValue(isSelected? 2: 0); // [!] ugly with decorations at the wrong place when selected
	this->setZValue(0);
	
	this->configurePainting( painter, linePart, isSelected );
	painter->drawPath( this->lineShape );

	this->configurePainting( painter, sourcePart, isSelected );
	painter->drawPath( this->sourceDecoration );

	this->configurePainting( painter, targetPart, isSelected );
	painter->drawPath( this->targetDecoration );
}

void EdgeGraphics::configurePainting(QPainter *painter, EdgePart type, bool isSelected)
{
	QPen p = painter->pen();
	
	QColor baseColor(this->style->red(), this->style->green(), this->style->blue());
	p.setColor(isSelected? baseColor: baseColor.dark(150));
	int penWidth = this->style->getPenWidth();
	p.setWidth(isSelected? penWidth*2: penWidth);
	
	switch (type) {
	case linePart:
		break;
	case sourcePart:
		if (this->style->getSourceDecoration() != triangleArrow)
			painter->setBrush(QBrush(Qt::white));
		else
			painter->setBrush(QBrush(baseColor));		
		break;
	case targetPart:
		if (this->style->getTargetDecoration() != triangleArrow)
			painter->setBrush(QBrush(Qt::white));
		else
			painter->setBrush(QBrush(baseColor));		
		break;
	}
	
	painter->setPen(p);
}

/***********
* setStyle *
************
* sets up the style
* Default style if none specified
*********************************/
void EdgeGraphics::setStyle(EdgeStyle *s) {
	if (s) this->style = s;
	else this->style = EdgeStyle::GetDefaultStyle();
}

/***************
* updateShapes *
****************
* gets the specified shapes from the style, given some control points
*********************************************************************/
void EdgeGraphics::updateShapes(bool lineUpdate, bool sourceUpdate, bool targetUpdate)
{
	this->setPos(0,0);
	
	// slightly costly operation, try to do only when necessary
	QGraphicsItem * sourceShape = NULL;
	QGraphicsItem * targetShape = NULL;

	if (lineUpdate)
	{
		sourceShape = this->getVertexGraphics(true);
		targetShape = this->getVertexGraphics(false);
		
		QPainterPath path;

		if (this->style->getIsCurvy() && (this->points.size() > 2) )
		{  // spline evaluation	
			path = getSplinePath(sourceShape, targetShape);
		}
		else if (this->points.size() != 0) // if zero points, empty path
		{ // straight line(s)
			path = getStraightPath(sourceShape, targetShape);			
		}		
		this->lineShape = path;
	}

	if (sourceUpdate)
	{
		if (!sourceShape) sourceShape = this->getVertexGraphics(true);
		this->sourceDecoration = this->getSourceDecoration(sourceShape);
	}

	if (targetUpdate)
	{
		if (!targetShape) targetShape = this->getVertexGraphics(false);
		this->targetDecoration = this->getTargetDecoration(targetShape);
	}
}

QPainterPath EdgeGraphics::getStraightPath(QGraphicsItem * sourceShape, QGraphicsItem * targetShape)
{
	QPainterPath path;
	
	std::list<QPointF>::iterator pi = this->points.begin(); // 1st point
	QPointF src = *pi;
	pi++; // 2nd point
	QPointF tar = *pi;
	pi++; // 3rd point (we can have a polyline)
	
	// initializes the path
	if (this->style->getSourceDecoration() == noDeco) path.moveTo(src);
	else // for reactant arrows of invertible reactions
		path.moveTo( this->getDecorationPos(src, tar, sourceShape, true) );

	// go through each point in the polyline
	while (pi != this->points.end())
	{
		path.lineTo(tar);
		src = tar;
		tar = *pi;
		pi++;
	}

	if (this->style->getTargetDecoration() == noDeco) path.lineTo(tar);
	else // for the trigger thingy
		path.lineTo( this->getDecorationPos(src, tar, targetShape, false) );
	
	return path;
}

bool EdgeGraphics::isInShape(QPointF p, QGraphicsItem * endItem, EdgeDecoration deco, float * angle)
{
	bool value = true;
	
	QPainterPath endShape = endItem->shape();
	QPointF endPoint( p.x() - endItem->scenePos().x(),  p.y() - endItem->scenePos().y() );

	if ( deco == circle )
	{
		QPainterPath decoShape;
		QRectF decoRect(0, 0, 10, 10);
		decoRect.moveCenter(endPoint);
		decoShape.addEllipse(decoRect);

		if (!endShape.intersects(decoShape) && !endShape.contains(decoShape)) value = false;
	}
	else if (!endShape.contains(endPoint)) value = false;

	if (angle)
	{
		if ( !endPoint.x() ) (*angle) = ( endPoint.y() > 0 )? -90: 90;
		else (*angle) = (180/3.1416) * atan( endPoint.y() / endPoint.x() );
		if ( endPoint.x() > 0 ) (*angle) += 180;
	}

	return value;
}

// computes the buffer to leave between the edge and its source if there is a decoration
QPointF EdgeGraphics::getDecorationPos(QPointF src, QPointF tar, QGraphicsItem * endItem, bool isSource, float * angle)
{
	QPointF pos = src;
	EdgeDecoration deco = isSource? this->style->getSourceDecoration() : this->style->getTargetDecoration();
				
	float d = sqrt((src.x()-tar.x())*(src.x()-tar.x()) + (src.y()-tar.y())*(src.y()-tar.y()));
	float interval = (float) 1 / d;
				
	float x, y;	

	for (float f=0; f<=1; f+=interval)
	{
		float g = isSource? f : 1-f;

		x = g*tar.x() + (1-g)*src.x();
		y = g*tar.y() + (1-g)*src.y();		

		QPointF p(x,y);
		
		if (!isInShape( p, endItem, deco, angle))
		{
			p = this->addDecorationOffset(p, deco, isSource);
			pos = p; break;
		}
	}

	return pos;
}

// To make sure the arc of modifiers points at the decoration, not the reaction itself
QPointF EdgeGraphics::addDecorationOffset(QPointF p, EdgeDecoration deco, bool isSource)
{
	// if it's not broken don't fix it
	if ( (deco == noDeco) || (deco == circle) || (deco == triangleArrow) ) return p;

	// determine the space taken by the decoration
	int offset = 0;
	if (deco == verticalline) offset = 5;
	if (deco == emptyTriangle) offset = 10;
	if (deco == diamond) offset = 10;
	if (deco == crossedArrow) offset = 15;
	
	// determine the orientation of the arc to the reaction: looks at position of side port (= arc target) to real center of glyph
	CloneContent * endContent = isSource? this->connector->getSource() : this->connector->getTarget();
	float dx = p.x() - endContent->x();
	float dy = p.y() - endContent->y();
	if (dx*dx > dy*dy) // horizontal offset
		if (dx > 0) p.setX(p.x() + offset);
		else p.setX (p.x() - offset);
	else // vertical offset
		if (dy > 0) p.setY(p.y() + offset);
		else p.setY (p.y() - offset);

	return p;
}

QPainterPath EdgeGraphics::getSplinePath(QGraphicsItem * sourceShape, QGraphicsItem * targetShape)
{	
	QPainterPath spline;

	// Creating the spline
	TW_TWINE xtwine;
	TW_TWINE ytwine;

	TW_InitialiseTwine(&xtwine, TW_TYPE_Limit);
	TW_InitialiseTwine(&ytwine, TW_TYPE_Limit);

	// Adding the control points
	int i = 0;
	for (std::list<QPointF>::iterator pi = this->points.begin(); pi != this->points.end(); ++pi) {
		TW_AddControl(xtwine, i, (*pi).x());
		TW_AddControl(ytwine, i, (*pi).y());	
		++i;
	}

	// Computing the spline
	QPointF src = this->points.front();
	QPointF tar = this->points.back();

	float d = sqrt((src.x()-tar.x())*(src.x()-tar.x()) + (src.y()-tar.y())*(src.y()-tar.y()));
	float interval = (float) (this->points.size() - 1) / d;

	spline.moveTo(src);

	EdgeDecoration deco = this->style->getTargetDecoration();

	TW_VALUE x, y;	
	for (float f = interval; f<= this->points.size() - 1; f += interval)
	{
		TW_EvaluateTwine(&x, xtwine, f);
		TW_EvaluateTwine(&y, ytwine, f);

		QPointF p(x,y);
		
		if (deco != noDeco)
			if ( isInShape( p, targetShape, this->style->getTargetDecoration()) ) break;
		
		spline.lineTo(p);
	}
		
	if (deco == noDeco) spline.lineTo(tar);
	
	return spline;	
}

QPointF EdgeGraphics::getDecorationPosOnSpline(QGraphicsItem * endItem, bool isSource, float * angle)
{
	// Creating the spline
	TW_TWINE xtwine;
	TW_TWINE ytwine;

	TW_InitialiseTwine(&xtwine, TW_TYPE_Limit);
	TW_InitialiseTwine(&ytwine, TW_TYPE_Limit);

	// Adding the control points
	int i = 0;
	for (std::list<QPointF>::iterator pi = this->points.begin(); pi != this->points.end(); ++pi)
	{
		TW_AddControl(xtwine, i, (*pi).x());
		TW_AddControl(ytwine, i, (*pi).y());	
		++i;
	}

	// Computing the spline intersection with the shape
	QPointF src = this->points.front();
	QPointF tar = this->points.back();	

	QPointF pos = isSource? tar : src;

	float d = sqrt((src.x()-tar.x())*(src.x()-tar.x()) + (src.y()-tar.y())*(src.y()-tar.y()));
	float interval = (float) (this->points.size() - 1) / d;

	EdgeDecoration deco = isSource? this->style->getSourceDecoration() : this->style->getTargetDecoration();

	TW_VALUE x, y;	
	for (float f = 0; f <= this->points.size() - 1; f += interval)
	{
		float g = isSource? f : this->points.size() - 1 - f;
		TW_EvaluateTwine(&x, xtwine, g);
		TW_EvaluateTwine(&y, ytwine, g);

		QPointF p(x,y);
	
		if ( !isInShape( p, endItem, deco, angle) ) { pos = p; break; }
	}
	
	return pos;
}

QPainterPath EdgeGraphics::getSourceDecoration(QGraphicsItem * sourceShape)
{
	QPainterPath deco;
	EdgeDecoration decoStyle = this->style->getSourceDecoration();
	if (decoStyle == noDeco) return deco;
	
	bool symetric = (decoStyle == circle);
	
	QPointF pos;
	float angle;

	if (this->style->getIsCurvy() && (this->points.size() > 2))
		pos = this->getDecorationPosOnSpline(sourceShape, true, &angle);
	else // Computing the line intersection with the shape
		pos = this->getDecorationPos(this->points.front(), this->points.back(), sourceShape, true, &angle);
	
	return this->getDecorationPath(decoStyle, pos, angle);
}

QPainterPath EdgeGraphics::getTargetDecoration(QGraphicsItem * targetShape)
{
	QPainterPath deco; // return value
	EdgeDecoration decoStyle = this->style->getTargetDecoration();
	if (decoStyle == noDeco) return deco;

	QPointF pos; // where to draw the decoration
	float angle; // at which angle

	// if dealing with curve
	if (this->style->getIsCurvy() && (this->points.size() > 2))
		pos = this->getDecorationPosOnSpline(targetShape, false, &angle);
	else // dealing with straight line
		pos = this->getDecorationPos(this->points.front(), this->points.back(), targetShape, false, &angle);
	
	return this->getDecorationPath(decoStyle, pos, angle);
}
	
QPainterPath EdgeGraphics::getDecorationPath(EdgeDecoration decoStyle, QPointF pos, float angle)
{
	QPainterPath deco;

	if (decoStyle == circle)
	{
		QRectF r(0, 0, 10, 10);
		r.moveCenter(pos);
		deco.addEllipse(r);
		return deco;
	}

	QMatrix matrix;
	matrix.translate(pos.x(), pos.y());
	matrix.rotate(angle);

	std::list<QPointF> pList;

	// only the filling and extra deco differs
	if (decoStyle == triangleArrow)
	{
		pList.push_back(matrix.map(QPointF(-10, -5)));
		pList.push_back(matrix.map(QPointF(-10, 5)));
		pList.push_back(matrix.map(QPointF(0, 0)));
	}

	if (decoStyle == emptyTriangle)
	{
		pList.push_back(matrix.map(QPointF(0, -5)));
		pList.push_back(matrix.map(QPointF(0, 5)));
		pList.push_back(matrix.map(QPointF(10, 0)));
	}

	if (decoStyle == crossedArrow)
	{
		pList.push_back(matrix.map(QPointF(5, -5)));
		pList.push_back(matrix.map(QPointF(5, 5)));
		pList.push_back(matrix.map(QPointF(15, 0)));	
	}

	if (decoStyle == diamond)
	{
		pList.push_back(matrix.map(QPointF(5, 5)));
		pList.push_back(matrix.map(QPointF(0, 0)));
		pList.push_back(matrix.map(QPointF(5, -5)));
		pList.push_back(matrix.map(QPointF(10, 0)));
	}

	if (decoStyle == verticalline)
	{
		pList.push_back(matrix.map(QPointF(0, -7)));
		pList.push_back(matrix.map(QPointF(0, 7)));		
	}

	deco.moveTo(pList.back());
	for (std::list<QPointF>::iterator it = pList.begin(); it != pList.end(); ++it) deco.lineTo(*it);
	
	if (decoStyle == crossedArrow)
	{
		deco.moveTo(matrix.map(QPointF(0, -7)));
		deco.lineTo(matrix.map(QPointF(0, 7)));	
	}
	
	return deco;
}

VertexGraphics * EdgeGraphics::getVertexGraphics(bool isSource) const
{
	CloneContent * c;
	
	if (isSource)	c = this->connector->getSource();
	else			c = this->connector->getTarget();

	return this->graphScene->getVertexGraphics(c);
}
