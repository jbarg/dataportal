package uk.ac.cclrc.xmlwrapper ; 

import java.util.* ;


public class CsmdMapperFactory
{

   static CsmdMapper create(String mapper)
   {
      if (mapper.compareToIgnoreCase("emin") == 0 )
      {
	 return new EMINCsmdMapper() ;
      }
      else if (mapper.compareToIgnoreCase("emat") == 0 )
      {
	 return new EMATCsmdMapper() ;
      }
      else
      {
	 return null ;
      }

   }

}
