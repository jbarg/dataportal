/*
 * Indexer.java
 *
 * Created on 29 July 2003, 17:24
 */

package uk.ac.dl.web;
import org.apache.lucene.index.IndexWriter;
import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.analysis.*;
import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.analysis.*;
import java.io.*;
import org.apache.lucene.search.*;
import org.apache.lucene.queryParser.QueryParser;
/**
 *
 * @author  gjd37
 */
public class Indexer {
    
    /** Creates a new instance of Indexer */
    public Indexer(String[] args) {
        String option = args[0];
        if(args.length == 0){
            System.out.println("Usage: java Indexer");
            return;
        }
        else if(args[0].equals("create")){
            String[] files = null;
            String dir = "";
            dir = (args.length<2) ? "C:\\Documents and Settings\\gjd37\\My Documents\\theDataPortal\\dataportalcvs\\dataportal\\WebInterface\\build\\help\\xml": args[1];
            System.out.println("Dir for indexiong files is "+dir);
            try{
                files = getFilesInDir(dir);
            }
            catch(Exception r){
                System.out.println(r);
                r.printStackTrace();
                return;
            }
            String index = "";
            index = (args.length<3) ? "C:\\Documents and Settings\\gjd37\\My Documents\\theDataPortal\\dataportalcvs\\dataportal\\WebInterface\\build"+File.separator+"help"+File.separator+"xml"+File.separator+"index":args[2];
            System.out.println("Dir for index "+index);
            addFilesToIndex(null,files , index,true);
            
        }
        
        else if(args[0].equals("search")){
            String index = "";
            if(args.length <3) index = "C:\\Documents and Settings\\gjd37\\My Documents\\theDataPortal\\dataportalcvs\\dataportal\\WebInterface\\build"+File.separator+"help"+File.separator+"xml"+File.separator+"index";
            if(args.length <2){
                System.out.println("Usage : java Indexer search search_string");
                return;
            }
            else search(args[1],index);
            
        }
        else System.out.println("Usage : java Indexer [search|create] options");
        
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        
        new Indexer(args);
    }
    
    
    public static void createNewIndexer(String indexPath, boolean isNew) throws IOException{
        
        IndexWriter writer;
        
        try{
            // An index is created by opening an IndexWriter with the
            // create argument set to true.
            writer = new IndexWriter(indexPath, null, true);
            writer.close();
            writer = null;
        }
        catch(Exception e){
            writer = null;
        }
        
    }
    
    public static void addFilesToIndex(String directoryName,String[] filesPath,String indexPath,boolean isNew){
        
        
        IndexWriter writer = null;
        try{
            writer = new IndexWriter(indexPath, new SimpleAnalyzer(), isNew);
            for (int i=1; i<filesPath.length; i++) {
                
                InputStream is = new FileInputStream(filesPath[i]);
                
                // We create a Document with two Fields, one which contains
                // the file path, and one the file's contents.
                Document doc = new Document();
                int point = filesPath[i].lastIndexOf(".");
                String type = filesPath[i].substring(point+1,filesPath[i].length());
                String name = "";
                //what OS is it
                String os = System.getProperty("os.name");
                if( os.toLowerCase().startsWith("windows")){
                    int index = filesPath[i].lastIndexOf("\\");
                    int lastpoint = filesPath[i].lastIndexOf(".");
                    name = filesPath[i].substring(index+1,point);
                }
                else{
                    //linux/unix
                    int index = filesPath[i].lastIndexOf("/");
                    int lastpoint = filesPath[i].lastIndexOf(".");
                    name = filesPath[i].substring(index+1,point);
                }
                if(type.equals("xml") || type.equals("html")){
                    System.out.println("type is "+type);
                    System.out.println("path is "+name);
                    doc.add(Field.UnIndexed("path", filesPath[i]));
                    doc.add(Field.UnIndexed("type", type));
                    doc.add(Field.UnIndexed("name", name));
                    doc.add(Field.Text("body", (Reader) new InputStreamReader(is)));
                    
                    writer.addDocument(doc);
                    is.close();
                }
                
            }
            writer.close();
            System.out.println("Indexing finished ok");
            
        }
        catch(Exception e){
            System.out.println(e);
            //writer = null;
            
        }
        
        
        
        
        
    }
    
    public void addDirectoryToIndex(String directoryPath){
        
        
    }
    
    public String[] getFilesInDir(String dir) throws IOException{
        //System.out.println("dir is "+dir);
        File xmlDir = new File(dir);
        File[] xmlFiles = xmlDir.listFiles();
        //System.out.println("total is "+xmlFiles.length);
        int i = 0;
        for(int j = 0; j < xmlFiles.length;j++){
            if(xmlFiles[j].isFile()) i++;
        }
        //System.out.println("Ammount of files in xml is "+i);
        String[] xmlString = new String[i];
        int g = 0;
        for(int k = 0;k<xmlFiles.length;k++){
            //  System.out.println("g is "+g +" and i is "+k);
            if(xmlFiles[k].isDirectory()){}
            else{
                xmlString[g] =  xmlFiles[k].getAbsolutePath();
                g++;
                System.out.println("adding "+xmlFiles[k].getAbsolutePath());
            }
        }
        return xmlString;
    }
    
    public void search(String queryString,String indexPath){
        try{
            
            
            // String indexPath = "C:\\Documents and Settings\\gjd37\\My Documents\\theDataPortal\\dataportalcvs\\dataportal\\WebInterface\\build\\help\\xml\\index", queryString = "This is a introduction to the database";
            
            Searcher searcher = new IndexSearcher(indexPath);
            Query query = QueryParser.parse(queryString, "body", new SimpleAnalyzer());
            Hits hits = searcher.search(query);
            
            for (int i=0; i<hits.length(); i++) {
                System.out.println(hits.doc(i).get("path") + "; Score: " +
                hits.score(i)+" type is "+hits.doc(i).get("type"));
            }
        }
        catch(Exception e){
            System.out.println(e);
        }
        
        
    }
    
}
