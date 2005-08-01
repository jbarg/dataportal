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

package edu.sdsc.matrix.srb.query;

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
import edu.sdsc.grid.io.srb.SRBMetaDataSet;

/**
	*
	* @author rmathew
	*
	* MatrixQueryProcessor class is used for processing a Matrix XQuery
	* The class is instantiated for each query.
	* It maintains the Matrix Query Information stored as clauses - For,Where and Return
	* It calls the appropriate methods to map
	* 	- Matrix Query  to  JARGON Query Format
	*  - JARGON Query Results to Matrix Query Result Format
	*/

	/* Developer Log
		* Original Code - Reena Mathew
		* Removed/commented some unused methods 1/31/05 Arun
		*/

public class MatrixQueryProcessor {
	public String forClauseString; // For Clause String - NOT USED
	public String whereClauseString; // Where Clause String - NOT USED
	public String returnClauseString; // Return Clause String - NOT USED
	private MatrixQueryForClause forClause; // For Clause
	private MatrixQueryWhereClause whereClause; // Where Clause
	private MatrixQueryReturnClause returnClause; // Return Clause
	private MatrixQueryWrapper matrixQueryWrapper; // Stores all the Matrix Query Information
	private SRBQueryWrapper srbQueryWrapper; // Stores all the JARGON Query Information
	private SRBQueryMapping queryMapper; // Maps JARGON to MATRIX formats and vice versa
	/*
		* Public Constructor
		* Instanitiates a new MatrixQueryWrapper to capture the Matrix Query Information
		*/
	public MatrixQueryProcessor() {
		matrixQueryWrapper = new MatrixQueryWrapper();
	}

	/*
		* Set Matrix Query Wrapper
		*/
	public void setMatrixQueryWrapper(MatrixQueryWrapper query) {
		matrixQueryWrapper = query;
	}

	/*
		* Return Matrix Query Wrapper
		*/
	public MatrixQueryWrapper getMatrixQueryWrapper() {
		return matrixQueryWrapper;
	}

	/*
		* Set SRB Query Wrapper
		*/
	public void setSRBQueryWrapper(SRBQueryWrapper query) {
		srbQueryWrapper = query;
	}

	/*
		* Return SRB Query Wrapper
		*/
	public SRBQueryWrapper getSRBQueryWrapper() {
		return srbQueryWrapper;
	}

	/*
		* Calls method to print MAtrix Query Details
		*/
	public void matrixQueryDetails() {
		matrixQueryWrapper.printClauses();
		//queryDetails();
		//executeQuery();
	}

	/*
		* Calls method to map Matrix Query to JARGON Query Format
		*/
	public void process() {
		MatrixUtil.toLog("MatrixQueryProcessor-process():BEGIN",
							MatrixUtil.DEBUG_LOG);
		mapQuery();
		MatrixUtil.toLog("MatrixQueryProcessor-process():END",
							MatrixUtil.DEBUG_LOG);
	}

	/*
		* Return Matrix Query For Clause
		*/
	public MatrixQueryForClause getForClause() {
		return matrixQueryWrapper.getMatrixQueryForClause();
	}

	/*
		* Return Matrix Query Where Clause
		*/
	public MatrixQueryWhereClause getWhereClause() {
		return matrixQueryWrapper.getMatrixQueryWhereClause();
	}

	/*
		* Return MAtrix Query Return Clause
		*/
	public MatrixQueryReturnClause getReturnClause() {
		return matrixQueryWrapper.getMatrixQueryReturnClause();
	}

	/*
		* Calls method that Maps Matrix Query Clause to JARGON Query Objects
		*/
	public void mapQuery() {
		MatrixUtil.toLog("MatrixQueryProcessor-mapQuery():BEGIN",
							MatrixUtil.DEBUG_LOG);
		queryMapper = new SRBQueryMapping();
		srbQueryWrapper = queryMapper.mapMatrixQueryToSRBQuery(
					matrixQueryWrapper);
		MatrixUtil.toLog("MatrixQueryProcessor-mapQuery():END",
							MatrixUtil.DEBUG_LOG);
	}

	/*
		* Calls method that Maps JARGON Query Results to Matrix Query Result Format
		*/
	public String mapQueryResults(MetaDataRecordList[] resultList) {
		MatrixUtil.toLog("MatrixQueryProcessor-mapQueryResults():BEGIN",
							MatrixUtil.DEBUG_LOG);
		String resultString = null;
		resultString = queryMapper.mapSRBQueryResultToMatrixQueryResult(
					resultList, matrixQueryWrapper.getMatrixQueryReturnClause());
		MatrixUtil.toLog("MatrixQueryProcessor-mapQueryResults():END",
							MatrixUtil.DEBUG_LOG);
		return resultString;
	}

	/*
		* Displays JARGON Format Query Details
		*/
	public void srbQueryDetails() {
		MetaDataCondition conditionArray[] = srbQueryWrapper.
					getMetaDataCondition();
		MetaDataSelect selectArray[] = srbQueryWrapper.getMetaDataSelect();
		MatrixUtil.toLog("MatrixQueryProcessor-srbQueryDetails():BEGIN",
							MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("MatrixQueryProcessor-srbQueryDetails():QUERY DETAILS",
							MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("MatrixQueryProcessor-srbQueryDetails():CONDITIONS",
							MatrixUtil.DEBUG_LOG);
		for (int i = 0; i < conditionArray.length; i++) {
			MetaDataCondition condition = conditionArray[i];
			MatrixUtil.toLog("MatrixQueryProcessor-srbQueryDetails()" +
								"Condition  : (" + i + ")" + condition.toString(),
								MatrixUtil.DEBUG_LOG);
		}
		MatrixUtil.toLog(
					"MatrixQueryProcessor-srbQueryDetails():SELECTION FIELDS",
					MatrixUtil.DEBUG_LOG);
		for (int i = 0; i < selectArray.length; i++) {
			MetaDataSelect selection = selectArray[i];
			MatrixUtil.toLog("MatrixQueryProcessor-srbQueryDetails() " +
								"selection : (" + i + ")" + selection.toString(),
								MatrixUtil.DEBUG_LOG);
		}
	}

	/*
		* Test Method to execute JARGON Query to SRB
		*/
//	public void executeQuery() {
//		//		host, port, user name, password,
//		//home directory, mdas domain home, default storage resource
//		try {
//			GeneralFileSystem fileSystem = FileFactory.newFileSystem(new
//						SRBAccount(
//						"srb.sdsc.edu", 6824, "rmathew", "",
//						"/home/rmathew.ucsd", "ucsd", "dtf-hpss-sdsc"));
//			System.out.println(
//						"MatrixQueryProcessor - executeQuery()-SRB Account Created");
//			String fileName = "myJARGONMetaDataTestFile";
//			GeneralFile file = FileFactory.newFile(fileSystem, fileName);
//			System.out.println(
//						"MatrixQueryProcessor - executeQuery()-Instantiate General File");
//			if (!file.exists()) {
//				file.createNewFile();
//				System.out.println(
//							"MatrixQueryProcessor - executeQuery()-createNewFile");
//			}
//			//		Number of values returned from server in first iteration.
//			int numOfResults = 20;
//			//Full query method
//			System.out.println(
//						"MatrixQueryProcessor - executeQuery()-Before Query");
//			MetaDataCondition conditionArray[] = {
//						MetaDataSet.newCondition(
//						SRBMetaDataSet.SIZE, MetaDataCondition.GREATER_OR_EQUAL, 0),
//			};
//			//MetaDataRecordList[] rl = fileSystem.query(conditionArray, srbQueryWrapper.getMetaDataSelect(), numOfResults );
//			//MetaDataRecordList[] rl = fileSystem.query(srbQueryWrapper.getMetaDataCondition(), srbQueryWrapper.getMetaDataSelect(), numOfResults );
//			/*
//				file = FileFactory.newFile( fileSystem, file.getParent() );
//				String[] selectFieldNames = {
//						SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES,
//						SRBMetaDataSet.USER_TYPE_NAME,
//						SRBMetaDataSet.USER_NAME,
//						SRBMetaDataSet.USER_GROUP_NAME,
//						SRBMetaDataSet.USER_ADDRESS,
//						SRBMetaDataSet.ACCESS_CONSTRAINT,
//						SRBMetaDataSet.FILE_NAME
//					};
//				MetaDataRecordList[] rl = file.query( selectFieldNames );
//				*/
//			System.out.println(
//						"MatrixQueryProcessor - executeQuery()-After Query");
//			//printQueryResults( rl );
//		} catch (Exception e) {
//			System.out.println("Exception in executeQuery():" + e);
//		}
//	}

	/*
		* Test Codes to display JARGON Query Results
		* Using JARGON MetaDataTest code
		*/
//	public static void printQueryResults(MetaDataRecordList[] rl) {
//		MatrixUtil.toLog("MatrixQueryProcessor-printQueryResults():BEGIN",
//							MatrixUtil.DEBUG_LOG);
//		//print query results
//		if (rl != null) { //Nothing in the database matched the query
//			MatrixUtil.toLog("\n", 5);
//			for (int i = 0; i < rl[0].getRecordCount(); i++) {
//				MatrixUtil.toLog(rl[0].getFieldName(i) + "\t", 5);
//			}
//			for (int i = 0; i < rl.length; i++) {
//				MatrixUtil.toLog("\n", 5);
//				for (int j = 0; j < rl[i].getRecordCount(); j++) {
//					if (rl[i].getFieldType(j) == MetaDataField.TABLE) {
//						//tables aren't working
//						MetaDataTable table = rl[i].getTableValue(j);
//						MatrixUtil.toLog("table: ", 5);
//						for (int k = 0; k < table.getRowCount(); k++) {
//							MatrixUtil.toLog("\n", 5);
//							for (int l = 0; l < table.getColumnCount(); l++) {
//								MatrixUtil.toLog(table.getStringValue(k, l) +
//													"\t", 5);
//							}
//						}
//					} else {
//						MatrixUtil.toLog(rl[i].getStringValue(j) + "\t", 5);
//					}
//				}
//			}
//		}
//		MatrixUtil.toLog("\n", 5);
//		MatrixUtil.toLog("MatrixQueryProcessor-printQueryResults():END",
//							MatrixUtil.DEBUG_LOG);
//	}
}