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

#include <twines/include/twine.h>
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
	int width = this->style->getPenWidth(); // [!] could differ depending on the type
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

	bool isSelected = (this->getVertexGraphics(true)->isSelected()) || (this->getVertexGraphics(false)->isSelected());

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

	if (lineUpdate)
	{
		QPainterPath path;

		if (this->style->getIsCurvy() && (this->points.size() > 2) )
		{  // spline evaluation	
			path = getSplinePath(this->points);
		}
		else if (this->points.size() != 0) // if zero points, empty path
		{ // straight line(s)		
			std::list<QPointF>::iterator pi = this->points.begin(); // 1st
			QPointF src = *pi;
			pi++; // 2nd
			QPointF tar = *pi;
			pi++; // 3rd
			
			if (this->style->getSourceDecoration() == noDeco) path.moveTo(src);
			else // for reactant arrows of invertible reactions
			{
				// the first source
				QPainterPath source = this->getVertexGraphics(true)->shape();
				QPointF tPoint = this->getVertexGraphics(true)->scenePos();
				QPointF pos = src;
				
				float d = sqrt((src.x()-tar.x())*(src.x()-tar.x()) + (src.y()-tar.y())*(src.y()-tar.y()));
				float interval = (float) 1 / d; // to roughly computes the thing every pixel between source and target...
				
				float x, y;	
				for (float f=0; f<=1; f+=interval)
				{
					x = f*tar.x() + (1-f)*src.x();
					y = f*tar.y() + (1-f)*src.y();

					QPointF p(x,y);
					QPointF p1( p.x() - tPoint.x(),  p.y() - tPoint.y() );

					QPainterPath shape;
					QRectF r(0, 0, 5, 5);
					r.moveCenter(p1);
					shape.addEllipse(r);

					if (!source.intersects(shape) && !source.contains(shape)) { pos = p; break; }
				}
				path.moveTo(pos);				
			}

			while (pi != this->points.end())
			{
				path.lineTo(tar);
				src = tar;
				tar = *pi;
				pi++;
			}

			if (this->style->getTargetDecoration() == noDeco) path.lineTo(target);
			else // for the trigger thingy
			{
				// the last target
				QPainterPath target = this->getVertexGraphics(false)->shape();
				QPointF tPoint = this->getVertexGraphics(false)->scenePos();
				QPointF pos = src;
				
				float d = sqrt((src.x()-tar.x())*(src.x()-tar.x()) + (src.y()-tar.y())*(src.y()-tar.y()));
				float interval = (float) 1 / d; // to roughly computes the thing every pixel between source and target...
				
				float x, y;	
				for (float f=1; f>=0; f-=interval)
				{
					x = f*tar.x() + (1-f)*src.x();
					y = f*tar.y() + (1-f)*src.y();

					QPointF p(x,y);
					QPointF p1( p.x() - tPoint.x(),  p.y() - tPoint.y() );

					QPainterPath shape;
					QRectF r(0, 0, 5, 5);
					r.moveCenter(p1);
					shape.addEllipse(r);

					if (!target.intersects(shape) && !target.contains(shape)) { pos = p; break; }
				}
				path.lineTo(pos);
			}
		}		
		this->lineShape = path;
	}

	if (sourceUpdate)
	{
		this->sourceDecoration = this->getSourceDecoration(this->points, this->getVertexGraphics(true));
	}

	if (targetUpdate)
	{
		this->targetDecoration = this->getTargetDecoration(this->points, this->getVertexGraphics(false));
	}
}

QPainterPath EdgeGraphics::getSplinePath(std::list<QPointF> controlPoints) {
	QPainterPath spline;

	// Creating the spline
	TW_TWINE xtwine;
	TW_TWINE ytwine;

	TW_InitialiseTwine(&xtwine, TW_TYPE_Limit);
	TW_InitialiseTwine(&ytwine, TW_TYPE_Limit);

	// Adding the control points
	int i = 0;
	for (std::list<QPointF>::iterator pi = controlPoints.begin(); pi != controlPoints.end(); ++pi) {
		TW_AddControl(xtwine, i, (*pi).x());
		TW_AddControl(ytwine, i, (*pi).y());	
		++i;
	}

	// Computing the spline
	QPointF src = controlPoints.front();
	QPointF tar = controlPoints.back();	

	float d = sqrt((src.x()-tar.x())*(src.x()-tar.x()) + (src.y()-tar.y())*(src.y()-tar.y()));
	float interval = (float) (controlPoints.size() - 1) / d;

	if (this->style->getTargetDecoration() == noDeco)
	{
		spline.moveTo(src);

		TW_VALUE x, y;	
		for (float f = interval; f<= controlPoints.size() - 1; f += interval)
		{
			TW_EvaluateTwine(&x, xtwine, f);
			TW_EvaluateTwine(&y, ytwine, f);

			spline.lineTo(QPointF(x,y));
		}

		spline.lineTo(tar);
	}

	else // for the trigger thingy
	{
		QPainterPath target = this->getVertexGraphics(false)->shape();
		QPointF tPoint = this->getVertexGraphics(false)->scenePos();

		spline.moveTo(src);

		TW_VALUE x, y;	
		for (float f = interval; f<= controlPoints.size() - 1; f += interval)
		{
			TW_EvaluateTwine(&x, xtwine, f);
			TW_EvaluateTwine(&y, ytwine, f);

			QPointF p(x,y);
			QPointF p1( p.x() - tPoint.x(),  p.y() - tPoint.y() );

			QPainterPath shape;
			QRectF r(0, 0, 5, 5);
			r.moveCenter(p1);
			shape.addEllipse(r);
			if (target.intersects(shape) || target.contains(shape)) { break; }
			
			spline.lineTo(p);
		}
	}

	
	return spline;	
}

QPainterPath EdgeGraphics::getSourceDecoration(std::list<QPointF> controlPoints, QGraphicsItem * mask)
{
	QPainterPath deco;
	EdgeDecoration decoStyle = this->style->getSourceDecoration();

	if (decoStyle == noDeco) return deco;
	bool symetric = (decoStyle == circle);
	
	QPainterPath target = mask->shape();
	QPointF tPoint = mask->scenePos();

	QPointF pos;
	float angle;

	if (this->style->getIsCurvy() && (controlPoints.size() > 2))
	{
		// Creating the spline
		TW_TWINE xtwine;
		TW_TWINE ytwine;

		TW_InitialiseTwine(&xtwine, TW_TYPE_Limit);
		TW_InitialiseTwine(&ytwine, TW_TYPE_Limit);

		// Adding the control points
		int i = 0;
		for (std::list<QPointF>::iterator pi = controlPoints.begin(); pi != controlPoints.end(); ++pi)
		{
			TW_AddControl(xtwine, i, (*pi).x());
			TW_AddControl(ytwine, i, (*pi).y());	
			++i;
		}

		// Computing the spline intersection with the shape
		QPointF src = controlPoints.front();
		QPointF tar = controlPoints.back();	

		float d = sqrt((src.x()-tar.x())*(src.x()-tar.x()) + (src.y()-tar.y())*(src.y()-tar.y()));
		float interval = (float) (controlPoints.size() - 1) / d;

		TW_VALUE x, y;	
		for (float f = 0; f <= controlPoints.size() - 1; f += interval)
		{
			TW_EvaluateTwine(&x, xtwine, f);
			TW_EvaluateTwine(&y, ytwine, f);

			QPointF p(x,y);
			QPointF p1( p.x() - tPoint.x(),  p.y() - tPoint.y() );
				
			if (symetric)
			{
				QPainterPath shape;
				QRectF r(0, 0, 10, 10);
				r.moveCenter(p1);
				shape.addEllipse(r);
				if (!target.intersects(shape) && !target.contains(shape)) {	pos = p; break;	}
			}				
			else
			{
				if (!target.contains(p1))
				{
					pos = p;
					if (!p1.x()) angle = (p1.y() > 0)? -90: 90;
					else angle = (180/3.1416) * atan(p1.y()/p1.x());
					if (p1.x() > 0) angle += 180;
					break;
				}
			}
		}			
	}
	else
	{
		// Computing the line intersection with the shape
		QPointF src = controlPoints.front();
		QPointF tar = controlPoints.back();	

		float d = sqrt((src.x()-tar.x())*(src.x()-tar.x()) + (src.y()-tar.y())*(src.y()-tar.y()));
		float interval = (float) 1 / d;

		float x, y;	
		for (float f=0; f<=1; f+=interval)
		{
			x = f*tar.x() + (1-f)*src.x();
			y = f*tar.y() + (1-f)*src.y();

			QPointF p(x,y);
			QPointF p1( p.x() - tPoint.x(),  p.y() - tPoint.y() );

			if (symetric)
			{
				QPainterPath shape;
				QRectF r(0, 0, 10, 10);
				r.moveCenter(p1);
				shape.addEllipse(r);
				
				if (!target.intersects(shape) && !target.contains(shape)) { pos = p; break; }
			}
			else
			{
				if (!target.contains(p1))
				{
					pos = p;
					if (!p1.x()) angle = (p1.y() > 0)? -90: 90;
					else angle = (180/3.1416) * atan(p1.y()/p1.x());
					if (p1.x() > 0) angle += 180;
					break;
				}
			}
		}			
	}
	
	if (symetric)
	{
		QRectF r(0, 0, 10, 10);
		r.moveCenter(pos);
		deco.addEllipse(r);
	}
	else
	{
		QMatrix matrix;
		matrix.translate(pos.x(), pos.y());
		matrix.rotate(angle);

		std::list<QPointF> pList;
		if (decoStyle == triangleArrow)
		{
			pList.push_back(matrix.map(QPointF(-10, -5)));
			pList.push_back(matrix.map(QPointF(-10, 5)));
			pList.push_back(matrix.map(QPointF(0, 0)));
		}
		else
		{
			pList.push_back(matrix.map(QPointF(-5, 5)));
			pList.push_back(matrix.map(QPointF(-10, 0)));
			pList.push_back(matrix.map(QPointF(-5, -5)));
			pList.push_back(matrix.map(QPointF(0, 0)));
		}
		deco.moveTo(pList.back());
		for (std::list<QPointF>::iterator it = pList.begin(); it != pList.end(); ++it) deco.lineTo(*it);
	}

	return deco;
}


QPainterPath EdgeGraphics::getTargetDecoration(std::list<QPointF> controlPoints, QGraphicsItem * mask)
{
	QPainterPath deco;
	EdgeDecoration decoStyle = this->style->getTargetDecoration();

	if (decoStyle == noDeco) return deco;
	bool symetric = (decoStyle == circle);
	
	QPainterPath target = mask->shape();
	QPointF tPoint = mask->scenePos();

	QPointF pos;
	float angle;

	if (this->style->getIsCurvy() && (controlPoints.size() > 2))
	{
		// Creating the spline
		TW_TWINE xtwine;
		TW_TWINE ytwine;

		TW_InitialiseTwine(&xtwine, TW_TYPE_Limit);
		TW_InitialiseTwine(&ytwine, TW_TYPE_Limit);

		// Adding the control points
		int i = 0;
		for (std::list<QPointF>::iterator pi = controlPoints.begin(); pi != controlPoints.end(); ++pi)
		{
			TW_AddControl(xtwine, i, (*pi).x());
			TW_AddControl(ytwine, i, (*pi).y());	
			++i;
		}

		// Computing the spline intersection with the shape
		QPointF src = controlPoints.front();
		QPointF tar = controlPoints.back();	

		float d = sqrt((src.x()-tar.x())*(src.x()-tar.x()) + (src.y()-tar.y())*(src.y()-tar.y()));
		float interval = (float) (controlPoints.size() - 1) / d;

		TW_VALUE x, y;	
		for (float f = controlPoints.size() - 1; f >= 0; f -= interval)
		{
			TW_EvaluateTwine(&x, xtwine, f);
			TW_EvaluateTwine(&y, ytwine, f);

			QPointF p(x,y);
			QPointF p1( p.x() - tPoint.x(),  p.y() - tPoint.y() );
				
			if (symetric)
			{
				QPainterPath shape;
				QRectF r(0, 0, 10, 10);
				r.moveCenter(p1);
				shape.addEllipse(r);
				if (!target.intersects(shape) && !target.contains(shape)) {	pos = p; break;	}
			}				
			else
			{
				if (!target.contains(p1))
				{
					pos = p;
					if (!p1.x()) angle = (p1.y() > 0)? -90: 90;
					else angle = (180/3.1416) * atan(p1.y()/p1.x());
					if (p1.x() > 0) angle += 180;
					break;
				}
			}
		}			
	}
	else
	{
		// Computing the line intersection with the shape
		QPointF src = controlPoints.front();
		QPointF tar = controlPoints.back();	

		float d = sqrt((src.x()-tar.x())*(src.x()-tar.x()) + (src.y()-tar.y())*(src.y()-tar.y()));
		float interval = (float) 1 / d;

		float x, y;	
		for (float f=1; f>=0; f-=interval)
		{
			x = f*tar.x() + (1-f)*src.x();
			y = f*tar.y() + (1-f)*src.y();

			QPointF p(x,y);
			QPointF p1( p.x() - tPoint.x(),  p.y() - tPoint.y() );

			if (symetric)
			{
				QPainterPath shape;
				QRectF r(0, 0, 10, 10);
				r.moveCenter(p1);
				shape.addEllipse(r);
				
				if (!target.intersects(shape) && !target.contains(shape)) { pos = p; break; }
			}
			else
			{
				if (!target.contains(p1))
				{
					pos = p;
					if (!p1.x()) angle = (p1.y() > 0)? -90: 90;
					else angle = (180/3.1416) * atan(p1.y()/p1.x());
					if (p1.x() > 0) angle += 180;
					break;
				}
			}
		}			
	}
	
	if (symetric)
	{
		if (decoStyle == circle)
		{
			QRectF r(0, 0, 10, 10);
			r.moveCenter(pos);
			deco.addEllipse(r);
		}	
	}
	else
	{
		QMatrix matrix;
		matrix.translate(pos.x(), pos.y());
		matrix.rotate(angle);

		std::list<QPointF> pList;

		if (decoStyle == triangleArrow || decoStyle == emptyTriangle || decoStyle == crossedArrow) // only the filling and extra deco differs
		{
			pList.push_back(matrix.map(QPointF(-10, -5)));
			pList.push_back(matrix.map(QPointF(-10, 5)));
			pList.push_back(matrix.map(QPointF(0, 0)));
		}
		if (decoStyle == diamond)
		{
			pList.push_back(matrix.map(QPointF(5, 5)));
			pList.push_back(matrix.map(QPointF(10, 0)));
			pList.push_back(matrix.map(QPointF(5, -5)));
			pList.push_back(matrix.map(QPointF(0, 0)));
		}
		if (decoStyle == verticalline)
		{
			pList.push_back(matrix.map(QPointF(-2, -7)));
			pList.push_back(matrix.map(QPointF(-2, 7)));		
		}

		deco.moveTo(pList.back());
		for (std::list<QPointF>::iterator it = pList.begin(); it != pList.end(); ++it) deco.lineTo(*it);

		if (decoStyle == crossedArrow)
		{
			deco.moveTo(matrix.map(QPointF(-13, -7)));
			deco.lineTo(matrix.map(QPointF(-13, 7)));
		}
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