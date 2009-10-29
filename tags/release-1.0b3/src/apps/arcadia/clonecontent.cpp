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
 *  CloneContent.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 27/02/2008.
 *	Last Documented on 17/04/2008.
 *
 */

#include "clonecontent.h"

// local
#include "graphlayout.h"
#include "containercontent.h"
#include "connector.h"
// to get proper dimensions
#include "stylesheet.h"
#include "vertexstyle.h"
#include "vertexproperty.h"

#include <iostream>

/**************
* Constructor *
***************
* Usual content initialisation (Container, can be NULL)
* Also sets up the Vertex and Layout (not NULL)
*
* Puts the Clone at a neutral position
* Maps the Clone to its Vertex in the Layout
*
* If no Container has been given,
* defines the default Container as the Layout's root
*******************************************************/
CloneContent::CloneContent(BGL_Vertex v, GraphLayout * l, ContainerContent * c) : Content(l, c), vertex(v), rotated(false), inverted(false)
{
	this->label =  this->layout->getGraphModel()->getProperties(this->vertex)->getLabel();
	if (this->label.size() > 100)
	{
		std::string shortLabel = "";
		bool start = true;
		std::string cur = "";
		for (int i = 0; i < label.size(); ++i)
		{
			if (label.at(i) == ' ' || label.at(i) == '-' || label.at(i) == ',') { start = true; cur = label.at(i); } 
			else if (start) { shortLabel += cur + label.at(i); start = false; }
		}
		this->label = shortLabel;
	}

	this->setPosition(0,0);
	this->layout->map(v, this);
	if (!this->getContainer()) this->setContainer(this->layout->getRoot());
}

/*************
* Destructor *
**************
* Deletes every Connector still connected to the Clone
* Removes the reference to the Clone in its Layout
******************************************************/
CloneContent::~CloneContent()
{
	std::list<Connector *> connectors = this->getOutterConnectors();
	for(std::list<Connector *>::iterator it = connectors.begin(); it != connectors.end(); ++it)
	{
		delete *it;
	}
	
	this->layout->unmap(this);
}

/***********************************************************************
* Vertex access                                                        *
***********************************************************************/

/************
* getVertex *
*************
* returns the vertex
************************/
BGL_Vertex CloneContent::getVertex()
{
	return this->vertex;
}

/***********************************************************************
* Position: abscisse and ordinate                                      *
***********************************************************************/

/**************
* setPosition *
***************
* updates x and y with new values
*********************************/
void CloneContent::setPosition(int x, int y)
{
	Content::setPosition(x, y);

	this->_x = x;
	this->_y = y;
}

int CloneContent::x() { return this->x(neutral); }
int CloneContent::y() { return this->y(neutral); }

/**************************
* x: returns the abscisse *
***************************/
int CloneContent::x(ConnectionType ct)
{
	switch (ct)
	{
	case in:
		if (this->rotated)
			if (this->inverted)	return this->right(false);
			else return this->left(false);
		else return this->_x;
		break;

	case out:
		if (this->rotated)
			if (this->inverted)	return this->left(false);
			else return this->right(false);
		else return this->_x;
		break;
		
	case side1:
		if (!this->rotated)
			if (this->inverted)	return this->right(false)+5;
			else return this->left(false)-5;
		else return this->_x;
		break;
	
	case side2:
		if (!this->rotated)
			if (this->inverted)	return this->left(false)-5;
			else return this->right(false)+5;
		else return this->_x;
		break;

	case neutral:
	default:
		return this->_x;
		break;
	}
}
/**************************
* y: returns the ordinate *
***************************/
int CloneContent::y(ConnectionType ct)
{
	switch (ct)
	{
	case in:
		if (this->rotated) return this->_y;
		else if (this->inverted) return this->bottom(false);
		else return this->top(false);
		break;

	case out:
		if (this->rotated) return this->_y;
		else if (this->inverted) return this->top(false);
		else return this->bottom(false);
		break;

	case side1:
		if (!this->rotated) return this->_y;
		else if (this->inverted) return this->bottom(false)+5;
		else return this->top(false)-5;
		break;

	case side2:
		if (!this->rotated) return this->_y;
		else if (this->inverted) return this->top(false)-5;
		else return this->bottom(false)+5;
		break;

	case neutral:
	default:
		return this->_y;
		break;
	}
}

/***********************************************************************
* Position and dimensions:                                             *
* the borders of the bounding rectangle, assuming (x,y) is the centre  *
***********************************************************************/

/*******************************
* left: depends on x and width *
********************************/
int CloneContent::left(bool withMargin) { return this->x() - this->width(withMargin)/2; }
/********************************
* right: depends on x and width *
*********************************/
int CloneContent::right(bool withMargin) {
	return this->x() + this->width(withMargin)/2;
}

/*******************************
* top: depends on y and height *
********************************/
int CloneContent::top(bool withMargin) { return this->y() - this->height(withMargin)/2; }
/**********************************
* bottom: depends on y and height *
***********************************/
int CloneContent::bottom(bool withMargin) { return this->y() + this->height(withMargin)/2; }

/***********************************************************************
* Dimensions: required for layout purposes, include margin and padding *
***********************************************************************/

/********************************************************
* width [!] default values, would need style sheet info *
*********************************************************/
int CloneContent::width(bool withMargin)
{
	VertexProperty * vp = this->layout->getGraphModel()->getProperties(this->vertex);
	CloneProperty cp;
	if (this->getNeighbours().size() > 0) cp = isClone;
	else if (this->getContainer()->getTypeLabel() == "CloneContainer") cp = midget;
	else cp = notClone;
//	if (this->rotated) cp = rotated; // overload
	VertexStyle * vs = this->layout->getStyleSheet()->getVertexStyle(vp, cp);

	if (withMargin) return vs->getFullWidth(this->label); // [!] what if the displayed label is not the property's label? Clones should store labels of their own, to be displayed by the VertexGraphics...
	else return vs->getBoundingWidth(this->label);;
}
/*********************************************************
* height [!] default values, would need style sheet info *
**********************************************************/
int CloneContent::height(bool withMargin)
{
	VertexProperty * vp = this->layout->getGraphModel()->getProperties(this->vertex);
	CloneProperty cp;
	if (this->getNeighbours().size()) cp = isClone;
	else if (this->getContainer()->getTypeLabel() == "CloneContainer") cp == midget;
	else cp = notClone;
//	if (this->rotated) cp = rotated; // overload
	VertexStyle * vs = this->layout->getStyleSheet()->getVertexStyle(vp, cp);
	
	if (withMargin) return vs->getFullHeight(this->label); // [!] what if the displayed label is not the property's label? Clones should store labels of their own, to be displayed by the VertexGraphics...
	else return vs->getBoundingHeight(this->label);
}

/***********************************************************************
* Neighbour management: defines which vertices the Clone is linked to  *
***********************************************************************/

/****************
* getNeighbours *
*****************
* returns the list of neighbours
********************************/
std::list<BGL_Vertex> CloneContent::getNeighbours()
{
	return this->neighbours;
}
		
/***************
* addNeighbour *
****************
* adds a neighbour to the list
******************************/
void CloneContent::addNeighbour(BGL_Vertex v)
{
	this->neighbours.push_back(v);
}

/******************
* removeNeighbour *
*******************
* removes a neighbour to the list
*********************************/
void CloneContent::removeNeighbour(BGL_Vertex v)
{
	this->neighbours.remove(v);
}

/***********************************************************************
* Connector management [!] redundant with neighbours?? not totally...? *
***********************************************************************/
	
/**********************
* getOutterConnectors *
***********************
* returns the list of connectors
********************************/
std::list<Connector*> CloneContent::getOutterConnectors()
{
	return this->cList;
}

/***************
* addConnector *
****************
* adds a connector to the list
* [!] doesn't check the connector actually points to the Clone
* it's the connector that checks this (and calls this method)
**************************************************************/	
void CloneContent::addConnector(Connector *c)
{
	this->cList.push_back(c);
}

/******************
* removeConnector *
*******************
* removes a connector from the list
* [!] doesn't check the connector actually points to the Clone
* it's the connector that checks this (and calls this method)
**************************************************************/	
void CloneContent::removeConnector(Connector *c)
{
	this->cList.remove(c);
}

std::list<CloneContent *> CloneContent::getNeighbourClones()
{
	std::list<CloneContent *> nClones;
	for (std::list<Connector *>::iterator it = this->cList.begin(); it != this->cList.end(); ++it)
	{
		nClones.push_back((*it)->getNeighbour(this));
	}
	return nClones;
}