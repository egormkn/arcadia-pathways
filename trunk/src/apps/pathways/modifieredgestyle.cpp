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
 *  ModifierEdgeStyle.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 21/01/2008.
 *	Last Documented on 17/04/2008.
 *
 */

#include "modifieredgestyle.h"

/******************************
* DefaultStyle: the Singleton *
******************************/
ModifierEdgeStyle* ModifierEdgeStyle::DefaultStyle = 0;

/*******************************
* GetDefaultStyle: the Factory *
*******************************/
ModifierEdgeStyle* ModifierEdgeStyle::GetDefaultStyle() {
	if (!ModifierEdgeStyle::DefaultStyle) ModifierEdgeStyle::DefaultStyle = new ModifierEdgeStyle();
	return ModifierEdgeStyle::DefaultStyle;
}

/***************************************
* ModifierEdgeStyle: green curvy edge  *
***************************************/
ModifierEdgeStyle::ModifierEdgeStyle() : EdgeStyle(64, 128, 64, true, noDeco, diamond) {}

////

StimulationModifierEdgeStyle* StimulationModifierEdgeStyle::DefaultStyle = 0;

StimulationModifierEdgeStyle* StimulationModifierEdgeStyle::GetDefaultStyle() {
	if (!StimulationModifierEdgeStyle::DefaultStyle) StimulationModifierEdgeStyle::DefaultStyle = new StimulationModifierEdgeStyle();
	return StimulationModifierEdgeStyle::DefaultStyle;
}

StimulationModifierEdgeStyle::StimulationModifierEdgeStyle() : ModifierEdgeStyle() { this->targetDecoration = emptyTriangle; }

////

CatalysisModifierEdgeStyle* CatalysisModifierEdgeStyle::DefaultStyle = 0;

CatalysisModifierEdgeStyle* CatalysisModifierEdgeStyle::GetDefaultStyle() {
	if (!CatalysisModifierEdgeStyle::DefaultStyle) CatalysisModifierEdgeStyle::DefaultStyle = new CatalysisModifierEdgeStyle();
	return CatalysisModifierEdgeStyle::DefaultStyle;
}

CatalysisModifierEdgeStyle::CatalysisModifierEdgeStyle() : ModifierEdgeStyle() { this->targetDecoration = circle; }

////

InhibitionModifierEdgeStyle* InhibitionModifierEdgeStyle::DefaultStyle = 0;

InhibitionModifierEdgeStyle* InhibitionModifierEdgeStyle::GetDefaultStyle() {
	if (!InhibitionModifierEdgeStyle::DefaultStyle) InhibitionModifierEdgeStyle::DefaultStyle = new InhibitionModifierEdgeStyle();
	return InhibitionModifierEdgeStyle::DefaultStyle;
}

InhibitionModifierEdgeStyle::InhibitionModifierEdgeStyle() : ModifierEdgeStyle() { this->targetDecoration = verticalline; }

////

TriggerModifierEdgeStyle* TriggerModifierEdgeStyle::DefaultStyle = 0;

TriggerModifierEdgeStyle* TriggerModifierEdgeStyle::GetDefaultStyle() {
	if (!TriggerModifierEdgeStyle::DefaultStyle) TriggerModifierEdgeStyle::DefaultStyle = new TriggerModifierEdgeStyle();
	return TriggerModifierEdgeStyle::DefaultStyle;
}

TriggerModifierEdgeStyle::TriggerModifierEdgeStyle() : ModifierEdgeStyle() { this->targetDecoration = crossedArrow; }