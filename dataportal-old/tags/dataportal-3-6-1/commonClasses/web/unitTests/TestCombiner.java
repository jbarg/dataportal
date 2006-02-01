import ac.dl.xml.*;
import java.io.*;
import org.jdom.*;
import junit.framework.*;
import java.util.*;
import org.jdom.input.*;

public class TestCombiner extends TestCase{
    
    
    public TestCombiner(String theName) {
        super(theName);
    }
    
    public static Test suite() {
        return new TestSuite(TestCombiner.class);
    }
    
    public void testBuild() throws Exception {
        Document[] docs = new Document[2];
        docs[0] = new Document(new Element("combined1"));
        docs[0].getRootElement().addContent(new Element("newelement1"));
        
        docs[1] = new Document(new Element("combined2"));
        docs[1].getRootElement().addContent(new Element("newelement2"));
        
        org.w3c.dom.Document docadded = Combiner.build(docs,"isThisNew");
        
        assertNotNull("What does the new doc look like",docadded);
        assertEquals("How many elements named newelement1, should be 1",1,docadded.getElementsByTagName("newelement2").getLength());
        Saver.save(docadded,"xml"+File.separator+"added.xml");
        
        
        
        
    }
    
    
    
    
    public static void main(String args[]) {
        junit.textui.TestRunner.run(suite());
    }
    
    
    
}