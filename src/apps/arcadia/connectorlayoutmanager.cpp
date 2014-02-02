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

#include <QProgressDialog>
#include <QApplication>

#include <ctime>

#include "clonecontent.h"
#include "connector.h"
#include "graphlayout.h"

ConnectorLayoutManager::ConnectorLayoutManager(GraphLayout * gl) : graphLayout(gl), router(NULL)
{
	if (this->graphLayout->isAvoiding()) this->init();
	this->totalCleanUpSteps = 0;
}

ConnectorLayoutManager::~ConnectorLayoutManager()
{
	if (!this->isRunning()) return; // to avoid displaying the progress bar needlessly

 	pd = new QProgressDialog("Cleaning up edge routing memory (can't be interrupted)", QString(), 0, this->totalCleanUpSteps);
	pd->setWindowModality(Qt::ApplicationModal);
    pd->show();

	while (this->isRunning())
	{
		pd->setValue(this->currentCleanUpSteps);
		pd->show();
	}

	pd->setValue(this->totalCleanUpSteps);
	pd->show();
	delete pd;
}

// This can be time consuming, but should not be interrupted (in order to preserve memory integrity)
// So it is run in a background thread...
void ConnectorLayoutManager::clear()
{
/*
	time_t start = time(NULL);
*/
	// Time required to clean up connectors is negligeable
	this->totalCleanUpSteps = this->myShapeList.size(); // + this->myConnList.size();
	this->currentCleanUpSteps = 0;

	// Because the router doesn't do it upon its own destruction!
	if (!this->myShapeList.empty())
	{
		for (std::map< CloneContent *, Avoid::ShapeRef * >::iterator it = this->myShapeList.begin(); it != this->myShapeList.end(); ++it)
		{
			router->delShape(it->second); // This is particularly time consuming
			// Unfortunately, simply deleting the shape object would result in memory unstability
			this->currentCleanUpSteps++;
		}
		this->myShapeList.clear();
	}

	if (!this->myConnList.empty())
	{
		for (std::map< Connector *, Avoid::ConnRef * >::iterator it = this->myConnList.begin(); it != this->myConnList.end(); ++it)
		{
			delete it->second;
			// Time required to clean up connectors is negligeable
//			this->currentCleanUpSteps++;
		}
		this->myConnList.clear();
	}

	if (this->router) { delete this->router; this->router = NULL; }

/*	
	time_t end = time(NULL);
	double t = difftime(end, start);
	if (t) std::cout << "Cleaning " << t << std::endl;	
*/
}

// Assumes a clear operation has already be performed if need be
void ConnectorLayoutManager::init()
{
/*
	time_t start = time(NULL);
*/
	this->connectors = this->graphLayout->getConnectors();
	this->cloneDescriptors = this->graphLayout->getCloneContents();

	// Initialize libavoid objects
	this->router = new Avoid::Router();	
	int objectCount = 0;

	pd->setLabelText("Edge routing in progress... Initializing shapes");
	pd->show();

/*	
	int current = 0;
	int total = cloneDescriptors.size();
*/
	// Each clone in the layout becomes a rectangular shape at a certain position
	for (std::list<CloneContent*>::iterator it = cloneDescriptors.begin(); it != cloneDescriptors.end(); ++it)
	{
		QApplication::processEvents();
		if (pd->wasCanceled()) break;

		CloneContent * v = *it;
		
		Avoid::Polygn shapePoly = Avoid::newPoly(4);
		shapePoly.ps[0]= Avoid::Point(v->left(true), v->top(true));
		shapePoly.ps[1]= Avoid::Point(v->right(true), v->top(true));
		shapePoly.ps[2]= Avoid::Point(v->right(true), v->bottom(true));
		shapePoly.ps[3]= Avoid::Point(v->left(true), v->bottom(true));

		Avoid::ShapeRef * shapeRef = new Avoid::ShapeRef(router, ++objectCount, shapePoly);
		Avoid::freePoly(shapePoly);		
		router->addShape(shapeRef);
			
		myShapeList[v] = shapeRef;

		pd->setValue( pd->value() + 1 );
		pd->show();
	
/*
		current++;
		float progress = (100.0 * current) / progress;
		pd->setLabelText("Edge routing in progress... Initializing shapes: " + progress + "%");
*/
	}

	pd->setLabelText("Edge routing in progress... Initializing connectors");
	pd->show();

/*
	current = 0;
	total = connectors.size();
*/
	// Each connector in the layout becames a connector from one point to another
	for (std::list<Connector*>::iterator it = connectors.begin(); it != connectors.end(); ++it)
	{
		QApplication::processEvents();
		if (pd->wasCanceled()) break;	

		Connector * e = *it;

		Avoid::Point srcPoint = Avoid::Point(e->getPoint(true).first, e->getPoint(true).second);
		Avoid::Point tarPoint = Avoid::Point(e->getPoint(false).first, e->getPoint(false).second);

		Avoid::ConnRef * connRef = new Avoid::ConnRef(router, ++objectCount, srcPoint, tarPoint);
		connRef->updateEndPoint(Avoid::VertID::src, srcPoint);
		connRef->updateEndPoint(Avoid::VertID::tar, tarPoint);	

		myConnList[e] = connRef;

		pd->setValue( pd->value() + 1 );
		pd->show();

/*
		current++;
		float progress = (100.0 * current) / progress;
		pd->setLabelText("Edge routing in progress... Initializing connectors: " + progress + "%");
*/
	}
	
/*
	time_t end = time(NULL);
	double t = difftime(end, start);
	if (t) std::cout << "Init " << t << std::endl;	
*/
}

// Assumes an initialization has already been performed. Can be interrupted
void ConnectorLayoutManager::process()
{
/*
	time_t start = time(NULL);
*/	

	pd->setLabelText("Edge routing in progress... Processing connectors");
	pd->show();

/*
	int current = 0;
	int total = connectors.size();
*/
	for (std::list<Connector*>::iterator it = this->connectors.begin(); it != this->connectors.end(); ++it)
	{
		QApplication::processEvents();
		if (pd->wasCanceled()) break;

		Connector * e = *it;
		Avoid::ConnRef * connRef = this->myConnList[e];

		connRef->generatePath();
		std::list< std::pair <int, int> > controlPoints;
		Avoid::Polygn route = connRef->route();
		for (int i=0; i<route.pn; ++i) controlPoints.push_back(std::pair<int,int>(route.ps[i].x, route.ps[i].y));
		e->setPoints(controlPoints);			

		pd->setValue( pd->value() + 1 );
		pd->show();

/*
		current++;
		float progress = (100.0 * current) / progress;
		pd->setLabelText("Edge routing in progress... Processing connectors: " + progress + "%");
*/
	}
/*	
	time_t end = time(NULL);
	double t = difftime(end, start);
	if (t) std::cout << "Process " << t << std::endl;	
*/
}

void ConnectorLayoutManager::layout()
{
	// we change the begin and end point of the edge (default connector layout)
	this->connectors = this->graphLayout->getConnectors();
	for (std::list<Connector*>::iterator it = this->connectors.begin(); it != this->connectors.end(); ++it)
	{
		Connector *edge = *it;
		std::list< std::pair <int, int> > controlPoints;
		controlPoints.push_back(edge->getPoint(true));
		controlPoints.push_back(edge->getPoint(false));			
		edge->setPoints(controlPoints);
	}	
	
	// If avoiding is on, we perform a process that can be more time consuming, and can be interrupted
	if (this->graphLayout->isAvoiding())
	{
		// Estimates the number of steps required:
		// number of nodes (init) + number of edges * 2 (init & process) + clean up steps (cf clear thread)
		int nConnectors = this->connectors.size();
		int nClones = this->graphLayout->getCloneContents().size();
		int nSteps = 2 * nConnectors + nClones + this->totalCleanUpSteps;

		// This modal dialog box indicates the progress of possibly costly operations being
		// performed in the main thread. The dialog box remains active, displays updates on progress
		// and can interrupt the main thread's tasks.
		// However, it is impossible to access other GUI elements while this dialog is active
	 	pd = new QProgressDialog("Edge routing in progress...", "Cancel", 0, nSteps);
		pd->setWindowModality(Qt::ApplicationModal);
	    pd->show();

		pd->setLabelText("Edge routing in progress... Cleaning up memory");
		pd->show();

		// We first need to wait for the previous clean up to be done with
		while (this->isRunning())
		{
			QApplication::processEvents();
			if (pd->wasCanceled()) break;

			pd->setValue(this->currentCleanUpSteps);
			pd->show();
/*
			float progress = (100.0*this->currentCleanUpSteps)/this->totalCleanUpSteps;
			pd->setLabelText("Edge routing in progress... Cleaning up memory: " + progress + "%"  );
*/
		}
		pd->setValue(this->totalCleanUpSteps);
		pd->show();

		// Pressing cancel won't cancel the clean up itself (running a background thread), just the
		// waiting for it to be finished. This means:
		// 1) init and process should be ignored (as cleaning is not finished yet and the user doesn't want to wait)
		// 2) a new cleaning process should NOT be started till that one is finished
		// 3) nevertheless, we still need to get rid of the dialog box

		pd->setLabelText("Edge routing in progress... Initializing");
		pd->show();

		// Init runs in the main thread, but can be interrupted by the dialog box
		if (!pd->wasCanceled()) this->init();
		// Running this in the main thread makes sense, as we don't want the users to change the
		// layout when we are busy initializing the libavoid router based on current layout data

		pd->setLabelText("Edge routing in progress... Processing");
		pd->show();

		// Process runs in the main thread, but can be interrupted by the dialog box
		if (!pd->wasCanceled()) this->process();
		// Running this in the main thread ensures that modifications performed on the connectors
		// at the end of the process are automatically taken into account by the app's display
		// The previous remark also applies: modification to the layout would render the current
		// computations meaningless... And synchronizing the GUI with libavoid is not 100% trivial
		// (ideally we may send more atomic updates instead of reinitializing all the libavoid data
		// every time we want to route edges... But I don't have the time to do that now) [!]
		// cf. commented out update method draft below (not interruptable or thread enabled!)
	
		// At that stage, we give back control to the main application and get rid of the dialog box
		pd->setValue(nSteps);
		pd->show();
		delete pd;
		// If we arrived there, it either means all the processing was completed, or we canceled it

		// In any case we need to run a clean up process in the background (only one at a time)
		if (!this->isRunning())
		{
			this->start();
		}
		// This clean up process can take time, but should NOT be interrupted - this would result
		// in very bad memory management. A full clean up is mandatory before we can start
		// to initialize and process another batch of data.
	}
}

// Runs in the background when the thread gets started
void ConnectorLayoutManager::run() { this->clear(); }

// Not currently used
// Instead of full reset then process then clear everytime...
// The approach below looks more optimal but is currently a bit broken? [!]
//	if (this->needsResetting) this->reset();
//	else this->update();

/*
void ConnectorLayoutManager::update()
{
	time_t start = time(NULL);	

	for (std::list<CloneContent*>::iterator it = this->cloneDescriptors.begin(); it != this->cloneDescriptors.end(); ++it)
	{
		CloneContent * v = *it;
		Avoid::ShapeRef * shapeRef = this->myShapeList[v];

	    Avoid::Polygn newPoly = Avoid::newPoly(4);
		newPoly.ps[0]= Avoid::Point(v->left(true), v->top(true));
		newPoly.ps[1]= Avoid::Point(v->right(true), v->top(true));
		newPoly.ps[2]= Avoid::Point(v->right(true), v->bottom(true));
		newPoly.ps[3]= Avoid::Point(v->left(true), v->bottom(true));

	    router->moveShape(shapeRef, &newPoly);
	    Avoid::freePoly(newPoly);	    
	}
    router->processMoves();
	
	for (std::list<Connector*>::iterator it = this->connectors.begin(); it != this->connectors.end(); ++it)
	{
		Connector * e = *it;			
		Avoid::ConnRef * connRef = this->myConnList[e];

		// If the node at either end just moved, we need to update that connector
		Avoid::Point srcPoint = Avoid::Point(e->getPoint(true).first, e->getPoint(true).second);
		Avoid::Point tarPoint = Avoid::Point(e->getPoint(false).first, e->getPoint(false).second);
		connRef->updateEndPoint(Avoid::VertID::src, srcPoint);
		connRef->updateEndPoint(Avoid::VertID::tar, tarPoint);

		connRef->generatePath();
		std::list< std::pair <int, int> > controlPoints;
		Avoid::Polygn route = connRef->route();
		for (int i=0; i<route.pn; ++i) controlPoints.push_back(std::pair<int,int>(route.ps[i].x, route.ps[i].y));
		e->setPoints(controlPoints);			
	}	
	
	time_t end = time(NULL);
	double t = difftime(end, start);
	if (t) std::cout << "Update " << t << std::endl;	
}
*/