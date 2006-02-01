
import java.io.*;
import java.text.*;
import java.util.*;

import java.sql.*;
import java.net.*;
import javax.xml.transform.sax.*;
import javax.xml.transform.*;
import javax.xml.transform.stream.*;
import org.xml.sax.helpers.DefaultHandler;
import org.w3c.dom.*;
import org.xml.sax.*;
import javax.xml.parsers.SAXParserFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.FactoryConfigurationError;
import javax.xml.parsers.ParserConfigurationException;
import ac.dl.xml.*;
import javax.xml.transform.dom.*;



public class urlconparserTEST {
    
    long finish = 0;
    long start = 0;
    
    public static void main(String[] args) {
        
        if(args == null || args.length !=1) {
            
            System.out.println("Usage is :");
            System.out.println("1 for dom then to jdom");
            System.out.println("2 for dom");
            System.out.println("5 for sax");
            System.out.println("3 for jdom dom");
            System.out.println("4 for jom sax");
            System.exit(0);
            return;
        }
        else{
            String a = args[0];
            new urlconparserTEST(a);
            
            /*DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
            DocumentBuilder db = dbf.newDocumentBuilder();
            Document document = db.parse(new FileInputStream("c:/testMPIM.xml"));*/
            
            
            //get sax parser
            
            
            //Element i1 = document.getDocumentElement();
            
            //System.out.println(i1);
            //FileWriter read = new FileWriter("c:/test2BADC.xml");
            // String inputLine;
            
            // while ((inputLine = in.readLine()) != null){
            //     System.out.println(inputLine);
            //     read.write(inputLine);
            
            // }
            //read.close();
            
/*
       //DOMSource s1 = new DOMSource(i1);
SAXSource s1 = new SAXSource(new InputSource(new FileReader("c:/testMPIM.xml")));
        StreamSource s2 = new StreamSource(new File("datasettable.xsl"));
 
        //stylesheet defined buy users preferance
      //  StreamSource s2 = new StreamSource("C:/old_tomcat stuff/experiment.xsl");
 
 
 
 
        //save result as inputfile.txt for workbench to read
        StreamResult r2 = new StreamResult(new File("datasettable.html"));
 
        //get new instance
        SAXTransformerFactory tfactory1 = (SAXTransformerFactory)TransformerFactory.newInstance();
 
        //assign the stylesheet to the factory
        Transformer transformer1 = tfactory1.newTransformer(s2);
 
        //transform xml
        transformer1.transform(s1,r2);*/
        }
    }
    
    public urlconparserTEST(String s){
        try{
            InputStream input=  new FileInputStream("users.xml");
            DocumentBuilderFactory dbf1 = DocumentBuilderFactory.newInstance();
            DocumentBuilder db1 = dbf1.newDocumentBuilder();
            Document document1 = db1.parse(input);
            Runtime run = Runtime.getRuntime();
            
            long tot = run.freeMemory();
            System.out.println(tot);
            
            start = System.currentTimeMillis();
            //URL url = new URL("http://www.admin.rl.ac.uk/clrcdataportal/returnxml.asp?where=%20StudyName%20like'%s%';UserID=dpuser");
            //URLConnection y1 = url.openConnection();
            //InputStream input = y1.getInputStream();
            
            
            if(s.equals("1")){
                //dom
                DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
                DocumentBuilder db = dbf.newDocumentBuilder();
                Document document = db.parse(input);
                //	Document document1 = db.newDocument();
                
                
                /*javax.xml.transform.dom.DOMSource dom = new javax.xml.transform.dom.DOMSource(document);
                 
                        java.io.FileInputStream input1 = new java.io.FileInputStream(new File("experiment.xsl"));
                 
                        javax.xml.transform.stream.StreamSource stream = new javax.xml.transform.stream.StreamSource(input1);
                 
                        javax.xml.transform.dom.DOMResult res = new javax.xml.transform.dom.DOMResult(document1);
                 
                        javax.xml.transform.TransformerFactory tfactory1 = javax.xml.transform.TransformerFactory.newInstance();
                 
                        javax.xml.transform.Transformer transformer1 = tfactory1.newTransformer(stream);
                 
                        transformer1.transform(dom,res);*/
                
                finish = System.currentTimeMillis();
                
                System.out.println("converting, "+(finish - start));
                Converter.DOMtoJDOM(document);
                
                
                
                
                System.out.println("mem taken is "+(tot-run.freeMemory()));
                
                
            }
            
            if(s.equals("2")){
                //dom
                DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
                DocumentBuilder db = dbf.newDocumentBuilder();
                Document document = db.parse(input);
                //Document document1 = db.newDocument();
                
                                        /*javax.xml.transform.stream.StreamSource ss = new javax.xml.transform.stream.StreamSource(input);
                                         
                                        java.io.FileInputStream input1 = new java.io.FileInputStream(new File("experiment.xsl"));
                                         
                                        javax.xml.transform.stream.StreamSource stream = new javax.xml.transform.stream.StreamSource(input1);
                                         
                                        javax.xml.transform.stream.StreamResult res = new javax.xml.transform.stream.StreamResult(System.out);
                                         
                                        javax.xml.transform.TransformerFactory tfactory1 = javax.xml.transform.TransformerFactory.newInstance();
                                         
                                        javax.xml.transform.Transformer transformer1 = tfactory1.newTransformer(stream);
                                         
                                        transformer1.transform(ss,res);*/
                
                
                
                
                System.out.println("mem taken is "+(tot-run.freeMemory()));
                
                
            }
            else if(s.equals("5")){
                // DefaultHandler handler = new Echo();
                
                
                SAXParserFactory sax = SAXParserFactory.newInstance();
                SAXParser saxp = sax.newSAXParser();
                //saxp.parse(input);
                //saxp.parse(new File("users.xml"),handler);
                System.out.println("mem taken is "+(tot-run.freeMemory()));
                
            }
            else if(s.equals("3")){
                
                org.jdom.input.DOMBuilder sax = new org.jdom.input.DOMBuilder();
                //org.jdom.Document doc = sax.build("users.xml");
                org.jdom.Document doc = sax.build(input);
                //Saver.save(doc,new File("saved.xml"));
                System.out.println("mem taken is "+(tot-run.freeMemory()));
                
                
            }
            else if(s.equals("4")){
                
                org.jdom.input.SAXBuilder sax = new org.jdom.input.SAXBuilder();
                //org.jdom.Document doc = sax.build("users.xml");
                org.jdom.Document doc = sax.build(input);
                //Saver.save(doc,new File("saved.xml"));
                System.out.println("mem taken is "+(tot-run.freeMemory()));
                
                
            }
            
            else if(s.equals("tojdom")){
                org.jdom.Document doc = Converter.DOMtoJDOM(document1, "hello.xml");
                System.out.println(doc);
                System.out.println("mem taken is "+(tot-run.freeMemory()));
                
                
            }
            else if(s.equals("todom")){
                org.jdom.Document doc = Converter.DOMtoJDOM(document1);
                System.out.println(doc);
                System.out.println("mem taken is "+(tot-run.freeMemory()));
                
                
            }
            
        }
        catch(Exception e){
            System.out.println(e.toString());}
        
        
        finish = System.currentTimeMillis();
        System.out.println("time taken is "+(finish - start));
        
        
        
        
        
    }
}