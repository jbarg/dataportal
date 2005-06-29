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

import edu.sdsc.matrix.srb.core.GridConnection;
import edu.sdsc.matrix.srb.core.SRBConnection;
import edu.sdsc.matrix.srb.parser.*;

import edu.sdsc.matrix.srb.util.MatrixUtil;

/**
 * This class is responsible for handling the SRB Operation of  
 * creating a Container, Collection or Data Set
 *  
 * @author rmathew
 * 
 */
public class CreateOperation extends MatrixAgent
{
	/**
	 * The method that executes the steps for Creating a Container,Collection or Data Set
	 */
	public void run()
	{
		boolean stepAbort = false;	
		try
		{
			toLog("CreateOperation() - run() -BEGIN",MatrixUtil.DEBUG_LOG);
			stepAbort = checkStepAbortFlag();
			if(!stepAbort)
			{
			
				logStepBeginStatus();
						
				CreateOp createOprn = getOperation().getCreateOp();
				CreateParams createParams = createOprn.getStdParams();
				
				// Implementation for Operation
				MatrixDatagridObject matrixGridObject = determineFilePath(createParams.getDatagridObject());
				Container defaultContainer = createParams.getDefaultContainerForCollection();
				
				if(matrixGridObject.isContainer())
					createContainer(matrixGridObject);
				else if(matrixGridObject.isCollection())
					createCollection(matrixGridObject,defaultContainer);
				else if(matrixGridObject.isDataSet())
					createDataSet(matrixGridObject);
				else
					throw new IllegalArgumentException("No Grid Details for create provided.");
				
		
				//setSRBResponse(new Object());
			}
			else
			{
				logStepAbortStatus();
			}
			
			toLog("CreateOperation() - run() -END",MatrixUtil.DEBUG_LOG);
		
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
	 * Creates a Container in SRB
	 * @param matrixGridObject Container Details	
	 */
	private void createContainer(MatrixDatagridObject matrixGridObject) throws Exception
	{
		toLog("CreateOperation() - createContainer() -BEGIN",MatrixUtil.DEBUG_LOG);
		String containerName = 	matrixGridObject.getGridPath();
		toLog("CreateOperation() - createContainer() -Container NAme : "+containerName,MatrixUtil.DEBUG_LOG);
		if(containerName!=null)
		{
			//SRBAccount account = srbCreateSRBAccount();
			//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
			SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
			SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
			toLog("CreateOperation() - createContainer() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
			SRBContainer srbContainer = new SRBContainer(srbFileSystem,containerName);
			srbContainer.createNewFile();
			srbFileSystem = null; // For Performance 
		}
		toLog("CreateOperation() - createContainer() -END",MatrixUtil.DEBUG_LOG);	
	}
	
	/**
	 * Creates a Collection in SRB
	 * @param matrixGridObject Collection Details
	 * @param defaultContainer Container in which to create Collection	
	 */
	private void createCollection(MatrixDatagridObject matrixGridObject,Container defaultContainer) throws Exception
	{
		toLog("CreateOperation() - createCollection() -BEGIN",MatrixUtil.DEBUG_LOG);
		String collectionName = matrixGridObject.getGridPath();
		toLog("CreateOperation() - createCollection() -collectionName:"+collectionName,MatrixUtil.DEBUG_LOG);
		if(collectionName!=null)
		{
			//SRBAccount account = srbCreateSRBAccount();
			//toLog("CreateOperation() - SRBAccount() -CREATED",MatrixUtil.DEBUG_LOG);
			//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
			SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
			SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
			toLog("CreateOperation() - createCollection() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
			
			SRBFile srbCollection = new SRBFile(srbFileSystem,collectionName);
			srbCollection.mkdir();
			srbFileSystem = null; // For Performance 
		}	
		toLog("CreateOperation() - createCollection() -END",MatrixUtil.DEBUG_LOG);
	}
	
	/**
	 * Creates a Data Set in SRB
	 * @param matrixGridObject Data Set Details	
	 */
	private void createDataSet(MatrixDatagridObject matrixGridObject) throws Exception
	{
		toLog("CreateOperation() - createDataSet() -BEGIN",MatrixUtil.DEBUG_LOG);
		
		String dataSetName = matrixGridObject.getGridPath();
		toLog("CreateOperation() - createDataSet() -dataSetName:"+dataSetName,MatrixUtil.DEBUG_LOG);
		if(dataSetName!=null)
			{
					SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
					SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
					
					toLog("CreateOperation() - createDataSet() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
			
					SRBFile srbDataSet = new SRBFile(srbFileSystem,dataSetName);
					boolean fileCreated  = srbDataSet.createNewFile();
					toLog("CreateOperation() - createDataSet() -fileCreated:"+fileCreated,MatrixUtil.DEBUG_LOG);
					srbFileSystem = null; // For Performance 
				}	
		
		toLog("CreateOperation() - createDataSet() -END",MatrixUtil.DEBUG_LOG);
	}
	
}