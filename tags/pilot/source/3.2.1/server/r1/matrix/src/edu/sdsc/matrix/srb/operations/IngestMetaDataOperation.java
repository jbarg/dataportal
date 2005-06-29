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

import edu.sdsc.grid.io.MetaDataRecordList;

import edu.sdsc.grid.io.GeneralFile;
import edu.sdsc.grid.io.GeneralFileSystem;
import edu.sdsc.grid.io.FileFactory;
import edu.sdsc.grid.io.srb.SRBMetaDataSet;
import edu.sdsc.grid.io.srb.SRBMetaDataRecordList; 
import edu.sdsc.grid.io.MetaDataTable;
import edu.sdsc.grid.io.MetaDataCondition;
import edu.sdsc.grid.io.MetaDataField;

import java.util.Vector;
import edu.sdsc.matrix.srb.util.SRBMetaDataFieldMappings;
import edu.sdsc.matrix.srb.util.MatrixUtil;
import java.util.List;
import java.util.Date;
import java.util.Calendar;
/**
 * 
 * @author rmathew
 *
 * This class is used for ingesting metadata for
 * 		- Collection
 * 		- Data Set
 * 		- Container
 */
public class IngestMetaDataOperation extends MatrixAgent
{
	/**
	 *	  The method that executes the steps for ingesting meta data for 
	 *    a Container,Collection or Data Set
	 */
	public void run()
	{
		boolean stepAbort = false;	
		try
		{
			toLog("IngestMetaDataOperation() - run() -BEGIN",MatrixUtil.DEBUG_LOG);
			stepAbort = checkStepAbortFlag();
			if(!stepAbort)
			{
			
				logStepBeginStatus();
						
				IngestMetadataOp ingestOprn = getOperation().getIngestMetadataOp();
				IngestMetadataParams ingestParams = ingestOprn.getStdParams();
				MetaData ingestMetaData = ingestParams.getMetadata(); 

				// Implementation for Operation
				MatrixDatagridObject matrixGridObject = determineFilePath(ingestParams.getDataGridObject());
								
				if(matrixGridObject.isContainer())
					ingestMetadataForContainer(matrixGridObject,ingestMetaData);
				else if(matrixGridObject.isCollection())
					ingestMetadataForFile(matrixGridObject,ingestMetaData);
				else if(matrixGridObject.isDataSet())
					ingestMetadataForFile(matrixGridObject,ingestMetaData);
				else
					throw new IllegalArgumentException("No Grid Details for ingest provided.");
				
		
				//setSRBResponse(new Object());
			}
			else
			{
				logStepAbortStatus();
			}
			
			toLog("IngestMetaDataOperation() - run() -END",MatrixUtil.DEBUG_LOG);
		
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
	 * Inserts Metadata for a container
	 * @param matrixGridObject Container Details 
	 * @param containerMetaData Meta data about Container
	 */
	private void ingestMetadataForContainer(MatrixDatagridObject matrixGridObject, MetaData containerMetaData) throws Exception
	{
		toLog("IngestMetaDataOperation() - ingestMetadataForContainer() -BEGIN",MatrixUtil.DEBUG_LOG);
		String containerName = 	matrixGridObject.getGridPath();
		toLog("IngestMetaDataOperation() - ingestMetadataForContainer() -Container NAme : "+containerName,MatrixUtil.DEBUG_LOG);
		if(containerName!=null)
		{
			//SRBAccount account = srbCreateSRBAccount();
			//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
			SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
			SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
			toLog("IngestMetaDataOperation() - createContainer() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
			SRBContainer srbContainer = new SRBContainer(srbFileSystem,containerName);
			srbContainer.createNewFile();
			srbFileSystem = null; // For Performance 
		}
		toLog("IngestMetaDataOperation() - ingestMetadataForContainer() -END",MatrixUtil.DEBUG_LOG);	
	}
	
	/**
	 * Inserts metadata for a collection
	 * NOTE : This isn't being used, instead ingestMetadataForFile() is used
	 */
	private void ingestMetadataForCollection(MatrixDatagridObject matrixGridObject,MetaData collectionMetaData) throws Exception
	{
		toLog("IngestMetaDataOperation() - ingestMetadataForCollection() -BEGIN",MatrixUtil.DEBUG_LOG);
		String collectionName = matrixGridObject.getGridPath();
		toLog("IngestMetaDataOperation() - ingestMetadataForCollection() -collectionName:"+collectionName,MatrixUtil.DEBUG_LOG);
		if(collectionName!=null)
		{
			//SRBAccount account = srbCreateSRBAccount();
			//toLog("CreateOperation() - SRBAccount() -CREATED",MatrixUtil.DEBUG_LOG);
			//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
			SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
			SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
			toLog("IngestMetaDataOperation() - ingestMetadataForCollection() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
			
			SRBFile srbCollection = new SRBFile(srbFileSystem,collectionName);
			srbCollection.mkdir();
			srbFileSystem = null; // For Performance 
		}	
		toLog("IngestMetaDataOperation() - ingestMetadataForCollection() -END",MatrixUtil.DEBUG_LOG);
	}
	
	/**
	 * Inserts metadata for a data set
	 * NOTE : This isn't being used, instead ingestMetadataForFile() is used
	 */
	private void ingestMetadataForDataSet(MatrixDatagridObject matrixGridObject,MetaData dataSetMetaData) throws Exception
	{
		toLog("IngestMetaDataOperation() - ingestMetadataForDataSet() -BEGIN",MatrixUtil.DEBUG_LOG);
		
		String dataSetName = matrixGridObject.getGridPath();
		toLog("IngestMetaDataOperation() - ingestMetadataForDataSet() -dataSetName:"+dataSetName,MatrixUtil.DEBUG_LOG);
		if(dataSetName!=null)
			{
					SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
					SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
					
					toLog("IngestMetaDataOperation() - ingestMetadataForDataSet() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
			
					SRBFile srbDataSet = new SRBFile(srbFileSystem,dataSetName);
					boolean fileCreated  = srbDataSet.createNewFile();
					toLog("IngestMetaDataOperation() - ingestMetadataForDataSet() -fileCreated:"+fileCreated,MatrixUtil.DEBUG_LOG);
					srbFileSystem = null; // For Performance 
				}	
		
		toLog("IngestMetaDataOperation() - ingestMetadataForDataSet() -END",MatrixUtil.DEBUG_LOG);
	}
	
	/**
	 * Inserts metadata for a collection or dataset
	 * @param matrixGridObject Data Set or Collection Details
	 * @param dataSetMetaData Meta Data to be ingested
	 */
	private void ingestMetadataForFile(MatrixDatagridObject matrixGridObject,MetaData dataSetMetaData) throws Exception
	{
		toLog("IngestMetaDataOperation() - ingestMetadataForFile() -BEGIN",MatrixUtil.DEBUG_LOG);
		String dataSetName = matrixGridObject.getGridPath();
		GeneralFile file = null;
		GeneralFileSystem fileSystem =null;
		MetaDataRecordList[] mdListForUpdate = null;
		toLog("IngestMetaDataOperation() - ingestMetadataForFile() -dataSetName:"+dataSetName,MatrixUtil.DEBUG_LOG);
		if(dataSetName!=null)
		{
			SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
			SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
			SRBAccount srbAccount = srbConnection.getSRBAccount();
			fileSystem = FileFactory.newFileSystem(srbAccount);		
			toLog("IngestMetaDataOperation() - ingestMetadataForFile() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
			file = FileFactory.newFile( fileSystem, dataSetName );
			toLog("IngestMetaDataOperation() - ingestMetadataForFile() -dataSetName:"+dataSetName,MatrixUtil.DEBUG_LOG);
			mdListForUpdate = extractMetadataValues(dataSetMetaData);
			
			if(mdListForUpdate!=null)
			{
				toLog("IngestMetaDataOperation() - ingestMetadataForFile() -Number of Update Calls"+mdListForUpdate.length,MatrixUtil.DEBUG_LOG);
				toLog("IngestMetaDataOperation() - ingestMetadataForFile() -File Name:"+file.getName(),MatrixUtil.DEBUG_LOG);
				toLog("IngestMetaDataOperation() - ingestMetadataForFile() -File Path:"+file.getPath(),MatrixUtil.DEBUG_LOG);
				//printQueryResults( mdListForUpdate );
				
				for(int listCount=0;listCount<mdListForUpdate.length;listCount++)
				{
					toLog("IngestMetaDataOperation() - ingestMetadataForFile() -MODIFY BEGIN for "+mdListForUpdate[listCount].getFieldName(0),MatrixUtil.DEBUG_LOG);
					file.modifyMetaData(mdListForUpdate[listCount] );
					toLog("IngestMetaDataOperation() - ingestMetadataForFile() -MODIFY COMPLETE",MatrixUtil.DEBUG_LOG);
				}			
			}	
		}
		toLog("IngestMetaDataOperation() - ingestMetadataForFile() -END",MatrixUtil.DEBUG_LOG);
	}
	
	
	/**
	 * Extract the metadata values for insert/update
	 * @param metaDataForUpdate metadata to be updated
	 * @return List of meta data
	 */	
	private MetaDataRecordList[] extractMetadataValues(MetaData metaDataForUpdate)
	{
		toLog("IngestMetaDataOperation() - extractMetadataValues() -BEGIN",MatrixUtil.DEBUG_LOG);
		MetaDataRecordList metaDataList[] = null;//new MetaDataRecordList[1];
		MetaDataRecordList tempRecord = null;
		Vector finalList = new Vector();//null;
		Vector tempList = null;
		
		SystemMetaData systemMD = metaDataForUpdate.getSystemMeta();
		
		if(systemMD!=null)
		{
		
			// Extract the different type of metadata parameters
			CollectionSystemMetaData collectionMD  = systemMD.getCollectionSysMeta(); 
			UserSystemMetaData userMD =  systemMD.getUserSysMeta() ;
			ResourceSystemMetaData resourceMD = systemMD.getResourceSysMeta(); 
			DatasetSystemMetaData dataSetMD = systemMD.getDataSetSysMeta()  ;
			
			/*
			tempList = 	  extractCollectionMetaData(collectionMD);
			if(tempList!=null)	
				finalList.addAll(tempList);
			*/
			
			/*
			tempList = 	  extractUserMetaData(userMD);
			if(tempList!=null)	
				finalList.addAll(tempList);
			*/
			/*
			tempList = 	  extractResourceMetaData(resourceMD);
			if(tempList!=null)	
				finalList.addAll(tempList);
			*/
			
			
			tempList = 	  extractDataSetMetaData(dataSetMD);
			if(tempList!=null)	
				finalList.addAll(tempList);
		}
			
		// User Defined Meta Data from AVU-Pairs	
		tempRecord =  extractAVUPairsForUserDefinedMetaData(metaDataForUpdate.getAVUPairs());
		if(tempRecord!=null)
			finalList.add(tempRecord);	
		
		// Convert the list of metadata values to an array
		if(finalList!=null && finalList.size()>0)
		{
			metaDataList =  new MetaDataRecordList[finalList.size()]; 
			finalList.copyInto(metaDataList);
		}	
		
		toLog("IngestMetaDataOperation() - extractMetadataValues() -END",MatrixUtil.DEBUG_LOG);
		return (metaDataList);
	}
	
	/**
	 * Extracts Collection related Meta Data
	 * @param collectionMD Meta Data for Collection
	 * @return List of meta data 
	 */
	private Vector extractCollectionMetaData(CollectionSystemMetaData collectionMD)
	{
		toLog("IngestMetaDataOperation() - extractCollectionMetaData() -BEGIN",MatrixUtil.DEBUG_LOG);
		Vector metaDataList = null;
		if(collectionMD!=null)
		{
		
			
			
			// Test Code
			MetaDataRecordList[] rl= new MetaDataRecordList[1]; 
			rl[0] = new SRBMetaDataRecordList( SRBMetaDataSet.getField(
								SRBMetaDataSet.FILE_COMMENTS ), "New Comment :"+getTransactionId() );
			//rl[0].addRecord( SRBMetaDataSet.getField(
			//					SRBMetaDataSet.FILE_COMMENTS ), "new comments go here." );
			
			metaDataList = new Vector();
			metaDataList.add(rl[0]);		
			toLog("IngestMetaDataOperation() - extractCollectionMetaData() -TEST MD ADDED",MatrixUtil.DEBUG_LOG);
					
			toLog("IngestMetaDataOperation() - extractCollectionMetaData() -END",MatrixUtil.DEBUG_LOG);
		}
		return (metaDataList);
	}
	
	/**
	 * Extracts User related Meta Data
	 * @param userMD User Related Meta Data
	 * @return List of user meta data
	 */
	private Vector extractUserMetaData(UserSystemMetaData userMD)
	{
			toLog("IngestMetaDataOperation() - extractUserMetaData() -BEGIN",MatrixUtil.DEBUG_LOG);
			Vector metaDataList = null;
			
			if(userMD!=null)
			{
				
			}
			
			// TEST CODE
//		Test Code for Meta Data Table
			 Object[][] definableMetaDataValues = new Object[3][2];
			 definableMetaDataValues[0][0] = "language";
			 definableMetaDataValues[0][1] = "French";

			 definableMetaDataValues[1][0] = "versionControl";
			 definableMetaDataValues[1][1] = "1.2.3";

			 definableMetaDataValues[2][0] = "versionSize";
			 definableMetaDataValues[2][1] = new Integer(789);
		
			 int[] operators = new int[definableMetaDataValues.length];
						 //only equal and not equal work for strings.
						 operators[0] = MetaDataCondition.EQUAL;
						 operators[1] = MetaDataCondition.EQUAL;
						 operators[2] = MetaDataCondition.EQUAL;
		
		
			 MetaDataTable metaDataTable = new MetaDataTable( operators, definableMetaDataValues );
		
		
			MetaDataRecordList[] rl= new MetaDataRecordList[1]; 
			rl[0] = new SRBMetaDataRecordList( SRBMetaDataSet.getField(
									SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES ), metaDataTable );
		
		metaDataList = new Vector();
				metaDataList.add(rl[0]);		
				toLog("IngestMetaDataOperation() - extractCollectionMetaData() -TEST METADATA TABLE ADDED",MatrixUtil.DEBUG_LOG);
		
		
			toLog("IngestMetaDataOperation() - extractUserMetaData() -END",MatrixUtil.DEBUG_LOG);
			return (metaDataList);
	}
	
	/**
	 * Extracts Resource related Meta Data
	 * @param resourceMD Meta data for resources
	 * @return List of meta data
	 */
	private Vector extractResourceMetaData(ResourceSystemMetaData resourceMD)
	{
			toLog("IngestMetaDataOperation() - extractResourceMetaData() -BEGIN",MatrixUtil.DEBUG_LOG);
			Vector metaDataList = null;
			if(resourceMD!=null)
			{
				
			}
			toLog("IngestMetaDataOperation() - extractResourceMetaData() -END",MatrixUtil.DEBUG_LOG);
			return (metaDataList);
	}
	
	
	/**
	 * Extracts Data Set related Meta Data
	 * @param dataSetMD Meta Data for Data Set
	 * @return List of meta data
	 */
	private Vector extractDataSetMetaData(DatasetSystemMetaData dataSetMD)
	{
			toLog("IngestMetaDataOperation() - extractDataSetMetaData() -BEGIN",MatrixUtil.DEBUG_LOG);
			Vector metaDataList = null;
			String fieldName = null; 
			String fieldValue = null;
			MetaDataRecordList mdForUpdate = null;
			Date tempDate = null;
			Calendar tempCalendar =null;
			
			if(dataSetMD!=null)
			{
				fieldValue = dataSetMD.getIsDirty();
				if(fieldValue!=null)
					mdForUpdate = createRecordForUpdate("IS_DIRTY", fieldValue,mdForUpdate);
				
				fieldValue = dataSetMD.getFileIsCompressed();
				if(fieldValue!=null)
					mdForUpdate = createRecordForUpdate("FILE_IS_COMPRESSED", fieldValue,mdForUpdate);
				
				if(dataSetMD.getFileHide()!=0)
				{
					fieldValue = String.valueOf(dataSetMD.getFileHide());
					if(fieldValue!=null)
						mdForUpdate = createRecordForUpdate("FILE_HIDE", fieldValue,mdForUpdate);
				}		
				
				// This field cannot be modified through JARGON
				/*
				fieldValue = dataSetMD.getFileVerNum();
				mdForUpdate = createRecordForUpdate("FILE_VER_NUM", fieldValue,mdForUpdate);
				*/
				
				// This field cannot be modified through JARGON
				/*
				fieldValue = dataSetMD.getFileClassType();
				mdForUpdate = createRecordForUpdate("FILE_CLASS_TYPE", fieldValue,mdForUpdate);
				*/
				
				tempCalendar = dataSetMD.getFileExpiryDate();
				if(tempCalendar!=null)
				{
					tempDate = tempCalendar.getTime();
					fieldValue = MatrixUtil.formatDate("yyyy-MM-dd-HH.mm.ss",tempDate);
					mdForUpdate = createRecordForUpdate("FILE_EXPIRY_DATE", fieldValue,mdForUpdate);
				}
				
				
				fieldValue = dataSetMD.getFileExpireDate2();
				if(fieldValue!=null)
					mdForUpdate = createRecordForUpdate("FILE_EXPIRE_DATE_2", fieldValue,mdForUpdate);
				
				
				tempCalendar = dataSetMD.getModificationDate();
				if(tempCalendar!=null)
				{
					tempDate = tempCalendar.getTime();
					fieldValue = MatrixUtil.formatDate("yyyy-MM-dd-HH.mm.ss",tempDate);
					mdForUpdate = createRecordForUpdate("MODIFICATION_DATE", fieldValue,mdForUpdate);
				}
				
				tempCalendar = dataSetMD.getFilePinExpiry();
				if(tempCalendar!=null)
				{
					tempDate = tempCalendar.getTime();
					fieldValue = MatrixUtil.formatDate("yyyy-MM-dd-HH.mm.ss",tempDate);
					mdForUpdate = createRecordForUpdate("FILE_PIN_EXPIRY", fieldValue,mdForUpdate);
				}
				
				fieldValue = dataSetMD.getFileAuditFlag();
				if(fieldValue!=null)
					mdForUpdate = createRecordForUpdate("FILE_AUDITFLAG", fieldValue,mdForUpdate);
				
				
				// Cannot be modfied : NOT IMPLEMENTED
				OrganizationInfo pinOrgInfo = dataSetMD.getFilePinOwnerDomain();
				if(pinOrgInfo!=null)
				{
						
				}
				
				
				if(dataSetMD.getFileReplicationEnum()!=0)
				{
					fieldValue = String.valueOf(dataSetMD.getFileReplicationEnum());
					if(fieldValue!=null)
						mdForUpdate = createRecordForUpdate("FILE_REPLICATION_ENUM", fieldValue,mdForUpdate);
				}		
				
				
				if(dataSetMD.getSize()!=0)
				{
					fieldValue = String.valueOf(dataSetMD.getSize());
					if(fieldValue!=null)
						mdForUpdate = createRecordForUpdate("SIZE", fieldValue,mdForUpdate);
				}		 
				
				DataLock fileLock = dataSetMD.getFileLock();
				if(fileLock!=null)
				{
					// Only Field Modifiable
					fieldValue = String.valueOf(fileLock.getLockNum());
					mdForUpdate = createRecordForUpdate("FILE_LOCK_NUM", fieldValue,mdForUpdate);  
				}
				
				
				// Cannot be modfied
				/*
				tempCalendar = dataSetMD.getCreationDate();
				if(tempCalendar!=null)
				{
					tempDate = tempCalendar.getTime();
					fieldValue = MatrixUtil.formatDate("yyyy-MM-dd-HH.mm.ss",tempDate);
					mdForUpdate = createRecordForUpdate("CREATION_DATE", fieldValue,mdForUpdate);
				}
				 */ 
				
				if(dataSetMD.getOffset()!=0)
				{
					fieldValue = String.valueOf(dataSetMD.getOffset());
					if(fieldValue!=null)
						mdForUpdate = createRecordForUpdate("OFFSET", fieldValue,mdForUpdate);
				}		 
				
				// Cannot be modified
				/*
				fieldValue = dataSetMD.getFileAccessId();
				mdForUpdate = createRecordForUpdate("FILE_ACCESS_ID", fieldValue,mdForUpdate); 	
				*/	
				
				UserInfo ownerInfo  = dataSetMD.getOwner();
				if(ownerInfo!=null)
				{ 
					fieldValue = ownerInfo.getUserID();
					mdForUpdate = createRecordForUpdate("OWNER", fieldValue,mdForUpdate);
				}	 
				
				if(mdForUpdate!=null)
				{
					metaDataList = new Vector();
					metaDataList.add(mdForUpdate);
				}
			}
		
			toLog("IngestMetaDataOperation() - extractDataSetMetaData() -END",MatrixUtil.DEBUG_LOG);
			return (metaDataList);
	}
	
	/**
	 * Create the metadata record used for ingest meta data
	 * @param fieldName Metadata Field Name
	 * @param mdValue Metadata Field Value
	 * @param mdForUpdate Metadata values for update
	 * @return Metadata values for update
	 */
	private MetaDataRecordList createRecordForUpdate(String fieldName, String mdValue,MetaDataRecordList mdForUpdate)
	{
		toLog("IngestMetaDataOperation() - createRecordForUpdate() -BEGIN",MatrixUtil.DEBUG_LOG);
		toLog("IngestMetaDataOperation() - createRecordForUpdate() -fieldName:"+fieldName,MatrixUtil.DEBUG_LOG);
		toLog("IngestMetaDataOperation() - createRecordForUpdate() -mdValue:"+mdValue,MatrixUtil.DEBUG_LOG);
		
		if(fieldName!=null && mdValue!=null)
		{
		
			if(mdForUpdate==null)
			{	
				mdForUpdate =  new SRBMetaDataRecordList( SRBMetaDataSet.getField(extractFieldName(fieldName) ), mdValue);
			}
			else
			{
				mdForUpdate.addRecord( SRBMetaDataSet.getField(extractFieldName(fieldName) ), mdValue);
			}
		}	
		
		
		toLog("IngestMetaDataOperation() - createRecordForUpdate() -END",MatrixUtil.DEBUG_LOG);
		return(mdForUpdate);
	}
	
	
	/**
 	 *	Retrieves the SRB Mapping for a Matrix Meta Data field
 	 *  Example : If input "field = SIZE" , the method would return the 
 	 *  		  field mapping SRBMetaDataSet.SIZE
 	 * @param field Meta Data Field Name
 	 * @return Jargon mapping for field name 
 	 */
	private String extractFieldName(String field)
	{
		toLog("IngestMetaDataOperation() - extractFieldName() -BEGIN",MatrixUtil.DEBUG_LOG);
		String name = null; 
		
		name = SRBMetaDataFieldMappings.mapMatrixVariableToSRBVariable(field);
		
		toLog("IngestMetaDataOperation() - extractFieldName() -field:"+field+",name:"+name,MatrixUtil.DEBUG_LOG);
		toLog("IngestMetaDataOperation() - extractFieldName() -END",MatrixUtil.DEBUG_LOG);
		return (name);
	}
	
	/** 
	 * Extract User Defined Meta Data 
	 * @param avuPairsList List of User Defined Meta Data
	 * @return List of User Defined Meta Data
	 */
	private MetaDataRecordList extractAVUPairsForUserDefinedMetaData(List avuPairsList)
	{
		toLog("IngestMetaDataOperation() - extractAVUPairsForUserDefinedMetaData() -BEGIN",MatrixUtil.DEBUG_LOG);
		MetaDataRecordList mtRecord = null; 
		Object[][] definableMetaDataValues = null;
		int[] operators = null;
		if(avuPairsList!=null)
		{
			if(avuPairsList.size()>0)
			{
				definableMetaDataValues = new Object[avuPairsList.size()][2];
				operators = new int[definableMetaDataValues.length];
										
				toLog("IngestMetaDataOperation() - extractAVUPairsForUserDefinedMetaData() -AVU Count:"+avuPairsList.size(),MatrixUtil.DEBUG_LOG);						
				
				for(int i=0;i<avuPairsList.size();i++)
				{
					AVUpair avuPair = (AVUpair) avuPairsList.get(i);
					if(avuPair!=null)
					{
						definableMetaDataValues[i][0] = avuPair.getAttribute();
						definableMetaDataValues[i][1] = avuPair.getValue();
						
						toLog("IngestMetaDataOperation() - extractAVUPairsForUserDefinedMetaData() -AVU Pair("+i+"):"+definableMetaDataValues[i][0]+","+definableMetaDataValues[i][1],MatrixUtil.DEBUG_LOG);	
						// Need to set for creating Metadata Table
						operators[i] = MetaDataCondition.EQUAL;
					}	
				}
				
				MetaDataTable metaDataTable = new MetaDataTable( operators, definableMetaDataValues );
				
				mtRecord = new SRBMetaDataRecordList( SRBMetaDataSet.getField(
						SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES ), metaDataTable );
			}			
		}
		
		
		toLog("IngestMetaDataOperation() - extractAVUPairsForUserDefinedMetaData() -END",MatrixUtil.DEBUG_LOG);
		return(mtRecord);
	}
	
	/**
	 * Displays the Query Results 
	 * Method details taken from jargon sample code
	 * @param rl Results
	 * @return Query Result in string format
	 * 
	 */
	public String printQueryResults( MetaDataRecordList[] rl )
	{
		toLog("IngestMetaDataOperation() - printQueryResults() -BEGIN",MatrixUtil.DEBUG_LOG);	
		toLog("IngestMetaDataOperation() - printQueryResults() ****************************",MatrixUtil.DEBUG_LOG);
		String resultString = new String("");
		//print query results
		if (rl != null) 
		{ //Nothing in the database matched the query
			toLog("IngestMetaDataOperation() printQueryResults() \n",MatrixUtil.DEBUG_LOG);
			resultString +="\n";
			for (int i=0;i<rl[0].getRecordCount();i++) 
			{
				toLog("IngestMetaDataOperation()- printQueryResults() -"+rl[0].getFieldName(i),MatrixUtil.DEBUG_LOG);
				resultString +=rl[0].getFieldName(i)+"\t";
			}

			for (int i=0;i<rl.length;i++) 
			{
				toLog("IngestMetaDataOperation() -printQueryResults() - \n",MatrixUtil.DEBUG_LOG);
				resultString +="\n";
				for (int j=0;j<rl[i].getRecordCount();j++) 
				{
					if (rl[i].getFieldType(j) == MetaDataField.TABLE) 
					{
						//tables aren't working
						MetaDataTable table = rl[i].getTableValue(j);
						toLog("IngestMetaDataOperation() -  printQueryResults() - table: ",MatrixUtil.DEBUG_LOG);
						resultString +="table: ";
						for (int k=0;k<table.getRowCount();k++) 
						{
							toLog("\n",MatrixUtil.DEBUG_LOG);
							resultString +="\n";
							for (int l=0;l<table.getColumnCount();l++) 
							{
								toLog("IngestMetaDataOperation() - printQueryResults() -"+table.getStringValue( k, l )+"\t",MatrixUtil.DEBUG_LOG);
								resultString +=table.getStringValue( k, l )+"\t";
							}
						}
					}
					else 
					{
						toLog("IngestMetaDataOperation() - printQueryResults() "+rl[i].getStringValue(j)+"\t",MatrixUtil.DEBUG_LOG);
						resultString +=rl[i].getStringValue(j)+"\t";
					}
				}
			}
		}
		else
		{
			resultString += " No Result matching the query";
		}
	
		toLog("IngestMetaDataOperation() - printQueryResults() ****************************",MatrixUtil.DEBUG_LOG);
		toLog("IngestMetaDataOperation() - printQueryResults() -END",MatrixUtil.DEBUG_LOG);	
		return(resultString);
	}

}