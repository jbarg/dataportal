import ac.dl.xml.*;
import java.io.*;
import org.jdom.*;
import junit.framework.*;
import java.util.*;
import org.jdom.input.*;

public class TestSaver extends TestCase{
    
    
    public TestSaver(String theName) {
        super(theName);
    }
    
    public static Test suite() {
        return new TestSuite(TestSaver.class);
    }
    
    public void testSave() throws Exception {
      
            Document doc = new Document(new Element("Thisisanewdocument"));
            Saver.save(doc,"xml"+File.separator+"saveddoc.xml");
            Document docloaded = new SAXBuilder().build(new File("xml"+File.separator+"saveddoc.xml"));
            String name = docloaded.getRootElement().getName();
            assertEquals("Is the element name Thisisanewdocument ","Thisisanewdocument",name);
        
    
    
    }
    
            
    
    
    public static void main(String args[]) {
        junit.textui.TestRunner.run(suite());
    }
    
    
    
}