package javacog;

import org.globus.mds.*;
import org.globus.common.*;
import java.util.*;
import javax.swing.*;
import java.awt.*;
import java.awt.event.*;

/**
 *  Query the GIIS and display the results in a table.
 *
 * @author     daniel
 * @created    January 8, 2001
 */
public class MyMDS {
    /**
     *  Perform the task
     *
     * @param  args  Default parameters, not used
     * @since
     */
    public static void main(String args[]) {
	//Just a quick test so no interactivity...
	MDS mds = new MDS("tcs10.dl.ac.uk", "2136", "o=Grid");
	try {
	    mds.connect();
	    Hashtable mdsResult = mds.search("(&(objectclass=GlobusServiceJobManager))",
					     new String[]{"hn"}, MDS.SUBTREE_SCOPE);
	    if ( mdsResult.size() == 0 ){
	    	System.out.println( "Empty result returned... exiting." );
		System.exit(1);
	    }
	    String dn;
	    //System.out.println("Got back results...");
	    //System.out.println("Hashtable with " + mdsResult.size() + " elements...");
	    Object[][] cellData = null;
	    String[] columnNames = new String[mdsResult.size() + 1];
	    columnNames[0] = "Variable";
	    int cnc = 1;
	    //Enumeration of hostnames (from "hn" in the search)
	    for (Enumeration e = mdsResult.keys(); e.hasMoreElements(); ) {
		columnNames[cnc++] = (String) (e.nextElement());
	    }
	    Vector v;
	    MDSResult mdsr;
	    for (int i = 0; i <= mdsResult.size(); i++) {
				//i.e. i refers to the jobmanager
		if (i == 0) {
		    mdsr = mds.getAttributes(columnNames[1]);
		    //0 is not a jobmanager...
		    cellData = new Object[mdsr.size()][mdsResult.size() + 1];
		    //i.e. [attribute name][job-manager]
		    int t = 0;
		    //i.e. t refers to the attribute number
		    for (Enumeration e = mdsr.keys(); e.hasMoreElements(); ) {
			cellData[t++][0] = (String) (e.nextElement());
		    }
		}
		else {
		    mdsr = mds.getAttributes(columnNames[i]);
		    //for each attribute
		    //Just for debugging...
		    //mdsr.print();
		    for (int j = 0; j < mdsr.size(); j++) {
			//Now get a list of all Attributes.
			//System.out.println("Searching for data with key = " + cellData[j][0]);
			v = mdsr.get((String) (cellData[j][0]));
			//System.out.println((v == null) ? "v == null" : "v.size()=" + v.size());
			cellData[j][i] = (v.size() > 1) ? "Multiple..." : v.elementAt(0);
		    }
		}
	    }
	    //Okay, all the data is in the object[][] now...
	    JFrame jf = new JFrame("GIIS contents");
	    jf.addWindowListener(
				 new WindowAdapter() {
					 /**
					  *  Handler for the window close icon.
					  *
					  * @param  we  Description of Parameter
					  * @since
					  */
					 public void windowClosing(WindowEvent we) {
					     System.exit(0);
					 }
				     });
	    JTable jt = new JTable(cellData, columnNames);
	    /*
	     * JTable jt = new JTable( new Object[][]{ { "0,0", "0,1" }, { "1,0", "1,1" } },
	     * new String[]{ "Column0", "Column1" } );
	     */
	    JScrollPane sp = new JScrollPane(jt);
	    jt.setPreferredSize(jt.getMaximumSize());
	    jf.getContentPane().add(sp, BorderLayout.CENTER);
	    jf.pack();
	    jf.setVisible(true);
	    mds.disconnect();
	}
	catch (MDSException e) {
	    System.err.println("Error:" + e.getMessage() + " "
			       + e.getLdapMessage());
	}
    }
}
