package uk.ac.dl.dp.core.clients.datacenter;
import java.util.Collection;
import java.util.Date;
import uk.ac.dl.dp.core.sessionbeans.datacenter.DataAuthorisationBean;
import uk.ac.dl.dp.coreutil.entity.DataRefAuthorisation;
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
           // String name = "LouisePrice";
             String name = "glen";
            
            if(sid == null || sid.equals("")){
                SessionRemote sless1 = (SessionRemote) csl.lookup(DataPortalConstants.SESSION);
                 sid = sless1.login(DataPortalConstants.MYPROXY_USERNAME,DataPortalConstants.MYPROXY_PASSWORD,3);
     
                //sid =  sless1.login(name,"llllll",2);
                System.out.println(sid);
            }
            
            DataAuthorisationRemote sless = (DataAuthorisationRemote) csl.lookup(DataPortalConstants.DATA_AUTHORISATION);
            //sless.addAuthorisedUser(sid,"/C=UK/O=eScience/OU=CLRC/L=DL/CN=glen drinkwater",new Date(),new Date(System.currentTimeMillis()+100000000),DPAuthType.ALL);
            
            Collection<DataRefAuthorisation> e = sless.getGivenAuthorisedList(sid, DPAuthType.NONE);
            System.out.println(name +" has given access to:");
            for(DataRefAuthorisation g: e){
                System.out.println(g.getUser().getDn());
            }
            
            System.out.println(name +" has received access from");
            Collection<DataRefAuthorisation> e1 = sless.getRecievedAuthorisedList(sid, DPAuthType.NONE);
            for(DataRefAuthorisation g: e1){
                System.out.println(g.getSource_user().getDn());
            }
            
            if(!e1.isEmpty()){
                Collection<DataReference> e2 = sless.getOtherUsersDataReferences(sid,e1.iterator().next().getSource_user().getDn() );
                System.out.println("others");
                for(DataReference g: e2){
                    System.out.println(g);
                }
            }
            System.out.println("");
            Collection<String> res = sless.searchUserDns(sid, "sufi");
            for(String re : res){
               System.out.println(re);
            }
            
            sless.removeAuthorisedUser(sid , res.iterator().next());
            
            
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
