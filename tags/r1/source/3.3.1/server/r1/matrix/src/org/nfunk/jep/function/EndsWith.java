package org.nfunk.jep.function;

import java.util.Stack;

import org.nfunk.jep.ParseException;

public class EndsWith extends PostfixMathCommand
{
  public EndsWith()
  {
    numberOfParameters = 2;
  }

  public void run(Stack inStack) throws ParseException
  {
    // check the stack
    checkStack(inStack);

    // get the parameter from the stack

    // the query is the second parameter, so it is popped first
    String queryString = (String) inStack.pop();
    String searchString = (String) inStack.pop();

    int i = searchString.endsWith(queryString) ? 1 : 0;
    inStack.push(new Double(i));
  }
}
