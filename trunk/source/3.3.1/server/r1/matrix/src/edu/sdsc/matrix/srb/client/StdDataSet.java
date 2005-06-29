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

public class StdDataSet extends edu.sdsc.matrix.srb.parser.impl.StdDataSetImpl
{
		public StdDataSet(edu.sdsc.matrix.srb.parser.impl.StdDataSetImpl dataSet)
		{
				this.setCheckSum(dataSet.getCheckSum());
				this.setContentType(dataSet.getContentType());
				this.setCreateTimeStamp(dataSet.getCreateTimeStamp());
				this.setDataCompressed(dataSet.isDataCompressed());
				this.setDataDeleted(dataSet.isDataDeleted());
				this.setDataDirty(dataSet.isDataDirty());
				this.setDataIdentifier(dataSet.getDataIdentifier());
				this.setExpiryDate1(dataSet.getExpiryDate1());
				this.setExpiryDate2(dataSet.getExpiryDate2());
				this.setHideLevel(dataSet.getHideLevel());
				this.setInContainer(dataSet.getInContainer());
				this.setLastAccessTimeStamp(dataSet.getLastAccessTimeStamp());
				this.setLastModificationTimeStamp(dataSet.getLastModificationTimeStamp());
				this.setLock(dataSet.getLock());
				this.setLogicalLocation(dataSet.getLogicalLocation());
				this.setLogicalResourceName(dataSet.getLogicalResourceName());
				this.setMetaInfo(dataSet.getMetaInfo());
				this.setOwner(dataSet.getOwner());
				this.setPhysicalLocation(dataSet.getPhysicalLocation());
				this.setReplicaNum(dataSet.getReplicaNum());
				this.setSize(dataSet.getSize());
		}

		public StdDataSet(String dataIdentifier)
		{
				this.setDataIdentifier(dataIdentifier);
		}
}
