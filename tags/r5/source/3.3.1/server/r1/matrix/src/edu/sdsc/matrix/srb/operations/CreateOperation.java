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
import edu.sdsc.matrix.srb.parser.Container;
import edu.sdsc.matrix.srb.parser.CreateOp;
import edu.sdsc.matrix.srb.parser.CreateParams;
import edu.sdsc.matrix.srb.parser.Step;
import edu.sdsc.matrix.srb.util.MatrixUtil;

import edu.sdsc.grid.io.srb.SRBContainer;
import edu.sdsc.grid.io.srb.SRBFile;
import edu.sdsc.grid.io.srb.SRBFileSystem;

/**
	* This class is responsible for handling the SRB Operation of
	* creating a Container, Collection or Data Set
	*
	* @author rmathew
	*
	*/
public class CreateOperation extends MatrixAgent
{
		public CreateOperation(Step step, FlowThread parent, ThreadGroup group)
		{
				super(step, parent, group);
		}

		/**
			* The method that executes the steps for Creating a Container,Collection or Data Set
			*/
		public void typeSpecificExecute() throws Exception
		{
				CreateOp createOprn = getOperation().getCreateOp();
				CreateParams createParams = createOprn.getStdParams();
				// Implementation for Operation
				MatrixDatagridObject matrixGridObject = determineFilePath(createParams.getDatagridObject());
				Container defaultContainer = createParams.getDefaultContainerForCollection();

				if (matrixGridObject.isContainer())
		createContainer(matrixGridObject);
				else if (matrixGridObject.isCollection())
		createCollection(matrixGridObject, defaultContainer);
				else if (matrixGridObject.isDataSet())
		createDataSet(matrixGridObject);
				else
		throw new IllegalArgumentException("No Grid Details for create provided.");

		}

		/**
			* Creates a Container in SRB
			* @param matrixGridObject Container Details
			*/
		private void createContainer(MatrixDatagridObject matrixGridObject) throws Exception
		{
				toLog("CreateOperation() - createContainer() -BEGIN", MatrixUtil.DEBUG_LOG);

				String containerName = matrixGridObject.getGridPath();
				toLog("CreateOperation() - createContainer() -Container NAme : " + containerName, MatrixUtil.DEBUG_LOG);

				if (containerName != null)
				{
		//SRBAccount account = srbCreateSRBAccount();
		//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
		SRBConnection srbConnection = (SRBConnection)
								retrieveGridConnection();
		SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
		toLog("CreateOperation() - createContainer() -USED FROM CONNECTION",
				MatrixUtil.DEBUG_LOG);
		SRBContainer srbContainer = new SRBContainer(srbFileSystem,
		containerName);
		srbContainer.createNewFile();
		srbFileSystem = null; // For Performance
				}
				else
				{
		throw new MatrixException("Cannot resolve variable");
				}
				toLog("CreateOperation() - createContainer() -END",
		MatrixUtil.DEBUG_LOG);
		}

		/**
			* Creates a Collection in SRB
			* @param matrixGridObject Collection Details
			* @param defaultContainer Container in which to create Collection
			*/
		private void createCollection(MatrixDatagridObject matrixGridObject,
								Container defaultContainer) throws Exception
		{
				toLog("CreateOperation() - createCollection() -BEGIN", MatrixUtil.DEBUG_LOG);
				String collectionName = matrixGridObject.getGridPath();

				toLog("CreateOperation() - createCollection() -collectionName:" + collectionName, MatrixUtil.DEBUG_LOG);
				if (collectionName != null)
				{
		//SRBAccount account = srbCreateSRBAccount();
		//toLog("CreateOperation() - SRBAccount() -CREATED",MatrixUtil.DEBUG_LOG);
		//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
		SRBConnection srbConnection = (SRBConnection) retrieveGridConnection();
		SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
		toLog(
		"CreateOperation() - createCollection() -USED FROM CONNECTION",
		MatrixUtil.DEBUG_LOG);
		SRBFile srbCollection = new SRBFile(srbFileSystem, collectionName);
		srbCollection.mkdir();
		srbFileSystem = null; // For Performance
				}
				else
				{
		throw new MatrixException("Cannot resolve variable");
				}
				toLog("CreateOperation() - createCollection() -END",
		MatrixUtil.DEBUG_LOG);
		}

		/**
			* Creates a Data Set in SRB
			* @param matrixGridObject Data Set Details
			*/
		private void createDataSet(MatrixDatagridObject matrixGridObject) throws Exception
		{
				toLog("CreateOperation() - createDataSet() -BEGIN", MatrixUtil.DEBUG_LOG);
				String dataSetName = matrixGridObject.getGridPath();
				toLog("CreateOperation() - createDataSet() -dataSetName:" + dataSetName, MatrixUtil.DEBUG_LOG);

				if (dataSetName != null)
				{
		SRBConnection srbConnection = (SRBConnection) retrieveGridConnection();
		SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();

		toLog("CreateOperation() - createDataSet() -USED FROM CONNECTION", MatrixUtil.DEBUG_LOG);
		SRBFile srbDataSet = new SRBFile(srbFileSystem, dataSetName);

		boolean fileCreated = srbDataSet.createNewFile();

		// toLog("CreateOperation() - createDataSet() -fileCreated:" + fileCreated, MatrixUtil.DEBUG_LOG);
		srbFileSystem = null; // For Performance
				}
				else
				{
		throw new MatrixException("Cannot resolve variable");
				}
				toLog("CreateOperation() - createDataSet() -END", MatrixUtil.DEBUG_LOG);
		}

		/**
			* Get Operation Name
			* @returns the name of the operation encapsulated by this agent
			*/
		public String getOperationName()
		{
				return "CreateOperation";
		}
}
