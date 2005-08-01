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
	* Contains the information for Matrix Query Request Where Clause
	* For Example if the where clause was
	* 	where $coll/SystemMetadata/OFFSET < 5000
	*
	* then the following fields would be set for this class
	* 		whereExpression = where $coll/SystemMetadata/OFFSET < 5000
	* 		conditionList will have the list of MetaDataQueryConditionClause for the conditions
	* 		-  $coll/SystemMetadata/OFFSET < 5000
	*/
public class MatrixQueryWhereClause {
	/*
		* The String form of the whole Where Clause
		*/
	private String whereExpression;
	/*
		* Contains a list of MetaDataConditionClauses that
		* maintain the where clause conditions
		*/
	private ArrayList conditionList;
	/*
		* Public Constructor
		* Initializes the Condition List
		*/
	public MatrixQueryWhereClause() {
		conditionList = new ArrayList();
	}

	/*
		* Set Where Expression
		*/
	public void setWhereExpression(String expr) {
		whereExpression = expr;
	}

	/*
		* Return Where Expression
		*/
	public String getWhereExpression() {
		return whereExpression;
	}

	public void parseWhereClause(String clause) {
	}

	/*
		* Display Where Clause Details
		*/
	public void printClause() {
		MatrixUtil.toLog("MatrixQueryWhereClause-printClause():BEGIN",
							MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("\nMatrixQueryWhereClause-printClause():Where Clause",
							MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog(
					"MatrixQueryWhereClause-printClause():Where Expression : " +
					whereExpression, MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog(
					"MatrixQueryWhereClause-printClause():Number of Conditions:" +
					conditionList.size(), MatrixUtil.DEBUG_LOG);
		for (int i = 0; i < conditionList.size(); i++) {
			MatrixQueryConditionClause tempClause = (MatrixQueryConditionClause)
						conditionList.get(i);
			tempClause.printClause();
		}
		MatrixUtil.toLog("MatrixQueryWhereClause-printClause():END",
							MatrixUtil.DEBUG_LOG);
	}

	/*
		* Return Condition List
		*/
	public ArrayList getConditionList() {
		return conditionList;
	}

	/*
		* Add New Condition to Condition List
		*/
	public void addCondition(MatrixQueryConditionClause condition) {
		conditionList.add(condition);
	}
}