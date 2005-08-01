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
import edu.sdsc.matrix.srb.parser.RenameOp;
import edu.sdsc.matrix.srb.parser.RenameParams;
import edu.sdsc.matrix.srb.parser.Step;
import edu.sdsc.matrix.srb.util.MatrixUtil;

import edu.sdsc.grid.io.GeneralFile;
import edu.sdsc.grid.io.srb.SRBContainer;
import edu.sdsc.grid.io.srb.SRBFile;
import edu.sdsc.grid.io.srb.SRBFileSystem;

/**
	* This class is responsible for handling the SRB Operation of
	* renaming a Container, Collection or Data Set
	*
	* @author rmathew
	*
	*/
public class RenameOperation extends MatrixAgent {
	public RenameOperation(Step step, FlowThread parent, ThreadGroup group) {
		super(step, parent, group);
	}

	/**
		* The method that executes the steps for renaming a Container,Collection or Data Set
		*/
	public void typeSpecificExecute() throws Exception {
		RenameOp renameOprn = getStep().getOperation().getRenameOp();
		RenameParams renameParams = renameOprn.getStdParams();
		MatrixDatagridObject matrixObject = determineFilePath(renameParams.
																	getOldName());
		String newName = evaluateExpressionToString(renameParams.getNewName());
		// Implementation for Operation
		if (matrixObject.isContainer()) {
			renameContainer(matrixObject, newName);
		} else if (matrixObject.isCollection()) {
			renameCollection(matrixObject, newName);
		} else if (matrixObject.isDataSet()) {
			renameDataSet(matrixObject, newName, renameParams);
		} else {
			throw new IllegalArgumentException(
						"No Grid Details for Rename Operation provided.");
		}
		//setSRBResponse(new Object());
	}

	private void renameContainer(MatrixDatagridObject matrixObject,
									String newName) throws Exception {
		toLog("RenameOperation() - renameContainer() -BEGIN",
					MatrixUtil.DEBUG_LOG);
		String oldFilePath = matrixObject.getGridPath();
		String newFilePath = newName;
		if (oldFilePath != null && newFilePath != null) {
			toLog("RenameOperation() - renameContainer() -oldFilePath:" +
						oldFilePath, MatrixUtil.DEBUG_LOG);
			toLog("RenameOperation() - renameContainer() -newFilePath:" +
						newFilePath, MatrixUtil.DEBUG_LOG);
			//SRBAccount account = srbCreateSRBAccount();
			SRBFile srbFile = null;
			//SRBFileSystem srbFileSystem = null;
			SRBConnection srbConnection = (SRBConnection)
						retrieveGridConnection();
			SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
			toLog("RenameOperation() - renameContainer() -USED FROM CONNECTION",
						MatrixUtil.DEBUG_LOG);
			//srbFileSystem = new SRBFileSystem(account);
			GeneralFile destinationFile = new SRBFile(srbFileSystem,
															newFilePath);
			SRBContainer srbContainer = new SRBContainer(srbFileSystem,
															oldFilePath);
			srbContainer.renameTo(destinationFile);
			srbFileSystem = null; // For Performance
		} else {
			throw new MatrixException("Cannot resolve variable");
		}
		toLog("RenameOperation() - renameContainer() -END",
					MatrixUtil.DEBUG_LOG);
	}

	private void renameCollection(MatrixDatagridObject matrixObject,
										String newName) throws Exception {
		toLog("RenameOperation() - renameCollection() -BEGIN",
					MatrixUtil.DEBUG_LOG);
		String oldFilePath = matrixObject.getGridPath();
		String newFilePath = newName;
		if (oldFilePath != null && newFilePath != null) {
			toLog("RenameOperation() - renameCollection() -oldFilePath:" +
						oldFilePath, MatrixUtil.DEBUG_LOG);
			toLog("RenameOperation() - renameCollection() -newFilePath:" +
						newFilePath, MatrixUtil.DEBUG_LOG);
			//SRBAccount account = srbCreateSRBAccount();
			SRBFile srbFile = null;
			//SRBFileSystem srbFileSystem = null;
			SRBConnection srbConnection = (SRBConnection)
						retrieveGridConnection();
			SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
			toLog(
						"RenameOperation() - renameCollection() -USED FROM CONNECTION",
						MatrixUtil.DEBUG_LOG);
			//srbFileSystem = new SRBFileSystem(account);
			GeneralFile destinationFile = new SRBFile(srbFileSystem,
															newFilePath);
			srbFile = new SRBFile(srbFileSystem, oldFilePath);
			srbFile.renameTo(destinationFile);
			srbFileSystem = null; // For Performance
		} else {
			throw new MatrixException("old or new file path is null");
		}
		toLog("RenameOperation() - renameCollection() -END",
					MatrixUtil.DEBUG_LOG);
	}

	private void renameDataSet(MatrixDatagridObject matrixObject,
										String newName, RenameParams renameParams) throws
				Exception {
		toLog("RenameOperation() - renameDataSet() -BEGIN",
					MatrixUtil.DEBUG_LOG);
		String oldFilePath = matrixObject.getGridPath();
		String newFilePath = newName;
		boolean renameReturnValue = false;
		if (oldFilePath != null && newFilePath != null) {
			// StdDatagridObject dataSetObject =  renameParams.getOldName();
			String logicalLocation = null; //dataSetObject.getDataSetNow().getLogicalLocation();
			toLog("RenameOperation() - renameDataSet() -oldFilePath:" +
						oldFilePath, MatrixUtil.DEBUG_LOG);
			toLog("RenameOperation() - renameDataSet() -newFilePath:" +
						newFilePath, MatrixUtil.DEBUG_LOG);
			//SRBAccount account = srbCreateSRBAccount();
			SRBFile srbFile = null;
			//SRBFileSystem srbFileSystem = null;
			//srbFileSystem = new SRBFileSystem(account);
			SRBConnection srbConnection = (SRBConnection)
						retrieveGridConnection();
			SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
			toLog("RenameOperation() - renameDataSet() -USED FROM CONNECTION",
						MatrixUtil.DEBUG_LOG);
			srbFile = new SRBFile(srbFileSystem, oldFilePath);
			logicalLocation = srbFile.getCanonicalPath();
			toLog("RenameOperation() - renameDataSet() -srbFileParent:" +
						srbFile.getParent(), MatrixUtil.DEBUG_LOG);
			//		Append LogicalLocation with newFilePath
			//newFilePath =  srbFile.getParent() + srbFile.getPathSeparator() + newFilePath;
			toLog(
						"RenameOperation() - renameDataSet() -newFilePathWithLogicalLocn:" +
						newFilePath, MatrixUtil.DEBUG_LOG);
			GeneralFile destinationFile = new SRBFile(srbFileSystem,
															newFilePath);
			renameReturnValue = srbFile.renameTo(destinationFile);
			srbFileSystem = null; // For Performance
			toLog("RenameOperation() - renameDataSet() -renameReturnValue:" +
						renameReturnValue, MatrixUtil.DEBUG_LOG);
		} else {
			throw new MatrixException("old or new file path is null");
		}
		toLog("RenameOperation() - renameDataSet() -END", MatrixUtil.DEBUG_LOG);
	}

	/**
		* Get Operation Name
		* @returns the name of the operation encapsulated by this agent
		*/
	public String getOperationName() {
		return "RenameOperation";
	}
}
