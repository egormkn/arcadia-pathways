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
 *  GraphWindow.h
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Last documented on 17/04/2008.
 *
 */

#ifndef GRAPHWINDOW_H
#define GRAPHWINDOW_H
 
// Qt base class
#include <QMainWindow>

// local
class GraphController;

/****************
* GraphWindow *
*****************
* The Main Window, contains a GraphController
* Can be created either in full or light version
*
* It displays the views handled by the Controller:
* - the main graphic view, which is interactive only in the full version
* - additional views (element list, element properties) that only appear in the full version
*
* In the full version, it also displays a menu and a toolbar
* that transmit actions to the Controller, to be performed on the GraphModel it handles:
* loading a new graph, opening a graph from a file, saving a graph...
*
* The Controller's Model contains both basic graph data
* and specific layout infos (cloning, placement info, + placement containers for autolayout)
*********************************************************************************************/
class GraphWindow : public QMainWindow
{
	Q_OBJECT

public:
	GraphWindow(bool fullVersion, GraphController * gc = NULL, bool preloaded = false);
	~GraphWindow();
	
private slots: 
	void newGraph();
	void openGraph();
	void saveGraph();
	void exportGraph();
	void screengrab();
	void toggleContainerVisibility();
	void toggleModifiersCloning();
	void toggleReactionsFusing();
	void toggleAvoidingEdges();
	void destroyLayout();
	void updateLayout();
	void arrangeSelection();

private:
	void closeGraph();
	void loadGraph(std::string filename = "");

	void createViews(bool fullVersion);

	void createControls(bool fullVersion);
	QAction * createAction(std::list<QAction*> &actionList, const char * name, const char * shortCut, const char * tip, std::string iconName = "");
	QMenu * createMenu(std::list<QAction*> &actionList, const char * name);
	QToolBar * createToolbar(std::list<QAction*> &actionList, const char * name, Qt::ToolBarAreas allowedAreas, Qt::ToolBarArea defaultArea);

protected:
	GraphController * controller;
};

#endif