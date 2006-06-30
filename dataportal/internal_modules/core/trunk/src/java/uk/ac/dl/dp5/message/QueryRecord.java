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
  public Timestamp sent;
  public Timestamp processed;
  public Object[] result;
  public String id;

  public QueryRecord (String id, Timestamp sent, Timestamp processed, Object[] result) {
    this.sent = sent;
    this.processed = processed;
    this.result = result;
    this.id = id;
  }
}