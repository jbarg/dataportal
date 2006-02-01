import ac.dl.xml.*;
import java.io.*;
import org.jdom.*;
import junit.framework.*;
import java.util.*;
import org.jdom.output.*;

public class TestDOMBuilder extends TestCase{


    public TestDOMBuilder(String theName) {
        super(theName);
    }

    public static Test suite() {
        return new TestSuite(TestDOMBuilder.class);
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

    public void testDOMBuilderInput() throws Exception{

            org.w3c.dom.Document doc1 = XML_DOMBuilder.build(new FileInputStream("xml"+File.separator+"users2.xml"));
             //Saver.save(doc1,"xml"+File.separator+"isThisHereDOM.xml");
            String element1 = doc1.getDocumentElement().getTagName();
              assertEquals("Is the element name CLRCMetaData from file","CLRCMetadata",element1);

    }
    public void testDOMBuilderFile() throws Exception{

              org.w3c.dom.Document doc2 = XML_DOMBuilder.build(new File("xml"+File.separator+"users2.xml"));
            //Saver.save(doc1,"xml"+File.separator+"isThisHereDOM.xml");
              String element2 = doc2.getDocumentElement().getTagName();
            assertEquals("Is the element name CLRCMetaData from file","CLRCMetadata",element2);

    }

     public void testDOMBuilderString() throws Exception{

	     Document doc = new Document(new Element("Thisisanewdocument"));
	     XMLOutputter out  = new XMLOutputter();
	     String xml = out.outputString(doc);

	              org.w3c.dom.Document doc2 = XML_DOMBuilder.build(xml);
	            //Saver.save(doc1,"xml"+File.separator+"isThisHereDOM.xml");
	              String element2 = doc2.getDocumentElement().getTagName();
	            assertEquals("Is the element name Thisisanewdocument from string","Thisisanewdocument",element2);

    }

    public static void main(String args[]) {
        junit.textui.TestRunner.run(suite());
    }



}