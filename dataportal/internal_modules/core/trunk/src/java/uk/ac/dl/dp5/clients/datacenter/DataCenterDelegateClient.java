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
import java.util.*;

import uk.ac.dl.dp5.clients.session.SessionDelegate;
import uk.ac.dl.dp5.entity.Bookmark;
import uk.ac.dl.dp5.entity.DataReference;
import uk.ac.dl.dp5.entity.Url;
import uk.ac.dl.dp5.util.DPUrlRefType;
/**
 *
 * @author gjd37
 */
public class DataCenterDelegateClient {
    
    static  double time ;
    
    private  static Logger log = Logger.getLogger(DataCenterDelegateClient.class);
    
    /** Creates a new instance of NewMain */
    public DataCenterDelegateClient() {
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        //PropertyConfigurator.configure("c:/log4j.properties");
        time =  new Date().getTime();
        try{
            DataCenterDelegate dd = DataCenterDelegate.getInstance();
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
            Bookmark dto = new Bookmark();
            
            dto.setStudyId((int)(Math.random()*10000L));
            dto.setFacility("ISIS");
            dto.setName(""+(int)(Math.random()*10000L)+" Random name");
            dto.setNote("bookmark ");
            dto.setQuery("sql select ");
            
            dd.addBookmark(sid,dto);
            
            printTime("Adding bookmark");
            
            //addUrl
            DataReference duto = new DataReference();
            
            duto.setTypeOfReference(DPUrlRefType.DATA_SET.toString());
            duto.setTypeOfObject("txt");
            duto.setFacility("ISIS");
            duto.setName("Bookmark");
            duto.setNote("Silly");
            duto.setQuery("query");
            Collection<Url> cs = new ArrayList<Url>();
            cs.add(new Url("srb://www.data.com"+(int)(Math.random()*10000L)));
            duto.setUrls(cs);
            
            dd.addDataReference(sid,duto);
            printTime("Adding urls");
            
            //getting bookmarks
            Collection<Bookmark> dto2 =  dd.getBookmarks(sid);
            
            for(Bookmark dtos : dto2){
                
                System.out.print(dtos.getFacility());
                System.out.print(" ,"+dtos.getName());
                System.out.print(" ,"+dtos.getNote());
                System.out.print(" ,"+dtos.getQuery()+"\n");
            }
            printTime("bookmarks");
            
            //getting urls
            Collection<DataReference> dto1 =  dd.getDataReferences(sid);
            
            for(DataReference dtos : dto1){
                System.out.print(dtos.getFacility());
                System.out.print(" ,"+dtos.getName());
                System.out.print(" ,"+dtos.getNote());
                System.out.print(" ,"+dtos.getQuery());
                for(Url url : dtos.getUrls()){
                    System.out.print("    "+url.getUrl()+      "\n");
                    
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
