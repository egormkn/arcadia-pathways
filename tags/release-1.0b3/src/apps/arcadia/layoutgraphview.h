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
 *  LayoutGraphView.h
 *  arcadia
 *
 *  Created by Alice Villeger on 27/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#ifndef LAYOUTGRAPHVIEW_H
#define LAYOUTGRAPHVIEW_H

// Qt base class
#include <QGraphicsScene>

// STL
#include <map>
#include <list>

// local base class
#include "graphview.h"

// local
class VertexGraphics;
class EdgeGraphics;
class ContainerGraphics;
class CloneContent;
class ContainerContent;
class Connector;

/*************
* LayoutGraphView *
**************
* This subclass of QGraphicsScene and GraphView displays
* graphically a given GraphLayout (cf. number) of a GraphModel
*
* That display can be exported as a set of graphic files
* [!] a bit "raw" ATM, not OK for public release
*
* The Controller can notify the View of
* changes in selection or clones
* 
* The View itself can send a signal when a new center is selected
* (that signal is read by the ModelGraphView)
*
* Internally, a map is kept between
* each Vertex and its VertexGraphics representations
*
* Specific methods let display a Vertex, Edge and Container tree
* as well as remove a Vertex
* [!] not used ATM, the entire view is reset when, eg, cloning happens
*
* Specific action is undertaken (sending signals to the Controller)
* when the selection changes or a double click is observed
**********************************************************************/
class LayoutGraphView : public QGraphicsScene, public GraphView
{
	Q_OBJECT
	
public:
	LayoutGraphView(GraphController * c, unsigned int lNumber);
	
	void display(GraphModel * gModel);
	void display();
	
	void exportGraphics(std::string filename);
	
	void select(std::list<BGL_Vertex> vList);
	void cloningGotToggled(BGL_Vertex v, GraphLayout * gl);
	void layoutGotUpdated(GraphLayout * gl, bool edgesOnly, bool fast);
	void containerVisibilityGotToggled();
	
	// Handles manual placement of nodes and resulting changes in the automatic placement of edges
	void mousePressEvent (QGraphicsSceneMouseEvent * event );
	void mouseMoveEvent (QGraphicsSceneMouseEvent * event );
	void mouseReleaseEvent (QGraphicsSceneMouseEvent * event );	

	void setVisible(bool visible);
	
	std::list<VertexGraphics *> getVertexGraphics(BGL_Vertex v);
	VertexGraphics * getVertexGraphics(CloneContent * c);	

	EdgeGraphics * getEdgeGraphics(Connector * c);	

	GraphLayout * getLayout() { return this->layout; }

signals:
	void newCenter(QPointF center);
	
protected:
	void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * mouseEvent );
	void contextMenuEvent( QGraphicsSceneContextMenuEvent *event );
	void keyPressEvent ( QKeyEvent * keyEvent );

private slots:
	void changeVertexSelection();
	
private:
	bool supersize;

	void displayVertex(BGL_Vertex v);
	void displayEdge(BGL_Edge e);
	void displayContainerTree(ContainerContent *c);
	
	void removeVertex(BGL_Vertex v);
	
	void resize();

	GraphLayout * layout;
	
	std::list<VertexGraphics*> vertices;
	std::list<EdgeGraphics*> edges;
	std::list<ContainerGraphics*> containers;
	std::map<BGL_Vertex, std::list<VertexGraphics*> > vertexToGraphics;

	std::map<Connector *, EdgeGraphics * > connectorToGraphics;
	std::list<EdgeGraphics *> inList;
	std::list<EdgeGraphics *> outList;
	
	bool mouseDown;
	bool moving;
	bool moved;
	bool selecting;
	bool saveAvoidingValue;

public:
	void updateMovingVertices();
	void updateMyLayout(bool edge, bool fast);
};

#endif