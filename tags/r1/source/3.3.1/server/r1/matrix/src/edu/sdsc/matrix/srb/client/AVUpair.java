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

	/* Developer Log
		* AVUpair.java
		* Changed licence Dec 22, 2004 - Arun
		* Created on July 7, 2003, 3:02 AM
		*/
package edu.sdsc.matrix.srb.client;

/**
	* Contains methods to create AVUpair objects.
	* An AVUpair object can be used in an ingesting metadata operation to specify user
	* defined attribute-value metadata.
	* @author  Allen Ding - SDSC
	*/
public class AVUpair extends edu.sdsc.matrix.srb.parser.impl.AVUpairImpl {
	/**
		* Internal method used to wrap a parser interface AVUpair object into this
		* class' representation. This method should not normally be used by the user.
		* @param avupair The parser.AVUpair to wrap.
		*/
	protected AVUpair(edu.sdsc.matrix.srb.parser.AVUpair avupair) {
		super.setAttribute(avupair.getAttribute());
		super.setValue(avupair.getValue());
		super.setUnit(avupair.getUnit());
		super.getAddlValues().addAll(avupair.getAddlValues());
	}

	/**
		* Creates an AVUpair object with the <b>required</b> attribute and
		* value parameters.
		* @param attribute The attribute.
		* @param value The value.
		*/
	public AVUpair(String attribute, String value) {
		super.setAttribute(attribute);
		super.setValue(value);
	}
}