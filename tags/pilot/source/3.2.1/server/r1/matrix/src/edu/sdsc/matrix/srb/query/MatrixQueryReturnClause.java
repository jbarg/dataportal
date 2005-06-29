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
public class MatrixQueryReturnClause
{
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
	public MatrixQueryReturnClause()
	{
		
	}
	
	/*
	 * Set Return Expression
	 */	
	public void setReturnExpression(String expr)
	{
		returnExpression = expr;
	}
	
	/*
	 * Return Return Expression
	 */
	public String getReturnExpression()
	{
		return returnExpression;
	}
	
	/*
	 * During Query Request Parsing, the list of Selection Field Names 
	 * contains the whole return expression, which shouldn't be there. 
	 * This method removes tha extra field from the list of selection field names
	 */
	public void fixToRemoveReturnExpresionFromSelectionFieldNames()
	{
		MatrixUtil.toLog("MatrixQueryReturnClause-fixToRemoveReturnExpresionFromSelectionFieldNames():BEGIN",MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("MatrixQueryReturnClause-fixToRemoveReturnExpresionFromSelectionFieldNames() - selectionFieldNames - size1:"+selectionFieldNames.size(),MatrixUtil.DEBUG_LOG);
		if(returnExpression!=null)
		{
			int indexOfReturnExpr = selectionFieldNames.indexOf(returnExpression);
			MatrixUtil.toLog("MatrixQueryReturnClause-fixToRemoveReturnExpresionFromSelectionFieldNames():indexOfReturnExpr:"+indexOfReturnExpr,MatrixUtil.DEBUG_LOG);
			if (indexOfReturnExpr!=-1)
			{
				selectionFieldNames.remove(indexOfReturnExpr);
				MatrixUtil.toLog("MatrixQueryReturnClause-fixToRemoveReturnExpresionFromSelectionFieldNames() - RETURN EXPR REMOVED",MatrixUtil.DEBUG_LOG);
			}
		}
		MatrixUtil.toLog("MatrixQueryReturnClause-fixToRemoveReturnExpresionFromSelectionFieldNames() - selectionFieldNames - size2:"+selectionFieldNames.size(),MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("MatrixQueryReturnClause-fixToRemoveReturnExpresionFromSelectionFieldNames():END",MatrixUtil.DEBUG_LOG);
	}
		
	
	public void parseReturnClause(String clause)
	{
			
	}
	
	/*
	 * Print Return Clause Details 
	 */
	public void printClause()
	{
		MatrixUtil.toLog("MatrixQueryReturnClause-printClause():BEGIN",MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("\nMatrixQueryReturnClause-printClause():Return Clause",MatrixUtil.DEBUG_LOG);
		MatrixUtil.toLog("MatrixQueryReturnClause-printClause():ReturnExpression : "+returnExpression,MatrixUtil.DEBUG_LOG);
		for(int i=0;i<selectionFieldNames.size();i++)
		{
			MatrixUtil.toLog("MatrixQueryReturnClause-printClause():Return Clause - Selection Field("+i+")-"+selectionFieldNames.get(i),MatrixUtil.DEBUG_LOG);		
		}
		MatrixUtil.toLog("MatrixQueryReturnClause-printClause():END",MatrixUtil.DEBUG_LOG);
	}
	
	/*
	 * Set the list of selection field names
	 */
	public void setSelectionFieldNames(ArrayList list)
	{
		selectionFieldNames = list;
	}
	
	/*
	 * Return the list of selection field names
	 */
	public ArrayList getSelectionFieldNames()
	{
		return selectionFieldNames;
	}
	
	/*
	 * Add a new field to the list of selection field names
	 */
	public void addSelectionFieldName(String fieldName)
	{
		if(selectionFieldNames==null)
			selectionFieldNames = new ArrayList(); 
		selectionFieldNames.add(fieldName);
	}
	
}