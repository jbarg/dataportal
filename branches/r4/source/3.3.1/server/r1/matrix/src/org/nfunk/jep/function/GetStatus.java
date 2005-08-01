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
import edu.sdsc.matrix.srb.core.Lineage;
import edu.sdsc.matrix.srb.core.expressions.ExpressionEvaluator;
import edu.sdsc.matrix.srb.parser.Status;

/**
	* This function finds
	* @author not attributable
	* @version
	*/
public class GetStatus extends PostfixMathCommand{
			// the store to be used to find the status
			private MatrixStore store;

// the base thread to start from
			private String baseInstanceId;
			private Lineage baseLineage;

			// the expressionEvaluator to use
			private ExpressionEvaluator expressionEvaluator;

			public GetStatus(String baseInstanceId, MatrixStore store,
					ExpressionEvaluator evaluator){
						this.baseInstanceId = baseInstanceId;
						this.expressionEvaluator = evaluator;
						this.store = store;
						numberOfParameters = 1;
						this.baseLineage = this.store.getLineage(this.baseInstanceId);
			}

			public void run(Stack inStack) throws ParseException{
						// check the stack
						checkStack(inStack);
						// get the parameter from the stack
						Object param = inStack.pop();
						// check whether the argument is of the right type
						if (param instanceof String){
									// we need to translate the param to a lineage, but the lineage should
									// start with the lineage of the thread we are currently in
									Lineage lineage = new Lineage();
									lineage.addElements(this.baseLineage);
									// "." is the separator (example: a[1].b[6].c[MAX]
									StringTokenizer tokenizer = new StringTokenizer((String) param, ".", false);
									while (tokenizer.hasMoreTokens()){
												String token = tokenizer.nextToken();
												int indexOfOpenBracket = token.indexOf("[");
												// if there is no open bracket then we assume max iteration is wanted
												if (indexOfOpenBracket == -1)
															lineage.addElement(token);
												else{
															int indexOfCloseBracket = token.indexOf("]");
															// the name is everything up to the open bracket
															String name = token.substring(0, indexOfOpenBracket);
															// inside the brackets is some expression that needs to be evaluated
															String iterationExpression = token.substring(indexOfOpenBracket +
																	1, indexOfCloseBracket);
															String evaluatedIterationString =
																	expressionEvaluator.evaluateExpression(iterationExpression).
																	toString();
															// the expression should evaluate to an int, if not, it's ok to throw
															// an exception
															int iterationNumber = (int) (Double.parseDouble(
																	evaluatedIterationString));
															// an an element to the lineage
															lineage.addElement(name, iterationNumber);
												}
									}
									String instanceId = this.store.getInstanceId(lineage);
									Status status = this.store.getStatus(instanceId);
									inStack.push(status);
						} else{
									throw new ParseException("Invalid parameter type");
						}
			}
}