/*
 * AcmWS.java
 *
 * Created on 09 December 2002, 10:58
 */

package uk.ac.clrc.dataportal.acm;

import java.sql.*;

/**
 *
 * @author  asm67
 */
public class AcmWS {

    public String getAccessInXMLString(String userId) throws Exception{
        try {
            DbAccess db = new DbAccess();
            ResultSet rs =db.query("SELECT * from userTable where userName='"+userId+"'");
            return db.buildXML(rs);
        } catch (Exception e) {
            throw e;
        }
    }
    public org.w3c.dom.Element getAccessInW3CElement(String userId) throws Exception{
        org.w3c.dom.Element el = null;
        try {
            DbAccess db = new DbAccess();
            ResultSet rs =db.query("SELECT * from userTable where userName='"+userId+"'");
            el= db.buildW3CElement(rs);
        } catch (Exception e) {
            throw e;
        }
        return el;
    }

}
