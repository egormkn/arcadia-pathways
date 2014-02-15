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
 *  SpeciesVertexStyle.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 04/06/2008.
 *  Last documented never.
 *
 */

#include "speciesvertexstyle.h"

//:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S:S

/******************************
* DefaultStyle: the Singleton *
*******************************/
SpeciesVertexStyle* SpeciesVertexStyle::DefaultStyle = NULL;

/******************
* GetDefaultStyle *
*******************
* Possibly creates, then returns
* the Singleton
********************************/
SpeciesVertexStyle* SpeciesVertexStyle::GetDefaultStyle() {
	if (!SpeciesVertexStyle::DefaultStyle) SpeciesVertexStyle::DefaultStyle = new SpeciesVertexStyle();
	return SpeciesVertexStyle::DefaultStyle;
}

/*************************************
* SpeciesVertexStyle: a Cyan Ellipse *
**************************************/
SpeciesVertexStyle::SpeciesVertexStyle() : VertexStyle(0, 255, 255, ellipse) {}

EnzymeSpeciesVertexStyle* EnzymeSpeciesVertexStyle::DefaultStyle = NULL;
EnzymeSpeciesVertexStyle* EnzymeSpeciesVertexStyle::GetDefaultStyle() {
	if (!EnzymeSpeciesVertexStyle::DefaultStyle) EnzymeSpeciesVertexStyle::DefaultStyle = new EnzymeSpeciesVertexStyle();
	return EnzymeSpeciesVertexStyle::DefaultStyle;
}
EnzymeSpeciesVertexStyle::EnzymeSpeciesVertexStyle() : SpeciesVertexStyle() { this->rColor = 255; this->gColor = 127; this->bColor = 0; }

MidgetEnzymeSpeciesVertexStyle* MidgetEnzymeSpeciesVertexStyle::DefaultStyle = NULL;
MidgetEnzymeSpeciesVertexStyle* MidgetEnzymeSpeciesVertexStyle::GetDefaultStyle() {
	if (!MidgetEnzymeSpeciesVertexStyle::DefaultStyle) MidgetEnzymeSpeciesVertexStyle::DefaultStyle = new MidgetEnzymeSpeciesVertexStyle();
	return MidgetEnzymeSpeciesVertexStyle::DefaultStyle;
}
MidgetEnzymeSpeciesVertexStyle::MidgetEnzymeSpeciesVertexStyle() : EnzymeSpeciesVertexStyle() { this->isMidget = true; }

ClonedEnzymeSpeciesVertexStyle* ClonedEnzymeSpeciesVertexStyle::DefaultStyle = NULL;
ClonedEnzymeSpeciesVertexStyle* ClonedEnzymeSpeciesVertexStyle::GetDefaultStyle() {
	if (!ClonedEnzymeSpeciesVertexStyle::DefaultStyle) ClonedEnzymeSpeciesVertexStyle::DefaultStyle = new ClonedEnzymeSpeciesVertexStyle();
	return ClonedEnzymeSpeciesVertexStyle::DefaultStyle;
}
ClonedEnzymeSpeciesVertexStyle::ClonedEnzymeSpeciesVertexStyle() : ClonedSpeciesVertexStyle() { }

TransporterSpeciesVertexStyle* TransporterSpeciesVertexStyle::DefaultStyle = NULL;
TransporterSpeciesVertexStyle* TransporterSpeciesVertexStyle::GetDefaultStyle() {
	if (!TransporterSpeciesVertexStyle::DefaultStyle) TransporterSpeciesVertexStyle::DefaultStyle = new TransporterSpeciesVertexStyle();
	return TransporterSpeciesVertexStyle::DefaultStyle;
}
TransporterSpeciesVertexStyle::TransporterSpeciesVertexStyle() : SpeciesVertexStyle() { this->rColor = this->gColor = 64; this->bColor = 210; }

MidgetTransporterSpeciesVertexStyle* MidgetTransporterSpeciesVertexStyle::DefaultStyle = NULL;
MidgetTransporterSpeciesVertexStyle* MidgetTransporterSpeciesVertexStyle::GetDefaultStyle() {
	if (!MidgetTransporterSpeciesVertexStyle::DefaultStyle) MidgetTransporterSpeciesVertexStyle::DefaultStyle = new MidgetTransporterSpeciesVertexStyle();
	return MidgetTransporterSpeciesVertexStyle::DefaultStyle;
}
MidgetTransporterSpeciesVertexStyle::MidgetTransporterSpeciesVertexStyle() : TransporterSpeciesVertexStyle() { this->isMidget = true; }

ClonedTransporterSpeciesVertexStyle* ClonedTransporterSpeciesVertexStyle::DefaultStyle = NULL;
ClonedTransporterSpeciesVertexStyle* ClonedTransporterSpeciesVertexStyle::GetDefaultStyle() {
	if (!ClonedTransporterSpeciesVertexStyle::DefaultStyle) ClonedTransporterSpeciesVertexStyle::DefaultStyle = new ClonedTransporterSpeciesVertexStyle();
	return ClonedTransporterSpeciesVertexStyle::DefaultStyle;
}
ClonedTransporterSpeciesVertexStyle::ClonedTransporterSpeciesVertexStyle() : ClonedSpeciesVertexStyle() { }

//:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C:C

/******************************
* DefaultStyle: the Singleton *
*******************************/
ClonedSpeciesVertexStyle* ClonedSpeciesVertexStyle::DefaultStyle = NULL;

/******************
* GetDefaultStyle *
*******************
* Possibly creates, then returns
* the Singleton
********************************/
ClonedSpeciesVertexStyle* ClonedSpeciesVertexStyle::GetDefaultStyle() {
	if (!ClonedSpeciesVertexStyle::DefaultStyle) ClonedSpeciesVertexStyle::DefaultStyle = new ClonedSpeciesVertexStyle();
	return ClonedSpeciesVertexStyle::DefaultStyle;
}

/********************************************************
* ClonedSpeciesVertexStyle: a Yellow Underlined Ellipse *
********************************************************/
ClonedSpeciesVertexStyle::ClonedSpeciesVertexStyle() : VertexStyle(255, 255, 0, underlinedEllipse) { this->isMidget = true; }

//:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M:M

/******************************
* DefaultStyle: the Singleton *
*******************************/
MidgetSpeciesVertexStyle* MidgetSpeciesVertexStyle::DefaultStyle = NULL;

/******************
* GetDefaultStyle *
*******************
* Possibly creates, then returns
* the Singleton
********************************/
MidgetSpeciesVertexStyle* MidgetSpeciesVertexStyle::GetDefaultStyle() {
	if (!MidgetSpeciesVertexStyle::DefaultStyle) MidgetSpeciesVertexStyle::DefaultStyle = new MidgetSpeciesVertexStyle();
	return MidgetSpeciesVertexStyle::DefaultStyle;
}

/********************************************************
* MidgetSpeciesVertexStyle: a Yellow Underlined Ellipse *
********************************************************/
MidgetSpeciesVertexStyle::MidgetSpeciesVertexStyle() : SpeciesVertexStyle() { this->isMidget = true; }

//:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A

/******************************
* DefaultStyle: the Singleton *
*******************************/
MacromoleculeSpeciesVertexStyle* MacromoleculeSpeciesVertexStyle::DefaultStyle = NULL;

/******************
* GetDefaultStyle *
*******************
* Possibly creates, then returns
* the Singleton
********************************/
MacromoleculeSpeciesVertexStyle* MacromoleculeSpeciesVertexStyle::GetDefaultStyle() {
	if (!MacromoleculeSpeciesVertexStyle::DefaultStyle) MacromoleculeSpeciesVertexStyle::DefaultStyle = new MacromoleculeSpeciesVertexStyle();
	return MacromoleculeSpeciesVertexStyle::DefaultStyle;
}

/************************************************************
* MacromoleculeSpeciesVertexStyle: a Cyan Rounded Rectangle *
************************************************************/
MacromoleculeSpeciesVertexStyle::MacromoleculeSpeciesVertexStyle() : VertexStyle(0, 255, 255, curvyRectangle) { }

ClonedMacromoleculeSpeciesVertexStyle* ClonedMacromoleculeSpeciesVertexStyle::DefaultStyle = NULL;
ClonedMacromoleculeSpeciesVertexStyle* ClonedMacromoleculeSpeciesVertexStyle::GetDefaultStyle()
{
	if (!ClonedMacromoleculeSpeciesVertexStyle::DefaultStyle) ClonedMacromoleculeSpeciesVertexStyle::DefaultStyle = new ClonedMacromoleculeSpeciesVertexStyle();
	return ClonedMacromoleculeSpeciesVertexStyle::DefaultStyle;
}
ClonedMacromoleculeSpeciesVertexStyle::ClonedMacromoleculeSpeciesVertexStyle() : VertexStyle(255, 255, 0, underlinedCurvyRectangle) { this->isMidget = true; }

MidgetMacromoleculeSpeciesVertexStyle* MidgetMacromoleculeSpeciesVertexStyle::DefaultStyle = NULL;
MidgetMacromoleculeSpeciesVertexStyle* MidgetMacromoleculeSpeciesVertexStyle::GetDefaultStyle()
{
	if (!MidgetMacromoleculeSpeciesVertexStyle::DefaultStyle) MidgetMacromoleculeSpeciesVertexStyle::DefaultStyle = new MidgetMacromoleculeSpeciesVertexStyle();
	return MidgetMacromoleculeSpeciesVertexStyle::DefaultStyle;
}
MidgetMacromoleculeSpeciesVertexStyle::MidgetMacromoleculeSpeciesVertexStyle() : MacromoleculeSpeciesVertexStyle() { this->isMidget = true; }

// Simple chemical
SimpleSpeciesVertexStyle* SimpleSpeciesVertexStyle::DefaultStyle = NULL;

SimpleSpeciesVertexStyle* SimpleSpeciesVertexStyle::GetDefaultStyle() {
	if (!SimpleSpeciesVertexStyle::DefaultStyle) SimpleSpeciesVertexStyle::DefaultStyle = new SimpleSpeciesVertexStyle();
	return SimpleSpeciesVertexStyle::DefaultStyle;
}

SimpleSpeciesVertexStyle::SimpleSpeciesVertexStyle() : VertexStyle(0, 255, 255, circular) { }

ClonedSimpleSpeciesVertexStyle* ClonedSimpleSpeciesVertexStyle::DefaultStyle = NULL;
ClonedSimpleSpeciesVertexStyle* ClonedSimpleSpeciesVertexStyle::GetDefaultStyle()
{
	if (!ClonedSimpleSpeciesVertexStyle::DefaultStyle) ClonedSimpleSpeciesVertexStyle::DefaultStyle = new ClonedSimpleSpeciesVertexStyle();
	return ClonedSimpleSpeciesVertexStyle::DefaultStyle;
}
ClonedSimpleSpeciesVertexStyle::ClonedSimpleSpeciesVertexStyle() : VertexStyle(255, 255, 0, underlinedCircular) { this->isMidget = true; }

MidgetSimpleSpeciesVertexStyle* MidgetSimpleSpeciesVertexStyle::DefaultStyle = NULL;
MidgetSimpleSpeciesVertexStyle* MidgetSimpleSpeciesVertexStyle::GetDefaultStyle()
{
	if (!MidgetSimpleSpeciesVertexStyle::DefaultStyle) MidgetSimpleSpeciesVertexStyle::DefaultStyle = new MidgetSimpleSpeciesVertexStyle();
	return MidgetSimpleSpeciesVertexStyle::DefaultStyle;
}
MidgetSimpleSpeciesVertexStyle::MidgetSimpleSpeciesVertexStyle() : SimpleSpeciesVertexStyle() { this->isMidget = true; }
//:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A:A

ComplexSpeciesVertexStyle* ComplexSpeciesVertexStyle::DefaultStyle = NULL;
ComplexSpeciesVertexStyle* ComplexSpeciesVertexStyle::GetDefaultStyle()
{
	if (!ComplexSpeciesVertexStyle::DefaultStyle) ComplexSpeciesVertexStyle::DefaultStyle = new ComplexSpeciesVertexStyle();
	return ComplexSpeciesVertexStyle::DefaultStyle;
}
ComplexSpeciesVertexStyle::ComplexSpeciesVertexStyle() : VertexStyle(0, 255, 255, cutRectangle) { }

ClonedComplexSpeciesVertexStyle* ClonedComplexSpeciesVertexStyle::DefaultStyle = NULL;
ClonedComplexSpeciesVertexStyle* ClonedComplexSpeciesVertexStyle::GetDefaultStyle()
{
	if (!ClonedComplexSpeciesVertexStyle::DefaultStyle) ClonedComplexSpeciesVertexStyle::DefaultStyle = new ClonedComplexSpeciesVertexStyle();
	return ClonedComplexSpeciesVertexStyle::DefaultStyle;
}
ClonedComplexSpeciesVertexStyle::ClonedComplexSpeciesVertexStyle() : VertexStyle(255, 255, 0, underlinedCutRectangle) { this->isMidget = true; }

MidgetComplexSpeciesVertexStyle* MidgetComplexSpeciesVertexStyle::DefaultStyle = NULL;
MidgetComplexSpeciesVertexStyle* MidgetComplexSpeciesVertexStyle::GetDefaultStyle()
{
	if (!MidgetComplexSpeciesVertexStyle::DefaultStyle) MidgetComplexSpeciesVertexStyle::DefaultStyle = new MidgetComplexSpeciesVertexStyle();
	return MidgetComplexSpeciesVertexStyle::DefaultStyle;
}
MidgetComplexSpeciesVertexStyle::MidgetComplexSpeciesVertexStyle() : ComplexSpeciesVertexStyle() { this->isMidget = true; }

//:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O

/******************************
* DefaultStyle: the Singleton *
*******************************/
SourceOrSinkSpeciesVertexStyle* SourceOrSinkSpeciesVertexStyle::DefaultStyle = NULL;

/******************
* GetDefaultStyle *
*******************
* Possibly creates, then returns
* the Singleton
********************************/
SourceOrSinkSpeciesVertexStyle* SourceOrSinkSpeciesVertexStyle::GetDefaultStyle() {
	if (!SourceOrSinkSpeciesVertexStyle::DefaultStyle) SourceOrSinkSpeciesVertexStyle::DefaultStyle = new SourceOrSinkSpeciesVertexStyle();
	return SourceOrSinkSpeciesVertexStyle::DefaultStyle;
}

/******************************************************
* SourceOrSinkSpeciesVertexStyle: a Black Null Symbol *
******************************************************/
SourceOrSinkSpeciesVertexStyle::SourceOrSinkSpeciesVertexStyle() : VertexStyle(0, 0, 0, nullSign) { this->isMidget = true; this->showLabel = false; this->fillInside = blank; }

//:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O

MultimerSpeciesVertexStyle* MultimerSpeciesVertexStyle::DefaultStyle = NULL;
MultimerSpeciesVertexStyle* MultimerSpeciesVertexStyle::GetDefaultStyle()
{
	if (!MultimerSpeciesVertexStyle::DefaultStyle) MultimerSpeciesVertexStyle::DefaultStyle = new MultimerSpeciesVertexStyle();
	return MultimerSpeciesVertexStyle::DefaultStyle;
}
MultimerSpeciesVertexStyle::MultimerSpeciesVertexStyle() : VertexStyle(0, 255, 255, doubleEllipse) { }

ClonedMultimerSpeciesVertexStyle* ClonedMultimerSpeciesVertexStyle::DefaultStyle = NULL;
ClonedMultimerSpeciesVertexStyle* ClonedMultimerSpeciesVertexStyle::GetDefaultStyle()
{
	if (!ClonedMultimerSpeciesVertexStyle::DefaultStyle) ClonedMultimerSpeciesVertexStyle::DefaultStyle = new ClonedMultimerSpeciesVertexStyle();
	return ClonedMultimerSpeciesVertexStyle::DefaultStyle;
}
ClonedMultimerSpeciesVertexStyle::ClonedMultimerSpeciesVertexStyle() : VertexStyle(255, 255, 0, underlinedDoubleEllipse) { this->isMidget = true; }

MidgetMultimerSpeciesVertexStyle* MidgetMultimerSpeciesVertexStyle::DefaultStyle = NULL;
MidgetMultimerSpeciesVertexStyle* MidgetMultimerSpeciesVertexStyle::GetDefaultStyle()
{
	if (!MidgetMultimerSpeciesVertexStyle::DefaultStyle) MidgetMultimerSpeciesVertexStyle::DefaultStyle = new MidgetMultimerSpeciesVertexStyle();
	return MidgetMultimerSpeciesVertexStyle::DefaultStyle;
}
MidgetMultimerSpeciesVertexStyle::MidgetMultimerSpeciesVertexStyle() : MultimerSpeciesVertexStyle() { this->isMidget = true; }

//:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O

ObservableSpeciesVertexStyle* ObservableSpeciesVertexStyle::DefaultStyle = NULL;
ObservableSpeciesVertexStyle* ObservableSpeciesVertexStyle::GetDefaultStyle()
{
	if (!ObservableSpeciesVertexStyle::DefaultStyle) ObservableSpeciesVertexStyle::DefaultStyle = new ObservableSpeciesVertexStyle();
	return ObservableSpeciesVertexStyle::DefaultStyle;
}
ObservableSpeciesVertexStyle::ObservableSpeciesVertexStyle() : VertexStyle(0, 255, 255, hexagon) { }

ClonedObservableSpeciesVertexStyle* ClonedObservableSpeciesVertexStyle::DefaultStyle = NULL;
ClonedObservableSpeciesVertexStyle* ClonedObservableSpeciesVertexStyle::GetDefaultStyle()
{
	if (!ClonedObservableSpeciesVertexStyle::DefaultStyle) ClonedObservableSpeciesVertexStyle::DefaultStyle = new ClonedObservableSpeciesVertexStyle();
	return ClonedObservableSpeciesVertexStyle::DefaultStyle;
}
ClonedObservableSpeciesVertexStyle::ClonedObservableSpeciesVertexStyle() : VertexStyle(255, 255, 0, underlinedHexagon) { this->isMidget = true; }

MidgetObservableSpeciesVertexStyle* MidgetObservableSpeciesVertexStyle::DefaultStyle = NULL;
MidgetObservableSpeciesVertexStyle* MidgetObservableSpeciesVertexStyle::GetDefaultStyle()
{
	if (!MidgetObservableSpeciesVertexStyle::DefaultStyle) MidgetObservableSpeciesVertexStyle::DefaultStyle = new MidgetObservableSpeciesVertexStyle();
	return MidgetObservableSpeciesVertexStyle::DefaultStyle;
}
MidgetObservableSpeciesVertexStyle::MidgetObservableSpeciesVertexStyle() : ObservableSpeciesVertexStyle() { this->isMidget = true; }

//:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O

PerturbationSpeciesVertexStyle* PerturbationSpeciesVertexStyle::DefaultStyle = NULL;
PerturbationSpeciesVertexStyle* PerturbationSpeciesVertexStyle::GetDefaultStyle()
{
	if (!PerturbationSpeciesVertexStyle::DefaultStyle) PerturbationSpeciesVertexStyle::DefaultStyle = new PerturbationSpeciesVertexStyle();
	return PerturbationSpeciesVertexStyle::DefaultStyle;
}
PerturbationSpeciesVertexStyle::PerturbationSpeciesVertexStyle() : VertexStyle(0, 255, 255, concaveHex) { }

ClonedPerturbationSpeciesVertexStyle* ClonedPerturbationSpeciesVertexStyle::DefaultStyle = NULL;
ClonedPerturbationSpeciesVertexStyle* ClonedPerturbationSpeciesVertexStyle::GetDefaultStyle()
{
	if (!ClonedPerturbationSpeciesVertexStyle::DefaultStyle) ClonedPerturbationSpeciesVertexStyle::DefaultStyle = new ClonedPerturbationSpeciesVertexStyle();
	return ClonedPerturbationSpeciesVertexStyle::DefaultStyle;
}
ClonedPerturbationSpeciesVertexStyle::ClonedPerturbationSpeciesVertexStyle() : VertexStyle(255, 255, 0, underlinedConcaveHex) { this->isMidget = true; }

MidgetPerturbationSpeciesVertexStyle* MidgetPerturbationSpeciesVertexStyle::DefaultStyle = NULL;
MidgetPerturbationSpeciesVertexStyle* MidgetPerturbationSpeciesVertexStyle::GetDefaultStyle()
{
	if (!MidgetPerturbationSpeciesVertexStyle::DefaultStyle) MidgetPerturbationSpeciesVertexStyle::DefaultStyle = new MidgetPerturbationSpeciesVertexStyle();
	return MidgetPerturbationSpeciesVertexStyle::DefaultStyle;
}
MidgetPerturbationSpeciesVertexStyle::MidgetPerturbationSpeciesVertexStyle() : PerturbationSpeciesVertexStyle() { this->isMidget = true; }

//:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O:O

NAFeatureSpeciesVertexStyle* NAFeatureSpeciesVertexStyle::DefaultStyle = NULL;
NAFeatureSpeciesVertexStyle* NAFeatureSpeciesVertexStyle::GetDefaultStyle()
{
	if (!NAFeatureSpeciesVertexStyle::DefaultStyle) NAFeatureSpeciesVertexStyle::DefaultStyle = new NAFeatureSpeciesVertexStyle();
	return NAFeatureSpeciesVertexStyle::DefaultStyle;
}
NAFeatureSpeciesVertexStyle::NAFeatureSpeciesVertexStyle() : VertexStyle(0, 255, 255, halfCurvyRectangle) { }

ClonedNAFeatureSpeciesVertexStyle* ClonedNAFeatureSpeciesVertexStyle::DefaultStyle = NULL;
ClonedNAFeatureSpeciesVertexStyle* ClonedNAFeatureSpeciesVertexStyle::GetDefaultStyle()
{
	if (!ClonedNAFeatureSpeciesVertexStyle::DefaultStyle) ClonedNAFeatureSpeciesVertexStyle::DefaultStyle = new ClonedNAFeatureSpeciesVertexStyle();
	return ClonedNAFeatureSpeciesVertexStyle::DefaultStyle;
}
ClonedNAFeatureSpeciesVertexStyle::ClonedNAFeatureSpeciesVertexStyle() : VertexStyle(255, 255, 0, underlinedHalfCurvyRectangle) { this->isMidget = true; }

MidgetNAFeatureSpeciesVertexStyle* MidgetNAFeatureSpeciesVertexStyle::DefaultStyle = NULL;
MidgetNAFeatureSpeciesVertexStyle* MidgetNAFeatureSpeciesVertexStyle::GetDefaultStyle()
{
	if (!MidgetNAFeatureSpeciesVertexStyle::DefaultStyle) MidgetNAFeatureSpeciesVertexStyle::DefaultStyle = new MidgetNAFeatureSpeciesVertexStyle();
	return MidgetNAFeatureSpeciesVertexStyle::DefaultStyle;
}
MidgetNAFeatureSpeciesVertexStyle::MidgetNAFeatureSpeciesVertexStyle() : NAFeatureSpeciesVertexStyle() { this->isMidget = true; }
