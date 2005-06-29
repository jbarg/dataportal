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

/*
	*
	* @author rmathew
	*
	* Class maintains the details of the "condition" clause for Matrix XQuery
	* For Example the Matrix XQuery clause might have a condition
	* $coll/SystemMetadata/OFFSET <5000
	* In this case the following values will be set for this class
	*  	conditionClause = $coll/SystemMetadata/OFFSET <5000
	*  	conditionVariable = $coll/SystemMetadata/OFFSET
	* 		operator = "="
	* 		conditionValue = 5000
	*/
import edu.sdsc.matrix.srb.util.MatrixUtil;

public class MatrixQueryConditionClause {
	private String conditionClause; // Condition Clause as String
	private String conditionVariable; // Variable for condition
	private String operator; // Operator used for Condition
	private String conditionValue; // Value checked for condition
	private String matrixCondition;
	/*
		* Public Constructor
		*/
	public MatrixQueryConditionClause() {
	}

	/*
		* Set the Condition Clause
		*/
	public void setConditionClause(String expr) {
		conditionClause = expr;
	}

	/*
		* Return Condition Clause
		*/
	public String getConditionClause() {
		return conditionClause;
	}

	/*
		* Set Condition Variable
		*/
	public void setConditionVariable(String var) {
		conditionVariable = var;
	}

	/*
		* Return Condition Variable
		*/
	public String getConditionVariable() {
		return conditionVariable;
	}

	/*
		* Set Operator
		*/
	public void setOperator(String op) {
		operator = op;
	}

	/*
		* Return Operator
		*/
	public String getOperator() {
		return operator;
	}

	/*
		* Set Condition Value
		* This method extracts the "" at the start & end of string
		*/
	public void setConditionValue(String value) {
		if (value != null && value.length() > 0) {
			if ( (value.charAt(0) == '\"') &&
				(value.charAt(value.length() - 1) == '\"')) {
				value = value.substring(1, value.length() - 1);
			}
		}
		conditionValue = value;
	}

	/*
		* Return Condition Value
		*/
	public String getConditionValue() {
		return conditionValue;
	}

	/*
		* Print the Clause details
		*/
	public void printClause() {
		MatrixUtil.toLog("MatrixQueryConditionClause-printClause():BEGIN",
							MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog(
					"\nMatrixQueryConditionClause-printClause():Condition Clause -" +
					conditionClause, MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog(
					"MatrixQueryConditionClause-printClause():Condition Variable : " +
					conditionVariable, MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog(
					"MatrixQueryConditionClause-printClause():Condition Operator : " +
					operator, MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog(
					"MatrixQueryConditionClause-printClause():Condition Value : " +
					conditionValue, MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("MatrixQueryConditionClause-printClause():END",
							MatrixUtil.DEBUG_LOG);
	}
}