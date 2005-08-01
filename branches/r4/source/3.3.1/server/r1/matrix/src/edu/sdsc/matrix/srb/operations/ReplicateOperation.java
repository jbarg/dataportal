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
import edu.sdsc.matrix.srb.parser.Collection;
import edu.sdsc.matrix.srb.parser.ReplicateOp;
import edu.sdsc.matrix.srb.parser.ReplicateParams;
import edu.sdsc.matrix.srb.parser.Resource;
import edu.sdsc.matrix.srb.parser.StdDatagridObject;
import edu.sdsc.matrix.srb.parser.Step;
import edu.sdsc.matrix.srb.parser.StdCollection;
import edu.sdsc.matrix.srb.util.MatrixUtil;

import edu.sdsc.grid.io.srb.SRBFile;
import edu.sdsc.grid.io.srb.SRBFileSystem;

/**
	* This class is responsible for handling the SRB Operation of
	* replicating a Container, Collection or Data Set
	*
	* @author rmathew
	*
	*/
//@TODO This code looks ugly -
// I just dont know why she is using this !=null comparision in succession - some thing wrong
// and has to be updated - Arun. Also if no resource, default resources/collection must be used.
// if(resourceName != null)
//   {
//	 toLog("ReplicateOperation() - replicateCollection():resourceName"+resourceName,MatrixUtil.DEBUG_LOG);
//	 if(resourceName!=null)
public class ReplicateOperation extends MatrixAgent {
	public ReplicateOperation(Step step, FlowThread parent, ThreadGroup group) {
		super(step, parent, group);
	}

	/**
		* The method that executes the steps for replicating a Container,Collection or Data Set
		*/
	public void typeSpecificExecute() throws Exception {
			ReplicateOp replicateOprn = getOperation().getReplicateOp();
			ReplicateParams replicateParams = replicateOprn.getStdParams();
			StdDatagridObject dataSource = replicateParams.getDataSource();
			Collection targetCollectionAbstract =
			replicateParams.getDataTarget().getTargetCollection();
			StdCollection targetCollection =
			resolvePossibleReference(targetCollectionAbstract);
			Resource targetResource = replicateParams.getDataTarget().
									getTargetResource();
			MatrixDatagridObject matrixSourceObject = determineFilePath(dataSource);
			// Implementation for Operation
			if (matrixSourceObject.isCollection())
			{
					replicateCollection(matrixSourceObject, targetCollection,
									targetResource);
			}
			else if (matrixSourceObject.isDataSet())
			{
					replicateDataSet(matrixSourceObject, targetCollection, targetResource);
			}
			else
			{
					throw new IllegalArgumentException(
					"No Grid Details for Replicate Operation provided.");
			}
			//setSRBResponse(new Object());
	}

	private void replicateContainer(MatrixDatagridObject matrixSourceObject,
									Collection targetCollection,
									Resource targetResource) {
	}

	private void replicateCollection(MatrixDatagridObject matrixSourceObject,
										StdCollection targetCollection,
										Resource targetResource) throws Exception {
		toLog("ReplicateOperation() - replicateCollection() -BEGIN",
					MatrixUtil.DEBUG_LOG);
		String resourceName = null;
		SRBFile srbFile = null;
		SRBFileSystem srbFileSystem = null;
		if (targetResource != null) {
			resourceName = evaluateExpressionToString(targetResource.getLogicalName());
		} else if (targetCollection != null) {
			resourceName = evaluateExpressionToString(targetCollection.getCollectionName());
		}
		if (resourceName != null) {
			toLog("ReplicateOperation() - replicateCollection():resourceName" +
						resourceName, MatrixUtil.DEBUG_LOG);
			if (resourceName != null) {
				//SRBAccount account = srbCreateSRBAccount();
				//srbFileSystem = new SRBFileSystem(account);
				SRBConnection srbConnection = (SRBConnection)
							retrieveGridConnection();
				srbFileSystem = srbConnection.getActiveConnection();
				toLog(
							"ReplicateOperation() - replicateCollection() -USED FROM CONNECTION",
							MatrixUtil.DEBUG_LOG);
				srbFile = new SRBFile(srbFileSystem,
											matrixSourceObject.getGridPath());
				srbFile.replicate(resourceName);
			}
			srbFileSystem = null; // For Performance
		} else {
			throw new MatrixException(
						"resource name is null: could be unresolved var");
		}
		toLog("ReplicateOperation() - replicateCollection() -END",
					MatrixUtil.DEBUG_LOG);
	}

	private void replicateDataSet(MatrixDatagridObject matrixSourceObject,
										StdCollection targetCollection,
										Resource targetResource) throws Exception {
		toLog("ReplicateOperation() - replicateDataSet() -BEGIN",
					MatrixUtil.DEBUG_LOG);
		String resourceName = null;
		SRBFile srbFile = null;
		SRBFileSystem srbFileSystem = null;
		if (targetResource != null) {
			resourceName = evaluateExpressionToString(targetResource.getLogicalName());
		} else if (targetCollection != null) {
			resourceName = evaluateExpressionToString(targetCollection.getCollectionName());
		}
		if (resourceName != null) {
			toLog("ReplicateOperation() - replicateDataSet():resourceName" +
						resourceName, MatrixUtil.DEBUG_LOG);
			if (resourceName != null) {
				//SRBAccount account = srbCreateSRBAccount();
				//srbFileSystem = new SRBFileSystem(account);
				SRBConnection srbConnection = (SRBConnection)
							retrieveGridConnection();
				srbFileSystem = srbConnection.getActiveConnection();
				toLog(
							"ReplicateOperation() - replicateDataSet() -USED FROM CONNECTION",
							MatrixUtil.DEBUG_LOG);
				srbFile = new SRBFile(srbFileSystem,
											matrixSourceObject.getGridPath());
				srbFile.replicate(resourceName);
			}
			srbFileSystem = null; // For Performance
		} else {
			throw new MatrixException(
						"resource name is null: could be unresolved var");
		}
		toLog("ReplicateOperation() - replicateDataSet() -END",
					MatrixUtil.DEBUG_LOG);
	}

	/**
		* Get Operation Name
		* @returns the name of the operation encapsulated by this agent
		*/
	public String getOperationName() {
		return "ReplicateOperation";
	}
}
