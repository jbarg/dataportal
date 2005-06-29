package org.nfunk.jep.function;

import java.util.*;
import java.util.StringTokenizer;

import org.nfunk.jep.ParseException;

import edu.sdsc.matrix.srb.core.MatrixStore;
import edu.sdsc.matrix.srb.core.Lineage;
import edu.sdsc.matrix.srb.core.expressions.ExpressionEvaluator;
import edu.sdsc.matrix.srb.parser.Status;
import edu.sdsc.matrix.srb.parser.TPart;
import edu.sdsc.matrix.srb.parser.*;
import edu.sdsc.matrix.srb.util.*;

/**
	* This function finds
	* @author not attributable
	* @version
	*/
public class StringSet2X extends PostfixMathCommand{

			// the expressionEvaluator to use
			public StringSet2X(){
						numberOfParameters = 3;
			}

			public void run(Stack inStack) throws ParseException{
						// check the stack
						checkStack(inStack);
						// get the parameter from the stack
						Object delimObj = inStack.pop();
						String delimStr;
						StringSet sourceSet;

						// check whether the argument is of the right type (delim = string)
									if (delimObj instanceof String){
												// this could be again variable thing - check it out
												delimStr = (String) delimObj;
									} else{
												throw new ParseException("StringSet2X:Invalid parameter type - 3rd Param");
									}
									// assuming the target objects are first parsed and have their obj values
									Object targetObj = inStack.pop();
									Object sourceObj = inStack.pop();
									if (sourceObj instanceof edu.sdsc.matrix.srb.parser.StringSet){
												sourceSet = (StringSet) sourceObj;
												if (sourceSet == null){
															throw new ParseException("StringSet2X:Invalid parameter type - 1st Param");
												}else{
														//
												}
									}else{
												throw new ParseException("StringSet2X:Invalid parameter type - 1st Param");
									}

									int targetType = 0;
									if (targetObj instanceof String){
												targetType = 2;
									}else if (targetObj instanceof edu.sdsc.matrix.srb.parser.DataSet){
												targetType = 5;
									}
									switch (targetType) {
														case 2: //string
														StringBuffer outBuf = new StringBuffer();
														StdStringSet sst = sourceSet.getStdStringSet();
														List stringElements = sst.getStringElement();
														for(Iterator i = stringElements.iterator(); i.hasNext(); ){
																	outBuf.append((String)i.next());
																	outBuf.append(delimStr);
														}
														targetObj = outBuf.toString();
														inStack.push((String)targetObj);
														break;

														case 5 : //dataset

														break;


														default:
														throw new ParseException("StringSet2X:Invalid parameter type - 2nd Param Type");
												}
									}
			}