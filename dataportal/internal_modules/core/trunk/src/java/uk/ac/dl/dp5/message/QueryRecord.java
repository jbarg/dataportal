/*
 * QueryRecord.java
 *
 * Created on 30 June 2006, 10:00
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.message;


import java.io.*;
import java.sql.Timestamp;

/**
 *
 * @author gjd37
 */
public class QueryRecord implements Serializable {
  private Timestamp sent;
  private Timestamp processed;
  private Object[] result;
  private String id;
  private String sid;

  public QueryRecord (String id, String sid, Timestamp sent, Timestamp processed, Object[] result) {
    this.setSent(sent);
    this.setProcessed(processed);
    this.setResult(result);
    this.setId(id);
    this.setSid(sid);
  }

    public Timestamp getSent() {
        return sent;
    }

    public void setSent(Timestamp sent) {
        this.sent = sent;
    }

    public Timestamp getProcessed() {
        return processed;
    }

    public void setProcessed(Timestamp processed) {
        this.processed = processed;
    }

    public Object[] getResult() {
        return result;
    }

    public void setResult(Object[] result) {
        this.result = result;
    }

    public String getId() {
        return id;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getSid() {
        return sid;
    }

    public void setSid(String sid) {
        this.sid = sid;
    }
}