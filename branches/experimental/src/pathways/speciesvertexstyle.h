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
 *  SpeciesVertexStyle.h
 *  arcadia
 *
 *  Created by Alice Villeger on 04/06/2008.
 *  Last documented never.
 *
 */

#ifndef SPECIESVERTEXSTYLE_H
#define SPECIESVERTEXSTYLE_H

// local
#include <arcadia/vertexstyle.h>

/*********************
* SpeciesVertexStyle *
**********************
* This subclass of VertexStyle
* defines the default style
* for a Vertex of the Species type
**********************************/
class SpeciesVertexStyle: public VertexStyle
{
public:
	static SpeciesVertexStyle* GetDefaultStyle();
	SpeciesVertexStyle();
private:
	static SpeciesVertexStyle* DefaultStyle;
}; // by default, metabolites

class ClonedSpeciesVertexStyle: public VertexStyle
{
public:
	static ClonedSpeciesVertexStyle* GetDefaultStyle();
	ClonedSpeciesVertexStyle();
private:
	static ClonedSpeciesVertexStyle* DefaultStyle;
};

class MidgetSpeciesVertexStyle: public SpeciesVertexStyle
{
public:
	static MidgetSpeciesVertexStyle* GetDefaultStyle();
	MidgetSpeciesVertexStyle();
private:
	static MidgetSpeciesVertexStyle* DefaultStyle;
};

////////////////////////////////////////////////////////////

class EnzymeSpeciesVertexStyle: public SpeciesVertexStyle
{
public:
	static EnzymeSpeciesVertexStyle* GetDefaultStyle();
	EnzymeSpeciesVertexStyle();
private:
	static EnzymeSpeciesVertexStyle* DefaultStyle;
};

class MidgetEnzymeSpeciesVertexStyle: public EnzymeSpeciesVertexStyle
{
public:
	static MidgetEnzymeSpeciesVertexStyle* GetDefaultStyle();
	MidgetEnzymeSpeciesVertexStyle();
private:
	static MidgetEnzymeSpeciesVertexStyle* DefaultStyle;
};

class ClonedEnzymeSpeciesVertexStyle: public ClonedSpeciesVertexStyle
{
public:
	static ClonedEnzymeSpeciesVertexStyle* GetDefaultStyle();
	ClonedEnzymeSpeciesVertexStyle();
private:
	static ClonedEnzymeSpeciesVertexStyle* DefaultStyle;
};

////////////////////////////////////////////////////////////

class TransporterSpeciesVertexStyle: public SpeciesVertexStyle
{
public:
	static TransporterSpeciesVertexStyle* GetDefaultStyle();
	TransporterSpeciesVertexStyle();
private:
	static TransporterSpeciesVertexStyle* DefaultStyle;
};

class  MidgetTransporterSpeciesVertexStyle: public TransporterSpeciesVertexStyle
{
public:
	static  MidgetTransporterSpeciesVertexStyle* GetDefaultStyle();
	 MidgetTransporterSpeciesVertexStyle();
private:
	static  MidgetTransporterSpeciesVertexStyle* DefaultStyle;
};

class ClonedTransporterSpeciesVertexStyle: public ClonedSpeciesVertexStyle
{
public:
	static ClonedTransporterSpeciesVertexStyle* GetDefaultStyle();
	ClonedTransporterSpeciesVertexStyle();
private:
	static ClonedTransporterSpeciesVertexStyle* DefaultStyle;
};

////////////////////////////////////////////////////////////

class MacromoleculeSpeciesVertexStyle: public VertexStyle
{
public:
	static MacromoleculeSpeciesVertexStyle* GetDefaultStyle();
	MacromoleculeSpeciesVertexStyle();
private:
	static MacromoleculeSpeciesVertexStyle* DefaultStyle;
};

class ClonedMacromoleculeSpeciesVertexStyle: public VertexStyle
{
public:
	static ClonedMacromoleculeSpeciesVertexStyle* GetDefaultStyle();
	ClonedMacromoleculeSpeciesVertexStyle();
private:
	static ClonedMacromoleculeSpeciesVertexStyle* DefaultStyle;
};

class MidgetMacromoleculeSpeciesVertexStyle: public MacromoleculeSpeciesVertexStyle
{
public:
	static MidgetMacromoleculeSpeciesVertexStyle* GetDefaultStyle();
	MidgetMacromoleculeSpeciesVertexStyle();
private:
	static MidgetMacromoleculeSpeciesVertexStyle* DefaultStyle;
};

////////////////////////////////////////////////////////////

class SimpleSpeciesVertexStyle: public VertexStyle
{
public:
	static SimpleSpeciesVertexStyle* GetDefaultStyle();
	SimpleSpeciesVertexStyle();

private:
	static SimpleSpeciesVertexStyle* DefaultStyle;
};

class ClonedSimpleSpeciesVertexStyle: public VertexStyle
{
public:
	static ClonedSimpleSpeciesVertexStyle* GetDefaultStyle();
	ClonedSimpleSpeciesVertexStyle();
private:
	static ClonedSimpleSpeciesVertexStyle* DefaultStyle;
};

class MidgetSimpleSpeciesVertexStyle: public SimpleSpeciesVertexStyle
{
public:
	static MidgetSimpleSpeciesVertexStyle* GetDefaultStyle();
	MidgetSimpleSpeciesVertexStyle();
private:
	static MidgetSimpleSpeciesVertexStyle* DefaultStyle;
};

////////////////////////////////////////////////////////////

class ComplexSpeciesVertexStyle: public VertexStyle
{
public:
	static ComplexSpeciesVertexStyle* GetDefaultStyle();
	ComplexSpeciesVertexStyle();
private:
	static ComplexSpeciesVertexStyle* DefaultStyle;
};

class ClonedComplexSpeciesVertexStyle: public VertexStyle
{
public:
	static ClonedComplexSpeciesVertexStyle* GetDefaultStyle();
	ClonedComplexSpeciesVertexStyle();
private:
	static ClonedComplexSpeciesVertexStyle* DefaultStyle;
};

class MidgetComplexSpeciesVertexStyle: public ComplexSpeciesVertexStyle
{
public:
	static MidgetComplexSpeciesVertexStyle* GetDefaultStyle();
	MidgetComplexSpeciesVertexStyle();
private:
	static MidgetComplexSpeciesVertexStyle* DefaultStyle;
};

////////////////////////////////////////////////////////////

class SourceOrSinkSpeciesVertexStyle: public VertexStyle
{
public:
	static SourceOrSinkSpeciesVertexStyle* GetDefaultStyle();
	SourceOrSinkSpeciesVertexStyle();
private:
	static SourceOrSinkSpeciesVertexStyle* DefaultStyle;
};

////////////////////////////////////////////////////////////

class MultimerSpeciesVertexStyle: public VertexStyle
{
public:
	static MultimerSpeciesVertexStyle* GetDefaultStyle();
	MultimerSpeciesVertexStyle();
private:
	static MultimerSpeciesVertexStyle* DefaultStyle;
};

class ClonedMultimerSpeciesVertexStyle: public VertexStyle
{
public:
	static ClonedMultimerSpeciesVertexStyle* GetDefaultStyle();
	ClonedMultimerSpeciesVertexStyle();
private:
	static ClonedMultimerSpeciesVertexStyle* DefaultStyle;
};

class MidgetMultimerSpeciesVertexStyle: public MultimerSpeciesVertexStyle
{
public:
	static MidgetMultimerSpeciesVertexStyle* GetDefaultStyle();
	MidgetMultimerSpeciesVertexStyle();
private:
	static MidgetMultimerSpeciesVertexStyle* DefaultStyle;
};

////////////////////////////////////////////////////////////

class NAFeatureSpeciesVertexStyle: public VertexStyle
{
public:
	static NAFeatureSpeciesVertexStyle* GetDefaultStyle();
	NAFeatureSpeciesVertexStyle();
private:
	static NAFeatureSpeciesVertexStyle* DefaultStyle;
};

class ClonedNAFeatureSpeciesVertexStyle: public VertexStyle
{
public:
	static ClonedNAFeatureSpeciesVertexStyle* GetDefaultStyle();
	ClonedNAFeatureSpeciesVertexStyle();
private:
	static ClonedNAFeatureSpeciesVertexStyle* DefaultStyle;
};

class MidgetNAFeatureSpeciesVertexStyle: public NAFeatureSpeciesVertexStyle
{
public:
	static MidgetNAFeatureSpeciesVertexStyle* GetDefaultStyle();
	MidgetNAFeatureSpeciesVertexStyle();
private:
	static MidgetNAFeatureSpeciesVertexStyle* DefaultStyle;
};

////////////////////////////////////////////////////////////

class ObservableSpeciesVertexStyle: public VertexStyle
{
public:
	static ObservableSpeciesVertexStyle* GetDefaultStyle();
	ObservableSpeciesVertexStyle();
private:
	static ObservableSpeciesVertexStyle* DefaultStyle;
};

class ClonedObservableSpeciesVertexStyle: public VertexStyle
{
public:
	static ClonedObservableSpeciesVertexStyle* GetDefaultStyle();
	ClonedObservableSpeciesVertexStyle();
private:
	static ClonedObservableSpeciesVertexStyle* DefaultStyle;
};

class MidgetObservableSpeciesVertexStyle: public ObservableSpeciesVertexStyle
{
public:
	static MidgetObservableSpeciesVertexStyle* GetDefaultStyle();
	MidgetObservableSpeciesVertexStyle();
private:
	static MidgetObservableSpeciesVertexStyle* DefaultStyle;
};

////////////////////////////////////////////////////////////

class PerturbationSpeciesVertexStyle: public VertexStyle
{
public:
	static PerturbationSpeciesVertexStyle* GetDefaultStyle();
	PerturbationSpeciesVertexStyle();
private:
	static PerturbationSpeciesVertexStyle* DefaultStyle;
};

class ClonedPerturbationSpeciesVertexStyle: public VertexStyle
{
public:
	static ClonedPerturbationSpeciesVertexStyle* GetDefaultStyle();
	ClonedPerturbationSpeciesVertexStyle();
private:
	static ClonedPerturbationSpeciesVertexStyle* DefaultStyle;
};

class MidgetPerturbationSpeciesVertexStyle: public PerturbationSpeciesVertexStyle
{
public:
	static MidgetPerturbationSpeciesVertexStyle* GetDefaultStyle();
	MidgetPerturbationSpeciesVertexStyle();
private:
	static MidgetPerturbationSpeciesVertexStyle* DefaultStyle;
};

#endif