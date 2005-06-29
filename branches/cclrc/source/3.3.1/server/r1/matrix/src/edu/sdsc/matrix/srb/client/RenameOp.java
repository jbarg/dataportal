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
	* RenameOp.java
	*
	* Created on July 8, 2003, 5:08 AM
	*/
package edu.sdsc.matrix.srb.client;

/**
	* Part of the Matrix client API.
	* Contains methods to create RenameOp objects.
	*
	* A RenameOp object can use either StdParams or FlexParams,
	* but not both.
	* @author  Allen Ding - SDSC
	*/
public class RenameOp extends edu.sdsc.matrix.srb.parser.impl.RenameOpImpl {

public RenameOp(StdDatagridObject dataObject, String newName)
{ super.setStdParams(new RenameParams(dataObject, newName)); }


		/**
		* Make a RenameOp renaming the first collection name to a new one.
		* @param collection The collection to rename
		* @param newName The new name for the collection
		*/
	public RenameOp(Collection collection, String newName) {
		super.setStdParams(new RenameParams(new StdDatagridObject(collection),
											newName));
	}

	/**
		* Make a RenameOp renaming the first dataset name to a new one.
		* @param dataset The dataset to rename
		* @param newName The new name for the collection
		*/
	public RenameOp(DataSet dataset, String newName) {
		super.setStdParams(new RenameParams(new StdDatagridObject(dataset),
											newName));
	}

					/**
		* Make a RenameOp renaming the container name to a new one.
		* @param container The container to rename
		* @param newName The new name for the collection
		*/
	public RenameOp(Container container, String newName) {
		super.setStdParams(new RenameParams(new StdDatagridObject(container),
											newName));
	}



	/**
		* Creates a new RenameOp object using RenameParams.
		* @param stdParams Rename Parameters.
		*/
	public RenameOp(RenameParams stdParams) {
		super.setStdParams(stdParams);
	}

	/**
		* Creates a new RenameOp object using flexParams.
		* @param flexParams Flex Parameters.
		*/
	public RenameOp(ParamList flexParams) {
		super.setFlexParams(flexParams);
	}

	/**
		* Makes the RenameOp use the specified RenameParams. Using this
		* method clobbers any flexParams paremeter.
		* @param stdParams A RenameParams.
		*/
	public void useStdParams(RenameParams stdParams) {
		super.setFlexParams(null);
		super.setStdParams(stdParams);
	}

	/**
		* Makes the RenameOp use the specified ParamList. Using this
		* method clobbers any stdParams parameter.
		* @param flexParams A ParamList.
		*/
	public void useFlexParams(ParamList flexParams) {
		super.setStdParams(null);
		super.setFlexParams(flexParams);
	}
}
