package uk.ac.clrc.dataportal.authent;


import java.io.*;
import java.util.*;
import java.sql.*;
//security classes
import java.security.*;
import java.security.interfaces.*;
import java.math.*;
import java.net.*;
import javax.xml.parsers.*;
import org.w3c.dom.*;
import java.lang.*;

//log classes
import org.apache.log4j.*;
//web axis stuff
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;


// We somehow recive a username and password in SOAP format. Need to work this out    
    
    
    public void doPost(HttpServletRequest request, HttpServletResponse response)
    throws ServletException, IOException {
        
        //users password and name from database
        String name = null;
        String pass = null;
        String role = null;
        
 // Because roles are no used, we need to replace this with some sort of facility array
        
        //how are new users added - do I do this?
        //inital request for the users record. If no cart create one.
        RecordUser user = (RecordUser)session.getAttribute("user");
        if(user == null){
            user = new RecordUser();
        }
        
        //get user input values
        String reName = request.getParameter("username");
        String rePass = request.getParameter("password");
        
        
        
        //encrypt password for comparsion
        byte[] enc = null;
        try{
            MessageDigest thedis = MessageDigest.getInstance("MD5");
            thedis.update(rePass.getBytes());
            enc = thedis.digest();
        }
        catch(Exception ex){
            logger.fatal("DataPortalException: Could not encrypt password",ex);
            response.sendRedirect("../Error.jsp");
        }
        //turn given encrypted password in to bytes
        String enName = new String(enc);
        byte[] givenPass = enName.getBytes();
        
        //set login status as false
        boolean loggedin = false;
        
        //comment this out for the cvs allowing access from your desk top
        Connection myConn = null;
        try{
            // Step 1.  Load the driver
            logger.info("Loading driver"  + odbcdriverName);
            Class.forName(odbcdriverName);
            
            // Step 2:  Get a connection
            logger.info("Connecting to the database: "  + odbcdbUrl);
            
            myConn = DriverManager.getConnection(odbcdbUrl, username, password);
            
            // Step 3:  Create and execute the SQL statement
            logger.info("Executing query...");
            Statement myStmt = myConn.createStatement();
            ResultSet myRs = myStmt.executeQuery("SELECT * from dpusers WHERE username = '"+reName+"'");
            
            // Step 4:  Process the Result Set
            while (myRs.next()) {
                
                //retrieves the name, role and password out of the database
                pass = myRs.getString("password");
                name = myRs.getString("username");
                role = myRs.getString("role");
                
                //de-ecrypt password
                byte[] bpass = pass.getBytes();
                
                for(int i = 0; i< bpass.length;i++){
                    Byte givenPass2 = new Byte(givenPass[i]);
                    if(givenPass2.intValue() < 0 || givenPass[i] == bpass[i]){
                        loggedin = true;
                    }
                    else if(bpass[i] != givenPass[i]) {
                        loggedin = false;
                        logger.warn("Incorrect password given for " +reName);
                        break;
                    }
                }
            }
        } catch(Exception e){
            logger.fatal("DataPortalException: could not check password",e);
        } finally{
            try{
                myConn.close();
            }
            catch(Exception ignore){}
        }

                    
            
            
            // Could lookup the name of the ACM at this point
            
            // We now need to go to the lookup module and get the site information
            // this is returned in XML
        // It is returned for each facility, but only BADC exists.
        // In an ideal world, we would look up all facilities in the Lookup Module
        // Note we only send the username
        // As only 1 facility exists at present, quetsion whether this is worth it now.
        // Presumably, we only get names from lookup module and therefore we may only
        // need to build this list once per instatiation of the Authent module.
        // THIS MODULE NEED NOT UNDERSTAND XML DOC FROM ACM
        
        
        // Need to bundle AC info into an array for Session Manager
        // Data is returned separately for each site?
        // I need to bundle this into a single document to send to Session Manager
        // 
            
          // We pass the above to the Session Manager to get a Session ID
            
          // We pass the session id on
 