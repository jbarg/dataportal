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

package org.nfunk.jep.function;

import java.util.Stack;
import java.util.StringTokenizer;

import org.nfunk.jep.ParseException;
import edu.sdsc.matrix.srb.core.MatrixStore;
import edu.sdsc.matrix.srb.util.MatrixUtil;
import edu.sdsc.matrix.srb.core.expressions.ExpressionEvaluator;

/**
	* This function finds
	* @author not attributable
	* @version
	*/
public class GetProperty extends PostfixMathCommand{
			/**
				* @param thisThreadInstanceId String The id of the thread whose status will
				* be mapped to the keyword "THIS".
				* @param store MatrixStore
				*/
			public GetProperty(){
						numberOfParameters = 2;
			}

			public void run(Stack inStack) throws ParseException{
						// check the stack
						checkStack(inStack);
						// get the parameter from the stack
						// the query is the second parameter, so it is popped first
						Object query = inStack.pop();
						Object baseObject = inStack.pop();
						if (query instanceof String){
									String queryString = (String) query;
									StringTokenizer tokenizer = new StringTokenizer(queryString, ".");
									try{
												while (tokenizer.hasMoreTokens() && baseObject != null){
															String nextField = tokenizer.nextToken();
															baseObject = MatrixUtil.getFieldByName(baseObject, nextField);
												}
									} catch (Exception e){
												throw new ParseException();
									}
									inStack.push(baseObject);
						} else{
									throw new ParseException("Invalid parameter type");
						}
			}
}