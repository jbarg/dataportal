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
import edu.sdsc.matrix.srb.core.MatrixException;
import edu.sdsc.matrix.srb.core.SRBConnection;
import edu.sdsc.matrix.srb.core.flow.FlowThread;
import edu.sdsc.matrix.srb.parser.DeleteOp;
import edu.sdsc.matrix.srb.parser.DeleteParams;
import edu.sdsc.matrix.srb.parser.Step;
import edu.sdsc.matrix.srb.util.MatrixUtil;

import edu.sdsc.grid.io.srb.SRBContainer;
import edu.sdsc.grid.io.srb.SRBFile;
import edu.sdsc.grid.io.srb.SRBFileSystem;

/**
	* This class is responsible for handling the SRB Operation of
	* deleting a Container, Collection or Data Set
	*
	* @author rmathew
	*
	*/
public class DeleteOperation extends MatrixAgent {
	public DeleteOperation(Step step, FlowThread parent, ThreadGroup group) {
		super(step, parent, group);
	}

	/**
		* The method that executes the steps for Deleting a Container, Collection or Data Set
		*/
	public void typeSpecificExecute() throws Exception {
		DeleteOp deleteOprn = getOperation().getDeleteOp();
		DeleteParams deleteParams = deleteOprn.getStdParams();
		// Implementation for Operation
		//String filePath =  determineFilePath(deleteParams.getGridObject());
		MatrixDatagridObject matrixGridObject = determineFilePath(deleteParams.
					getGridObject());
		if (matrixGridObject.isContainer()) {
			deleteContainer(matrixGridObject);
		} else if (matrixGridObject.isCollection()) {
			deleteCollection(matrixGridObject);
		} else if (matrixGridObject.isDataSet()) {
			deleteDataSet(matrixGridObject);
		} else {
			throw new MatrixException("No Grid Details for deletion provided.");
		}
		//setSRBResponse(new Object());
	}

	/**
		* Deletes a Container in SRB
		* @param gridObject Container Details
		*/
	private void deleteContainer(MatrixDatagridObject gridObject) throws
				Exception {
		toLog("DeleteOperation() - deleteContainer() -BEGIN",
					MatrixUtil.DEBUG_LOG);
		String containerName = evaluateExpressionToString(gridObject.getGridPath());
		toLog("DeleteOperation() - deleteContainer() -containerName:" +
					containerName, MatrixUtil.DEBUG_LOG);
		if (containerName != null) {
			//SRBAccount account = srbCreateSRBAccount();
			//srbFileSystem = new SRBFileSystem(account);
			SRBConnection srbConnection = (SRBConnection)
						retrieveGridConnection();
			SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
			toLog("DeleteOperation() - deleteContainer() -USED FROM CONNECTION",
						MatrixUtil.DEBUG_LOG);
			SRBContainer srbContainer = new SRBContainer(srbFileSystem,
															containerName);
			srbContainer.delete();
			srbFileSystem = null; // For Performance
		} else {
			throw new MatrixException("Cannot resolve variable");
		}
		toLog("DeleteOperation() - deleteContainer() -END",
					MatrixUtil.DEBUG_LOG);
	}

	/**
		* Deletes a Collection in SRB
		* @param gridObject Collection Details
		*/
	private void deleteCollection(MatrixDatagridObject gridObject) throws
				Exception {
		toLog("DeleteOperation() - deleteCollection() -BEGIN",
					MatrixUtil.DEBUG_LOG);
		String filePath = gridObject.getGridPath();
		toLog("DeleteOperation() - deleteCollection() -filePath:" + filePath,
					MatrixUtil.DEBUG_LOG);
		if (filePath != null) {
			//SRBAccount account = srbCreateSRBAccount();
			//srbFileSystem = new SRBFileSystem(account);
			SRBConnection srbConnection = (SRBConnection)
						retrieveGridConnection();
			SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
			toLog(
						"DeleteOperation() - deleteCollection() -USED FROM CONNECTION",
						MatrixUtil.DEBUG_LOG);
			SRBFile srbFile = new SRBFile(srbFileSystem, filePath);
			srbFile.delete();
			srbFileSystem = null; // For Performance
		} else {
			throw new MatrixException("Cannot resolve variable");
		}
		toLog("DeleteOperation() - deleteCollection() -END",
					MatrixUtil.DEBUG_LOG);
	}

	/**
		* Deletes a Data Set in SRB
		* @param gridObject Data Set Details
		*/
	private void deleteDataSet(MatrixDatagridObject gridObject) throws
				Exception {
		toLog("DeleteOperation() - deleteDataSet() -BEGIN",
					MatrixUtil.DEBUG_LOG);
		String filePath = gridObject.getGridPath();
		toLog("DeleteOperation() - deleteDataSet() -filePath:" + filePath,
					MatrixUtil.DEBUG_LOG);
		if (filePath != null) {
			//SRBAccount account = srbCreateSRBAccount();
			//srbFileSystem = new SRBFileSystem(account);
			SRBConnection srbConnection = (SRBConnection)
						retrieveGridConnection();
			SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
			toLog("DeleteOperation() - deleteDataSet() -USED FROM CONNECTION",
						MatrixUtil.DEBUG_LOG);
			SRBFile srbFile = new SRBFile(srbFileSystem, filePath);
			srbFile.delete();
			srbFileSystem = null; // For Performance
		} else {
			throw new MatrixException("Cannot resolve variable");
		}
		toLog("DeleteOperation() - deleteDataSet() -END", MatrixUtil.DEBUG_LOG);
	}

	/**
		* Get Operation Name
		* @returns the name of the operation encapsulated by this agent
		*/
	public String getOperationName() {
		return "DeleteOperation";
	}
}
