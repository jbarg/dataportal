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
	* DataLock.java
	*
	* Created on September 4, 2003, 10:05 AM
	*/
package edu.sdsc.matrix.srb.client;

/**
	* Part of DatasetSystemMetaData.
	* @author  Allen Ding
	*/
public class DataLock extends edu.sdsc.matrix.srb.parser.impl.DataLockImpl {
	/**
		* Internal method used to wrap a parser interface DataLock object into this
		* class' representation. This method should not normally be used by the user.
		* @param datalock The parser.Datalock to wrap.
		*/
	protected DataLock(edu.sdsc.matrix.srb.parser.DataLock datalock) {
		super.setLockNum(datalock.getLockNum());
		super.setLockOwner(new UserInfo(datalock.getLockOwner()));
		super.setLockDescription(datalock.getLockDescription());
		super.setLockStartTimeStamp(datalock.getLockStartTimeStamp());
		super.setLockEndTimeStamp(datalock.getLockEndTimeStamp());
	}
}