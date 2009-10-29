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
 *  ModelGraphView.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
 
#include "modelgraphview.h"

#include <iostream>

// Qt Widgets & co
#include <QToolButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsView>
#include <QTabBar>
#include <QTabWidget>
#include <QLabel>

// for the pow function in resetMatrix
#include <math.h>

// local
#include "layoutgraphview.h"
#include "graphlayout.h"
#include "graphcontroller.h"

/**************
* Constructor *
***************
* Sets up the View's Controller and Frame's parent
* [!] is the controller even useful??? should this be a GraphView?
* Creates and lays out all the internal Widgets:
* - a tab bar for switching between Scenes
* - an interactive-or-not QGraphicsView to look at the current Scene
* - a set of Slider + Buttons for zooming in/out of the Scene
* Then we reset the view
*/
ModelGraphView::ModelGraphView(GraphController * c,  bool zoomEnabled, QWidget * parent) : GraphView(c), zoom(zoomEnabled), QFrame(parent), currentScene(0), resetting(false) 
{
    QVBoxLayout *topLayout = new QVBoxLayout();
	topLayout->setContentsMargins (0,0,0,0);

	this->tabWidget = new QTabWidget();	
	this->tabWidget->setElideMode(Qt::ElideNone);
	this->tabWidget->setUsesScrollButtons(true);
	this->tabWidget->setTabsClosable(true);
	QObject::connect(this->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(sceneSwitched(int)));
	QObject::connect(this->tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(layoutDestroyed()));

	TabbedWidget * tw = new TabbedWidget(NULL, this->zoom);
	this->tabWidget->addTab(tw, QString("Empty"));
	
	topLayout->addWidget(this->tabWidget);
    this->setLayout(topLayout);
		
	this->display(this->graphModel);
}

void ModelGraphView::layoutDestroyed()
{
	this->getController()->destroyLayout();
}


#include <fstream>

/**********
* display *
***********
* Sets the model as the GraphView's GraphModel
* then resets the local viewport and destroys existing scenes
* Also removes every tabs in the tab bar
*
* Then for every layout in the model,
* creates a scene displaying that model's layout
* add a tab for it and switch to that scene
* [!] it really seems there is no point in making this class a GraphView itself
*******************************************************************************/
void ModelGraphView::display(GraphModel *gModel)
{
	GraphView::display(gModel);

	if (!this->graphModel) return;

	// resetting stuff
// /*
	this->resetting = true;
	while (this->tabWidget->count())
	{
		TabbedWidget * tw = (TabbedWidget*)(this->tabWidget->widget(0));
		this->tabWidget->removeTab(0);
		delete tw->getScene();
		delete tw;
	}

	this->resetting = false;
// */

/*
	TabbedWidget * tw = (TabbedWidget*)(this->tabWidget->widget(0));
	delete tw->getScene();
*/

//	this->tabWidget->clear();

/*
	tw->setScene(this->controller, 0);
*/

// /*

	// handling layouts	
	unsigned int layoutNumber = this->graphModel->layoutNumber();
	for (unsigned int l = 0; l < layoutNumber; ++l) 
	{	
		TabbedWidget * tw = new TabbedWidget(NULL, this->zoom);

		tw->setScene(this->controller, l);

		std::string name = tw->getScene()->getLayout()->name;

		this->tabWidget->addTab(tw, QString(name.c_str()));

		this->switchScene(l);
	}
	
// */
}

/**************
* switchScene *
***************
* first we disconnect the current scene, signal wise (centering system)
* If not real scene number is given (-1) we take the scene number n+1 (%N)
* We set up the new scene and update it
* We also update the scene tab if necessary
* Finally we connect the new scene, signal wise
*************************************************************************/
void ModelGraphView::switchScene(int scene) 
{
	int newIndex = (scene < 0) ? (this->tabWidget->currentIndex()+1)%this->tabWidget->count() : scene;
	if (newIndex >= this->tabWidget->count()) return;

	if ( this->tabWidget->currentIndex() != newIndex ) this->tabWidget->setCurrentIndex(newIndex);
	else // [!] a quick fix, so that when we load a single layout, it is set to visible (by default the current index is zero!)
	{
		TabbedWidget * tw = (TabbedWidget *)(this->tabWidget->widget(newIndex));
		tw->getScene()->setVisible(true);
	}
}

void ModelGraphView::selectLayout(GraphLayout * gl)
{
	unsigned int tabNumber = this->tabWidget->count();
	unsigned int l;
	for (l = 0; l < tabNumber; ++l)
	{
		TabbedWidget * widget = (TabbedWidget*) this->tabWidget->widget(l);
		if (gl == widget->getScene()->getLayout()) break;
	}
	if (l < tabNumber) this->switchScene(l);
}

#include "graphcontroller.h"

void ModelGraphView::sceneSwitched(int scene) 
{
	TabbedWidget * tw = (TabbedWidget *)(this->tabWidget->widget(this->currentScene));
	if (tw && tw->getScene()) tw->getScene()->setVisible(false);
	this->currentScene = this->tabWidget->currentIndex();
	tw = (TabbedWidget *)(this->tabWidget->widget(this->currentScene));
	if (tw && tw->getScene()) tw->getScene()->setVisible(true);
	if (tw && tw->getScene()) tw->getScene()->update();

	if (tw && tw->getScene() && !this->resetting)
	{
		this->controller->selectLayout(tw->getScene()->getLayout(), this);
	}
}

void ModelGraphView::layoutGotAdded()
{
	this->displayLayout(this->graphModel->layoutNumber() - 1);
}

void ModelGraphView::displayLayout(int l)
{
	TabbedWidget * tw = new TabbedWidget(NULL, this->zoom);
	tw->setScene(this->controller, l);
	
	std::string name = tw->getScene()->getLayout()->name;
	this->tabWidget->addTab(tw, QString(name.c_str()));
		
	this->switchScene(l);
}

void ModelGraphView::layoutGotRemoved(GraphLayout * gl)
{
	TabbedWidget * tw = NULL;
	int i;
	for (i = 0; i < this->tabWidget->count(); ++i)
	{
		tw = (TabbedWidget*)(this->tabWidget->widget(i));
		if (tw->getScene()->getLayout() == gl)
		{
			this->removeLayout(i);
			break;
		}
	}
}

void ModelGraphView::removeLayout(int i)
{
	TabbedWidget * tw = (TabbedWidget*)(this->tabWidget->widget(i));
	this->tabWidget->removeTab(i);
	delete tw->getScene();
	delete tw;	
}

/*
    void setScene(QGraphicsScene *scene) {
		QObject::disconnect(scene, SIGNAL(sceneSwitch(QGraphicsScene*)), this, SLOT(setScene(QGraphicsScene*)));

		graphicsView->setScene(scene);

		QObject::connect(scene, SIGNAL(sceneSwitch(QGraphicsScene*)), this, SLOT(setScene(QGraphicsScene*)));
		QObject::connect(this, SIGNAL(newFocus()), scene, SLOT(checkCenter()) );
		emit(newFocus());
	}
*/

#include <QToolBar>
#include <QAction>
#include <QCheckBox>
#include <QPushButton>

TabbedWidget::TabbedWidget(QWidget * parent, bool zoomEnabled): QWidget(parent), scene(NULL), modCloned(false), busy(false)
{
    QVBoxLayout *topLayout = new QVBoxLayout();
	topLayout->setContentsMargins (0,0,0,0);
	
	this->mapView = new QGraphicsView();
	this->mapView->setRenderHint(QPainter::Antialiasing, false);
	this->mapView->setDragMode(QGraphicsView::RubberBandDrag);
	this->mapView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	this->mapView->setCacheMode(QGraphicsView::CacheBackground);
	this->mapView->setInteractive(true); // [!] or zoomEnabled?
	this->mapView->setResizeAnchor(QGraphicsView::AnchorViewCenter);

    topLayout->addWidget(this->mapView);
	
    QHBoxLayout *horiLayout = new QHBoxLayout();
	horiLayout->setContentsMargins (0,0,0,0);

	// creating the layout toolbar
	QToolBar * toolBar = new QToolBar(this);
	QAction * action;

	action = this->createAction("&Route Edges", "Ctrl+R", "Toggles automatic edge routing");
	QObject::connect(action, SIGNAL( triggered() ), this, SLOT( toggleAvoidingEdges() ));
	action->setCheckable(true);
	action->setChecked(false);
	toolBar->addAction(action);

	toolBar->addSeparator();

	action = this->createAction("Layout Modifiers", "", "");
	QObject::connect(action, SIGNAL(triggered()), this, SLOT( cloneModifiers() ));	
	toolBar->addAction(action);

	action = this->createAction("Arrange Selection", "", "");
	QObject::connect(action, SIGNAL(triggered()), this, SLOT( arrangeSelection() ));	
	toolBar->addAction(action);

	toolBar->addSeparator();

	action = this->createAction("|<", "", "");
	QObject::connect(action, SIGNAL(triggered()), this, SLOT( farLeft() ));	
	toolBar->addAction(action);

	action = this->createAction("<", "", "");
	QObject::connect(action, SIGNAL(triggered()), this, SLOT( left() ));	
	toolBar->addAction(action);

	action = this->createAction(">", "", "");
	QObject::connect(action, SIGNAL(triggered()), this, SLOT( right() ));	
	toolBar->addAction(action);

	action = this->createAction(">|", "", "");
	QObject::connect(action, SIGNAL(triggered()), this, SLOT( farRight() ));	
	toolBar->addAction(action);

	action = this->createAction("Select layout", "", "");
	QObject::connect(action, SIGNAL(triggered()), this, SLOT( newLayout() ));	
	toolBar->addAction(action);

	action = this->createAction("Update layout", "", "");
	QObject::connect(action, SIGNAL(triggered()), this, SLOT( updateLayout() ));	
	toolBar->addAction(action);

/* // now on the tabbar
	action = this->createAction("Destroy layout", "", "");
	QObject::connect(action, SIGNAL(triggered()), this, SLOT( destroyLayout() ));	
	toolBar->addAction(action);
*/

	horiLayout->addWidget(toolBar);

	// dealing with the zoom bar
	if (zoomEnabled)
	{
/*
		QToolButton *zoomInIcon = new QToolButton();
		zoomInIcon->setText("+");
		zoomInIcon->setAutoRepeat(true);
		zoomInIcon->setAutoRepeatDelay(100);
		zoomInIcon->setAutoRepeatInterval(100);

		QToolButton *zoomOutIcon = new QToolButton();
		zoomOutIcon->setText("-");
		zoomOutIcon->setAutoRepeat(true);
		zoomOutIcon->setAutoRepeatDelay(100);
		zoomOutIcon->setAutoRepeatInterval(100);
*/
		QLabel * zoomLabel = new QLabel("Zoom", this);

		this->zoomSlider = new QSlider(Qt::Horizontal);
		this->zoomSlider->setMinimum(0);
		this->zoomSlider->setMaximum(100);
		this->zoomSlider->setTickInterval(this->zoomSlider->maximum()/20);
		this->zoomSlider->setTickPosition(QSlider::NoTicks);
	
		this->zoomSlider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		this->zoomSlider->updateGeometry();
	
		QVBoxLayout *zoomSliderLayout = new QVBoxLayout();
//		zoomSliderLayout->addWidget(zoomOutIcon);
		zoomSliderLayout->addWidget(this->zoomSlider);
//		zoomSliderLayout->addWidget(zoomInIcon);
	
		QObject::connect(zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(setupMatrix()));
//		QObject::connect(zoomInIcon, SIGNAL(clicked()), this, SLOT(zoomIn()));
//		QObject::connect(zoomOutIcon, SIGNAL(clicked()), this, SLOT(zoomOut()));

		horiLayout->addStretch();
		horiLayout->addWidget(zoomLabel);
		horiLayout->addLayout(zoomSliderLayout);
	}
	else this->zoomSlider = NULL;

	topLayout->addLayout(horiLayout);
	
    this->setLayout(topLayout);

	this->resetView();
}

void TabbedWidget::left()
{
	GraphLayout * layout = this->scene->getLayout();
	this->scene->getController()->moveToLayout(layout, -1);
}

void TabbedWidget::right()
{
	GraphLayout * layout = this->scene->getLayout();
	this->scene->getController()->moveToLayout(layout, +1);
}

void TabbedWidget::farLeft()
{
	GraphLayout * layout = this->scene->getLayout();
	this->scene->getController()->moveToLayout(layout, -2);
}

void TabbedWidget::farRight()
{
	GraphLayout * layout = this->scene->getLayout();
	this->scene->getController()->moveToLayout(layout, +2);
}

void TabbedWidget::cloneModifiers()
{
	this->scene->getController()->toggleModifiersCloning();
	modCloned = !modCloned;
}

void TabbedWidget::newLayout()
{
	this->scene->getController()->newLayout();
}

void TabbedWidget::updateLayout()
{
	this->scene->getController()->updateLayout(NULL, false, false);
}

void TabbedWidget::destroyLayout()
{
	this->scene->getController()->destroyLayout();
}

void TabbedWidget::arrangeSelection()
{
	this->scene->getController()->arrangeSelection();
}

QAction * TabbedWidget::createAction(const char * name, const char * shortCut, const char * tip)
{
	QAction * action = new QAction(tr(name), this);
	if (shortCut != "") action->setShortcut(tr(shortCut));
	if (tip != "") action->setStatusTip(tr(tip));
	return action;
}

void TabbedWidget::toggleAvoidingEdges()
{
	this->scene->getController()->toggleAvoidingEdges();
}

/*****************
* exportGraphics *
******************
* Called by GraphWindow[!]?
* It just calls the scenes' method
**********************************/
void ModelGraphView::exportGraphics(std::string filename)
{
	for(unsigned int i = 0; i < this->tabWidget->count(); ++i)
	{
		TabbedWidget * tw = (TabbedWidget *)(this->tabWidget->widget(i));
		tw->getScene()->exportGraphics(filename);
	}
}

/************************************************************************************************
* changeCenter: Called by the current Scene newCenter signal, changes the center of the mapView *
************************************************************************************************/
void TabbedWidget::changeCenter(QPointF p)
{
	if (this->busy == true) return; // this prevents the app to slow down when moving the node very fast
	
	this->busy = true;
	
	int size = 1;
	this->mapView->ensureVisible(QRectF(p.x()-size/2,p.y()-size/2, size,size));
//	this->mapView->centerOn(p);
	
	this->busy = false;
}

/************
* resetView *
*************
* sets up the zoom to a maximum
* sets up the matrix accordingly
* scroll to make the origin visible
* sets the current scene as the 1st one
***************************************/
void TabbedWidget::resetView()
{
	if (this->zoomSlider) this->zoomSlider->setValue(this->zoomSlider->maximum());
	this->setupMatrix();
    this->mapView->ensureVisible(QRectF(0, 0, 0, 0));
}

/**************
* setupMatrix *
***************
* sets up the current GraphicsView matrix
* according to the value on the zoom slider
*******************************************/
void TabbedWidget::setupMatrix()
{
    QMatrix matrix;
	
	if (this->zoomSlider) {
		qreal scale = ::pow(2.0, (this->zoomSlider->value() - this->zoomSlider->maximum())*4./this->zoomSlider->maximum());
		matrix.scale(scale, scale);
	}

	this->mapView->setMatrix(matrix);
}

/********************************************
* zoomIn: increments the zoomSlider's value *
********************************************/
void TabbedWidget::zoomIn() { if (this->zoomSlider) this->zoomSlider->setValue(this->zoomSlider->value() + 1); }

/*********************************************
* zoomOut: decrements the zoomSlider's value *
*********************************************/
void TabbedWidget::zoomOut() { if (this->zoomSlider) this->zoomSlider->setValue(this->zoomSlider->value() - 1); }

void TabbedWidget::setScene(GraphController * c, int l)
{
	this->scene = new LayoutGraphView(c, l);

	this->mapView->setScene(this->scene);

	this->scene->update();

	QObject::connect(this->scene, SIGNAL(newCenter(QPointF)), this, SLOT(changeCenter(QPointF)));
}

TabbedWidget::~TabbedWidget()
{
//	if (this->scene) delete this->scene;
// [!] problem here! the scenes should be deleted not automatically along with the widget, but only when they are removed manually
// (if not, during the big delete at the end, both the widget and the controller try to delete the scene => seg fault)
}