/*
	* 	SDSC Matrix. A Gridflow Management System for data grids and digtal libraries
	*  Copyright (C) 2004 SDSC Matrix Project
	*
	* This library is free software; you can redistribute it and/or
	* modify it under the terms of the GNU Lesser General Public
	* License as published by the Free Software Foundation; either
	* version 2.1 of the License, or (at your option) any later version.
	*
	* This library is distributed in the hope that it will be useful,
	*	but WITHOUT ANY WARRANTY; without even the implied warranty of
	*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	*	Lesser General Public License for more details.
	*
	*	You should have received a copy of the GNU Lesser General Public
	*	License along with this library; if not, write to the Free Software
	*	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
	*
	* ====================================================================
	*
	* This software consists of voluntary contributions from the developers of the SDSC
	* Matrix Project. We request that all redistribution and software made using this code
	* acknowledge their use of the “SDSC Matrix project” in their end-user documentation or
	* in their website.
	*
	* SDSC Matrix Project (list of developers as of Dec 2005)
	*	Designer & Architect: Arun swaran Jagatheesan (arun@sdsc.edu)
	* 9500 Gilman Drive, MC0505
	* San Diego Supercomputer Center
	* University of California
	* La Jolla CA 92093
	*
	* Allen Ding (alding@sdsc.edu)
	* Lucas Gilbert (iktome@sdsc.edu)
	* Arun Jagatheesan (arun@sdsc.edu)
	* Reena Mathew	(rmathew@sdsc.edu)
	* Daniel Moore (mixx@umail.ucsb.edu)
	* Erik Vandekieft (evk@sdsc.edu)
	* Jonathan Weinberg (jonw@sdsc.edu)
	*
	*/

package edu.sdsc.matrix.srb.client;

/**
	* This class represents a Flow with a TForLoop control structure
	*/
public class ForLoopFlow extends Flow
{
		public ForLoopFlow(edu.sdsc.matrix.srb.parser.Flow flow)
		{ super(flow); }

		/**
			* Creates a Flow with this ID
			* @param flowID String
			*/
		public ForLoopFlow(String flowID, TAction init, TCondition condition, TAction action)
		{
				super(flowID);
				TForLoop logic = new TForLoop(init, condition, action);
				super.setFlowLogic(new AnyControlStructure(logic));
		}

		/**
			* Takes in string versions of the init, invariant, and increments.  It assumes
			* that the invariant is a simple query (not XQuery)
			* @param flowID String
			* @param init String
			* @param invariant String
			* @param increment String
			*/
		public ForLoopFlow(String flowID, String init, String invariant, String increment)
		{
				super(flowID);
				TAction initAction = new TAction("initAction", init);
				TCondition invariantCondition = new TCondition(invariant, false);
				TAction incrementAction = new TAction("incrementAction", increment);

				TForLoop logic = new TForLoop(initAction, invariantCondition, incrementAction);
				super.setFlowLogic(new AnyControlStructure(logic));
		}
}
