/*
 * TopicManager.java
 *
 * Created on 24 September 2003, 15:59
 */
package uk.ac.dl.topicmanager;

// Database classes
import java.sql.ResultSet;
import java.sql.SQLException;
import uk.ac.dl.db.DBConnection;

// JDOM
import org.jdom.Document;
import org.jdom.Element;
import org.jdom.Text;
import org.jdom.JDOMException;

/**
 *
 * @author  ljb53
 */
public class TopicManager {
    
    private Document doc = null;
    
    /** Creates a new instance of buildTopics */
    public Document BuildTopics() throws SQLException {
        
        // Create root element <active value="start"> </active>
        Element root = new Element("active");
        root.setAttribute("value", "start");
        Element lastElem = root;
        int level = 0;
        
        DBConnection db = new DBConnection();
        
        // Get hierarchy of topics from the database
        db.connect();
        ResultSet rs = db.getData("select level, topic, leaf, "+
        "SYS_CONNECT_BY_PATH(topic, '/') as \"final\" "+
        "from topic "+
        "start with topic='Chemistry' "+
        "connect by prior topic_id = parent ");
        
        int newLevel;
        while (rs.next()) { // get next row
            
            newLevel = rs.getInt("LEVEL");
            
            String leaf = rs.getString("LEAF");
            String finalStr = rs.getString("FINAL");
            int lastSlash = finalStr.lastIndexOf("/");
            String finalLessTopic = finalStr.substring(0,lastSlash);
            
            Element newElem = new Element("level"+newLevel);
            newElem.setAttribute("canSubmit", "false");
            newElem.setAttribute("value", rs.getString("TOPIC"));
            newElem.setAttribute("final", finalLessTopic);
            
            // Add extra level to new element if a leaf (req'd for dataportal)
            // <levelx canSubmit="true" value="submit" final=full path />
            if (leaf.equals("true")) {
                int submitLevel = newLevel+1;
                Element submitElem = new Element("level"+submitLevel);
                submitElem.setAttribute("canSubmit", "true");
                submitElem.setAttribute("value", "submit");
                submitElem.setAttribute("final", finalStr);
                newElem.addContent(submitElem);
            }
            
            if (newLevel > level) { // going down tree
                
                // Add new element as content of last element
                lastElem.addContent(newElem);
                
                lastElem = newElem;
                level = newLevel;
                
            }
            else { // going up tree
                
                
                // Traverse up tree x times (last level - new level + 1) then
                // add element as content
                
                for (int i = 0; i < (1 + (level - newLevel)); i++) {
                    
                    lastElem = lastElem.getParent();
                }
                
                // now we are at correct level, add the new element
                lastElem.addContent(newElem);
                lastElem = newElem;
                level = newLevel;
                
            }
        }
        
        doc = new Document(root);
        return doc;
    }
    
    // FOR TESTING ONLY
    public static void main(String[] args) {
        
        TopicManager tm = new TopicManager();
        try {
            
            Document mydoc = tm.BuildTopics();
            org.jdom.output.XMLOutputter serializer = new org.jdom.output.XMLOutputter();
            serializer.setIndent("  "); // use two space indent
            serializer.setNewlines(true);
            serializer.output(mydoc, System.out);
        }
        catch (Exception e) {
            e.printStackTrace();
        }
        
        
    }
    
}