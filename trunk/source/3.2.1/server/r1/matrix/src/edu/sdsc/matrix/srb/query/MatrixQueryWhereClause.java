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

public class MatrixQueryWhereClause
{
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
	public MatrixQueryWhereClause()
	{
		conditionList = new ArrayList();
	}
	
	/*
	 * Set Where Expression
	 */
	public void setWhereExpression(String expr)
	{
		whereExpression = expr;
	}
	
	/*
	 * Return Where Expression
	 */
	public String getWhereExpression()
	{
		return whereExpression;
	}
		
	
	public void parseWhereClause(String clause)
	{
		
	}
	
	/*
	 * Display Where Clause Details
	 */	
	public void printClause()
	{
		MatrixUtil.toLog("MatrixQueryWhereClause-printClause():BEGIN",MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("\nMatrixQueryWhereClause-printClause():Where Clause",MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("MatrixQueryWhereClause-printClause():Where Expression : "+whereExpression,MatrixUtil.DEBUG_LOG);	
		MatrixUtil.toLog("MatrixQueryWhereClause-printClause():Number of Conditions:"+conditionList.size(),MatrixUtil.DEBUG_LOG);
		for(int i=0;i<conditionList.size();i++)
		{
			MatrixQueryConditionClause tempClause = (MatrixQueryConditionClause)conditionList.get(i);
			tempClause.printClause();	
		}
		MatrixUtil.toLog("MatrixQueryWhereClause-printClause():END",MatrixUtil.DEBUG_LOG);
	}
	
	/*
	 * Return Condition List
	 */
	public ArrayList getConditionList()
	{
		return conditionList;				
	}
	
	/*
	 * Add New Condition to Condition List
	 */
	public void addCondition(MatrixQueryConditionClause condition)
	{
		conditionList.add(condition);		
	}
	
	
	
}