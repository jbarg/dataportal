/*
 * Log4jServer.java
 *
 * Created on 06 May 2004, 16:06
 */
package uk.ac.cclrc.log4j;

import java.net.*;
import java.io.*;
import java.util.*;
import javax.mail.*;
import javax.mail.internet.*;
import org.apache.log4j.*;
import org.apache.log4j.spi.*;


/**
 *
 * @author  gjd37
 */
public class Log4jServer {

    Logger log = Logger.getLogger(this.getClass().getName());
    Properties props_conf;
    int port;
    int max_wait;
    String prop_file_name;

    /** Creates a new instance of Log4jServer */
    public void runServer(String[] args) {
        ServerSocket server;
        Socket socket;
        ObjectOutputStream output;
        ObjectInputStream input;

        //put these globubally
        props_conf = new Properties();

        port = new Integer(args[0]).intValue();
        max_wait = new Integer(args[1]).intValue();
        prop_file_name = args[2];

        try{
            server = new ServerSocket(port,max_wait);
            log.info("Started server on port "+port);


            while(true){
                log.info("Awaiting a connection....");
                socket = server.accept();
                log.info("Connection accepted from "+socket.getInetAddress().getHostName());

                //output = new ObjectOutputStream(socket.getOutputStream());

                input = new ObjectInputStream(socket.getInputStream());

                LoggingEvent event;
                //do while loop will never gomout of since while is just set to true
                //only bottom catch can be got to if there is a problem above
                do{
                    try{
                        //get logging event
                        event = (LoggingEvent)input.readObject();
                        log.info("Logging Event incoming.....");

                        //get Exception message as a string[]
                        ThrowableInformation throwable = (ThrowableInformation)event.getThrowableInformation();
                        String[] exception = {"No Exception thrown with this logging event."};
                        exception = (throwable != null) ? throwable.getThrowableStrRep() : exception;

                        //get the level of the log4j event

                        Level _level = event.getLevel();
                        String level = _level.toString();

                        props_conf.load(new FileInputStream(new File(prop_file_name)));
                        String confLevel = props_conf.getProperty("level", "FATAL");
                        boolean email = false;
                        //only fatal and error message can get send to emails.
                        if(confLevel.equalsIgnoreCase("FATAL")) email = (level.equalsIgnoreCase("FATAL"))? true : false;
                        else if(confLevel.equalsIgnoreCase("ERROR")){
                            email = (level.equalsIgnoreCase("ERROR") ||level.equalsIgnoreCase("FATAL") )? true : false;
                        }

                        log.debug("Logging level is "+level+" . Sending email(s): "+email);
                        //get the time of the event
                        long time = event.getStartTime();
                        Date date = new Date(time);

                        //get the unique NDC
                        String ndc  = event.getNDC();

                        //get the message
                        String message = event.getRenderedMessage();

                        if(email) sendEmail(ndc,date,level,exception, message);

                    }
                    catch(NoClassDefFoundError ncdfe){
                        log.fatal("Unable to run server without Class Def. Shutting down",ncdfe);
                        System.exit(0);
                    }
                    catch(ClassNotFoundException cnfe){
                        log.fatal("Unable to run server without Class Def. Shutting down",cnfe);
                        System.exit(0);
                    }
                    catch(SocketException se){
                        log.error(se);
                        //
                        try{
                            log.warn("Waiting for 2 minutes... then will try to reconnect...");
                            input.close();
                            socket.close();
                            Thread.sleep(120000);
                        }
                        catch(Exception ignore){}
                        break;
                    }
                    catch(Exception io){
                        log.error(io);
                    }
                }while(true);
            }
        }
        catch(EOFException eofe){
            log.error("Client shutdown connection");
        }
        catch(IOException io){
            log.error(io);
        }
        catch(Exception io){
            log.error(io);
        }


    }

    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
        Log4jServer server = new Log4jServer();

        server.runServer(args);
    }


    private void sendEmail(String ndc, Date date, String level, String[] exception, String _message ) {
        try{
            //read in configuration file for the email properties
            props_conf.load(new FileInputStream(new File(prop_file_name)));
            String dp_id = props_conf.getProperty("dataportal.id");

            Properties mailProps = new Properties();
            mailProps.put("mail.smtp.host", props_conf.getProperty("mail.host"));

            // Create a session with the Java Mail API
            Session mailSession = Session.getDefaultInstance(mailProps);

            // Create a transport object for sending mail
            Transport transport = mailSession.getTransport("smtp");

            // Create a new mail message
            MimeMessage message = new MimeMessage(mailSession);

            // Set the From and the Recipient
            message.setFrom(new InternetAddress(dp_id+"@dl.ac.uk"));

            String _names = props_conf.getProperty("email.address");
            String[] names = _names.split("\\s+");
            if(names.length != 0){
                log.debug(names[0]);
                message.setRecipient(Message.RecipientType.TO, new InternetAddress(names[0]));
                for(int i = 1; i<names.length;i++){
                    log.debug(names[i]);
                    message.setRecipient(Message.RecipientType.BCC, new InternetAddress(names[i]));
                }
                // Set the subject
                message.setSubject("Data Portal instance "+dp_id+": Logging event "+level);

                // Set the message text
                StringBuffer buff = new StringBuffer();
                buff.append("Exception thrown on the "+dp_id+" Data Portal.\n\n" );
                buff.append("NDC: "+ndc+"\n");
                buff.append("Date: "+date+"\n");
                buff.append("Level: "+level+"\n");
                buff.append("Message: "+_message+"\n");
                buff.append("Exception: "+exception[0]+"\n");
                for(int i = 1;i<exception.length;i++){
                    buff.append(exception[i]+"\n");
                }

                message.setText(buff.toString());

                // Save all the changes you have made to the message
                message.saveChanges();

                // Send the message
                transport.send(message);
                StringBuffer email = new StringBuffer();
                for(int i = 0;i< names.length;i++){
                    if(i==0) email.append(names[i]+", ");
                    else if (i != (names.length -1)) email.append(names[i]+", ");
                    else email.append(names[i]+".");
                }
                log.info("Email sent to "+email.toString());
            }
            else log.warn("No email addresses given");

        }
        catch(Exception e){
            log.error(e);
        }
    }

}

