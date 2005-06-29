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

import java.util.List;

import edu.sdsc.matrix.srb.core.SRBConnection;
import edu.sdsc.matrix.srb.parser.CollectionList;
import edu.sdsc.matrix.srb.parser.StdCollectionList;
import edu.sdsc.matrix.srb.parser.DataSet;
import edu.sdsc.matrix.srb.parser.DataSetExtendedList;
import edu.sdsc.matrix.srb.parser.ListOp;
import edu.sdsc.matrix.srb.parser.ListParams;
import edu.sdsc.matrix.srb.parser.ListResult;
import edu.sdsc.matrix.srb.parser.OpResult;
import edu.sdsc.matrix.srb.parser.Step;
import edu.sdsc.matrix.srb.parser.StdDataSet;
import edu.sdsc.matrix.srb.parser.StdCollection;
import edu.sdsc.matrix.srb.util.MatrixUtil;
import edu.sdsc.matrix.srb.core.flow.FlowThread;
import edu.sdsc.matrix.srb.core.MatrixDatagridObject;
import edu.sdsc.matrix.srb.core.MatrixException;
import edu.sdsc.matrix.srb.parser.Collection;
import edu.sdsc.matrix.srb.parser.StdCollection;
import edu.sdsc.matrix.srb.parser.StdDatagridObject;

import edu.sdsc.grid.io.*;
import edu.sdsc.grid.io.srb.SRBContainer;
import edu.sdsc.grid.io.srb.SRBFile;
import edu.sdsc.grid.io.srb.SRBFileSystem;
import edu.sdsc.grid.io.srb.*;

import javax.xml.bind.JAXBException;

/**
	* This class is responsible for handling the SRB Operation of
	* listing a Container, Collection or Data Set
	*
	* @author rmathew
	*
	*/
public class ListOperation extends MatrixAgent{
			public ListOperation(Step step, FlowThread parent, ThreadGroup group){
						super(step, parent, group);
			}

			/**
				* The method that executes the steps for listing contents of a
				* Container, Collection or Data Set
				*/
			public void typeSpecificExecute() throws Exception{
						ListOp listOprn = getOperation().getListOp();
						ListParams listParams = listOprn.getStdParams();
						// Implementation for Operation
						try{
									StdDatagridObject gridObject = MatrixUtil.getObjectFactory().
											createStdDatagridObject();
									if (listParams.getCollection() != null)
												gridObject.setCollectionNow(listParams.getCollection());
									else if (listParams.getContainer() != null)
												gridObject.setContainerNow(listParams.getContainer());
									MatrixDatagridObject matrixGridObject = determineFilePath(gridObject);
									if (matrixGridObject.isContainer()){
												listContainer(matrixGridObject);
									} else if (matrixGridObject.isCollection()){
												listCollection(matrixGridObject, listParams.getRecursive(),
														listParams.getExtendedInfoLevel());
												toLog("one time log: recursive="+listParams.getRecursive(), MatrixUtil.DEBUG_LOG);
									} else if (matrixGridObject.isDataSet()){
												listDataSet(matrixGridObject);
									} else{
												throw new IllegalArgumentException(
														"No GridObject Details for deletion provided.");
									}
						} catch (JAXBException jbe){
									throw new RuntimeException("could not execute ListOperation", jbe);
						}
			}

			/**
				* Lists contents of a container
				* @param matrixGridObject Container Details
				*/
			private void listContainer(MatrixDatagridObject matrixGridObject) throws
					Exception{
						toLog("ListOperation() - listContainer() -BEGIN", MatrixUtil.DEBUG_LOG);
						String containerName = matrixGridObject.getGridPath();
						toLog("ListOperation() - listContainer() -Container NAme : " +
								containerName, MatrixUtil.DEBUG_LOG);
						String[] listContents = null;
						List collectionList = null;
						List dataSetList = null;
						if (containerName != null){
									//SRBAccount account = srbCreateSRBAccount();
									//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
									SRBConnection srbConnection = (SRBConnection)
											retrieveGridConnection();
									SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
									toLog("ListOperation() - listContainer() -USED FROM CONNECTION",
											MatrixUtil.DEBUG_LOG);
									SRBContainer srbContainer = new SRBContainer(srbFileSystem,
											containerName);
									listContents = srbContainer.list();
									toLog("ListOperation() - listContainer() -listContents:" +
											listContents, MatrixUtil.DEBUG_LOG);
									//Create Response from Lists
									ListResult listResult = getObjectFactory().createListResult();
									StdCollectionList collList = getObjectFactory().
											createStdCollectionList();
									collectionList = collList.getCollections();
									dataSetList = collList.getDataSets();
									String tempFileName = null;
									String logicalIdentifier = null;
									if (listContents != null){
												if (listContents.length > 0){
															//logicalIdentifier = listContents[0];
															toLog(
																	"ListOperation() - listContainer() -logicalIdentifier:" +
																	logicalIdentifier, MatrixUtil.DEBUG_LOG);
															for (int index = 0; index < listContents.length; index++){
																		tempFileName = listContents[index];
																		toLog(
																				"ListOperation() - listContainer() -tempFileName:" +
																				tempFileName, MatrixUtil.DEBUG_LOG);
																		toLog(
																				"ListOperation() - listContainer() -tempFileName.charAt(0):" +
																				tempFileName.charAt(0), MatrixUtil.DEBUG_LOG);
																		if (tempFileName.charAt(0) == '/'){
																					toLog(
																							"ListOperation() - listContainer() -isDirectory:true",
																							MatrixUtil.DEBUG_LOG);
																					// What to set ????
																					StdCollection tempCollection = getObjectFactory().
																							createStdCollection();
																					tempCollection.setCollectionName(tempFileName);
																					Collection collection = getObjectFactory().
																							createCollection();
																					collection.setStdCollection(tempCollection);
																					//collectionList.add(tempCollection);
																					collectionList.add(collection); // CollectionList->Collection->StdCollectionType
																		} else{ // if(tempGeneralFile.isFile())
																					toLog(
																							"ListOperation() - listContainer() -isFile:true",
																							MatrixUtil.DEBUG_LOG);
																					// What to set ????
																					StdDataSet tempDataSet = getObjectFactory().
																							createStdDataSet();
																					DataSet dataSet = getObjectFactory().createDataSet();
																					dataSet.setStdDataSet(tempDataSet);
																					tempDataSet.setDataIdentifier(tempFileName);
																					DataSetExtendedList dataSetExtList = getObjectFactory().
																							createDataSetExtendedList();
																					dataSetExtList.setDataSet(dataSet);
																					//tempDataSet.setLogicalLocation(logicalIdentifier);
																					dataSetList.add(dataSetExtList); // DataSetList->DataSetExtendedList->DataSet
																		}
															}
												}
												CollectionList collListAbstract = getObjectFactory().
														createCollectionList();
												collListAbstract.setStdCollectionList(collList);
												listResult.setListing(collListAbstract);
												OpResult opResult = getObjectFactory().createOpResult();
												opResult.setListResult(listResult);
												setResponse(opResult);
												toLog("ListOperation() - listContainer() -setSRBResponse",
														MatrixUtil.DEBUG_LOG);
									}
									srbFileSystem = null; // For Performance
						} else{
									throw new MatrixException("Cannot resolve variable");
						}
						toLog("ListOperation() - listContainer() -END", MatrixUtil.DEBUG_LOG);
			}

			/**
				* Lists contents of a Collection
				* @param matrixGridObject Collection Details
				*/
			private void listCollection(MatrixDatagridObject matrixGridObject,
					int recursiveLevel, int infoLevel) throws
					Exception{
						toLog("ListOperation() - listCollection() -BEGIN", MatrixUtil.DEBUG_LOG);
						String collectionName = matrixGridObject.getGridPath();
						toLog("ListOperation() - listCollection() -Collection NAme : " +
								collectionName, MatrixUtil.DEBUG_LOG);
						String[] listContents = null;
						List collectionList = null;
						List dataSetList = null;
						if (collectionName != null){
									SRBConnection srbConnection = (SRBConnection) retrieveGridConnection();
									SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
									toLog("ListOperation() - listCollection() -USED FROM CONNECTION",
											MatrixUtil.DEBUG_LOG);
									SRBFile srbCollection = new SRBFile(srbFileSystem, collectionName);
									// create result
									ListResult listResult = getObjectFactory().createListResult();
									StdCollectionList collList = getObjectFactory().
											createStdCollectionList();
									collectionList = collList.getCollections();
									dataSetList = collList.getDataSets();
									String tempFileName = null;
									//logicalIdentifier = srbCollection.getCanonicalPath();
									if (recursiveLevel == -1){ //we only support -1 (full recursive) or currentDir
												toLog("ListOperation() - listCollection() -recursive", MatrixUtil.DEBUG_LOG);
												MetaDataCondition conditions[] = {
														MetaDataSet.newCondition(
														SRBMetaDataSet.DIRECTORY_NAME,
														MetaDataCondition.LIKE,
														srbCollection.getAbsolutePath() + "%"),
												};
												MetaDataSelect selects[] = {MetaDataSet.newSelection(
														SRBMetaDataSet.FILE_NAME),
														MetaDataSet.newSelection(SRBMetaDataSet.DIRECTORY_NAME)
												};
												MetaDataRecordList[] rl = srbFileSystem.query(conditions,
														selects);
												rl = MetaDataRecordList.getAllResults(rl);
												toLog("ListOperation() - listCollection() -listContents-length : " +
														rl.length, MatrixUtil.DEBUG_LOG);
												// handle this rl carefully - it must be a big chunk in memory
												if ((rl != null) && (rl.length > 0)){ //are there results?
															toLog("ListOperation()-listCollection() - resultSets:" +
																	rl.length, MatrixUtil.DEBUG_LOG);
															// every thing goes under this collection
															StdCollection tempCollection = getObjectFactory().
																	createStdCollection();
															tempCollection.setCollectionName(srbCollection.getAbsolutePath());
															Collection collection = getObjectFactory().createCollection();
															collection.setStdCollection(tempCollection);
															collectionList.add(collection); // CollectionList->Collection->StdCollectionType
															for (int i = 0; i < rl.length; i++){
																		StdDataSet tempDataSet = getObjectFactory().
																				createStdDataSet();
																		tempDataSet.setDataIdentifier(rl[i].getValue(1) +
																				SRBFile.PATH_SEPARATOR + rl[i].getValue(0));
																				//one time test ...
																				//toLog("ONE_TIME:"+rl[i].getValue(1) +SRBFile.PATH_SEPARATOR + rl[i].getValue(0), MatrixUtil.DEBUG_LOG);
																		DataSet dataSet = getObjectFactory().createDataSet();
																		dataSet.setStdDataSet(tempDataSet);
																		DataSetExtendedList dataSetExtList = getObjectFactory().
																				createDataSetExtendedList();
																		dataSetExtList.setDataSet(dataSet);
																		//tempDataSet.setLogicalLocation(logicalIdentifier);
																		dataSetList.add(dataSetExtList); // DataSetList->DataSetExtendedList->DataSet
															}
												}
									} else{ //not recursive
												listContents = srbCollection.list();
												toLog("ListOperation() - listCollection() -listContents-length : " +
														listContents.length, MatrixUtil.DEBUG_LOG);
												if (listContents != null){
															if (listContents.length > 0){
																		toLog(
																				"ListOperation() - listCollection() -logicalIdentifier:" +
																				srbCollection.getCanonicalPath(), MatrixUtil.DEBUG_LOG);
																		for (int index = 0; index < listContents.length; index++){
																					tempFileName = listContents[index];
																					toLog(
																							"ListOperation() - listCollection() -tempFileName:" +
																							tempFileName, MatrixUtil.DEBUG_LOG);
																					if (tempFileName.charAt(0) == '/'){
																								toLog(
																										"ListOperation() - listCollection() -isDirectory:true",
																										MatrixUtil.DEBUG_LOG);
																								StdCollection tempCollection = getObjectFactory().
																										createStdCollection();
																								tempCollection.setCollectionName(tempFileName);
																								Collection collection = getObjectFactory().
																										createCollection();
																								collection.setStdCollection(tempCollection);
																								//collectionList.add(tempCollection);
																								collectionList.add(collection); // CollectionList->Collection->StdCollectionType
																					} else{
																								toLog(
																										"ListOperation() - listContainer() -isFile:true",
																										MatrixUtil.DEBUG_LOG);
																								// What to set ????
																								StdDataSet tempDataSet = getObjectFactory().
																										createStdDataSet();
																								tempDataSet.setDataIdentifier(tempFileName);
																								DataSet dataSet = getObjectFactory().createDataSet();
																								dataSet.setStdDataSet(tempDataSet);
																								DataSetExtendedList dataSetExtList = getObjectFactory().
																										createDataSetExtendedList();
																								dataSetExtList.setDataSet(dataSet);
																								//tempDataSet.setLogicalLocation(logicalIdentifier);
																								dataSetList.add(dataSetExtList); // DataSetList->DataSetExtendedList->DataSet
																					}
																		}
															}
												}
									}
									CollectionList collListAbstract = getObjectFactory().
											createCollectionList();
									collListAbstract.setStdCollectionList(collList);
									listResult.setListing(collListAbstract);
									OpResult opResult = getObjectFactory().createOpResult();
									opResult.setListResult(listResult);
									setResponse(opResult);
									toLog("ListOperation() - listCollection() -setSRBResponse",
											MatrixUtil.DEBUG_LOG);
									srbFileSystem = null; // For Performance
									toLog("ListOperation() - listCollection() -listContents:" +
											listContents, MatrixUtil.DEBUG_LOG);
						} else{
									throw new MatrixException("Cannot resolve collection name");
						}
						toLog("ListOperation() - listCollection() -END", MatrixUtil.DEBUG_LOG);
			}

			/**
				* Lists contents of a data set
				* @param matrixGridObject Data Set Details
				*/
			private void listDataSet(MatrixDatagridObject matrixGridObject) throws
					Exception{
						toLog("ListOperation() - listDataSet() -BEGIN", MatrixUtil.DEBUG_LOG);
						String collectionName = matrixGridObject.getGridPath();
						toLog("ListOperation() - listDataSet() -Collection NAme : " +
								collectionName, MatrixUtil.DEBUG_LOG);
						GeneralFile[] listFileContents = null;
						String[] listContents = null;
						List collectionList = null;
						List dataSetList = null;
						if (collectionName != null){
									SRBConnection srbConnection = (SRBConnection)
											retrieveGridConnection();
									SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
									toLog("ListOperation() - listDataSet() -USED FROM CONNECTION",
											MatrixUtil.DEBUG_LOG);
									SRBFile srbCollection = new SRBFile(srbFileSystem, collectionName);
									listFileContents = srbCollection.listFiles();
									listContents = srbCollection.list();
									toLog("ListOperation() - listDataSet() -listFileContents-length : " +
											listFileContents.length, MatrixUtil.DEBUG_LOG);
									toLog("ListOperation() - listDataSet() -listContents-length : " +
											listContents.length, MatrixUtil.DEBUG_LOG);
									String logicalIdentifier = null;
									//	Create Response from Lists
									ListResult listResult = getObjectFactory().createListResult();
									StdCollectionList collList = getObjectFactory().
											createStdCollectionList();
									collectionList = collList.getCollections();
									dataSetList = collList.getDataSets();
									String tempFileName = null;
									if (listContents != null){
												if (listContents.length > 0){
															logicalIdentifier = listContents[0];
															toLog("ListOperation() - listDataSet() -logicalIdentifier:" +
																	logicalIdentifier, MatrixUtil.DEBUG_LOG);
															for (int index = 1; index < listContents.length; index++){
																		tempFileName = listContents[index];
																		toLog("ListOperation() - listDataSet() -tempFileName:" +
																				tempFileName, MatrixUtil.DEBUG_LOG);
																		toLog(
																				"ListOperation() - listDataSet() -tempFileName.charAt(0):" +
																				tempFileName.charAt(0), MatrixUtil.DEBUG_LOG);
																		if (tempFileName.charAt(0) == '/'){
																					toLog(
																							"ListOperation() - listDataSet() -isDirectory:true",
																							MatrixUtil.DEBUG_LOG);
																					// What to set ????
																					StdCollection tempCollection = getObjectFactory().createStdCollection();
																					tempCollection.setCollectionName(tempFileName);
																					Collection collection = getObjectFactory().
																							createCollection();
																					collection.setStdCollection(tempCollection);
																					//collectionList.add(tempCollection);
																					collectionList.add(collection); // CollectionList->Collection->StdCollectionType
																		} else{ // if(tempGeneralFile.isFile())
																					toLog(
																							"ListOperation() - listContainer() -isFile:true",
																							MatrixUtil.DEBUG_LOG);
																					// What to set ????
																					StdDataSet dataSetConcrete = getObjectFactory().
																							createStdDataSet();
																					DataSet dataSetAbstract = getObjectFactory().createDataSet();
																					dataSetAbstract.setStdDataSet(dataSetConcrete);
																					dataSetConcrete.setDataIdentifier(tempFileName);
																					DataSetExtendedList dataSetExtList =
																							getObjectFactory().createDataSetExtendedList();
																					dataSetExtList.setDataSet(dataSetAbstract);
																					//dataSetConcrete.setLogicalLocation(logicalIdentifier);
																					dataSetList.add(dataSetExtList); // DataSetList->DataSetExtendedList->DataSet
																		}
															}
												}
												CollectionList collListAbstract = getObjectFactory().createCollectionList();
												collListAbstract.setStdCollectionList(collList);
												listResult.setListing(collListAbstract);
												OpResult opResult = getObjectFactory().createOpResult();
												opResult.setListResult(listResult);
												setResponse(opResult);
												toLog("ListOperation() - listDataSet() -setSRBResponse",
														MatrixUtil.DEBUG_LOG);
									}
									srbFileSystem = null; // For Performance
									toLog("ListOperation() - listDataSet() -listContents:" +
											listContents, MatrixUtil.DEBUG_LOG);
						} else{
									throw new MatrixException("Cannot resolve variable");
						}
						toLog("ListOperation() - listDataSet() -END", MatrixUtil.DEBUG_LOG);
			}

			/**
				* Get Operation Name
				* @returns the name of the operation encapsulated by this agent
				*/
			public String getOperationName(){
						return "ListOperation";
			}
}