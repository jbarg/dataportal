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
import edu.sdsc.matrix.srb.parser.ChangePermissionOp;
import edu.sdsc.matrix.srb.parser.ChangePermissionParams;
import edu.sdsc.matrix.srb.parser.Step;
import edu.sdsc.matrix.srb.parser.UserInfo;
import edu.sdsc.matrix.srb.util.MatrixUtil;

import edu.sdsc.grid.io.srb.SRBContainer;
import edu.sdsc.grid.io.srb.SRBFile;
import edu.sdsc.grid.io.srb.SRBFileSystem;

/**
	* This class is responsible for handling the SRB Operation of Changing
	* Permissions for a Container, Collection or Data Set
	*
	* @author rmathew
	*
	*/
public class ChangePermissionOperation extends MatrixAgent {
	public ChangePermissionOperation(Step step, FlowThread parent,
										ThreadGroup group) {
		super(step, parent, group);
	}

	/**
		* The method that executes the steps for Changing Permsissions
		*/
	public void typeSpecificExecute() {
		ChangePermissionOp chmodOprn = getOperation().getChangePermissionOp();
		ChangePermissionParams chmodParams = chmodOprn.getStdParams();
		MatrixDatagridObject matrixObject = determineFilePath(chmodParams.getGridData());
		UserInfo targetUser = chmodParams.getTargetUser();
		String permission =
					evaluateExpressionToString(getPermissionString(chmodParams.
													getNewPermission()));
		try {
			if (matrixObject.isContainer()) {
				changePermissionsForContainer(matrixObject, targetUser,
													permission);
			} else if (matrixObject.isCollection()) {
				changePermissionsForCollection(matrixObject, targetUser,
														permission);
			} else if (matrixObject.isDataSet()) {
				changePermissionsForDataSet(matrixObject, targetUser,
											permission);
			} else {
				throw new IllegalArgumentException(
							"No Grid Details for Change Permission Operation provided.");
			}
			// Implementation for Operation
		} catch (IOException ioe) {
			throw new MatrixException("Could not execute ChangePermissionOp",
											ioe);
		}
	}

	/**
		* Changes Permissions for a Container in SRB
		* @param matrixGridObject Has the Container Details
		* @param targetUser User for which persmisions are being modified
		* @param permission New permission for the Container
		*/
	private void changePermissionsForContainer(MatrixDatagridObject
														matrixGridObject,
														UserInfo targetUser,
														String permission) throws
				IOException {
		toLog(
					"ChangePermissionOperation() - changePermissionsForContainer() -BEGIN",
					MatrixUtil.DEBUG_LOG);
		String containerName = matrixGridObject.getGridPath();
		toLog("ChangePermissionOperation() - changePermissionsForContainer() -Container NAme : " +
					containerName, MatrixUtil.DEBUG_LOG);
		String newUserName = null;
		String newUserOrganizationName = null;
		if (containerName != null) {
			newUserName = evaluateExpressionToString(targetUser.getUserID());
			newUserOrganizationName = evaluateExpressionToString(targetUser.
																	getOrganization().
																	getOrganizationName());
			if (newUserName != null && newUserOrganizationName != null) {
				//SRBAccount account = srbCreateSRBAccount();
				//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
				SRBConnection srbConnection = (SRBConnection)
							retrieveGridConnection();
				SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
				toLog("ChangePermissionOperation() - changePermissionsForContainer() -USED FROM CONNECTION",
							MatrixUtil.DEBUG_LOG);
				SRBContainer srbContainer = new SRBContainer(srbFileSystem,
																containerName);
				srbContainer.changePermissions(permission, newUserName,
														newUserOrganizationName);
				srbFileSystem = null; // For Performance
			} else {
				throw new MatrixException("Cannot resolve user or org variable");
			}
		} else {
			throw new MatrixException("Cannot resolve variable");
		}
		toLog(
					"ChangePermissionOperation() - changePermissionsForContainer() -END",
					MatrixUtil.DEBUG_LOG);
	}

	/**
		* Changes Permissions for a Collection in SRB
		* @param matrixGridObject Has the Collection Details
		* @param targetUser User for which persmisions are being modified
		* @param permission New permission for the Collection
		*/
	private void changePermissionsForCollection(MatrixDatagridObject
												matrixGridObject,
												UserInfo targetUser,
												String permission) throws
				IOException {
		toLog(
					"ChangePermissionOperation() - changePermissionsForCollection() -BEGIN",
					MatrixUtil.DEBUG_LOG);
		String collectionName = matrixGridObject.getGridPath();
		toLog("ChangePermissionOperation() - changePermissionsForCollection() -collectionName:" +
					collectionName, MatrixUtil.DEBUG_LOG);
		String newUserName = null;
		String newUserOrganizationName = null;
		if (collectionName != null) {
			newUserName = evaluateExpressionToString(targetUser.getUserID());
			newUserOrganizationName = evaluateExpressionToString(targetUser.
																	getOrganization().
																	getOrganizationName());
			if (newUserName != null && newUserOrganizationName != null) {
				//SRBAccount account = srbCreateSRBAccount();
				//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
				SRBConnection srbConnection = (SRBConnection)
							retrieveGridConnection();
				SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
				toLog("ChangePermissionOperation() - changePermissionsForCollection() -USED FROM CONNECTION",
							MatrixUtil.DEBUG_LOG);
				SRBFile srbCollection = new SRBFile(srbFileSystem,
													collectionName);
				srbCollection.changePermissions(permission, newUserName,
												newUserOrganizationName);
				srbFileSystem = null; // For Performance
			} else {
				throw new MatrixException(
							"Cannot resolve username or organization variable");
			}
		} else {
			throw new MatrixException("Cannot resolve variable");
		}
		toLog(
					"ChangePermissionOperation() - changePermissionsForCollection() -END",
					MatrixUtil.DEBUG_LOG);
	}

	/**
		* Changes Permissions for a Data Set in SRB
		* @param matrixGridObject Has the Data Set Details
		* @param targetUser User for which persmisions are being modified
		* @param permission New permission for the Data Set
		*/
	private void changePermissionsForDataSet(MatrixDatagridObject
												matrixGridObject,
												UserInfo targetUser,
												String permission) throws
				IOException {
		toLog(
					"ChangePermissionOperation() - changePermissionsForDataSet() -BEGIN",
					MatrixUtil.DEBUG_LOG);
		String filePath = evaluateExpressionToString(matrixGridObject.getGridPath());
		toLog(
					"ChangePermissionOperation() - changePermissionsForDataSet() -filePath:" +
					filePath, MatrixUtil.DEBUG_LOG);
		String newUserName = null;
		String newUserOrganizationName = null;
		if (filePath != null) {
			newUserName = evaluateExpressionToString(targetUser.getUserID());
			newUserOrganizationName = evaluateExpressionToString(targetUser.
																	getOrganization().
																	getOrganizationName());
			if (newUserName != null && newUserOrganizationName != null) {
				//SRBAccount account = srbCreateSRBAccount();
				//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
				SRBConnection srbConnection = (SRBConnection)
							retrieveGridConnection();
				SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
				toLog("ChangePermissionOperation() - changePermissionsForDataSet() -USED FROM CONNECTION",
							MatrixUtil.DEBUG_LOG);
				SRBFile srbFile = new SRBFile(srbFileSystem, filePath);
				srbFile.changePermissions(permission, newUserName,
												newUserOrganizationName);
				srbFileSystem = null; // For Performance
			} else {
				throw new MatrixException("Cannot resolve user or org variable");
			}
		} else {
			throw new MatrixException("Cannot resolve variable");
		}
		toLog(
					"ChangePermissionOperation() - changePermissionsForDataSet() -END",
					MatrixUtil.DEBUG_LOG);
	}

	/**
		* Map the request permission string to SRB Permission details
		*
		* @param permission Permission passed with request
		*/
	private String getPermissionString(String permission) {
		String srbPermission = null;
		if (permission != null) {
			if (permission.equals("write")) {
				srbPermission = "w";
			} else if (permission.equals("read")) {
				srbPermission = "r";
			} else if (permission.equals("ownership")) {
				srbPermission = "rw";
			} else if (permission.equals("null")) {
				srbPermission = "n";
			} else if (permission.equals("annotate")) {
				srbPermission = "t";
			}
		} else {
			srbPermission = new String("n");
		}
		return (srbPermission);
	}

	/**
		* Get Operation Name
		* @returns the name of the operation encapsulated by this agent
		*/
	public String getOperationName() {
		return "ChangePermissionOperation";
	}
}
