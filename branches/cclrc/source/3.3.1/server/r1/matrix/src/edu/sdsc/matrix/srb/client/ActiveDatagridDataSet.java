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
	* ActiveDatagridDataSet.java
	*
	* Created on July 7, 2003, 5:27 AM
	*/
package edu.sdsc.matrix.srb.client;

/**
	* Contains methods to create ActiveDatagridDataSet objects.
	* @author  Allen Ding - SDSC
	*/
public class ActiveDatagridDataSet extends edu.sdsc.matrix.srb.parser.impl.
			ActiveDatagridDataSetImpl {


	public ActiveDatagridDataSet(edu.sdsc.matrix.srb.parser.ActiveDatagridDataSet adSet)
	{
			this.setDataSet(adSet.getDataSet());
			if(adSet.getProvider() != null)
					this.setProvider(new UserInfo(adSet.getProvider()));
			this.setStatic(adSet.isStatic());
			if(adSet.getVO() != null)
					this.setVO(new VirtualOrganizationInfo( (adSet.getVO())));
	}


	/**
		* Creates an ActiveDatagridDataSet object using the <b>required</b>
		* dataSet, VirtualOrganizationInfo, provided, and staticflag parameters.
		* @param dataSet The DataSet to use.
		* @param vo The VirtualOrganizationInfo.
		* @param provider UserInfo object specifying provider.
		* @param staticflag Flag for static or not.
		*/
	public ActiveDatagridDataSet(DataSet dataSet,
																																					VirtualOrganizationInfo vo,
																																					UserInfo provider,
																																					boolean staticflag) {
		super.setDataSet(dataSet);
		super.setVO(vo);
		super.setProvider(provider);
		super.setStatic(staticflag);
	}
}
