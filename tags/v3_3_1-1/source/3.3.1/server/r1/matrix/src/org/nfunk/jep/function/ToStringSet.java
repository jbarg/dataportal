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

import edu.sdsc.matrix.srb.parser.StdStringSet;
import edu.sdsc.matrix.srb.parser.StringSet;
import edu.sdsc.matrix.srb.parser.impl.StdStringSetImpl;
import edu.sdsc.matrix.srb.parser.impl.StringSetImpl;

import org.nfunk.jep.ParseException;

/**
	* JEP function to convert certains objects (Sting, datasets) into a StringSet
	* For example, a sting AA@bb@cc could be converted to an orderd pair or StringSet.
	* The StringSet could be delimited by the character "@" and result in <S..>AA</S>
	* <S..>bb</S..> and <S..>cc</S..>. This function is very useful in handling data or
	* meta data and operating them in a for-each loop.
	*
	*  @author Arun swaran Jagatheesan (arun@sdsc.edu)
	* @version 1.3
	*/
public class ToStringSet extends PostfixMathCommand{

			public ToStringSet(){
						numberOfParameters = 2;
			}

			public void run(Stack inStack) throws ParseException{
						// check the stack
						checkStack(inStack);
						// get the parameter from the stack
						Object delimObj = inStack.pop();
						String delimStr;
						StringSet targetSet;
						Object sourceObj = inStack.pop();

						// check whether the argument is of the right type (delim = string)
									if (delimObj instanceof String){
												// this could be again variable thing - check it out
												delimStr = (String) delimObj;
									}else{
												throw new ParseException("Invalid parameter type. Expected a string as delimiter in toStringSet ");
									}
									// assuming the target objects are first parsed and have their obj values
									int sourceType = 0;
									if (sourceObj instanceof String){
												sourceType = 2;
									}else if (sourceObj instanceof edu.sdsc.matrix.srb.parser.DataSet){
												sourceType = 5;
									}
									switch (sourceType) {
														case 2: //string
														StringTokenizer str = new StringTokenizer((String)sourceObj, delimStr, false);
														targetSet = new StringSetImpl();
														targetSet.setStdStringSet( new StdStringSetImpl());
														StdStringSet ssst = targetSet.getStdStringSet();
														while (str.hasMoreTokens()){
																	ssst.getStringElement().add(str.nextToken());
														}
														inStack.push(targetSet);
														break;

														case 5 : //dataset

														break;


														default:
														throw new ParseException("ToStringSet:Invalid parameter type - 2nd Param Type");
												}
									}
			}