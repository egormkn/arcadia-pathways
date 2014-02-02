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
 *  ContainerGraphics.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 10/04/2008.
 *  Last documented on 17/04/2008.
 *
 */

#include "containergraphics.h"

#include <iostream>

// Qt painting class
#include <QPainter>

// local
#include "containercontent.h";
#include "containerstyle.h";

#include <twines/include/twine.h>

/**************
* Constructor *
***************
* The represented container is set up
* Since [!] there is no style
* the padding, border and margin are set to arbitrary values
* and from there (and the Container's dimensions) are computed
* the paintingShape, boundingRect and boundingRect
* The pos is also set to mirror the Container's position
* [!] overall problem: these value are not updated!!!
**************************************************************/
ContainerGraphics::ContainerGraphics(ContainerContent * c, ContainerStyle * s) : container(c)
{
	this->setStyle(s);
	this->setPosition();
}

void ContainerGraphics::setPosition()
{
	this->setPos(this->container->x(), this->container->y());
//	std::cout << "Pos " << this->x() << " " << this->y() << std::endl;
}

void ContainerGraphics::setStyle(ContainerStyle *s)
{
	this->style = s? s: ContainerStyle::GetDefaultStyle();
	this->updateShapesAndRect();
}

void ContainerGraphics::updateShapesAndRect()
{
	if (!this->style) { this->setStyle(); return; }

	QPointF center(0, 0); // the geometric center of the shape, in the container referential

	center.setX((this->container->left(true)+this->container->right(true)) / 2 - this->container->x());
	center.setY((this->container->top(true)+this->container->bottom(true)) / 2 - this->container->y());

	// the rectangle bounding the shape, including margins, in the container referential
	this->fullRect.setWidth(this->container->width(true));
	this->fullRect.setHeight(this->container->height(true));
	this->fullRect.moveCenter(center);

/*	
	std::cout << "Width " << this->container->width(true) << std::endl;	
	std::cout << "Height " << this->container->height(true) << std::endl;	

	std::cout << "Hori " << this->fullRect.left() << " " << this->fullRect.right() << std::endl;	
	std::cout << "Vert " << this->fullRect.top() << " " << this->fullRect.bottom() << std::endl;	
*/

	// the bounding rectangle does not include margins, just padding and border
	this->boundingRectangle = this->getRect(this->fullRect, this->style->getBoundingOffset() - this->style->getFullOffset());
	// the bounding shape, for convenience, matches the bounding rectangle
	this->boundingShape = this->getShape(this->boundingRectangle);
	
	// the painting shape, however, depends on the container style
/*
	if (this->style->shape == concaveHullContainer)
		this->paintingShape = this->getShape();	// this provides a concave hull
	else // by default it's a rectangle that will fit right inside the bounding rectangle, considering the border width
*/
		this->paintingShape = this->getShape(this->getRect(this->fullRect, this->style->getPaintingOffset() - this->style->getFullOffset()));
}

QRectF ContainerGraphics::getRect(const QRectF r, const int offset) const
{
	QRectF rect = r;
	rect.setWidth(r.width() + offset);
	rect.setHeight(r.height() + offset);
	rect.moveCenter(r.center());
	return rect;
}

QPainterPath ContainerGraphics::getShape(const QRectF r) const
{
	QPainterPath path;
	path.addRect(r);
	return path;
}


// [!] concave hull attempt
QPainterPath ContainerGraphics::getShape() const
{
	QPainterPath path;
/*	
	QList<QPointF> points;
	std::list< Content* > cList = this->container->getChildren();
	for (std::list<Content *>::iterator it = cList.begin(); it != cList.end(); ++it)
	{
		Content * c = (*it);		
		QRectF r(QPointF(c->left(true) - this->container->x(), c->top(true) - this->container->y()), QPointF(c->right(true)- this->container->x(), c->bottom(true) - this->container->y()));
		points.append(r.topLeft());
		points.append(r.topRight());
		points.append(r.bottomLeft());
		points.append(r.bottomRight());
	}

	int k = 3;
	QPolygonF skeleton = concaveHull(points, k);

    // For each skeleton point, create required control points of hull
    QPolygonF hull;
    for (int i = 0; i < skeleton.size() - 1; ++i)
    {
        QPointF prevPoint = (i == 0 ? skeleton.at(skeleton.size() - 2) : skeleton.at(i - 1));
        QPointF currPoint = skeleton.at(i);
        QPointF nextPoint = (i == skeleton.size() - 2 ? skeleton.first() : skeleton.at(i + 1));
        
        float y1= prevPoint.y() - currPoint.y();
        float x1= prevPoint.x() - currPoint.x();
        float angle1= atan2(y1,x1) * RAD2DEG;

        float y2= nextPoint.y() - currPoint.y();
        float x2= nextPoint.x() - currPoint.x();
        float angle2= atan2(y2,x2) * RAD2DEG;

        float angle= angle2 - angle1;
        if (angle < 0) angle += 360;

        if (angle >= 0 && angle <= 180)
        {
            printf("convex\n");
            QPointF offset(y1, -x1);
            offset /= sqrt(offset.x()*offset.x() + offset.y()*offset.y());
            offset *= 30.0;
            hull.append(currPoint + offset);
            
            offset = QPointF(-y2, x2);
            offset /= sqrt(offset.x()*offset.x() + offset.y()*offset.y());
            offset *= 30.0;
            hull.append(currPoint + offset);
        }
        else
        {
            printf("concave\n");
            QPointF p1(x1, y1);
            p1 /= sqrt(p1.x()*p1.x() + p1.y()*p1.y());
            QPointF p2(x2, y2);
            p2 /= sqrt(p2.x()*p2.x() + p2.y()*p2.y());
            QPointF offset = p1 + p2;
            offset /= sqrt(offset.x()*offset.x() + offset.y()*offset.y());
            offset *= 30.0;
            hull.append(currPoint + offset);
        }
    }
    if (hull.size() > 0)
    {
        hull.append(hull.first());
    }
    printf("\n");

	
	TW_TWINE xTwine;
	TW_TWINE yTwine;
	
	TW_InitialiseTwine(&xTwine, TW_TYPE_Repeat);
	TW_InitialiseTwine(&yTwine, TW_TYPE_Repeat);
	
	QPolygonF polygon;
    for (int i = 0; i < hull.size(); ++i)
    {
        TW_AddControl(xTwine, i, hull.at(i).x());
        TW_AddControl(yTwine, i, hull.at(i).y());
		polygon << hull.at(i);
    }

	QPolygonF smooth;
    for (float i = 0; i < hull.size(); i+=0.1)
    {
		double x, y;	
		TW_EvaluateTwine(&x, xTwine, i);
		TW_EvaluateTwine(&y, yTwine, i);
        smooth << QPointF(x,y);
    }
	
	TW_DeleteTwine(xTwine);
	TW_DeleteTwine(yTwine);

	path.addPolygon(polygon);
*/
	return path;
}

// [!] I update the pos even when not necessary => performance loss...
void ContainerGraphics::updatePos()
{
	if ((this->pos().x() != this->container->x()) || (this->pos().y() != this->container->y()))
	{
// old stuff
//		std::cout << "yatta!" << " " << this->pos().x() << " " << this->pos().y() << "\t" << this->container->x() << " " << this->container->y() << std::endl;
		this->setPosition();
	}

	float L, R, T, B;
	L = this->fullRect.left() + this->pos().x();
	R = this->fullRect.right() + this->pos().x();
	T = this->fullRect.top() + this->pos().y();
	B = this->fullRect.bottom() + this->pos().y();

// old stuff
/*
	std::cout << this->pos().x() << " " << this->pos().y() << std::endl;
	std::cout << L << " " << R << " " << T << " " << B << std::endl;
*/

	if (L != this->container->left(true) || R != this->container->right(true) || T != this->container->top(true) || B != this->container->bottom(true))
	{
// old stuff
//		std::cout << "ploup..." << " " << L - this->container->left() << " " <<  R - this->container->right() << "\t" << T - this->container->top() << " " << B - this->container->bottom() << std::endl;
		this->updateShapesAndRect();
	}

// old stuff
/*
	if ((this->fullRect.width() != this->container->width()) || (this->fullRect.height() != this->container->height()))
	{
		this->updateShapesAndRect();
	}
	else if (this->style->shape == concaveHullContainer)
	{
		this->updateShapesAndRect();	
	}
*/
}

void ContainerGraphics::setUpdateFlag()
{
	this->container->setUpdateFlag();
}

/**********************************************
* boundingRect: returns the boundingRectangle *
**********************************************/	
QRectF ContainerGraphics::boundingRect() const { return this->boundingRectangle; }
/***********************************
* shape: returns the boundingShape *
***********************************/	
QPainterPath ContainerGraphics::shape() const { return this->boundingShape; }

/********
* paint *
*********
* Sets up the pen to a fixed width (default color black)
* then paints the painting shape
********************************************************/	
void ContainerGraphics::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	if (!this->style->isVisible()) return;

	QPainterPath shape = this->paintingShape;
	std::string label = this->container->getLabel();

	QColor c(this->style->red(), this->style->green(), this->style->blue());
	Qt::PenStyle ps = this->style->getContinuousLine()? Qt::SolidLine: Qt::DashLine;

	QPen p;
	p.setColor(c);
	p.setStyle (ps);
	p.setWidth( this->style->getBorder() );
	painter->setPen(p);
	if (!this->container->isHidden()) painter->drawPath(shape);
	
	if (label == "") return;

	QRectF r = this->getRect(this->fullRect, -this->style->getFullOffset());
	painter->drawText(r, Qt::AlignLeft || Qt::AlignTop, label.c_str());
}