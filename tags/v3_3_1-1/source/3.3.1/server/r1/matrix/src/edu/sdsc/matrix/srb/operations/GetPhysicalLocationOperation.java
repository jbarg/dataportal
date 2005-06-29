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

package edu.sdsc.matrix.srb.operations;

import edu.sdsc.matrix.srb.core.MatrixDatagridObject;
import edu.sdsc.matrix.srb.core.SRBConnection;
import edu.sdsc.matrix.srb.core.flow.FlowThread;
import edu.sdsc.matrix.srb.parser.GetPhysicalLocationOp;
import edu.sdsc.matrix.srb.parser.GetPhysicalLocationResult;
import edu.sdsc.matrix.srb.parser.OpResult;
import edu.sdsc.matrix.srb.parser.Step;
import edu.sdsc.matrix.srb.util.MatrixUtil;

import edu.sdsc.grid.io.MetaDataRecordList;
import edu.sdsc.grid.io.srb.SRBFile;
import edu.sdsc.grid.io.srb.SRBFileSystem;
import edu.sdsc.grid.io.srb.SRBMetaDataSet;

public class GetPhysicalLocationOperation extends MatrixAgent{
		public GetPhysicalLocationOperation(Step step, FlowThread parent,
			ThreadGroup group){
				super(step, parent, group);
		}

		protected void typeSpecificExecute() throws Exception{
				toLog("GetPhysicalLocationOperation -BEGIN", MatrixUtil.DEBUG_LOG);
				// get the filePath and use it to create an SRBFile
				GetPhysicalLocationOp op = getOperation().getGetPhysicalLocationOp();
				MatrixDatagridObject file = determineFilePath(op.getStdParams().
					getLogicalResource());
				toLog("GetPhysicalLocationOperation - file path is " + file.getGridPath(),
					MatrixUtil.DEBUG_LOG);
				SRBConnection srbConnection = (SRBConnection) retrieveGridConnection();
				SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
				SRBFile srbDataSet = new SRBFile(srbFileSystem, file.getGridPath());
				// query for the pathname which is the physical location
				MetaDataRecordList[] record = srbDataSet.query(SRBMetaDataSet.PATH_NAME);
				// extract the physical location
				String physicalLocation = record[0].getStringValue(0);
				toLog("GetPhysicalLocationOperation - physical locations is: " +
					physicalLocation, MatrixUtil.DEBUG_LOG);
				// construct a result object
				OpResult result = MatrixUtil.getObjectFactory().createOpResult();
				GetPhysicalLocationResult resultChild = MatrixUtil.getObjectFactory().
					createGetPhysicalLocationResult();
				resultChild.setPhysicalLocation(physicalLocation);
				result.setGetPhysicalLocationResult(resultChild);
				this.setResponse(result);
				toLog("GetPhysicalLocationOperation -EXIT", MatrixUtil.DEBUG_LOG);
		}
}