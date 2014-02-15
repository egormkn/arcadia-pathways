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
 *  GraphWindow.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *	Last Documented on 17/04/2008.
 *
 */

#include "graphwindow.h"

#include <iostream>

// Qt Application Widgets
#include <QFileDialog>
#include <QDockWidget>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QMessageBox>

// local GraphViews
#include "modelgraphview.h"
#include "listgraphview.h"
#include "propertygraphview.h"

#include "graphcontroller.h"

/*********************************************************************
* Public Methods                                                     *
*********************************************************************/

/**************
* Constructor *
***************
* Creates controls and views,
* Loads a new graph
* And displays the window
*****************************/
GraphWindow::GraphWindow(bool fullVersion, GraphController * gc, bool preloaded) : controller(NULL)
{
	if (!gc)	gc = new GraphController();

	this->controller = gc;

	this->createControls(fullVersion);

	this->createViews(fullVersion);

	if (!preloaded)	this->setWindowTitle("No Document");//this->newGraph();
	else			if (this->controller) this->setWindowTitle(this->controller->getModelTitle().c_str());
				
	this->show();
	
	gc->setGraphWindow(this);// linking the undo flag
}

/*************
* Destructor *
**************
* Closes the current graph
**************************/
GraphWindow::~GraphWindow()
{
	this->closeGraph();
	if (this->controller) delete this->controller;
}

/*********************************************************************
* Private Slots                                                      *
*********************************************************************/

/***********
* newGraph *
************
* Loads an empty graph
**********************/
void GraphWindow::newGraph()
{
	this->loadGraph();
}

/************
* openGraph *
*************
* Opens a dialog box to choose a file
* then loads the graph described by the chosen file
* If no file is chosen, does nothing
* [!] at the moment, only SBML files can be opened
*****************************************************/
void GraphWindow::openGraph()
{
	std::string fileTypes = (this->controller? this->controller->getImportFileTypes() : "");
	std::string path = (this->controller? this->controller->getFileName() : "");

	QString openedFile = QFileDialog::getOpenFileName(NULL, QObject::tr("Open Graph File"), path.c_str(), QObject::tr(fileTypes.c_str()));
	if (!openedFile.length()) return;
	
	this->loadGraph(openedFile.toStdString());
}

/************
* saveGraph *
*************
* Saves the current graph
* [!] with the default saving method of the model
* and saves a screenshot as a16.png
* [!] buggy, and should not be there in the public release
**********************************************************/
void GraphWindow::saveGraph()
{
	if (this->controller) this->controller->save("");
}

void GraphWindow::exportGraph()
{
	if (!this->controller) return;
	if (this->controller->isEmpty()) return;
	
	std::string fileTypes = this->controller->getExportFileTypes();
	std::string path = this->controller->getFileName();

	QString savedFile = QFileDialog::getSaveFileName(this, QObject::tr("Save Graph File"), path.c_str(), QObject::tr(fileTypes.c_str()));
	if (!savedFile.length()) return;

	this->controller->save(savedFile.toStdString());
}

void GraphWindow::screengrab()
{
	// PDF Screenshot?
	std::string fName =  (this->controller? this->controller->getFileName() : "no_name") + ".png";
	// doesn't seem actually possible in the current release!.png instead
	QPixmap pixmap(this->size());
	this->render(&pixmap);
	pixmap.save(fName.c_str());	
}

/*********************************************************************
* Private Methods for handling the model                             *
*********************************************************************/

/*************
* closeGraph *
**************
* If a graph model exists, it gets deleted
* [!] Doesn't check at the moment whether people may want to save changes first
* [!] Maybe this method is useless and this->controller.close() should be called directly?
******************************************************************************************/
void GraphWindow::closeGraph()
{
	if (this->controller) this->controller->close();
}

/************
* loadGraph *
*************
* Closes any existing graph handled by the Controller
*
* The Controller gets a new graph model from a filename (possibly empty)
* and lets the graph be displayed by every views it handles
*
* The window title alse becomes that of the graph
************************************************************************/
void GraphWindow::loadGraph(std::string filename)
{
	this->closeGraph();
	
	try
	{
		if (this->controller) this->controller->load(filename);
	}
	catch(std::exception& err)
	{
		// create an alert window with the relevant error message
//		std::string title = "Cannot open file";
		std::string title = "Errors were encountered while opening the file";
		std::string text = title + "\n\n" + err.what();
		QMessageBox::critical ( this, title.c_str(), text.c_str() );
//		return;
	}

	if (this->controller) this->setWindowTitle(this->controller->getModelTitle().c_str());
}

/*********************************************************************
* Private Methods for handling the views                             *
*********************************************************************/

/**************
* createViews *
***************
* Creates a central graphic view (=ModelGraphView)
*
* If in full version, also sets up Dock properties
* and puts there an ListGraphView and a PropertyGraphView
*
* Every GraphView is handled by the GraphController
* [!] No option to activate/deactivate views interactively
******************************************************************/
void GraphWindow::createViews(bool fullVersion)
{
	// Central: graph map
	ModelGraphView * graphMap = new ModelGraphView(this->controller, fullVersion, this);
	this->setCentralWidget(graphMap);
	graphMap->show();

	if (!fullVersion) return;

	// Dock
	// Set active corners for the dock
	this->setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	this->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	this->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	this->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	// Element list
	QDockWidget * listWindow = new QDockWidget(tr("List of model elements"), this);//, Qt::Drawer);
	ListGraphView * elementList = new ListGraphView(this->controller, listWindow);
	listWindow->setWidget(elementList);
	listWindow->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	listWindow->setFeatures(QDockWidget::DockWidgetMovable	| QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetVerticalTitleBar | QDockWidget::DockWidgetClosable);
	this->addDockWidget(Qt::LeftDockWidgetArea, listWindow);	
	
	// Property view
	QDockWidget * propertyWindow = new QDockWidget(tr("Properties of selected elements"), this);
	PropertyGraphView * propertyView = new PropertyGraphView(this->controller, propertyWindow);
	propertyWindow->setWidget(propertyView);
	propertyWindow->setAllowedAreas(Qt::TopDockWidgetArea | Qt::BottomDockWidgetArea);
	this->addDockWidget(Qt::BottomDockWidgetArea, propertyWindow);

	QMenu * menu = this->menuBar()->addMenu(tr("Windows"));
	menu->addAction(listWindow->toggleViewAction ());
	menu->addAction(propertyWindow->toggleViewAction ());
}

/*********************************************************************
* Private Methods for handling the controls                          *
*********************************************************************/

/*****************
* createControls *
******************
* Creates the New, Open and Save actions (linked to private slots)
* Depending on options, also creates a menu and a toolbar
* and put all the actions inside
******************************************************************/
void GraphWindow::createControls(bool fullVersion)
{
	bool menu = fullVersion;
	bool toolbar = fullVersion;
	
	// Creating actions
	std::list<QAction*> fileActionList;
	std::list<QAction*> editActionList;
	std::list<QAction*> viewActionList;

	QAction * action;
/*
	action = this->createAction(fileActionList, "&New", "Ctrl+N", "Create a new file");
	QObject::connect(action, SIGNAL( triggered() ), this, SLOT( newGraph() ));
*/	

	action = this->createAction(fileActionList, "&Open", "Ctrl+O", "Open an existing file");
	QObject::connect(action, SIGNAL( triggered() ), this, SLOT( openGraph() ));

	action = this->createAction(fileActionList, "&Save", "Ctrl+S", "Save an existing file");
	QObject::connect(action, SIGNAL( triggered() ), this, SLOT( saveGraph() ));

	action = this->createAction(fileActionList, "&Export", "Ctrl+E", "Export an existing file");
	QObject::connect(action, SIGNAL( triggered() ), this, SLOT( exportGraph() ));

/*
	action = this->createAction("Screen&grab", "Ctrl+G", "Take a screenshot of the application");
	QObject::connect(action, SIGNAL( triggered() ), this, SLOT( screengrab() ));
	actionList.push_back(action);
*/

/*	
	action = this->createAction(editActionList, "Fuse/Unfuse Similar &Reactions", "Ctrl+R", "Fuse/unfuse similar reactions");
	QObject::connect(action, SIGNAL( triggered() ), this, SLOT( toggleReactionsFusing() ));
*/
	action = this->createAction(editActionList, "Undo", "Ctrl+Z", "Cancels last action");
	QObject::connect(action, SIGNAL( triggered() ), this, SLOT( undo() ));
	undoAction = action;
	undoAction->setDisabled(true);
	
	action = this->createAction(editActionList, "&Update Layout", "Ctrl+U", "Computes automatic layout");
	QObject::connect(action, SIGNAL( triggered() ), this, SLOT( updateLayout() ));

	action = this->createAction(editActionList, "Destroy Layout", "Ctrl+D", "Destroy layout");
	QObject::connect(action, SIGNAL( triggered() ), this, SLOT( destroyLayout() ));

	action = this->createAction(editActionList, "Layout &Modifiers", "Ctrl+M", "Clone/unclone modifiers");
	QObject::connect(action, SIGNAL( triggered() ), this, SLOT( toggleModifiersCloning() ));

	action = this->createAction(editActionList, "&Arrange Selection", "Ctrl+A", "(un)clone or (un)branch selection");
	QObject::connect(action, SIGNAL( triggered() ), this, SLOT( arrangeSelection() ));

	action = this->createAction(viewActionList, "Show &Containers", "Ctrl+C", "Toggles container visibility");
	QObject::connect(action, SIGNAL( triggered() ), this, SLOT( toggleContainerVisibility() ));
	action->setCheckable(true);
	action->setChecked(false);

/*
	action = this->createAction(viewActionList, "&Route Edges", "Ctrl+R", "Toggles automatic edge routing");
	QObject::connect(action, SIGNAL( triggered() ), this, SLOT( toggleAvoidingEdges() ));
	action->setCheckable(true);
	action->setChecked(false);
*/

	if (menu)
	{
		this->createMenu(fileActionList, "&File");
		this->createMenu(editActionList, "&Action");
		this->createMenu(viewActionList, "&View");
	}
	
	if (toolbar)
	{
		this->createToolbar(fileActionList, "File", Qt::TopToolBarArea | Qt::BottomToolBarArea, Qt::TopToolBarArea);
		this->createToolbar(undoAction, "Undo", Qt::TopToolBarArea | Qt::BottomToolBarArea, Qt::TopToolBarArea);
//		this->createToolbar(editActionList, "Action", Qt::TopToolBarArea | Qt::BottomToolBarArea, Qt::TopToolBarArea);
//		this->createToolbar(viewActionList, "View", Qt::TopToolBarArea | Qt::BottomToolBarArea, Qt::TopToolBarArea);
	}
}

QMenu * GraphWindow::createMenu(std::list<QAction*> &actionList, const char * name)
{
	QMenu * menu = this->menuBar()->addMenu(tr(name));
	for (std::list<QAction*>::iterator it = actionList.begin(); it != actionList.end(); ++it)
	{
		menu->addAction(*it);
	}
	
	return menu;
}

QToolBar * GraphWindow::createToolbar(QAction * action, const char * name, Qt::ToolBarAreas allowedAreas, Qt::ToolBarArea defaultArea)
{
	std::list<QAction*> actionList;
	actionList.push_back(action);
	this->createToolbar(actionList, name, allowedAreas, defaultArea);
}

QToolBar * GraphWindow::createToolbar(std::list<QAction*> &actionList, const char * name, Qt::ToolBarAreas allowedAreas, Qt::ToolBarArea defaultArea)
{
	QToolBar * toolBar = this->addToolBar(tr(name));
	for (std::list<QAction*>::iterator it = actionList.begin(); it != actionList.end(); ++it)
	{
		toolBar->addAction(*it);
	}
	toolBar->setAllowedAreas(allowedAreas);
	this->addToolBar(defaultArea, toolBar);
	return toolBar;
}

/***************
* createAction *
****************
* Creates an action with a given name, shortcut and tooltip
***********************************************************/
QAction * GraphWindow::createAction(std::list<QAction*> &actionList, const char * name, const char * shortCut, const char * tip, std::string iconFile)
{
	QAction * action = new QAction(tr(name), this);
	
	action->setShortcut(tr(shortCut));
	action->setStatusTip(tr(tip));

	if (iconFile != "")
	{
		QIcon icon = QIcon();
		QString menuFile = QString::fromStdString (iconFile+"16.png");
		QString toolbarFile = QString::fromStdString (iconFile+"24.png");
		icon.addFile( menuFile, QSize(16,16) ); // menu icon
		icon.addFile( toolbarFile, QSize(24,24) ); // toolbar icon
		action->setIcon(icon);
	}

	actionList.push_back(action);

	return action;
}

void GraphWindow::toggleAvoidingEdges()
{
	if (this->controller) this->controller->toggleAvoidingEdges();
}

void GraphWindow::toggleModifiersCloning()
{
	if (this->controller) this->controller->toggleModifiersCloning();
}

void GraphWindow::toggleReactionsFusing()
{
	if (this->controller) this->controller->toggleReactionsFusing();
}

void GraphWindow::destroyLayout()
{
	if (this->controller) this->controller->destroyLayout();
}

void GraphWindow::undo()
{
	if (this->controller) this->controller->undo();
}

void GraphWindow::enableUndo(bool enable)
{
	this->undoAction->setEnabled(enable);
}

void GraphWindow::updateLayout()
{
	if (this->controller) this->controller->updateLayout(NULL, false, false);
}

void GraphWindow::arrangeSelection()
{
	if (this->controller) this->controller->arrangeSelection();
}

void GraphWindow::toggleContainerVisibility()
{
	if (this->controller) this->controller->toggleContainerVisibility();
}