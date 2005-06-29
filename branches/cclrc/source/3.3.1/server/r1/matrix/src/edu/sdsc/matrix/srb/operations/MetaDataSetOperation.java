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

import edu.sdsc.matrix.srb.core.SRBConnection;
import edu.sdsc.matrix.srb.core.flow.FlowThread;
import edu.sdsc.matrix.srb.parser.AnyType;
import edu.sdsc.matrix.srb.parser.MetadataQueryInfo;
import edu.sdsc.matrix.srb.parser.MetadataQueryOp;
import edu.sdsc.matrix.srb.parser.MetadataQueryParams;
import edu.sdsc.matrix.srb.parser.MetadataQueryResult;
import edu.sdsc.matrix.srb.parser.OpResult;
import edu.sdsc.matrix.srb.parser.Step;
import edu.sdsc.matrix.srb.query.ASTStart;
import edu.sdsc.matrix.srb.query.MatrixQueryParser;
import edu.sdsc.matrix.srb.query.MatrixQueryParserDumpVisitor;
import edu.sdsc.matrix.srb.query.MatrixQueryParserPool;
import edu.sdsc.matrix.srb.query.MatrixQueryParserVisitor;
import edu.sdsc.matrix.srb.query.SRBQueryWrapper;
import edu.sdsc.matrix.srb.util.MatrixUtil;

import edu.sdsc.grid.io.FileFactory;
import edu.sdsc.grid.io.GeneralFile;
import edu.sdsc.grid.io.GeneralFileSystem;
import edu.sdsc.grid.io.MetaDataCondition;
import edu.sdsc.grid.io.MetaDataField;
import edu.sdsc.grid.io.MetaDataRecordList;
import edu.sdsc.grid.io.MetaDataSelect;
import edu.sdsc.grid.io.MetaDataSet;
import edu.sdsc.grid.io.MetaDataTable;
import edu.sdsc.grid.io.srb.SRBAccount;
import edu.sdsc.grid.io.srb.SRBFileSystem;
import edu.sdsc.grid.io.srb.SRBMetaDataSet;

/**
	* This class is responsible for handling the SRB Operation of
	* querying meta data about  a Container, Collection or Data Set
	*
	* @author rmathew
	*
	*/

/* Developer Log
	* Original Code - Reena Mathew
	* Removed a bug or unnecessary creation of SRBFileSystem instead of using connection pool
				- 1/31/05 Arun
	* Commented/removed testcode() 1/31/05 Arun
	*/
public class MetaDataSetOperation extends MatrixAgent{
			public MetaDataSetOperation(Step step, FlowThread parent, ThreadGroup group){
						super(step, parent, group);
			}

			/**
				* Not Used
				*/
			private MetaDataCondition createConditionForQuery(){
						toLog("MetaDataSetOperation() - createConditionForQuery() -BEGIN",
								MatrixUtil.DEBUG_LOG);
						MetaDataCondition mdCond = null;
						toLog("MetaDataSetOperation() - createConditionForQuery() -END",
								MatrixUtil.DEBUG_LOG);
						return (mdCond);
			}

			/**
				* Not Used
				*/
			private MetaDataCondition[] createConditionArrayForQuery(){
						toLog("MetaDataSetOperation() - createConditionArrayForQuery() -BEGIN",
								MatrixUtil.DEBUG_LOG);
						MetaDataCondition[] conditionArray = null;
						MetaDataCondition tempMDCond = null;
						int conditionCount = 0;
						// Determine Number of Conditions
						conditionCount = 0; // ?????;
						conditionArray = new MetaDataCondition[conditionCount];
						for (int i = 0; i < conditionCount; i++){
									// Create Condition
									tempMDCond = createConditionForQuery();
									// Append Condition To Array
									conditionArray[i] = tempMDCond;
						}
						toLog("MetaDataSetOperation() - createConditionArrayForQuery() -END",
								MatrixUtil.DEBUG_LOG);
						return (conditionArray);
			}

			/**
				* Not Used
				*/
			private MetaDataSelect[] createSelectionForQuery(){
						toLog("MetaDataSetOperation() - createSelectionForQuery() -BEGIN",
								MatrixUtil.DEBUG_LOG);
						String selectFieldNames[] = null;
						MetaDataSelect selectArray[] = null;
						// Extract Field Names
						// Create Select Array
						selectArray = MetaDataSet.newSelection(selectFieldNames);
						toLog("MetaDataSetOperation() - createSelectionForQuery() -END",
								MatrixUtil.DEBUG_LOG);
						return (selectArray);
			}

			/**
				* Not Used
				*/
			private int createResultCountForQuery(){
						toLog("MetaDataSetOperation() - createResultCountForQuery() -BEGIN",
								MatrixUtil.DEBUG_LOG);
						int resultCount = 0;
						int count = 0;
						// Extract Count
						count = 20; // ???????
						resultCount = count;
						toLog("MetaDataSetOperation() - createResultCountForQuery() -END",
								MatrixUtil.DEBUG_LOG);
						return (resultCount);
			}

			/**
				* This method is for checking if the query is a pre-defined query
				* 1) Retrieve User Defined Attribute Names
				*       If a selection field is {"USER_DEFINED_ATTRIBUTES_NAMES",SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES0}
				*
				* @param conditionArray List of Condition for Query
				* @param selectArray List of Selection Fields for Query
				* @param queryFileName Name of data set querying about
				* @return  Returns new condition
				*/
			private MetaDataCondition processSpecialQueryForRetrievingUDAttributes(
					MetaDataCondition[] conditionArray,
					MetaDataSelect[] selectArray,
					String queryFileName
					){
						boolean retrieveUDAttributeNamesQuery = false;
						MetaDataCondition dirLikeCondition = null;
						toLog(
								"MetaDataSetOperation-processSpecialQueryForRetrievingUDAttributes() -BEGIN",
								MatrixUtil.DEBUG_LOG);
						for (int i = 0; i < selectArray.length; i++){
									MetaDataSelect selection = selectArray[i];
									toLog(
											"MetaDataSetOperation-processSpecialQueryForRetrievingUDAttributes() " +
											"selection : (" + i + ")" + selection.getFieldName(),
											MatrixUtil.DEBUG_LOG);
									if (selection.getFieldName().equals(SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES)){
												retrieveUDAttributeNamesQuery = true;
												break;
									}
						}
						// If retrieveUDAttributeNamesQuery is true, then a new condition has to be added
						// DIRECTORY_NAME LIKE queryFileName%
						if (retrieveUDAttributeNamesQuery){
									dirLikeCondition = MetaDataSet.newCondition(SRBMetaDataSet.
											DIRECTORY_NAME,
											MetaDataCondition.LIKE,
											queryFileName + "%");
									toLog(
											"MetaDataSetOperation-processSpecialQueryForRetrievingUDAttributes() -DIR LIKE COND CREATED: " +
											queryFileName, MatrixUtil.DEBUG_LOG);
						}
						toLog(
								"MetaDataSetOperation-processSpecialQueryForRetrievingUDAttributes() -END",
								MatrixUtil.DEBUG_LOG);
						return (dirLikeCondition);
			}

			/**
				* Executes the query by making a call to SRB through JARGON
				* @param fileSystem File System for Query
				* @param conditionArray List of Query Conditions
				* @param selectArray List of Query Selections
				* @param numberOfResults Number of Query Results
				* @param queryFileName Name of Data Set querying about
				* @return Metadata Query Results
				*/
			private MetaDataRecordList[] executeQuery(GeneralFileSystem fileSystem,
					MetaDataCondition[]
					conditionArray,
					MetaDataSelect[] selectArray,
					int numberOfResults,
					String queryFileName) throws
					Exception{
						toLog("MetaDataSetOperation() - executeQuery() -BEGIN",
								MatrixUtil.DEBUG_LOG);
						MetaDataRecordList[] resultList = null;
						MetaDataCondition[] updatedConditionArray = null;
						GeneralFile file = null;
						MetaDataCondition dirLikeCondition = null; // New Condition for Special Query - Attribute Name Retrieval
						dirLikeCondition = processSpecialQueryForRetrievingUDAttributes(
								conditionArray, selectArray, queryFileName);
						if (dirLikeCondition == null && queryFileName != null &&
								queryFileName.trim().length() > 0){
									toLog("MetaDataSetOperation() - executeQuery() -queryFileName:" +
											queryFileName + ",END", MatrixUtil.DEBUG_LOG);
									toLog(
											"MetaDataSetOperation() - executeQuery() -queryFileName-trimmed length:" +
											queryFileName.trim().length() + ",END", MatrixUtil.DEBUG_LOG);
									file = FileFactory.newFile(fileSystem, queryFileName);
									// Code From Meta Data Test
									//if (!file.exists())
									//		file.createNewFile();
									//MetaDataCondition dirCond = MetaDataSet.newCondition( SRBMetaDataSet.DIRECTORY_NAME,MetaDataCondition.EQUAL,file.getParent());
									toLog("MetaDataSetOperation() - executeQuery() -file.getName():" +
											file.getName(), MatrixUtil.DEBUG_LOG);
									MetaDataCondition dirCond = MetaDataSet.newCondition(SRBMetaDataSet.
											DIRECTORY_NAME,
											MetaDataCondition.EQUAL, queryFileName);
									updatedConditionArray = addNewCondition(conditionArray, dirCond);
									/*
												// Add New Condition To condition Array
												if(conditionArray!=null && conditionArray.length>0)
												{
										updatedConditionArray =  new  MetaDataCondition[conditionArray.length+1];
										toLog("MetaDataSetOperation() - executeQuery() -original conditionArray LENGTH:"+conditionArray.length,MatrixUtil.DEBUG_LOG);
										toLog("MetaDataSetOperation() - executeQuery() -updated conditionArray LENGTH:"+updatedConditionArray.length,MatrixUtil.DEBUG_LOG);
										for(int ind=0;ind<conditionArray.length;ind++)
										{
												toLog("MetaDataSetOperation() - executeQuery() -COPY index:"+ind,MatrixUtil.DEBUG_LOG);
												updatedConditionArray[ind]= conditionArray[ind];
												MatrixUtil.toLog("MetaDataSetOperation-executeQuery()"+"Updated Condition  : ("+ind+")"+updatedConditionArray[ind].toString(), 5);
										}
										MatrixUtil.toLog("MetaDataSetOperation-executeQuery()"+"Updated Condition dirCond : ("+(updatedConditionArray.length-1)+")"+dirCond.toString(), 5);
										updatedConditionArray[updatedConditionArray.length-1]= dirCond;
												toLog("MetaDataSetOperation() - executeQuery() -DIR..NAME condition added",MatrixUtil.DEBUG_LOG);
												}
										*/
						} else if (dirLikeCondition != null){
									updatedConditionArray = addNewCondition(conditionArray,
											dirLikeCondition);
						} else{
									toLog(
											"MetaDataSetOperation() - executeQuery() -DIR..NAME condition NOT added",
											MatrixUtil.DEBUG_LOG);
									updatedConditionArray = conditionArray;
						}
						// DEBUG CODE
						toLog("MetaDataSetOperation-executeQuery():QUERY DETAILS",
								MatrixUtil.DEBUG_LOG);
						toLog("MetaDataSetOperation-executeQuery():CONDITIONS",
								MatrixUtil.DEBUG_LOG);
						for (int i = 0; i < updatedConditionArray.length; i++){
									MetaDataCondition condition = updatedConditionArray[i];
									//MatrixUtil.toLog("MetaDataSetOperation-executeQuery()"+"Condition  : ("+i+")"+condition.getFieldName(), MatrixUtil.DEBUG_LOG);
									//MatrixUtil.toLog("MetaDataSetOperation-executeQuery()"+"Condition  Details: ("+i+")"+condition.toString(), MatrixUtil.DEBUG_LOG);
									if (condition.getFieldName().equals(SRBMetaDataSet.
											DEFINABLE_METADATA_FOR_FILES)){
												toLog("\nMetaDataSetOperation-executeQuery()" +
														"USER DEFINED CONDITION  : (" + i + ")",
														MatrixUtil.DEBUG_LOG);
												MetaDataTable mdTable = condition.getTableValue();
												toLog("MetaDataSetOperation-executeQuery():mdTable row count= " +
														mdTable.getRowCount(), MatrixUtil.DEBUG_LOG);
												toLog(
														"MetaDataSetOperation-executeQuery():mdTable column count=" +
														mdTable.getColumnCount(), MatrixUtil.DEBUG_LOG);
												toLog("MetaDataSetOperation-executeQuery():mdTable opr count=" +
														mdTable.getOperators().length, MatrixUtil.DEBUG_LOG);
												for (int row = 0; row < mdTable.getRowCount(); row++){
															toLog(
																	"MetaDataSetOperation-executeQuery():mdTable -Operator " +
																	mdTable.getOperator(row), MatrixUtil.DEBUG_LOG);
															for (int col = 0; col < mdTable.getColumnCount(); col++){
																		toLog("MetaDataSetOperation-executeQuery():mdTable (" +
																				row + "," + col + "):" + "= " +
																				mdTable.getStringValue(row, col),
																				MatrixUtil.DEBUG_LOG);
															}
												}
									} else{
												toLog("\nMetaDataSetOperation-executeQuery()" +
														"SYSTEM DEFINED CONDITION  : (" + i + ")" +
														condition.getFieldName(), MatrixUtil.DEBUG_LOG);
												toLog("MetaDataSetOperation-executeQuery()" +
														"Condition  Operator: (" + i + ")" +
														condition.getOperator(), MatrixUtil.DEBUG_LOG);
												toLog("MetaDataSetOperation-executeQuery()" +
														"Condition  VALUE: (" + i + ")" +
														condition.getStringValue(), MatrixUtil.DEBUG_LOG);
									}
						}
						toLog("MetaDataSetOperation-executeQuery():SELECTION FIELDS",
								MatrixUtil.DEBUG_LOG);
						for (int i = 0; i < selectArray.length; i++){
									MetaDataSelect selection = selectArray[i];
									toLog("MetaDataSetOperation-executeQuery() " + "selection : (" + i +
											")" + selection.toString(), MatrixUtil.DEBUG_LOG);
						}
						toLog("MetaDataSetOperation-executeQuery():numberOfResults:" +
								numberOfResults, MatrixUtil.DEBUG_LOG);
						//resultList = fileSystem.query(updatedConditionArray,selectArray,numberOfResults);
						resultList = fileSystem.query(updatedConditionArray, selectArray);
						fileSystem = null;
						file = null;
						toLog("MetaDataSetOperation() - executeQuery() -END",
								MatrixUtil.DEBUG_LOG);
						return (resultList);
			}

			/**
				* Adds new Condition to condition array
				* @param conditionArray List of Conditions
				* @param newCond New Condition to be added to list
				* @return Updated list of conditions
				*/
			private MetaDataCondition[] addNewCondition(MetaDataCondition[]
					conditionArray,
					MetaDataCondition newCond){
						toLog("MetaDataSetOperation() - addNewCondition() -BEGIN",
								MatrixUtil.DEBUG_LOG);
						MetaDataCondition[] updatedConditionArray = null;
						// Add New Condition To condition Array
						if (conditionArray == null && conditionArray.length > 0){
									updatedConditionArray = new MetaDataCondition[1];
									updatedConditionArray[0] = newCond;
						} else{
									updatedConditionArray = new MetaDataCondition[conditionArray.length +
											1];
									toLog(
											"MetaDataSetOperation() - addNewCondition() -original conditionArray LENGTH:" +
											conditionArray.length, MatrixUtil.DEBUG_LOG);
									toLog(
											"MetaDataSetOperation() - addNewCondition() -updated conditionArray LENGTH:" +
											updatedConditionArray.length, MatrixUtil.DEBUG_LOG);
									for (int ind = 0; ind < conditionArray.length; ind++){
												toLog("MetaDataSetOperation() - addNewCondition() -COPY index:" +
														ind, MatrixUtil.DEBUG_LOG);
												updatedConditionArray[ind] = conditionArray[ind];
												toLog("MetaDataSetOperation-addNewCondition()" +
														"New Condition  : (" + ind + ")" +
														updatedConditionArray[ind].toString(),
														MatrixUtil.DEBUG_LOG);
									}
									toLog("MetaDataSetOperation-addNewCondition()" +
											"New Condition dirCond : (" +
											(updatedConditionArray.length - 1) + ")" + newCond.toString(),
											MatrixUtil.DEBUG_LOG);
									updatedConditionArray[updatedConditionArray.length - 1] = newCond;
									toLog(
											"MetaDataSetOperation() - addNewCondition() -NEW condition added",
											MatrixUtil.DEBUG_LOG);
						}
						toLog("MetaDataSetOperation() - addNewCondition() -END",
								MatrixUtil.DEBUG_LOG);
						return (updatedConditionArray);
			}

			/**
				* Creates Response For Query Results
				* @param recordList List of Query Results
				* @return Query Results in string format
				*/
			private String createMatrixResponseForQueryResults(MetaDataRecordList[]
					recordList){
						toLog(
								"MetaDataSetOperation() - createMatrixResponseForQueryResults() -BEGIN",
								MatrixUtil.DEBUG_LOG);
						Object matrixResponse = null;
						String matrixResponseString = null;
						// Print Results
						matrixResponseString = printQueryResults(recordList);
						// Create Response
						matrixResponse = new Object();
						//matrixResponseString = queryParser.queryProcessor.mapQueryResults(recordList);
						//Return Response
						toLog(
								"MetaDataSetOperation() - createMatrixResponseForQueryResults() -END",
								MatrixUtil.DEBUG_LOG);
						//return(matrixResponse);
						return (matrixResponseString);
			}

			/**
				* The method that executes the steps for querying meta data about
				* a Container,Collection or Data Set
				*/
			public void typeSpecificExecute() throws Exception{
						boolean stepAbort = false;
						MetaDataCondition[] conditionArray = null;
						MetaDataSelect[] selectArray = null;
						int resultCount = 0;
						MetaDataRecordList[] resultList = null;
						GeneralFileSystem fileSystem = null;
						String queryFileName = null;
						Object matrixResponse = null;
						OpResult opResult = null;
						String queryResultString = null;
						MetadataQueryOp queryOprn = getOperation().getMetadataQueryOp();
						MetadataQueryParams queryParams = queryOprn.getStdParams();
						String queryRequestString = evaluateExpressionToString(queryParams.
								getQueryRequestString());
						String queryString = queryRequestString;
						MatrixQueryParser queryParser = MatrixQueryParserPool.
								getQueryParserForMatrix(queryString);
						// PARSE QUERY STRING
						ASTStart start = queryParser.Start();
						MatrixQueryParserVisitor visitor = new MatrixQueryParserDumpVisitor();
						start.jjtAccept(visitor, null);
						toLog("MetaDataSetOperation() - run() -QUERY PARSED SUCCESSFULLY",
								MatrixUtil.DEBUG_LOG);
						queryParser.queryProcessor.process();
						queryParser.queryProcessor.matrixQueryDetails();
						queryParser.queryProcessor.srbQueryDetails();
						// Determine Query Type
						SRBQueryWrapper srbQuery = queryParser.queryProcessor.
								getSRBQueryWrapper();
						queryFileName = srbQuery.getQueryFileNameParameter();
						// Determine Condition For Query
						conditionArray = srbQuery.getMetaDataCondition(); //createConditionArrayForQuery();
						// Determing Selection For Query
						selectArray = srbQuery.getMetaDataSelect(); //createSelectionForQuery();
						// Determine Result Count For Query // How to get this ?
						resultCount = createResultCountForQuery();
						// Execute Query
						SRBConnection srbConnection = (SRBConnection) retrieveGridConnection();
						SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
						resultList = executeQuery(fileSystem, conditionArray, selectArray,
								resultCount, queryFileName);
						// Create Response for Query Results
						queryResultString = queryParser.queryProcessor.mapQueryResults(
								resultList); //createMatrixResponseForQueryResults(resultList);
						// Return Query Results
						opResult = getObjectFactory().createOpResult();
						MetadataQueryResult queryResult = getObjectFactory().
								createMetadataQueryResult();
						MetadataQueryInfo queryInfo = getObjectFactory().
								createMetadataQueryInfo();
						AnyType queryAnyType = getObjectFactory().createAnyType();
						queryAnyType.getContent().add(queryResultString);
						queryInfo.setQueryRequestString(queryRequestString);
						//queryInfo.setQueryResultString("JARGON NOT INTEGRATED");
						queryInfo.setQueryResultString(queryAnyType); // Return Type is AnyType
						queryResult.setQueryResults(queryInfo);
						opResult.setMetadataQueryResult(queryResult);
						setResponse(opResult);
			}

			/**
				* Displays the Query Results
				* Method details taken from jargon sample code
				* @param rl Results
				* @return Query Result in string format
				*
				*/
			public String printQueryResults(MetaDataRecordList[] rl){
						toLog("MetaDataSetOperation() - printQueryResults() -BEGIN",
								MatrixUtil.DEBUG_LOG);
						toLog("MetaDataSetOperation() - ****************************",
								MatrixUtil.DEBUG_LOG);
						String resultString = new String("");
						//print query results
						if (rl != null){ //Nothing in the database matched the query
									System.out.print("\n");
									resultString += "\n";
									for (int i = 0; i < rl[0].getRecordCount(); i++){
												System.out.print(rl[0].getFieldName(i) + "\t");
												resultString += rl[0].getFieldName(i) + "\t";
									}
									for (int i = 0; i < rl.length; i++){
												System.out.print("\n");
												resultString += "\n";
												for (int j = 0; j < rl[i].getRecordCount(); j++){
															if (rl[i].getFieldType(j) == MetaDataField.TABLE){
																		//tables aren't working
																		MetaDataTable table = rl[i].getTableValue(j);
																		System.out.print("table: ");
																		resultString += "table: ";
																		for (int k = 0; k < table.getRowCount(); k++){
																					System.out.print("\n");
																					resultString += "\n";
																					for (int l = 0; l < table.getColumnCount(); l++){
																								System.out.print(table.getStringValue(k, l) +
																										"\t");
																								resultString += table.getStringValue(k, l) +
																										"\t";
																					}
																		}
															} else{
																		System.out.print(rl[i].getStringValue(j) + "\t");
																		resultString += rl[i].getStringValue(j) + "\t";
															}
												}
									}
						} else{
									resultString += " No Result matching the query";
						}
						toLog("MetaDataSetOperation() - ****************************",
								MatrixUtil.DEBUG_LOG);
						toLog("MetaDataSetOperation() - printQueryResults() -END",
								MatrixUtil.DEBUG_LOG);
						return (resultString);
			}

//	/**
//		* Test Code
//		*/
//	public MetaDataRecordList[] testCode(GeneralFileSystem fileSystem) {
//		toLog("MetaDataSetOperation - testCode()-BEGIN", MatrixUtil.DEBUG_LOG);
//		MetaDataRecordList[] rl = null;
//		try {
//			String[] values = {"staff", "sysadmin"};
//			//SRBBConnection srbConnection = (SRBConnection)retrieveGridConnection();
//			//SRBAccount account = srbConnection.getSRBAccount();
//			fileSystem = FileFactory.newFileSystem(new SRBAccount(
//						"srb.sdsc.edu", 7573, "Matrix-demo", "******",
//						"/home/Matrix-demo.sdscFileFactory.newFileSystem(", "sdsc", "hpss-sdsc"));
//			//fileSystem = FileFactory.newFileSystem(account);
//			String fileName = "myJARGONMetaDataTestFile";
//			GeneralFile file = FileFactory.newFile(fileSystem, fileName);
//			if (!file.exists()) {
//				file.createNewFile();
//			}
//			MetaDataCondition conditionArray[] = {
//						MetaDataSet.newCondition(
//						SRBMetaDataSet.OFFSET, MetaDataCondition.LESS_THAN, 5000),
//						MetaDataSet.newCondition(
//						SRBMetaDataSet.SIZE, MetaDataCondition.BETWEEN, 0, 10000),
//						MetaDataSet.newCondition(
//						SRBMetaDataSet.USER_TYPE_NAME, MetaDataCondition.IN, values),
//						MetaDataSet.newCondition(SRBMetaDataSet.DIRECTORY_NAME,
//										MetaDataCondition.EQUAL,
//										file.getParent())
//			};
//			/**
//				* For items which matched the query, met the conditions above,
//				* the following values will be returned.
//				*/
//			String[] selectFieldNames = {
//						SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES,
//						//						SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES,
//						//						SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS,
//						//						SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES,
//						SRBMetaDataSet.USER_TYPE_NAME,
//						SRBMetaDataSet.USER_NAME,
//						SRBMetaDataSet.USER_GROUP_NAME,
//						SRBMetaDataSet.USER_ADDRESS,
//						SRBMetaDataSet.ACCESS_CONSTRAINT,
//						SRBMetaDataSet.FILE_NAME
//			};
//			MetaDataSelect selectArray[] =
//						MetaDataSet.newSelection(selectFieldNames);
//			/**
//				* The number of metadata values you want the query to return at one time.
//				* If more values would result from the query the can be appended to the
//				* MetaDataRecordList object using the .getMoreResults( int )
//				*/
//			//Number of values returned from server in first iteration.
//			int numOfResults = 13;
//			//Full query method
//			rl = fileSystem.query(conditionArray, selectArray, numOfResults);
//			if (rl == null) {
//				toLog("MetaDataSetOperation - testCode()-RESULT NULL",
//							MatrixUtil.DEBUG_LOG);
//				file = FileFactory.newFile(fileSystem, file.getParent());
//				rl = file.query(selectFieldNames);
//				if (rl == null) {
//					toLog("MetaDataSetOperation - testCode()-RESULT2 NULL",
//								MatrixUtil.DEBUG_LOG);
//				} else {
//					toLog("MetaDataSetOperation - testCode()-RESULT2 COUNT:" +
//								rl.length, MatrixUtil.DEBUG_LOG);
//				}
//			} else {
//				toLog("MetaDataSetOperation - testCode()-RESULT COUNT:" +
//							rl.length, MatrixUtil.DEBUG_LOG);
//			}
//			toLog("MetaDataSetOperation - testCode()-END", MatrixUtil.DEBUG_LOG);
//		} catch (Exception e) {
//			toLog("MetaDataSetOperation - testCode()-EXCEPTION:" + e,
//						MatrixUtil.DEBUG_LOG);
//		}
//		return (rl);
//	}

			/**
				* Get Operation Name
				* @returns the name of the operation encapsulated by this agent
				*/
			public String getOperationName(){
						return "MetaDataSetOperation";
			}
}