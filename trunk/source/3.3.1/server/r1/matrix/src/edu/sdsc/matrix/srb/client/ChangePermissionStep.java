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

public class ChangePermissionStep extends Step
{
		public ChangePermissionStep(edu.sdsc.matrix.srb.parser.Step step)
		{
				super(step.getStepID(), new Operation(step.getOperation()));
				if(step.getOperation().getChangePermissionOp() == null)
				{
						throw new RuntimeException("Cannot construct a ChangePermissionStep from "+
																																"a Step whose changePermissionOp is null");
				}
		}

		public ChangePermissionStep(String stepID, ChangePermissionOp op)
		{ super(stepID, new Operation(op)); }

		public ChangePermissionStep(String stepID, ChangePermissionParams stdParams)
		{ super(stepID, new Operation(new ChangePermissionOp(stdParams))); }

		public ChangePermissionStep(String stepID, ParamList flexParams)
		{ super(stepID, new Operation(new ChangePermissionOp(flexParams))); }

		public ChangePermissionStep(String stepID, StdDatagridObject gridData, UserInfo user, String permission)
		{ super(stepID, new Operation(new ChangePermissionOp(gridData, user, permission))); }

		public ChangePermissionStep(String stepID, Collection collection, UserInfo user, String permission)
		{ super(stepID, new Operation(new ChangePermissionOp(new StdDatagridObject(collection), user, permission))); }

		public ChangePermissionStep(String stepID, DataSet dataSet, UserInfo user, String permission)
		{ super(stepID, new Operation(new ChangePermissionOp(new StdDatagridObject(dataSet), user, permission))); }

		public ChangePermissionStep(String stepID, Container container, UserInfo user, String permission)
		{ super(stepID, new Operation(new ChangePermissionOp(new StdDatagridObject(container), user, permission))); }

		public ChangePermissionStep(String stepID, ActiveDatagridDataSet data, UserInfo user, String permission)
		{ super(stepID, new Operation(new ChangePermissionOp(new StdDatagridObject(data), user, permission)));}

			public ChangePermissionStep(String stepID, ActiveDatagridCollection col, UserInfo user, String permission)
		{ super(stepID, new Operation(new ChangePermissionOp(new StdDatagridObject(col), user, permission)));}
}
