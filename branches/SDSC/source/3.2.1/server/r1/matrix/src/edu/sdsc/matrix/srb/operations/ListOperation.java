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
import edu.sdsc.grid.io.GeneralFile;

import edu.sdsc.matrix.srb.parser.*;
import edu.sdsc.matrix.srb.core.SRBConnection;

import java.util.List;

import edu.sdsc.matrix.srb.util.MatrixUtil;


/**
 * This class is responsible for handling the SRB Operation of  
 * listing a Container, Collection or Data Set
 *  
 * @author rmathew
 * 
 */
public class ListOperation extends MatrixAgent
{
	/**
	 * The method that executes the steps for listing contents of a 
	 * Container, Collection or Data Set
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
						
				ListOp listOprn = getOperation().getListOp();
				ListParams listParams = listOprn.getStdParams();
				
				// Implementation for Operation
				MatrixDatagridObject matrixGridObject = determineFilePath(listParams.getDataSource());
				
				if(matrixGridObject.isContainer())
					listContainer(matrixGridObject);
				else if(matrixGridObject.isCollection())
					listCollection(matrixGridObject);
				else if(matrixGridObject.isDataSet())
					listDataSet(matrixGridObject);
				else
					throw new IllegalArgumentException("No Grid Details for deletion provided.");
				
				
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
	 * Lists contents of a container
	 * @param matrixGridObject Container Details
	 */
	private void listContainer(MatrixDatagridObject matrixGridObject) throws Exception
	{
			toLog("ListOperation() - listContainer() -BEGIN",MatrixUtil.DEBUG_LOG);
			String containerName = 	matrixGridObject.getGridPath();
			toLog("ListOperation() - listContainer() -Container NAme : "+containerName,MatrixUtil.DEBUG_LOG);
			String[] listContents = null;
			List collectionList = null;
			List dataSetList = null;
			if(containerName!=null)
			{
				//SRBAccount account = srbCreateSRBAccount();
				//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
				
				SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
				SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
				toLog("ListOperation() - listContainer() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
				
				
				
				SRBContainer srbContainer = new SRBContainer(srbFileSystem,containerName);
				listContents = srbContainer.list();
				toLog("ListOperation() - listContainer() -listContents:"+listContents,MatrixUtil.DEBUG_LOG);
				
				
				//Create Response from Lists
				ListResult listResult = getObjectFactory().createListResult();
				CollectionList collList  = getObjectFactory().createCollectionList();
				collectionList = collList.getCollections();
				dataSetList = collList.getDataSets();
	
				String tempFileName = null;
				String logicalIdentifier = null;
				if(listContents!=null)
				{
					if(listContents.length>0)
					{
						//logicalIdentifier = listContents[0];
						toLog("ListOperation() - listContainer() -logicalIdentifier:"+logicalIdentifier,MatrixUtil.DEBUG_LOG);
						for(int index=0;index<listContents.length;index++)
						{
							tempFileName = listContents[index];
							toLog("ListOperation() - listContainer() -tempFileName:"+tempFileName,MatrixUtil.DEBUG_LOG);
				
							toLog("ListOperation() - listContainer() -tempFileName.charAt(0):"+tempFileName.charAt(0),MatrixUtil.DEBUG_LOG);
							if(tempFileName.charAt(0)=='/')
							{
								toLog("ListOperation() - listContainer() -isDirectory:true",MatrixUtil.DEBUG_LOG);
								// What to set ????
								Collection.StdCollectionType tempCollection = getObjectFactory().createCollectionStdCollectionType();
								tempCollection.setCollectionName(tempFileName);
								Collection collection = getObjectFactory().createCollection();
								collection.setStdCollection(tempCollection) ;
								//collectionList.add(tempCollection);
								collectionList.add(collection);					// CollectionList->Collection->StdCollectionType				
							}
							else// if(tempGeneralFile.isFile())
							{
								
								toLog("ListOperation() - listContainer() -isFile:true",MatrixUtil.DEBUG_LOG);
								// What to set ????
								DataSet tempDataSet = getObjectFactory().createDataSet();
								tempDataSet.setDataIdentifier(tempFileName);
								
								DataSetExtendedList dataSetExtList = getObjectFactory().createDataSetExtendedList(); 
								dataSetExtList.setDataSet(tempDataSet);
								
								
								//tempDataSet.setLogicalLocation(logicalIdentifier);
								dataSetList.add(dataSetExtList);	 // DataSetList->DataSetExtendedList->DataSet
							}
						}
			
			
					}
		
					listResult.setListing(collList);
					OpResult opResult = getObjectFactory().createOpResult();
					opResult.setListResult(listResult);
					setSRBResponse(opResult);
					toLog("ListOperation() - listContainer() -setSRBResponse",MatrixUtil.DEBUG_LOG);
				}
				
				srbFileSystem = null; // For Performance 				
				
			}	
		    toLog("ListOperation() - listContainer() -END",MatrixUtil.DEBUG_LOG);		
	}
	
	/**
	 * Lists contents of a Collection
	 * @param matrixGridObject Collection Details
	 */
	private void listCollection(MatrixDatagridObject matrixGridObject) throws Exception
	{
			toLog("ListOperation() - listCollection() -BEGIN",MatrixUtil.DEBUG_LOG);
			String collectionName = 	matrixGridObject.getGridPath();
			toLog("ListOperation() - listCollection() -Collection NAme : "+collectionName,MatrixUtil.DEBUG_LOG);
			GeneralFile[] listFileContents =null;
			String[] listContents = null;
			List collectionList = null;
			List dataSetList = null;
			if(collectionName!=null)
			{
				//SRBAccount account = srbCreateSRBAccount();
				//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
				
				SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
				SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
				toLog("ListOperation() - listCollection() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
				
				
				SRBFile srbCollection = new SRBFile(srbFileSystem,collectionName);
				listFileContents = srbCollection.listFiles();
				listContents = srbCollection.list();
				toLog("ListOperation() - listCollection() -listFileContents-length : "+listFileContents.length,MatrixUtil.DEBUG_LOG);
				toLog("ListOperation() - listCollection() -listContents-length : "+listContents.length,MatrixUtil.DEBUG_LOG);
				
				String logicalIdentifier = null;
				/*
				for(int index=0;index<listContents.length;index++)
				{
					toLog("ListOperation() - listCollection() -listContents"+index+":"+listContents[index],MatrixUtil.DEBUG_LOG);
				}
				*/
				
				//	Create Response from Lists
				ListResult listResult = getObjectFactory().createListResult();
				CollectionList collList  = getObjectFactory().createCollectionList();
				collectionList = collList.getCollections();
				dataSetList = collList.getDataSets();
				
				String tempFileName = null;
				logicalIdentifier = srbCollection.getCanonicalPath();
				if(listContents!=null)
				{
					if(listContents.length>0)
					{						
						toLog("ListOperation() - listCollection() -logicalIdentifier:"+logicalIdentifier,MatrixUtil.DEBUG_LOG);
						for(int index=0;index<listContents.length;index++)
						{
							tempFileName = listContents[index];
							toLog("ListOperation() - listCollection() -tempFileName:"+tempFileName,MatrixUtil.DEBUG_LOG);
							
							toLog("ListOperation() - listCollection() -tempFileName.charAt(0):"+tempFileName.charAt(0),MatrixUtil.DEBUG_LOG);
							if(tempFileName.charAt(0)=='/')
							{
								toLog("ListOperation() - listCollection() -isDirectory:true",MatrixUtil.DEBUG_LOG);
								// What to set ????
								Collection.StdCollectionType tempCollection = getObjectFactory().createCollectionStdCollectionType();
								tempCollection.setCollectionName(tempFileName);
								Collection collection = getObjectFactory().createCollection();
								collection.setStdCollection(tempCollection) ;
								//collectionList.add(tempCollection);
								collectionList.add(collection);					// CollectionList->Collection->StdCollectionType									
							}
							else// if(tempGeneralFile.isFile())
							{
								toLog("ListOperation() - listContainer() -isFile:true",MatrixUtil.DEBUG_LOG);
								// What to set ????
								DataSet tempDataSet = getObjectFactory().createDataSet();
								tempDataSet.setDataIdentifier(tempFileName);
								
								DataSetExtendedList dataSetExtList = getObjectFactory().createDataSetExtendedList(); 
								dataSetExtList.setDataSet(tempDataSet);
								
								
								//tempDataSet.setLogicalLocation(logicalIdentifier);
								dataSetList.add(dataSetExtList);	 // DataSetList->DataSetExtendedList->DataSet	
							}
						}
						
						
					}
					
					listResult.setListing(collList);
					OpResult opResult = getObjectFactory().createOpResult();
					opResult.setListResult(listResult);
					setSRBResponse(opResult);
					toLog("ListOperation() - listCollection() -setSRBResponse",MatrixUtil.DEBUG_LOG);
				}
				
				srbFileSystem = null; // For Performance 	
				toLog("ListOperation() - listCollection() -listContents:"+listContents,MatrixUtil.DEBUG_LOG);
			}		
			toLog("ListOperation() - listCollection() -END",MatrixUtil.DEBUG_LOG);		
	}
	
	/**
	 * Lists contents of a data set
	 * @param matrixGridObject Data Set Details
	 */
	private void listDataSet(MatrixDatagridObject matrixGridObject) throws Exception
	{
		toLog("ListOperation() - listDataSet() -BEGIN",MatrixUtil.DEBUG_LOG);
		String collectionName = 	matrixGridObject.getGridPath();
		toLog("ListOperation() - listDataSet() -Collection NAme : "+collectionName,MatrixUtil.DEBUG_LOG);
		GeneralFile[] listFileContents =null;
		String[] listContents = null;
		List collectionList = null;
		List dataSetList = null;
		if(collectionName!=null)
		{
			//SRBAccount account = srbCreateSRBAccount();
			//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
			
			SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
			SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
			toLog("ListOperation() - listDataSet() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
			
			
			SRBFile srbCollection = new SRBFile(srbFileSystem,collectionName);
			listFileContents = srbCollection.listFiles();
			listContents = srbCollection.list();
			toLog("ListOperation() - listDataSet() -listFileContents-length : "+listFileContents.length,MatrixUtil.DEBUG_LOG);
			toLog("ListOperation() - listDataSet() -listContents-length : "+listContents.length,MatrixUtil.DEBUG_LOG);
				
			String logicalIdentifier = null;
			/*
			for(int index=0;index<listContents.length;index++)
			{
				toLog("ListOperation() - listCollection() -listContents"+index+":"+listContents[index],MatrixUtil.DEBUG_LOG);
			}
			*/
				
			//	Create Response from Lists
			ListResult listResult = getObjectFactory().createListResult();
			CollectionList collList  = getObjectFactory().createCollectionList();
			collectionList = collList.getCollections();
			dataSetList = collList.getDataSets();
				
			String tempFileName = null;
			if(listContents!=null)
			{
				if(listContents.length>0)
				{
					logicalIdentifier = listContents[0];
					toLog("ListOperation() - listDataSet() -logicalIdentifier:"+logicalIdentifier,MatrixUtil.DEBUG_LOG);
					for(int index=1;index<listContents.length;index++)
					{
						tempFileName = listContents[index];
						toLog("ListOperation() - listDataSet() -tempFileName:"+tempFileName,MatrixUtil.DEBUG_LOG);
							
						toLog("ListOperation() - listDataSet() -tempFileName.charAt(0):"+tempFileName.charAt(0),MatrixUtil.DEBUG_LOG);
						if(tempFileName.charAt(0)=='/')
						{
							toLog("ListOperation() - listDataSet() -isDirectory:true",MatrixUtil.DEBUG_LOG);
							// What to set ????
								Collection.StdCollectionType tempCollection = getObjectFactory().createCollectionStdCollectionType();
								tempCollection.setCollectionName(tempFileName);
								Collection collection = getObjectFactory().createCollection();
								collection.setStdCollection(tempCollection) ;
								//collectionList.add(tempCollection);
								collectionList.add(collection);					// CollectionList->Collection->StdCollectionType									
						}
						else// if(tempGeneralFile.isFile())
						{
							toLog("ListOperation() - listContainer() -isFile:true",MatrixUtil.DEBUG_LOG);
								// What to set ????
								DataSet tempDataSet = getObjectFactory().createDataSet();
								tempDataSet.setDataIdentifier(tempFileName);
								
								DataSetExtendedList dataSetExtList = getObjectFactory().createDataSetExtendedList(); 
								dataSetExtList.setDataSet(tempDataSet);
								
								
								//tempDataSet.setLogicalLocation(logicalIdentifier);
								dataSetList.add(dataSetExtList);	 // DataSetList->DataSetExtendedList->DataSet
						}
					}
						
						
				}
					
				listResult.setListing(collList);
				OpResult opResult = getObjectFactory().createOpResult();
				opResult.setListResult(listResult);
				setSRBResponse(opResult);
				toLog("ListOperation() - listDataSet() -setSRBResponse",MatrixUtil.DEBUG_LOG);
			}
				
			srbFileSystem = null; // For Performance 	
			toLog("ListOperation() - listDataSet() -listContents:"+listContents,MatrixUtil.DEBUG_LOG);
		}		
		toLog("ListOperation() - listDataSet() -END",MatrixUtil.DEBUG_LOG);			
	}
}