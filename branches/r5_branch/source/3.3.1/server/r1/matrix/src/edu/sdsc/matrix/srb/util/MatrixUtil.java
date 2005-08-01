/*
	* 	SDSC Matrix. A Gridflow Management System for data grids and digtal libraries
	*  Copyright (C) 2004 SDSC Matrix Project
	*
	* This library is free software; you can redistribute it and/or
	* modify it under the terms of the GNU Lesser General Public
	* License as published by the Free Software Foundation; either
	* version 2.1 of the License, or (at your option) any later version.
	*
	* This library is distributed in the hope that it will be useful,
	*	but WITHOUT ANY WARRANTY; without even the implied warranty of
	*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	*	Lesser General Public License for more details.
	*
	*	You should have received a copy of the GNU Lesser General Public
	*	License along with this library; if not, write to the Free Software
	*	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
	*
	* ====================================================================
	*
	* This software consists of voluntary contributions from the developers of the SDSC
	* Matrix Project. We request that all redistribution and software made using this code
	* acknowledge their use of the “SDSC Matrix project” in their end-user documentation or
	* in their website.
	*
	* SDSC Matrix Project (list of developers as of Dec 2005)
	*	Designer & Architect: Arun swaran Jagatheesan (arun@sdsc.edu)
	* 9500 Gilman Drive, MC0505
	* San Diego Supercomputer Center
	* University of California
	* La Jolla CA 92093
	*
	* Allen Ding (alding@sdsc.edu)
	* Lucas Gilbert (iktome@sdsc.edu)
	* Arun Jagatheesan (arun@sdsc.edu)
	* Reena Mathew	(rmathew@sdsc.edu)
	* Daniel Moore (mixx@umail.ucsb.edu)
	* Erik Vandekieft (evk@sdsc.edu)
	* Jonathan Weinberg (jonw@sdsc.edu)
	*
	*/

package edu.sdsc.matrix.srb.util;

import java.io.File;
import java.io.FileOutputStream;
import java.lang.reflect.InvocationTargetException;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Properties;

import javax.xml.bind.JAXBException;

import edu.sdsc.matrix.srb.parser.Collection;
import edu.sdsc.matrix.srb.parser.CollectionList;
import edu.sdsc.matrix.srb.parser.DataSet;
import edu.sdsc.matrix.srb.parser.ObjectFactory;
import edu.sdsc.matrix.srb.parser.StreamData;
import edu.sdsc.matrix.srb.parser.StringSet;
import edu.sdsc.matrix.srb.parser.TPart;

/**
	* Class provides utilities for Matrix
	*
	*
	* @author  rmathew
	* @version 1.0
	*/
public class MatrixUtil {

	private static ObjectFactory objFactory = new ObjectFactory();

	//public static final int filterLevel = 0;
	// Have to make lower filter level to see debug messages
	/**
		* Retrieved from Property File
		* This sets the level for the logging of the Matrix Server.
		* The server will log messages only with a logging level >= filter level
		*/
	public static final int filterLevel = Integer.parseInt(
			getPropertyFromPropertiesManager("matrix.logLevel"));
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
	public final static int DEBUG_LOG = 10;
	/**
		* Logging level for Information Logs
		*/
	public final static int INFO_LOG = 20;
	/**
		* Logging level for Warning Logs
		*/
	public final static int WARNING_LOG = 30;
	/**
		* Logging level for Error Logs
		*/
	public final static int ERROR_LOG = 40;
	/**
		* Logging level for Fatal Logs
		*/
	public final static int FATAL_LOG = 50;
	/**
		* Returns a unique ticket id in String format
		*
		* @param None
		* @return the unique ticket id
		*
		*/
	public static String generateUniqueTicketId() {
		//Let the current time be the ticket id
		String ticketId = (new Long(System.currentTimeMillis())).toString();
		return (ticketId);
	}

	/**
		* Writes log to the log file
		*
		* @param log String for logging
		*
		*/
	public static void writeToLogFile(String log) {
		try {
			if (logFile == null) {
				logFile = new File(getPropertyFromPropertiesManager(
						"matrix.logFile"));
				logFile.createNewFile();
			}
			if (logFileWriter == null) {
				logFileWriter = new FileOutputStream(logFile, true);
			}
			logFileWriter.write( ("\n" + log).getBytes());
			logFileWriter.flush();
		} catch (Exception e) {
			System.out.println(
					"CRITICAL ERROR : Cannot write to log file.\n Error Description :" +
					e);
		}
	}

	/**
		* Logs message with a flow id
		*
		* @param flowId flow ID for log
		* @param msg Message to be logged
		* @param errLevel Logging level for message
		*/
	public static void toLog(String flowID, String msg, int errLevel) {
		String newMsg = new String("");
		if (flowID != null) {
			newMsg = "FLOW_ID -" + flowID + ": ";
		}
		if (msg != null) {
			newMsg += msg;
		}
		toLog(newMsg, errLevel);
	}

/**
	* get the current loging level
	* @return (MatrixUtil.DEBUG, MatrixUtil.INFO, ..)
*/
public static int getLoggingLevel() {
		return filterLevel;
}

	/**
		* Logs messages after formatting message
		*
		* @param msg Message logged
		* @param logLevel Logging Level for Message
		*/
	public static void toLog(String msg, int logLevel) {
		if (logLevel >= filterLevel) {
			if (logLevel == DEBUG_LOG) {
				msg = "DEBUG   :" + msg;
			} else if (logLevel == INFO_LOG) {
				msg = "INFO    :" + msg;
			} else if (logLevel == WARNING_LOG) {
				msg = "WARNING :" + msg;
			} else if (logLevel == ERROR_LOG) {
				msg = "ERROR   :" + msg;
			} else if (logLevel == FATAL_LOG) {
				msg = "FATAL   :" + msg;
			} else {
				msg = "LOG     :" + msg;
				//System.err.println("SAgent"+msg);
			}
			if (getPropertyFromPropertiesManager("writeLogToConsole").equals(
					"true")) {
				System.out.println(msg);
			}
			if (getPropertyFromPropertiesManager("writeToLogFile").equals(
					"true")) {
				writeToLogFile(msg);
			}
		}
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
	public static String getPropertyFromPropertiesManager(String propName) {
		String propValue = null;
		String applicationMode = null;
		try {
			if (propManager == null) {
				propManager = new PropertiesManager("MATRIX_HOME",
													"conf" +
													File.separator
													+
													"matrix.properties", 1);
				applicationMode = propManager.getPropertyWithDetail(
						"applicationMode");
				if (applicationMode != null) {
					if (applicationMode.equalsIgnoreCase("production")) {
						propManager.setDeployMode(0);
					} else if (applicationMode.equalsIgnoreCase("debug")) {
						propManager.setDeployMode(1);
					}
				}
			}
			propValue = propManager.getPropertyWithDetail(propName).trim();
		} catch (Exception e) {
				System.err.println(
						"FATAL MATRIX ERROR: MatrixUtil class cannot load matrix properties file due to :" +
						e);

			System.out.println(
					"WARNING: MatrixUtil class cannot load matrix properties file due to :" +
					e);
		}
		return (propValue);
	}

		public static String getMatrixCacheDir(){
				String returnString = getPropertyFromPropertiesManager("jargon.filecache");
				if (returnString == null || returnString.trim().equals("")) returnString =
							getPropertyFromPropertiesManager("matrix.filecache");
			return returnString.trim();
		}


	/**
		* Formats a date to a specific format
		*
		* @param dateFormat Format to which date is formatted
		* @param inputDate Date to be formatted
		*/
	public static String formatDate(String dateFormat, Date inputDate) {
		String formattedDate = null;
		if (dateFormat != null && inputDate != null) {
			SimpleDateFormat dateFormatter = new SimpleDateFormat(dateFormat);
			formattedDate = dateFormatter.format(inputDate);
		}
		return (formattedDate);
	}

		public static ObjectFactory getObjectFactory()
		{ return objFactory; }

		/**
			* Accepts an object and wraps it in a TPart
			*/
		public static TPart createTPart(Object value) throws JAXBException
		{
				TPart part = MatrixUtil.getObjectFactory().createTPart();
				if (value instanceof String)
						part.setString( (String) value);
				else if (value instanceof java.math.BigDecimal)
						part.setNumber( (java.math.BigDecimal) value);
												else if (value instanceof java.lang.Double)
														part.setNumber(new java.math.BigDecimal(((Double)value).doubleValue()));
				else if (value instanceof CollectionList)
						part.setCollectionList( (CollectionList) value);
				else if (value instanceof Collection)
						part.setCollection( (Collection) value);
				else if (value instanceof DataSet)
						part.setDataSet( (DataSet) value);
				else if (value instanceof StringSet)
						part.setStringSet( (StringSet) value);
				else if (value instanceof StreamData)
						part.setStreamData( (StreamData) value);
												else
														part.setString(value.toString());
				return part;
		}

										/**
											* Copy all the variable values from source to target.  The only
											* field that does not get overridden is the name.
											* @param source TPart
											* @param target TPart
											*/
										public static void copyTPartValues(TPart source, TPart target)
		{
			// if a part exists, we should clear it of its previous value
			target.setString(source.getString());
			target.setNumber(source.getNumber());
			target.setCollectionList(source.getCollectionList());
															target.setCollection(source.getCollection());
			target.setDataSet(source.getDataSet());
			target.setStringSet(source.getStringSet());
															target.setStreamData(source.getStreamData());
		}

										public static Object extractVariable(TPart holder)
										{
												if(holder.getNumber() != null)
														return holder.getNumber();
												if(holder.getString() != null)
														return holder.getString();
												if(holder.getCollectionList() != null)
														return holder.getCollectionList();
												if(holder.getCollection() != null)
														return holder.getCollection();
												if(holder.getDataSet() != null)
														return holder.getDataSet();
												if(holder.getStreamData() != null)
														return holder.getStreamData();
												if(holder.getStringSet() != null)
														return holder.getStringSet();

												return null;
										}

										/**
											* Sets all the fields of the TPart except the name, to null
											* @param holder TPart
											*/
										public static void clearTPart(TPart holder)
										{
												holder.setCollection(null);
												holder.setCollectionList(null);
												holder.setDataSet(null);
												holder.setNumber(null);
												holder.setStreamData(null);
												holder.setString(null);
												holder.setStringSet(null);
										}

									/**
		* Accepts any Object (obj) and the name of one of its fields
		* (fieldToGet) and trys to get the field by calling a parameterless
		* getter method
		*
		* e.g.
		* fieldToGet = "length" would invoke obj.getLength() and return the
		* value
		*
		* @param obj Object the object from which to get the field
		*
		* @param fieldToGet String the name of the field - the capitalization
		* should look exactly like the getter method's except for the
		* first letter, which is case insensitive.
		*
		* @return Object
		*/
	public static Object getFieldByName(Object obj, String fieldToGet) throws
					NoSuchMethodException, IllegalAccessException,
					InvocationTargetException
	{

			// capitalize the first letter of the field passed in
			String capitalFirstLetter = fieldToGet.substring(0, 1).toUpperCase();
			fieldToGet = capitalFirstLetter + fieldToGet.substring(1);

			// get the getter method
			Class c = obj.getClass();
			Class[] parameterTypes = new Class[0];
			java.lang.reflect.Method method = c.getMethod("get" + fieldToGet,
													parameterTypes);

			// invoke the getter method and return the value
			return method.invoke(obj, new Object[0]);
	}

					private static long lastTime = 0;
					public static void printTimeCheck()
					{
							long time = System.currentTimeMillis();
							System.out.println(time+" ("+(time-lastTime)+" since last check)");
							lastTime = time;
					}
}
