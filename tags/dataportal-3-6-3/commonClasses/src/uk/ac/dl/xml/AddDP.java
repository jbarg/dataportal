/*
 * AddXml.java
 *
 * Created on 16 April 2002, 13:51
 */
package uk.ac.dl.xml;
import java.net.*;
import java.io.*;
import javax.xml.transform.*;
import javax.xml.transform.stream.*;
import javax.xml.parsers.*;
import org.w3c.dom.*;
import org.xml.sax.*;
import javax.xml.transform.dom.*;
import java.util.*;
/**
 *
 * @author  Glen Drinkwater
 * public class AddXml {
 *
 * /** Creates a new instance of AddXml */
public final class AddDP{

 /*   public static  Document add(org.jdom.Document doc1, org.jdom.Document doc2, org.jdom.Document doc3, org.jdom.Document doc4) throws  org.jdom.JDOMException {
        org.w3c.dom.Document docreturn = null;
        org.jdom.Document docnew = null;

            docnew = new org.jdom.Document(new org.jdom.Element("active"));
            org.jdom.Element root = docnew.getRootElement();
            root.setAttribute("value","start");


            org.jdom.Element data1 = doc1.getRootElement();
            List list1 = data1.getContent();
            org.jdom.Element data2 = doc2.getRootElement();
            List list2 = data2.getContent();
            org.jdom.Element data3 = doc3.getRootElement();
            List list3 = data3.getContent();
            org.jdom.Element data4 = doc4.getRootElement();
            List list4 = data4.getContent();

            Iterator it  = list1.iterator();
            while(it.hasNext()){
                Object o = it.next();
                if(o instanceof org.jdom.Element){
                    org.jdom.Element w = (org.jdom.Element)o;
                    it.remove();
                    root.addContent(w);
                }
                else if(o instanceof org.jdom.Text){
                    org.jdom.Text t = (org.jdom.Text)o;
                    it.remove();
                    root.addContent(t);
                }
            }

            Iterator it2 = list2.iterator();
            while(it2.hasNext()){
                Object o = it2.next();
                if(o instanceof org.jdom.Element){
                    org.jdom.Element w = (org.jdom.Element)o;
                    it2.remove();
                    root.addContent(w);
                }
                else if(o instanceof org.jdom.Text){
                    org.jdom.Text t = (org.jdom.Text)o;
                    it2.remove();
                    root.addContent(t);
                }
            }
            Iterator it3 = list3.iterator();
            while(it3.hasNext()){
                Object o = it3.next();
                if(o instanceof org.jdom.Element){
                    org.jdom.Element w = (org.jdom.Element)o;
                    it3.remove();
                    root.addContent(w);
                }
                else if(o instanceof org.jdom.Text){
                    org.jdom.Text t = (org.jdom.Text)o;
                    it3.remove();
                    root.addContent(t);
                }
            }
            Iterator it4 = list4.iterator();
            while(it4.hasNext()){
                Object o = it4.next();
                if(o instanceof org.jdom.Element){
                    org.jdom.Element w = (org.jdom.Element)o;
                    it4.remove();
                    root.addContent(w);
                }
                else if(o instanceof org.jdom.Text){
                    org.jdom.Text t = (org.jdom.Text)o;
                    it4.remove();
                    root.addContent(t);
                }
            }

            // org.jdom.output.DOMOutputter outputter = new org.jdom.output.DOMOutputter();
            //docreturn = outputter.output(docnew);
            docreturn = Converter.JDOMtoDOM(docnew);
            return docreturn;
          }


    public static Document add(org.jdom.Document doc1, org.jdom.Document doc2, org.jdom.Document doc3)throws org.jdom.JDOMException{
        org.w3c.dom.Document docreturn = null;
        org.jdom.Document docnew = null;

            docnew = new org.jdom.Document(new org.jdom.Element("active"));
            org.jdom.Element root = docnew.getRootElement();
            root.setAttribute("value","start");


            org.jdom.Element data1 = doc1.getRootElement();
            List list1 = data1.getContent();
            org.jdom.Element data2 = doc2.getRootElement();
            List list2 = data2.getContent();
            org.jdom.Element data3 = doc3.getRootElement();
            List list3 = data3.getContent();

            Iterator it  = list1.iterator();
            while(it.hasNext()){
                Object o = it.next();
                if(o instanceof org.jdom.Element){
                    org.jdom.Element w = (org.jdom.Element)o;
                    it.remove();
                    root.addContent(w);
                }
                else if(o instanceof org.jdom.Text){
                    org.jdom.Text t = (org.jdom.Text)o;
                    it.remove();
                    root.addContent(t);
                }
            }

            Iterator it2 = list2.iterator();
            while(it2.hasNext()){
                Object o = it2.next();
                if(o instanceof org.jdom.Element){
                    org.jdom.Element w = (org.jdom.Element)o;
                    it2.remove();
                    root.addContent(w);
                }
                else if(o instanceof org.jdom.Text){
                    org.jdom.Text t = (org.jdom.Text)o;
                    it2.remove();
                    root.addContent(t);
                }
            }
            Iterator it3 = list3.iterator();
            while(it3.hasNext()){
                Object o = it3.next();
                if(o instanceof org.jdom.Element){
                    org.jdom.Element w = (org.jdom.Element)o;
                    it3.remove();
                    root.addContent(w);
                }
                else if(o instanceof org.jdom.Text){
                    org.jdom.Text t = (org.jdom.Text)o;
                    it3.remove();
                    root.addContent(t);
                }
            }
            //  org.jdom.output.DOMOutputter outputter = new org.jdom.output.DOMOutputter();
            // docreturn = outputter.output(docnew);

            docreturn = Converter.JDOMtoDOM(docnew);
            return docreturn;




    }

    public static Document add(org.jdom.Document doc1, org.jdom.Document doc2)throws org.jdom.JDOMException{
        org.w3c.dom.Document docreturn = null;

            org.jdom.Document docnew = new org.jdom.Document(new org.jdom.Element("active"));
            org.jdom.Element root = docnew.getRootElement();
            root.setAttribute("value","start");

            org.jdom.Element data1 = doc1.getRootElement();
            List list1 = data1.getContent();
            org.jdom.Element data2 = doc2.getRootElement();
            List list2 = data2.getContent();


            Iterator it  = list1.iterator();
            while(it.hasNext()){
                Object o = it.next();
                if(o instanceof org.jdom.Element){
                    org.jdom.Element w = (org.jdom.Element)o;
                    it.remove();
                    root.addContent(w);
                }
                else if(o instanceof org.jdom.Text){
                    org.jdom.Text t = (org.jdom.Text)o;
                    it.remove();
                    root.addContent(t);
                }
            }

            Iterator it2 = list2.iterator();
            while(it2.hasNext()){
                Object o = it2.next();
                if(o instanceof org.jdom.Element){
                    org.jdom.Element w = (org.jdom.Element)o;
                    it2.remove();
                    root.addContent(w);
                }
                else if(o instanceof org.jdom.Text){
                    org.jdom.Text t = (org.jdom.Text)o;
                    it2.remove();
                    root.addContent(t);
                }
            }

            // org.jdom.output.DOMOutputter outputter = new org.jdom.output.DOMOutputter();
            //  docreturn = outputter.output(docnew);

            docreturn = Converter.JDOMtoDOM(docnew);
            return docreturn;


    }*/
}
