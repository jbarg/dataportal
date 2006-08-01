package uk.ac.dl.dp5.clients.datacenter;
/*
 * NewMain.java
 *
 * Created on 31 March 2006, 15:45
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

import org.apache.log4j.Logger;
import org.apache.log4j.PropertyConfigurator;
import uk.ac.dl.dp5.clients.dto.BookmarkDTO;
import uk.ac.dl.dp5.clients.dto.DataUrlDTO;
import java.util.*;

import uk.ac.dl.dp5.clients.session.SessionDelegate;
import uk.ac.dl.dp5.util.DPUrlRefType;
/**
 *
 * @author gjd37
 */
public class DataCenterDelegateDTOClient {
    
    static  double time ;
    
    private  static Logger log = Logger.getLogger(DataCenterDelegateClient.class);
    
    /** Creates a new instance of NewMain */
    public DataCenterDelegateDTOClient() {
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        //PropertyConfigurator.configure("c:/log4j.properties");
        time =  new Date().getTime();
        try{
            DataCenterDelegateDTO dd = DataCenterDelegateDTO.getInstance();
            SessionDelegate sd = SessionDelegate.getInstance();
            printTime("initial lookup");
            
            //start session
            String sid  = sd.login("glen","kkkkkk",2);
            log.debug("Started session with sid "+sid);
            printTime("login");
            
            log.debug("Restarting timer");
            time =  new Date().getTime();
            
            DataCenterDelegate.getInstance();
            printTime("lookup again");
            
            //adding a bookmark
            BookmarkDTO dto = new BookmarkDTO();
            
            dto.setStudyid((int)(Math.random()*10000L));
            dto.setFacility("ISIS");
            dto.setName(""+(int)(Math.random()*10000L)+" Random name");
            dto.setNote("Silly ");
            dto.setQuery("Query ");
            
            Collection<BookmarkDTO> notadded =   dd.addBookmark(sid,dto);
            for(BookmarkDTO dto1 : notadded){
                System.out.println("Not added: "+dto1.getStudyid());
            }
            printTime("Adding bookmark");
            
            //addUrl
            DataUrlDTO duto = new DataUrlDTO();
            
            duto.setTypeOfReference(DPUrlRefType.DATA_SET);
            duto.setTypeOfObject("txt");
            duto.setFacility("ISIS");
            duto.setName("Bookmark");
            duto.setNote("Silly");
            duto.setQuery("query");
            Collection<String> cs = new ArrayList<String>();
            cs.add("srb://www.data.com"+(int)(Math.random()*10000L));
            duto.setUrl(cs);
            
            dd.addDataUrl(sid,duto);
            printTime("Adding urls");
            
            //getting bookmarks
            Collection<BookmarkDTO> dto2 =  dd.getBookmarks(sid);
            
            for(BookmarkDTO dtos : dto2){
                
                System.out.print(dtos.getFacility());
                System.out.print(" ,"+dtos.getName());
                System.out.print(" ,"+dtos.getNote());
                System.out.print(" ,"+dtos.getQuery()+"\n");
            }
            printTime("bookmarks");
            
            //getting urls
            Collection<DataUrlDTO> dto1 =  dd.getUrls(sid);
            
            for(DataUrlDTO dtos : dto1){
                System.out.print(dtos.getFacility());
                System.out.print(" ,"+dtos.getName());
                System.out.print(" ,"+dtos.getNote());
                System.out.print(" ,"+dtos.getQuery());
                for(String url : dtos.getUrl()){
                    System.out.print("    "+url+      "\n");
                    
                }
            }
            printTime("urls");
            
            //end session
            sd.logout(sid);
            log.debug("Ended session with sid "+sid);
            printTime("logout");
            
        }catch(Exception e){
            e.printStackTrace();
        }
        
    }
    
    private static void printTime(String message){
        log.debug(message+": "+(new Date().getTime()-time)/1000+" secs\n");
        
    }
    
}
