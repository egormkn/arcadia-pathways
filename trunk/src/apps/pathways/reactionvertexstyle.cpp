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
 *  ReactionVertexStyle.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 04/06/2008.
 *  Last documented never.
 *
 */

#include "reactionvertexstyle.h"

/******************************
* DefaultStyle: the Singleton *
*******************************/
ReactionVertexStyle* ReactionVertexStyle::DefaultStyle = NULL;

/******************
* GetDefaultStyle *
*******************
* Possibly creates, then returns
* the Singleton
********************************/
ReactionVertexStyle* ReactionVertexStyle::GetDefaultStyle() {
	if (!ReactionVertexStyle::DefaultStyle) ReactionVertexStyle::DefaultStyle = new ReactionVertexStyle();
	return ReactionVertexStyle::DefaultStyle;
}

/*******************************************
* ReactionVertexStyle: a Magenta Rectangle *
********************************************/
ReactionVertexStyle::ReactionVertexStyle() : VertexStyle(255, 0, 255, squareIOVert) { this->showLabel = false; }

/******************************
* DefaultStyle: the Singleton *
*******************************/
RotatedReactionVertexStyle* RotatedReactionVertexStyle::DefaultStyle = NULL;

/******************
* GetDefaultStyle *
*******************
* Possibly creates, then returns
* the Singleton
********************************/
RotatedReactionVertexStyle* RotatedReactionVertexStyle::GetDefaultStyle() {
	if (!RotatedReactionVertexStyle::DefaultStyle) RotatedReactionVertexStyle::DefaultStyle = new RotatedReactionVertexStyle();
	return RotatedReactionVertexStyle::DefaultStyle;
}

/*******************************************
* ReactionVertexStyle: a Magenta Rectangle *
********************************************/
RotatedReactionVertexStyle::RotatedReactionVertexStyle() : VertexStyle(255, 0, 255, squareIOHori) { this->showLabel = false; }

//////////////////////
// Association
//////////////////////

AssociationReactionVertexStyle* AssociationReactionVertexStyle::DefaultStyle = NULL;

AssociationReactionVertexStyle* AssociationReactionVertexStyle::GetDefaultStyle() {
	if (!AssociationReactionVertexStyle::DefaultStyle) AssociationReactionVertexStyle::DefaultStyle = new AssociationReactionVertexStyle();
	return AssociationReactionVertexStyle::DefaultStyle;
}

AssociationReactionVertexStyle::AssociationReactionVertexStyle() : VertexStyle(255, 0, 255, circleIOVert) { this->showLabel = false; this->fillInside = background; }

//////////////////////
// Association
//////////////////////

RotatedAssociationReactionVertexStyle* RotatedAssociationReactionVertexStyle::DefaultStyle = NULL;

RotatedAssociationReactionVertexStyle* RotatedAssociationReactionVertexStyle::GetDefaultStyle() {
	if (!RotatedAssociationReactionVertexStyle::DefaultStyle) RotatedAssociationReactionVertexStyle::DefaultStyle = new RotatedAssociationReactionVertexStyle();
	return RotatedAssociationReactionVertexStyle::DefaultStyle;
}

RotatedAssociationReactionVertexStyle::RotatedAssociationReactionVertexStyle() : VertexStyle(255, 0, 255, circleIOHori) { this->showLabel = false; this->fillInside = background; }

//////////////////////
// Dissociation
//////////////////////

DissociationReactionVertexStyle* DissociationReactionVertexStyle::DefaultStyle = NULL;

DissociationReactionVertexStyle* DissociationReactionVertexStyle::GetDefaultStyle() {
	if (!DissociationReactionVertexStyle::DefaultStyle) DissociationReactionVertexStyle::DefaultStyle = new DissociationReactionVertexStyle();
	return DissociationReactionVertexStyle::DefaultStyle;
}

DissociationReactionVertexStyle::DissociationReactionVertexStyle() : VertexStyle(255, 0, 255, donutIOVert) { this->showLabel = false; }

//////////////////////
// Dissociation
//////////////////////

RotatedDissociationReactionVertexStyle* RotatedDissociationReactionVertexStyle::DefaultStyle = NULL;

RotatedDissociationReactionVertexStyle* RotatedDissociationReactionVertexStyle::GetDefaultStyle() {
	if (!RotatedDissociationReactionVertexStyle::DefaultStyle) RotatedDissociationReactionVertexStyle::DefaultStyle = new RotatedDissociationReactionVertexStyle();
	return RotatedDissociationReactionVertexStyle::DefaultStyle;
}

RotatedDissociationReactionVertexStyle::RotatedDissociationReactionVertexStyle() : VertexStyle(255, 0, 255, donutIOHori) { this->showLabel = false; }