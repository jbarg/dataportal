package uk.ac.cclrc.xmlwrapper ; 

import java.util.* ;


public class DBHelperFactory
{

   static DBHelper create(String helper)
   {
      if (helper.compareToIgnoreCase("relational") == 0 )
      {
	 return new RelationalDBHelper() ;
      }
      else if (helper.compareToIgnoreCase("xml") == 0 )
      {
	 return new XMLDBHelper() ;
      }
      else
      {
	 return null ;
      }

   }

}
