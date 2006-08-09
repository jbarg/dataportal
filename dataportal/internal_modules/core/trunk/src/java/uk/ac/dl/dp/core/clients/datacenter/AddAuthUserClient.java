package uk.ac.dl.dp.core.clients.datacenter;
import java.util.Collection;
import java.util.Date;
import uk.ac.dl.dp.coreutil.entity.DataReference;

import uk.ac.dl.dp.coreutil.interfaces.DataAuthorisationRemote;
import uk.ac.dl.dp.coreutil.interfaces.SessionRemote;
import uk.ac.dl.dp.coreutil.util.CachingServiceLocator;
import uk.ac.dl.dp.coreutil.util.DPAuthType;
import uk.ac.dl.dp.coreutil.util.DataPortalConstants;
/*
 * BookmarkClient.java
 *
 * Created on 27 June 2006, 12:20
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

/**
 *
 * @author gjd37
 */
public class AddAuthUserClient {
    String sid = "";
    /** Creates a new instance of BookmarkClient */
    public AddAuthUserClient() {
        try{
            
            CachingServiceLocator csl = CachingServiceLocator.getInstance();
            String name = "LouisePrice";
            // String name = "glen";
            
            if(sid == null || sid.equals("")){
                SessionRemote sless1 = (SessionRemote) csl.lookup(DataPortalConstants.SESSION);
                // sid = sless1.login(TestConstants.MYPROXY_USERNAME,TestConstants.MYPROXY_PASSWORD,3);
     
                sid =  sless1.login(name,"llllll",2);
                System.out.println(sid);
            }
            
            DataAuthorisationRemote sless = (DataAuthorisationRemote) csl.lookup(DataPortalConstants.DATA_AUTHORISATOIN);
            //sless.addAuthorisedUser(sid,"/C=UK/O=eScience/OU=CLRC/L=DL/CN=glen drinkwater",new Date(),new Date(System.currentTimeMillis()+100000000),DPAuthType.ALL);
            
            Collection<String> e = sless.getGivenAuthorisedList(sid, DPAuthType.ALL);
            System.out.println(name +" has given access to:");
            for(String g: e){
                System.out.println(g);
            }
            
            System.out.println(name +" has received access from");
            Collection<String> e1 = sless.getRecievedAuthorisedList(sid, DPAuthType.ALL);
            for(String g: e1){
                System.out.println(g);
            }
            
            if(!e1.isEmpty()){
                Collection<DataReference> e2 = sless.getOtherUsersDataReferences(sid,e1.iterator().next() );
                System.out.println("others");
                for(DataReference g: e2){
                    System.out.println(g.getName());
                }
            }
            
            //
        }catch(Exception e){
            System.out.println(e.getMessage());
            if(e.getCause() instanceof java.sql.SQLException){
                System.out.println("sql");
            } else e.printStackTrace();
        }
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
        new AddAuthUserClient();
    }
    
}