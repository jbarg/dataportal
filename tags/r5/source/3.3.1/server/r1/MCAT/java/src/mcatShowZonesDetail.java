/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/*****************************************************************************/
/*                                                                           */
/*  WHAT:   mcatShowZones.java                                               */
/*                                                                           */
/*  WHY:    xxx                                                              */
/*                                                                           */
/*  WHERE:  San Diego Supercomputer Center (SDSC)                            */
/*                                                                           */
/*  WHEN:   8/2003                                                           */
/*                                                                           */
/*  HOW:    JAVA                                                             */
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


import edu.sdsc.grid.io.srb.*;
import edu.sdsc.grid.io.*;
import edu.sdsc.grid.io.local.*;
import java.io.IOException;

/**
 * mcatShowZonesDetail
 *
 */
public class mcatShowZonesDetail extends JFrame {

    private static String modMsgStr = "To modify Zone information, click on a row";

    private SRBFileSystem fileSystem;

    private JTable table;
    private final static int COLUMNS=14;
    private String          dataValues[][];

    public Font msgFont
        = new Font("TimeRoman", Font.BOLD, 16);

    private JTextArea msgText;

    public mcatShowZonesDetail(String ZoneName, SRBFileSystem srbFileSystem)
    {
        fileSystem = srbFileSystem;

        setTitle (new String ("Display Zones details panel"));

        msgText = new JTextArea (3, 60);
        msgText.setLineWrap(true);
        msgText.setWrapStyleWord(true);
        //        msgText.setFont(msgFont);

        getContentPane().add (msgText);

        readData(ZoneName);

        pack();

        setBounds(100, 100, 640, 260);

        setVisible(true);

    }

    void readData(String zoneName) {
        MetaDataCondition[] conditions = { MetaDataSet.newCondition(
           SRBMetaDataSet.ZONE_NAME, MetaDataCondition.EQUAL, zoneName ) };

        MetaDataSelect[] selects = {
            MetaDataSet.newSelection( SRBMetaDataSet.ZONE_NAME ),
            MetaDataSet.newSelection( SRBMetaDataSet.ZONE_LOCALITY ),
            MetaDataSet.newSelection( SRBMetaDataSet.ZONE_LOCN_DESC ),
            MetaDataSet.newSelection( SRBMetaDataSet.ZONE_NETPREFIX ),
            MetaDataSet.newSelection( SRBMetaDataSet.ZONE_PORT_NUM ),
            MetaDataSet.newSelection( SRBMetaDataSet.ZONE_ADMIN_AUTH_SCHEME_NAME ),
            MetaDataSet.newSelection( SRBMetaDataSet.ZONE_ADMIN_DISTIN_NAME ),
            MetaDataSet.newSelection( SRBMetaDataSet.ZONE_STATUS ),
            MetaDataSet.newSelection( SRBMetaDataSet.ZONE_CREATE_DATE ),
            MetaDataSet.newSelection( SRBMetaDataSet.ZONE_MODIFY_DATE ),
            MetaDataSet.newSelection( SRBMetaDataSet.ZONE_COMMENTS ),
            MetaDataSet.newSelection( SRBMetaDataSet.ZONE_CONTACT ),
            MetaDataSet.newSelection( SRBMetaDataSet.ZONE_ADMIN_NAME ),
            MetaDataSet.newSelection( SRBMetaDataSet.ZONE_ADMIN_DOMAIN_NAME )
        };

        MetaDataRecordList[] rl = null;
        try {
            rl = fileSystem.query( conditions, selects );
        } catch (IOException e) {
            //throw away?
            return;
        }

        if (rl != null) {
            for (int i=0; i<rl.length; i++) {
                msgText.append("Name:");
                msgText.append(rl[i].getValue(SRBMetaDataSet.ZONE_NAME).toString()  + "\n");;
                msgText.append("Locality (1=local):");
                msgText.append(rl[i].getValue(SRBMetaDataSet.ZONE_LOCALITY).toString()  + "\n");;
                msgText.append("Location:");
                msgText.append(rl[i].getValue(SRBMetaDataSet.ZONE_LOCN_DESC).toString()  + "\n");;
                msgText.append("NetPrefix:");
                msgText.append(rl[i].getValue(SRBMetaDataSet.ZONE_NETPREFIX).toString()  + "\n");;
                msgText.append("Port Number:");
                msgText.append(rl[i].getValue(SRBMetaDataSet.ZONE_PORT_NUM).toString()  + "\n");;
                msgText.append("Administrator Authentication Scheme:");
                msgText.append(rl[i].getValue(SRBMetaDataSet.ZONE_ADMIN_AUTH_SCHEME_NAME).toString()  + "\n");;
                msgText.append("Administrator GSI Distinguished Name:");
                msgText.append(rl[i].getValue(SRBMetaDataSet.ZONE_ADMIN_DISTIN_NAME).toString()  + "\n");;
                msgText.append("Status (1=active=normal):");
                msgText.append(rl[i].getValue(SRBMetaDataSet.ZONE_STATUS).toString()  + "\n");;
                msgText.append("Create Date:");
                msgText.append(rl[i].getValue(SRBMetaDataSet.ZONE_CREATE_DATE).toString()  + "\n");;
                msgText.append("Modify Date:");
                msgText.append(rl[i].getValue(SRBMetaDataSet.ZONE_MODIFY_DATE).toString()  + "\n");;
                msgText.append("Comments:");
                msgText.append(rl[i].getValue(SRBMetaDataSet.ZONE_COMMENTS).toString()  + "\n");;
                msgText.append("Contact:");
                msgText.append(rl[i].getValue(SRBMetaDataSet.ZONE_CONTACT).toString()  + "\n");;
                msgText.append("Administrator Name:");
                msgText.append(rl[i].getValue(SRBMetaDataSet.ZONE_ADMIN_NAME).toString()  + "\n");;
                msgText.append("Administrator Domain:");
                msgText.append(rl[i].getValue(SRBMetaDataSet.ZONE_ADMIN_DOMAIN_NAME).toString()  + "\n");;
            }
        }
    }

    public static void main(String[] args) {
        try {
            mcatShowZonesDetail myMcatShowZonesDetail =
                new mcatShowZonesDetail("foo", new SRBFileSystem());
        } catch (Exception e) {
            System.exit(0);
        }
    }
}
