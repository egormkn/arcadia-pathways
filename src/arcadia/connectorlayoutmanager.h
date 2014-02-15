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
 *  ConnnectorLayoutManager.h
 *  arcadia
 *
 *  Created by Alice Villeger on 12/05/2008.
 *  Last documented never.
 *
 */

#ifndef CONNECTORLAYOUTMANAGER_H
#define CONNECTORLAYOUTMANAGER_H

#include <list>
#include <libavoid/libavoid.h>

#include <QThread>

class Connector;
class CloneContent;
class GraphLayout;
class QProgressDialog;

/*************************
* ConnectorLayoutManager *
**************************/
class ConnectorLayoutManager : public QThread
{
public:
	ConnectorLayoutManager(GraphLayout * gl);
	~ConnectorLayoutManager();
	void layout();

protected:
	int totalCleanUpSteps;
	int currentCleanUpSteps;

	void run();
	
	void init();
	void process();
	void clear();

	GraphLayout * graphLayout;
	std::list<Connector*> connectors;
	std::list<CloneContent*> cloneDescriptors;

	Avoid::Router * router;
	std::map< CloneContent *, Avoid::ShapeRef * > myShapeList;
	std::map< Connector *, Avoid::ConnRef * > myConnList;	

	QProgressDialog * pd;
};

#endif