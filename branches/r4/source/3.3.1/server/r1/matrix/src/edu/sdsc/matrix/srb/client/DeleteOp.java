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
	* acknowledge their use of the �SDSC Matrix project� in their end-user documentation or
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
	* DeleteOp.java
	*
	* Created on July 8, 2003, 5:03 AM
	*/
package edu.sdsc.matrix.srb.client;

/**
	* Part of the Matrix client API.
	* Contains methods to create DeleteOp objects.
	*
	* A DeleteOp object can use either StdParams or FlexParams,
	* but not both.
	* @author  Allen Ding - SDSC
	*/
public class DeleteOp extends edu.sdsc.matrix.srb.parser.impl.DeleteOpImpl {

		public DeleteOp (edu.sdsc.matrix.srb.parser.DeleteOp op)
		{
				if(op.getStdParams() != null)
						this.setStdParams(new DeleteParams(op.getStdParams()));
				else if(op.getFlexParams() != null)
						this.setFlexParams(new ParamList(op.getFlexParams()));
		}


		/**
		* Creates a new DeleteOp object using DeleteParams.
		* @param stdParams Delete Parameters.
		*/
	public DeleteOp(DeleteParams stdParams) {
		super.setStdParams(stdParams);
	}

					public DeleteOp(StdDatagridObject o)
					{
									super.setStdParams(new DeleteParams(o));
					}

					public DeleteOp(Collection o)
					{ super.setStdParams(new DeleteParams(new StdDatagridObject(o))); }

					public DeleteOp(DataSet o)
					{ super.setStdParams(new DeleteParams(new StdDatagridObject(o))); }

					public DeleteOp(Container o)
					{ super.setStdParams(new DeleteParams(new StdDatagridObject(o))); }

					public DeleteOp(ActiveDatagridCollection o)
					{ super.setStdParams(new DeleteParams(new StdDatagridObject(o))); }

					public DeleteOp(ActiveDatagridDataSet o)
					{ super.setStdParams(new DeleteParams(new StdDatagridObject(o))); }



	/**
		* Creates a new DeleteOp object using flexParams.
		* @param flexParams Flex Parameters.
		*/
	public DeleteOp(ParamList flexParams) {
		super.setFlexParams(flexParams);
	}

	/**
		* Makes the DeleteOp use the specified DeleteParams. Using this
		* method clobbers any flexParams paremeter.
		* @param stdParams A DeleteParams.
		*/
	public void useStdParams(DeleteParams stdParams) {
		super.setFlexParams(null);
		super.setStdParams(stdParams);
	}

	/**
		* Makes the DeleteOp use the specified ParamList. Using this
		* method clobbers any stdParams parameter.
		* @param flexParams A ParamList.
		*/
	public void useFlexParams(ParamList flexParams) {
		super.setStdParams(null);
		super.setFlexParams(flexParams);
	}
}