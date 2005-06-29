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
 * This class is responsible for handling the SRB Operation of  
 * deleting a Container, Collection or Data Set
 *  
 * @author rmathew
 * 
 */
public class DeleteOperation extends MatrixAgent
{
	/**
	 * The method that executes the steps for Deleting a Container, Collection or Data Set
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
				toLog("DeleteOperation() - createContainer() -BEGIN",MatrixUtil.DEBUG_LOG);		
				DeleteOp deleteOprn = getOperation().getDeleteOp();
				DeleteParams deleteParams = deleteOprn.getStdParams();
				
				// Implementation for Operation
				
				//String filePath =  determineFilePath(deleteParams.getGridObject());
				MatrixDatagridObject matrixGridObject = determineFilePath(deleteParams.getGridObject());
	
				if(matrixGridObject.isContainer())
					deleteContainer(matrixGridObject);
				else if(matrixGridObject.isCollection())
					deleteCollection(matrixGridObject);
				else if(matrixGridObject.isDataSet())
					deleteDataSet(matrixGridObject);
				else
					throw new IllegalArgumentException("No Grid Details for deletion provided.");
						
				//setSRBResponse(new Object());
			}
			else
			{
				logStepAbortStatus();
			}		
			toLog("DeleteOperation() - createContainer() -END",MatrixUtil.DEBUG_LOG);
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
				logStepEndStatus();
		}
	}	
	
	/**
	 * Deletes a Container in SRB
	 * @param gridObject Container Details	
	 */
	private void deleteContainer(MatrixDatagridObject gridObject) throws Exception
	{
		toLog("DeleteOperation() - deleteContainer() -BEGIN",MatrixUtil.DEBUG_LOG);
		SRBFile srbFile = null;
		SRBFileSystem srbFileSystem = null;	
		String containerName = gridObject.getGridPath();
		toLog("DeleteOperation() - deleteContainer() -containerName:"+containerName,MatrixUtil.DEBUG_LOG);
		if(containerName!=null)
		{
			//SRBAccount account = srbCreateSRBAccount();
			//srbFileSystem = new SRBFileSystem(account);
			
			SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
			srbFileSystem = srbConnection.getActiveConnection();
			toLog("DeleteOperation() - deleteContainer() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
			
			
			SRBContainer srbContainer = new SRBContainer(srbFileSystem,containerName);
			srbContainer.delete();
			srbFileSystem = null; // For Performance 
		}	
		toLog("DeleteOperation() - deleteContainer() -END",MatrixUtil.DEBUG_LOG);
	}
	
	/**
	 * Deletes a Collection in SRB
	 * @param gridObject Collection Details	
	 */
	private void deleteCollection(MatrixDatagridObject gridObject) throws Exception
	{
		toLog("DeleteOperation() - deleteCollection() -BEGIN",MatrixUtil.DEBUG_LOG);
		SRBFile srbFile = null;
		SRBFileSystem srbFileSystem = null;
		String filePath = gridObject.getGridPath();
		toLog("DeleteOperation() - deleteCollection() -filePath:"+filePath,MatrixUtil.DEBUG_LOG);
		if(filePath!=null)
		{
			//SRBAccount account = srbCreateSRBAccount();
			//srbFileSystem = new SRBFileSystem(account);
			
			SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
			srbFileSystem = srbConnection.getActiveConnection();
			toLog("DeleteOperation() - deleteCollection() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
			
			
			srbFile = new SRBFile( srbFileSystem, filePath );
			srbFile.delete();
			srbFileSystem = null; // For Performance 
		}
		toLog("DeleteOperation() - deleteCollection() -END",MatrixUtil.DEBUG_LOG);
	}		
	
	/**
	 * Deletes a Data Set in SRB
	 * @param gridObject Data Set Details	
	 */
	private void deleteDataSet(MatrixDatagridObject gridObject) throws Exception
	{
		toLog("DeleteOperation() - deleteDataSet() -BEGIN",MatrixUtil.DEBUG_LOG);
		SRBFile srbFile = null;
		SRBFileSystem srbFileSystem = null;
		String filePath = gridObject.getGridPath();
		toLog("DeleteOperation() - deleteDataSet() -filePath:"+filePath,MatrixUtil.DEBUG_LOG);
		if(filePath!=null)
		{
			//SRBAccount account = srbCreateSRBAccount();
			//srbFileSystem = new SRBFileSystem(account);
			
			SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
			srbFileSystem = srbConnection.getActiveConnection();
			toLog("DeleteOperation() - deleteDataSet() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
			
			
			srbFile = new SRBFile( srbFileSystem, filePath );
			srbFile.delete();
			srbFileSystem = null; // For Performance 
		}
		toLog("DeleteOperation() - deleteDataSet() -END",MatrixUtil.DEBUG_LOG);
	}
	
	
}