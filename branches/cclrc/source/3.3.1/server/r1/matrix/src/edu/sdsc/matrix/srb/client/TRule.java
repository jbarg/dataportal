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

/*
	* TRule.java
	*
	*/
package edu.sdsc.matrix.srb.client;

/**
	* Contains methods to create TRule.
	*
	* Matrix provide Event-Condition-Action based Rules that can be used for good in Workflows.
	*
	* @author  Arun Jagatheesan
	*/
public class TRule extends edu.sdsc.matrix.srb.parser.impl.TRuleImpl {
		protected TRule(edu.sdsc.matrix.srb.parser.TRule tRule)
		{
				super.setCondition(tRule.getCondition());
				super.setName(tRule.getName());
				for (java.util.Iterator itr = super.getAction().iterator();
										itr.hasNext(); )
				{
		super.getAction().add(itr.next());
				}
		}

		public TRule(boolean beforeEntry, TCondition condition, TAction action)
		{
				if (beforeEntry)
		super.setName("beforeEntry");
				else
		super.setName("afterExit");

				super.setCondition(condition);
				super.getAction().add(action);
		}

		public TRule(boolean beforeEntry, String simpleCondition)
		{
				if (beforeEntry)
		super.setName("beforeEntry");
				else
		super.setName("afterExit");

				this.setCondition(new TCondition(simpleCondition));
		}

//====================== add action methods ============================//

		public void addAction(TAction action) { super.getAction().add(action); }

		public void addAction(String name, String actionString)
		{ super.getAction().add(new TAction(name, actionString)); }
}
