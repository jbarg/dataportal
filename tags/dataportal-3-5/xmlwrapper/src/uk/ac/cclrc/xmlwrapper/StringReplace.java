package uk.ac.cclrc.xmlwrapper ;

public class StringReplace
{

   StringBuffer sbr ;

   StringReplace(int sb_size)
   {
      sbr = new StringBuffer(sb_size) ;
   }

   StringReplace()
   {
      sbr = new StringBuffer(200000) ;
   }

   // replaces the literal string value with the entity reference
   public String LitWithEnt(String input)
   {
      if(sbr.length() != 0 )
      {
         sbr.delete(0,sbr.length()) ;
      }
    
      sbr.append(input) ;

      int sbl = sbr.length() ;

      boolean contains_escaped = false ;

     for(int i = 0 ; i < sbr.length() ; i++)
     {
        if(sbr.charAt(i) == '<')
        {
           sbr.replace(i, (i+1), "&lt;") ;
           i+=3 ;
           // System.out.println("Added LT :\t" + sbr.toString()) ;

        }
        else if (sbr.charAt(i) == '&')
        {

          //so we don't double escape escaped text
          // note when testing for &gt; lazy eval has already made sure of the length
          contains_escaped = false ;

          if((i+3) < sbr.length())
          {
             if((sbr.substring(i, (i+4)).compareTo("&gt;") == 0)
                ||
                (sbr.substring(i,(i+4)).compareTo("&lt;") == 0)
               )
             {
                contains_escaped = true ;
             }
          }

          if(contains_escaped == false)
          {
             if((i+4) < sbr.length() && sbr.substring(i, (i+5)).compareTo("&amp;") == 0)
             {
                contains_escaped = true ;
             }
          }
          
          if(contains_escaped == false)
          {
             // System.out.println("Index    :\t" + i + "\t" + sbr.charAt(i)) ;
             sbr.replace(i, (i+1), "&amp;") ;
             i+=4 ;
             // System.out.println("Added AMP:\t" + sbr.toString()) ;
             // System.out.println("Index    :\t" + i + "\t" + sbr.charAt(i)) ;
          }

        }
        else if (sbr.charAt(i) == '>')
        {
           sbr.replace(i, (i+1), "&gt;") ;
           i+=3 ;
           // System.out.println("Added GT :\t" + sbr.toString()) ;
        }

     }

     return sbr.toString();

   }
     
      

      

   //
   // main test
   //

   public static void main(String[] argv) 
   {
      StringReplace sr = new StringReplace() ;

      String in;
      try
      {
         in=argv[0];
      }
      catch(ArrayIndexOutOfBoundsException e)
      {
         in="Hello & World";
      }	

      System.out.println("Result: " + sr.LitWithEnt(in));

      return ;
   }

}


