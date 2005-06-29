/* ====================================================================
 * SDSC Matrix Software License, Version 1.0
 *
 * Copyright (c) 2003 San Diego Supercomputer Center (SDSC), San Diego, USA.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that ALL the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by the
 *        SDSC Matrix Project (http://www.npaci.edu/DICE/SRB/matrix/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "SDSC" and "SDSC Matrix" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact arun@sdsc.edu or srb-chat@sdsc.edu.
 *
 * 5. Products derived from this software may not be called "SDSC",
 *    nor may "SDSC" appear in their name, without prior written
 *    permission of the SDSC.
 *
 * 6. License is not granted for commercial resale, in whole or in part, 
 *	  without the prior written permission from SDSC.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL SDSC OR
 * ITS CONTRIBUTORS (SDSC Matrix Team) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software is intended to consist of voluntary contributions made by many
 * individuals for the SDSC Matrix Project.
 *
 * SDSC Matrix.  
 * Designer & Architect: Arun swaran Jagatheesan (arun@sdsc.edu)
 *
 * Authors (Credits 02/SEP/2003):
 * Allen Ding (alding@sdsc.edu)
 * Arun Jagatheesan (arun@sdsc.edu)
 * Reena Mathew	(rmathew@sdsc.edu)
 * Erik Vandekieft (evk@sdsc.edu)
 */



package edu.sdsc.matrix.srb.util;

import java.util.Properties;
import java.io.FileInputStream;
import java.io.File;
import java.io.InputStream;
import java.net.URL;
import java.io.FileWriter;
import java.io.FileOutputStream;

import java.text.SimpleDateFormat;
import java.util.Date;

/**
 * Class provides utilities for Matrix
 * 
 *
 * @author  rmathew
 * @version 1.0
 */
public class MatrixUtil
{
	
	//public static final int filterLevel = 0;
	// Have to make lower filter level to see debug messages
	/**
	 * Retrieved from Property File
	 * This sets the level for the logging of the Matrix Server.
	 * The server will log messages only with a logging level >= filter level
	 */
	public static final int filterLevel = Integer.parseInt(getPropertyFromPropertiesManager("matrix.logLevel"));
	
	/**
	 * Name of the properties file
	 */
	private static final String PROPERTIES_FILE = "matrix.properties";
	
	/**
	 * Captures the properties loaded from the properties file
	 */
	private static Properties matrixProp;
	
	/**
	 * File to which the logs are written to
	 */
	private static File logFile = null;
	//private static FileWriter logFileWriter = null;
	
	/**
	 * Output stream used to write to log file
	 */
	private static FileOutputStream logFileWriter = null;
	
	// LOGGING LEVELS
	/**
	 * Logging level for Debug Logs
	 */
	public final static int DEBUG_LOG 	= 10;
	
	/**
	 * Logging level for Information Logs
	 */
	public final static int INFO_LOG    = 20;
	
	/**
	 * Logging level for Warning Logs
	 */
	public final static int WARNING_LOG = 30;
	
	/**
	 * Logging level for Error Logs
	 */
	public final static int ERROR_LOG 	= 40;
	
	/**
	 * Logging level for Fatal Logs
	 */
	public final static int FATAL_LOG 	= 50;
	
	/**
	 * Returns a unique ticket id in String format
	 * 
	 * @param None
	 * @return the unique ticket id
	 *  
	 */
	public static String generateUniqueTicketId()
	{
		//Let the current time be the ticket id
		String ticketId =(new Long(System.currentTimeMillis())).toString();
		return(ticketId);
	}
	
	/**
	 * Writes log to the log file
	 * 
	 * @param log String for logging	 
	 *  
	 */
	public static void writeToLogFile(String log)
	{
		try
		{
		
			if(logFile==null)
			{
				logFile = new File(getPropertyFromPropertiesManager("matrix.logFile"));
				logFile.createNewFile();
			}
			if(logFileWriter==null)	
			{	
				logFileWriter = new FileOutputStream(logFile,true);
			}	
			logFileWriter.write(("\n"+log).getBytes());
			logFileWriter.flush();			
		}
		catch(Exception e)
		{
			System.out.println("CRITICAL ERROR : Cannot write to log file.\n Error Description :"+e);	
		}
	}
	
	
	/**
	 * Logs message with a transaction id
	 * 
	 * @param txnID Transaction ID for log
	 * @param msg Message to be logged
	 * @param errLevel Logging level for message
	 */
	public static void toLog(String txnID,String msg, int errLevel)
	{
		String newMsg = new String("");
		if(txnID!=null)
			newMsg = "TRANSACTION ID -" +txnID+": ";
		
		if(msg!=null)
			newMsg += msg;
		toLog(newMsg, errLevel);		
		
	}
	
	/**
	 * Logs messages after formatting message
	 * 
	 * @param msg Message logged
	 * @param logLevel Logging Level for Message
	 */
	public static void toLog(String msg, int logLevel)
	{
			if (logLevel >= filterLevel){
				
				if(logLevel == DEBUG_LOG)
					msg = "DEBUG   :" + msg;
				else if(logLevel == INFO_LOG)
					msg = "INFO    :" + msg;
				else if(logLevel == WARNING_LOG)
					msg = "WARNING :" + msg;
				else if(logLevel == ERROR_LOG)
					msg = "ERROR   :" + msg;
				else if(logLevel == FATAL_LOG)
					msg = "FATAL   :" + msg;
				else
					msg = "LOG     :" + msg;	
				 //System.err.println("SAgent"+msg);
				if(getPropertyFromPropertiesManager("writeLogToConsole").equals("true")) 
				 System.out.println(msg);
				 
				if(getPropertyFromPropertiesManager("writeToLogFile").equals("true"))
					writeToLogFile(msg);
			}
	}
	
	// Not working from inside a WAR file in Tomcat or JWSDP
	/**
	 * Retrieves the property from file or memory
	 * 
	 * @param property Name of Property to be retrieved
	 */
	public static String getMatrixProperty(String property)
	{
			if(matrixProp==null)
			{
				matrixProp =  new Properties();
				try
				{	
					//matrixProp.load(new FileInputStream(".\\"+PROPERTIES_FILE));
					//matrixProp.load(ClassLoader.getSystemResourceAsStream("/matrix"));
					String resourcePathRelativeToClasspath = "edu/sdsc/matrix/srb/util/matrix.properties";

					ClassLoader cl = ClassLoader.getSystemClassLoader();
					URL url = cl.getResource(resourcePathRelativeToClasspath);
					if (url == null)
					{
						// Could not find resource on classpath
						toLog("Could not find resource on classpath:url-"+url, 10);
					}
					else
						toLog("resource on classpath:url"+url, 10);

					InputStream rf = url.openStream();
					if (rf == null)
					{
						//   Failed opening stream for this resource
						toLog("Failed opening stream for this resource:rf-"+rf, 10);
					}
					else
						toLog("opening stream for this resource:rf-"+rf, 10);
					matrixProp.load(rf);

 
				}
				catch(Exception e)
				{
					toLog("Cannot load Matrix Properties file:"+e, 10);	
				}
			}
			
			return(matrixProp.getProperty(property));
	}
	
	
	/**
	 * Instance of Properties Manager 
	 * @see PropertiesManager
	 */
	private static PropertiesManager propManager;
	
	/**
	 * Retrieves property from Properties Manager
	 * 
	 * @param propName Property Name
	 */
	public static String getPropertyFromPropertiesManager(String propName)
	{
		String propValue =  null;
		String applicationMode= null;
		try
		{
			
			if(propManager==null)
			{
					propManager=new PropertiesManager("MATRIX_HOME", "conf"+File.separator+"matrix.properties", 1);
					applicationMode = propManager.getPropertyWithDetail("applicationMode");
					
					if(applicationMode!=null)
					{
						if(applicationMode.equals("production"))
							propManager.setDeployMode(0);
						else if(applicationMode.equals("debug"))
							propManager.setDeployMode(1);
					}				
			}		
			propValue = propManager.getPropertyWithDetail(propName);
			
		}
		catch(Exception e)
		{
			System.out.println("WARNING: MatrixUtil class cannot load matrix properties file due to :"+e);		
		}
		finally
		{
			return (propValue);
		}
	}
	
	
	public static void main(String[] args)
	{
		System.out.println(getMatrixProperty("server.url"));
	}
	
	
	/**
	 * Formats a date to a specific format
	 * 
	 * @param dateFormat Format to which date is formatted
	 * @param inputDate Date to be formatted
	 */
	public static String formatDate(String dateFormat, Date inputDate)
	{
		String formattedDate = null;
		if(dateFormat!=null && inputDate!=null)
		{
			SimpleDateFormat dateFormatter = new SimpleDateFormat(dateFormat);
			formattedDate = dateFormatter.format(inputDate);
		}	
		return(formattedDate);
	}
}    