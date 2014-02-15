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
 *  ContainerGraphics.h
 *  arcadia
 *
 *  Created by Alice Villeger on 01/05/2009.
 *  Never documented yet.
 *
 */

#ifndef GRAPHGRAPHICS_H
#define GRAPHGRAPHICS_H

// Qt base class
#include <QGraphicsItem>

class GraphGraphics: public QGraphicsItem
{
public:
	virtual bool isEdge() { return false; }
	virtual bool isVertex() { return false; }
	virtual bool isContainer() { return false; }
};

#endif