import ac.dl.xml.*;
import java.io.*;
import org.jdom.*;
import org.jdom.output.*;
import junit.framework.*;
import java.util.*;

public class TestJDOMBuilder extends TestCase{


    public TestJDOMBuilder(String theName) {
        super(theName);
    }

    public static Test suite() {
        return new TestSuite(TestJDOMBuilder.class);
    }

   /* public void atestSave() {
        try{
            Document doc = XML_JDOMBuilder.build(new File("users2.xml"));
            Saver.save(doc,"isThisHere.xml");
            org.w3c.dom.Document doc1 = XML_DOMBuilder.build(new File("users2.xml"));
            //Saver s = new Saver();

            Saver.save(doc1,"isThisHereDOM.xml");
            assertTrue("File called isThisHere.xml",new File("isThisHere.xml").isFile());
            assertTrue("File called isThisHere.xml using dom",new File("isThisHereDOM.xml").isFile());
        }
        catch(Exception e){
            System.out.println(e);
        }


    }*/

    public void testJDOMBuilder() throws Exception{

            Document doc1 = XML_JDOMBuilder.build(new FileInputStream("xml"+File.separator+"users2.xml"));
            Document doc2 = XML_JDOMBuilder.build(new File("xml"+File.separator+"users2.xml"));
             XMLOutputter out  = new XMLOutputter();
	     String xml = out.outputString(doc2);
	     Document doc3 = XML_JDOMBuilder.build(xml);

            //Saver.save(doc1,"xml"+File.separator+"isThisHereDOM.xml");
            String element1 = doc1.getRootElement().getName();
            String element2 = doc2.getRootElement().getName();
            String element3 = doc3.getRootElement().getName();
            assertEquals("Is the element name CLRCMetaData from file","CLRCMetadata",element2);
            assertEquals("Is the element name CLRCMetaData inputstream","CLRCMetadata",element1);
   			assertEquals("Is the element name CLRCMetaData string","CLRCMetadata",element3);



    }

    public static void main(String args[]) {
        junit.textui.TestRunner.run(suite());
    }



}