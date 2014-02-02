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
 *  ReactionVertexStyle.h
 *  arcadia
 *
 *  Created by Alice Villeger on 04/06/2008.
 *  Last documented never.
 *
 */

#ifndef REACTIONVERTEXSTYLE_H
#define REACTIONVERTEXSTYLE_H

// local
#include <arcadia/vertexstyle.h>

/**********************
* ReactionVertexStyle *
***********************
* This subclass of VertexStyle
* defines the default style
* for a Vertex of the Reaction type
***********************************/
class ReactionVertexStyle: public VertexStyle {
public:
	static ReactionVertexStyle* GetDefaultStyle();
	ReactionVertexStyle();
private:
	static ReactionVertexStyle* DefaultStyle;
};

// [!] ugly hack. I need to determine a new way to deal with slight alterations of style
class RotatedReactionVertexStyle: public VertexStyle {
public:
	static RotatedReactionVertexStyle* GetDefaultStyle();
	RotatedReactionVertexStyle();
private:
	static RotatedReactionVertexStyle* DefaultStyle;
};

class AssociationReactionVertexStyle: public VertexStyle {
public:
	static AssociationReactionVertexStyle* GetDefaultStyle();
	AssociationReactionVertexStyle();
private:
	static AssociationReactionVertexStyle* DefaultStyle;
};

// [!] ugly hack. I need to determine a new way to deal with slight alterations of style
class RotatedAssociationReactionVertexStyle: public VertexStyle {
public:
	static RotatedAssociationReactionVertexStyle* GetDefaultStyle();
	RotatedAssociationReactionVertexStyle();
private:
	static RotatedAssociationReactionVertexStyle* DefaultStyle;
};

class DissociationReactionVertexStyle: public VertexStyle {
public:
	static DissociationReactionVertexStyle* GetDefaultStyle();
	DissociationReactionVertexStyle();
private:
	static DissociationReactionVertexStyle* DefaultStyle;
};

// [!] ugly hack. I need to determine a new way to deal with slight alterations of style
class RotatedDissociationReactionVertexStyle: public VertexStyle {
public:
	static RotatedDissociationReactionVertexStyle* GetDefaultStyle();
	RotatedDissociationReactionVertexStyle();
private:
	static RotatedDissociationReactionVertexStyle* DefaultStyle;
};

#endif