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

/**
* Arcadia 2008
*/ 

// Qt application framework
#include <QApplication>

// Local main window class
#include <pathways/pathwaygraphwindow.h>

#include <iostream>

/*******
* main *
********
* Creates an ArcadiaWindow
**************************/
int main(int argc, char * argv[])
{
    QApplication app(argc, argv);

	bool interactive = true;

	// to find the path to the SBO file
	std::string dir = argv[0];
	int endOfDir = dir.find_last_of('\\');
	#ifdef MAC_COMPILATION
	endOfDir = dir.find_last_of('/');
	#endif
	dir = dir.substr(0, endOfDir);

	// to open the required model
	std::string file = "";
	if (argc > 1) file = argv[1];
	PathwayGraphWindow pathwayGraphWindow(interactive, dir, file);
	
	return app.exec();
}