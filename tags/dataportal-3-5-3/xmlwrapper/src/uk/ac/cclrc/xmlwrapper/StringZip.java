package uk.ac.cclrc.xmlwrapper ;

import java.io.*;
import java.util.zip.*;


public class StringZip
{

   // an overall buffer should do the job
   static byte[] buf ;

   //initialise static variables
   static
   {
      buf = new byte[2000000] ; //2,000,000 chars should be fine for each string
   }
   
   public static String byteOnly(String trg)
   {
      try
      {
	 int l;
	 Deflater comp=new Deflater();	     
	 comp.setInput(trg.getBytes());
	 comp.finish();

         l=comp.deflate(buf);
         Inflater decomp=new Inflater();	     
         decomp.setInput(buf,0,l);
     
         l=decomp.inflate(buf);
         return new String(buf,0,l);
      }
      catch(Exception e)
      {
         e.printStackTrace() ;
      }
         return "" ;
   }
   
   public static String compress(String trg)
   {
      if(trg != null)
      {
         try
         {
            int l;
            Deflater comp=new Deflater();	     
            comp.setInput(trg.getBytes());
            comp.finish();

            l=comp.deflate(buf);

           //System.out.print("Byte array: ");
           //for(int i=0;i<buf.length;++i)
           //{
           //   System.out.write(buf[i]);
	   //}
           //System.out.print("\n");

           return new String(buf,0,l,"ISO-8859-1");
         
         }
         catch(Exception e)
         {
            e.printStackTrace();
            return "";
         }
      }
      
      return trg ;
   }
   
   public static String decompress(String trg)
   {
      if(trg != null)
      {
         try
         {
            int l;
            Inflater decomp=new Inflater();	     
            decomp.setInput(trg.getBytes("ISO-8859-1"));
     
            l=decomp.inflate(buf);
            return new String(buf,0,l);
         }
         catch(Exception e)
         {
            e.printStackTrace();
            return "";
         }
      }

      return trg ;	
   }


   //
   // main test
   //

   public static void main(String[] argv) 
   {
      String in;
      try
      {
         in=argv[0];
      }
      catch(ArrayIndexOutOfBoundsException e)
      {
         in="Hello World";
      }	

      String comp;
      System.out.println("Compress/Decompress using byte arrays only:");
      System.out.println("Result: "+StringZip.byteOnly(in));
      System.out.println("");
      System.out.println("Compress/Decompress using String as storage:");
      System.out.println("Source: "+in);
      comp=StringZip.compress(in);
      System.out.println("Compressed: "+comp);
      in=StringZip.decompress(comp);
      System.out.println("Result: "+in);
   }

}


