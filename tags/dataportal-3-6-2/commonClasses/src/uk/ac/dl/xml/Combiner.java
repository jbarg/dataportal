package uk.ac.dl.xml;

import java.io.*;
import java.util.*;
import org.apache.log4j.*;

/** This combines xml documents together.  Takes the document
 * element from each one and adds them all together under the
 * root element given.  Each document must be off the JDOM type,
 * but can use the Converter class to convert a DOM document.  The
 * results can be given back as either JDOM or DOM.
 *
 * @author Glen Drinkwater
 * @version 1.1
 */
public final class Combiner{
    
    private static Logger logger;
    
    static {
        logger = Logger.getLogger(Combiner.class);
        //BasicConfigurator.configure();
    }
    
    /** Combines an array of JDOM documents and returns as a w3c DOM
     * document with root element given.
     * @return org.w3c.dom.Document
     * @param doc1 w3c document
     * @param doc2 w3c document
     * @param rootElement root element of xml returned back.
     * @throws JDOMException When exception thrown during parsing
     */
    public static org.w3c.dom.Document build(org.jdom.Document[] docs, String rootElement) throws XMLException{
        
        for(int i = 0; i < docs.length;i++){
            if(docs[i] ==null){
                throw new XMLException("Cannot combine one or more null documents");
            }
        }
        if(!Character.isLetter(rootElement.charAt(0))) throw new XMLException("Root element name must begin with a letter");
        
        org.w3c.dom.Document docreturn = null;
        
        org.jdom.Document docnew = new org.jdom.Document(new org.jdom.Element(rootElement));
        org.jdom.Element root = docnew.getRootElement();
        
        for(int i = 0;i<docs.length;i++){
            org.jdom.Element data1 = docs[i].getRootElement();
            List list1 = data1.getContent();
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
        }
        
        // org.jdom.output.DOMOutputter outputter = new org.jdom.output.DOMOutputter();
        //docreturn = outputter.output(docnew);
        
        docreturn  =  Converter.JDOMtoDOM(docnew);
        
        return docreturn;
        
        
        
    }
    
    /** Combines an array of file representing a JDOM documents and returns as a w3c DOM
     * document with root element given.
     * @return org.w3c.dom.Document
     * @param doc1 w3c document
     * @param doc2 w3c document
     * @param rootElement root element of xml returned back.
     * @throws JDOMException When exception thrown during parsing
     */
    public static org.w3c.dom.Document build(File[] files, String rootElement) throws XMLException{
        
        for(int i = 0; i < files.length;i++){
            if(!files[i].exists()){
                throw new XMLException("Cannot combine one or more null files");
            }
        }
        if(!Character.isLetter(rootElement.charAt(0))) throw new XMLException("Root element name must begin with a letter");
        
        org.jdom.Document[] docs = new org.jdom.Document[(files.length)];
        org.jdom.input.SAXBuilder builder = new org.jdom.input.SAXBuilder();
        
        try{
            for(int i = 0;i < files.length;i++){
                docs[i] =  builder.build(files[i]);
            }
            
            org.w3c.dom.Document docreturn = build(docs, rootElement);
            
            return docreturn;
        }
        catch(org.jdom.JDOMException e){
            throw new XMLException(e.toString());
        }
        catch(IOException e){
            throw new XMLException(e.toString());
        }
        
        
    }
    
    /** Combines 2 JDOM documents and returns as a w3c DOM
     * document with root element given.
     * @return org.w3c.dom.Document
     * @param doc1 w3c document
     * @param doc2 w3c document
     * @param rootElement root element of xml returned back.
     * @throws JDOMException When exception thrown during parsing
     * @deprecated Deprecated in 1.4.1.  Use build(Document[] .. or build(File[] ..
     */
    public static org.w3c.dom.Document build(org.jdom.Document doc1, org.jdom.Document doc2, String rootElement) throws Exception, org.jdom.JDOMException{
        
        if(doc1==null || doc2==null) throw new XMLException("Cannot combine one or more null documents");
        if(!Character.isLetter(rootElement.charAt(0))) throw new XMLException("Root element name must begin with a letter");
        
        org.w3c.dom.Document docreturn = null;
        
        org.jdom.Document docnew = new org.jdom.Document(new org.jdom.Element(rootElement));
        org.jdom.Element root = docnew.getRootElement();
        
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
        //docreturn = outputter.output(docnew);
        
        docreturn  =  Converter.JDOMtoDOM(docnew);
        
        return docreturn;
        
        
        
        
        
    }
    
    /** Combines 3 JDOM documents and returns as a w3c DOM
     * document with root element given.
     * @return org.w3c.dom.Document
     * @param doc1 w3c document
     * @param doc2 w3c document
     * @param doc3 w3c document
     * @param rootElement root element of xml returned back.
     * @throws JDOMException When exception thrown during parsing
     * @deprecated Deprecated in 1.4.1.  Use build(Document[] .. or build(File[] ..
     */
    public static org.w3c.dom.Document build(org.jdom.Document doc1, org.jdom.Document doc2, org.jdom.Document doc3, String rootElement) throws Exception{
        
        if(doc1==null || doc2==null || doc3==null) throw new org.jdom.JDOMException("Cannot combine one or more null documents");
        if(!Character.isLetter(rootElement.charAt(0))) throw new org.jdom.JDOMException("Root element name must begin with a letter");
        
        org.w3c.dom.Document docreturn = null;
        
        org.jdom.Document docnew = new org.jdom.Document(new org.jdom.Element(rootElement));
        org.jdom.Element root = docnew.getRootElement();
        
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
        // org.jdom.output.DOMOutputter outputter = new org.jdom.output.DOMOutputter();
        //docreturn = outputter.output(docnew);
        docreturn  =  Converter.JDOMtoDOM(docnew);
        
        return docreturn;
        
        
    }
    /** Combines 4 JDOM documents and returns as a w3c DOM
     * document with root element given.
     * @return org.w3c.dom.Document
     * @param doc1 w3c document
     * @param doc2 w3c document
     * @param doc3 w3c document
     * @param doc4 w3c document
     * @param rootElement root element of xml returned back.
     * @throws JDOMException When exception thrown during parsing
     * @deprecated Deprecated in 1.4.1.  Use build(Document[] .. or build(File[] ..
     */
    public static org.w3c.dom.Document build(org.jdom.Document doc1, org.jdom.Document doc2, org.jdom.Document doc3, org.jdom.Document doc4, String rootElement) throws Exception{
        
        if(doc1==null || doc2==null || doc3==null|| doc4==null) throw new org.jdom.JDOMException("Cannot combine one or more null documents");
        if(!Character.isLetter(rootElement.charAt(0))) throw new org.jdom.JDOMException("Root element name must begin with a letter");
        
        org.w3c.dom.Document docreturn = null;
        
        org.jdom.Document docnew = new org.jdom.Document(new org.jdom.Element(rootElement));
        org.jdom.Element root = docnew.getRootElement();
        
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
        docreturn  =  Converter.JDOMtoDOM(docnew);
        
        return docreturn;
        
        
        
    }
    
    /** Combines 5 JDOM documents and returns as a w3c DOM
     * document with root element given.
     * @return org.w3c.dom.Document
     * @param doc1 w3c document
     * @param doc2 w3c document
     * @param doc3 w3c document
     * @param doc4 w3c document
     * @param doc5 w3c document
     * @param rootElement root element of xml returned back.
     * @throws JDOMException When exception thrown during parsing
     * @deprecated Deprecated in 1.4.1.  Use build(Document[] .. or build(File[] ..
     */
    public static org.w3c.dom.Document build(org.jdom.Document doc1, org.jdom.Document doc2, org.jdom.Document doc3, org.jdom.Document doc4, org.jdom.Document doc5, String rootElement) throws Exception{
        
        if(doc1==null || doc2==null || doc3==null|| doc4==null|| doc5==null) throw new org.jdom.JDOMException("Cannot combine one or more null documents");
        if(!Character.isLetter(rootElement.charAt(0))) throw new org.jdom.JDOMException("Root element name must begin with a letter");
        
        org.w3c.dom.Document docreturn = null;
        
        org.jdom.Document docnew = new org.jdom.Document(new org.jdom.Element(rootElement));
        org.jdom.Element root = docnew.getRootElement();
        
        org.jdom.Element data1 = doc1.getRootElement();
        List list1 = data1.getContent();
        org.jdom.Element data2 = doc2.getRootElement();
        List list2 = data2.getContent();
        org.jdom.Element data3 = doc3.getRootElement();
        List list3 = data3.getContent();
        org.jdom.Element data4 = doc4.getRootElement();
        List list4 = data4.getContent();
        org.jdom.Element data5 = doc5.getRootElement();
        List list5 = data5.getContent();
        
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
        
        Iterator it5 = list5.iterator();
        while(it5.hasNext()){
            Object o = it5.next();
            if(o instanceof org.jdom.Element){
                org.jdom.Element w = (org.jdom.Element)o;
                it5.remove();
                root.addContent(w);
            }
            else if(o instanceof org.jdom.Text){
                org.jdom.Text t = (org.jdom.Text)o;
                it5.remove();
                root.addContent(t);
            }
        }
        // org.jdom.output.DOMOutputter outputter = new org.jdom.output.DOMOutputter();
        //docreturn = outputter.output(docnew);
        
        docreturn  =  Converter.JDOMtoDOM(docnew);
        return docreturn;
        
        
        
        
    }//end of 5
    
    
    /** Combines 6 JDOM documents and returns as a w3c DOM
     * document with root element given.
     * @return org.w3c.dom.Document
     * @param doc1 w3c document
     * @param doc2 w3c document
     * @param doc3 w3c document
     * @param doc4 w3c document
     * @param doc5 w3c document
     * @param doc6 w3c document
     * @param rootElement root element of xml returned back.
     * @throws JDOMException When exception thrown during parsing
     * @deprecated Deprecated in 1.4.1.  Use build(Document[] .. or build(File[] ..
     */
    public static org.w3c.dom.Document build(org.jdom.Document doc1, org.jdom.Document doc2, org.jdom.Document doc3, org.jdom.Document doc4, org.jdom.Document doc5, org.jdom.Document doc6, String rootElement) throws Exception{
        
        if(doc1==null || doc2==null || doc3==null|| doc4==null|| doc5==null|| doc6==null) throw new  org.jdom.JDOMException("Cannot combine one or more null documents");
        if(!Character.isLetter(rootElement.charAt(0))) throw new org.jdom.JDOMException("Root element name must begin with a letter");
        
        org.w3c.dom.Document docreturn = null;
        
        org.jdom.Document docnew = new org.jdom.Document(new org.jdom.Element(rootElement));
        org.jdom.Element root = docnew.getRootElement();
        
        org.jdom.Element data1 = doc1.getRootElement();
        List list1 = data1.getContent();
        org.jdom.Element data2 = doc2.getRootElement();
        List list2 = data2.getContent();
        org.jdom.Element data3 = doc3.getRootElement();
        List list3 = data3.getContent();
        org.jdom.Element data4 = doc4.getRootElement();
        List list4 = data4.getContent();
        org.jdom.Element data5 = doc5.getRootElement();
        List list5 = data5.getContent();
        org.jdom.Element data6 = doc6.getRootElement();
        List list6 = data6.getContent();
        
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
        
        Iterator it5 = list5.iterator();
        while(it5.hasNext()){
            Object o = it5.next();
            if(o instanceof org.jdom.Element){
                org.jdom.Element w = (org.jdom.Element)o;
                it5.remove();
                root.addContent(w);
            }
            else if(o instanceof org.jdom.Text){
                org.jdom.Text t = (org.jdom.Text)o;
                it5.remove();
                root.addContent(t);
            }
        }
        
        Iterator it6 = list6.iterator();
        while(it6.hasNext()){
            Object o = it6.next();
            if(o instanceof org.jdom.Element){
                org.jdom.Element w = (org.jdom.Element)o;
                it6.remove();
                root.addContent(w);
            }
            else if(o instanceof org.jdom.Text){
                org.jdom.Text t = (org.jdom.Text)o;
                it6.remove();
                root.addContent(t);
            }
        }
        ///org.jdom.output.DOMOutputter outputter = new org.jdom.output.DOMOutputter();
        //docreturn = outputter.output(docnew);
        docreturn  =  Converter.JDOMtoDOM(docnew);
        
        return docreturn;
        
        
        
    }//end of 6
    
    /** Combines 3 JDOM documents and returns as a org.jdomDocument
     * document with root element given.
     * @return org.w3c.dom.Document
     * @param doc1 w3c document
     * @param doc2 w3c document
     * @param doc3 w3c document
     * @param rootElement root element of xml returned back.
     * @deprecated Deprecated in 1.4.1.  Use build(Document[] .. or build(File[] ..
     */
    public static org.jdom.Document buildJDOM(org.jdom.Document doc1, org.jdom.Document doc2, org.jdom.Document doc3,String rootElement) throws Exception{
        
        if(doc1==null || doc2==null) throw new org.jdom.JDOMException("Cannot combine one or more null documents");
        if(!Character.isLetter(rootElement.charAt(0))) throw new org.jdom.JDOMException("Root element name must begin with a letter");
        
        org.w3c.dom.Document docreturn = null;
        
        org.jdom.Document docnew = new org.jdom.Document(new org.jdom.Element(rootElement));
        org.jdom.Element root = docnew.getRootElement();
        
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
        
        return docnew;
        
        
    }
    
    
    
    
    /** Combines 4 JDOM documents and returns as a org.jdomDocument
     * document with root element given.
     * @return org.w3c.dom.Document
     * @param doc1 w3c document
     * @param doc2 w3c document
     * @param doc3 w3c document
     * @param doc4 w3c document
     * @param rootElement root element of xml returned back.
     * @deprecated Deprecated in 1.4.1.  Use build(Document[] .. or build(File[] ..
     */
    public static org.jdom.Document buildJDOM(org.jdom.Document doc1, org.jdom.Document doc2, org.jdom.Document doc3,  org.jdom.Document doc4, String rootElement) throws Exception{
        
        if(doc1==null || doc2==null || doc3==null) throw new org.jdom.JDOMException("Cannot combine one or more null documents");
        if(!Character.isLetter(rootElement.charAt(0))) throw new org.jdom.JDOMException("Root element name must begin with a letter");
        
        org.w3c.dom.Document docreturn = null;
        
        org.jdom.Document docnew = new org.jdom.Document(new org.jdom.Element(rootElement));
        org.jdom.Element root = docnew.getRootElement();
        
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
        
        
        return docnew;
        
        
    }
    
    /** Combines 5 JDOM documents and returns as a org.jdomDocument
     * document with root element given.
     * @return org.w3c.dom.Document
     * @param doc1 w3c document
     * @param doc2 w3c document
     * @param doc3 w3c document
     * @param doc4 w3c document
     * @param doc5 w3c document
     * @param rootElement root element of xml returned back.
     * @deprecated Deprecated in 1.4.1.  Use build(Document[] .. or build(File[] ..
     */
    public static org.jdom.Document buildJDOM(org.jdom.Document doc1, org.jdom.Document doc2, org.jdom.Document doc3,  org.jdom.Document doc4, org.jdom.Document doc5, String rootElement) throws Exception{
        if(doc1==null || doc2==null || doc3==null|| doc4==null|| doc5==null) throw new org.jdom.JDOMException("Cannot combine one or more null documents");
        if(!Character.isLetter(rootElement.charAt(0))) throw new org.jdom.JDOMException("Root element name must begin with a letter");
        
        org.w3c.dom.Document docreturn = null;
        
        org.jdom.Document docnew = new org.jdom.Document(new org.jdom.Element(rootElement));
        org.jdom.Element root = docnew.getRootElement();
        
        org.jdom.Element data1 = doc1.getRootElement();
        List list1 = data1.getContent();
        org.jdom.Element data2 = doc2.getRootElement();
        List list2 = data2.getContent();
        org.jdom.Element data3 = doc3.getRootElement();
        List list3 = data3.getContent();
        org.jdom.Element data4 = doc4.getRootElement();
        List list4 = data4.getContent();
        org.jdom.Element data5 = doc5.getRootElement();
        List list5 = data5.getContent();
        
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
        
        Iterator it5 = list5.iterator();
        while(it5.hasNext()){
            Object o = it5.next();
            if(o instanceof org.jdom.Element){
                org.jdom.Element w = (org.jdom.Element)o;
                it5.remove();
                root.addContent(w);
            }
            else if(o instanceof org.jdom.Text){
                org.jdom.Text t = (org.jdom.Text)o;
                it5.remove();
                root.addContent(t);
            }
        }
        
        return docnew;
        
    }//end of 5
    
    /** Combines 6 JDOM documents and returns as a org.jdomDocument
     * document with root element given.
     * @return org.w3c.dom.Document
     * @param doc1 w3c document
     * @param doc2 w3c document
     * @param doc3 w3c document
     * @param doc4 w3c document
     * @param doc5 w3c document
     * @param doc6 w3c document
     * @param rootElement root element of xml returned back.
     * @deprecated Deprecated in 1.4.1.  Use build(Document[] .. or build(File[] ..
     *
     */
    public static org.jdom.Document buildJDOM(org.jdom.Document doc1, org.jdom.Document doc2, org.jdom.Document doc3,  org.jdom.Document doc4, org.jdom.Document doc5 , org.jdom.Document doc6, String rootElement) throws Exception{
        
        if(doc1==null || doc2==null || doc3==null|| doc4==null|| doc5==null||doc6==null) throw new org.jdom.JDOMException("Cannot combine one or more null documents");
        if(!Character.isLetter(rootElement.charAt(0))) throw new org.jdom.JDOMException("Root element name must begin with a letter");
        
        org.jdom.Document docnew = null;
        
        docnew = new org.jdom.Document(new org.jdom.Element(rootElement));
        org.jdom.Element root = docnew.getRootElement();
        
        org.jdom.Element data1 = doc1.getRootElement();
        List list1 = data1.getContent();
        org.jdom.Element data2 = doc2.getRootElement();
        List list2 = data2.getContent();
        org.jdom.Element data3 = doc3.getRootElement();
        List list3 = data3.getContent();
        org.jdom.Element data4 = doc4.getRootElement();
        List list4 = data4.getContent();
        org.jdom.Element data5 = doc5.getRootElement();
        List list5 = data5.getContent();
        org.jdom.Element data6 = doc6.getRootElement();
        List list6 = data6.getContent();
        
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
        
        Iterator it5 = list5.iterator();
        while(it5.hasNext()){
            Object o = it5.next();
            if(o instanceof org.jdom.Element){
                org.jdom.Element w = (org.jdom.Element)o;
                it5.remove();
                root.addContent(w);
            }
            else if(o instanceof org.jdom.Text){
                org.jdom.Text t = (org.jdom.Text)o;
                it5.remove();
                root.addContent(t);
            }
        }
        
        Iterator it6 = list6.iterator();
        while(it6.hasNext()){
            Object o = it6.next();
            if(o instanceof org.jdom.Element){
                org.jdom.Element w = (org.jdom.Element)o;
                it6.remove();
                root.addContent(w);
            }
            else if(o instanceof org.jdom.Text){
                org.jdom.Text t = (org.jdom.Text)o;
                it6.remove();
                root.addContent(t);
            }
        }
        return docnew;
    }
}