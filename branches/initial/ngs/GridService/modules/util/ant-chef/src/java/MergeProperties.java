
package org.chefproject.ant;

import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;
import java.util.Vector;
import java.util.Iterator;
import java.io.*;

public class MergeProperties extends Task {
    private File infile = null;
    private File includefile = null;
    private File outfile = null;
    private String pattern = null;
    private boolean after = true;
    private boolean always = true;
    private boolean filter = true;
    private boolean once = true;
    private boolean verbose = false;

    private int count = 0;

    private Vector vString = new Vector();
    private Vector vFilter = new Vector();

    // Load the mapping file

    private void loadInclude() {

    	try {
        	BufferedReader in = new BufferedReader(new FileReader(includefile.getAbsolutePath()));
        	String str;
		String fstr;
		int ipos;
		int countlines = 0;
		int countfilters=0;
        	while ((str = in.readLine()) != null) {
			vString.add(str);
			countlines++;
			if ( !filter ) continue;
			fstr = str.trim();
			if (fstr.length() <= 0 ) continue;
			if ( fstr.charAt(0) == '#' ) continue;
			ipos = fstr.indexOf("=");
			// System.out.println("ipos="+ipos+" "+fstr);
			if ( ipos <= 0 || ipos >= fstr.length() - 1 ) continue;
			vFilter.add(fstr.substring(0,ipos+1));
			// System.out.println("Filter:"+fstr.substring(0,ipos+1));
			countfilters++;
        	}
        	in.close();
		if ( verbose ) {
			System.out.println("Loaded "+countlines+" lines and "+countfilters+" filters.");
		}
    	} catch (IOException e) {
		throw new BuildException("MergeProperties unable to read "+includefile);
    	}
    }

    private boolean filterMatch(String inputLine)  {

	String inp = inputLine.trim();

	Iterator i = vFilter.iterator();
	while( i.hasNext() ) {
		if ( inp.startsWith((String) i.next()) ) return true;
	}
	return false;
    }

    // The method executing the task
    public void execute() throws BuildException {
	if (infile == null ) throw new BuildException("MergeProperties requires infile to be specified");

	// Load the include file
	loadInclude();

	boolean front = false;

	String matchPattern = pattern.trim();
	if (matchPattern.length() == 0 ) matchPattern = null;
	if (matchPattern != null ) {
		if (pattern.length() > 1 && pattern.charAt(0) == '^' ) {
			matchPattern = pattern.substring(1);
			front = true;
		}
	}

	boolean found;
	String errorString = "IO Exception";
	int countfilters = 0;
	int countlines = 0;
    	try {
		errorString = "Unable to open:"+infile;
        	BufferedReader in = new BufferedReader(new FileReader(infile.getAbsolutePath()));
		errorString = "Unable to open:"+outfile;
        	PrintWriter out = new PrintWriter(new FileWriter(outfile.getAbsolutePath()));
		errorString = "Internal error";
        	String str;
        	while ((str = in.readLine()) != null) {
			countlines ++;
			if ( filterMatch(str) ) {
				countfilters++;
				continue;
			}
			found = false;
			if ( matchPattern != null && (!once || count == 0) ) {
				if ( front ) {
					found = str.indexOf(matchPattern) == 0  ;
				} else {
					found = str.indexOf(matchPattern) >= 0  ;
				}
			}
			if ( found ) {
				count ++;
				Iterator i = vString.iterator();
				if ( after ) out.println(str);
				while( i.hasNext() ) {
					out.println(i.next());
				}
				if ( ! after ) out.println(str);
			} else {
        			out.println(str);
			}
        	}

		// Should we always include at least once?
		if ( always && count == 0 ) {
			if (verbose) System.out.println("file merged at the end");
			count ++;
			Iterator i = vString.iterator();
			if ( after ) out.println(str);
			while( i.hasNext() ) {
				out.println(i.next());
			}
			if ( ! after ) out.println(str);
		}
        	in.close();
		out.close();
		if ( verbose ) {
			System.out.println(countlines+" lines read, "+countfilters+" lines removed, and "+count+" include match(es)");
		}
    	} catch (IOException e) {
		throw new BuildException("BuildProperties:"+errorString);
    	}

    }

    public void setInfile(File msg) {
        this.infile = msg;
    }
    public void setIncludeFile(File msg) {
        this.includefile = msg;
    }
    public void setOutfile(File msg) {
        this.outfile = msg;
    }
    public void setPattern(String msg) {
        this.pattern = msg;
    }
    public void setAfter(boolean value) {
	this.after = value;
    }
    public void setAlways(boolean value) {
	this.always = value;
    }
    public void setVerbose(boolean value) {
	this.verbose = value;
    }
    public void setFilter(boolean value) {
	this.filter = value;
    }
    public void setOnce(boolean value) {
	this.once = value;
    }
}

