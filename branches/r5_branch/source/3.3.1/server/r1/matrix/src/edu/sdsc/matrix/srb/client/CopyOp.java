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
	* CopyOp.java
	*
	* Created on July 7, 2003, 6:13 AM
	*/
package edu.sdsc.matrix.srb.client;

/**
	* Operation to copy a dataset to another dataset.
	* Contains methods to create CopyOp objects.
	*
	* A CopyOp object can use either StdParams or FlexParams,
	* but not both.
	* @author  Allen Ding - SDSC
	*/
public class CopyOp extends edu.sdsc.matrix.srb.parser.impl.CopyOpImpl {

		public CopyOp(edu.sdsc.matrix.srb.parser.CopyOp op)
		{
				if(op.getFlexParams() != null)
						this.setFlexParams(new ParamList(op.getFlexParams()));
				if(op.getStdParams() != null)
						this.setStdParams(new CopyParams(op.getStdParams()));
		}

	/**
		* Creates a new CopyOp object using CopyParams.
		* @param stdParams Copy Parameters.
		*/
	public CopyOp(CopyParams stdParams) {
		super.setStdParams(stdParams);
	}

	/**
		* Creates a new CopyOp object to copy one collection to another.
		* @param source The source collection
		* @param dest The destination collection
		*/
	public CopyOp(Collection source, Collection dest) {
		super.setStdParams(new CopyParams(new StdDatagridObject(source),
												new StdDatagridObject(dest)));
	}

	/**
		* Creates a new CopyOp object to copy one dataset to another.
		* @param source The source dataset
		* @param dest The destination dataset
		*/
	public CopyOp(DataSet source, DataSet dest) {
		super.setStdParams(new CopyParams(new StdDatagridObject(source),
												new StdDatagridObject(dest)));
	}

	/**
		* Creates a new CopyOp object using flexParams.
		* @param flexParams Flex Parameters.
		*/
	public CopyOp(ParamList flexParams) {
		super.setFlexParams(flexParams);
	}

	/**
		* Makes the CopyOp use the specified CopyParams. Using this
		* method clobbers any flexParams paremeter.
		* @param stdParams A CopyParams.
		*/
	public void useStdParams(CopyParams stdParams) {
		super.setFlexParams(null);
		super.setStdParams(stdParams);
	}

	/**
		* Makes the CopyOp use the specified ParamList. Using this
		* method clobbers any stdParams parameter.
		* @param flexParams A ParamList.
		*/
	public void useFlexParams(ParamList flexParams) {
		super.setStdParams(null);
		super.setFlexParams(flexParams);
	}
}
