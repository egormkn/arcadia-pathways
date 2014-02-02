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

#include <QMessageBox>
#include <QFileDialog>

void PathwayGraphWindow::loadSBO(std::string dirName, bool fullPath)
{
	try
	{
		((PathwayGraphController*)this->controller)->setSBO(dirName, fullPath);
	}
	catch (std::exception &err)
	{
		std::string targetDir = dirName + "\\res\\";
		#ifdef MAC_COMPILATION
		targetDir = "arcadia.app/Contents/Resources/";
		#endif
	
		std::string title = "Cannot find SBO description file";

		std::string text = "SBO_XML.xml expected in " + targetDir;
		#ifdef MAC_COMPILATION
		text = title + ":\n" + text; // no title on Mac
		#endif

		std::string infoText = "Do you want to...\n\n";
		infoText += "- manually download the file from ";
		infoText += "http://www.ebi.ac.uk/sbo/exports/Main/SBO_XML.xml";
		infoText += " into " + targetDir + " then [Retry]?\n\n";
		infoText += "- [Open] an existing SBO description file from a different location on your machine?\n\n";
		infoText += "- [Ignore] the problem and disable SBO support for this session?";

		QMessageBox msgBox;
		msgBox.setWindowTitle(title.c_str());
		msgBox.setText(text.c_str());
		msgBox.setInformativeText(infoText.c_str());

		msgBox.setStandardButtons(QMessageBox::Retry | QMessageBox::Open | QMessageBox::Ignore);
		msgBox.setDefaultButton(QMessageBox::Retry);

		int ret = msgBox.exec();

		if (ret == QMessageBox::Retry)
		{
			this->loadSBO(dirName, false); // load from dir + usual path
		}
		if (ret == QMessageBox::Open)
		{
			// open dialog => new dirName
			std::string fileTypes = "XML Files (*.xml);;Any (*)";
			std::string path = dirName;
			#ifdef MAC_COMPILATION
			path += "/../Resources/";
			#endif

			QString openedFile = QFileDialog::getOpenFileName(NULL, QObject::tr("Open SBO File"), path.c_str(), QObject::tr(fileTypes.c_str()));
			if (openedFile.length()>0) dirName = openedFile.toStdString();
			this->loadSBO(dirName, openedFile.length()>0); // Load from provided full path (or retries if no file selected)
		}
		if (ret == QMessageBox::Ignore)
		{
			this->loadSBO("", true); // load empty ontology
		}
	}
}

PathwayGraphWindow::PathwayGraphWindow(bool fullVersion, std::string dirName, std::string fileName) : GraphWindow(fullVersion,  new PathwayGraphController(), false)
{
	this->loadSBO(dirName, false);
	this->loadGraph(fileName);
}

/*
#if utopia
// [!] utopia node
PathwayGraphWindow::PathwayGraphWindow(UTOPIA2::Node * model, std::string fileName, bool fullVersion) : GraphWindow(fullVersion, new PathwayGraphController(model, fileName), true)
{
}
#endif
*/