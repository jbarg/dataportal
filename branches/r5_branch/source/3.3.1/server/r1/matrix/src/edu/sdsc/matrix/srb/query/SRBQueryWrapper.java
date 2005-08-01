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

package edu.sdsc.matrix.srb.query;

import edu.sdsc.grid.io.GeneralFile;
import edu.sdsc.grid.io.GeneralFileSystem;
import edu.sdsc.grid.io.MetaDataCondition;
import edu.sdsc.grid.io.MetaDataSelect;

/*
	*
	* @author rmathew
	*
	* Contains the JARGON Query Request Details
	* 		- Query File/Directory Name
	* 		-  List of MetaDataConditions - Conditions
	* 		-  List of MetaDataSelections - Selection Fields
	*/
public class SRBQueryWrapper {
	private String queryFileNameParameter; // Name of File/Dir to query
	private GeneralFileSystem fileSystem; // File System used for Query
	private GeneralFile file; // File for Query
	private MetaDataCondition conditionArray[]; // List of Conditions
	private MetaDataSelect selectArray[]; // List of Selections
	private boolean queryForFileSystem; // Flag to check whether query for File/FileSystem -NOT USED
	/*
		* Constructor For Class
		*/
	public SRBQueryWrapper() {
	}

	/*
		* Set Query File Name
		*/
	public void setQueryFileNameParameter(String fileSys) {
		queryFileNameParameter = fileSys;
	}

	/*
		* Return Query File Name
		*/
	public String getQueryFileNameParameter() {
		return queryFileNameParameter;
	}

	/*
		* Set General File System
		*/
	public void setGeneralFileSystem(GeneralFileSystem fileSys) {
		fileSystem = fileSys;
	}

	/*
		* Return General File System
		*/
	public GeneralFileSystem getGeneralFileSystem() {
		return fileSystem;
	}

	/*
		* Set General File
		*/
	public void setGeneralFile(GeneralFile gFile) {
		file = gFile;
	}

	/*
		* Return General File
		*/
	public GeneralFile getGeneralFile() {
		return file;
	}

	/*
		* Set condition list
		*/
	public void setMetaDataCondition(MetaDataCondition[] condArray) {
		conditionArray = condArray;
	}

	/*
		* Return Condition List
		*/
	public MetaDataCondition[] getMetaDataCondition() {
		return conditionArray;
	}

	/*
		* Set selection list
		*/
	public void setMetaDataSelect(MetaDataSelect[] selArray) {
		selectArray = selArray;
	}

	/*
		* Return selection list
		*/
	public MetaDataSelect[] getMetaDataSelect() {
		return selectArray;
	}

	/*
		* Set flag for query type
		*/
	public void setQueryForFileSystem(boolean flag) {
		queryForFileSystem = flag;
	}

	/*
		* Return flag for query type
		*/
	public boolean isQueryForFileSystem() {
		return queryForFileSystem;
	}
}