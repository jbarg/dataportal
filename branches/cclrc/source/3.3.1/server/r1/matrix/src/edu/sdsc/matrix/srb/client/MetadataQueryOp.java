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
	*  MetadataQueryOp.java
	*
	* Created on July 9, 2003, 4:00 AM
	*/
package edu.sdsc.matrix.srb.client;

/**
	* Part of the Matrix client API.
	* Contains methods to create  MetadataQueryOp objects.
	*
	* A  MetadataQueryOp object can use either StdParams or FlexParams,
	* but not both.
	* @author  Allen Ding - SDSC
	*/
public class MetadataQueryOp extends edu.sdsc.matrix.srb.parser.impl.
			MetadataQueryOpImpl {
	/**
		* Creates a new  MetadataQueryOp object using  MetadataQueryParams.
		* @param stdParams  MetadataQuery Parameters.
		*/
	public MetadataQueryOp(MetadataQueryParams stdParams) {
		super.setStdParams(stdParams);
	}

	/**
		* Creates a new  MetadataQueryOp object using flexParams.
		* @param flexParams Flex Parameters.
		*/
	public MetadataQueryOp(ParamList flexParams) {
		super.setFlexParams(flexParams);
	}

	/**
		* Makes the  MetadataQueryOp use the specified  MetadataQueryParams. Using this
		* method clobbers any flexParams paremeter.
		* @param stdParams A  MetadataQueryParams.
		*/
	public void useStdParams(MetadataQueryParams stdParams) {
		super.setFlexParams(null);
		super.setStdParams(stdParams);
	}

	/**
		* Makes the  MetadataQueryOp use the specified Param MetadataQuery. Using this
		* method clobbers any stdParams parameter.
		* @param flexParams A Param MetadataQuery.
		*/
	public void useFlexParams(ParamList flexParams) {
		super.setStdParams(null);
		super.setFlexParams(flexParams);
	}
}