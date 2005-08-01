package org.nfunk.jep.function;

public class FunctionUtils
{
  public static String getString(Object o)
  {
    if(o instanceof Number)
    {
      Number p1 = (Number)o;
      if(p1.intValue() == p1.doubleValue())
        return p1.intValue()+"";
    }

    return o.toString();
  }
}
