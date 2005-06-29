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

import java.util.ArrayList;

import edu.sdsc.matrix.srb.util.MatrixUtil;

/*
	*
	* @author rmathew
	*
	* Class that contains the Matrix Query Return Clause
	* For example if the return clause was as follows
	* 	"return <Metadata>
	* 					<FileName>{$coll/SystemMetadata/FILE_NAME}</FileName>
	* 					<Offset>{$coll/SystemMetadata/OFFSET}</Offset>
	* 			</Metadata>"
	*
	* the following fields will be set for this class
	* 		returnExpression = "return <Metadata><FileName>{$coll/SystemMetadata/FILE_NAME}</FileName></Metadata>"
	* and
	* 	selectionFieldNames = {"$coll/SystemMetadata/FILE_NAME","$coll/SystemMetadata/OFFSET"}
	*
	*/
public class MatrixQueryReturnClause {
	/*
		* This contains the Return Expression
		* This is use for parsing the query selection fields and
		* for formatting the query result string
		*/
	private String returnExpression;
	private ArrayList selectionFieldNames; // List of Selection Fields Names
	/*
		* Public Constructor
		*/
	public MatrixQueryReturnClause() {
	}

	/*
		* Set Return Expression
		*/
	public void setReturnExpression(String expr) {
		returnExpression = expr;
	}

	/*
		* Return Return Expression
		*/
	public String getReturnExpression() {
		return returnExpression;
	}

	/*
		* During Query Request Parsing, the list of Selection Field Names
		* contains the whole return expression, which shouldn't be there.
		* This method removes tha extra field from the list of selection field names
		*/
	public void fixToRemoveReturnExpresionFromSelectionFieldNames() {
		MatrixUtil.toLog("MatrixQueryReturnClause-fixToRemoveReturnExpresionFromSelectionFieldNames():BEGIN",
							MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("MatrixQueryReturnClause-fixToRemoveReturnExpresionFromSelectionFieldNames() - selectionFieldNames - size1:" +
							selectionFieldNames.size(), MatrixUtil.DEBUG_LOG);
		if (returnExpression != null) {
			int indexOfReturnExpr = selectionFieldNames.indexOf(
						returnExpression);
			MatrixUtil.toLog("MatrixQueryReturnClause-fixToRemoveReturnExpresionFromSelectionFieldNames():indexOfReturnExpr:" +
								indexOfReturnExpr, MatrixUtil.DEBUG_LOG);
			if (indexOfReturnExpr != -1) {
				selectionFieldNames.remove(indexOfReturnExpr);
				MatrixUtil.toLog("MatrixQueryReturnClause-fixToRemoveReturnExpresionFromSelectionFieldNames() - RETURN EXPR REMOVED",
									MatrixUtil.DEBUG_LOG);
			}
		}
		MatrixUtil.toLog("MatrixQueryReturnClause-fixToRemoveReturnExpresionFromSelectionFieldNames() - selectionFieldNames - size2:" +
							selectionFieldNames.size(), MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("MatrixQueryReturnClause-fixToRemoveReturnExpresionFromSelectionFieldNames():END",
							MatrixUtil.DEBUG_LOG);
	}

	public void parseReturnClause(String clause) {
	}

	/*
		* Print Return Clause Details
		*/
	public void printClause() {
		MatrixUtil.toLog("MatrixQueryReturnClause-printClause():BEGIN",
							MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog(
					"\nMatrixQueryReturnClause-printClause():Return Clause",
					MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog(
					"MatrixQueryReturnClause-printClause():ReturnExpression : " +
					returnExpression, MatrixUtil.DEBUG_LOG);
		for (int i = 0; i < selectionFieldNames.size(); i++) {
			MatrixUtil.toLog(
						"MatrixQueryReturnClause-printClause():Return Clause - Selection Field(" +
						i + ")-" + selectionFieldNames.get(i), MatrixUtil.DEBUG_LOG);
		}
		MatrixUtil.toLog("MatrixQueryReturnClause-printClause():END",
							MatrixUtil.DEBUG_LOG);
	}

	/*
		* Set the list of selection field names
		*/
	public void setSelectionFieldNames(ArrayList list) {
		selectionFieldNames = list;
	}

	/*
		* Return the list of selection field names
		*/
	public ArrayList getSelectionFieldNames() {
		return selectionFieldNames;
	}

	/*
		* Add a new field to the list of selection field names
		*/
	public void addSelectionFieldName(String fieldName) {
		if (selectionFieldNames == null) {
			selectionFieldNames = new ArrayList();
		}
		selectionFieldNames.add(fieldName);
	}
}