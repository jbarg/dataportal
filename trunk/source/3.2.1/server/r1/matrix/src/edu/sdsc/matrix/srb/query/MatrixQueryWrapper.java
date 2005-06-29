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

/*
 * 
 * @author rmathew
 *
 * Contains the Matrix Query Request Details
 * 		- For Clause
 * 		- Where Clause 
 * 		- Return Clause
 */

public class MatrixQueryWrapper
{
	
	private MatrixQueryForClause forClause;	// MatrixQueryRequest For Clause
	private MatrixQueryWhereClause whereClause; // MatrixQueryRequest Where Clause
	private MatrixQueryReturnClause returnClause; // Matrix Query Request Return Clause
	
	/*
	 * Public Constructor for this class
	 * Initializes objects for the For,Where and Return Clauses
	 */	
	public MatrixQueryWrapper()
	{
		forClause = new MatrixQueryForClause();
		whereClause = new MatrixQueryWhereClause();
		returnClause = new MatrixQueryReturnClause();
	}
	
	/*
	 * Set For Clause
	 */
	public void setMatrixQueryForClause(MatrixQueryForClause clause)
	{
		forClause = clause;
	}
	
	/*
	 * Return For Clause
	 */
	public MatrixQueryForClause getMatrixQueryForClause()
	{
		return forClause;
	}
	
	
	/*
	 * Set Where Clause
	 */
	public void setMatrixQueryWhereClause(MatrixQueryWhereClause clause)
	{
		whereClause =clause;
	}
	
	/*
	 * Return Where Clause
	 */
	public MatrixQueryWhereClause getMatrixQueryWhereClause()
	{
		return whereClause;
	}
	
	/*
	 * Set Return Clause
	 */
	public void setMatrixQueryReturnClause(MatrixQueryReturnClause clause)
	{
		returnClause = clause;
	}
	
	/*
	 * Return Return Clause
	 */
	public MatrixQueryReturnClause getMatrixQueryReturnClause()
	{
		return returnClause;
	}
	
	/*
	 * Display Matrix Query Request Details 
	 */
	public void printClauses()
	{
			forClause.printClause();
			whereClause.printClause();
			returnClause.printClause();	
	}
	
	
	
}