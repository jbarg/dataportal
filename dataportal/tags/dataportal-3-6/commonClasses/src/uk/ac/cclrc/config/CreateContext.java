/*
 * CreateContext.java
 *
 * Created on 04 October 2004, 11:10
 */

package uk.ac.cclrc.config;

import org.jdom.*;
import uk.ac.dl.xml.*;

/**
 *
 * @author  gjd37
 */
public class CreateContext {
    
    /** Creates a new instance of CreateContext */
    public CreateContext(String args[]) {
        try{
            Document doc = new Document(new Element("Context"));
            Element el = doc.getRootElement();
            Attribute att = new Attribute("path", "/"+args[0]);
            Attribute att1 = new Attribute("docBase", args[1] );
            el.setAttribute(att);
            el.setAttribute(att1);
            
            Saver.save(doc,args[2]);
           //System.out.println("Saved file to "+args[2]);
        }
        catch(Exception r){
            System.out.println(r);
        }
        
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
        new CreateContext(args);
    }
    
}
