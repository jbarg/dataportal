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

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.Properties;

/**
	* Properties Manager class is used by the Matrix Engine to inquire on the runtime properties.
	* The properties manager uses the matrix.properties file to provide the values. The properties
	* manager can be used both in production and debug/development environments.
	*
	* @author Arun swaran Jagatheesan (arun@sdsc.edu)
	* @since Matrix 2.0
	*/
public class PropertiesManager extends java.util.Properties {
	// change class access to package?
	public static final int PRODUCTION_MODE = 0;
	public static final int DEBUG_MODE = 1;
	private static boolean logStarted = false;
	private static boolean doSysErrLog = true;
	private int deploymentMode = 1;
	private String pathAsSystemProperty;
	private String propertiesFile;
	private String fullFileName;
	/**
		* Creates a new Properties Manager for any application to manage configuaration details.
		* @param pathAsSystemProperty Path of the file identified as a system property (e.g) "MATRIX_HOME"
		* @param propertileFile The file that has the properties or config details.
		* The <i>pathAsSystemProperty</i> is concatenated with a file.separator and this propertilesFile to
		* identify the properties file.
		* @param deploymentMode If used in PRODUCTION_MODE (=0) or DEBUG_MODE (=1).
		* <p> In PRODUCTION_MODE, the properties file is read only once during the start of the application.
		* In DEBUG_MODE (=1), the properties file is read every time on demand. The user can change settings
		* without restarting the application server. </p>
		* <br>
		* <p><i> Usage Example:</i>  new PropertiesManager("MATRIX_HOME", "matrix.properties", 1); </p>
		*
		* <p> It is suggested that the application open the properties manager in debug mode and then
		* read the properties file (to see the logging level and deployment mode) to decide to run in
		* production mode or debug mode. Once it makes this decision by reading the properties file,
		* the application can change the mode of the properties manager using setDeploy Mode.
		*  </p>
		*/
	public PropertiesManager(String pathAsSystemProperty,
								String propertiesFile,
								int deploymentMode) throws IOException,
				SecurityException,
				IllegalArgumentException {
		super();
		String location = "";
		try {
			this.pathAsSystemProperty = pathAsSystemProperty;
			this.propertiesFile = propertiesFile;
			location = System.getProperty(pathAsSystemProperty);
			//System.out.println(pathAsSystemProperty+"="+location);
			if ( (location == null) || (location.trim().equals(""))) {
				location = getEnvFirstTime(pathAsSystemProperty);
				if ( (location == null) || (location.trim().equals(""))) {
					toLog(pathAsSystemProperty + " is not set.");
					throw new NullPointerException();
				}
			}
			if ( (propertiesFile == null) || (propertiesFile.trim().equals(""))) {
				toLog(propertiesFile + " is not valid file name.");
				throw new IllegalArgumentException();
			}
			fullFileName = location + System.getProperty("file.separator") +
						propertiesFile;
			this.load(new FileInputStream(fullFileName));
		} catch (SecurityException se) {
			toLog("Access denied to read system properties.");
			throw se;
		} catch (IllegalArgumentException iae) {
			toLog("Illegal Arguments for SystemProperty or the properties file");
			throw iae;
		} catch (IOException ie) {
			toLog("File not found: " + location
						+ System.getProperty("file.separator")
						+ propertiesFile);
			throw ie;
		} catch (NullPointerException ne) {
			throw new NullPointerException("System environment variable " +
													pathAsSystemProperty
													+ " is not set or can not be read");
			//throw new IllegalArgumentException(ne);
		}
	}

	/**
		* get a property. null is returned in case any exception is thrown.
		*/
	public String getProperty(String key) {
		try {
			return getPropertyWithDetail(key);
		} catch (Exception e) {
			toLog("Exception:" + e.toString());
			return null;
		}
	}

	/**
		* get a property. "With detail" is use to indicate that exception can be thrown
		*/
	public String getPropertyWithDetail(String key) throws IOException,
				SecurityException, IllegalArgumentException {
		if (deploymentMode >= DEBUG_MODE) { //on demand
			loadAgain();
		}
		return super.getProperty(key);
	}

	/**
		* Loads the config info again from the properties file
		*/
	private void loadAgain() throws IOException, SecurityException,
				IllegalArgumentException {
		String location = "";
		try {
			location = System.getProperty(pathAsSystemProperty);
			if ( (location == null) || (location.trim().equals(""))) {
				/*
					* If unable to find form System.properties call, uses the previous
					* file name got from the constructor. Does not call the getEnvFirstTime again, as it
					* makes a JNI call by forking a new process every time.
					*/
				// This if is unnecessary. but just in case.
				if ( (fullFileName == null) || (fullFileName.trim().equals(""))) {
					location = getEnvFirstTime(pathAsSystemProperty);
					fullFileName = location +
								System.getProperty("file.separator") + propertiesFile;
				}
			} else {
				// dynamically lookup the properties file
				fullFileName = location + System.getProperty("file.separator") +
							propertiesFile;
			}
			// load properties again
			this.load(new FileInputStream(fullFileName));
		} catch (SecurityException se) {
			toLog("Access denied to read system properties.");
			throw se;
		} catch (IllegalArgumentException iae) {
			toLog("Illegal Arguments for SystemProperty");
			throw iae;
		} catch (IOException ie) {
			toLog("File not found: " + location
						+ System.getProperty("file.separator")
						+ propertiesFile);
			throw ie;
		} catch (NullPointerException ne) {
			throw new IllegalArgumentException();
		}
	}

	private static String getEnvFirstTime(String pathAsSystemProperty) throws
				IOException {
		// THis is used if all else fails.
		// WITH thanks and due credit to http://www.rgagnon.com/javadetails/java-0150.html
		toLog("Using getEnvFirstTime()");
		Process p = null;
		Properties envVars = new Properties();
		Runtime r = Runtime.getRuntime();
		String OS = System.getProperty("os.name").toLowerCase();
		// System.out.println(OS);
		if (OS.indexOf("windows 9") > -1) {
			p = r.exec("command.com /c set");
		} else if ( (OS.indexOf("nt") > -1)
							|| (OS.indexOf("windows 2000") > -1)
							|| (OS.indexOf("windows xp") > -1)) {
			// thanks to JuanFran for the xp fix!
			p = r.exec("cmd.exe /c set");
		} else {
			// our last hope, we assume Unix (thanks to H. Ware for the fix)
			p = r.exec("env");
		}
		BufferedReader br = new BufferedReader
					(new InputStreamReader(p.getInputStream()));
		String line;
		while ( (line = br.readLine()) != null) {
			int idx = line.indexOf('=');
			String key = line.substring(0, idx);
			String value = line.substring(idx + 1);
			envVars.setProperty(key, value);
			//System.out.println("added:"+ key + " = " + value );
		}
		return envVars.getProperty(pathAsSystemProperty).trim();
	}

	/**
		* The application can change the deployment mode to control
		* if the properties file is cached or is read every time on demand.
		*
		*@param mode 0 = Production, 1 = debug
		*/
	public void setDeployMode(int mode) {
		this.deploymentMode = mode;
	}

	public static void toLog(String msg) {
		if (doSysErrLog) {
			System.err.println("Warning: " + msg);
		}
	} //toLog
	/*  EXAMPLE usage....
		*
		*
			public static String getStringInput ( int min, int max, BufferedReader in) throws IOException{
		// arun@sdsc.edu
		String returnValue;
		do{
			System.out.flush();
			String input = in.readLine();
			try{
		returnValue = input.trim();
		if ((input.length() < min) || (input.length() > max)) throw new NumberFormatException("Out of desired input range");
		else return returnValue;
			}catch(Exception e){
			System.out.println("Enter a string of length between "+min+" and "+max);
			}
		}while(true);
		//return returnValue;
			}
			public static void main(String s[]){
		try{
			BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
			PropertiesManager pm = new PropertiesManager("MATRIX_HOME", "matrix.properties", 1);
			System.out.println("prop.1 = "+pm.getProperty("prop.1"));
			System.out.println("prop.2 = "+pm.getProperty("prop.2"));
			System.out.println("The config file is loaded. Enter a property to check dynamically");
			String propTest = PropertiesManager.getStringInput(1,20,in);
			System.out.println(propTest +" = "+ pm.getProperty(propTest));
			System.out.println("Now change the value of "+propTest+" in the file and press enter");
			PropertiesManager.getStringInput(0,20,in);
			System.out.println(propTest +" = "+ pm.getProperty(propTest));
		}catch(Exception e){
			e.printStackTrace();
		}
			}//main
		*/
} //class
