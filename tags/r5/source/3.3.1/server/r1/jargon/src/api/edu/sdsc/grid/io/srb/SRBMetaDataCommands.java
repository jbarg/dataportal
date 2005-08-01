//
//	Copyright (c) 2003  San Diego Supercomputer Center (SDSC),
//	University of California, San Diego (UCSD), San Diego, CA, USA.
//
//	Users and possessors of this source code are hereby granted a
//	nonexclusive, royalty-free copyright and design patent license
//	to use this code in individual software.  License is not granted
//	for commercial resale, in whole or in part, without prior written
//	permission from SDSC/UCSD.  This source is provided "AS IS"
//	without express or implied warranty of any kind.
//
//
//  FILE
//	SRBMetaDataCommands.java	-  edu.sdsc.grid.io.srb.SRBMetaDataCommands
//
//  CLASS HIERARCHY
//	java.lang.Object
//     |
//     +-.SRBMetaDataCommands
//
//  PRINCIPAL AUTHOR
//	Lucas Gilbert, SDSC/UCSD
//
//
package edu.sdsc.grid.io.srb;

import edu.sdsc.grid.io.*;
import edu.sdsc.grid.io.local.*;

import java.io.IOException;
import java.lang.reflect.Array;
import java.util.HashSet;
import java.util.Vector;


/**
 * SRBMetaDataCommands handles the SRB server call srbGetDataDirInfo and
 * all related methods. All the metadata methods were getting to complicated
 * and unwieldy in the SRBCommands class. So they were moved here to
 * keep the code, a tiny bit more, readable.
 *
 * <P>
 * @author	Lucas Gilbert, San Diego Supercomputer Center
 * @since   JARGON1.2
 */
//Btw, if you ever inherit the job of maintaining/developing this code
//and you have to do anything with the DEFINABLE_METADATA_FOR_...
//get the SRB server database guy to write an outer join function.
//all this complictaed merge and join stuff would be much better
//done on the server.
class SRBMetaDataCommands
{
//----------------------------------------------------------------------
//  Constants
//----------------------------------------------------------------------



//----------------------------------------------------------------------
//  Fields
//----------------------------------------------------------------------
	/**
	 * Keep track of the connection.
	 */
	private SRBCommands commands;






//----------------------------------------------------------------------
//  Constructors and Destructors
//----------------------------------------------------------------------
	/**
	 * Constructor
	 *<P>
	 * @param userInfoDirectory	the directory to find the user info
	 * @throws	IOException	if the connection to the SRB fails.
	 */
	SRBMetaDataCommands( SRBCommands commands )
		throws IOException
	{
		this.commands = commands;
	}





	/**
	 * Finalizes the object by explicitly letting go of each of
	 * its internally held values.
	 */
	protected void finalize( )
	{
		if (commands != null)
			commands = null;
	}




//----------------------------------------------------------------------
// Various methods for handling metadata used by srbGetDataDirInfo(...).
//----------------------------------------------------------------------
	static String getOperator( MetaDataCondition condition )
	{
		return MetaDataCondition.getOperatorString(condition.getOperator());
	}

//TODO redundant, should change references to direct MetaDataCondition call.
	static String getOperator( int condition )
	{
		return MetaDataCondition.getOperatorString(condition);
	}


	/**
	 * used to properly group the value with the operator,
	 * string values are in single quotes, ints are not...
	 */
	private String quotes( int type )
	{
		switch (type) {
			case MetaDataField.INT:
			case MetaDataField.LONG:
			case MetaDataField.FLOAT:
				return "";
			case MetaDataField.STRING:
			case MetaDataField.DATE:
			default:
				return "'";
		}
	}


	//for the qVal when the conditions contain a table
	private void setTableQval( MetaDataTable table, char[][] qVal,
		String fieldName )
	{
			//1st row
		String queryStatement = new String( "= '"+table.getStringValue(0,0)+"'");
		System.arraycopy(	queryStatement.toCharArray(), 0,
			qVal[SRBMetaDataSet.getSRBID( fieldName+"0_1" )],	0,
			queryStatement.length());

		queryStatement = new String(
			getOperator( table.getOperator(0) )+" '"+table.getStringValue(0,1)+"'");
		System.arraycopy(	queryStatement.toCharArray(), 0,
			qVal[SRBMetaDataSet.getSRBID( fieldName+"1_1" )],	0,
			queryStatement.length());

			//2nd row
		if (table.getRowCount() >= 2) {
			queryStatement =	new String(	"= '"+table.getStringValue(1,0)+"'");
			System.arraycopy(	queryStatement.toCharArray(), 0,
				qVal[SRBMetaDataSet.getSRBID( fieldName+"0_2" )],	0,
				queryStatement.length());

			queryStatement =	new String(
				getOperator( table.getOperator(1) )+" '"+table.getStringValue(1,1)+"'");
			System.arraycopy(	queryStatement.toCharArray(), 0,
				qVal[SRBMetaDataSet.getSRBID( fieldName+"1_2" )],	0,
				queryStatement.length());
		}

			//3rd row
		if (table.getRowCount() >= 3) {
			queryStatement = new String( "= '"+table.getStringValue(2,0)+"'");
			System.arraycopy(	queryStatement.toCharArray(), 0,
				qVal[SRBMetaDataSet.getSRBID( fieldName+"0_3" )],	0,
				queryStatement.length());

			queryStatement = new String(
				getOperator( table.getOperator(2) )+" '"+table.getStringValue(2,1)+"'");
			System.arraycopy(	queryStatement.toCharArray(), 0,
				qVal[SRBMetaDataSet.getSRBID( fieldName+"1_3" )],	0,
				queryStatement.length());
		}

			//4th row
		if (table.getRowCount() >= 4) {
			queryStatement = new String( "= '"+table.getStringValue(3,0)+"'");
			System.arraycopy(	queryStatement.toCharArray(), 0,
				qVal[SRBMetaDataSet.getSRBID( fieldName+"0_4" )],	0,
				queryStatement.length());

			queryStatement = new String(
				getOperator( table.getOperator(3) )+" "+"'"+
					table.getStringValue(3,1)+"'");
			System.arraycopy(	queryStatement.toCharArray(), 0,
				qVal[SRBMetaDataSet.getSRBID( fieldName+"1_4" )],	0,
				queryStatement.length());
		}


			//5th row, users and resources don't have fifth row.
		if (fieldName.equals( SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES ) ||
			fieldName.equals( SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES ))
		{
			if (table.getRowCount() >= 5) {
				queryStatement = new String( "= '"+table.getStringValue(4,0)+"'");
				System.arraycopy(	queryStatement.toCharArray(), 0,
					qVal[SRBMetaDataSet.getSRBID( fieldName+"0_5" )],	0,
					queryStatement.length());

				queryStatement = new String(
					getOperator( table.getOperator(4) )+" '"+
						table.getStringValue(4,1)+"'");
				System.arraycopy(	queryStatement.toCharArray(), 0,
					qVal[SRBMetaDataSet.getSRBID( fieldName+"1_5" )],	0,
					queryStatement.length());
			}
		}
	}


	//for the qVal when the conditions contain a table
	//new query protocol
	private String setTableGenQuery( MetaDataTable table, String fieldName )
	{
		if (table == null)
			return "";

			//1st row
		String queryStatement = new String( "= '"+table.getStringValue(0,0)+"'\u0000");
		SRBMetaDataSet.getSRBID( fieldName+"0_1" );


		queryStatement += new String(
			getOperator( table.getOperator(0) )+" '"+table.getStringValue(0,1)+"'\u0000");
		SRBMetaDataSet.getSRBID( fieldName+"1_1" );


			//2nd row
		if (table.getRowCount() >= 2) {
			queryStatement +=	new String(	"= '"+table.getStringValue(1,0)+"'\u0000");
			SRBMetaDataSet.getSRBID( fieldName+"0_2" );

			queryStatement +=	new String(
				getOperator( table.getOperator(1) )+" '"+table.getStringValue(1,1)+"'\u0000");
			SRBMetaDataSet.getSRBID( fieldName+"1_2" );
		}

			//3rd row
		if (table.getRowCount() >= 3) {
			queryStatement += new String( "= '"+table.getStringValue(2,0)+"'\u0000");
			SRBMetaDataSet.getSRBID( fieldName+"0_3" );

			queryStatement += new String(
				getOperator( table.getOperator(2) )+" '"+table.getStringValue(2,1)+"'\u0000");
			SRBMetaDataSet.getSRBID( fieldName+"1_3" );
		}

			//4th row
		if (table.getRowCount() >= 4) {
			queryStatement += new String( "= '"+table.getStringValue(3,0)+"'\u0000");
			SRBMetaDataSet.getSRBID( fieldName+"0_4" );

			queryStatement += new String(
				getOperator( table.getOperator(3) )+" "+"'"+table.getStringValue(3,1)+"'\u0000");
			SRBMetaDataSet.getSRBID( fieldName+"1_4" );
		}


			//5th row, users and resources don't have fifth row.
		if (fieldName.equals( SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES ) ||
			fieldName.equals( SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES ))
		{
			if (table.getRowCount() >= 5) {
				queryStatement += new String( "= '"+table.getStringValue(4,0)+"'\u0000");
				SRBMetaDataSet.getSRBID( fieldName+"0_5" );

				queryStatement += new String(
					getOperator( table.getOperator(4) )+" '"+table.getStringValue(4,1)+"'\u0000");
				SRBMetaDataSet.getSRBID( fieldName+"1_5" );
			}
		}

		return queryStatement;
	}

	//To set the ID value early enough in the new query protocol
	private byte[] setTableConditionIDs( MetaDataTable table, String fieldName )
	{
			//1st row
		Vector ids = new Vector(10);
		byte[] idsArray;
		ids.add( new Integer(SRBMetaDataSet.getSRBID( fieldName+"0_1" )));
		ids.add( new Integer(SRBMetaDataSet.getSRBID( fieldName+"1_1" )));


			//2nd row
		if (table.getRowCount() >= 2) {
			ids.add( new Integer(SRBMetaDataSet.getSRBID( fieldName+"0_2" )));
			ids.add( new Integer(SRBMetaDataSet.getSRBID( fieldName+"1_2" )));
		}

			//3rd row
		if (table.getRowCount() >= 3) {
			ids.add( new Integer(SRBMetaDataSet.getSRBID( fieldName+"0_3" )));
			ids.add( new Integer(SRBMetaDataSet.getSRBID( fieldName+"1_3" )));
		}

			//4th row
		if (table.getRowCount() >= 4) {
			ids.add( new Integer(SRBMetaDataSet.getSRBID( fieldName+"0_4" )));
			ids.add( new Integer(SRBMetaDataSet.getSRBID( fieldName+"1_4" )));
		}


			//5th row, users and resources don't have fifth row.
		if (fieldName.equals( SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES ) ||
			fieldName.equals( SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES ))
		{
			if (table.getRowCount() >= 5) {
				ids.add( new Integer(SRBMetaDataSet.getSRBID( fieldName+"0_5" )));
				ids.add( new Integer(SRBMetaDataSet.getSRBID( fieldName+"1_5" )));
			}
		}

		idsArray = new byte[ids.size()*SRBCommands.INT_LENGTH];
		Host.copyInt( ((Integer) ids.get(0)).intValue(), idsArray, 0 );

		for (int i=1;i<ids.size();i++) {
			Host.copyInt( ((Integer) ids.get(i)).intValue(), idsArray,
				i*SRBCommands.INT_LENGTH );
		}		return idsArray;
	}


	/**
	 * The definable metadata requires the merging of two queries.
	 * If the results of the query have the same ID (by comparators),
	 * the data from both queries is joined into one MetaDataRecordList.
	 * Otherwise the MetaDataRecordList array is just expanded.
	 */
	SRBMetaDataRecordList[] joinRecordLists(
		SRBMetaDataRecordList[] firstRecordList,
		SRBMetaDataRecordList[] secondRecordList,
		String comparator, String comparator2 )
	{
		int i=0,j=0,k=0;

		boolean[] nullValues = null;
		int totalNull = 0;
		SRBMetaDataRecordList[] totalRecordList = null;
		if ((comparator != null) && (firstRecordList != null) &&
			(secondRecordList != null))
		{
			nullValues = new boolean[secondRecordList.length];
			for (i=0;i<firstRecordList.length;i++) {
				int matchingID = -1;
				int matchingID2 = -1;
				//TODO? ha, what? I was really sleepy.
				for (j=0;j<firstRecordList[i].getFieldCount();j++) {
					if (firstRecordList[i].getField(j).equals(MetaDataSet.getField(comparator))) {
						for (k=0;k<secondRecordList.length;k++) {
							for (int l=0;l<secondRecordList[k].getFieldCount();l++) {
								if (firstRecordList[i].getValue(j).equals(secondRecordList[k].getValue(l) ) ) {
									if (secondRecordList[k].getField(l).equals(MetaDataSet.getField(comparator)) ) {
										matchingID = k;
										if (comparator2 != null) {
											for (int jj=0;jj<firstRecordList[i].getFieldCount();jj++) {
												if (firstRecordList[i].getField(jj).equals(MetaDataSet.getField(comparator2)))	{
													for (int ll=0;ll<secondRecordList[k].getFieldCount();ll++) {
														if (firstRecordList[i].getValue(jj).equals(secondRecordList[k].getValue(ll) ) ) {
															matchingID2 = k;
														}
													}
												}
											}
										}
									}
								}
							}
						}
					}
				}

				if ( (matchingID >= 0) && ((comparator2 == null) ||
					((comparator2 != null) && (matchingID == matchingID2))) ) {
					firstRecordList[i].combineRecordLists( secondRecordList[matchingID] );
					nullValues[matchingID] = true;
					totalNull++;
				}
			}
			if (totalNull > secondRecordList.length) {
				//All in second were in first, so just return first.
				totalRecordList = firstRecordList;
			}
			else {
				totalRecordList = new SRBMetaDataRecordList[
					firstRecordList.length + secondRecordList.length - totalNull];

				System.arraycopy(	firstRecordList, 0, totalRecordList,
					0, firstRecordList.length);

				System.arraycopy(	secondRecordList, 0, totalRecordList,
					firstRecordList.length, secondRecordList.length-totalNull);

				if (nullValues != null) {
					for (i=0,k=0;i<secondRecordList.length;i++) {
						if (!nullValues[i]) {
							totalRecordList[firstRecordList.length+k] = secondRecordList[i];
							k++;
						}
					}
				}
			}
			return totalRecordList;
		}
		else if ((comparator == null) && (firstRecordList != null) &&
			(secondRecordList != null))
		{
			totalRecordList = new SRBMetaDataRecordList[
				firstRecordList.length + secondRecordList.length - totalNull];

			System.arraycopy(	firstRecordList, 0, totalRecordList,
				0, firstRecordList.length);

			System.arraycopy(	secondRecordList, 0, totalRecordList,
				firstRecordList.length, secondRecordList.length-totalNull);

			return totalRecordList;
		}

		if (firstRecordList == null) {
			return secondRecordList;
		}
		else if (secondRecordList == null) {
			return firstRecordList;
		}
		else {
			return null;
		}
	}


	/**
	 * Two queries need to be done then joined to create the result.
	 * The first gets the unique IDs. It also get those files that
	 * match the conditional but do not have definable metadata.
	 * The second query gets the files that match and have
	 * definable metadata. The rows wanted from the second query
	 * will not equal the rows received because of the MetaDataTable.
	 * So all the results from both queries will have to be obtained
	 * before the results can be joined.
	 */
	private SRBMetaDataRecordList[] getRecordsForDefinableQueries( int catType,
		MetaDataCondition[] conditions, MetaDataSelect[] selects,
		int rowsWanted,	SRBMetaDataRecordList[] firstRecordList,
		boolean orderBy, boolean nonDistinct )
		throws IOException
	{
		SRBMetaDataRecordList[] secondRecordList = null;

		//Multiplying the rows wanted by 100 speeds things up.
//		if (rowsWanted < 100) rowsWanted *= 10;

		secondRecordList = srbGetDataDirInfo( catType, conditions,
			selects, rowsWanted, orderBy, nonDistinct );

		if (secondRecordList != null) {
//TODO should use?
/*			for (int i=0;i<secondRecordList.length;i++) {
				int continuationIndex = secondRecordList[i].getContinuationIndex();
				if (continuationIndex >= 0) {
					secondRecordList = joinRecordLists( secondRecordList,
						commands.srbGetMoreRows( catType, continuationIndex,	rowsWanted ),
						null, null );
				}
			}
*/
			for (int i=0;i<secondRecordList.length;i++) {
				secondRecordList[i].setContinuationIndex(-1);
			}

			if (( firstRecordList != null) && ( secondRecordList != null )) {
				secondRecordList = joinRecordLists( firstRecordList, secondRecordList,
					null, null );
			}
			return secondRecordList;
		}
		else
			return null;
	}


	/**
	 * Takes an array and removes null and duplicate values
	 */
	static final Object[] cleanArray( Object[] obj )
	{
		Vector temp = new Vector(obj.length);
		boolean add = true;
		int i=0,j=0;

		for (i=0;i<obj.length;i++) {
			if (obj[i] != null) {
				for (j=i+1;j<obj.length;j++) {
					if (obj[i].equals(obj[j])){
						add = false;
						j = obj.length;
					}
				}

				if (add) {
					temp.add(obj[i]);
				}
				add = true;
			}
		}

		//need a loop in case obj[0] is null
		for (i=0;i<obj.length;i++) {
			if (obj[i] != null)
				return temp.toArray((Object[]) Array.newInstance(obj[i].getClass(), 0));
		}
		return null;
	}


	/**
	 * each query style has to be handled differently
	 * first the consequent (value) of the conditional is formed.
	 * @param style whether the condition is scalar, rangepair, enum, etc
	 * @param the WHERE value to be converted
	 * @param need to pass along in case the style is MetaDataCondition.TABLE
	 */
	private String addValueByStyle( int style, MetaDataCondition condition,
		char qVal[][] )
	{
		String qValCondition = "";
		switch (style) {
			case MetaDataCondition.SCALAR:
				switch(condition.getFieldType()) {
					case MetaDataField.STRING:
					case MetaDataField.DATE:
						qValCondition +=
							"'" + condition.getStringValue() + "'";
						break;
					case MetaDataField.INT:
						qValCondition += condition.getIntValue();
						break;
					case MetaDataField.FLOAT:
						qValCondition += condition.getFloatValue();
						break;
					default:
						throw new RuntimeException(
							"TODO temp SRBMetaDataCommands.getdatadir,type,scalar" );
				}
				break;
			//eg.
			case MetaDataCondition.RANGEPAIR:
				//The operator is always BETWEEN or NOT_BETWEEN
				switch(condition.getFieldType()) {
					case MetaDataField.STRING:
					case MetaDataField.DATE:
						qValCondition +=
							"'" + condition.getStringValue( 0 ) + "' and "+
							"'" + condition.getStringValue( 1 ) + "'";
						break;
					case MetaDataField.INT:
						qValCondition +=
							condition.getIntValue( 0 ) + " and "+
							condition.getIntValue( 1 );
						break;
					case MetaDataField.FLOAT:
						qValCondition +=
							condition.getFloatValue( 0 ) + " and "+
							condition.getFloatValue( 1 );
						break;
					default:
						throw new RuntimeException(
							"TODO temp SRBMetaDataCommands.getdatadir,type,range" );
				}
				break;
			case MetaDataCondition.ENUM:
				//The operator is always IN or NOT_IN
				switch(condition.getFieldType()) {
					case MetaDataField.STRING:
					case MetaDataField.DATE:
						qValCondition += "(";
						for (int i=0; i<condition.getCount(); i++) {
							qValCondition +=
								"'" + condition.getStringValue( i ) + "',";
						}
						break;
					case MetaDataField.INT:
						qValCondition += "(";
						for (int i=0; i<condition.getCount(); i++) {
							qValCondition +=
								condition.getIntValue( i ) + ",";
						}
						break;
					case MetaDataField.FLOAT:
						qValCondition += "(";
						for (int i=0; i<condition.getCount(); i++) {
							qValCondition +=
								condition.getFloatValue( i ) + ",";
						}
						break;
					default:
						throw new RuntimeException(
							"TODO temp SRBMetaDataCommands.getdatadir,type,enum" );
				}
				//delete the last ',' and close the paraenthesis.
				qValCondition =
					qValCondition.substring(0,qValCondition.length()-1) + ")";
				break;
			case MetaDataCondition.TABLE:
				//random notes
				//SRB user defined metadata, API treats as one table.
				//SRB only handles user defined metadata as strings

				MetaDataTable table = condition.getTableValue();
				if (qVal == null) {
					qValCondition += setTableGenQuery( table,	condition.getFieldName() );
				}
				else {
					setTableQval( table, qVal, condition.getFieldName() );
				}
				if (table.getRowCount() > 5) {
/*TODO talk to raja
						table.removeRow(0);
						table.removeRow(1);
						table.removeRow(2);
						table.removeRow(3);
					//users and resources don't have fifth row.
					if ((fieldName.equals( SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES ) ||
						(fieldName.equals( SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES ))
					{
						table.removeRow(4);
					}
					condition = MetaDataSet.newCondition(
						condition.getFieldName(), table );
					srbGetDataDirInfo( catType, conditions, selects, rowsWanted);
*/
				}
				break;
			default:
				throw new RuntimeException(
					"TODO temp SRBMetaDataCommands.getdatadir,style" );
		}
		return qValCondition;
	}


	/**
	 * Get metadata info by querying the MDAS catalog.
	 * The server uses the input qval[][] and selval[] array to compose
	 * and execute SQL queries and returns the query result in
	 * SRBMetaDataRecordList. The selval[] array specifies a list of attrbutes
	 * to retrieve and qval[][] specifies a lists of "=" predicates to search.
	 * Both selval[] and qval[][] must be arrays of size SRBFileSystem.TOTAL_METADATA_ATTRIBUTESand
	 * are indexed by values given in mdasC_db2_externs.h under the heading
	 * DCS-ATTRIBUTE-INDEX DEFINES.
	 *<P>
	 * For the selval[] array, setting an element of the array to 1 means that
	 * the attribute associated with this element is to be retrieved. e.g.,
	 * selval[USER_NAME] = 1; means the "user_name" attribute is to be retrieved.
	 *<P>
	 * The qval[][] array specifies the "=" predicates to search. e.g.,
	 * sprintf(qval[FILE_NAME],"'%s'", "unixFileObj1"); means that the search
	 * condition includes the term (data_name = "unixFileObj1").
	 *<P>
	 * An example of srbGetDataDirInfo:
	 *<br>
	 *  mdasC_sql_result_struct srbMetaDataRecordList;
	 *  char qval[SRBFileSystem.TOTAL_METADATA_ATTRIBUTES][MAX_TOKEN];
	 *  int selval[SRBFileSystem.TOTAL_METADATA_ATTRIBUTES];
	 *<P>
	 * for (i = 0; i < SRBFileSystem.TOTAL_METADATA_ATTRIBUTES; i++) {
	 *      selval[i] = 0;
	 *      sprintf(qval[i],"");
	 *  }
	 *  sprintf(qval[FILE_NAME],"'%s'",argv[2]);
	 *  selval[PATH_NAME] = 1;
	 *  selval[RESOURCE_NAME] = 1;
	 *	.
	 *	.
	 *  if (srbGetDataDirInfo(
	 *	 conn, MDAS_CATALOG, qval, selval, &srbMetaDataRecordList) < 0)
	 *	{
	 *		fprintf(stderr, "can't srbGetDataDirInfo \n");
	 *		exit_nicely(conn);
	 *  }
	 *
	 *
	 * @param catType catalog type. e.g., MDAS_CATALOG.
	 *
	 * @param metaDataConditions[] The user version of the SRB
	 * 		qval[SRBFileSystem.TOTAL_METADATA_ATTRIBUTES][MAX_TOKEN] array.
	 *
	 * @param metaDataSelects[] The user version of the SRB
	 *		selval[SRBFileSystem.TOTAL_METADATA_ATTRIBUTES] array.
	 * @param rowsWanted number of rows of result wanted.
	 *
	 * @return SRBMetaDataRecordList. Use srbGetMoreRows() to retrieve more rows.
	 */
//TODO internal comments, cruft
//orderBy and nonDistinct are only used in SRB3.0 and thus only in genQuery
	synchronized SRBMetaDataRecordList[] srbGetDataDirInfo( int catType,
		MetaDataCondition[] metaDataConditions, MetaDataSelect[] metaDataSelects,
		int rowsWanted, boolean orderBy, boolean nonDistinct )
		throws IOException
	{
//System.out.println("\n srbGetDataDirInfo");

		String version = SRBAccount.getVersion();
		if ((!version.equals(SRBAccount.SRB_VERSION_1_1_8)) &&
			(!version.equals(SRBAccount.SRB_VERSION_2)))
		{
			//TODO zone
			return srbGenQuery(catType, commands.getZone(), metaDataConditions,
				metaDataSelects, rowsWanted, orderBy, nonDistinct);
		}

  	int i=0, j=0, k=0;
		int conditionsLength = metaDataConditions.length;
		int selectsLength = metaDataSelects.length;

		//make duplicates, to leave original unmodified
		MetaDataCondition[] conditions = new MetaDataCondition[conditionsLength];
		MetaDataSelect[] selects = new MetaDataSelect[selectsLength];
		System.arraycopy(	metaDataConditions, 0, conditions, 0, conditionsLength);
		System.arraycopy(	metaDataSelects, 0, selects, 0, selectsLength);


		//remove duplicate/null conditions.
		conditions = (MetaDataCondition[]) cleanArray( conditions );
		if (conditions == null) return null;
		conditionsLength = conditions.length;

		//remove duplicate/null selects.
		selects = (MetaDataSelect[]) cleanArray( selects );
		if (selects == null) return null;
		selectsLength = selects.length;

		try {
			char qVal[][] = new
				char[SRBFileSystem.TOTAL_METADATA_ATTRIBUTES][SRBFileSystem.MAX_TOKEN];
			int selVal[] = new int[SRBFileSystem.TOTAL_METADATA_ATTRIBUTES];
			String queryStatement;
			int fieldID;
			SRBMetaDataRecordList[] firstRecordList = null;
			SRBMetaDataRecordList[] secondRecordList = null;
			SRBMetaDataRecordList[] totalRecordList = null;


			//**************************************
			//Create the predefined SRB qVal char[][] array.
			//used for WHERE
			//which looks something like:
			//qVal[25] = " = 'value'";
			//qVal[85] = " in ('a','b','c')";
			//qVal[98] = " <> 55";
			//string values must be in single quotes.
			//operator strings (=, <>, in) are found by getOperator(...);
			for (i=0; i<conditionsLength; i++) {
				if (conditions[i] != null) {
					int style = conditions[i].getStyle();
					queryStatement = "";
					fieldID = SRBMetaDataSet.getSRBID( conditions[i].getFieldName() );

					if (style != MetaDataField.TABLE) {
						queryStatement = getOperator(conditions[i]) + " " +queryStatement;
					}
					queryStatement += addValueByStyle( style, conditions[i], qVal );

					if (style != MetaDataField.TABLE) {
						System.arraycopy(	queryStatement.toCharArray(), 0,
							qVal[fieldID],	0, queryStatement.length() );
					}
				}
			}


			//**************************************
			//create the SRB selval int[] array, used for SELECT
			//
			int continuationIndex = -1;
			String comparator = null;
			String comparator2 = null;
			//Create the predefined SRB selVal integer array
			for (i=0; i<selectsLength; i++) {
				if ( selects[i] != null ) {
					fieldID = SRBMetaDataSet.getSRBID( selects[i].getFieldName() );
					//= 1; the byte order is reversed.

					if (fieldID == -1) {
						//**************************************
						//select DEFINABLE_METADATA_FOR_FILES
						//Two queries need to be done then joined to create the result.
						//The first gets the unique IDs. It also gets those files that
						//match the conditional but do not have definable metadata.
						//The second query gets the files that match and have
						//definable metadata. The rows wanted from the second query
						//will not equal the rows received because of the MetaDataTable.
						//So all the results from both queries will have to be obtained
						//before the results can be combined.

						//The first query
						selects[i] = MetaDataSet.newSelection(SRBMetaDataSet.FILE_NAME);
						selects = MetaDataSet.mergeSelects(	MetaDataSet.newSelection(
							SRBMetaDataSet.DIRECTORY_NAME), selects );

						secondRecordList = getRecordsForDefinableQueries( catType,
							conditions, selects, rowsWanted, firstRecordList, orderBy, nonDistinct );

						selVal = new int[SRBFileSystem.TOTAL_METADATA_ATTRIBUTES];
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES0)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES1)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES2)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES3)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES4)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES5)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES6)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES7)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES8)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES9)] = 16777216;
						//also need to be sure to return FILE_NAME and DIRECTORY_NAME
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.FILE_NAME)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DIRECTORY_NAME)] = 16777216;
						comparator = SRBMetaDataSet.FILE_NAME;
						comparator2 = SRBMetaDataSet.DIRECTORY_NAME;
						break;
					}
					else if (fieldID == -2) {
						selects[i] = MetaDataSet.newSelection(SRBMetaDataSet.DIRECTORY_NAME);

						secondRecordList = getRecordsForDefinableQueries( catType,
							conditions, selects, rowsWanted, firstRecordList, orderBy, nonDistinct );

						selVal = new int[SRBFileSystem.TOTAL_METADATA_ATTRIBUTES];
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES0)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES1)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES2)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES3)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES4)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES5)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES6)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES7)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES8)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES9)] = 16777216;
						//also need to be sure to return DIRECTORY_NAME
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DIRECTORY_NAME)] = 16777216;
						break;
					}
					else if (fieldID == -3) {
//TODO USER_ID? how does this work?
//old						selects[i] = MetaDataSet.newSelection(SRBMetaDataSet.USER_ID);

						selects[i] = MetaDataSet.newSelection(SRBMetaDataSet.USER_NAME);
						selects = MetaDataSet.mergeSelects(	MetaDataSet.newSelection(
							SRBMetaDataSet.USER_DOMAIN), selects );

						secondRecordList = getRecordsForDefinableQueries( catType, conditions, selects,
							rowsWanted, firstRecordList, orderBy, nonDistinct );

						selVal = new int[SRBFileSystem.TOTAL_METADATA_ATTRIBUTES];
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS0)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS1)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS2)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS3)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS4)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS5)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS6)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS7)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS8)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS9)] = 16777216;
						//also need to be sure to return USER_NAME and USER_DOMAIN
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.USER_NAME)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.USER_DOMAIN)] = 16777216;
						break;
					}
					else if (fieldID == -4) {
						selects[i] = MetaDataSet.newSelection(SRBMetaDataSet.RESOURCE_NAME);

						secondRecordList = getRecordsForDefinableQueries( catType, conditions, selects,
							rowsWanted, firstRecordList, orderBy, nonDistinct );

						selVal = new int[SRBFileSystem.TOTAL_METADATA_ATTRIBUTES];
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES0)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES1)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES2)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES3)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES4)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES5)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES6)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES7)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES8)] = 16777216;
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES9)] = 16777216;
						//also need to be sure to return RESOURCE_NAME
						selVal[SRBMetaDataSet.getSRBID(SRBMetaDataSet.RESOURCE_NAME)] = 16777216;
						break;
					}
					else {
						selVal[fieldID] = 16777216;
					}
				}
			}
/*
//for debuging, print qval and selval
System.out.print("\nqVal.length "+qVal.length+" qVal[0].length "+qVal[0].length);
for (i=0;i<qVal.length;i++){
	if (qVal[i][0] != 0)
		System.out.print(" "+i+":");
	for (j=0;j<qVal[i].length;j++){
		if (qVal[i][j] != 0)
			System.out.print(qVal[i][j]);
	}
}
System.out.println("\nselVal "+selVal.length);
for(i=0;i<selVal.length;i++){
	if (selVal[i] != 0)
		System.out.print(" "+i+"="+selVal[i]);
}
*/
//System.out.println("rowsWanted "+rowsWanted);



			commands.startSRBCommand( commands.F_SRBO_GET_DATADIR_INFO, 4 );
			commands.sendArg(catType);
			commands.sendArg(qVal);
			commands.sendArg(selVal);
			commands.sendArg(rowsWanted);
			commands.flush( );

			commands.commandStatus();
			firstRecordList = commands.returnSRBMetaDataRecordList();







/*
byte resultBuffer[]=commands.read(4);
System.out.print("\n error: \n");
for (k=0;k<resultBuffer.length;k++)
{
System.out.print(resultBuffer[k]);
System.out.print("="+(char)resultBuffer[k]+"\n");
}
System.out.print("\n");
int errorNum = (int) commands.readUnsignedInt();
System.out.print("error: "+errorNum);
*/
/*
byte[] zxcv = commands.read(1);
for (int ii=0;ii<1000000;ii++) {
zxcv = commands.read(1);
System.out.print(" "+(char) zxcv[0]);
}
*/
			if ( secondRecordList != null ) {
				if ( firstRecordList != null ) {
					for (j=0;j<firstRecordList.length;j++) {
						if (continuationIndex < firstRecordList[j].getContinuationIndex())
						{
							continuationIndex = firstRecordList[j].getContinuationIndex();
							firstRecordList = joinRecordLists( firstRecordList,
								commands.srbGetMoreRows( catType, continuationIndex,
									rowsWanted ),	comparator, comparator2 );
						}
					}
					for (j=0;j<firstRecordList.length;j++) {
						firstRecordList[j].setContinuationIndex(-1);
					}
				}
				totalRecordList = joinRecordLists( secondRecordList, firstRecordList,
					comparator, comparator2 );
			}
			else {
				totalRecordList = firstRecordList;
			}



/*
System.out.println("totalRecordList "+totalRecordList);
if (totalRecordList != null) {
	for (j=0;j<totalRecordList.length;j++) {
		for (k=0;k<totalRecordList[j].getRecordCount();k++) {
			Object asdf = totalRecordList[j].getValue(k);
			System.out.println(asdf);
			if (asdf.toString().indexOf( "MetaDataTable" ) >= 0) {
//				for (i=0;i<asdf.getRowCount(),i++) {
					System.out.println(((MetaDataTable) asdf).getStringValue(0,0));
					System.out.println(((MetaDataTable) asdf).getStringValue(0,1));
//				}
			}
		}
	}
}
*/
//if (totalRecordList != null)
//	System.out.println("total.contInx "+totalRecordList[0].getContinuationIndex());

			return totalRecordList;
		}
		catch( SRBException e ) {
			if (e.getType() == -3005) {
				//the database contains no objects matching the query.
				return null;
			}
			else
				throw e;
		}
	}


	/**TODO
	 * This is a more compact form of srbGetDataDirInfo.
	 * Instead of using fixed array of selval and qval, it uses the
	 * genQuery struct.
	 *
	 *
	 * @param catType The catalog type, 0 = MDAS_CATALOG
	 * @param myMcatZone The MCAT zone for this query. Only valid
	 *          for version 3.x.x and above. For version 2.x.x, this should be NULL.
	 * @param genQuery_t *myGenQuery - The input query in the form of genQuery_t
	 *           instead of selval and qval of srbGetDataDirInfo.
	 * @param myresult The output query result.
	 * @param rowsWanted The number of rows of result wanted.
	 *
	 * @return The query result, use srbGenGetMoreRows() to retrieve more rows.
	 */
	SRBMetaDataRecordList[] srbGenQuery( int catType, String myMcatZone,
		MetaDataCondition[] metaDataConditions,	MetaDataSelect[] metaDataSelects,
		int rowsWanted, boolean orderBy, boolean nonDistinct )
		throws IOException
	{
		if (SRBCommands.DEBUG > 0) System.out.println("\n srbGenQuery");


		/*
		The conversion of the query in the SRB query protocol
		takes the following form:
			int ? = total length of the packed query? zero works...
			int selectsLength = a count of the selects to be sent
			int conditionsLength = a count of the conditions to be sent
			int[selectsLength] selectsIDs = an int ID# for every select
			int[conditionsLength] conditionIDs = an int ID# for every condition
			int[selectsLength] selectsValues = an int value for every select
			String[conditionsLength] conditionValues = a null-term String for every condition

		for example:
			? 									0x0000
			selectsLength 			0x0001
			conditionsLength 		0x0002
			selectsIDs[0]				0x0012
			selectsIDs[0]				0x001D
			selectsValues[0]		0x0001
			selectsValues[0]		0x0001
			conditionIDs[0]			0x0002
			conditionIDs[1]			0x000F
			conditionValue[0]		 = 'myJARGONMetaDataTestFile'\u0000
			conditionValue[1]		 = '/demozone/home/testuser.sdsc'\u0000
		*/



		int i=0, j=0, temp=0;

		String queryStatement;
		int fieldID;
		SRBMetaDataRecordList[] firstRecordList = null;
		SRBMetaDataRecordList[] secondRecordList = null;
		SRBMetaDataRecordList[] totalRecordList = null;

		//the byte string sent to the SRB as the query
//TODO size
byte[] packedQuery = new byte[999];



		int conditionsLength = metaDataConditions.length;
		int selectsLength = metaDataSelects.length;

		//make duplicates, to leave original unmodified
		MetaDataCondition[] conditions = new MetaDataCondition[conditionsLength];
		MetaDataSelect[] selects = new MetaDataSelect[selectsLength];
		System.arraycopy(	metaDataConditions, 0, conditions, 0, conditionsLength);
		System.arraycopy(	metaDataSelects, 0, selects, 0, selectsLength);




		//remove duplicate/null conditions.
		conditions = (MetaDataCondition[]) cleanArray( conditions );
		if (conditions == null) return null;
		conditionsLength = conditions.length;

		//remove duplicate/null selects.
		selects = (MetaDataSelect[]) cleanArray( selects );
		if (selects == null) return null;
		selectsLength = selects.length;
		int[] selectsIDs = new int[selectsLength];
		int[] selectsValues = new int[selectsLength];




		//Set the proper conditionsLength, when MetaDataTable
		for (i=0;i<conditions.length;i++) {
if (SRBCommands.DEBUG > 3) System.out.println("conditions		 "+conditions[i].getFieldName() +" "+conditions[i].getStringValue());
			if (conditions[i].getStyle() == MetaDataCondition.TABLE) {
				int rowCount = conditions[i].getTableValue().getRowCount();
				if (rowCount >= 5) {
					//TODO 5 rows max, users and resources don't have fifth row.
					if (
						conditions[i].getFieldName().equals(
							SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES ) ||
						conditions[i].getFieldName().equals(
							SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES ))
					{
						conditionsLength +=
							5*conditions[i].getTableValue().getColumnCount()-1;
					}
					else {
						conditionsLength +=
							4*conditions[i].getTableValue().getColumnCount()-1;
					}
				}
				else {
					//the table condition is already included, so rows*columns minus one
					conditionsLength +=
						rowCount*conditions[i].getTableValue().getColumnCount()-1;
				}
			}
		}
//TODO hey don't only column 0 and 1 work? that's a raja thing isn't it?




		//**************************************
		//create the SRB selval int[] array, used for SELECT
		//
		int continuationIndex = -1;
		String comparator = null;
		String comparator2 = null;
		//Create the predefined SRB selVal integer array
		temp = selectsLength;
		for (i=0; i<temp; i++) {
			fieldID = SRBMetaDataSet.getSRBID( selects[i].getFieldName() );
			//= 1; the byte order is reversed.

			if (fieldID == -1) {
				//**************************************
				//select DEFINABLE_METADATA_FOR_FILES
				//Two queries need to be done then joined to create the result.
				//The first gets the unique IDs. It also gets those files that
				//match the conditional but do not have definable metadata.
				//The second query gets the files that match and have
				//definable metadata. The rows wanted from the second query
				//will not equal the rows received because of the MetaDataTable.
				//So all the results from both queries will have to be obtained
				//before the results can be combined.

				//The first query
				selects[i] = MetaDataSet.newSelection(SRBMetaDataSet.FILE_NAME);
				selects = MetaDataSet.mergeSelects(	MetaDataSet.newSelection(
					SRBMetaDataSet.DIRECTORY_NAME), selects );

				secondRecordList = getRecordsForDefinableQueries( catType,
					conditions, selects, rowsWanted, firstRecordList, orderBy, nonDistinct );

				selectsLength = 12;
				selectsIDs = new int[selectsLength];
				selectsValues = new int[selectsLength];

				//need to be sure to return FILE_NAME and DIRECTORY_NAME
				selectsIDs[0] = SRBMetaDataSet.getSRBID(SRBMetaDataSet.FILE_NAME);
				selectsIDs[1] = SRBMetaDataSet.getSRBID(SRBMetaDataSet.DIRECTORY_NAME);
				selectsIDs[2] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES0);
				selectsIDs[3] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES1);
				selectsIDs[4] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES2);
				selectsIDs[5] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES3);
				selectsIDs[6] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES4);
				selectsIDs[7] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES5);
				selectsIDs[8] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES6);
				selectsIDs[9] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES7);
				selectsIDs[10] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES8);
				selectsIDs[11] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_FILES9);
				selectsValues[0] = 1;
				selectsValues[1] = 1;
				selectsValues[2] = 1;
				selectsValues[3] = 1;
				selectsValues[4] = 1;
				selectsValues[5] = 1;
				selectsValues[6] = 1;
				selectsValues[7] = 1;
				selectsValues[8] = 1;
				selectsValues[9] = 1;
				selectsValues[10] = 1;
				selectsValues[11] = 1;

				comparator = SRBMetaDataSet.FILE_NAME;
				comparator2 = SRBMetaDataSet.DIRECTORY_NAME;
				i = temp;
			}
			else if (fieldID == -2) {
				selects[i] = MetaDataSet.newSelection(SRBMetaDataSet.DIRECTORY_NAME);

				secondRecordList = getRecordsForDefinableQueries( catType,
					conditions, selects, rowsWanted, firstRecordList, orderBy, nonDistinct );

				selectsLength = 11;
				selectsIDs = new int[selectsLength];
				selectsValues = new int[selectsLength];

				//need to be sure to return DIRECTORY_NAME
				selectsIDs[0] = SRBMetaDataSet.getSRBID(SRBMetaDataSet.DIRECTORY_NAME);
				selectsIDs[1] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES0);
				selectsIDs[2] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES1);
				selectsIDs[3] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES2);
				selectsIDs[4] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES3);
				selectsIDs[5] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES4);
				selectsIDs[6] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES5);
				selectsIDs[7] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES6);
				selectsIDs[8] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES7);
				selectsIDs[9] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES8);
				selectsIDs[10] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_DIRECTORIES9);
				selectsValues[0] = 1;
				selectsValues[1] = 1;
				selectsValues[2] = 1;
				selectsValues[3] = 1;
				selectsValues[4] = 1;
				selectsValues[5] = 1;
				selectsValues[6] = 1;
				selectsValues[7] = 1;
				selectsValues[8] = 1;
				selectsValues[9] = 1;
				selectsValues[10] = 1;

				//TODO don't? can't remember why not
				//comparator = SRBMetaDataSet.DIRECTORY_NAME;
				i = temp;
			}
			else if (fieldID == -3) {
//TODO USER_ID? how does this work?
//old				selects[i] = MetaDataSet.newSelection(SRBMetaDataSet.USER_ID);

				selects[i] = MetaDataSet.newSelection(SRBMetaDataSet.USER_NAME);
				selects = MetaDataSet.mergeSelects(	MetaDataSet.newSelection(
					SRBMetaDataSet.USER_DOMAIN), selects );

				secondRecordList = getRecordsForDefinableQueries( catType, conditions, selects,
					rowsWanted, firstRecordList, orderBy, nonDistinct );

				selectsLength = 12;
				selectsIDs = new int[selectsLength];
				selectsValues = new int[selectsLength];

				//need to be sure to return USER_NAME and USER_DOMAIN
				selectsIDs[0] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.USER_NAME);
				selectsIDs[1] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.USER_DOMAIN);
				selectsIDs[2] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS0);
				selectsIDs[3] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS1);
				selectsIDs[4] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS2);
				selectsIDs[5] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS3);
				selectsIDs[6] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS4);
				selectsIDs[7] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS5);
				selectsIDs[8] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS6);
				selectsIDs[9] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS7);
				selectsIDs[10] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS8);
				selectsIDs[11] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_USERS9);
				selectsValues[0] = 1;
				selectsValues[1] = 1;
				selectsValues[2] = 1;
				selectsValues[3] = 1;
				selectsValues[4] = 1;
				selectsValues[5] = 1;
				selectsValues[6] = 1;
				selectsValues[7] = 1;
				selectsValues[8] = 1;
				selectsValues[9] = 1;
				selectsValues[10] = 1;
				selectsValues[11] = 1;

				//TODO don't? can't remember why not
				//comparator =
				i = temp;
			}
			else if (fieldID == -4) {
				selects[i] = MetaDataSet.newSelection(SRBMetaDataSet.RESOURCE_NAME);

				secondRecordList = getRecordsForDefinableQueries( catType, conditions, selects,
					rowsWanted, firstRecordList, orderBy, nonDistinct );

				selectsLength = 11;
				selectsIDs = new int[selectsLength];
				selectsValues = new int[selectsLength];

				//need to be sure to return RESOURCE_NAME
				selectsIDs[0] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.RESOURCE_NAME);
				selectsIDs[1] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES0);
				selectsIDs[2] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES1);
				selectsIDs[3] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES2);
				selectsIDs[4] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES3);
				selectsIDs[5] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES4);
				selectsIDs[6] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES5);
				selectsIDs[7] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES6);
				selectsIDs[8] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES7);
				selectsIDs[9] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES8);
				selectsIDs[10] = SRBMetaDataSet.getSRBID(
					SRBMetaDataSet.DEFINABLE_METADATA_FOR_RESOURCES9);
				selectsValues[0] = 1;
				selectsValues[1] = 1;
				selectsValues[2] = 1;
				selectsValues[3] = 1;
				selectsValues[4] = 1;
				selectsValues[5] = 1;
				selectsValues[6] = 1;
				selectsValues[7] = 1;
				selectsValues[8] = 1;
				selectsValues[9] = 1;
				selectsValues[10] = 1;

				//TODO don't? can't remember why not
				//comparator =
				i = temp;
			}
			else {
				//the normal, non-user definable metadata
				selectsIDs[i] = fieldID;
				selectsValues[i] = selects[i].getOperation();
			}
		}

		byte[] correctSizeQuery = null;
		temp = 0;
		//-------------------------------------------------
		//Form the final packed query
		//-------------------------------------------------
		//how many selects, int
		if (nonDistinct) {
			Host.copyInt( selectsLength+1, packedQuery, temp );
			temp+=SRBCommands.INT_LENGTH;
		}
		else {
			Host.copyInt( selectsLength, packedQuery, temp );
			temp+=SRBCommands.INT_LENGTH;
		}

		//how many conditions, int
		if (orderBy) {
			Host.copyInt( conditionsLength+1, packedQuery, temp );
			temp+=SRBCommands.INT_LENGTH;
		}
		else {
			Host.copyInt( conditionsLength, packedQuery, temp );
			temp+=SRBCommands.INT_LENGTH;
		}

		//the id # of the selects, ints
		for (i=0;i<selectsLength;i++) {
			Host.copyInt( selectsIDs[i],	packedQuery, temp );
			temp+=SRBCommands.INT_LENGTH;
		}
		if (nonDistinct) {
			Host.copyInt( SRBMetaDataSet.getSRBID(SRBMetaDataSet.NONDISTINCT),
				packedQuery, temp );
			temp+=SRBCommands.INT_LENGTH;
		}

		//the values of the selects, ints
		for (i=0;i<selectsLength;i++) {
			Host.copyInt( selectsValues[i], packedQuery, temp );
			temp+=SRBCommands.INT_LENGTH;
		}
		if (nonDistinct) {
			Host.copyInt( 1, packedQuery, temp );
			temp+=SRBCommands.INT_LENGTH;
		}


		//the id # of the conditions, ints
		for (i=0;i<conditions.length;i++) {
			if (conditions[i] != null) {
				if (conditions[i].getStyle() == MetaDataCondition.TABLE) {
					byte[] conIDs = setTableConditionIDs( conditions[i].getTableValue(),
						conditions[i].getFieldName() );
					System.arraycopy( conIDs, 0, packedQuery, temp, conIDs.length );
					temp+=conIDs.length;
				}
				else {
					Host.copyInt( SRBMetaDataSet.getSRBID(conditions[i].getFieldName()),
						packedQuery, temp );
					temp+=SRBCommands.INT_LENGTH;

				}
			}
		}

		if (orderBy) {
			Host.copyInt( SRBMetaDataSet.getSRBID(SRBMetaDataSet.ORDERBY),
				packedQuery, temp );
			temp+=SRBCommands.INT_LENGTH;
		}

String banana;
		//the value of the conditions, null-term string
		for (i=0;i<conditions.length;i++) {
//System.out.println(conditions[i]);
			if (conditions[i] != null) {
				int style = conditions[i].getStyle();
				if (style == MetaDataCondition.TABLE) {
					banana = " "+addValueByStyle( style, conditions[i], null );
				}
				else {
					banana = " "+getOperator(conditions[i]) + " "+
						addValueByStyle( style, conditions[i], null );
				}

				System.arraycopy( banana.getBytes(),
					0, packedQuery, temp, banana.length());
				temp+=banana.length()+1; //length+null
			}
		}

		if (orderBy) {
			banana = SRBMetaDataSet.getSRBName( selects[0].getFieldName() );
			for (j=1;j<selects.length;j++) {
				banana += ", "+SRBMetaDataSet.getSRBName( selects[j].getFieldName() );
			}
			System.arraycopy( banana.getBytes(),
				0, packedQuery, temp, banana.length());
			temp+=banana.length()+1; //length+null
		}

correctSizeQuery = new byte[temp+SRBCommands.INT_LENGTH];
System.arraycopy( packedQuery,	0, correctSizeQuery, SRBCommands.INT_LENGTH, temp);

/*
		System.out.print("\ncorrectSizeQuery1  ");
for (i=0;i<correctSizeQuery.length;i++) {
	if (correctSizeQuery[i] > 32) {
		System.out.print((char)correctSizeQuery[i]);
	}
	else {
		System.out.print(correctSizeQuery[i]+" ");
	}
}
System.out.print("\n\n");
*/
//must add total length of packedQuery to beginning
Host.copyInt( temp, correctSizeQuery, 0 );

		commands.startSRBCommand( commands.F_GEN_QUERY, 4 );

		commands.sendArg( catType );
		commands.sendArg( myMcatZone );
		commands.sendArg( correctSizeQuery );
		commands.sendArg( rowsWanted );
		commands.flush( );

		try {
			commands.commandStatus();
		}
		catch( SRBException e ) {
			if (e.getType() == -3005) {
				//the database contains no objects matching the query.
				return null;
			}
			else
				throw e;
		}
		firstRecordList = commands.returnSRBMetaDataRecordList(true);

			if ( secondRecordList != null ) {
				if ( firstRecordList != null ) {
					for (j=0;j<firstRecordList.length;j++) {
						if (continuationIndex < firstRecordList[j].getContinuationIndex())
						{
							continuationIndex = firstRecordList[j].getContinuationIndex();
							firstRecordList = joinRecordLists( firstRecordList,
								commands.srbGetMoreRows( catType, continuationIndex,
									rowsWanted ),	comparator, comparator2 );
						}
					}
					for (j=0;j<firstRecordList.length;j++) {
						firstRecordList[j].setContinuationIndex(-1);
					}
				}
				totalRecordList = joinRecordLists( secondRecordList, firstRecordList,
					comparator, comparator2 );
			}
			else {
				totalRecordList = firstRecordList;
			}
/*
		if ( secondRecordList != null ) {
			if ( firstRecordList != null ) {
				for (j=0;j<firstRecordList.length;j++) {
					if (continuationIndex < firstRecordList[j].getContinuationIndex())
					{
						continuationIndex = firstRecordList[j].getContinuationIndex();
//						firstRecordList = joinRecordLists( firstRecordList,
//							commands.srbGetMoreRows( catType, continuationIndex,
//								rowsWanted ),	comparator, comparator2 );
					}
				}
				for (j=0;j<firstRecordList.length;j++) {
					firstRecordList[j].setContinuationIndex(-1);
				}
			}
			totalRecordList= joinRecordLists( secondRecordList, firstRecordList,
				comparator, comparator2 );
		}
		else {
			totalRecordList= firstRecordList;
		}
*/
/*
System.out.println("totalRecordList "+totalRecordList);
if (totalRecordList != null) {
	for (j=0;j<totalRecordList.length;j++) {
		for (int k=0;k<totalRecordList[j].getRecordCount();k++) {
			Object asdf = totalRecordList[j].getValue(k);
			System.out.println(asdf);
			if (asdf.toString().indexOf( "MetaDataTable" ) >= 0) {
//				for (i=0;i<asdf.getRowCount(),i++) {
					System.out.println(((MetaDataTable) asdf).getStringValue(0,0));
					System.out.println(((MetaDataTable) asdf).getStringValue(0,1));
//				}
			}
		}
	}
}
*/
/*
if (totalRecordList != null)
	System.out.println("total.contInx "+totalRecordList[0].getContinuationIndex());
*/
		return totalRecordList;
	}
}
// End SRBMetaDataCommands-----
