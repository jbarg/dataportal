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

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.lang.reflect.Method;
import java.net.URL;

/**
	* A CompilingClassLoader compiles your Java source on-the-fly.  It
	* checks for nonexistant .class files, or .class files that are older
	* than their corresponding source code.
	*
	* Programmer credit:
	* Arun Jagatheesan would like to credit  Greg Travis (mito@panix.com) for this java file
	* https://www6.software.ibm.com/developerworks/education/j-classloader/j-classloader-6-1.html
	*
	*/
public class CompileClassLoader extends ClassLoader {
	// Given a filename, read the entirety of that file from disk
	// and return it as a byte array.
	private byte[] getBytes(String filename) throws IOException {
		// Find out the length of the file
		File file = new File(filename);
		long len = file.length();
		// Create an array that's just the right size for the file's
		// contents
		byte raw[] = new byte[ (int) len];
		// Open the file
		FileInputStream fin = new FileInputStream(file);
		// Read all of it into the array; if we don't get all,
		// then it's an error.
		int r = fin.read(raw);
		if (r != len) {
			throw new IOException("Can't read all, " + r + " != " + len);
		}
		// Don't forget to close the file!
		fin.close();
		// And finally return the file contents as an array
		return raw;
	}

	// Spawn a process to compile the java source code file
	// specified in the 'javaFile' parameter.  Return a true if
	// the compilation worked, false otherwise.
	public boolean compile(String javaFile) throws IOException {
		// Let the user know what's going on
		MatrixUtil.toLog("CCL: Compiling " + javaFile + "...",MatrixUtil.DEBUG_LOG);
		// Start up the compiler
		Process p = Runtime.getRuntime().exec("javac " + javaFile);
		// Wait for it to finish running
		try {
			p.waitFor();
		} catch (InterruptedException ie) {
			MatrixUtil.toLog(ie.toString(),MatrixUtil.ERROR_LOG);
		}
		// Check the return code, in case of a compilation error
		int ret = p.exitValue();
		// Tell whether the compilation worked
		return ret == 0;
	}

	// The heart of the ClassLoader -- automatically compile
	// source as necessary when looking for class files
	public Class loadClass(String name, boolean resolve) throws
			ClassNotFoundException {
		// Our goal is to get a Class object
		Class clas = null;
		// First, see if we've already dealt with this one
		clas = findLoadedClass(name);
		//System.out.println( "findLoadedClass: "+clas );
		// Create a pathname from the class name
		// E.g. java.lang.Object => java/lang/Object
		String fileStub = name.replace('.', '/');
		// Build objects pointing to the source code (.java) and object
		// code (.class)
		String javaFilename = fileStub + ".java";
		String classFilename = fileStub + ".class";
		File javaFile = new File(javaFilename);
		File classFile = new File(classFilename);
		//System.out.println( "j "+javaFile.lastModified()+" c "+
		//  classFile.lastModified() );
		// First, see if we want to try compiling.  We do if (a) there
		// is source code, and either (b0) there is no object code,
		// or (b1) there is object code, but it's older than the source
		if (javaFile.exists() &&
			(!classFile.exists() ||
				javaFile.lastModified() > classFile.lastModified())) {
			try {
				// Try to compile it.  If this doesn't work, then
				// we must declare failure.  (It's not good enough to use
				// and already-existing, but out-of-date, classfile)
				if (!compile(javaFilename) || !classFile.exists()) {
					throw new ClassNotFoundException("Compile failed: " +
														javaFilename);
				}
			} catch (IOException ie) {
				// Another place where we might come to if we fail
				// to compile
				throw new ClassNotFoundException(ie.toString());
			}
		}
		// Let's try to load up the raw bytes, assuming they were
		// properly compiled, or didn't need to be compiled
		try {
			// read the bytes
			byte raw[] = getBytes(classFilename);
			// try to turn them into a class
			clas = defineClass(name, raw, 0, raw.length);
		} catch (IOException ie) {
		// This is not a failure!  If we reach here, it might
		// mean that we are dealing with a class in a library,
		// such as java.lang.Object
		}
		//System.out.println( "defineClass: "+clas );
		// Maybe the class is in a library -- try loading
		// the normal way
		if (clas == null) {
			clas = findSystemClass(name);
		}
		//System.out.println( "findSystemClass: "+clas );
		// Resolve the class, if any, but only if the "resolve"
		// flag is set to true
		if (resolve && clas != null) {
			resolveClass(clas);
			// If we still don't have a class, it's an error
		}
		if (clas == null) {
			throw new ClassNotFoundException(name);
		}
		// Otherwise, return the class
		return clas;
	}
}
