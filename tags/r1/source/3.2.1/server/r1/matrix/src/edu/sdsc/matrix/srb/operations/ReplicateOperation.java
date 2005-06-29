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

import edu.sdsc.matrix.srb.parser.*;
import edu.sdsc.matrix.srb.core.SRBConnection;

import edu.sdsc.matrix.srb.util.MatrixUtil;

/**
 * This class is responsible for handling the SRB Operation of  
 * replicating a Container, Collection or Data Set
 *  
 * @author rmathew
 * 
 */
public class ReplicateOperation extends MatrixAgent
{
	/**
	 * The method that executes the steps for replicating a Container,Collection or Data Set
	 */
	public void run()
	{
		boolean stepAbort = false;
		try
		{
			logStepBeginStatus();
			toLog("ReplicateOperation() - run() -BEGIN",MatrixUtil.DEBUG_LOG);	
			stepAbort = checkStepAbortFlag();
			if(!stepAbort)
			{		
				ReplicateOp replicateOprn = getOperation().getReplicateOp();
				ReplicateParams replicateParams = replicateOprn.getStdParams();
				StdDatagridObject dataSource = replicateParams.getDataSource();
				Collection targetCollection  = replicateParams.getDataTarget().getTargetCollection();
				Resource targetResource = replicateParams.getDataTarget().getTargetResource();
				MatrixDatagridObject matrixSourceObject = determineFilePath(dataSource);		
				
				// Implementation for Operation
				if(matrixSourceObject.isCollection())
					replicateCollection(matrixSourceObject,targetCollection,targetResource);
				else if(matrixSourceObject.isDataSet())
					replicateDataSet(matrixSourceObject,targetCollection,targetResource);			
				else
					throw new IllegalArgumentException("No Grid Details for Replicate Operation provided.");
					
				//setSRBResponse(new Object());
			}
			else
			{
				logStepAbortStatus();
			}	
			toLog("ReplicateOperation() - run() -BEGIN",MatrixUtil.DEBUG_LOG);
		
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
	
	private void replicateContainer(MatrixDatagridObject matrixSourceObject,
									Collection targetCollection,
									Resource targetResource)
	{
				
	}
	
	private void replicateCollection(MatrixDatagridObject matrixSourceObject,
									 Collection targetCollection,
									 Resource targetResource) throws Exception
	{
		toLog("ReplicateOperation() - replicateCollection() -BEGIN",MatrixUtil.DEBUG_LOG);
		String resourceName = null;
		SRBFile srbFile = null;
		SRBFileSystem srbFileSystem = null;
	
		if(targetResource!=null)
		{
			resourceName = 	targetResource.getLogicalName();
		}	
		else if(targetCollection!=null)
		{
			resourceName = targetCollection.getStdCollection().getCollectionName();			
		}
		toLog("ReplicateOperation() - replicateCollection():resourceName"+resourceName,MatrixUtil.DEBUG_LOG);
		if(resourceName!=null)
		{
			//SRBAccount account = srbCreateSRBAccount();
			//srbFileSystem = new SRBFileSystem(account);
			
			SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
			srbFileSystem = srbConnection.getActiveConnection();
			toLog("ReplicateOperation() - replicateCollection() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
			
			
			srbFile = new SRBFile( srbFileSystem, matrixSourceObject.getGridPath() );
			srbFile.replicate(resourceName);
		}
		srbFileSystem = null; // For Performance 	
		toLog("ReplicateOperation() - replicateCollection() -BEGIN",MatrixUtil.DEBUG_LOG);
					
	}
	
	private void replicateDataSet(MatrixDatagridObject matrixSourceObject,
								  Collection targetCollection,
								  Resource targetResource) throws Exception
	{
		toLog("ReplicateOperation() - replicateDataSet() -BEGIN",MatrixUtil.DEBUG_LOG);
		String resourceName = null;
		SRBFile srbFile = null;
		SRBFileSystem srbFileSystem = null;
			
		if(targetResource!=null)
		{
			resourceName = 	targetResource.getLogicalName();
		}	
		else if(targetCollection!=null)
		{
			resourceName = targetCollection.getStdCollection().getCollectionName();			
		}
		toLog("ReplicateOperation() - replicateDataSet():resourceName"+resourceName,MatrixUtil.DEBUG_LOG);
		if(resourceName!=null)
		{
			//SRBAccount account = srbCreateSRBAccount();
			//srbFileSystem = new SRBFileSystem(account);
			
			SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
			srbFileSystem = srbConnection.getActiveConnection();
			toLog("ReplicateOperation() - replicateDataSet() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
			
			
			srbFile = new SRBFile( srbFileSystem, matrixSourceObject.getGridPath() );
			srbFile.replicate(resourceName);
		}
		srbFileSystem = null; // For Performance 	
		toLog("ReplicateOperation() - replicateDataSet() -BEGIN",MatrixUtil.DEBUG_LOG);
	}
	
}