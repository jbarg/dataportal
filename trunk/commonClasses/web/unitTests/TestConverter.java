import ac.dl.xml.*;
import java.io.*;
import org.jdom.*;
import junit.framework.*;
import java.util.*;
import org.jdom.input.*;

public class TestConverter extends TestCase{    
    
    public TestConverter(String theName) {
        super(theName);
    }
    
    public static Test suite() {
        return new TestSuite(TestConverter.class);
    }
    
    public void testJDOMtoDOM() throws Exception {
        Document[] docs = new Document[2];
        docs[0] = new Document(new Element("combined1"));
        docs[0].getRootElement().addContent(new Element("newelement1"));
        
        //docs[1] = new Document(new Element("combined2"));
        //docs[1].getRootElement().addContent(new Element("newelement2"));
        
        org.w3c.dom.Document docconverted = Converter.JDOMtoDOM(docs[0]);
          Saver.save(docconverted,"xml"+File.separator+"toDOM.xml");
        assertNotNull("What does the new doc look like",docconverted);
        assertEquals("How many elements named newelement1, should be 1",1,docconverted.getElementsByTagName("newelement1").getLength());
        
      
        
        
        
        
    }
     public void testDOMtoJDOM() throws Exception {
        org.w3c.dom.Document docs = XML_DOMBuilder.build(new File("xml"+File.separator+"users.xml"));
        Document jdom = Converter.DOMtoJDOM(docs);
        Saver.save(jdom,"xml"+File.separator+"toJDOM.xml");
        assertNotNull("What does the new doc look like",jdom);
        assertEquals("What is the name of the doc elemetn , should be CLRCMetadata.","CLRCMetadata",jdom.getRootElement().getName());
       
        
        
        
        
        
    }
    
    
    
    public static void main(String args[]) {
        junit.textui.TestRunner.run(suite());
    }
    
    
    
}