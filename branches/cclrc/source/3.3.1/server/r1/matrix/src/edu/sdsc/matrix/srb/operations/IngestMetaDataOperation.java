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

import java.util.Calendar;
import java.util.Date;
import java.util.List;
import java.util.Vector;

import edu.sdsc.matrix.srb.core.MatrixDatagridObject;
import edu.sdsc.matrix.srb.core.MatrixException;
import edu.sdsc.matrix.srb.core.SRBConnection;
import edu.sdsc.matrix.srb.core.flow.FlowThread;
import edu.sdsc.matrix.srb.parser.AVUpair;
import edu.sdsc.matrix.srb.parser.CollectionSystemMetaData;
import edu.sdsc.matrix.srb.parser.DataLock;
import edu.sdsc.matrix.srb.parser.DatasetSystemMetaData;
import edu.sdsc.matrix.srb.parser.IngestMetadataOp;
import edu.sdsc.matrix.srb.parser.IngestMetadataParams;
import edu.sdsc.matrix.srb.parser.MetaData;
import edu.sdsc.matrix.srb.parser.OrganizationInfo;
import edu.sdsc.matrix.srb.parser.ResourceSystemMetaData;
import edu.sdsc.matrix.srb.parser.Step;
import edu.sdsc.matrix.srb.parser.SystemMetaData;
import edu.sdsc.matrix.srb.parser.UserInfo;
import edu.sdsc.matrix.srb.parser.UserSystemMetaData;
import edu.sdsc.matrix.srb.util.MatrixUtil;
import edu.sdsc.matrix.srb.util.SRBMetaDataFieldMappings;

import edu.sdsc.grid.io.FileFactory;
import edu.sdsc.grid.io.GeneralFile;
import edu.sdsc.grid.io.GeneralFileSystem;
import edu.sdsc.grid.io.MetaDataCondition;
import edu.sdsc.grid.io.MetaDataField;
import edu.sdsc.grid.io.MetaDataRecordList;
import edu.sdsc.grid.io.MetaDataTable;
import edu.sdsc.grid.io.srb.SRBAccount;
import edu.sdsc.grid.io.srb.SRBContainer;
import edu.sdsc.grid.io.srb.SRBFileSystem;
import edu.sdsc.grid.io.srb.SRBMetaDataRecordList;
import edu.sdsc.grid.io.srb.SRBMetaDataSet;

/**
	*
	* @author rmathew
	*
	* This class is used for ingesting metadata for
	* 		- Collection
	* 		- Data Set
	* 		- Container
	*/

/* Developer log
	*
	* Original code - Reena Mathew
	* removed unnecessary toLog statements, formatting, this developer log - 1/31/05 Arun
	* Removed a bug or unnecessary creation of SRBFileSystem instead of using connection pool
					- 1/31/05 Arun
	*
	*/

public class IngestMetaDataOperation extends MatrixAgent{
			public IngestMetaDataOperation(Step step, FlowThread parent,
					ThreadGroup group){
						super(step, parent, group);
			}

			/**
				*    The method that executes the steps for ingesting meta data for
				*    a Container,Collection or Data Set
				*/
			public void typeSpecificExecute() throws Exception{
						IngestMetadataOp ingestOprn = getOperation().getIngestMetadataOp();
						IngestMetadataParams ingestParams = ingestOprn.getStdParams();
						MetaData ingestMetaData = ingestParams.getMetadata();
						// Implementation for Operation
						MatrixDatagridObject matrixGridObject = determineFilePath(ingestParams.
								getDataGridObject());
						if (matrixGridObject.isContainer()){
									ingestMetadataForContainer(matrixGridObject, ingestMetaData);
						} else if (matrixGridObject.isCollection()){
									ingestMetadataForFile(matrixGridObject, ingestMetaData);
						} else if (matrixGridObject.isDataSet()){
									MatrixUtil.toLog("ingestMetaData for file", MatrixUtil.DEBUG_LOG);
									ingestMetadataForFile(matrixGridObject, ingestMetaData);
									MatrixUtil.toLog("done ingestMetaData for file", MatrixUtil.DEBUG_LOG);
						} else{
									throw new IllegalArgumentException(
											"No Grid Details for ingest provided.");
						}
			}

			/**
				* Inserts Metadata for a container
				* @param matrixGridObject Container Details
				* @param containerMetaData Meta data about Container
				*/
			private void ingestMetadataForContainer(MatrixDatagridObject
					matrixGridObject, MetaData containerMetaData) throws Exception{
						toLog("IngestMetaDataOperation() - ingestMetadataForContainer() -BEGIN",
								MatrixUtil.DEBUG_LOG);
						String containerName = matrixGridObject.getGridPath();
						toLog(
								"IngestMetaDataOperation() - ingestMetadataForContainer() -Container NAme : " +
								containerName, MatrixUtil.DEBUG_LOG);
						if (containerName != null){
									SRBConnection srbConnection = (SRBConnection) retrieveGridConnection();
									SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
									toLog(
											"IngestMetaDataOperation() - createContainer() -USED FROM CONNECTION",
											MatrixUtil.DEBUG_LOG);
									SRBContainer srbContainer = new SRBContainer(srbFileSystem,
											containerName);
									srbContainer.createNewFile();
									srbFileSystem = null; // For Performance
						} else{
									throw new MatrixException("Cannot resolve variable");
						}
						toLog("IngestMetaDataOperation() - ingestMetadataForContainer() -END",
								MatrixUtil.DEBUG_LOG);
			}

			/**
				* Inserts metadata for a collection or dataset
				* @param matrixGridObject Data Set or Collection Details
				* @param dataSetMetaData Meta Data to be ingested
				*/
			private void ingestMetadataForFile(MatrixDatagridObject matrixGridObject,
					MetaData dataSetMetaData) throws Exception{
						toLog("IngestMetaDataOperation() - ingestMetadataForFile() -BEGIN",
								MatrixUtil.DEBUG_LOG);
						// get the name of the dataSet to write metadata about
						String dataSetName = matrixGridObject.getGridPath();
						toLog("IngestMetaDataOperation() - ingestMetadataForFile() -dataSetName:" +
								dataSetName, MatrixUtil.DEBUG_LOG);
						if (dataSetName != null){
									//TODO use the connection and not a new fileSystem + check rest of code
									SRBConnection srbConnection = (SRBConnection) retrieveGridConnection();
									//SRBAccount srbAccount = srbConnection.getSRBAccount();
									GeneralFileSystem fileSystem = srbConnection.getActiveConnection();
									//FileFactory.newFileSystem(srbAccount); //new SRBFileSystem(srbAccount); //
									toLog(
											"IngestMetaDataOperation() - ingestMetadataForFile() -USED FROM CONNECTION",
											MatrixUtil.DEBUG_LOG);
									GeneralFile file = FileFactory.newFile(fileSystem, dataSetName);
									MetaDataRecordList[] mdListForUpdate = extractMetadataValues(
											dataSetMetaData, matrixGridObject.isDataSet());
									if (mdListForUpdate != null){
												toLog(
														"IngestMetaDataOperation() - ingestMetadataForFile() -Number of Update Calls" +
														mdListForUpdate.length, MatrixUtil.DEBUG_LOG);
												toLog(
														"IngestMetaDataOperation() - ingestMetadataForFile() -File Path:" +
														file.getPath(), MatrixUtil.DEBUG_LOG);
												for (int listCount = 0; listCount < mdListForUpdate.length;
														listCount++){
															toLog(
																	"IngestMetaDataOperation() - ingestMetadataForFile() -MODIFY BEGIN for " +
																	mdListForUpdate[listCount].getFieldName(0),
																	MatrixUtil.DEBUG_LOG);
															file.modifyMetaData(mdListForUpdate[listCount]);
															toLog(
																	"IngestMetaDataOperation() - ingestMetadataForFile() -MODIFY COMPLETE",
																	MatrixUtil.DEBUG_LOG);
												}
									}
						} else{
									throw new MatrixException("Cannot resolve variable");
						}
						toLog("IngestMetaDataOperation() - ingestMetadataForFile() -END",
								MatrixUtil.DEBUG_LOG);
			}

			/**
				* Extract the metadata values for insert/update
				* @param metaDataForUpdate metadata to be updated
				* @return List of meta data
				*/
			private MetaDataRecordList[] extractMetadataValues(MetaData
					metaDataForUpdate, boolean isDataSet){
						// some variables that will be used
						toLog("IngestMetaDataOperation() - extractMetadataValues() -BEGIN",
								MatrixUtil.DEBUG_LOG);
						MetaDataRecordList metaDataList[] = null; //new MetaDataRecordList[1];
						MetaDataRecordList tempRecord = null;
						Vector finalList = new Vector(); //null;
						Vector tempList = null;
						SystemMetaData systemMD = metaDataForUpdate.getSystemMeta();
						// if there is any system metadata, get it and add it to the final list
						if (systemMD != null){
									// Extract the different type of metadata parameters
									CollectionSystemMetaData collectionMD = systemMD.getCollectionSysMeta();
									tempList = extractCollectionMetaData(collectionMD);
									if (tempList != null)
												finalList.addAll(tempList);
									UserSystemMetaData userMD = systemMD.getUserSysMeta();
									tempList = extractUserMetaData(userMD);
									if (tempList != null)
												finalList.addAll(tempList);
									ResourceSystemMetaData resourceMD = systemMD.getResourceSysMeta();
									tempList = extractResourceMetaData(resourceMD);
									if (tempList != null)
												finalList.addAll(tempList);
									DatasetSystemMetaData dataSetMD = systemMD.getDataSetSysMeta();
									tempList = extractDataSetMetaData(dataSetMD);
									if (tempList != null)
												finalList.addAll(tempList);
						}
						// User Defined Meta Data from AVU-Pairs
						tempRecord = extractAVUPairsForUserDefinedMetaData(metaDataForUpdate.
								getAVUPairs(), isDataSet);
						if (tempRecord != null){
									finalList.add(tempRecord);
									// Convert the list of metadata values to an array
						}
						if (finalList != null && finalList.size() > 0){
									metaDataList = new MetaDataRecordList[finalList.size()];
									finalList.copyInto(metaDataList);
						}
						toLog("IngestMetaDataOperation() - extractMetadataValues() -END",
								MatrixUtil.DEBUG_LOG);
						return (metaDataList);
			}

			/**
				* Extracts Collection related Meta Data
				* @param collectionMD Meta Data for Collection
				* @return List of meta data
				*/
			private Vector extractCollectionMetaData(CollectionSystemMetaData
					collectionMD){
						toLog("IngestMetaDataOperation() - extractCollectionMetaData() -BEGIN",
								MatrixUtil.DEBUG_LOG);
						Vector metaDataList = null;
						if (collectionMD != null){
									// Test Code
									MetaDataRecordList[] rl = new MetaDataRecordList[1];
									rl[0] = new SRBMetaDataRecordList(SRBMetaDataSet.getField(
											SRBMetaDataSet.FILE_COMMENTS),
											"New Comment :" +
											getProgenitor().getMatrixThreadId());
									//rl[0].addRecord( SRBMetaDataSet.getField(
									//					SRBMetaDataSet.FILE_COMMENTS ), "new comments go here." );
									metaDataList = new Vector();
									metaDataList.add(rl[0]);
									toLog(
											"IngestMetaDataOperation() - extractCollectionMetaData() -TEST MD ADDED",
											MatrixUtil.DEBUG_LOG);
									toLog(
											"IngestMetaDataOperation() - extractCollectionMetaData() -END",
											MatrixUtil.DEBUG_LOG);
						}
						return (metaDataList);
			}

			/**
				* Extracts User related Meta Data
				* @param userMD User Related Meta Data
				* @return List of user meta data
				*/
			private Vector extractUserMetaData(UserSystemMetaData userMD){
						toLog("IngestMetaDataOperation() - extractUserMetaData() -BEGIN",
								MatrixUtil.DEBUG_LOG);
						Vector metaDataList = null;
						if (userMD != null){
						}
						// TEST CODE
//		Test Code for Meta Data Table
						String[][] definableMetaDataValues = new String[3][2];
						definableMetaDataValues[0][0] = "language";
						definableMetaDataValues[0][1] = "French";
						definableMetaDataValues[1][0] = "versionControl";
						definableMetaDataValues[1][1] = "1.2.3";
						definableMetaDataValues[2][0] = "versionSize";
						definableMetaDataValues[2][1] = "789";
						int[] operators = new int[definableMetaDataValues.length];
						//only equal and not equal work for strings.
						operators[0] = MetaDataCondition.EQUAL;
						operators[1] = MetaDataCondition.EQUAL;
						operators[2] = MetaDataCondition.EQUAL;
						MetaDataTable metaDataTable = new MetaDataTable(operators,
								definableMetaDataValues);
						MetaDataRecordList[] rl = new MetaDataRecordList[1];
						rl[0] = new SRBMetaDataRecordList(SRBMetaDataSet.getField(
								SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES), metaDataTable);
						metaDataList = new Vector();
						metaDataList.add(rl[0]);
						toLog("IngestMetaDataOperation() - extractCollectionMetaData() -TEST METADATA TABLE ADDED",
								MatrixUtil.DEBUG_LOG);
						toLog("IngestMetaDataOperation() - extractUserMetaData() -END",
								MatrixUtil.DEBUG_LOG);
						return (metaDataList);
			}

			/**
				* Extracts Resource related Meta Data
				* @param resourceMD Meta data for resources
				* @return List of meta data
				*/
			private Vector extractResourceMetaData(ResourceSystemMetaData resourceMD){
						toLog("IngestMetaDataOperation() - extractResourceMetaData() -BEGIN",
								MatrixUtil.DEBUG_LOG);
						Vector metaDataList = null;
						if (resourceMD != null){
						}
						toLog("IngestMetaDataOperation() - extractResourceMetaData() -END",
								MatrixUtil.DEBUG_LOG);
						return (metaDataList);
			}

			/**
				* Extracts Data Set related Meta Data
				* @param dataSetMD Meta Data for Data Set
				* @return List of meta data
				*/
			private Vector extractDataSetMetaData(DatasetSystemMetaData dataSetMD){
						toLog("IngestMetaDataOperation() - extractDataSetMetaData() -BEGIN",
								MatrixUtil.DEBUG_LOG);
						Vector metaDataList = null;
						String fieldName = null;
						String fieldValue = null;
						MetaDataRecordList mdForUpdate = null;
						Date tempDate = null;
						Calendar tempCalendar = null;
						if (dataSetMD != null){
									fieldValue = dataSetMD.getIsDirty();
									if (fieldValue != null){
												mdForUpdate = createRecordForUpdate("IS_DIRTY", fieldValue,
														mdForUpdate);
									}
									fieldValue = dataSetMD.getFileIsCompressed();
									if (fieldValue != null){
												mdForUpdate = createRecordForUpdate("FILE_IS_COMPRESSED",
														fieldValue, mdForUpdate);
									}
									if (dataSetMD.getFileHide() != 0){
												fieldValue = String.valueOf(dataSetMD.getFileHide());
												if (fieldValue != null){
															mdForUpdate = createRecordForUpdate("FILE_HIDE", fieldValue,
																	mdForUpdate);
												}
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
									if (tempCalendar != null){
												tempDate = tempCalendar.getTime();
												fieldValue = MatrixUtil.formatDate("yyyy-MM-dd-HH.mm.ss",
														tempDate);
												mdForUpdate = createRecordForUpdate("FILE_EXPIRY_DATE",
														fieldValue, mdForUpdate);
									}
									fieldValue = dataSetMD.getFileExpireDate2();
									if (fieldValue != null){
												mdForUpdate = createRecordForUpdate("FILE_EXPIRE_DATE_2",
														fieldValue, mdForUpdate);
									}
									tempCalendar = dataSetMD.getModificationDate();
									if (tempCalendar != null){
												tempDate = tempCalendar.getTime();
												fieldValue = MatrixUtil.formatDate("yyyy-MM-dd-HH.mm.ss",
														tempDate);
												mdForUpdate = createRecordForUpdate("MODIFICATION_DATE",
														fieldValue, mdForUpdate);
									}
									tempCalendar = dataSetMD.getFilePinExpiry();
									if (tempCalendar != null){
												tempDate = tempCalendar.getTime();
												fieldValue = MatrixUtil.formatDate("yyyy-MM-dd-HH.mm.ss",
														tempDate);
												mdForUpdate = createRecordForUpdate("FILE_PIN_EXPIRY",
														fieldValue, mdForUpdate);
									}
									fieldValue = dataSetMD.getFileAuditFlag();
									if (fieldValue != null){
												mdForUpdate = createRecordForUpdate("FILE_AUDITFLAG",
														fieldValue, mdForUpdate);
												// Cannot be modfied : NOT IMPLEMENTED
									}
									OrganizationInfo pinOrgInfo = dataSetMD.getFilePinOwnerDomain();
									if (pinOrgInfo != null){
									}
									if (dataSetMD.getFileReplicationEnum() != 0){
												fieldValue = String.valueOf(dataSetMD.getFileReplicationEnum());
												if (fieldValue != null){
															mdForUpdate = createRecordForUpdate("FILE_REPLICATION_ENUM",
																	fieldValue, mdForUpdate);
												}
									}
									if (dataSetMD.getSize() != 0){
												fieldValue = String.valueOf(dataSetMD.getSize());
												if (fieldValue != null){
															mdForUpdate = createRecordForUpdate("SIZE", fieldValue,
																	mdForUpdate);
												}
									}
									DataLock fileLock = dataSetMD.getFileLock();
									if (fileLock != null){
												// Only Field Modifiable
												fieldValue = String.valueOf(fileLock.getLockNum());
												mdForUpdate = createRecordForUpdate("FILE_LOCK_NUM", fieldValue,
														mdForUpdate);
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
									if (dataSetMD.getOffset() != 0){
												fieldValue = String.valueOf(dataSetMD.getOffset());
												if (fieldValue != null){
															mdForUpdate = createRecordForUpdate("OFFSET", fieldValue,
																	mdForUpdate);
												}
									}
									// Cannot be modified
									/*
												fieldValue = dataSetMD.getFileAccessId();
												mdForUpdate = createRecordForUpdate("FILE_ACCESS_ID", fieldValue,mdForUpdate);
										*/
									UserInfo ownerInfo = dataSetMD.getOwner();
									if (ownerInfo != null){
												fieldValue = ownerInfo.getUserID();
												mdForUpdate = createRecordForUpdate("OWNER", fieldValue,
														mdForUpdate);
									}
									if (mdForUpdate != null){
												metaDataList = new Vector();
												metaDataList.add(mdForUpdate);
									}
						}
						toLog("IngestMetaDataOperation() - extractDataSetMetaData() -END",
								MatrixUtil.DEBUG_LOG);
						return (metaDataList);
			}

			/**
				* Create the metadata record used for ingest meta data
				* @param fieldName Metadata Field Name
				* @param mdValue Metadata Field Value
				* @param mdForUpdate Metadata values for update
				* @return Metadata values for update
				*/
			private MetaDataRecordList createRecordForUpdate(String fieldName,
					String mdValue,
					MetaDataRecordList
					mdForUpdate){
						toLog("IngestMetaDataOperation() - createRecordForUpdate() -BEGIN",
								MatrixUtil.DEBUG_LOG);
						toLog("IngestMetaDataOperation() - createRecordForUpdate() -fieldName:" +
								fieldName, MatrixUtil.DEBUG_LOG);
						toLog("IngestMetaDataOperation() - createRecordForUpdate() -mdValue:" +
								mdValue, MatrixUtil.DEBUG_LOG);
						if (fieldName != null && mdValue != null){
									if (mdForUpdate == null){
												mdForUpdate = new SRBMetaDataRecordList(SRBMetaDataSet.getField(
														extractFieldName(fieldName)), mdValue);
									} else{
												mdForUpdate.addRecord(SRBMetaDataSet.getField(extractFieldName(
														fieldName)), mdValue);
									}
						}
						toLog("IngestMetaDataOperation() - createRecordForUpdate() -END",
								MatrixUtil.DEBUG_LOG);
						return (mdForUpdate);
			}

			/**
				*  Retrieves the SRB Mapping for a Matrix Meta Data field
				*  Example : If input "field = SIZE" , the method would return the
				*        field mapping SRBMetaDataSet.SIZE
				* @param field Meta Data Field Name
				* @return Jargon mapping for field name
				*/
			private String extractFieldName(String field){
						toLog("IngestMetaDataOperation() - extractFieldName() -BEGIN",
								MatrixUtil.DEBUG_LOG);
						String name = null;
						name = SRBMetaDataFieldMappings.mapMatrixVariableToSRBVariable(field);
						toLog("IngestMetaDataOperation() - extractFieldName() -field:" + field +
								",name:" + name, MatrixUtil.DEBUG_LOG);
						toLog("IngestMetaDataOperation() - extractFieldName() -END",
								MatrixUtil.DEBUG_LOG);
						return (name);
			}

			/**
				* Extract User Defined Meta Data
				* @param avuPairsList List of User Defined Meta Data
				* @return List of User Defined Meta Data
				*/
			private MetaDataRecordList extractAVUPairsForUserDefinedMetaData(List
					avuPairsList, boolean isDataSet){
						toLog(
								"IngestMetaDataOperation() - extractAVUPairsForUserDefinedMetaData() -BEGIN",
								MatrixUtil.DEBUG_LOG);
						MetaDataRecordList mtRecord = null;
						String[][] definableMetaDataValues = null;
						int[] operators = null;
						if (avuPairsList != null && avuPairsList.size() > 0){
									definableMetaDataValues = new String[avuPairsList.size()][2];
									operators = new int[definableMetaDataValues.length];
									toLog("IngestMetaDataOperation() - extractAVUPairsForUserDefinedMetaData() -AVU Count:" +
											avuPairsList.size(), MatrixUtil.DEBUG_LOG);
									for (int i = 0; i < avuPairsList.size(); i++){
												AVUpair avuPair = (AVUpair) avuPairsList.get(i);
												if (avuPair != null){
															definableMetaDataValues[i][0] = this.evaluateExpressionToString(
																	avuPair.getAttribute().toString());
															definableMetaDataValues[i][1] = this.evaluateExpressionToString(
																	avuPair.getValue().toString());
															toLog(
																	"IngestMetaDataOperation() - extractAVUPairsForUserDefinedMetaData() -AVU Pair(" +
																	i + "):" + definableMetaDataValues[i][0] +
																	"," + definableMetaDataValues[i][1], MatrixUtil.DEBUG_LOG);
															// Need to set for creating Metadata Table
															operators[i] = MetaDataCondition.EQUAL;
												}
									}
									MetaDataTable metaDataTable = new MetaDataTable(operators,
											definableMetaDataValues);
									// determines if this should
									String fileType = isDataSet ?
											SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES :
											SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES;
									mtRecord =
											new SRBMetaDataRecordList(SRBMetaDataSet.getField(fileType),
											metaDataTable);
						}
						toLog(
								"IngestMetaDataOperation() - extractAVUPairsForUserDefinedMetaData() -END",
								MatrixUtil.DEBUG_LOG);
						return (mtRecord);
			}

			/**
				* Displays the Query Results
				* Method details taken from jargon sample code
				* @param rl Results
				* @return Query Result in string format
				*
				*/
			public String printQueryResults(MetaDataRecordList[] rl){
						toLog("IngestMetaDataOperation() - printQueryResults() -BEGIN",
								MatrixUtil.DEBUG_LOG);
						toLog(
								"IngestMetaDataOperation() - printQueryResults() ****************************",
								MatrixUtil.DEBUG_LOG);
						String resultString = new String("");
						//print query results
						if (rl != null){ //Nothing in the database matched the query
									toLog("IngestMetaDataOperation() printQueryResults() \n",
											MatrixUtil.DEBUG_LOG);
									resultString += "\n";
									for (int i = 0; i < rl[0].getRecordCount(); i++){
												toLog("IngestMetaDataOperation()- printQueryResults() -" +
														rl[0].getFieldName(i), MatrixUtil.DEBUG_LOG);
												resultString += rl[0].getFieldName(i) + "\t";
									}
									for (int i = 0; i < rl.length; i++){
												toLog("IngestMetaDataOperation() -printQueryResults() - \n",
														MatrixUtil.DEBUG_LOG);
												resultString += "\n";
												for (int j = 0; j < rl[i].getRecordCount(); j++){
															if (rl[i].getFieldType(j) == MetaDataField.TABLE){
																		//tables aren't working
																		MetaDataTable table = rl[i].getTableValue(j);
																		toLog(
																				"IngestMetaDataOperation() -  printQueryResults() - table: ",
																				MatrixUtil.DEBUG_LOG);
																		resultString += "table: ";
																		for (int k = 0; k < table.getRowCount(); k++){
																					toLog("\n", MatrixUtil.DEBUG_LOG);
																					resultString += "\n";
																					for (int l = 0; l < table.getColumnCount(); l++){
																								toLog(
																										"IngestMetaDataOperation() - printQueryResults() -" +
																										table.getStringValue(k, l) + "\t",
																										MatrixUtil.DEBUG_LOG);
																								resultString += table.getStringValue(k, l) +
																										"\t";
																					}
																		}
															} else{
																		toLog(
																				"IngestMetaDataOperation() - printQueryResults() " +
																				rl[i].getStringValue(j) + "\t",
																				MatrixUtil.DEBUG_LOG);
																		resultString += rl[i].getStringValue(j) + "\t";
															}
												}
									}
						} else{
									resultString += " No Result matching the query";
						}
						toLog(
								"IngestMetaDataOperation() - printQueryResults() ****************************",
								MatrixUtil.DEBUG_LOG);
						toLog("IngestMetaDataOperation() - printQueryResults() -END",
								MatrixUtil.DEBUG_LOG);
						return (resultString);
			}

			/**
				* Get Operation Name
				* @returns the name of the operation encapsulated by this agent
				*/
			public String getOperationName(){
						return "IngestMetaDataOperation";
			}
}