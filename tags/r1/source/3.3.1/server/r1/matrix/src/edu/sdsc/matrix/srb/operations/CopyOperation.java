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

import java.io.IOException;

import edu.sdsc.matrix.srb.core.MatrixDatagridObject;
import edu.sdsc.matrix.srb.core.MatrixException;
import edu.sdsc.matrix.srb.core.SRBConnection;
import edu.sdsc.matrix.srb.core.flow.FlowThread;
import edu.sdsc.matrix.srb.parser.CopyOp;
import edu.sdsc.matrix.srb.parser.CopyParams;
import edu.sdsc.matrix.srb.parser.StdDatagridObject;
import edu.sdsc.matrix.srb.parser.Step;
import edu.sdsc.matrix.srb.util.MatrixUtil;

import edu.sdsc.grid.io.srb.SRBContainer;
import edu.sdsc.grid.io.srb.SRBFile;
import edu.sdsc.grid.io.srb.SRBFileSystem;

/**
	* This class is responsible for handling the SRB Operation of
	* copying a Container, Collection or Data Set
	*
	* @author rmathew
	*
	*/
public class CopyOperation extends MatrixAgent {
	public CopyOperation(Step step, FlowThread parent, ThreadGroup group) {
		super(step, parent, group);
	}

	/**
		* The method that executes the steps for Copy Operation
		*/
	public void typeSpecificExecute() throws Exception {
		CopyOp copyOprn = getOperation().getCopyOp();
		CopyParams copyParams = copyOprn.getStdParams();
		StdDatagridObject dataSource = copyParams.getDataSource();
		StdDatagridObject dataTarget = copyParams.getDataTarget();
		MatrixDatagridObject matrixSource = determineFilePath(dataSource);
		MatrixDatagridObject matrixTarget = determineFilePath(dataTarget);
		if (matrixSource.isContainer()) {
			copyContainer(matrixSource, matrixTarget);
		} else if (matrixSource.isCollection()) {
			copyCollection(matrixSource, matrixTarget);
		} else if (matrixSource.isDataSet()) {
			copyDataSet(matrixSource, matrixTarget);
		} else {
			throw new IllegalArgumentException(
						"No Grid Details for Copy Operation provided.");
		}
		// Implementation for Operation
		//setSRBResponse(new Object());
	}

	/**
		* Makes a copy of a container
		* @param matrixSource Container Details for copying
		* @param matrixTarget Container Details to copy to
		*/
	private void copyContainer(MatrixDatagridObject source,
										MatrixDatagridObject target) throws IOException {
		toLog("CopyOperation() - copyContainer() -BEGIN", MatrixUtil.DEBUG_LOG);
		String sourceName = evaluateExpressionToString(source.getGridPath());
		String targetName = evaluateExpressionToString(target.getGridPath());
		toLog("CopyOperation() - copyContainer() -sourceName:" + sourceName,
					MatrixUtil.DEBUG_LOG);
		toLog("CopyOperation() - copyContainer() -targetName:" + targetName,
					MatrixUtil.DEBUG_LOG);
		if (sourceName != null && targetName != null) {
			//SRBAccount account = srbCreateSRBAccount();
			//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
			SRBConnection srbConnection = (SRBConnection)
						retrieveGridConnection();
			SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
			toLog("CopyOperation() - copyContainer() -USED FROM CONNECTION",
						MatrixUtil.DEBUG_LOG);
			SRBContainer srbSourceContainer = new SRBContainer(srbFileSystem,
																		sourceName);
			srbSourceContainer.copyTo(new SRBContainer(srbFileSystem,targetName));
			srbFileSystem = null; // For Performance
		} else {
			throw new MatrixException("Cannot resolve variable");
		}
		toLog("CopyOperation() - copyContainer() -END", MatrixUtil.DEBUG_LOG);
	}

	/**
		* Makes a copy of a collection
		* @param matrixSource Collection Details for copying
		* @param matrixTarget Collection Details to copy to
		*/
	private void copyCollection(MatrixDatagridObject source,
								MatrixDatagridObject target) throws IOException {
		toLog("CopyOperation() - copyCollection() -BEGIN", MatrixUtil.DEBUG_LOG);
		String sourceName = evaluateExpressionToString(source.getGridPath());
		String targetName = evaluateExpressionToString(target.getGridPath());
		toLog("CopyOperation() - copyCollection() -sourceName:" + sourceName,
					MatrixUtil.DEBUG_LOG);
		toLog("CopyOperation() - copyCollection() -targetName:" + targetName,
					MatrixUtil.DEBUG_LOG);
		if (sourceName != null && targetName != null) {
			//SRBAccount account = srbCreateSRBAccount();
			//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
			SRBConnection srbConnection = (SRBConnection)
						retrieveGridConnection();
			SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
			toLog("CopyOperation() - copyCollection() -USED FROM CONNECTION",
						MatrixUtil.DEBUG_LOG);
			SRBFile srbSourceCollection = new SRBFile(srbFileSystem, sourceName);
			srbSourceCollection.copyTo(new SRBFile(srbFileSystem,targetName));
			srbFileSystem = null; // For Performance
		} else {
			throw new MatrixException("Cannot resolve variable");
		}
		toLog("CopyOperation() - copyCollection() -END", MatrixUtil.DEBUG_LOG);
	}

	/**
		* Makes a copy of a Data Set
		* @param matrixSource Data Set Details for copying
		* @param matrixTarget Data Set  Details to copy to
		*/
	private void copyDataSet(MatrixDatagridObject source,
								MatrixDatagridObject target) throws IOException {
		toLog("CopyOperation() - copyDataSet() -BEGIN", MatrixUtil.DEBUG_LOG);
		String sourceName = evaluateExpressionToString(source.getGridPath());
		String targetName = evaluateExpressionToString(target.getGridPath());
		toLog("CopyOperation() - copyDataSet() -sourceName:" + sourceName,
					MatrixUtil.DEBUG_LOG);
		toLog("CopyOperation() - copyDataSet() -targetName:" + targetName,
					MatrixUtil.DEBUG_LOG);
		if (sourceName != null && targetName != null) {
			//SRBAccount account = srbCreateSRBAccount();
			//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
			SRBConnection srbConnection = (SRBConnection)
						retrieveGridConnection();
			SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
			toLog("CopyOperation() - copyDataSet() -USED FROM CONNECTION",
						MatrixUtil.DEBUG_LOG);
			SRBFile srbSourceDataSet = new SRBFile(srbFileSystem, sourceName);
			srbSourceDataSet.copyTo(new SRBFile(srbFileSystem,targetName),true);
			srbFileSystem = null; // For Performance
		} else {
			throw new MatrixException("Cannot resolve variable");
		}
		toLog("CopyOperation() - copyDataSet() -END", MatrixUtil.DEBUG_LOG);
	}

	/**
		* Get Operation Name
		* @returns the name of the operation encapsulated by this agent
		*/
	public String getOperationName() {
		return "CopyOperation";
	}
}
