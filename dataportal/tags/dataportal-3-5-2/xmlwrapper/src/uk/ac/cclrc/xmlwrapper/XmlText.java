package uk.ac.cclrc.xmlwrapper ;

import java.util.* ;
import java.io.* ;

public class XmlText
{

   // replaces the literal string value with the entity reference
   public String makeValid(String input)
   {
      String ret = "";

      byte [] in_str ;
      byte [] out_str ;
   
      try
      {
         in_str =  input.getBytes("ISO-8859-1");

         out_str = new byte[in_str.length] ;

         int j = 0 ;

         for(int i = 0 ; i < in_str.length ; i++)
         {
            // 32 - space, 9 - tab, 10 - line feed, 13 - carriage return all in decimal
	    if(in_str[i] > 31 ||
	       in_str[i] == 9 ||
	       in_str[i] == 10 ||
	       in_str[i] == 13 )
	    {
	       out_str[j++]=in_str[i] ;
	    }
         }
	 
	 ret = new String (out_str, 0, j, "ISO-8859-1") ;

      }
      catch (UnsupportedEncodingException uee)
      {
	 uee.printStackTrace() ;
      }

      return ret ;

   }
     
   //
   // main test
   //

   public static void main(String[] argv) 
   {
      XmlText xt = new XmlText() ;
      
      String in;
      try
      {
         in=argv[0];
      }
      catch(ArrayIndexOutOfBoundsException e)
      {
	      
	 //works
         in="H ello&World" + "\u0014" + "\u0014" + "\u0014" + "\r" + "\n" + "\u0009" + "d" ;
    
      }	

      System.out.println("Source: '" + in + "' length: " + in.length());
      System.out.println("Result: '" + xt.makeValid(in) + "' length: " + (xt.makeValid(in)).length());

      return ;
   }

}


