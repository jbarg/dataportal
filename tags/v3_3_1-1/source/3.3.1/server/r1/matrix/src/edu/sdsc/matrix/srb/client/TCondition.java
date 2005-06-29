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
	* Contains methods to create TCondition. A TCondition object can be used to describe conditions
	* using XQuery.
	* <p> The XQuery syntax is not checked by the client code or the Matrix DGL Compiler.
	* The DGL interpreter during the execution passes this XQuery string to our XQuery Compiler that
	* is  built using Qexo. Qexo checks the syntax at runtime. (so, if your xquery string has a problem,
	* you will get a Matrix Runtime error) </p>
	*
	*  @author  Arun Jagatheesan
	*/

public class TCondition extends edu.sdsc.matrix.srb.parser.impl.TConditionImpl {

		protected TCondition(edu.sdsc.matrix.srb.parser.TCondition tc) {
		super.setSimpleQuery(tc.getSimpleQuery());
		//  super.setXQuery(tc.getXQuery());
	}

					protected TCondition(edu.sdsc.matrix.srb.parser.TCondition tc, boolean isXQuery) {
							if(isXQuery)
								super.setXQuery(tc.getXQuery());
						else
								super.setSimpleQuery(tc.getSimpleQuery());

		//  super.setXQuery(tc.getXQuery());
	}

				public TCondition(String query, boolean isXQuery)
				{
						if(isXQuery)
								super.setXQuery(query);
						else
								super.setSimpleQuery(query);
				}

	public TCondition(String simpleQuery) {
		super.setSimpleQuery(simpleQuery);
	}
}
