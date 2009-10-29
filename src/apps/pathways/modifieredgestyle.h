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
 *  ModifierEdgeStyle.h
 *  arcadia
 *
 *  Created by Alice Villeger on 05/06/2008.
 *	Last Documented never.
 *
 */

#ifndef MODIFIEREDGESTYLE_H
#define MODIFIEREDGESTYLE_H

// local base class
#include <arcadia/edgestyle.h>

/********************
* ModifierEdgeStyle *
*********************
* This subclass of EdgeStyle implements its own static Factory and Singleton
* Its constructor defines the default style of Edges of the Modifier type
****************************************************************************/
class ModifierEdgeStyle: public EdgeStyle
{
private:
	static ModifierEdgeStyle* DefaultStyle;
public:
	static ModifierEdgeStyle* GetDefaultStyle();

	ModifierEdgeStyle();	
};

class StimulationModifierEdgeStyle: public ModifierEdgeStyle
{
private:
	static StimulationModifierEdgeStyle* DefaultStyle;
public:
	static StimulationModifierEdgeStyle* GetDefaultStyle();

	StimulationModifierEdgeStyle();	
};

class CatalysisModifierEdgeStyle: public ModifierEdgeStyle
{
private:
	static CatalysisModifierEdgeStyle* DefaultStyle;
public:
	static CatalysisModifierEdgeStyle* GetDefaultStyle();

	CatalysisModifierEdgeStyle();	
};

class InhibitionModifierEdgeStyle: public ModifierEdgeStyle
{
private:
	static InhibitionModifierEdgeStyle* DefaultStyle;
public:
	static InhibitionModifierEdgeStyle* GetDefaultStyle();

	InhibitionModifierEdgeStyle();	
};

class TriggerModifierEdgeStyle: public ModifierEdgeStyle
{
private:
	static TriggerModifierEdgeStyle* DefaultStyle;
public:
	static TriggerModifierEdgeStyle* GetDefaultStyle();

	TriggerModifierEdgeStyle();	
};

#endif