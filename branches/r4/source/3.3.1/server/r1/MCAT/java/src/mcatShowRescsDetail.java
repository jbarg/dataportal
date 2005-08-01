/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/*****************************************************************************/
/*                                                                           */
/*  mcatShowRescsDetail.java  brings up a detailed list of attribute values  */
/*  for a particular resource                                                */
/*                                                                           */
/*****************************************************************************/

package edu.sdsc.SrbAdmin;

import javax.swing.*;
import javax.swing.text.*;
import javax.swing.border.*;
import javax.swing.event.*;
import javax.swing.table.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.text.*;

import edu.sdsc.SrbBrowser.SrbElement;
import edu.sdsc.SrbBrowser.SrbCellRenderer;

import edu.sdsc.grid.io.srb.*;
import edu.sdsc.grid.io.*;
import edu.sdsc.grid.io.local.*;
import java.io.IOException;

/**
 * mcatShowRescsDetail
 *
 */
public class mcatShowRescsDetail extends JFrame {
    private SRBFileSystem fileSystem;
    private JTable table;

    // Fonts
    public Font msgFont
	= new Font("TimeRoman", Font.BOLD, 16);

    private String dataValues[][];

    private final static int COLUMNS=2;
    private String columnNames[] = {"Item", "Value"};


    public mcatShowRescsDetail(String RescName,
			       String RescType, SRBFileSystem srbFileSystem)
    {
	fileSystem = srbFileSystem;

	TableColumn column = null;

	int i;

	if (RescType.equals("compound")) {
	    setTitle (new String
		("Resources that are part of the Compound resource " +
		 RescName));
	}
	else {
	    setTitle (new String
		("Details Panel for Resource " + RescName));

	}

	i = readData(RescName, RescType);

	table = new JTable(dataValues, columnNames);


	column = table.getColumnModel().getColumn(0);
	column.setPreferredWidth(20);

	column = table.getColumnModel().getColumn(1);
	column.setPreferredWidth(400);

        final JScrollPane scrollPane = new JScrollPane(table);
	getContentPane().add(scrollPane);

	pack();
	if (i==0) i=1;
	int j=i*480;
	if (j>900) j=900;
	setBounds(100, 100, 900, j);

	setVisible(true);

    }

    int readData(String rescName, String rescType) {

	int Items1=20;      // Number of items in the first set 
	int Items2=7;
	int Items=Items1+Items2; // Number of items displayed (select + select2)

	MetaDataCondition[] conditions_resc=
	{ MetaDataSet.newCondition(
	       SRBMetaDataSet.RESOURCE_NAME, MetaDataCondition.EQUAL,
	       rescName )};

	MetaDataCondition[] conditions_compound=
	    { MetaDataSet.newCondition(
	       SRBMetaDataSet.COMPOUND_RESOURCE_NAME, MetaDataCondition.EQUAL,
	       rescName )};

	MetaDataSelect[] selects = {
	   MetaDataSet.newSelection(SRBMetaDataSet.RESOURCE_NAME ),
	   MetaDataSet.newSelection(SRBMetaDataSet.RESOURCE_ADDRESS_NETPREFIX),
	   MetaDataSet.newSelection(SRBMetaDataSet.RESOURCE_TYPE_NAME),
	   MetaDataSet.newSelection(SRBMetaDataSet.RESOURCE_DEFAULT_PATH),
	   MetaDataSet.newSelection(SRBMetaDataSet.
				    PHYSICAL_RESOURCE_DEFAULT_PATH),
	   MetaDataSet.newSelection(SRBMetaDataSet.PHYSICAL_RESOURCE_NAME),
	   MetaDataSet.newSelection(SRBMetaDataSet.
				    PHYSICAL_RESOURCE_TYPE_NAME ),
	   MetaDataSet.newSelection(SRBMetaDataSet.RESOURCE_REPLICATION_ENUM),
	   MetaDataSet.newSelection(SRBMetaDataSet.RESOURCE_CLASS),
	   MetaDataSet.newSelection(SRBMetaDataSet.RSRC_COMMENTS),
	   MetaDataSet.newSelection(SRBMetaDataSet.RSRC_CREATE_DATE),
	   MetaDataSet.newSelection(SRBMetaDataSet.RSRC_MODIFY_DATE),
	   MetaDataSet.newSelection(SRBMetaDataSet.RSRC_MAX_OBJ_SIZE),
	   MetaDataSet.newSelection(SRBMetaDataSet.RSRC_OWNER_NAME),
	   MetaDataSet.newSelection(SRBMetaDataSet.RSRC_OWNER_DOMAIN),
	   MetaDataSet.newSelection(SRBMetaDataSet.RSRC_OWNER_ZONE),
	};

	MetaDataSelect[] selects2 = {
	    MetaDataSet.newSelection(SRBMetaDataSet.RSRC_MLSEC_LATENCY_MAX),
	    MetaDataSet.newSelection(SRBMetaDataSet.RSRC_MLSEC_LATENCY_MIN),
	    MetaDataSet.newSelection(SRBMetaDataSet.RSRC_MBPS_BANDWIDTH),
	    MetaDataSet.newSelection(SRBMetaDataSet.RSRC_CONCURRENCY_MAX),
	    MetaDataSet.newSelection(SRBMetaDataSet.RSRC_NUM_OF_HIERARCHIES),
	    MetaDataSet.newSelection(SRBMetaDataSet.RSRC_NUM_OF_STRIPES),
	    MetaDataSet.newSelection(SRBMetaDataSet.RSRC_MEGAB_CAPACITY),
	};

	MetaDataSelect[] selects3 = {
	   MetaDataSet.newSelection(SRBMetaDataSet.RSRC_ACCESS_CONSTRAINT),
	   MetaDataSet.newSelection(SRBMetaDataSet.RSRC_ACCS_GRPUSER_NAME),
	   MetaDataSet.newSelection(SRBMetaDataSet.RSRC_ACCS_GRPUSER_DOMAIN),
	   MetaDataSet.newSelection(SRBMetaDataSet.RSRC_ACCS_GRPUSER_ZONE),
	};

	MetaDataRecordList[] rl = null;
	MetaDataRecordList[] rl3 = null;

	try {
	    rl = fileSystem.query( conditions_resc, selects );
	} catch (IOException e) {
	    //throw away
	    return 0;
	}
	
	if (rl == null) {
	    dataValues = new String[1][Items+1];
	    dataValues[0][1]=
		"Error, no results returned for query on resource";
	    return 0;
	}

	try {
	    // get the multiple sets of access constraints
	    rl3 = fileSystem.query( conditions_resc, selects3 ); 
	} catch (IOException e){ 
	    //throw away
	    return 0;
	}
	int rl3Length = 0;
	if (rl3 != null) rl3Length=rl3.length;


	int NumResources = rl.length;
	int j;
	j = (NumResources*Items)+(rl.length-1); // i sets of Items plus the 
	                                        // ----- lines in between
	String phyRescs[] = new String[NumResources];
        dataValues = new String[j][2];
	j=0;
	for (int i=0; i<rl.length; i++) {
	    j=(i*Items);
	    if (i>0) j=j+i;

	    dataValues[j][0]="RESOURCE_NAME";
	    dataValues[j++][1]=rl[i].getValue(
		  SRBMetaDataSet.RESOURCE_NAME).toString();

	    dataValues[j][0]="RESOURCE_ADDRESS_NETPREFIX";
	    dataValues[j++][1]=rl[i].getValue(
		  SRBMetaDataSet.RESOURCE_ADDRESS_NETPREFIX).toString();

	    dataValues[j][0]="RESOURCE_TYPE_NAME";
	    dataValues[j++][1]=rl[i].getValue(
		  SRBMetaDataSet.RESOURCE_TYPE_NAME).toString();

	    dataValues[j][0]="RESOURCE_DEFAULT_PATH";
	    dataValues[j++][1]=rl[i].getValue(
		  SRBMetaDataSet.RESOURCE_DEFAULT_PATH).toString();

	    dataValues[j][0]="PHYSICAL_RESOURCE_DEFAULT_PATH";
	    dataValues[j++][1]=rl[i].getValue(
		  SRBMetaDataSet.PHYSICAL_RESOURCE_DEFAULT_PATH).toString();

	    dataValues[j][0]="PHYSICAL_RESOURCE_NAME";
	    dataValues[j][1]=rl[i].getValue(
		  SRBMetaDataSet.PHYSICAL_RESOURCE_NAME).toString();
	    phyRescs[i]=dataValues[j][1];
	    j++;

	    dataValues[j][0]="PHYSICAL_RESOURCE_TYPE_NAME";
	    dataValues[j++][1]=rl[i].getValue(
		  SRBMetaDataSet.PHYSICAL_RESOURCE_TYPE_NAME).toString();

	    dataValues[j][0]="RESOURCE_REPLICATION_ENUM";
	    dataValues[j++][1]=rl[i].getValue(
		  SRBMetaDataSet.RESOURCE_REPLICATION_ENUM).toString();

	    dataValues[j][0]="RESOURCE_CLASS";
	    dataValues[j++][1]=rl[i].getValue(
		  SRBMetaDataSet.RESOURCE_CLASS).toString();

	    dataValues[j][0]="RSRC_COMMENTS";
	    dataValues[j++][1]=rl[i].getValue(
		  SRBMetaDataSet.RSRC_COMMENTS).toString();

	    dataValues[j][0]="RSRC_CREATE_DATE";
	    dataValues[j++][1]=rl[i].getValue(
		  SRBMetaDataSet.RSRC_CREATE_DATE).toString();

	    dataValues[j][0]="RSRC_MODIFY_DATE";
	    dataValues[j++][1]=rl[i].getValue(
		  SRBMetaDataSet.RSRC_MODIFY_DATE).toString();

	    dataValues[j][0]="RSRC_MAX_OBJ_SIZE";
	    dataValues[j++][1]=rl[i].getValue(
	    	  SRBMetaDataSet.RSRC_MAX_OBJ_SIZE).toString();

	    dataValues[j][0]="RSRC_OWNER_NAME";
	    dataValues[j++][1]=rl[i].getValue(
	    	  SRBMetaDataSet.RSRC_OWNER_NAME).toString();

	    dataValues[j][0]="RSRC_OWNER_DOMAIN";
	    dataValues[j++][1]=rl[i].getValue(
	    	  SRBMetaDataSet.RSRC_OWNER_DOMAIN).toString();

	    dataValues[j][0]="RSRC_OWNER_ZONE";
	    dataValues[j++][1]=rl[i].getValue(
	    	  SRBMetaDataSet.RSRC_OWNER_ZONE).toString();

	    dataValues[j][0]="RSRC_ACCESS_CONSTRAINT";
	    dataValues[j][1]=new String("");
	    for (int k=0; k<rl3Length; k++) {
		dataValues[j][1]+= "(" + k + "):";
		dataValues[j][1]=dataValues[j][1] + " " + rl3[k].getValue(
	    	  SRBMetaDataSet.RSRC_ACCESS_CONSTRAINT).toString() + "  " ;
	    }
	    j++;

	    dataValues[j][0]="RSRC_ACCS_GRPUSER_NAME";
	    dataValues[j][1]=new String("");
	    for (int k=0; k<rl3Length; k++) {
		dataValues[j][1]+= "(" + k + "):";
		dataValues[j][1]=dataValues[j][1] + " " + rl3[k].getValue(
	    	  SRBMetaDataSet.RSRC_ACCS_GRPUSER_NAME).toString() + "  " ;
	    }
	    j++;

	    dataValues[j][0]="RSRC_ACCS_GRPUSER_DOMAIN";
	    dataValues[j][1]=new String("");
	    for (int k=0; k<rl3Length; k++) {
		dataValues[j][1]+= "(" + k + "):";
		dataValues[j][1]=dataValues[j][1] + " " + rl3[k].getValue(
	    	  SRBMetaDataSet.RSRC_ACCS_GRPUSER_DOMAIN).toString() + "  " ;
	    }
	    j++;

	    dataValues[j][0]="RSRC_ACCS_GRPUSER_ZONE";
	    dataValues[j][1]=new String("");
	    for (int k=0; k<rl3Length; k++) {
		dataValues[j][1]+= "(" + k + "):";
		dataValues[j][1]=dataValues[j][1] + " " + rl3[k].getValue(
	    	  SRBMetaDataSet.RSRC_ACCS_GRPUSER_ZONE).toString() + "  " ;
	    }
	    j++;
	}

	// Now try to get the second set of values, which may not
	// exist.  If they don't, display blanks for the values.
	// Query on just the physical resource names recorded above.

	j=Items1;
	for (int i=0; i<NumResources; i++) {

	    MetaDataCondition[] conditions2=
	    { MetaDataSet.newCondition(
	       SRBMetaDataSet.PHYSICAL_RESOURCE_NAME, MetaDataCondition.EQUAL,
	       phyRescs[i] ) 
	    };

	    try {
		rl = fileSystem.query( conditions2, selects2 );
	    } catch (IOException e){ 
		//throw away
		return 0;
	    }

	    j=(i*Items)+Items1;    // start at the second set of items
	    if (i>0) j=j+i;        // offset by one for the "----" lines between

	    dataValues[j][0]="RSRC_MLSEC_LATENCY_MAX";
	    if (rl == null) {
		dataValues[j++][1]="";
	    }
	    else {
		dataValues[j++][1]=rl[0].getValue(
		     SRBMetaDataSet.RSRC_MLSEC_LATENCY_MAX).toString();
	    }

	    dataValues[j][0]="RSRC_MLSEC_LATENCY_MIN";
	    if (rl == null) {
		dataValues[j++][1]="";
	    }
	    else {
		dataValues[j++][1]=rl[0].getValue(
		     SRBMetaDataSet.RSRC_MLSEC_LATENCY_MIN).toString();
	    }

	    dataValues[j][0]="RSRC_MBPS_BANDWIDTH";
	    if (rl == null) {
		dataValues[j++][1]="";
	    }
	    else {
		dataValues[j++][1]=rl[0].getValue(
		     SRBMetaDataSet.RSRC_MBPS_BANDWIDTH).toString();
	    }

	    dataValues[j][0]="RSRC_CONCURRENCY_MAX";
	    if (rl == null) {
		dataValues[j++][1]="";
	    }
	    else {
		dataValues[j++][1]=rl[0].getValue(
		     SRBMetaDataSet.RSRC_CONCURRENCY_MAX).toString();
	    }

	    dataValues[j][0]="RSRC_NUM_OF_HIERARCHIES";
	    if (rl == null) {
		dataValues[j++][1]="";
	    }
	    else {
		dataValues[j++][1]=rl[0].getValue(
		     SRBMetaDataSet.RSRC_NUM_OF_HIERARCHIES).toString();
	    }

	    dataValues[j][0]="RSRC_NUM_OF_STRIPES";
	    if (rl == null) {
		dataValues[j++][1]="";
	    }
	    else {
		dataValues[j++][1]=rl[0].getValue(
		     SRBMetaDataSet.RSRC_NUM_OF_STRIPES).toString();
	    }

	    dataValues[j][0]="RSRC_MEGAB_CAPACITY";
	    if (rl == null) {
		dataValues[j++][1]="";
	    }
	    else {
		dataValues[j++][1]=rl[0].getValue(
		     SRBMetaDataSet.RSRC_MEGAB_CAPACITY).toString();
	    }

	    // Add dashes between resources
	    if (NumResources>1 && i+1<NumResources) {
		dataValues[j][0]  ="------------------";
		dataValues[j++][1]="------------------";
	    }
	}

	return NumResources;
    }

    public static void main(String[] args) {
        try {
	    mcatShowRescsDetail myMcatShowRescsDetail =
		new mcatShowRescsDetail("foo", "baz", new SRBFileSystem());
        } catch (Exception e) {
            System.exit(0);
        }
    }
}

