package uk.ac.dl.rasgrib;
/**
 *  This class handles RasGribServlet exception type for this servlet.
 */
class RasGribException extends Exception
{
  public RasGribException()
  {
   super();
  }
  public RasGribException(String s)
  {
    super(s);
  }
}
