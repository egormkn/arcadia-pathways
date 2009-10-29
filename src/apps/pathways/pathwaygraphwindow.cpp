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
 *  PathwayGraphWindow.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 27/05/2008.
 *	Last Documented never.
 *
 */

#include "pathwaygraphwindow.h"

#include "pathwaygraphcontroller.h"

PathwayGraphWindow::PathwayGraphWindow(bool fullVersion) : GraphWindow(fullVersion, new PathwayGraphController())
{
}

/*
#if utopia
// [!] utopia node
PathwayGraphWindow::PathwayGraphWindow(UTOPIA2::Node * model, std::string fileName, bool fullVersion) : GraphWindow(fullVersion, new PathwayGraphController(model, fileName), true)
{
}
#endif
*/