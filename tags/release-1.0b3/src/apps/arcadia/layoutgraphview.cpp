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
 *  LayoutGraphView.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 27/02/2008.
 *	Last Documented on 17/04/2008.
 *
 */
 
#include "layoutgraphview.h"

#include <iostream>

// Qt utilities for graphics export
#include <QPrinter>
#include <QPainter>
#include <QtSvg>
#include <QSvgGenerator>

// local
#include "graphloader.h"
#include "graphlayout.h"
#include "vertexgraphics.h"
#include "edgegraphics.h"
#include "vertexproperty.h"
#include "clonecontent.h"
#include "containercontent.h"
#include "containergraphics.h"
#include "stylesheet.h"

#include <fstream>

/**************
* Constructor *
***************
* sets up the views's Controller and Model, as well as the local layout number
* Also connects the local selectionChanged signal to changeVertexSelection
******************************************************************************/
LayoutGraphView::LayoutGraphView(GraphController * c, unsigned int lNumber) :  GraphView(c), moving(false), moved(false), mouseDown(false), selecting(false), supersize(false)
{
	this->layout = this->graphModel->getLayout(lNumber);

	if (!this->layout) 	this->layout = this->graphModel->getLayout(0);

	QObject::connect(this, SIGNAL(selectionChanged()), this, SLOT(changeVertexSelection()));

	this->setVisible(false);

	this->display();
}

void LayoutGraphView::display(GraphModel * gModel)
{	
	GraphView::display(gModel);
}

/**********
* display *
***********
* sets up the View's model
* [!] a bit redundant with the constructor??
*
* The scene is reset by clearing the vertexToGraphics map
* and removing + deleting every graphics item
* [!] do we NEED to remove + delete?
*
* From the model, Vertices get added first, then edges
* then the container tree of that layout
* [!] I should probably add all the Content in one go,
* [!] instead of adding Clones and Containers separately?
*********************************************************/
void LayoutGraphView::display()
{	
	if (!this->graphModel) return;

	// reset
	this->edges.clear();
	this->vertices.clear();
	this->containers.clear();
	this->vertexToGraphics.clear();
	this->connectorToGraphics.clear();
	QList<QGraphicsItem *> list = this->items();
	for (int i=0; i<list.size(); ++i)
	{
		this->removeItem(list.at(i));
		delete(list.at(i));
	}
	
	// adding Vertices		
	std::list<BGL_Vertex> vList = this->graphModel->getVertices();	
	
	for (std::list<BGL_Vertex>::iterator it = vList.begin(); it != vList.end(); ++it)
	{	
		this->displayVertex(*it);
		if (this->vertices.size() > 4000) { this->supersize = true; break; }
	}

	if (!this->supersize)
	{
		// adding Edges
		std::list<BGL_Edge> eList = this->graphModel->getEdges();

		for (std::list<BGL_Edge>::iterator it = eList.begin(); it != eList.end(); ++it)
		{
			this->displayEdge(*it);
			if (this->edges.size() > 9000) { this->supersize = true; break; }
		}
	}

	if (this->supersize)
	{
		// reset
		this->edges.clear();
		this->vertices.clear();
		this->containers.clear();
		this->vertexToGraphics.clear();
		this->connectorToGraphics.clear();
		QList<QGraphicsItem *> list = this->items();
		for (int i=0; i<list.size(); ++i)
		{
			this->removeItem(list.at(i));
			delete(list.at(i));
		}
		
		this->setBackgroundBrush(QBrush(QPixmap(QString("spaghetti.jpg"))));

		return;
	}

	// let's not forget our containers!
	this->displayContainerTree(this->layout->getRoot());
	
	// to layout edges
	this->updateLayout(this->layout, true);
}

/***********************
* displayContainerTree *
************************
* That recursive method adds the given container as a ContainerGraphics
* [!] no styling!
* then does the same again for every children that is a Container too
* [!] detected through the id, a bit dodgy, ugly casting...? (getType?)
***********************************************************************/
void LayoutGraphView::displayContainerTree(ContainerContent *root)
{
	std::string type = root->getTypeLabel(); 
	// if (root->isHidden()) type = "GenericContainer"; // actually, it's upon painting that we decide what to do about the hidden parameter = hide the border but not the text

	ContainerStyle * cls = this->layout->getStyleSheet()->getContainerStyle(type);

	ContainerGraphics * cg = new ContainerGraphics(root, cls);

	this->addItem(cg);

	this->containers.push_back(cg);

	std::list< Content* > children = root->getChildren();
	for (std::list< Content* >::iterator it = children.begin(); it != children.end(); ++it)
	{
		Content * c = *it;
		if (c->getId() != "") this->displayContainerTree((ContainerContent*)c);
	}
}

/*****************
* exportGraphics *
******************
* For this given layout, creates 3 files:
* a pdf, ps and svg file
* The scene is rendered in each format
* [!] the SVG export of Qt is broken, labels don't appear
*********************************************************/
void LayoutGraphView::exportGraphics(std::string filename)
{
	std::string baseName = GraphLoader::GetFileNameOnly(filename);
	char buf[8];
	sprintf(buf, "_%d", this->graphModel->getNumber(this->layout));
	baseName += buf;

	std::string ext = GraphLoader::GetFileExtension(filename);
	baseName += "." + ext;
	
	if (ext == "pdf")
	{
		// Dump scene to PDF
		QPrinter *printer = new QPrinter();
		printer->setOutputFormat(QPrinter::PdfFormat);
		printer->setOutputFileName(baseName.c_str());
		QPainter *pdfPainter = new QPainter(printer);
		this->render(pdfPainter);
		pdfPainter->end();
	}

	if (ext == "ps")
	{
		// Dump scene to PS
		QPrinter *printer = new QPrinter();
		printer->setOutputFormat(QPrinter::PostScriptFormat);
		printer->setOutputFileName(baseName.c_str());
		QPainter *psPainter = new QPainter(printer);
		this->render(psPainter);
		psPainter->end();
	}

	if (ext == "svg")
	{
		// Dump scene to SVG // broken
		QSvgGenerator *gen = new QSvgGenerator();
		gen->setFileName(baseName.c_str());
		gen->setTitle( this->getLayout()->name.c_str());
		gen->setDescription(this->getLayout()->name.c_str());
		QRectF r = this->sceneRect();
		gen->setSize(QSize(r.width(), r.height())); // essential to get a proper scale!
//		gen->setViewBox (QRectF(0,0,200,400));
//		gen->setResolution(200);

		QPainter *svgPainter = new QPainter(gen);
		this->render(svgPainter);
		svgPainter->end();
	}
}

/****************
* displayVertex *
*****************
* For a given vertex, a label and default style are obtained
* For every clone of the vertex in this layout
* we create a VertexGraphics with that style and label
* The items are added to the scene, and mapped to the vertex
************************************************************/
void LayoutGraphView::displayVertex(BGL_Vertex v)
{
	GraphLayout * graphLayout = this->layout;

	std::list<VertexGraphics *> vGraphics;
	std::list<CloneContent *> clones = graphLayout->getClones(v);

	VertexStyle * vls;
	if (!clones.empty())
	{
		CloneProperty cp;

		if (clones.size() > 1) cp = isClone;
		else if (clones.front()->getContainer()->getTypeLabel() == "CloneContainer") cp = midget;
		else cp = notClone;

		// for reactions
		if (clones.front()->rotated) cp = rotated;
		
		vls = graphLayout->getStyleSheet()->getVertexStyle(this->graphModel->getProperties(v), cp);
	}

	for (std::list<CloneContent *>::iterator it = clones.begin(); it != clones.end(); ++it)
	{
		CloneContent * clone = *it;
		VertexGraphics* vg = new VertexGraphics(this, clone, clone->getLabel(), vls);
		this->addItem(vg);
		vGraphics.push_back(vg);
		this->vertices.push_back(vg);
	}
	
	this->vertexToGraphics[v] = vGraphics;
}

/**************
* displayEdge *
***************
* We get the edge's default style (cf its EdgeProperty)
* We also get the source and target vertices of the edge
* For the scene's layout, we obtain the connector between these two
* We create an EdgeGraphics for that style and connector
* and add the Item to the Scene
*******************************************************************/
void LayoutGraphView::displayEdge(BGL_Edge e)
{
	GraphLayout * graphLayout = this->layout;
	
	EdgeStyle *els = graphLayout->getStyleSheet()->getEdgeStyle(this->graphModel->getProperties(e));
	
	BGL_Vertex u = this->graphModel->getSource(e);
	BGL_Vertex v = this->graphModel->getTarget(e);

	Connector * c = graphLayout->getConnector(u, v);
	if (!c) return; // this can happen when displaying a neighbourhood layout

//	std::cout << graphLayout->getClone(u)->getLabel() << "->" << graphLayout->getClone(v)->getLabel() << std::endl;

	EdgeGraphics* ei = new EdgeGraphics(this, c, els);

	this->addItem(ei);

	this->edges.push_back(ei);
	
	this->connectorToGraphics[c] = ei;
}

/*********
* select *
**********
* We reset the current selection
* For every vertex in the list,
* we get the corresponding VertexGraphics
* and select+update these one by one
* and sets them as the new center
* [!] it would be better to tell the ModelGraphView to center on every selected items
*******************************************************************************/
void LayoutGraphView::select(std::list<BGL_Vertex> vList)
{
	this->clearSelection();

	for (std::list<BGL_Vertex>::iterator it = vList.begin(); it != vList.end(); ++it) {
		BGL_Vertex v = *it;

		std::list<VertexGraphics *> vGraphics = this->vertexToGraphics[v];
		for (std::list<VertexGraphics *>::iterator ig = vGraphics.begin(); ig != vGraphics.end(); ++ig)
		{
			VertexGraphics * vg = *ig;
			vg->setSelected(true);
			vg->update();
			emit newCenter(vg->scenePos());
		}
	}
}

/************************
* mouseDoubleClickEvent *
*************************
* This overloaded method is called whenever a double click occurs on the scene
* If no item is currently selected we do nothing
* [!] could update the layout...
* If an item is, we toggle the cloning of its vertex in the current layout
* [!] we should check whether the vertex is actually clonable!!!
******************************************************************************/
void LayoutGraphView::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * mouseEvent ) {
	QGraphicsScene::mouseDoubleClickEvent(mouseEvent);	

	QList<QGraphicsItem*> list = this->selectedItems();
	if (!list.size())
	{
		this->updateLayout(this->layout);
	}
	else {
		GraphGraphics *gg = (GraphGraphics*)list.at(0);
		if (gg->isVertex())
		{
			VertexGraphics *vg = (VertexGraphics*)gg;
			this->toggleCloning(vg->getCloneContent()->getVertex(), this->layout, vg->getCloneContent());
		}
	}
}

/*
void PathwayScene::doubleClickNode(NodeView* node) {
	if (node->getStyle() == &this->reactionStyle) { this->branchReaction(node); }
	else if ( (node->getStyle() == &this->speciesStyle)
			||(node->getStyle() == &this->enzymeStyle)
			||(node->getStyle() == &this->transporterStyle)
			||(node->getStyle() == &this->proteinStyle)
			||(node->getStyle() == &this->metaboliteStyle)) 
	{ this->cloneNodeView(node); }
	else { this->uncloneNodeView(node); }
	
	this->layout(false);
}
*/

/************************
* changeVertexSelection *
*************************
* Called when the selectionChanged signal is detected
* [!] we assume selected items are VertexGraphics
* and put the corresponding vertices in a list
* sent by the View to the Controller to be selected
*****************************************************/
void LayoutGraphView::changeVertexSelection()
{
	// only when the mouse gets released
	if (this->mouseDown) { this->selecting = true; return; }

	this->selecting = false;
	
	this->update(); // to get rid of refreshing bugs
	
	std::list<BGL_Vertex> vList;
	QList<QGraphicsItem *> list = this->selectedItems();

	for (int i=0; i<list.size(); ++i) {
		GraphGraphics * gg = (GraphGraphics *) list.at(i);
		if (gg->isVertex())
		{
			VertexGraphics * vg = (VertexGraphics *) gg;
			vList.push_back(vg->getCloneContent()->getVertex());
		}
	}

	this->vertexSelectionChanged(vList);
}

/*****************
* cloningToggled *
******************
* First checks whether the scene is concerned by that update:
* -1 (= global update), or the local layout number
*
* then we resets the display of the current model
* [!] that's a bit of an overkill!
* [!] we should just remove and re-add the concerned vertex
* [!] + notify other items they should check their pos agains the model
* [!] also what about clone styles???
***********************************************************************/
void LayoutGraphView::cloningGotToggled(BGL_Vertex v, GraphLayout * gl)
{
	if ((gl) && (gl != this->layout)) return;

	this->display();
	
	this->update();

	this->resize();
}

// [!] we totally ignore the edge only bit...
void LayoutGraphView::layoutGotUpdated(GraphLayout * gl, bool edgesOnly, bool fast)
{
	if ((gl) && (gl != this->layout)) return;
	
	if (!fast)
	{
		// updating edge position is only required when moving them non manually
		// but the reaction orientation question is always on
		for (std::list<VertexGraphics*>::iterator it = this->vertices.begin(); it != this->vertices.end(); ++it)
		{
			if (!edgesOnly)	(*it)->updatePos(); // but this is fast, anyway
		
			// to display reactions with the right style...
			CloneContent * c = (*it)->getCloneContent();
			BGL_Vertex v = c->getVertex();
			std::list<CloneContent *> clones = this->layout->getClones(v);

			if (clones.size() > 1) continue; // reactions cannot be cloned

			CloneProperty cp = notClone;		
			if (clones.front()->getContainer()->getTypeLabel() == "CloneContainer") cp = midget; // but they can be part of a clone container

			if (clones.front()->rotated) cp = rotated;

			VertexStyle * vls = this->layout->getStyleSheet()->getVertexStyle(this->graphModel->getProperties(v), cp);
		
			(*it)->updateStyle(vls); // ideally I should only update reactions for which rotation has just changed
		}
	
		// this takes a stupid lot of time, especially when selections are big
		for (std::list<EdgeGraphics*>::iterator it = this->edges.begin(); it != this->edges.end(); ++it)
		{
			(*it)->updatePos();
		}

		// and the whole container thing takes lot of time too
		for (std::list<ContainerGraphics*>::iterator it = this->containers.begin(); it != this->containers.end(); ++it)
		{
			(*it)->setUpdateFlag(); // notify all the containers that their data may be obsolete
		}

		for (std::list<ContainerGraphics*>::iterator it = this->containers.begin(); it != this->containers.end(); ++it)
		{
			(*it)->updatePos(); // gets the new pos for the containers (the data is only updated once per container)
		}
		
		// and so does that bit, to some extent?
		this->update();
		this->resize();
	}
	
	if (fast)
	{
		for (std::list<EdgeGraphics *>::iterator it = inList.begin(); it != inList.end(); ++it)
		{
			(*it)->quickTranslate();
		}
		for (std::list<EdgeGraphics *>::iterator it = outList.begin(); it != outList.end(); ++it)
		{
			(*it)->quickUpdate();
		}	

		this->update();
		this->resize();
	}
}

/***************
* removeVertex *
****************
* [!] Currently not really used
* Removes and deletes ([!]overkill?)
* all the VertexGraphics mapped to a Vertex
* and removes the reference to them in the map too
**************************************************/
void LayoutGraphView::removeVertex(BGL_Vertex v)
{
	std::list<VertexGraphics *> vGraphics = this->vertexToGraphics[v];
	for (std::list<VertexGraphics *>::iterator it = vGraphics.begin(); it != vGraphics.end(); ++it)
	{
		VertexGraphics * vg = *it;
		this->removeItem(vg);
		delete(vg);
		this->vertices.remove(vg);
	}
	this->vertexToGraphics.erase(v);
}

#include "connector.h"

// Mouse Events: cf. manual placement
// [!] maybe I only need one state variable = moving
// [!] would it be possible to define a new temporary container tree in which the moving items would be layed out?
// to determine if a moving sequence is about to start: some items must be selected first
void LayoutGraphView::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsScene::mousePressEvent(event);
	this->mouseDown = true;
	float x = event->scenePos().x();
	float y = event->scenePos().y();		
	emit(newCenter(QPointF(x, y)));

	QList<QGraphicsItem *> list = this->selectedItems();
	this->moving = (list.size() > 0);
}

// to handle movement
void LayoutGraphView::mouseMoveEvent (QGraphicsSceneMouseEvent * event ) {
	QGraphicsScene::mouseMoveEvent(event);

	if (this->mouseDown)
	{
		float x = event->scenePos().x();
		float y = event->scenePos().y();		
		emit(newCenter(QPointF(x, y)));
	}

	if (this->moving)
	{
		this->moving = false;
		this->moved = true;

		this->saveAvoidingValue = this->layout->isAvoiding();
		this->layout->setAvoiding(false);
		
		// creating the edge lists from the vertex list
		inList.clear();
		outList.clear();

		this->getLayout()->inList.clear();
		this->getLayout()->outList.clear();

		QList<QGraphicsItem *> list = this->selectedItems();
		for (int i = 0; i < list.size(); ++i)
		{
			GraphGraphics *gg = (GraphGraphics*)list.at(i);
			if (gg->isVertex())
			{
				VertexGraphics *v = (VertexGraphics*)gg;
				CloneContent *c = v->getCloneContent();
				std::list<Connector*> cList = c->getOutterConnectors();
				for (std::list<Connector *>::iterator it = cList.begin(); it != cList.end(); ++it)
				{
					EdgeGraphics * eg = this->getEdgeGraphics(*it);
					if (!eg)
					{
						std::cout << "Bug!!!!!!!" << std::endl;
						std::cout << (*it)->getSource()->getLabel() << "->" << (*it)->getTarget()->getLabel() << std::endl;
					}
					else
					{
						if (std::find(outList.begin(), outList.end(), eg) == outList.end())
						{
							outList.push_back(eg);
							this->getLayout()->outList.push_back(*it);
						}
						else
						{
							outList.remove(eg);
							inList.push_back(eg);
							this->getLayout()->outList.remove(*it);
							this->getLayout()->inList.push_back(*it);
						}
					}
				}
			}
		}
	}

	if (this->moved)
	{	
		this->updateMovingVertices();
		this->updateLayout(this->layout, true, true);
	}	
}

void LayoutGraphView::mouseReleaseEvent (QGraphicsSceneMouseEvent * event ) {
	QGraphicsScene::mouseReleaseEvent(event);

	this->mouseDown = false;
	float x = event->scenePos().x();
	float y = event->scenePos().y();		
	emit(newCenter(QPointF(x, y)));
	
	if (this->moved)
	{
		this->layout->setAvoiding(this->saveAvoidingValue);
	
		// [!] suboptimal? Maybe I should only update these when the selection actually changes?
		inList.clear();
		outList.clear();

		this->getLayout()->inList.clear();
		this->getLayout()->outList.clear();

		this->updateMovingVertices();
		this->updateLayout(this->layout, true);

		this->moved = false;
	}
	
	if (this->moving) this->moving = false;
	
	if (this->selecting) this->changeVertexSelection();
//	this->update();
}

void LayoutGraphView::resize()
{
	float padding = 10;
	QRectF r;

 // this gives me a rectangle that ALWAYS contains (0,0) for some reason (maybe the edges??? setPos never called for them)
	r = this->itemsBoundingRect();
	
/*
	// it turns out that for some reason, the bounding rect of edges is strangely bound:
	left can't be bigger than zero, right can't be smaller than it
	same thing for top and bottom...
*/

	QPointF c =  r.center();

	r.setWidth(r.width()+2*padding); 
	r.setHeight(r.height()+2*padding);
	r.moveCenter(c);
	
	if (r != this->sceneRect())	this->setSceneRect(r);  
}

void LayoutGraphView::updateMovingVertices()
{
	QList<QGraphicsItem *> list = this->selectedItems();
	for (int i = 0; i < list.size(); ++i)
	{
		GraphGraphics *gg = (GraphGraphics*)list.at(i);
		if (gg->isVertex())
		{
			VertexGraphics *v = (VertexGraphics*)gg;
			v->updateClonePosition();
		}
	}
}

void LayoutGraphView::contextMenuEvent( QGraphicsSceneContextMenuEvent *event )
{	
	QGraphicsScene::contextMenuEvent(event);

//	QList<QGraphicsItem *> list = this->selectedItems();
	QList<QGraphicsItem *> list;
	if (QGraphicsItem * target = this->itemAt(event->scenePos()) )
	{
		list += target;
	}

	std::list<BGL_Vertex> vList;
	for (int i=0; i<list.size(); ++i)
	{
		GraphGraphics *gg = (GraphGraphics*)list.at(i);
		if (gg->isVertex())
		{
			VertexGraphics *vg = (VertexGraphics*)gg;
			BGL_Vertex v = vg->getCloneContent()->getVertex();
			if (std::find(vList.begin(), vList.end(), v) == vList.end()) vList.push_back(v);
		}
	}
	
	this->switchView(vList);
}

void LayoutGraphView::keyPressEvent ( QKeyEvent * keyEvent )
{
	QGraphicsScene::keyPressEvent(keyEvent);
	
//	if (keyEvent->key() != Qt::Key_Space) return;

// [!] nothing! I do fusing in the menu of the pathway window, atm (along with modifiers toggling)
}


void LayoutGraphView::setVisible(bool visible)
{
	if (this->layout) this->layout->setVisible(visible);
}

std::list<VertexGraphics *> LayoutGraphView::getVertexGraphics(BGL_Vertex v)
{
	return this->vertexToGraphics[v];
}

// [!] stupidly unefficient, need to rethink the storage map
VertexGraphics * LayoutGraphView::getVertexGraphics(CloneContent * c)
{
	VertexGraphics * vg = NULL;
	
	std::list<VertexGraphics *> vgList = this->vertexToGraphics[c->getVertex()];
	for (std::list<VertexGraphics *>::iterator it = vgList.begin(); it != vgList.end(); ++it)
	{
		vg = *it;
		if (vg->getCloneContent() == c) break;
		vg = NULL;
	}
	
	return vg;
}

EdgeGraphics * LayoutGraphView::getEdgeGraphics(Connector * c)
{
	if (this->connectorToGraphics.find(c) == this->connectorToGraphics.end()) return NULL;
	else return this->connectorToGraphics[c];
}

void LayoutGraphView::containerVisibilityGotToggled()
{
	this->update();
}

void LayoutGraphView::updateMyLayout(bool edge, bool fast)
{
	this->updateLayout(this->layout, edge, fast);
}