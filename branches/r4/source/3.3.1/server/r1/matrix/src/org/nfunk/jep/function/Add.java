/*****************************************************************************
	JEP - Java Math Expression Parser 2.24
						December 30 2002
						(c) Copyright 2002, Nathan Funk
						See LICENSE.txt for license information.
	*****************************************************************************/

package org.nfunk.jep.function;

import java.util.Iterator;
import java.util.List;
import java.util.Stack;

import edu.sdsc.matrix.srb.parser.StdStringSet;
import edu.sdsc.matrix.srb.parser.StringSet;
import edu.sdsc.matrix.srb.parser.impl.StdStringSetImpl;
import edu.sdsc.matrix.srb.parser.impl.StringSetImpl;

import org.nfunk.jep.ParseException;
import org.nfunk.jep.type.Complex;

public class Add extends PostfixMathCommand{

			public Add(){
						numberOfParameters = -1;
			}

			/**
				* Calculates the result of applying the "+" operator to the arguments from
				* the stack and pushes it back on the stack.
				*/
			public void run(Stack stack) throws ParseException{
						checkStack(stack); // check the stack
						Object sum = stack.pop();
						Object param;
						int i = 1;
						// repeat summation for each one of the current parameters
						while (i < curNumberOfParameters){
									// get the parameter from the stack
									param = stack.pop();
									// add it to the sum (order is important for String arguments)
									sum = add(param, sum);
									i++;
						}
						stack.push(sum);
						return;
			}

			public Object add(Object param1, Object param2) throws ParseException{


						if (param1 instanceof edu.sdsc.matrix.srb.parser.StringSet && param2 instanceof edu.sdsc.matrix.srb.parser.StringSet){
									StringSet _param1 = (StringSet) param1;
									StringSet _param2 = (StringSet) param2;
									// WARNING check for _param1.getStringSetReference() on both later
									if (_param1.getStdStringSet() == null || _param2.getStdStringSet() == null){
												if(_param1.getStdStringSet() != null) return _param1;
												return _param2;
												// The above return _param2 is the same logic as the
												// two lines that follow
												//if(_param2.getStdStringSet() != null) return _param2;
												//return null;
									}else{
												List list1 = _param1.getStdStringSet().getStringElement();
												List list2 = _param2.getStdStringSet().getStringElement();
												// we dont check for length - just add
												StringSet targetSet =		new StringSetImpl();
												targetSet.setStdStringSet( new StdStringSetImpl());
												StdStringSet ssst = targetSet.getStdStringSet();
												List resultList = ssst.getStringElement();
												// HPC SCALABILITY WARNING: (DATA_R_US)
												// This will add only lists of size() upto Integer.MAX_VALUE
												// the rest will be ignored (so the returned targetSet will
												// not have more than Integer.MAX_VALUE elements)
												// BTW, Integer.MAX_VALUE = 2,147,483,647
												Iterator minItr = (list1.size() <= list2.size() ) ? list1.iterator() : list2.iterator();
												Iterator maxItr = (list1.size() > list2.size() ) ? list1.iterator() : list2.iterator();
												while (minItr.hasNext()){
															String s1 = (String) maxItr.next();
															String s2 = (String) minItr.next();
															try{
																		resultList.add(Double.toString(Double.parseDouble(s1) + Double.parseDouble(s2)));
															} catch (java.lang.NumberFormatException nfe){
																		resultList.add( s1 + s2 );
															}
												}
												if (list1.size() > list2.size()){
															resultList.addAll(list1.subList(list2.size(),list1.size()));
												}else if (list2.size() > list1.size()){
															resultList.addAll(list2.subList(list1.size(),list2.size()));
												}
												return targetSet;
									}

						// Arun comment: the following line was coded by jon - i got no idea
						// what the logic or condition he wanted - not touching it.
						// Jon (7/6/04)
						// if either of the parameters is not a number then we just concat their
						// string representations

						}else if(!(param1 instanceof Number || param1 instanceof Complex) ||
								!(param2 instanceof Number || param2 instanceof Complex)){
									return FunctionUtils.getString(param1) +
											FunctionUtils.getString(param2);
						}else if (param1 instanceof Number){
									if (param2 instanceof Number){
												return add((Number) param1, (Number) param2);
									} else if (param2 instanceof Complex){
												return add((Complex) param2, (Number) param1);
									}
						} else if (param1 instanceof Complex){
									if (param2 instanceof Number){
												return add((Complex) param1, (Number) param2);
									} else if (param2 instanceof Complex){
												return add((Complex) param1, (Complex) param2);
									}
						}
						throw new ParseException("Invalid parameter type");
			}

			public Double add(Number d1, Number d2){
						return new Double(d1.doubleValue() + d2.doubleValue());
			}

			public Complex add(Complex c1, Complex c2){
						return new Complex(c1.re() + c2.re(), c1.im() + c2.im());
			}

			public Complex add(Complex c, Number d){
						return new Complex(c.re() + d.doubleValue(), c.im());
			}
}