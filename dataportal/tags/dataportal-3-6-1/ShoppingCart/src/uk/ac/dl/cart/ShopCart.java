package uk.ac.dl.cart;
import uk.ac.dl.xml.*;
import java.security.cert.*;
import org.jdom.*;
import org.jdom.input.*;
import java.util.*;
import java.sql.*;
import java.io.*;
import org.apache.log4j.*;


public class ShopCart {
    
    //set static log for the class
    static Logger logger = Logger.getLogger(ShopCart.class);
    
    static {
         //locate the prop file.  Normal get this from web.xml file
        PropertyConfigurator.configure(Config.getContextPath()+"logger.properties");
        
    }
    
    public org.w3c.dom.Element getCart(String sid) throws Exception{
        //locate the prop file.  Normal get this from web.xml file
        
        //System.out.println("path given is  "+Config.getContextPath());
        // String dn = sid;
        if(sid == null){
            //is cert valid.Check also the cert issur against the dn
            //dn = cert.getSubjectDN().getName();
        }
        else{
            //go to session module.  See if sid is valid. If so, get the associated dn
            //here we would compare the 2 dns
            //if false, throw exception.  Assume true for now
            //  dn = cert.getSubjectDN().getName();
        }
        
        Document doc=null;
        org.w3c.dom.Element el = null;
        try{
            doc= Cart.getDoc(sid);
            if(doc == null){
                doc = new Document(new Element("shoppingCart"));
            }
            //change to w3c and send back the xml
            org.w3c.dom.Document document = Converter.JDOMtoDOM(doc);
            el = document.getDocumentElement();
            
        }
        catch(SessionException e){
            logger.warn("Error with the session manager",e);
            throw e;
        }
        catch(CartException e){
            logger.warn("Error with the shopping cart",e);
            throw e;
        }
        catch(Exception e){
            logger.warn("Error with the shopping cart",e);
            throw e;
        }
        
        
        return el;
    }
    
    /** Creates a new instance of AddToCart */
    public Boolean addToCart(String sid, org.w3c.dom.Element element)throws Exception {
        
       
        //String dn = sid;
        try{
            DOMBuilder builder = new DOMBuilder();
            
            Element el = builder.build(element);
            Document doc = null;
            
            
            doc = Cart.getDoc(sid);
            System.out.println("cart is "+doc);
            if(doc == null){
                doc = new Document(new Element("shoppingCart"));
            }
            HashMap alist  = new HashMap();
            
            Element root = doc.getRootElement();
            List list1 = root.getContent();
            Iterator iter  = list1.iterator();
            
            //get a string array of the urls
            while(iter.hasNext()){
                Object o = iter.next();
                if(o instanceof Element){
                    Element w = (Element)o;
                    alist.put(w.getAttributeValue("url"),w.getAttributeValue("url"));
                    
                }
            }
            
            //System.out.println("1");
            List list = el.getChildren("item");
            Iterator it  = list.iterator();
            // System.out.println("2");
            while(it.hasNext()){
                Object o = it.next();
                if(o instanceof Element){
                    Element w = (Element)o;
                    String w_url = w.getAttributeValue("url");
                    //go through alist a see if url is there
                    
                    if(alist.isEmpty()){
                        //w.detach();
                        it.remove();
                        root.addContent(w);
                        alist.put(w.getAttributeValue("url"),w.getAttributeValue("url"));
                    }
                    else{
                        
                        
                        if(alist.containsKey(w_url)){
                            System.out.println("already have "+w_url);
                        }
                        else{
                            it.remove();
                            // w.detach();
                            root.addContent(w);
                        }
                        
                    }
                }
            }
            //Saver.save(doc,"c:/addniote.xml");
            Cart.insertDoc(sid,doc);
        }  catch(SessionException e){
            logger.warn("Error with the session manager",e);
            throw e;
        }
        catch(CartException e){
            logger.warn("Error with the shopping cart",e);
            throw e;
        }
        catch(Exception e){
            logger.warn("Error with the shopping cart",e);
            throw e;
        }
        
        
        
        
        
        
        return new Boolean(true);
    }
    
    /** Creates a new instance of RemoveFromCart */
    public Boolean removeFromCart(String sid,String[] url) throws Exception{
        //locate the prop file.  Normal get this from web.xml file
        
        Document doc = null;
        Connection myConn = null;
        
        
        try{
            doc = Cart.getDoc(sid);
            
            //got the document now remove the item.
            org.jdom.Element el = doc.getRootElement();
            List list = el.getChildren("item");
            
            
            Iterator it  = list.iterator();
            // System.out.println("size if children is "+list.size());
            while(it.hasNext()){
                Object o = it.next();
                if(o instanceof Element){
                    Element w = (Element)o;
                    if(w.getName().equals("item")){
                        System.out.println("element name is "+w.getName()+" with url "+w.getAttributeValue("url")+" looking for url "+url);
                        for(int j= 0; j< url.length;j++){
                            System.out.println(w.getAttributeValue("url")+"is the db url, looking for "+url[j]);
                            if(w.getAttributeValue("url").equals(url[j])){
                                System.out.println("yes");
                                it.remove();
                            }
                        }
                    }
                }
            }
            //put it back in
            Cart.insertDoc(sid,doc);
            
        }
        
        catch(SessionException e){
            logger.warn("Error with the session manager",e);
            throw e;
        }
        catch(CartException e){
            logger.warn("Error with the shopping cart",e);
            throw e;
        } catch(Exception e){
            logger.warn("Error with the shopping cart",e);
            throw e;
        }
        
        
        return new Boolean(true);
        
    }
    
    /** Creates a new instance of GetNotes */
    public String[] getNote(String url,String sid) throws Exception{
        //locate the prop file.  Normal get this from web.xml file
          
        String[] ret = new String [6];
        
        Document doc=null;
        try{
            doc = Cart.getDoc(sid);
            //Saver.save(doc, "c:/hetNporte.xml");
            //cart class odes the password thing
            /*if(!passwordDB.equals(password)){
                throw new Exception("Incorrect password");
            }
            else{*/
            
            Element root = doc.getRootElement();
            
            List list = root.getChildren();
            
            Iterator it = list.iterator();
            while(it.hasNext()){
                Object ob = (Object)it.next();
                if( ob instanceof Element){
                    Element el = (Element)ob;
                    //System.out.println("url for notes is "+url+" url found is "+el.getAttributeValue("url"));
                    if(el.getAttributeValue("url").equals(url)){
                        Element el2 = el.getChild("notes");
                        //System.out.println("notes for see notes is "+el2.getText());
                        ret[0] =  el2.getText();
                        Element el4 = el.getChild("name");
                        ret[1] = el4.getText();
                        
                        //now get the query
                        Element query = el.getChild("query");
                        
                        Element fac = query.getChild("facility");
                        ret[2] = fac.getText();
                        Element dis = query.getChild("discipline");
                        ret[3] = dis.getText();
                        Element wait = query.getChild("wait");
                        ret[4] = wait.getText();
                        
                        //get associated study
                        Element assStudy = el.getChild("assStudy");
                        ret[5] = assStudy.getText();
                        
                        // System.out.println("name for see notes is "+el4.getText());
                        //Element el3 = el.getChild("url");
                        //System.out.println("url for see notes is "+el3.getText());
                    }
                }
            }
        }
        
        catch(SessionException e){
            logger.warn("Error with the session manager",e);
            throw e;
        }
        catch(CartException e){
            logger.warn("Error with the shopping cart",e);
            throw e;
        }
        catch(Exception e){
            logger.warn("Error with the shopping cart",e);
            throw e;
        }
        
        if(ret[0] == null) ret[0] = "";
        
        return ret;
    }
    
    /** Creates a new instance of AddNote */
    public Boolean addNote(String sid,String notes,String url)throws Exception{
        //locate the prop file.  Normal get this from web.xml file
          
        Document doc = null;
        Connection myConn = null;
        
        try{
            doc = Cart.getDoc(sid);
            
        }
        catch(Exception e){
            //ystem.out.println("getting the doc out of the database "+e);
            throw new Exception(e);
        }
        
        //got the document, now add the note
        Element root = doc.getRootElement();
        List list = root.getChildren();
        Iterator it = list.iterator();
        while(it.hasNext()){
            Object ob = it.next();
            if(ob instanceof Element){
                Element el = (Element)ob;
                if(el.getAttributeValue("url").equals(url)){
                    Element e_notes = el.getChild("notes");
                    System.out.println("element notes is "+e_notes);
                    e_notes.setText(notes);
                    System.out.println("text for notes is "+e_notes.getText());
                }
            }
        }
        
        //changed the document now put it back in.
        try{
            Cart.insertDoc(sid,doc);
            
        }  catch(SessionException e){
            logger.warn("Error with the session manager",e);
            throw e;
        }
        catch(CartException e){
            logger.warn("Error with the shopping cart",e);
            throw e;
        }
        catch(Exception e){
            logger.warn("Error with the shopping cart",e);
            throw e;
        }
        
        
        
        return new Boolean(true);
        
    }
    
    
}
