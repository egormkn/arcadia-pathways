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
 *  VertexGraphics.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 25/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#include "vertexgraphics.h"

#include <iostream>

#include <math.h>

// Qt
#include <QPainter>
#include <QStyleOptionGraphicsItem>

// Local
#include "vertexstyle.h"
#include "vertexproperty.h"
#include "clonecontent.h"
#include "edgegraphics.h"
#include "layoutgraphview.h"

/**************
* Constructor *
***************
* Initialises the CloneContent
* Defines the Item as Selectable 
*
* Sets up the style and the label (possibly default values)
* and consequently computes the shapes and rectangle
*
* Sets up the Item pos to the Clone's position
* [!] ATM I never update these!! no good if the Clone moves
***********************************************************/
VertexGraphics::VertexGraphics( LayoutGraphView *gs, CloneContent * c, std::string l, VertexStyle *s) : graphScene(gs), cloneDescriptor(c), label("") {
	this->setFlags(QGraphicsItem::ItemIsSelectable|QGraphicsItem::ItemIsMovable);

	this->setStyle(s);
	this->setLabel(l);
	
	this->setPosition();
}

/****************************************************************
* getCloneContent: read-only access to the represented Clone *
*****************************************************************/
CloneContent * VertexGraphics::getCloneContent() { return this->cloneDescriptor; }

/************************************************
* boundingRect: returns the style-computed rect *
*************************************************/
QRectF VertexGraphics::boundingRect() const { return this->boundingRectangle; }
/******************************************
* shape: returns the style-computed shape *
*******************************************/
QPainterPath VertexGraphics::shape() const { return this->boundingShape; }
	
/********
* paint *
*********
* Sets up the Z Value
* then lets the style do the painting
* according to the paintingShape, label
* level of detail, and selection status
***************************************/
void VertexGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
	bool isSelected = this->isSelected();
	bool isToPrint = true;

	this->setZValue( this->style->getZValue( isSelected ) );
	
	QPainterPath shape = this->paintingShape;
	const std::string text = this->label;
	float zoom = option->levelOfDetail;
	zoom = 1; // to test the SVG rendering

	painter->setRenderHint(QPainter::Antialiasing, false);

	QColor baseColor(this->style->red(), this->style->green(), this->style->blue());
	
	// black and white
/*
	if (this->style->red() > this->style->green())
	{
		baseColor = QColor(63, 63, 63);
	}
	else if (this->style->red() < this->style->green())
	{
		baseColor = QColor(127, 127, 127);	
	}
	else
	{
		baseColor = QColor(191, 191, 191);		
	}
*/

    QColor fillColor = baseColor;
	if (isSelected || isToPrint)	fillColor = fillColor.light(180);
	else							fillColor.setHsv(fillColor.hue(), fillColor.saturation()/2, fillColor.value()*0.8);

	if (zoom < 1./8) { painter->fillPath(shape, fillColor); return; }

	QColor lineColor = baseColor;
	if (isSelected || isToPrint)	lineColor = lineColor.dark(180);
	else			lineColor.setHsv(fillColor.hue(), fillColor.saturation()*0.8, fillColor.value()/2);

	if (this->style->getFillInside() == foreground)	painter->setBrush(fillColor);
	if (this->style->getFillInside() == blank) painter->setBrush(Qt::white);
	if (this->style->getFillInside() == background)	painter->setBrush(lineColor);

	QPen p;
	p.setColor(lineColor);
	p.setWidth( isSelected? this->style->getBorder()*2 : this->style->getBorder() );
	if (this->style->getIsMidget()) p.setWidth(p.width()/2);
	painter->setPen(p);
	
    if (zoom < 1./4)	{ painter->drawPath(shape); return; }

	painter->setRenderHint(QPainter::Antialiasing, true);
	painter->drawPath(shape);

	p.setColor(isSelected||isToPrint? Qt::black: baseColor.dark(270));
	painter->setPen(p);
    painter->setFont(this->font); // [!] find a way to store font info in the style???
	if (this->style->getShowLabel()) painter->drawText( shape.boundingRect(), Qt::AlignCenter, text.c_str()); 
}

/***********
* setStyle *
************
* Changes the current style
* If none is given, a default style is used
* The shapes and rect get updated
*******************************************/
void VertexGraphics::setStyle(VertexStyle *s) {
	this->style = s? s: VertexStyle::GetDefaultStyle();	
	this->updateShapesAndRect();
}

/***********
* setLabel *
************
* Changes the current label
* The shapes and rect get updated
*********************************/
void VertexGraphics::setLabel(std::string l) {
    this->label = l;
	this->updateShapesAndRect();	
}

/**********************
* updateShapesAndRect *
***********************
* Picks up a default Style if none exists
* Otherwise, lets the Style compute from the Label
* the boundingRectangle, boundingShape and paintingShape
********************************************************/
void VertexGraphics::updateShapesAndRect() {
	if (!this->style) { this->setStyle(NULL); return; }
	this->boundingRectangle = this->getBoundingRect(this->label);
	this->boundingShape = this->getBoundingShape(this->label);
	this->paintingShape = this->getPaintingShape(this->label);
}

QRectF VertexGraphics::getBoundingRect(const std::string text) const
{
	return this->getRect(text, this->style->getBoundingOffset());
}

QPainterPath VertexGraphics::getBoundingShape(const std::string text) const
{
	return this->getShape(this->getBoundingRect(text));
}

QPainterPath VertexGraphics::getPaintingShape(const std::string text) const
{
	return this->getShape(this->getRect(text, this->style->getPaintingOffset()));
}

QRectF VertexGraphics::getRect(const std::string text, int offset) const
{
	QFontMetricsF fm(this->font); 
	QRectF r;
	if (this->style->getShowLabel())	r = fm.boundingRect(text.c_str());	

	r.setWidth(r.width() + offset);
	r.setHeight(r.height() + offset);
	if (this->style->getShape() == circular || this->style->getShape() == underlinedCircular)
	{
		r.setHeight(r.width());
	}
	
	r.moveCenter(QPointF(0,0));

	return r;
}

QPainterPath VertexGraphics::getShape(const QRectF r) const
{
	QRectF myRect;
	myRect.setWidth(r.width() - 10);
	myRect.setHeight(r.height() - 10);
	myRect.moveCenter(r.center());
	
	QPainterPath p;
	
	float w, h, y, x, abs, ord; // used only in some cases
	// but some compilers don't like to see initialisation in the switch
	float i, j;

	QPainterPath path;
	path.setFillRule(Qt::WindingFill);
	switch(this->style->getShape())
	{
	case circular: // the rectangle given is always a square
	case ellipse:
		path.addEllipse(r);
		break;

	case underlinedCircular:
	case underlinedEllipse:
		path.addEllipse(r);

		w = (r.right() - r.left())/2;
		h = (r.bottom() - r.top())/2;
		y = r.bottom() - 5;
		x = sqrt( w*w * (1 - y*y / (h*h) ) );

		path.moveTo( -x, y );
		path.lineTo( +x, y );
		
		for (i = -x; i<= x ; i+=1)
		{
			path.moveTo( i, y );
			j = sqrt( h*h * (1 - i*i / (w*w) ) );
			path.lineTo( i, j );
		}
		break;
		
	case rectangle:
		path.addRect(r);
		break;
		
	case curvyRectangle:
		path.addRoundRect (r, 50);
		break;
		
	case underlinedCurvyRectangle:
		path.addRoundRect (r, 50);

		abs = (r.right() - r.left())/2;
		ord = r.bottom() - 5;

		path.moveTo( -abs, ord );
		path.lineTo( +abs, ord );
		break;
	
	case cutRectangle:
		path.moveTo(r.left(), r.top()+5);
		path.lineTo(r.left()+5, r.top());
		path.lineTo(r.right()-5, r.top());
		path.lineTo(r.right(), r.top()+5);
		path.lineTo(r.right(), r.bottom()-5);
		path.lineTo(r.right()-5, r.bottom());
		path.lineTo(r.left()+5, r.bottom());
		path.lineTo(r.left(), r.bottom()-5);		
		path.lineTo(r.left(), r.top()+5);
		break;
	
	case underlinedCutRectangle:
		path.moveTo(r.left(), r.top()+5);
		path.lineTo(r.left()+5, r.top());
		path.lineTo(r.right()-5, r.top());
		path.lineTo(r.right(), r.top()+5);
		path.lineTo(r.right(), r.bottom()-5);
		path.lineTo(r.right()-5, r.bottom());
		path.lineTo(r.left()+5, r.bottom());
		path.lineTo(r.left(), r.bottom()-5);		
		path.lineTo(r.left(), r.top()+5);
		path.moveTo(r.left(), r.bottom()-5);
		path.lineTo(r.right(), r.bottom()-5);
		break;

	case nullSign:
		path.addEllipse(r);
		path.moveTo(r.bottomLeft());
		path.lineTo(r.topRight());
		break;
		
	case squareIOHori: // flat edge
/*		path.moveTo(r.left(), (r.top()+r.bottom()) / 2);
		path.lineTo(r.right(), (r.top()+r.bottom()) / 2);
*/
		path.moveTo(r.left(), (r.top()+r.bottom()) / 2);
		path.lineTo(r.left()-5, (r.top()+r.bottom()) / 2);
		path.moveTo(r.right(), (r.top()+r.bottom()) / 2);
		path.lineTo(r.right()+5, (r.top()+r.bottom()) / 2);

		path.moveTo(r.left(), r.top());
		path.lineTo(r.left(), r.bottom());		
		path.lineTo(r.right(), r.bottom());		
		path.lineTo(r.right(), r.top());		
		path.lineTo(r.left(), r.top());		
		break;

	case squareIOVert: // flat edge
/*
		path.moveTo((r.left()+r.right())/2, r.top());
		path.lineTo((r.left()+r.right())/2, r.bottom());
*/
		path.moveTo((r.left()+r.right())/2, r.top());
		path.lineTo((r.left()+r.right())/2, r.top()-5);
		path.moveTo((r.left()+r.right())/2, r.bottom());
		path.lineTo((r.left()+r.right())/2, r.bottom()+5);

		path.moveTo(r.left(), r.top());
		path.lineTo(r.left(), r.bottom());		
		path.lineTo(r.right(), r.bottom());		
		path.lineTo(r.right(), r.top());		
		path.lineTo(r.left(), r.top());		
		break;
		
	case circleIOHori:
		path.moveTo(r.left(), (r.top()+r.bottom()) / 2);
		path.lineTo(r.left()+5, (r.top()+r.bottom()) / 2);
		path.moveTo(r.right(), (r.top()+r.bottom()) / 2);
		path.lineTo(r.right()-5, (r.top()+r.bottom()) / 2);

		path.addEllipse(myRect);
		break;

	case circleIOVert:
		path.moveTo((r.left()+r.right())/2, r.top());
		path.lineTo((r.left()+r.right())/2, r.top()+5);
		path.moveTo((r.left()+r.right())/2, r.bottom());
		path.lineTo((r.left()+r.right())/2, r.bottom()-5);

		path.addEllipse(myRect);
		break;
		
	case donutIOHori:
		path.moveTo(r.left(), (r.top()+r.bottom()) / 2);
		path.lineTo(r.left()+5, (r.top()+r.bottom()) / 2);
		path.moveTo(r.right(), (r.top()+r.bottom()) / 2);
		path.lineTo(r.right()-5, (r.top()+r.bottom()) / 2);

		path.addEllipse(myRect);
		myRect.setWidth(r.width() - 15);
		myRect.setHeight(r.height() - 15);
		myRect.moveCenter(r.center());
		path.addEllipse(myRect);
		break;

	case donutIOVert:
		path.moveTo((r.left()+r.right())/2, r.top());
		path.lineTo((r.left()+r.right())/2, r.top()+5);
		path.moveTo((r.left()+r.right())/2, r.bottom());
		path.lineTo((r.left()+r.right())/2, r.bottom()-5);

		path.addEllipse(myRect);
		myRect.setWidth(r.width() - 15);
		myRect.setHeight(r.height() - 15);
		myRect.moveCenter(r.center());
		path.addEllipse(myRect);
		break;
	
	case doubleEllipse: 
		myRect = r;
		myRect.moveTo(r.x()+5, r.y()+5);
		p.addEllipse(myRect);		
		path.addEllipse(r);
		p = p.subtracted(path);
		path.addPath(p);
		break;

	case underlinedDoubleEllipse:
		myRect = r;
		myRect.moveTo(r.x()+5, r.y()+5);
		p.addEllipse(myRect);		
		path.addEllipse(r);
		p = p.subtracted(path);
		path.addPath(p);

		w = (r.right() - r.left())/2;
		h = (r.bottom() - r.top())/2;
		y = r.bottom() - 5;
		x = sqrt( w*w * (1 - y*y / (h*h) ) );

		path.moveTo( -x, y );
		path.lineTo( +x, y );
		break;

	case halfCurvyRectangle:
		myRect = r;
		myRect.setY(r.y()-30);
		
		path.addRoundRect (myRect, 50);

		myRect = r;
		myRect.setY(r.y()-30);
		myRect.setHeight(30);

		p.addRect(myRect);
		
		path = path.subtracted(p);

		break;

	case underlinedHalfCurvyRectangle:
		myRect = r;
		myRect.setY(r.y()-30);
		
		path.addRoundRect (myRect, 50);

		myRect = r;
		myRect.setY(r.y()-30);
		myRect.setHeight(30);

		p.addRect(myRect);
	
		path = path.subtracted(p);

		abs = (r.right() - r.left())/2 - 3;
		ord = r.bottom() - 5;

		path.moveTo( -abs, ord );
		path.lineTo( +abs, ord );
		break;

	case hexagon:
		path.moveTo(r.left()+10, r.top());
		path.lineTo(r.right()-10, r.top());
		path.lineTo(r.right(), (r.top()+r.bottom())/2);
		path.lineTo(r.right()-10, r.bottom());
		path.lineTo(r.left()+10, r.bottom());
		path.lineTo(r.left(), (r.top()+r.bottom())/2);
		path.lineTo(r.left()+10, r.top());
		break;

	case underlinedHexagon:
		path.moveTo(r.left()+10, r.top());
		path.lineTo(r.right()-10, r.top());
		path.lineTo(r.right(), (r.top()+r.bottom())/2);
		path.lineTo(r.right()-10, r.bottom());
		path.lineTo(r.left()+10, r.bottom());
		path.lineTo(r.left(), (r.top()+r.bottom())/2);
		path.lineTo(r.left()+10, r.top());
		path.moveTo(r.left()+6, r.bottom()-5);
		path.lineTo(r.right()-6, r.bottom()-5);
		break;

	case concaveHex:
		path.moveTo(r.left()-10, r.top());
		path.lineTo(r.right()+10, r.top());
		path.lineTo(r.right(), (r.top()+r.bottom())/2);
		path.lineTo(r.right()+10, r.bottom());
		path.lineTo(r.left()-10, r.bottom());
		path.lineTo(r.left(), (r.top()+r.bottom())/2);
		path.lineTo(r.left()-10, r.top());
		break;

	case underlinedConcaveHex:
		path.moveTo(r.left()-10, r.top());
		path.lineTo(r.right()+10, r.top());
		path.lineTo(r.right(), (r.top()+r.bottom())/2);
		path.lineTo(r.right()+10, r.bottom());
		path.lineTo(r.left()-10, r.bottom());
		path.lineTo(r.left(), (r.top()+r.bottom())/2);
		path.lineTo(r.left()-10, r.top());
		path.moveTo(r.left()-6, r.bottom()-5);
		path.lineTo(r.right()+6, r.bottom()-5);
		break;
	}
	
	return path;
}

/*************************************************************
* updatePos: lets the Item's pos mirror the Clone's position *
**************************************************************/
void VertexGraphics::setPosition() { this->setPos(this->cloneDescriptor->x(), this->cloneDescriptor->y()); }
void VertexGraphics::updatePos() { if ( (this->cloneDescriptor->x() != this->pos().x()) || (this->cloneDescriptor->y() != this->pos().y()) ) this->setPosition(); }

void VertexGraphics::updateStyle(VertexStyle * s)
{
	this->setStyle(s);
}

// [!] doesn't do anything ATM
QVariant VertexGraphics::itemChange(GraphicsItemChange change, const QVariant &value) {
	if (change == QGraphicsItem::ItemPositionHasChanged)
	{
//		this->graphScene->updateMovingVertices();
//		this->graphScene->updateMyLayout(true, true);		

//		this->cloneDescriptor->setPosition(this->scenePos().x(), this->scenePos().y());
/*
		if (this->getContainer()) this->getContainer()->layoutEdges();		
*/
/*
		// notifies its own layout manager
		if (this->getContainer()) this->getContainer()->dealWithChange(this);

		// also notify edges that they should be updated
		std::list<EdgeItem *> updateList = this->getEdges();
		for (std::list<EdgeItem *>::iterator it = updateList.begin(); it != updateList.end(); ++it) {
			(*it)->sourceOrTargetMoved();
		}
*/
	}
	
/*
	if (change == QGraphicsItem::ItemSelectedChange || change == QGraphicsItem::ItemSelectedHasChanged)
	{
		std::list<Connector*> cList = this->getCloneContent()->getOutterConnectors();
		for (std::list<Connector*>::iterator it = cList.begin(); it != cList.end(); ++it)
		{
			EdgeGraphics * eg = this->graphScene->getEdgeGraphics(*it);		
			eg->setSelected(value.toBool()); // I give up on the edge selection
			
//			std::cout << value.toBool() << std::endl;
		}
	}
*/
	return QGraphicsItem::itemChange(change, value);
}

void VertexGraphics::updateClonePosition()
{
	this->cloneDescriptor->setPosition(this->scenePos().x(), this->scenePos().y());
}