

/**
 * <p>Title: </p>
 * <p>Description: </p>
 * <p>Copyright: Copyright (c) 2003</p>
 * <p>Company: </p>
 * @author not attributable
 * @version 1.0
 */

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

	public static String getEnvFirstTime(String pathAsSystemProperty) throws
	   IOException {
		// THis is used if all else fails.
		// WITH thanks and due credit to http://www.rgagnon.com/javadetails/java-0150.html
		//toLog("Using getEnvFirstTime()");
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