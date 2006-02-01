/*
 * BuidingTest.java
 *
 * Created on 02 April 2003, 10:00
 */

//package xml-dl.unitTest;

import java.beans.*;
import java.io.*;
import ac.dl.xml.*;
import org.jdom.*;
import org.jdom.input.*;
import javax.xml.parsers.*;
/**
 *
 * @author  gjd37
 */
public class BuildingTest {
    
    
    /** Creates new BuidingTest */
    public BuildingTest() {
        try{
            DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
            dbf.setIgnoringElementContentWhitespace(true);
            DocumentBuilder db = dbf.newDocumentBuilder();
            org.w3c.dom.Document dom = db.parse("xml/users2.xml");
            
             FileWriter filew = new FileWriter("xml/dom.xml");
       
            org.jdom.Document doc = Converter.DOMtoJDOM(dom);
            org.jdom.output.XMLOutputter outputter = new org.jdom.output.XMLOutputter("  ", true);
            outputter.output(doc, filew);
            filew.close();
            //org.w3c.dom.Document dom = XML_DOMBuilder.build(new File("xml/users2.xml"));
            //System.out.println(dom.toString());
            //org.w3c.dom.Element dom_el = dom.getDocumentElement();
           // Saver.save(dom,"xml/dom.xml");
            
            //try to make jdom
          /*  Document doc= new Document();
            DOMBuilder builder = new DOMBuilder();
            Element jdom_el = builder.build(dom_el);
            jdom_el.detach();
            // System.out.println(jdom_el.getParent());
            //Element name = jdom_el.setDocument(doc);
            // System.out.println(name);
            org.jdom.Document doc2 = new org.jdom.Document(jdom_el);
            //     System.out.println(doc2.setRootElement(jdom_el));
            Saver.save(doc2,"xml/jdom.xml");*/
            
        }
        catch (Exception e){
            System.out.println(e);
        }
        
    }
    
    
    public static void main(String[] args){
        new BuildingTest();
    }
    
}
