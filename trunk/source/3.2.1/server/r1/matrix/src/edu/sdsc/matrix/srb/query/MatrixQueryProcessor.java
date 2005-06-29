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



package edu.sdsc.matrix.srb.query;

import edu.sdsc.grid.io.local.*;
import edu.sdsc.grid.io.srb.*;
import edu.sdsc.grid.io.*;
import edu.sdsc.matrix.srb.util.MatrixUtil;

/*
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


public class MatrixQueryProcessor
{
	public String forClauseString;		// For Clause String - NOT USED
	public String whereClauseString;	// Where Clause String - NOT USED
	public String returnClauseString;	// Return Clause String - NOT USED
	
	private MatrixQueryForClause forClause;	// For Clause
	private MatrixQueryWhereClause whereClause;	// Where Clause
	private MatrixQueryReturnClause returnClause; // Return Clause
	 
	private MatrixQueryWrapper matrixQueryWrapper; // Stores all the Matrix Query Information
	private SRBQueryWrapper srbQueryWrapper; // Stores all the JARGON Query Information
	
	private SRBQueryMapping queryMapper; 	// Maps JARGON to MATRIX formats and vice versa
	
	
	/*
	 * Public Constructor
	 * Instanitiates a new MatrixQueryWrapper to capture the Matrix Query Information
	 */
	public MatrixQueryProcessor()
	{
		matrixQueryWrapper =  new MatrixQueryWrapper();
		
	}
	
	/*
	 * Set Matrix Query Wrapper
	 */
	public void setMatrixQueryWrapper(MatrixQueryWrapper query)
	{
		matrixQueryWrapper = query;
	}
	
	/*
	 * Return Matrix Query Wrapper
	 */
	public MatrixQueryWrapper getMatrixQueryWrapper()
	{
		return matrixQueryWrapper;
	}	
	
	/*
	 * Set SRB Query Wrapper
	 */
	public void setSRBQueryWrapper(SRBQueryWrapper query)
	{
		srbQueryWrapper = query;
	}
	
	/*
	 * Return SRB Query Wrapper
	 */
	public SRBQueryWrapper getSRBQueryWrapper()
	{
		return srbQueryWrapper;
	}
	
	/*
	 * Calls method to print MAtrix Query Details
	 */
	public void matrixQueryDetails()
	{
		matrixQueryWrapper.printClauses();		
		//queryDetails();		
		//executeQuery();		
	}
	
	/*
	 * Calls method to map Matrix Query to JARGON Query Format
	 */
	public void process()
	{
		MatrixUtil.toLog("MatrixQueryProcessor-process():BEGIN",MatrixUtil.DEBUG_LOG);	
		mapQuery();
		MatrixUtil.toLog("MatrixQueryProcessor-process():END",MatrixUtil.DEBUG_LOG);	
	}
	
	/*
	 * Return Matrix Query For Clause
	 */
	public MatrixQueryForClause getForClause()
	{
		return matrixQueryWrapper.getMatrixQueryForClause();
	}
	
	/*
	 * Return Matrix Query Where Clause
	 */
	public MatrixQueryWhereClause getWhereClause()
	{
		return matrixQueryWrapper.getMatrixQueryWhereClause();
	}
	
	/*
	 * Return MAtrix Query Return Clause
	 */
	public MatrixQueryReturnClause getReturnClause()
	{
		return matrixQueryWrapper.getMatrixQueryReturnClause();
	}
	
	/*
	 * Calls method that Maps Matrix Query Clause to JARGON Query Objects
	 */
	public void mapQuery()
	{
		MatrixUtil.toLog("MatrixQueryProcessor-mapQuery():BEGIN",MatrixUtil.DEBUG_LOG);
		queryMapper = new SRBQueryMapping();
		srbQueryWrapper = queryMapper.mapMatrixQueryToSRBQuery(matrixQueryWrapper);
		MatrixUtil.toLog("MatrixQueryProcessor-mapQuery():END",MatrixUtil.DEBUG_LOG);
	}
	
	/*
	 * Calls method that Maps JARGON Query Results to Matrix Query Result Format
	 */
	public String mapQueryResults(MetaDataRecordList[] resultList)
	{
		MatrixUtil.toLog("MatrixQueryProcessor-mapQueryResults():BEGIN",MatrixUtil.DEBUG_LOG);
		String resultString = null;
		resultString = queryMapper.mapSRBQueryResultToMatrixQueryResult(resultList ,matrixQueryWrapper.getMatrixQueryReturnClause());		
		MatrixUtil.toLog("MatrixQueryProcessor-mapQueryResults():END",MatrixUtil.DEBUG_LOG);
		return resultString;
	}
	
	/*
	 * Displays JARGON Format Query Details
	 */
	public void srbQueryDetails()
	{
		MetaDataCondition conditionArray[] =srbQueryWrapper.getMetaDataCondition();
		MetaDataSelect selectArray[] = srbQueryWrapper.getMetaDataSelect();
		
		MatrixUtil.toLog("MatrixQueryProcessor-srbQueryDetails():BEGIN",MatrixUtil.DEBUG_LOG);
		
		MatrixUtil.toLog("MatrixQueryProcessor-srbQueryDetails():QUERY DETAILS",MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("MatrixQueryProcessor-srbQueryDetails():CONDITIONS",MatrixUtil.DEBUG_LOG);
		
		for(int i=0;i<conditionArray.length;i++)
		{
			MetaDataCondition condition = 	conditionArray[i];
			MatrixUtil.toLog("MatrixQueryProcessor-srbQueryDetails()"+"Condition  : ("+i+")"+condition.toString(),MatrixUtil.DEBUG_LOG);			
		}
		
		
		MatrixUtil.toLog("MatrixQueryProcessor-srbQueryDetails():SELECTION FIELDS",MatrixUtil.DEBUG_LOG);
			for(int i=0;i<selectArray.length;i++)
			{
				MetaDataSelect selection = 	selectArray[i];
				MatrixUtil.toLog("MatrixQueryProcessor-srbQueryDetails() "+"selection : ("+i+")"+selection.toString(),MatrixUtil.DEBUG_LOG);						
			}
		
	}
	
	/*
	 * Test Method to execute JARGON Query to SRB
	 */
	public void executeQuery()
	{
		//		host, port, user name, password,
		//home directory, mdas domain home, default storage resource
		try
		{
		
			GeneralFileSystem fileSystem = FileFactory.newFileSystem(new SRBAccount(
							  "srb.sdsc.edu", 6824, "rmathew", "RMATHEW",
							  "/home/rmathew.ucsd", "ucsd", "dtf-hpss-sdsc" ));
			System.out.println("MatrixQueryProcessor - executeQuery()-SRB Account Created");				  
			String fileName = "myJARGONMetaDataTestFile";
			GeneralFile file = FileFactory.newFile( fileSystem, fileName );
			System.out.println("MatrixQueryProcessor - executeQuery()-Instantiate General File");	
			if (!file.exists())
			{
				  file.createNewFile();
				System.out.println("MatrixQueryProcessor - executeQuery()-createNewFile");
			}		  
			//		Number of values returned from server in first iteration.
			 int numOfResults = 20;
			 //Full query method
			System.out.println("MatrixQueryProcessor - executeQuery()-Before Query");
			
			MetaDataCondition conditionArray[] = {
							MetaDataSet.newCondition(
								SRBMetaDataSet.SIZE, MetaDataCondition.GREATER_OR_EQUAL, 0 ),							
						};

			
			
			
			
			
			//MetaDataRecordList[] rl = fileSystem.query(conditionArray, srbQueryWrapper.getMetaDataSelect(), numOfResults );
						
			 //MetaDataRecordList[] rl = fileSystem.query(srbQueryWrapper.getMetaDataCondition(), srbQueryWrapper.getMetaDataSelect(), numOfResults );
			 /*
			file = FileFactory.newFile( fileSystem, file.getParent() );
			String[] selectFieldNames = {
							SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES,
							SRBMetaDataSet.USER_TYPE_NAME,
							SRBMetaDataSet.USER_NAME,
							SRBMetaDataSet.USER_GROUP_NAME,
							SRBMetaDataSet.USER_ADDRESS,
							SRBMetaDataSet.ACCESS_CONSTRAINT,
							SRBMetaDataSet.FILE_NAME
						};
			
			MetaDataRecordList[] rl = file.query( selectFieldNames );
			 */
			System.out.println("MatrixQueryProcessor - executeQuery()-After Query");
	    	  //printQueryResults( rl );	
		}
		catch(Exception e)
		{
			System.out.println("Exception in executeQuery():"+e);    
		}
	}
	
	
	/*
	 * Test Codes to display JARGON Query Results
	 * Using JARGON MetaDataTest code 
	 */
	
	public static void printQueryResults( MetaDataRecordList[] rl )
	{
		MatrixUtil.toLog("MatrixQueryProcessor-printQueryResults():BEGIN",MatrixUtil.DEBUG_LOG);
		//print query results
		if (rl != null) { //Nothing in the database matched the query
			MatrixUtil.toLog("\n",5);
			for (int i=0;i<rl[0].getRecordCount();i++) {
				MatrixUtil.toLog(rl[0].getFieldName(i)+"\t",5);
			}

			for (int i=0;i<rl.length;i++) {
				MatrixUtil.toLog("\n",5);
				for (int j=0;j<rl[i].getRecordCount();j++) {
					if (rl[i].getFieldType(j) == MetaDataField.TABLE) {
						//tables aren't working
						MetaDataTable table = rl[i].getTableValue(j);
						MatrixUtil.toLog("table: ",5);
						for (int k=0;k<table.getRowCount();k++) {
							MatrixUtil.toLog("\n",5);
							for (int l=0;l<table.getColumnCount();l++) {
								MatrixUtil.toLog(table.getStringValue( k, l )+"\t",5);
							}
						}
					}
					else {
						MatrixUtil.toLog(rl[i].getStringValue(j)+"\t",5);
					}
				}
			}
		}
		MatrixUtil.toLog("\n",5);

		MatrixUtil.toLog("MatrixQueryProcessor-printQueryResults():END",MatrixUtil.DEBUG_LOG);
	}

	
}