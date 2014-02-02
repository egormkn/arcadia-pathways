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
 *  PropertyGraphView.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 21/02/2008.
 *  Last documented on 17/04/2008.
 *
 */
 
#include "propertygraphview.h"

// local, contains info to display
#include "vertexproperty.h"
 
#include <iostream> 

#include <QLabel> 
 
 void PropertyGraphView::display(GraphModel * gModel)
{
	GraphView::display(gModel);
}

#ifdef tabletest
#include <QHeaderView>
#include <QTableWidget>

PropertyGraphView::PropertyGraphView(GraphController * c, QWidget * parent) : GraphView(c), QTabWidget(parent)
{
	std::list<BGL_Vertex> emptyList;
	this->select(emptyList);
		
	if (this->graphModel) this->display(this->graphModel);		
}

void PropertyGraphView::select(std::list<BGL_Vertex> vList)
{
///*
	// emptying the view
	std::list<QWidget *> wList;
	for (int i = 0; i < this->count(); ++i)
	{
		wList.push_back(this->widget(i));
	}
	this->clear();
	for (std::list<QWidget*>::iterator it = wList.begin(); it != wList.end(); ++it)
	{
		delete *it;
	}	

	// nothing to do if there's nothing selected
	if (vList.empty())
	{
		QLabel * widget = new QLabel(this);
		std::string text = "";
/*
		text += "Select one or several nodes to display their properties\n";
		text += "Drag and drop nodes to edit the layout manually\n";
		text += "Right click a node to display a local view\n";
		text += "Right click the background to switch back to the main view\n";
		text += "Double click a node to modify its layout depending on context\n";
		text += "Double click the background to update the whole layout\n";
*/
		text += "\n\n\n\n";
		widget->setText(text.c_str());
//		this->addTab(widget, "Mouse shortcuts");
		this->addTab(widget, "No selection");

		return;
	}

	// creating the tabs: need to determine how many there are
	typedef std::map< std::string, std::string > InfoMapType;
	typedef std::map< std::string, std::list< InfoMapType > > TypeMapType;

	// getting the tab list	
	TypeMapType typeMap;
	for (std::list<BGL_Vertex>::iterator it = vList.begin(); it != vList.end(); ++it)
	{
		VertexProperty* vp = this->graphModel->getProperties(*it);
		if (!vp)  continue;

		InfoMapType infoMap = vp->getInfoMap();

		typeMap[ infoMap["Type"] ].push_back(infoMap);
	}

	// populating the tabs
	for (TypeMapType::iterator it = typeMap.begin(); it != typeMap.end(); ++it)
	{
		QTableWidget * tableWidget = new QTableWidget(this);
		this->addTab(tableWidget, (*it).first.c_str());

		std::list< InfoMapType > infoList = (*it).second;
		tableWidget->setRowCount(infoList.size());

		// now defining the headers
		std::list< std::string > hList;
		for (std::list<InfoMapType>::iterator tt = infoList.begin(); tt != infoList.end(); ++tt)
		{ // looking at all the selected elements
			InfoMapType infoMap = (*tt);
			for (InfoMapType::iterator xt = infoMap.begin(); xt != infoMap.end(); ++xt)
			{ // looking at all individual attributes
				std::string h = (*xt).first;
				
				// we filter out certain types
				if (h == "Type") continue;
				
				if ( std::find(hList.begin(), hList.end(), h) == hList.end() )
					hList.push_back(h);
			}
		} // headers is the union of all attributes for all selected elements (in that type)

	 	tableWidget->setColumnCount(hList.size());
		QStringList headerList;
		for (std::list<std::string>::iterator tt = hList.begin(); tt != hList.end(); ++tt)
		{
			headerList << (*tt).c_str();
		}

		tableWidget->setHorizontalHeaderLabels(headerList);

		// now populating the table for good, based on the headers found
		int row = 0;
		for (std::list<InfoMapType>::iterator tt = infoList.begin(); tt != infoList.end(); ++tt)
		{ // for each row / selected element
			InfoMapType infoMap = (*tt);

			int col = 0;
			for (std::list< std::string >::iterator xt = hList.begin(); xt != hList.end(); ++xt)
			{ // looking at every header
				std::string text = infoMap[*xt];
				QTableWidgetItem *newItem = new QTableWidgetItem(text.c_str());
				tableWidget->setItem(row, col, newItem);
				col++;				
			}
			row++;
		}		
	
		tableWidget->setSortingEnabled(true);

//		tableWidget->resizeColumnsToContents();
//		tableWidget->resizeRowsToContents();

		tableWidget->horizontalHeader()->setCascadingSectionResizes (true);
		tableWidget->horizontalHeader()->setMovable (true);
		tableWidget->horizontalHeader()->setStretchLastSection(true);
		tableWidget->verticalHeader()->setCascadingSectionResizes (true);
		tableWidget->verticalHeader()->setMovable (true);

		tableWidget->setAlternatingRowColors (true);
		tableWidget->setTextElideMode ( Qt::ElideNone );

		tableWidget->setVerticalScrollMode( QAbstractItemView::ScrollPerItem );
		tableWidget->setHorizontalScrollMode( QAbstractItemView::ScrollPerItem );

//		tableWidget->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
//		tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
//		tableWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
	}
//*/
}
#endif

#ifndef tabletest
/**************
* Constructor *
***************
* Sets up the View's Controller and parent Widget
* Also selects an empty vertex list
*************************************************/
PropertyGraphView::PropertyGraphView(GraphController * c, QWidget * parent) : GraphView(c), QTextEdit(parent)
{
	this->setReadOnly(true);
	std::list<BGL_Vertex> emptyList;
	this->select(emptyList);
	
	if (this->graphModel) this->display(this->graphModel);
}

/*********
* select *
**********
* If the list is empty, displays a default text
* If not, for every vertex in the list
* displays the info contained in its properties
***********************************************/
void PropertyGraphView::select(std::list<BGL_Vertex> vList)
{
	this->clear();

	if (vList.empty()) 
	{
		this->insertPlainText("No item selected");
		return;
	}
	
	std::map<BGL_Vertex, bool> cloneBuster; // to avoid displaying the same properties twice
	// in case several clones of the same species are selected

	std::string info = "";
	for (std::list<BGL_Vertex>::iterator it = vList.begin(); it != vList.end(); ++it)
	{
		if (cloneBuster.find(*it) != cloneBuster.end()) continue;
		cloneBuster[*it] = true;
	
		VertexProperty* vp = this->graphModel->getProperties(*it);
	
		if (vp) info += vp->getInfo();
		else info += "Missing vertex properties";
		
		info += "\n";
	}
	
	this->insertPlainText(QString(info.c_str()));
}
#endif