package uk.ac.dl.web;
import java.io.*;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;
import org.apache.lucene.index.IndexWriter;
import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.analysis.*;
import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.analysis.*;
import java.io.*;
import org.apache.lucene.search.*;
import org.apache.lucene.queryParser.QueryParser;
//log classes
import org.apache.log4j.*;



/** This is the help search servlet.
 * Its INIT reindexes the folder
 *  Its also searches the indes and sends back the results
 * @author Glen Drinkwater
 * @date 04/07/03
 */
public class HelpSearch1 extends HttpServlet{
    
    //set static log for the class
    static Logger logger = Logger.getLogger(HelpSearch1.class);
    String workingDir = null;
    private ServletConfig scon = null ;
    
    public void init(ServletConfig config) throws ServletException {
        //add section here to allow no reindexing at start up
     /*  System.out.println("reloading index's");
        
        scon = config ;
        ServletContext sc = scon.getServletContext();
        workingDir = sc.getRealPath("");
        
        
        try{
            //try to delete lock in index
           /* File indexLock = new File(workingDir+File.separator+"help"+File.separator+"xml"+File.separator+"xml"+File.separator+"index"+File.separator+"write.lock");
            if(indexLock.exists()){
                if(!indexLock.delete()) System.out.println("Cannot delete index lock");
            }
                indexLock =null;
            //delete all the index's
            File dir = new File(workingDir+File.separator+"help"+File.separator+"xml"+File.separator+"index");
            File[] files = dir.listFiles();
            for(int i = 0 ; i<files.length;i++){
                files[i].delete();
                files[i]= null;
            }*/
            
            //create new index
            // Indexer.createNewIndexer(workingDir+File.separator+"help"+File.separator+"xml"+File.separator+"index",true);
            
            //return index
       /*    File xmlDir = new File(workingDir+File.separator+"help"+File.separator+"xml");
            File[] xmlFiles = xmlDir.listFiles();
            System.out.println("total is "+xmlFiles.length);
            int i = 0;
            for(int j = 0; j < xmlFiles.length;j++){
                if(xmlFiles[j].isFile()) i++;
            }
            System.out.println("Ammount of files in xml is "+i);
            String[] xmlString = new String[i];
            int g = 0;
            for(int k = 0;k<xmlFiles.length;k++){
                System.out.println("g is "+g +" and i is "+k);
                if(xmlFiles[k].isDirectory()){}
                else{
                    xmlString[g] =  xmlFiles[k].getAbsolutePath();
                    g++;
                    //System.out.println("adding "+files[i].getAbsolutePath());
                }
            }
            
            //now reindex
            Indexer.addFilesToIndex(null,xmlString,workingDir+File.separator+"help"+File.separator+"xml"+File.separator+"index",true);
            //xmlString = null;
            //xmlFiles = null;
        }
        catch(Exception e){
            System.out.println(e);
        }*/
        
        
    }
    
    
    public void doPost(HttpServletRequest request, HttpServletResponse response)
    throws IOException, ServletException {
        
        HttpSession session = request.getSession(false);
        if(session == null){
            response.sendRedirect("../Login.html");
            return;
        }
        String wd = (String )session.getAttribute("wd");
        //locate the prop file.  Normal get this from web.xml file
        
        //added, take out later
        session = request.getSession();
        wd = "C:\\Documents and Settings\\gjd37\\My Documents\\theDataPortal\\dataportalcvs\\dataportal\\WebInterface\\build";
        System.out.println("wd is "+wd);
        
        //end of added section
        
        PropertyConfigurator.configure(wd+File.separator+"WEB-INF"+File.separator+"logger.properties");
        
        if(session == null){
            response.sendRedirect("../Login.html");
        }
        else{
            try{
                //get the search string
                String queryString = request.getParameter("search_string");
                String indexPath = wd+File.separator+"help"+File.separator+"xml"+File.separator+"index";
                
                Searcher searcher = new IndexSearcher(indexPath);
                Query query = QueryParser.parse(queryString, "body", new SimpleAnalyzer());
                Hits hits = searcher.search(query);
               // Hits clone = (Hits)hits.clone();
                session.setAttribute("hits", hits);
                session.setAttribute("text", queryString);
                session.setAttribute("searcher", searcher);
                //searcher.close();
                //query = null;
                response.sendRedirect("../help/menu.jsp");  
            }
            catch(Exception e){
                System.out.println(e);
            }
        }
    }
}