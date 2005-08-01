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
	* DatasetSystemMetaData.java
	*
	* Created on September 3, 2003, 3:36 PM
	*/
package edu.sdsc.matrix.srb.client;

/**
	* Used as part of MetaData.
	* @author  Allen Ding
	*/
public class DatasetSystemMetaData extends edu.sdsc.matrix.srb.parser.impl.
			DatasetSystemMetaDataImpl {
	/**
		* Internal method used to wrap a parser interface DatasetSystemMetaData object into this
		* class' representation. This method should not normally be used by the user.
		* @param metadata The parser.DatasetSystemMetaData to wrap.
		*/
	protected DatasetSystemMetaData(edu.sdsc.matrix.srb.parser.
									DatasetSystemMetaData dsmd) {
		super.setIsDirty(dsmd.getIsDirty());
		super.setFileIsCompressed(dsmd.getFileIsCompressed());
		super.setFileHide(dsmd.getFileHide());
		super.setFileVerNum(dsmd.getFileVerNum());
		super.setFileClassType(dsmd.getFileClassType());
		super.setFileExpiryDate(dsmd.getFileExpiryDate());
		super.setFileExpireDate2(dsmd.getFileExpireDate2());
		super.setModificationDate(dsmd.getModificationDate());
		super.setFilePinExpiry(dsmd.getFilePinExpiry());
		super.setFileAuditFlag(dsmd.getFileAuditFlag());
		if (dsmd.getFilePinOwnerDomain() != null) {
			super.setFilePinOwnerDomain(new OrganizationInfo(dsmd.
						getFilePinOwnerDomain()));
		}
		super.setFileReplicationEnum(dsmd.getFileReplicationEnum());
		super.setSize(dsmd.getSize());
		if (dsmd.getFileLock() != null) {
			super.setFileLock(new DataLock(dsmd.getFileLock()));
		}
		super.setCreationDate(dsmd.getCreationDate());
		if (dsmd.getPhysicalResouce() != null) {
			super.setPhysicalResouce(new Resource(dsmd.getPhysicalResouce()));
		}
		if (dsmd.getContainerInfo() != null) {
			super.setContainerInfo(new Container(dsmd.getContainerInfo()));
		}
		super.setOffset(dsmd.getOffset());
		super.setFileAccessId(dsmd.getFileAccessId());
		if (dsmd.getOwner() != null) {
			super.setOwner(new UserInfo(dsmd.getOwner()));
		}
	}
}
