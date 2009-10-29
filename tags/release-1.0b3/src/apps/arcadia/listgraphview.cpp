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
 *  ListGraphView.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Last documented on 17/04/2008.
 *
 */
 
#include "listgraphview.h"

// Qt
#include <QHeaderView>
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QCheckBox>

// local information on Vertices and Edges
#include "vertexproperty.h"
#include "edgeproperty.h"

#include "graphcontroller.h"

#include "graphlayout.h"

#include <iostream>

/**************
* Constructor *
***************
* Sets up the View's Controller and the parent Widget
* Allows extended selection
* Creates 3 columns: type, label and neighbours
* The columns can be sorted, and are sized to fit their content
* Internal signals are connected to appropriates slots
* to react to vertex selection and double click
***************************************************************/
ListGraphView::ListGraphView(GraphController * c, QWidget * parent) : GraphView(c), QWidget(parent), graphLayout(NULL)
{
	QVBoxLayout *layout = new QVBoxLayout;
	layout->setContentsMargins (0,0,0,0);

	this->treeWidget = new QTreeWidget(this);
	this->treeWidget->setSelectionMode(QAbstractItemView::ExtendedSelection);
	this->treeWidget->setColumnCount(3);
	QStringList headers;
	headers << "Type " << " Label " << " Neighbours";
	this->treeWidget->setHeaderLabels(headers);
	this->treeWidget->setSortingEnabled(true);

	this->treeWidget->header()->setResizeMode(QHeaderView::ResizeToContents);
//	this->treeWidget->header()->setCascadingSectionResizes (true);
	this->treeWidget->setMinimumWidth(this->treeWidget->header()->width()+110);

	layout->addWidget(this->treeWidget);

	this->toggleView = new QCheckBox("Show only current layout");
	this->toggleView->setCheckState(Qt::Checked);
	layout->addWidget(this->toggleView);

	this->setLayout(layout);

	QObject::connect(this->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(changeVertexSelection()));
	QObject::connect(this->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)), this, SLOT(doubleClickVertex(QTreeWidgetItem *, int)));
	QObject::connect(this->toggleView, SIGNAL(stateChanged (int)), this, SLOT(updateTree()));
	
	this->menu = NULL;
	
	if (this->graphModel) this->display(this->graphModel);
}

#include "clonecontent.h"

void ListGraphView::selectLayout(GraphLayout * gl)
{
	this->toggleView->setText(QString::fromStdString("Show only " + gl->name));
	
	this->graphLayout = gl;
	this->updateTree();
}

// display only the bits that should be shown
void ListGraphView::updateTree()
{
	if (!this->graphLayout) return;

	// hack to hide all the node items => looks for item with something in the neighbou column
	QList<QTreeWidgetItem *> list = this->treeWidget->findItems(QString(" "), Qt::MatchContains|Qt::MatchWrap|Qt::MatchRecursive, 2);
	for (int i = 0; i < list.size(); ++i)
	{
		list.at(i)->setHidden((this->toggleView->checkState() == Qt::Checked));
	}

	if (this->toggleView->checkState() != Qt::Checked) return;

	std::list< CloneContent * > cList = this->graphLayout->getCloneContents();
	for (std::list< CloneContent * >::iterator it = cList.begin(); it != cList.end(); ++it)
	{
		BGL_Vertex v = (*it)->getVertex();
		std::map<BGL_Vertex, QTreeWidgetItem *>::iterator it = vertexToListItem.find(v);
		if( it != vertexToListItem.end()) (*it).second->setHidden(false);
	}
}

/**********
* display *
***********
* calls the base class method
* then populates the Tree:
* first the tree and maps get cleared
* then we add every vertices
* then (if the option is true)
* [!] it's not!
* we add every edges as well
* [!] shouldn't the tree be more tree-like?
* (edge/vertex branches, then reac/spec branches, etc.)
*******************************************************/
void ListGraphView::display(GraphModel * gModel)
{
	GraphView::display(gModel);

	this->treeWidget->clear();
	this->vertexToListItem.clear();
	this->listItemToVertex.clear();
	this->classToListItem.clear();

	std::list<BGL_Vertex> vList = this->graphModel->getVertices();
	for (std::list<BGL_Vertex>::iterator it = vList.begin(); it != vList.end(); ++it)
	{	
		this->addVertex(*it);		
	}

	bool edges = false;

	this->updateTree();
	
	if (!edges) return;

	std::list<BGL_Edge> eList = this->graphModel->getEdges();
	for (std::list<BGL_Edge>::iterator it = eList.begin(); it != eList.end(); ++it)
	{
		this->addEdge(*it);
	}	
}

/************
* addVertex *
*************
* From the Vertex properties,
* displays the typeLabel and Label
* then the edge connectivity
* [!] connectivity not well sorted when n > 9
* (string sorting, not int sorting...)
*********************************************/
void ListGraphView::addVertex(BGL_Vertex v)
{
	VertexProperty * vp = this->graphModel->getProperties(v);

	std::string superClassLabel = "";
	std::string classLabel = "";
	std::string instanceLabel = "";
	if (vp)
	{
		superClassLabel = vp->getSuperTypeLabel();
		classLabel = vp->getTypeLabel();
		instanceLabel = vp->getLabel();
	}

	// the branch
		
	QTreeWidgetItem * branch;

	if (this->classToListItem.find(classLabel) != this->classToListItem.end()) branch = this->classToListItem[classLabel];
	else
	{
		if (superClassLabel == "") branch = new QTreeWidgetItem(this->treeWidget);
		else
		{
			QTreeWidgetItem * superBranch;

			if (this->classToListItem.find(superClassLabel) != this->classToListItem.end()) superBranch = this->classToListItem[superClassLabel];
			else
			{
				superBranch = new QTreeWidgetItem(this->treeWidget);
				superBranch->setText(0, superClassLabel.c_str());
				this->classToListItem[superClassLabel] = superBranch;
			}
			branch = new QTreeWidgetItem(superBranch);
		}
		branch->setText(0, classLabel.c_str());
		this->classToListItem[classLabel] = branch;
	}	

	// the item

	// [!] The list view should display not just names but ids and compartments too

	QTreeWidgetItem * vertex = new QTreeWidgetItem(branch);
	
	vertex->setText(1, instanceLabel.c_str());
	
	int connectivity = this->graphModel->getEdges(v).size();
	char buf[8];
	if (connectivity > 999)
		sprintf(buf, " %d", connectivity);
	else if (connectivity > 99)
		sprintf(buf, "  %d", connectivity);	
	else if (connectivity > 9)
		sprintf(buf, "   %d", connectivity);	
	else
		sprintf(buf, "    %d", connectivity);
	vertex->setText(2, buf);
	
	this->vertexToListItem[v] = vertex;
	this->listItemToVertex[vertex] = v;
}

/**********
* addEdge *
***********
* From the Edge properties
* displays Edge, then the Label
* [!] no typeLabel nor connectivity?
************************************/
void ListGraphView::addEdge(BGL_Edge e)
{
	EdgeProperty * ep = this->graphModel->getProperties(e);

	std::string classLabel = "";
	std::string instanceLabel = "";
	if (ep) {
		classLabel = ep->getTypeLabel();
		instanceLabel = ep->stringVersion();
	}

	QTreeWidgetItem * branch;
	QList<QTreeWidgetItem *> list = this->treeWidget->findItems(QString(classLabel.c_str()), Qt::MatchExactly|Qt::MatchRecursive, 0);
	if (list.count()) branch = list.at(0);
	else
	{
		branch = new QTreeWidgetItem(this->treeWidget);
		branch->setText(0, classLabel.c_str());
	}

	QTreeWidgetItem * edge = new QTreeWidgetItem(branch);

	edge->setText(1, instanceLabel.c_str());
}

/*********
* select *
**********
* Called by the Controller
* Sets up the current selection to match the given list
* and scrolls to the last selected item
* [!] should I try to show the entire list, focus wise?
*******************************************************/
void ListGraphView::select(std::list<BGL_Vertex> vList)
{
	this->treeWidget->clearSelection();
	QTreeWidgetItem *item = NULL;
	for (std::list<BGL_Vertex>::iterator it = vList.begin(); it != vList.end(); ++it)
	{
		BGL_Vertex v = (*it);		
		item = this->vertexToListItem[v];
		item->setSelected(true);
	}
	if (item) this->treeWidget->scrollToItem(item);
}

/************************
* changeVertexSelection *
*************************
* Called internally when a change to the selection is detected
* A list of selected vertex is gathered
* and sent through the standard GraphView interface to the Controller
*********************************************************************/
void ListGraphView::changeVertexSelection()
{	
	if (!this->graphModel) return;

	std::list<BGL_Vertex> vList;
	QList<QTreeWidgetItem *> list = this->treeWidget->selectedItems();

	for (int i=0; i<list.count(); ++i) {
		QTreeWidgetItem *item = list.at(i);
				
		if (this->listItemToVertex.find(item) == this->listItemToVertex.end()) continue;
		vList.push_back(this->listItemToVertex[item]);
	}
	this->vertexSelectionChanged(vList);
}

/********************
* doubleClickVertex *
*********************
* Called internally when a doubleClick on a Vertex is detected
* The Controller gets notified to toggle the cloning of that Vertex
* through the standard GraphView interface
*******************************************************************/
void ListGraphView::doubleClickVertex(QTreeWidgetItem * item, int column)
{
	if (this->listItemToVertex.find(item) == this->listItemToVertex.end()) return;
	BGL_Vertex v = this->listItemToVertex[item];
	this->toggleCloning(v);
}

void ListGraphView::createMenu()
{
	std::list<QAction*> actionList;
	QAction * action;
	action = this->createAction("&Local view", "Ctrl+L", "Switches to local view");
	QObject::connect(action, SIGNAL( triggered() ), this, SLOT( switchToLocalView() ));
	actionList.push_back(action);

	action = this->createAction("&New view", "Ctrl+N", "Creates a new view");
	QObject::connect(action, SIGNAL( triggered() ), this, SLOT( createNewView() ));
	actionList.push_back(action);

	action = this->createAction("&Expand view", "Ctrl+E", "Expands the view");
	QObject::connect(action, SIGNAL( triggered() ), this, SLOT( expandCurrentView() ));
	actionList.push_back(action);

	action = this->createAction("&Remove", "Ctrl+R", "Remove from the view");
	QObject::connect(action, SIGNAL( triggered() ), this, SLOT( removeFromView() ));
	actionList.push_back(action);

	action = this->createAction("&Move", "Ctrl+M", "Move to a new view");
	QObject::connect(action, SIGNAL( triggered() ), this, SLOT( moveToNewView() ));
	actionList.push_back(action);

/*
	std::list<std::string> aList = this->controller->getActions();
	for (std::list<std::string>::iterator it = aList.begin(); it != aList.end(); ++it)
	{
		actionList.push_back(this->createAction((*it).c_str()));
	}
*/

	this->menu = new QMenu(this);
	for (std::list<QAction*>::iterator it = actionList.begin(); it != actionList.end(); ++it)
	{
		menu->addAction(*it);
	}
	QObject::connect( this->menu, SIGNAL( triggered ( QAction * ) ), this, SLOT( executeAction( QAction * ) ) );
}

void ListGraphView::contextMenuEvent(QContextMenuEvent * event) {
	QWidget::contextMenuEvent(event);

/*	
	if (!this->menu) this->createMenu();

	this->menu->exec(event->globalPos());
*/
	this->switchToLocalView();
}

void ListGraphView::switchToLocalView()
{
	std::list<BGL_Vertex> vList;
	QList<QTreeWidgetItem *> list = this->treeWidget->selectedItems();

	for (int i=0; i<list.count(); ++i)
	{
		QTreeWidgetItem *item = list.at(i);
		if (this->listItemToVertex.find(item) == this->listItemToVertex.end()) continue;
		vList.push_back( this->listItemToVertex[item] );
	}

	this->switchView(vList);
}

void ListGraphView::createNewView()
{
	std::list<BGL_Vertex> vList;
	QList<QTreeWidgetItem *> list = this->treeWidget->selectedItems();

	for (int i=0; i<list.count(); ++i)
	{
		QTreeWidgetItem *item = list.at(i);
		if (this->listItemToVertex.find(item) == this->listItemToVertex.end()) continue;
		vList.push_back( this->listItemToVertex[item] );
	}

	this->createView(vList);
}

void ListGraphView::expandCurrentView()
{
	std::list<BGL_Vertex> vList;
	QList<QTreeWidgetItem *> list = this->treeWidget->selectedItems();

	for (int i=0; i<list.count(); ++i)
	{
		QTreeWidgetItem *item = list.at(i);
		if (this->listItemToVertex.find(item) == this->listItemToVertex.end()) continue;
		vList.push_back( this->listItemToVertex[item] );
	}

	this->expandView(vList, NULL);
}

void  ListGraphView::removeFromView()
{
}

void  ListGraphView::moveToNewView()
{
}

QAction * ListGraphView::createAction(const char * name, const char * shortCut, const char * tip)
{
	QAction * action = new QAction(tr(name), this);
	if (shortCut != "") action->setShortcut(tr(shortCut));
	if (tip != "") action->setStatusTip(tr(tip));
	return action;
}

void  ListGraphView::executeAction(QAction * action)
{
	std::string label = action->text().toStdString();
	this->controller->executeAction(label);
}