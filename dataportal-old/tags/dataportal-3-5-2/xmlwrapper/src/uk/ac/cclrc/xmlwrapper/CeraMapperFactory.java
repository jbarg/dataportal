package uk.ac.cclrc.xmlwrapper ; 

import java.util.* ;


public class CeraMapperFactory
{

   static CeraMapper create(String mapper)
   {
      if (mapper.compareToIgnoreCase("badc") == 0 )
      {
	 return new BADCCeraMapper() ;
      }
      else if (mapper.compareToIgnoreCase("mpim") == 0 )
      {
	 return new MPIMCeraMapper() ;
      }
      else
      {
	 return null ;
      }

   }

}
