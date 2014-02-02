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
 *  PathwayStyleSheet.h
 *  arcadia
 *
 *  Created by Alice Villeger on 04/06/2008.
 *	Last Documented never.
 *
 */

#ifndef PATHWAYSTYLESHEET_H
#define PATHWAYSTYLESHEET_H

#include <arcadia/stylesheet.h>

/********************
* PathwayStyleSheet *
*********************
* Links properties to appropriate styles
****************************************/
class PathwayStyleSheet: public StyleSheet
{
public:
	VertexStyle * getVertexStyle(VertexProperty * vp, CloneProperty cp);
	EdgeStyle * getEdgeStyle(EdgeProperty *ep);
	ContainerStyle * getContainerStyle(std::string type);
	
	static int UnspecifiedEntity;
	static int SimpleChemical;
	static int Macromolecule;
	static int NucleicAcidFeature;
	static int Multimer;
	static int SourceSink;
	static int Observable;
	static int Perturbation;
	static int PerturbingAgent;
	static int Complex;

	static int Transition;
	static int Association;
	static int Dissociation;
	
	static int Modulation;
	static int Stimulation;
	static int Catalysis;
	static int Inhibition;
	static int Trigger;

	static int ModulationActor;
	static int StimulationActor;
	static int CatalysisActor;
	static int InhibitionActor;
	static int TriggerActor;

	static int AndOperator;
	static int OrOperator;
	static int NotOperator;	
};

#endif