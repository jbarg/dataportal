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
	* Class captures the details of the For Clause for the XQuery
	* For Example , if the For Clause was "for $coll in collection("testCollection")",
	* the following values will be set for the class.
	* 		variableName = coll
	* 		functionName = collection
	* 		functionParameter = testCollection
	*/
import edu.sdsc.matrix.srb.util.MatrixUtil;

public class MatrixQueryForClause {
	private String variableName; // Variable Name for For Clause
	private String functionName; // Function Name For Clause
	private String functionParameter; // PArameter of Function
	/*
		* Public Constructor
		*/
	public MatrixQueryForClause() {
	}

	/*
		* Set Variable Name
		*/
	public void setVariableName(String name) {
		variableName = name;
	}

	/*
		* Return Variable Name
		*/
	public String getVariableName() {
		return variableName;
	}

	/*
		* Set Function Name
		*/
	public void setFunctionName(String name) {
		functionName = name;
	}

	/*
		* Return Function Names
		*/
	public String getFunctionName() {
		return functionName;
	}

	/*
		* Set Function Parameters
		*/
	public void setFunctionParameter(String param) {
		functionParameter = param;
	}

	/*
		* Return Function Parameters
		*/
	public String getFunctionParameter() {
		return functionParameter;
	}

	public void parseForClause(String clause) {
	}

	/*
		* Print Clause Details
		*/
	public void printClause() {
		MatrixUtil.toLog("MatrixQueryForClause-printClause():BEGIN",
							MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("\nMatrixQueryForClause-printClause():For Clause",
							MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("MatrixQueryForClause-printClause():Variable Name : " +
							variableName, MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("MatrixQueryForClause-printClause():Function Name : " +
							functionName, MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog(
					"MatrixQueryForClause-printClause():FunctionParameter : " +
					functionParameter, MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog(
					"MatrixQueryForClause-printClause():MatrixQueryForClause-printClause():END",
					MatrixUtil.DEBUG_LOG);
	}
}