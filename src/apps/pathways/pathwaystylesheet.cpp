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
 *  PathwayStyleSheet.cpp
 *  arcadia
 *
 *  Created by Alice Villeger on 27/05/2008.
 *	Last Documented never.
 *
 */

#include "pathwaystylesheet.h"

// local
#include <arcadia/vertexproperty.h>
#include <arcadia/edgeproperty.h>
#include "reactionvertexstyle.h"
#include "speciesvertexstyle.h"
#include "modifieredgestyle.h"
#include "productedgestyle.h"
#include "reactantedgestyle.h"
#include "compartmentcontainerstyle.h"

#include <iostream>

int PathwayStyleSheet::UnspecifiedEntity = 285;
int PathwayStyleSheet::SimpleChemical = 247;
int PathwayStyleSheet::Macromolecule = 245;
int PathwayStyleSheet::NucleicAcidFeature = 354;
int PathwayStyleSheet::Multimer = 286;
int PathwayStyleSheet::SourceSink = 291;
int PathwayStyleSheet::Observable = 358;
int PathwayStyleSheet::Perturbation = 357;
int PathwayStyleSheet::Complex = 253;
int PathwayStyleSheet::Transition = 167;
int PathwayStyleSheet::Association = 177;
int PathwayStyleSheet::Dissociation = 180;
int PathwayStyleSheet::Modulation = 168;
int PathwayStyleSheet::Stimulation = 170;
int PathwayStyleSheet::Catalysis = 172;
int PathwayStyleSheet::Inhibition = 169;
int PathwayStyleSheet::Trigger = 171;
int PathwayStyleSheet::AndOperator = 173;
int PathwayStyleSheet::OrOperator = 174;
int PathwayStyleSheet::NotOperator = 238;



/*
missing:
289 compartment/functional compartment
173 and
174 or
238 not
*/

VertexStyle * PathwayStyleSheet::getVertexStyle(VertexProperty * vp, CloneProperty cp)
{
	if (!vp) { return StyleSheet::getVertexStyle(vp, cp); }

	VertexStyle * vs = NULL;

	if (vp->getSBOTerm() != -1)
	{
		int dmin = -1;
		int sbo = -1;
		int d = -1;

		// Reaction
		if (vp->getSuperTypeLabel() == "Reaction")
		{
			d = vp->inherits(PathwayStyleSheet::Transition);
			if (d != -1) if (dmin == -1 || d < dmin) { dmin = d; sbo = PathwayStyleSheet::Transition; }
	
			d = vp->inherits(PathwayStyleSheet::Association);
			if (d != -1) if (dmin == -1 || d < dmin) { dmin = d; sbo = PathwayStyleSheet::Association; }

			d = vp->inherits(PathwayStyleSheet::Dissociation);
			if (d != -1) if (dmin == -1 || d < dmin) { dmin = d; sbo = PathwayStyleSheet::Dissociation; }

			if (sbo == PathwayStyleSheet::Transition)
			{
				if (cp == rotated) vs = RotatedReactionVertexStyle::GetDefaultStyle();
				else vs = ReactionVertexStyle::GetDefaultStyle();			
			}

			if (sbo == PathwayStyleSheet::Association)
			{
				if (cp == rotated) vs = RotatedAssociationReactionVertexStyle::GetDefaultStyle();
				else vs = AssociationReactionVertexStyle::GetDefaultStyle();
			}

			if (sbo == PathwayStyleSheet::Dissociation)
			{
				if (cp == rotated) vs = RotatedDissociationReactionVertexStyle::GetDefaultStyle();
				else vs = DissociationReactionVertexStyle::GetDefaultStyle();
			}
		}

		// Species
		if (vp->getSuperTypeLabel() == "Species")
		{
			d = vp->inherits(PathwayStyleSheet::SimpleChemical);
			if (d != -1) if (dmin == -1 || d < dmin) { dmin = d; sbo = PathwayStyleSheet::SimpleChemical; }
	
			d = vp->inherits(PathwayStyleSheet::Macromolecule);
			if (d != -1) if (dmin == -1 || d < dmin) { dmin = d; sbo = PathwayStyleSheet::Macromolecule; }

			d = vp->inherits(PathwayStyleSheet::Complex);
			if (d != -1) if (dmin == -1 || d < dmin) { dmin = d; sbo = PathwayStyleSheet::Complex; }

			d = vp->inherits(PathwayStyleSheet::Multimer);
			if (d != -1) if (dmin == -1 || d < dmin) { dmin = d; sbo = PathwayStyleSheet::Multimer; }

			d = vp->inherits(PathwayStyleSheet::NucleicAcidFeature);
			if (d != -1) if (dmin == -1 || d < dmin) { dmin = d; sbo = PathwayStyleSheet::NucleicAcidFeature; }

			d = vp->inherits(PathwayStyleSheet::Observable);
			if (d != -1) if (dmin == -1 || d < dmin) { dmin = d; sbo = PathwayStyleSheet::Observable; }

			d = vp->inherits(PathwayStyleSheet::Perturbation);
			if (d != -1) if (dmin == -1 || d < dmin) { dmin = d; sbo = PathwayStyleSheet::Perturbation; }

			d = vp->inherits(PathwayStyleSheet::UnspecifiedEntity);
			if (d != -1) if (dmin == -1 || d < dmin) { dmin = d; sbo = PathwayStyleSheet::UnspecifiedEntity; }

			if (sbo == PathwayStyleSheet::SimpleChemical)
			{
				if (cp == isClone) { vs = ClonedSimpleSpeciesVertexStyle::GetDefaultStyle(); }
				else if (cp == midget) { vs = MidgetSimpleSpeciesVertexStyle::GetDefaultStyle(); }
				else { vs = SimpleSpeciesVertexStyle::GetDefaultStyle(); }		
			}

			if (sbo == PathwayStyleSheet::Macromolecule)
			{
				if (cp == isClone) { vs = ClonedMacromoleculeSpeciesVertexStyle::GetDefaultStyle(); }
				else if (cp == midget) { vs = MidgetMacromoleculeSpeciesVertexStyle::GetDefaultStyle(); }
				else { vs = MacromoleculeSpeciesVertexStyle::GetDefaultStyle(); }
			}

			if (sbo == PathwayStyleSheet::Complex)
			{
				if (cp == isClone) { vs = ClonedComplexSpeciesVertexStyle::GetDefaultStyle(); }
				else if (cp == midget) { vs = MidgetComplexSpeciesVertexStyle::GetDefaultStyle(); }
				else { vs = ComplexSpeciesVertexStyle::GetDefaultStyle(); }
			}			

			if (sbo == PathwayStyleSheet::Multimer)
			{
				if (cp == isClone) { vs = ClonedMultimerSpeciesVertexStyle::GetDefaultStyle(); }
				else if (cp == midget) { vs = MidgetMultimerSpeciesVertexStyle::GetDefaultStyle(); }
				else { vs = MultimerSpeciesVertexStyle::GetDefaultStyle(); }
			}			

			if (sbo == PathwayStyleSheet::NucleicAcidFeature)
			{
				if (cp == isClone) { vs = ClonedNAFeatureSpeciesVertexStyle::GetDefaultStyle(); }
				else if (cp == midget) { vs = MidgetNAFeatureSpeciesVertexStyle::GetDefaultStyle(); }
				else { vs = NAFeatureSpeciesVertexStyle::GetDefaultStyle(); }
			}			

			if (sbo == PathwayStyleSheet::Observable)
			{
				if (cp == isClone) { vs = ClonedObservableSpeciesVertexStyle::GetDefaultStyle(); }
				else if (cp == midget) { vs = MidgetObservableSpeciesVertexStyle::GetDefaultStyle(); }
				else { vs = ObservableSpeciesVertexStyle::GetDefaultStyle(); }
			}			

			if (sbo == PathwayStyleSheet::Perturbation)
			{
				if (cp == isClone) { vs = ClonedPerturbationSpeciesVertexStyle::GetDefaultStyle(); }
				else if (cp == midget) { vs = MidgetPerturbationSpeciesVertexStyle::GetDefaultStyle(); }
				else { vs = PerturbationSpeciesVertexStyle::GetDefaultStyle(); }
			}			

			if (sbo == PathwayStyleSheet::UnspecifiedEntity)
			{
				if (cp == isClone) { vs = ClonedSpeciesVertexStyle::GetDefaultStyle(); }
				else if (cp == midget) { vs = MidgetSpeciesVertexStyle::GetDefaultStyle(); }
				else { vs = SpeciesVertexStyle::GetDefaultStyle(); }
			}
		}
	}

	if (!vs)
	{
		std::string type = vp->getTypeLabel();

		if (type == "Reaction")
		{
			if (cp == rotated) vs = RotatedReactionVertexStyle::GetDefaultStyle();
			else vs = ReactionVertexStyle::GetDefaultStyle();
		}

		if (type == "enzyme") // Not SBGN! what is SBO?
		{
			if (cp == isClone) { vs = ClonedEnzymeSpeciesVertexStyle::GetDefaultStyle(); }
			else if (cp == midget) { vs = MidgetEnzymeSpeciesVertexStyle::GetDefaultStyle(); }
			else { vs = EnzymeSpeciesVertexStyle::GetDefaultStyle(); }
		}

		if (type == "transporter") // Not SBGN! what is SBO?
		{
			if (cp == isClone) { vs = ClonedTransporterSpeciesVertexStyle::GetDefaultStyle(); }
			else if (cp == midget) { vs = MidgetTransporterSpeciesVertexStyle::GetDefaultStyle(); }
			else { vs = TransporterSpeciesVertexStyle::GetDefaultStyle(); }
		}
	
		if (type == "empty set") // SBGN, but no SBO because I create it myself... Maybe I should fix that? [!]
		{
			vs = SourceOrSinkSpeciesVertexStyle::GetDefaultStyle();
		}
	
		if (type == "Species" || type == "metabolite")
		{
			if (cp == isClone) { vs = ClonedSpeciesVertexStyle::GetDefaultStyle(); }
			else if (cp == midget) { vs = MidgetSpeciesVertexStyle::GetDefaultStyle(); }
			else { vs = SpeciesVertexStyle::GetDefaultStyle(); }
		}
	}

	if (!vs) { vs = StyleSheet::getVertexStyle(vp, cp); }

	return vs;
}

EdgeStyle * PathwayStyleSheet::getEdgeStyle(EdgeProperty * ep)
{
	if (!ep) { return StyleSheet::getEdgeStyle(ep); }

	EdgeStyle *es = NULL;
	std::string type = ep->getTypeLabel();

	int st = ep->getSBOTerm();
	
	if (st != -1)
	{
		int dmin = -1;
		int sbo = -1;
		int d = -1;
		
		d = ep->inherits(PathwayStyleSheet::Modulation);
		if (d != -1) if (dmin == -1 || d < dmin) { dmin = d; sbo = PathwayStyleSheet::Modulation; }

		d = ep->inherits(PathwayStyleSheet::Stimulation);
		if (d != -1) if (dmin == -1 || d < dmin) { dmin = d; sbo = PathwayStyleSheet::Stimulation; }

		d = ep->inherits(PathwayStyleSheet::Catalysis);
		if (d != -1) if (dmin == -1 || d < dmin) { dmin = d; sbo = PathwayStyleSheet::Catalysis; }

		d = ep->inherits(PathwayStyleSheet::Inhibition);
		if (d != -1) if (dmin == -1 || d < dmin) { dmin = d; sbo = PathwayStyleSheet::Inhibition; }

		d = ep->inherits(PathwayStyleSheet::Trigger);
		if (d != -1) if (dmin == -1 || d < dmin) { dmin = d; sbo = PathwayStyleSheet::Trigger; }

		if (sbo == PathwayStyleSheet::Modulation)
		{
			es = ModifierEdgeStyle::GetDefaultStyle();
		}

		if (sbo == PathwayStyleSheet::Stimulation)
		{
			es = StimulationModifierEdgeStyle::GetDefaultStyle();	
		}

		if (sbo == PathwayStyleSheet::Catalysis)
		{
			es = CatalysisModifierEdgeStyle::GetDefaultStyle();
		}			

		if (sbo == PathwayStyleSheet::Inhibition)
		{
			es = InhibitionModifierEdgeStyle::GetDefaultStyle();
		}

		if (sbo == PathwayStyleSheet::Trigger)
		{
			es = TriggerModifierEdgeStyle::GetDefaultStyle();
		}
	}

	if (!es) // SBML hints
	{
		if (type == "Modifier")	es = ModifierEdgeStyle::GetDefaultStyle();
		if (type == "Product")	es = ProductEdgeStyle::GetDefaultStyle();
		if (type == "Reactant")
		{
			if (ep->isOriented) es = ReactantEdgeStyle::GetDefaultStyle();
			else es = ReversibleReactantEdgeStyle::GetDefaultStyle();
		}
	}

	if (!es)
	{
		es = StyleSheet::getEdgeStyle(ep);
	}

	return es;
}

ContainerStyle * PathwayStyleSheet::getContainerStyle(std::string type)
{
	ContainerStyle * s = NULL;

	if (type == "CompContainer")
	{
		s = CompartmentContainerStyle::GetDefaultStyle();
	}
	else s = StyleSheet::getContainerStyle(type);
	
	return s;
}