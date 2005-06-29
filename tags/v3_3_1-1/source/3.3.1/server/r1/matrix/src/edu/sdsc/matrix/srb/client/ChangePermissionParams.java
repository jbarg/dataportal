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
	* ChangePermissionParams.java
	*
	* Created on July 8, 2003, 3:06 AM
	*/
package edu.sdsc.matrix.srb.client;

/**
	* Parameters given to ChangePermissionOp.
	* Contains methods to create ChangePermissionParam objects.
	* @author  Allen Ding - SDSC
	*/
public class ChangePermissionParams extends edu.sdsc.matrix.srb.parser.impl.
			ChangePermissionParamsImpl {

	public ChangePermissionParams(edu.sdsc.matrix.srb.parser.ChangePermissionParams params)
	{
			this.setAuditNewAccess(new AnyType(params.getAuditNewAccess()));
			this.setCollectionOnly(params.isCollectionOnly());
			this.setDataSetOnly(params.isDataSetOnly());
			this.setGridData(new StdDatagridObject(params.getGridData()));
			this.setNewPermission(params.getNewPermission());
			this.setRecursive(params.isRecursive());
			this.setTargetUser(new UserInfo(params.getTargetUser()));
	}

		/**
								* Creates a ChangePermissionParam object with the <b>required</b> gridData,
		* targetUser, and newPermission parameters.
		* @param gridData Information about the grid.
		* @param targetUser Targetted user.
		* @param newPermission Permission to change to.
		*/
	public ChangePermissionParams(StdDatagridObject gridData,
										UserInfo targetUser,
										String newPermission) {
		init(gridData, targetUser, newPermission);
	}


		public ChangePermissionParams(Collection collection, UserInfo user, String permission)
		{
				init(new StdDatagridObject(collection), user, permission);
		}

		public ChangePermissionParams(DataSet dataSet, UserInfo user, String permission)
		{
				init(new StdDatagridObject(dataSet), user, permission);
		}

		public ChangePermissionParams(Container container, UserInfo user, String permission)
		{
				init(new StdDatagridObject(container), user, permission);
		}

		public ChangePermissionParams(ActiveDatagridDataSet data, UserInfo user, String permission)
		{
				init(new StdDatagridObject(data), user, permission);
		}

			public ChangePermissionParams(ActiveDatagridCollection col, UserInfo user, String permission)
		{
				init(new StdDatagridObject(col), user, permission);
		}


		private void init(StdDatagridObject dataObj, UserInfo user, String permission)
		{
				setGridData(dataObj);
				setTargetUser(user);
				setNewPermission(permission);
		}
}
