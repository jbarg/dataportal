package uk.ac.cclrc.xmlwrapper ; 

import java.util.* ;


public class CsmdMapperFactory
{

   static CsmdMapper create(String mapper)
   {
      if (mapper.compareToIgnoreCase("isis") == 0 )
      {
	 return new ISISCsmdMapper() ;
      }
      else if (mapper.compareToIgnoreCase("isisy") == 0 )
      {
         //needs to be changed to something like:
	 //return new ISISCsmdMapper() ;
         //in the future

	 return new ISISCsmdMapper() ;
      }
      else
      {
	 return null ;
      }

   }

}
