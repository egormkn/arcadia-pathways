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
 *  ListGraphView.h
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#ifndef LISTGRAPHVIEW_H
#define LISTGRAPHVIEW_H

// STL
#include <map>

// Qt base class
#include <QWidget>

// local base class
#include "graphview.h"

class QTreeWidget;
class QTreeWidgetItem;
class QAction;
class QMenu;
class QCheckBox;

/**************
* ListGraphView *
***************
* This subclass of GraphView displays the GraphModel's elements in a QTreeView
* It keeps a map from Vertex to TreeWidgetItems and conversely
* When displaying a Model, the Tree gets populated with Vertices and Edges
* [!] except ATM I don't display edges!
* [!] also I should probably put different vertex types in different branches
* The View notifies its Controller when the vertex selection changes
* or when a vertex gets double clicked (toggles cloning)
* Also, it gets notified by the Controller of which Vertices to select
******************************************************************************/
class ListGraphView : public QWidget, public GraphView
{
	Q_OBJECT
	
public:
	ListGraphView(GraphController * c, QWidget * parent = NULL);

	void display(GraphModel *gModel);

	void select(std::list<BGL_Vertex> vList);
	void selectLayout(GraphLayout * gl);

protected:
	void contextMenuEvent(QContextMenuEvent * event);

private slots:
	void changeVertexSelection();
	void doubleClickVertex(QTreeWidgetItem * item, int column);
	void switchToLocalView();
	void createNewView();
	void expandCurrentView();
	void removeFromView();
	void moveToNewView();
	void executeAction(QAction * action);
	void updateTree();
	
private:
	GraphLayout * graphLayout;

	QTreeWidget * treeWidget;
	QCheckBox * toggleView;

	void addVertex(BGL_Vertex v);
	void addEdge(BGL_Edge e);
		
	std::map<BGL_Vertex, QTreeWidgetItem*> vertexToListItem;
	std::map<QTreeWidgetItem*, BGL_Vertex> listItemToVertex;

	std::map<std::string, QTreeWidgetItem*> classToListItem;
	
	QAction * createAction(const char * name, const char * shortCut="", const char * tip="");
	void createMenu();
	QMenu * menu;
};

#endif