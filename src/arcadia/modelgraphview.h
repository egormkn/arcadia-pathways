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
 *  ModelGraphView.h
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#ifndef MODELGRAPHVIEW_H
#define MODELGRAPHVIEW_H

#include <iostream>

// Qt base class
#include <QFrame>

// local base class
#include "graphview.h"

// Qt inner components
#include <QGraphicsView>
class QSlider;
class QTabWidget;

// local inner component
class LayoutGraphView;

class MapView : public QGraphicsView
{
/*
protected:
	void mouseDoubleClickEvent ( QMouseEvent * event )
	{
		QGraphicsView::mouseDoubleClickEvent(event);
	}
*/
};

/***********
* ModelGraphView *
************
* This subclass of GraphView and QFrame displays a GraphModel
* through a list of LayoutGraphView on which it acts as a viewport
* [!] does it make sense for this class to be a GraphView??
* [!] does it need the Model and the Controller?
* [!] don't think it will send notifications at least...
* [!] should this belong to a NonInterativeGraphView class?
*
* As a QFrame, the ModelGraphView contains
* - a QGraphicsView for displaying the current LayoutGraphView
* - a QSlider to set up the zoom level (+Buttons for that too)
* - a QTabWidget to switch between LayoutGraphViews
*
* The constructor sets up the View's Controller and QFrame's parent
* and indicates whether the zoom should be enabled or not
*
* When a Model gets displayed, previous Scenes get destroyed
* and for every layout a Scene is created to display it
*
* The ModelGraphView can export the current graphics
* by telling every scene to do so
* [!] is that actually useful??? Can't the scenes be called directly?
*
* In term of internal invents, the ModelGraphView supports
* zooming (and overall resetting), scene switching, and also recentering
************************************************************************/
class ModelGraphView : public QFrame, public GraphView
{
	Q_OBJECT

/*	
protected:
	void mouseDoubleClickEvent ( QMouseEvent * event )
	{
		QFrame::mouseDoubleClickEvent(event);
	}
*/
	
public:
	ModelGraphView(GraphController * c, bool zoomEnabled = true, QWidget * parent = NULL);

	void display(GraphModel *gModel);
	
	void exportGraphics(std::string filename);
	
	void layoutGotAdded();
	void layoutGotRemoved(GraphLayout * gl);
	void selectLayout(GraphLayout * gl);
	
	LayoutGraphView * getLayoutView(int i=0);
	MapView * getLayoutWidget(int i=0);
	
private slots:
	void sceneSwitched(int scene = -1);
	void layoutDestroyed();
	
private:	
	void switchScene(int scene = -1);

	QTabWidget * tabWidget;
	bool zoom;
	int currentScene;

	void displayLayout(int l);
	void removeLayout(int l);
	
	bool resetting;
};

class TabbedWidget : public QWidget
{
	Q_OBJECT

public:
	TabbedWidget(QWidget * parent = NULL, bool zoomEnabled = true);
	~TabbedWidget();
	void setScene(GraphController * c, int l);
	LayoutGraphView * getScene() { return this->scene; }
	MapView * getView() { return this->mapView; }

private slots:
    void zoomIn();
    void zoomOut();	
    void setupMatrix();
	
	void changeCenter(QPointF p);

	void toggleAvoidingEdges();
	void destroyLayout();
	void updateLayout();
	void cloneModifiers();
	void arrangeSelection();
	void newLayout();
	
	void left();
	void farLeft();
	void right();
	void farRight();
		
private:
	bool busy;
	
	bool modCloned;

	void resetView();

    MapView * mapView;
    QSlider * zoomSlider;
	LayoutGraphView * scene;
	
	QAction * createAction(const char * name, const char * shortCut, const char * tip);
};

#endif