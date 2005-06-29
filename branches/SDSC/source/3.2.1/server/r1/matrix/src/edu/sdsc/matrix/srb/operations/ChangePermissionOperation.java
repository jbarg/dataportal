
/* ====================================================================
 * SDSC Matrix Software License, Version 1.0
 *
 * Copyright (c) 2003 San Diego Supercomputer Center (SDSC), San Diego, USA.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that ALL the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by the
 *        SDSC Matrix Project (http://www.npaci.edu/DICE/SRB/matrix/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "SDSC" and "SDSC Matrix" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact arun@sdsc.edu or srb-chat@sdsc.edu.
 *
 * 5. Products derived from this software may not be called "SDSC",
 *    nor may "SDSC" appear in their name, without prior written
 *    permission of the SDSC.
 *
 * 6. License is not granted for commercial resale, in whole or in part, 
 *	  without the prior written permission from SDSC.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL SDSC OR
 * ITS CONTRIBUTORS (SDSC Matrix Team) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software is intended to consist of voluntary contributions made by many
 * individuals for the SDSC Matrix Project.
 *
 * SDSC Matrix.  
 * Designer & Architect: Arun swaran Jagatheesan (arun@sdsc.edu)
 *
 * Authors (Credits 02/SEP/2003):
 * Allen Ding (alding@sdsc.edu)
 * Arun Jagatheesan (arun@sdsc.edu)
 * Reena Mathew	(rmathew@sdsc.edu)
 * Erik Vandekieft (evk@sdsc.edu)
 */


package edu.sdsc.matrix.srb.operations;


import edu.sdsc.grid.io.srb.SRBException;
import edu.sdsc.grid.io.srb.SRBAccount;
import edu.sdsc.grid.io.srb.SRBFileSystem;
import edu.sdsc.grid.io.srb.SRBFile;
import edu.sdsc.grid.io.srb.SRBContainer;


import edu.sdsc.matrix.srb.parser.*;
import edu.sdsc.matrix.srb.core.SRBConnection;

import edu.sdsc.matrix.srb.util.MatrixUtil;

/**
 * This class is responsible for handling the SRB Operation of Changing
 * Permissions for a Container, Collection or Data Set
 *  
 * @author rmathew
 * 
 */
public class ChangePermissionOperation extends MatrixAgent
{
	/**
	 * The method that executes the steps for Changing Permsissions
	 */
	public void run()
	{
		boolean stepAbort = false;		
		try
		{
			stepAbort = checkStepAbortFlag();
			if(!stepAbort)
			{	
				logStepBeginStatus();
						
				ChangePermissionOp chmodOprn = getOperation().getChangePermissionOp();
				ChangePermissionParams chmodParams = chmodOprn.getStdParams();
				MatrixDatagridObject matrixObject = determineFilePath(chmodParams.getGridData());
				UserInfo targetUser = chmodParams.getTargetUser();
				String permission = getPermissionString(chmodParams.getNewPermission());
				
				if(matrixObject.isContainer())
					changePermissionsForContainer(matrixObject,targetUser,permission);
				else if(matrixObject.isCollection())	
					changePermissionsForCollection(matrixObject,targetUser,permission);
				else if(matrixObject.isDataSet())
					changePermissionsForDataSet(matrixObject,targetUser,permission);
				else
					throw new IllegalArgumentException("No Grid Details for Change Permission Operation provided.");		
				// Implementation for Operation
		
				//setSRBResponse(new Object());
			}
			else
			{
				logStepAbortStatus();
			}	
			
			
		
		}
		catch(SRBException e)
		{
			String exceptionMessage = SRBException.getStandardMessage(e.getType());
			setSRBException(new Exception(exceptionMessage));	
		}
		catch(Exception e)
		{
			setSRBException(e);
		}
		finally
		{
			if(!stepAbort)
			{
				try{
					logStepEndStatus();
				}
				catch(Exception e)
				{
						
				}
			}	
		}
	}	
	
	/**
	 * Changes Permissions for a Container in SRB
	 * @param matrixGridObject Has the Container Details 
	 * @param targetUser User for which persmisions are being modified
	 * @param permission New permission for the Container
	 */
	private void changePermissionsForContainer(MatrixDatagridObject matrixGridObject, 
											   UserInfo targetUser,
											   String permission) throws Exception
	{
		toLog("ChangePermissionOperation() - changePermissionsForContainer() -BEGIN",MatrixUtil.DEBUG_LOG);
		String containerName = 	matrixGridObject.getGridPath();
		toLog("ChangePermissionOperation() - changePermissionsForContainer() -Container NAme : "+containerName,MatrixUtil.DEBUG_LOG);
		String newUserName = null;
		String newUserMdasDomainName = null;
		if(containerName!=null)
		{
			newUserName = targetUser.getUserID();
			newUserMdasDomainName = targetUser.getOrganization().getOrganizationName();
			if(newUserName!=null && newUserMdasDomainName!=null)
			{
				//SRBAccount account = srbCreateSRBAccount();
				//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
				SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
				SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
				toLog("ChangePermissionOperation() - changePermissionsForContainer() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
				
				SRBContainer srbContainer = new SRBContainer(srbFileSystem,containerName);
				srbContainer.changePermissions(permission,newUserName,newUserMdasDomainName);
				srbFileSystem = null; // For Performance 
			}
			else
				throw new IllegalArgumentException("No User Details for Change Permission Operation provided.");	
		}
		else
				throw new IllegalArgumentException("No Container Details for Change Permission Operation provided.");	
		toLog("ChangePermissionOperation() - changePermissionsForContainer() -END",MatrixUtil.DEBUG_LOG);	
	}
	
	/**
	 * Changes Permissions for a Collection in SRB
	 * @param matrixGridObject Has the Collection Details 
	 * @param targetUser User for which persmisions are being modified
	 * @param permission New permission for the Collection
	 */
	private void changePermissionsForCollection(MatrixDatagridObject matrixGridObject,
												UserInfo targetUser,
												String permission) throws Exception
	{
		toLog("ChangePermissionOperation() - changePermissionsForCollection() -BEGIN",MatrixUtil.DEBUG_LOG);
		String collectionName = matrixGridObject.getGridPath();
		toLog("ChangePermissionOperation() - changePermissionsForCollection() -collectionName:"+collectionName,MatrixUtil.DEBUG_LOG);
		String newUserName = null;
		String newUserMdasDomainName = null;
		if(collectionName!=null)
		{
			newUserName = targetUser.getUserID();
			newUserMdasDomainName = targetUser.getOrganization().getOrganizationName();
			if(newUserName!=null && newUserMdasDomainName!=null)
			{
				//SRBAccount account = srbCreateSRBAccount();
				//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
				
				SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
				SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
				toLog("ChangePermissionOperation() - changePermissionsForCollection() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
				
				SRBFile srbCollection = new SRBFile(srbFileSystem,collectionName);
				srbCollection.changePermissions(permission,newUserName,newUserMdasDomainName);
				srbFileSystem = null; // For Performance 
			}
			else
				throw new IllegalArgumentException("No User Details for Change Permission Operation provided.");	
		}
		else
			throw new IllegalArgumentException("No Collection Details for Change Permission Operation provided.");			
		toLog("ChangePermissionOperation() - changePermissionsForCollection() -END",MatrixUtil.DEBUG_LOG);
	}
	
	
	/**
	 * Changes Permissions for a Data Set in SRB
	 * @param matrixGridObject Has the Data Set Details 
	 * @param targetUser User for which persmisions are being modified
	 * @param permission New permission for the Data Set
	 */
	private void changePermissionsForDataSet(MatrixDatagridObject matrixGridObject,
											 UserInfo targetUser,
											 String permission) throws Exception
	{
		toLog("ChangePermissionOperation() - changePermissionsForDataSet() -BEGIN",MatrixUtil.DEBUG_LOG);
		String filePath = matrixGridObject.getGridPath();
		toLog("ChangePermissionOperation() - changePermissionsForDataSet() -filePath:"+filePath,MatrixUtil.DEBUG_LOG);
		String newUserName = null;
		String newUserMdasDomainName = null;
		if(filePath!=null)
		{
			newUserName = targetUser.getUserID();
			newUserMdasDomainName = targetUser.getOrganization().getOrganizationName();
			if(newUserName!=null && newUserMdasDomainName!=null)
			{
				//SRBAccount account = srbCreateSRBAccount();
				//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
				
				SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
				SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
				toLog("ChangePermissionOperation() - changePermissionsForDataSet() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
				
				SRBFile srbFile= new SRBFile(srbFileSystem,filePath);
				srbFile.changePermissions(permission,newUserName,newUserMdasDomainName);
				srbFileSystem = null; // For Performance 
			}
			else
				throw new IllegalArgumentException("No User Details for Change Permission Operation provided.");	
		}
		else
			throw new IllegalArgumentException("No DataSet Details for Change Permission Operation provided.");			
		toLog("ChangePermissionOperation() - changePermissionsForDataSet() -END",MatrixUtil.DEBUG_LOG);
	}
	
	/**
	 * Map the request permission string to SRB Permission details
	 * 
	 * @param permission Permission passed with request
	 */
	private String getPermissionString(String permission)
	{
		String srbPermission  = null;
		if(permission!=null)
		{
		
			if(permission.equals("write"))
				srbPermission = new String("w");
			else if(permission.equals("read"))
				srbPermission = new String("r");
			else if(permission.equals("ownership"))
				srbPermission = new String("rw");
			else if(permission.equals("null"))
				srbPermission = new String("n");
			else if(permission.equals("annotate"))
				srbPermission = new String("t");
		}
		else
			srbPermission = new String("n");
		return(srbPermission);
	}
	
}