/*
 * DataCenterBeanTest.java
 * JUnit based test
 *
 * Created on 03 August 2006, 17:11
 */

package uk.ac.dl.dp5.query;

import java.util.ArrayList;
import java.util.Date;
import junit.framework.*;
import java.util.Collection;
import uk.ac.cclrc.dpal.beans.Investigation;
import uk.ac.cclrc.dpal.enums.LogicalOperator;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.delegates.SessionDelegate;
import uk.ac.dl.dp.coreutil.entity.Bookmark;
import uk.ac.dl.dp.coreutil.util.QueryRequest;
import uk.ac.dl.dp5.util.TestConstants;

/**
 *
 * @author gjd37
 */
public class QueryTest extends TestCase {
    
    private String sid;
    private static SessionDelegate sd = SessionDelegate.getInstance();
    private static QueryDelegate qd = QueryDelegate.getInstance();
    
    
    private long time = new Date().getTime();
    
    
    public QueryTest(String testName) {
        super(testName);
    }
    
    protected void setUp() throws Exception {
        //login
        sid = sd.login(TestConstants.MYPROXY_USERNAME,TestConstants.MYPROXY_PASSWORD,3);
        System.out.println("logging in");
    }
    
    protected void tearDown() throws Exception {
        //logout
        System.out.println("logging out");
        sd.logout(sid);
    }
    
    /**
     * Test of addBookmark method, of class uk.ac.dl.dp5.sessionbeans.datacenter.DataCenterBean.
     */
    public void testQuery() throws Exception {
        System.out.println("Query");
        
        Collection<String> facs = new ArrayList<String>();
        facs.add("ISIS");
        
        String[] keywords = {"raw"};
        
        qd = QueryDelegate.getInstance();
        QueryRequest queryRequest = null;
        
        try {
            //start download
            queryRequest = qd.queryByKeyword(sid,keywords,facs, LogicalOperator.AND);
            System.out.println("quering ...");
        } catch (Exception ex) {
            System.out.println("Download error "+ex);
            return ;
        }
        
        while(!qd.isFinished(queryRequest)){
            System.out.println("not finished yet!");
            Collection<String> com = qd.getCompleted(queryRequest);
            if(com != null){
                for(String g : com){
                    //System.out.println("Got results from "+g);
                }
            }
            try {
                Thread.sleep(2000);
            } catch (InterruptedException ex) {
                ex.printStackTrace();
            }
        }
        
        Collection<Investigation> qr =  qd.getQueryResults(queryRequest);
        System.out.println("Finished with: "+qr.size());
        
    }
    
    /**
     * Test of addBookmark method, of class uk.ac.dl.dp5.sessionbeans.datacenter.DataCenterBean.
     */
    public void testSendOffQuery() throws Exception {
        System.out.println("Query");
        
        Collection<String> facs = new ArrayList<String>();
        facs.add("ISIS");
        
        String[] keywords = {"raw"};
        
        qd = QueryDelegate.getInstance();
        QueryRequest queryRequest = null;
        
        try {
            //start download
            queryRequest = qd.queryByKeyword(sid,keywords,facs, LogicalOperator.AND);
            System.out.println("quering ...");
        } catch (Exception ex) {
            System.out.println("Download error "+ex);
            return ;
        }
        
        System.out.println("Finished");
        
    }
    
 /*   public void testQueryStress() throws Exception {
        System.out.println("testQueryStress");
  
        int MAX = 50;
        for(int i =0 ; i < MAX ; i++){
            testQuery();
        }
        tearDown();
        System.out.println("Time taken was : "+(new Date().getTime() - time)/1000+" seconds");
    }*/
    
    
    public void testQueryStressTest() throws Exception {
        System.out.println("testQueryStressTest");
        
        int MAX = 1500;
        for(int i =0 ; i < MAX ; i++){
            testSendOffQuery();
        }
        //tearDown();
        System.out.println("Time taken was : "+(new Date().getTime() - time)/1000+" seconds");
    }
    
    
    
    
    
    
    
    
}
