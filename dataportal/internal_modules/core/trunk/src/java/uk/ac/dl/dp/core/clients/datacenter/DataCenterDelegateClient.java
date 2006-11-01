package uk.ac.dl.dp.core.clients.datacenter;
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
import uk.ac.dl.dp.coreutil.delegates.DataCenterDelegate;


import uk.ac.dl.dp.coreutil.delegates.SessionDelegate;
import uk.ac.dl.dp.coreutil.entity.Bookmark;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.entity.Url;
import uk.ac.dl.dp.coreutil.util.DPUrlRefType;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
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
            String sid  = sd.login(DataPortalConstants.MYPROXY_USERNAME,DataPortalConstants.MYPROXY_PASSWORD,3);
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
            dto.setNote("note bookmark ");
            dto.setQuery("sql select ");
            
            dd.addBookmark(sid,dto);
            
            printTime("Adding bookmark");
            
            //addUrl
            DataReference duto = new DataReference();
            
            duto.setTypeOfReference(DPUrlRefType.DATA_SET.toString());
            duto.setTypeOfObject("txt");
            duto.setFacility("ISIS");
            duto.setName("DataReference");
            duto.setNote("Silly");
            duto.setQuery("query");
            duto.setReferenceId(4);
            duto.setInvestigationId(new Integer("2"));
            Collection<Url> cs = new ArrayList<Url>();
            Url re = new Url("srb://www.data.com"+(int)(Math.random()*10000L));
            re.setName("Name");
            re.setDataRefId(duto);
            cs.add(re);
            
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
