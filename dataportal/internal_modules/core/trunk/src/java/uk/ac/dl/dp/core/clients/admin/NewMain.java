/*
 * NewMain.java
 *
 * Created on 24 November 2006, 14:09
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.clients.admin;

import java.sql.Timestamp;
import java.util.Date;

/**
 *
 * @author gjd37
 */
public class NewMain {
    
    /** Creates a new instance of NewMain */
    public NewMain() {
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
        System.out.println(new Timestamp(new Date().getTime()));
        System.out.println(new  Date(2000,5,5));
        System.out.println("1901-02-01 00:00:00.0");
    }
    
}
