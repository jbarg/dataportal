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
 * copying a Container, Collection or Data Set
 *  
 * @author rmathew
 * 
 */
public class CopyOperation extends MatrixAgent
{
	/**
	 * The method that executes the steps for Copy Operation
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
						
				CopyOp copyOprn = getOperation().getCopyOp();
				CopyParams copyParams = copyOprn.getStdParams();
				StdDatagridObject dataSource = copyParams.getDataSource();
				StdDatagridObject dataTarget = copyParams.getDataTarget();
				
				MatrixDatagridObject matrixSource = determineFilePath(dataSource);
				MatrixDatagridObject matrixTarget = determineFilePath(dataTarget);
				
				if(matrixSource.isContainer())
					copyContainer(matrixSource,matrixTarget);
				else if(matrixSource.isCollection())
					copyCollection(matrixSource,matrixTarget);
				else if(matrixSource.isDataSet())
					copyDataSet(matrixSource,matrixTarget);
				else
					throw new IllegalArgumentException("No Grid Details for Copy Operation provided.");	
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
				logStepEndStatus();
		}
	}
	
	/**
	 * Makes a copy of a container
	 * @param matrixSource Container Details for copying
	 * @param matrixTarget Container Details to copy to
	 */
	private void copyContainer(MatrixDatagridObject matrixSource,
							   MatrixDatagridObject matrixTarget) throws Exception
	{	
		toLog("CopyOperation() - copyContainer() -BEGIN",MatrixUtil.DEBUG_LOG);
		String sourceName = matrixSource.getGridPath();
		String targetName = matrixTarget.getGridPath();
		toLog("CopyOperation() - copyContainer() -sourceName:"+sourceName,MatrixUtil.DEBUG_LOG);
		toLog("CopyOperation() - copyContainer() -targetName:"+targetName,MatrixUtil.DEBUG_LOG);
		if(sourceName!=null && targetName!=null)
		{
			//SRBAccount account = srbCreateSRBAccount();
			//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
			
			SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
			SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
			toLog("CopyOperation() - copyContainer() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
			
			
			SRBContainer srbSourceContainer = new SRBContainer(srbFileSystem,sourceName);
			srbSourceContainer.copyToRemote(targetName);
			srbFileSystem = null; // For Performance 
		}	
		toLog("CopyOperation() - copyContainer() -END",MatrixUtil.DEBUG_LOG);		
	}
	
	/**
	 * Makes a copy of a collection
	 * @param matrixSource Collection Details for copying
	 * @param matrixTarget Collection Details to copy to
	 */
	private void copyCollection(MatrixDatagridObject matrixSource,
								MatrixDatagridObject matrixTarget) throws Exception
	{
		toLog("CopyOperation() - copyCollection() -BEGIN",MatrixUtil.DEBUG_LOG);
		String sourceName = matrixSource.getGridPath();
		String targetName = matrixTarget.getGridPath();
		toLog("CopyOperation() - copyCollection() -sourceName:"+sourceName,MatrixUtil.DEBUG_LOG);
		toLog("CopyOperation() - copyCollection() -targetName:"+targetName,MatrixUtil.DEBUG_LOG);
		if(sourceName!=null && targetName!=null)
		{
			//SRBAccount account = srbCreateSRBAccount();
			//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
			
			SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
			SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
			toLog("CopyOperation() - copyCollection() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
			
			
			SRBFile srbSourceCollection = new SRBFile(srbFileSystem,sourceName);			
			srbSourceCollection.copyToRemote(targetName);
			srbFileSystem = null; // For Performance 
		}	
		toLog("CopyOperation() - copyCollection() -END",MatrixUtil.DEBUG_LOG);		
	}
	
	
	/**
	 * Makes a copy of a Data Set
	 * @param matrixSource Data Set Details for copying
	 * @param matrixTarget Data Set  Details to copy to
	 */
	private void copyDataSet(MatrixDatagridObject matrixSource,
							 MatrixDatagridObject matrixTarget) throws Exception
	{		
		toLog("CopyOperation() - copyDataSet() -BEGIN",MatrixUtil.DEBUG_LOG);
		String sourceName = matrixSource.getGridPath();
		String targetName = matrixTarget.getGridPath();
		toLog("CopyOperation() - copyDataSet() -sourceName:"+sourceName,MatrixUtil.DEBUG_LOG);
		toLog("CopyOperation() - copyDataSet() -targetName:"+targetName,MatrixUtil.DEBUG_LOG);
		if(sourceName!=null && targetName!=null)
		{
			//SRBAccount account = srbCreateSRBAccount();
			//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
			
			SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
			SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
			toLog("CopyOperation() - copyDataSet() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
			
			
			SRBFile srbSourceDataSet = new SRBFile(srbFileSystem,sourceName);			
			srbSourceDataSet.copyToRemote(targetName);
			srbFileSystem = null; // For Performance 
		}	
		toLog("CopyOperation() - copyDataSet() -END",MatrixUtil.DEBUG_LOG);		
	}
	
}