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
 *  ConnectorLayoutManager.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 12/05/2008.
 *  Last documented never.
 *
 */

#include "connectorlayoutmanager.h"

#include <libavoid/libavoid.h>
#include <utility>

#include "clonecontent.h"
#include "connector.h"
#include "graphmodel.h"
#include "vertexproperty.h"

void ConnectorLayoutManager::layout(std::list<Connector*> connectors, std::list<CloneContent*> cloneDescriptors)
{
	if (cloneDescriptors.empty())
	{
		for (std::list<Connector*>::iterator it = connectors.begin(); it != connectors.end(); ++it)
		{
			Connector *edge = *it;
			std::list< std::pair <int, int> > controlPoints;
			controlPoints.push_back(edge->getPoint(true));
			controlPoints.push_back(edge->getPoint(false));
			edge->setPoints(controlPoints);
		}
	}
	else
	{
		Avoid::Router *router = new Avoid::Router();
		int objectCount = 0;
	
		for (std::list<CloneContent*>::iterator it = cloneDescriptors.begin(); it != cloneDescriptors.end(); ++it)
		{
			CloneContent * v = *it;
		
			Avoid::Polygn shapePoly = Avoid::newPoly(4);
			shapePoly.ps[0]= Avoid::Point(v->left(true), v->top(true));
			shapePoly.ps[1]= Avoid::Point(v->right(true), v->top(true));
			shapePoly.ps[2]= Avoid::Point(v->right(true), v->bottom(true));
			shapePoly.ps[3]= Avoid::Point(v->left(true), v->bottom(true));

			Avoid::ShapeRef *shapeRef = new Avoid::ShapeRef(router, ++objectCount, shapePoly);
			Avoid::freePoly(shapePoly);		
			router->addShape(shapeRef);
		}
	
		for (std::list<Connector*>::iterator it = connectors.begin(); it != connectors.end(); ++it)
		{
			Connector * e = *it;	

//			if (this->graphModel->getProperties(e)->getTypeLabel() == "Modifier") continue;

			Avoid::Point srcPoint = Avoid::Point(e->getPoint(true).first, e->getPoint(true).second);
			Avoid::Point tarPoint = Avoid::Point(e->getPoint(false).first, e->getPoint(false).second);

			Avoid::ConnRef *connRef = new Avoid::ConnRef(router, ++objectCount, srcPoint, tarPoint);
			connRef->updateEndPoint(Avoid::VertID::src, srcPoint);
			connRef->updateEndPoint(Avoid::VertID::tar, tarPoint);	
			connRef->generatePath();
		
			std::list< std::pair <int, int> > controlPoints;
			Avoid::Polygn route = connRef->route();
			for (int i=0; i<route.pn; ++i) controlPoints.push_back(std::pair<int,int>(route.ps[i].x, route.ps[i].y));

			e->setPoints(controlPoints);
		}
	
/*
		for (std::list<Connector*>::iterator it = connectors.begin(); it != connectors.end(); ++it)
		{
			Connector * e = *it;	

			if (this->graphModel->getProperties(e)->getTypeLabel() == "Modifier") continue;

			float dx = e->getPoint(true).first - e->getPoint(false).first;
			float dy = e->getPoint(true).second - e->getPoint(false).second;
			
			float x1, x2, x3, x4, y1, y2, y3, y4;
			x1 = x2 = x3 = x4 = y1 = y2 = y3 = y4 = 0;
			if (dx*dx > dy*dy)
			{
				x1 = e->getPoint(true).first;
				x2 = e->getPoint(false).first;
				x3 = e->getPoint(false).first;
				x4 = e->getPoint(true).first;
				y1 = e->getPoint(true).second+1;
				y2 = e->getPoint(false).second+1;
				y3 = e->getPoint(false).second-1;
				y4 = e->getPoint(true).second-1;
			}	
			else
			{
				x1 = e->getPoint(true).first+1;
				x2 = e->getPoint(false).first+1;
				x3 = e->getPoint(false).first-1;
				x4 = e->getPoint(true).first-1;
				y1 = e->getPoint(true).second;
				y2 = e->getPoint(false).second;
				y3 = e->getPoint(false).second;
				y4 = e->getPoint(true).second;
			}

			Avoid::Polygn shapePoly = Avoid::newPoly(4);
			shapePoly.ps[0]= Avoid::Point(x1, y1);
			shapePoly.ps[1]= Avoid::Point(x2, y2);
			shapePoly.ps[2]= Avoid::Point(x3, y3);
			shapePoly.ps[3]= Avoid::Point(x4, y4);

			Avoid::ShapeRef *shapeRef = new Avoid::ShapeRef(router, ++objectCount, shapePoly);
			Avoid::freePoly(shapePoly);		
			router->addShape(shapeRef);
		}
		
		for (std::list<Connector*>::iterator it = connectors.begin(); it != connectors.end(); ++it)
		{
			Connector * e = *it;	

			if (this->graphModel->getProperties(e)->getTypeLabel() != "Modifier") continue;

			Avoid::Point srcPoint = Avoid::Point(e->getPoint(true).first, e->getPoint(true).second);
			Avoid::Point tarPoint = Avoid::Point(e->getPoint(false).first, e->getPoint(false).second);

			Avoid::ConnRef *connRef = new Avoid::ConnRef(router, ++objectCount, srcPoint, tarPoint);
			connRef->updateEndPoint(Avoid::VertID::src, srcPoint);
			connRef->updateEndPoint(Avoid::VertID::tar, tarPoint);	
			connRef->generatePath();
		
			std::list< std::pair <int, int> > controlPoints;
			Avoid::Polygn route = connRef->route();
			for (int i=0; i<route.pn; ++i) controlPoints.push_back(std::pair<int,int>(route.ps[i].x, route.ps[i].y));

			e->setPoints(controlPoints);
		}
	*/
		delete router;
	}
}