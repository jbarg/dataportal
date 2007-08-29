/*
 * EventLocal.java
 *
 * Created on 26 June 2007, 10:57
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.util;

import java.util.Collection;
import oracle.toplink.essentials.exceptions.QueryException;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.interfaces.SendMDBLocal;
import uk.ac.dl.dp.coreutil.util.DPEvent;
import uk.ac.dl.dp.coreutil.util.EventMessage;
import uk.ac.dl.dp.coreutil.util.KeywordMessage;
import uk.ac.dl.dp.coreutil.util.LoginICATMessage;
import uk.ac.dl.dp.coreutil.util.QueryRequest;

/**
 *
 * @author gjd37
 */
public class sendMDBLocalTest implements SendMDBLocal{
    
    public void sendEvent(String sid, EventMessage eventMessage){
        System.out.println("sendEvent");
    }
    public void sendEvent(String sid, DPEvent event, String description){
        System.out.println("sendEvent");
    }
    public void sendKeywordEvent(String sid, Collection<String> facilities, Collection<String>  keywords){
        System.out.println("sendKeywordEvent");
    }
    public void sendKeywordEvent(String sid, Collection<String> facilities, Collection<String>  keywords, DPEvent event){
        System.out.println("sendKeywordEvent");
    }
    public void sendDownloadEvent(String sid, String message, Collection<String> srburls){
        System.out.println("sendDownloadEvent");
    }
    public QueryRequest queryICATs(String sid, QueryRequest q_request) throws QueryException{
        System.out.println("queryICATs");
        return new QueryRequest();
    }
    public void loginICATs(String sid, LoginICATMessage loginICATMessage) throws SessionException {
        System.out.println("loginICATs");
    }
    public void downloadKeywords(String sid, KeywordMessage keywordMessage) {
        System.out.println("downloadKeywords");
    }
    public void downloadKeywords(String sid, KeywordMessage keywordMessage, String facility) {
        System.out.println("downloadKeywords");
    }
}
