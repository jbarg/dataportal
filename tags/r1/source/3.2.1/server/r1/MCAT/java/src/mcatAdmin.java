/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/



/*****************************************************************************/
/*                                                                           */
/*  WHAT:   mcatAdmin.java                                                   */
/*                                                                           */
/*  WHY:    xxx                                                              */
/*                                                                           */
/*  WHERE:  San Diego Supercomputer Center (SDSC)                            */
/*                                                                           */
/*  WHEN:   2/03, new version                                                */
/*                                                                           */
/*  HOW:    JAVA                                                             */
/*                                                                           */
/*****************************************************************************/

package edu.sdsc.SrbAdmin;

import javax.swing.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.text.*;

import edu.sdsc.grid.io.srb.*;
import edu.sdsc.grid.io.*;
import edu.sdsc.grid.io.local.*;
import java.io.IOException;

/**
 * mcatAdmin  (Srb/Mcat Admin)
 *
 */
public class mcatAdmin
        extends         JFrame
        implements      ActionListener
{
    private final int       ITEM_PLAIN      =       0;      // Item types
    private final int       ITEM_CHECK      =       1;
    private final int       ITEM_RADIO      =       2;

    public static String EXIT = "Exit";
    public static String HELP = "Help";
    public static String CONNECT = "Connect to a Server";
    public static String SHOW_ZONES = "Display Zones";
    public static String NEW_ZONES = "Create Zones";
    public static String MOD_ZONES = "Modify Zones";
    public static String DEL_ZONES = "Delete Zones";
    public static String SHOW_USERS = "Display User Information";
    public static String NEW_USERS = "Add New Users";
    public static String MOD_USERS = "Modify User Info";
    public static String ADD_GROUP = "Create New Groups";
    public static String DEL_USERS = "Delete Users";

    public static String SHOW_RESCS = "Display Resources";
    public static String SHOW_LOCATIONS = "Display Locations";
    public static String NEW_LOC = "Add New Location";
    public static String NEW_RESC = "Create Physical Resource";
    public static String NEW_LOG_RESC = "Create Logical Resource";
    public static String ADD_TO_LOG_RESC = "Add to Logical Resource";
    public static String RM_FROM_LOG_RESC = "Remove from Logical Resource";
    public static String NEW_COMP_RESC = "Create Compound Resource";
    public static String ADD_TO_COMP_RESC = "Add to Compound Resource";
    public static String DEL_LOG_RESC = "Delete Logical Resource";
    public static String DEL_RESC = "Delete Physical Resource";
    public static String DEL_LOC = "Delete a Location";

    public static String SHOW_TOKENS = "Display Tokens";
    public static String NEW_TOKEN = "Add New Token";
    public static String NEW_DOMAIN = "Add New Domain";


    private SRBFileSystem fileSystem;
    private SRBAccount mySRBAccount;

    mcatNewZone myNewZone;
    mcatConnect myConnect;
    mcatShowZones myShowZones;
    mcatModZone myModZone;
    mcatDelZone myDelZone;
    mcatShowUsers myShowUsers;
    mcatNewUser	myNewUser;
    mcatModUser	myModUser;
    mcatDelUser myDelUser;
    mcatNewGroup myNewGroup;
    mcatShowRescs myShowRescs;
    mcatShowTokens myShowLocs;
    mcatNewLoc	myNewLoc;
    mcatShowTokens	myShowTokens;
    mcatNewToken	myNewToken;
    mcatNewDomain       myNewDomain;
    mcatNewResource	myNewResc;
    mcatNewLogResc	myNewLogResc;
    mcatAddLogResc	myAddLogResc;
    mcatRmFromLogResc	myRmFromLogResc;
    mcatNewCompoundResc	myNewCompResc;
    mcatAddToCompResc   myAddToCompResc;
    mcatDelLogResc myDelLogResc;
    mcatDelResc myDelResc;
    mcatDelLoc  myDelLoc;

    private JPanel          topPanel;
    private JMenuBar        menuBar;

    private JMenu           menuFile;
    private JMenuItem       menuFileHelp;
    private JMenuItem       menuFileConnect;
    private JMenuItem       menuFileExit;

    private JMenu           menuManZones;
    private JMenuItem       menuManZonesNew;

    private JMenu           menuManUsers;
    private JMenuItem       menuManUsersNew;
    private JMenuItem       menuManUsersMod;
    private JMenuItem       menuManUsersDel;
    private JMenuItem       menuManUsersNewGroup;

    private JMenu           menuManResc;
    private JMenuItem       menuManRescShowRescs;
    private JMenuItem       menuManRescShowLocs;
    private JMenuItem       menuManRescNewLoc;
    private JMenuItem       menuManRescNewPhy;
    private JMenuItem       menuManRescAddToLog;
    private JMenuItem       menuManRescRmFromLog;
    private JMenuItem       menuManRescNewComp;
    private JMenuItem       menuManRescAddToComp;
    private JMenuItem       menuManRescLogDel;
    private JMenuItem       menuManRescDel;    
    private JMenuItem       menuManRescDelLoc;

    private JMenu           menuToken;
    private JMenuItem       menuTokenNewToken;
    private JMenuItem       menuTokenNewDomain;

    private JTextArea myText;
    private boolean doingHelp=false;
    private boolean skipConnect=false;

    public mcatAdmin(String inArg) {

	if (inArg.equals("Help")) doingHelp=true;
	if (inArg.equalsIgnoreCase("NoConnect")) skipConnect=true;

	if (doingHelp==false) {
	    //	    String myHome = System.getProperty("user.home");
	    //	    System.out.println("myHome is " + myHome + "\n");

	    // connect to the srb
	    boolean wasSecurityException;
	    if (!skipConnect) {
		wasSecurityException=false;
		try {
		    fileSystem = new SRBFileSystem();
		}
		catch (SecurityException se) {
		    wasSecurityException=true;
		}
		catch (IOException e) {
		}
		if (fileSystem==null) {
		    if (wasSecurityException) {
			myText = new JTextArea("The attempt to connect to the SRB server failed.  Check your authentication environment (the ~/.srb/.MdasAuth in particular) and try again.  Or you can enter parameters and try again in the Connect window.  Until you connect, other operations via this tool (other than help) will also fail.  ");
		    }
		    else {
			myText = new JTextArea("The attempt to connect to the SRB server failed.  You can enter parameters and try again in the Connect window.  Until you connect, other operations via this tool (other than help) will also fail.  Make sure the SRB is running and that your authentication environment is set up properly (~/.srb/.MdasEnv and ~/.srb/.MdasAuth).  ");
		    }
		}
	    }
	    else {
		myText = new JTextArea("Auto SRB connect bypassed.  You can enter parameters to connect in the Connect window.");
	    }
	    if (fileSystem==null) {
		myConnect = new mcatConnect (this, false);
	    }
	    setTitle( "The Srb/Mcat Admin Main panel");
	    setSize( 310, 130 );
	}
	else {
	    setTitle( "The Srb/Mcat Admin Help panel");
	    setSize(550,450);
	    setLocation(310,1);
	    myText = new JTextArea(
"This is the Help window for the SRB/MCAT administration tool.\nClick on a pull-down menu item to get a description of that function.\nUse the File/Help pull-down option for a general introduction.\n");
	    myText.setLineWrap(true);
	    myText.setWrapStyleWord(true);
	    getContentPane().add( myText );
	    show();
	}

	topPanel = new JPanel();
	topPanel.setLayout( new BorderLayout() );
	getContentPane().add( topPanel );

	if (myText != null) {
	    myText.setLineWrap(true);
	    myText.setWrapStyleWord(true);
	    getContentPane().add( myText );
	}

	// Create the menu bar
	menuBar = new JMenuBar();

	// Set this instance as the application's menu bar
	setJMenuBar( menuBar );

        // Create the File menu
	menuFile = new JMenu( "File" );
	menuFile.setMnemonic( 'F' );
	menuBar.add( menuFile );

	menuFileHelp = CreateMenuItem( menuFile, ITEM_PLAIN,
		      HELP, null, 'H', "Display a help window");
	menuFileConnect = CreateMenuItem( menuFile, ITEM_PLAIN,
		      CONNECT, null, 'C', "Connect to a SRB Server");
	menuFileExit = CreateMenuItem( menuFile, ITEM_PLAIN,
		      EXIT, null, 'E', "Exit the program");

        // Create the Manage Zones menu
	menuManZones = new JMenu( "Zones" );
	menuManZones.setMnemonic( 'Z' );
	menuBar.add( menuManZones );

        // Build a Manage Zone menu items
	menuManZonesNew = CreateMenuItem( menuManZones, ITEM_PLAIN,
		      NEW_ZONES, null, 'D', "Create new zones");
	menuManZonesNew = CreateMenuItem( menuManZones, ITEM_PLAIN,
		      SHOW_ZONES, null, 'D', "Display zones");
	menuManZonesNew = CreateMenuItem( menuManZones, ITEM_PLAIN,
		      MOD_ZONES, null, 'M', "Modify zones");
	//	menuManZonesNew = CreateMenuItem( menuManZones, ITEM_PLAIN,
	//		      DEL_ZONES, null, 'D', "Delete zones");
	// (Delete zones not implemented in Mcat)

        // Create the Manage Users menu
	menuManUsers = new JMenu( "Users" );
	menuManUsers.setMnemonic( 'U' );
	menuBar.add( menuManUsers );

        // Build a Manage Users menu items
	menuManUsersNew = CreateMenuItem( menuManUsers, ITEM_PLAIN,
		      SHOW_USERS, null, 'd', "Display user information");
	menuManUsersNew = CreateMenuItem( menuManUsers, ITEM_PLAIN,
		      NEW_USERS, null, 'A', "Add new users");
        menuManUsersMod = CreateMenuItem( menuManUsers, ITEM_PLAIN,
                      MOD_USERS, null, 'M', "Modify User Info" );
        menuManUsersNewGroup = CreateMenuItem( menuManUsers, ITEM_PLAIN,
                      ADD_GROUP,null, 'C', "Create New Groups" );
        menuManUsersDel = CreateMenuItem( menuManUsers, ITEM_PLAIN,
                      DEL_USERS, null, 'D', "Delete users" );


        // Create the Manage Resources menu
	menuManResc = new JMenu( "Resources" );
	menuManResc.setMnemonic( 'R' );
	menuBar.add( menuManResc );

        // Build a Manage Resource menu items

	menuManRescShowRescs = CreateMenuItem( menuManResc, ITEM_PLAIN,
                SHOW_RESCS, null, 'S', "Display Resources");
	menuManRescShowLocs = CreateMenuItem( menuManResc, ITEM_PLAIN,
                SHOW_LOCATIONS, null, 'L', "Display Locations");
	menuManRescNewLoc = CreateMenuItem( menuManResc, ITEM_PLAIN,
                NEW_LOC, null, 'A', "Create a new Location");
	menuManRescNewPhy = CreateMenuItem( menuManResc, ITEM_PLAIN,
                NEW_RESC, null, 'C', "Create a Physical Resource");
	menuManRescNewLoc = CreateMenuItem( menuManResc, ITEM_PLAIN,
                NEW_LOG_RESC, null, 'C', "Add a new Logical Resource");
	menuManRescAddToLog = CreateMenuItem( menuManResc, ITEM_PLAIN,
                ADD_TO_LOG_RESC, null, 'A',
		"Add a Physical Resource to a Logical Resource");
	menuManRescRmFromLog = CreateMenuItem( menuManResc, ITEM_PLAIN,
                RM_FROM_LOG_RESC, null, 'R',
		"Remove a Physical Resource from a Logical Resource");
	menuManRescNewComp = CreateMenuItem( menuManResc, ITEM_PLAIN,
                NEW_COMP_RESC, null, 'C', "Create a new Compound Resource");
	menuManRescAddToComp = CreateMenuItem( menuManResc, ITEM_PLAIN,
		ADD_TO_COMP_RESC, null, 'A',
                "Add a Physical Resource to a Compound Resource");
	menuManRescLogDel = CreateMenuItem( menuManResc, ITEM_PLAIN,
		DEL_LOG_RESC, null, 'D', "Delete a Logical Resource");
	menuManRescDel = CreateMenuItem( menuManResc, ITEM_PLAIN,
		DEL_RESC, null, 'D', "Delete a Physical Resource");
	menuManRescDelLoc = CreateMenuItem( menuManResc, ITEM_PLAIN,
		DEL_LOC, null, 'D', "Delete a Location");

        // Create the Manage Resources menu
	menuToken = new JMenu( "Tokens" );
	menuToken.setMnemonic( 'T' );
	menuBar.add( menuToken );
        // Build a Manage Resource menu items
	menuTokenNewToken = CreateMenuItem( menuToken, ITEM_PLAIN,
                SHOW_TOKENS, null, 'D', "Display current tokens");
	menuTokenNewToken = CreateMenuItem( menuToken, ITEM_PLAIN,
                NEW_TOKEN, null, 'C', "Create a new Token");
	menuTokenNewDomain = CreateMenuItem( menuToken, ITEM_PLAIN,
                NEW_DOMAIN, null, 'C', "Create a new Domain token");

    }

    public JMenuItem CreateMenuItem( JMenu menu, int iType, String sText,
				     ImageIcon image, int acceleratorKey,
				     String sToolTip )
    {
	// Create the item
	JMenuItem menuItem;

	switch( iType )
	    {
	    case ITEM_RADIO:
		menuItem = new JRadioButtonMenuItem();
		break;

	    case ITEM_CHECK:
		menuItem = new JCheckBoxMenuItem();
		break;

	    default:
		menuItem = new JMenuItem();
		break;
	    }

	// Add the item test
	menuItem.setText( sText );

	// Add the optional icon
	if( image != null )
	    menuItem.setIcon( image );

	// Add the accelerator key
	if( acceleratorKey > 0 )
                        menuItem.setMnemonic( acceleratorKey );

	// Add the optional tool tip text
	if( sToolTip != null )
	    menuItem.setToolTipText( sToolTip );

	// Add an action handler to this menu item
	menuItem.addActionListener( this );

	menu.add( menuItem );

	return menuItem;
    }

    public void actionPerformed( ActionEvent event )
    {
	//	System.out.println( event );

	if (doingHelp == true) {
	    actionPerformedHelp(event);
	    return;
	}

        String actionCommand = event.getActionCommand();
        if (actionCommand.equals(EXIT)) {
	    System.exit(0);
	}
        if (actionCommand.equals(HELP)) {
	    new mcatAdmin("Help");
	}
        if (actionCommand.equals(CONNECT)) {
	    if (myConnect == null) {
		boolean connected;
		connected=false;
		if (fileSystem!=null) connected=true;
		myConnect = new mcatConnect (this, connected);
	    }
	    else {
		myConnect.setVisible(true);
	    }
	}
        if (actionCommand.equals(SHOW_ZONES)) {
	    if (myShowZones == null) {
		myShowZones = new mcatShowZones (fileSystem);
	    } else {
		//		myShowZones.setVisible(true);
		myShowZones = new mcatShowZones (fileSystem);
	    }
	}
        if (actionCommand.equals(NEW_ZONES)) {
	    if (myNewZone == null) {
		myNewZone = new mcatNewZone (fileSystem);
	    } else {
		myNewZone.setVisible(true);
	    }
	}
        if (actionCommand.equals(MOD_ZONES)) {
	    if (myModZone == null) {
		myModZone = new mcatModZone (fileSystem);
	    } else {
		myModZone.setVisible(true);
	    }
	}
        if (actionCommand.equals(DEL_ZONES)) {
	    if (myDelZone == null) {
		myDelZone = new mcatDelZone (fileSystem);
	    } else {
		myDelZone.setVisible(true);
	    }
	}

        if (actionCommand.equals(SHOW_USERS)) {
	    // Just create a new one with current information
	    myShowUsers = new mcatShowUsers (fileSystem);
	}
        if (actionCommand.equals(NEW_USERS)) {
	    if (myNewUser == null) {
		myNewUser = new mcatNewUser (fileSystem);
	    } else {
		myNewUser.setVisible(true);
	    }
	}
        if (actionCommand.equals(MOD_USERS)) {
	    if (myModUser == null) {
		myModUser = new mcatModUser (fileSystem);
	    } else {
		myModUser.setVisible(true);
	    }
	}
        if (actionCommand.equals(ADD_GROUP)) {
	    if (myNewGroup == null) {
		myNewGroup = new mcatNewGroup (fileSystem);
	    } else {
		myNewGroup.setVisible(true);
	    }
	}
        if (actionCommand.equals(DEL_USERS)) {
	    if (myDelUser == null) {
		myDelUser = new mcatDelUser (fileSystem);
	    } else {
		myDelUser.setVisible(true);
	    }
	}
        if (actionCommand.equals(SHOW_RESCS)) {
	    if (myShowRescs == null) {
		myShowRescs = new mcatShowRescs (fileSystem);
	    } else {
		myShowRescs.setVisible(true);
	    }
	}
        if (actionCommand.equals(SHOW_LOCATIONS)) {
	    // Just create a new one with current information
	    myShowLocs = new mcatShowTokens (fileSystem, "Location");
	}
        if (actionCommand.equals(NEW_LOC)) {
	    if (myNewLoc == null) {
		myNewLoc = new mcatNewLoc (fileSystem);
	    } else {
		myNewLoc.setVisible(true);
	    }
	}
        if (actionCommand.equals(NEW_RESC)) {
	    if (myNewResc == null) {
		myNewResc = new mcatNewResource (fileSystem);
	    } else {
		myNewResc.setVisible(true);
	    }
	}
        if (actionCommand.equals(NEW_LOG_RESC)) {
	    if (myNewLogResc == null) {
		myNewLogResc = new mcatNewLogResc (fileSystem);
	    } else {
		myNewLogResc.setVisible(true);
	    }
	}
        if (actionCommand.equals(ADD_TO_LOG_RESC)) {
	    if (myAddLogResc == null) {
		myAddLogResc = new mcatAddLogResc (fileSystem);
	    } else {
		myAddLogResc.setVisible(true);
	    }
	}
        if (actionCommand.equals(RM_FROM_LOG_RESC)) {
	    if (myRmFromLogResc == null) {
		myRmFromLogResc = new mcatRmFromLogResc (fileSystem);
	    } else {
		myRmFromLogResc.setVisible(true);
	    }
	}
        if (actionCommand.equals(NEW_COMP_RESC)) {
	    if (myNewCompResc == null) {
		myNewCompResc = new mcatNewCompoundResc (fileSystem);
	    } else {
		myNewCompResc.setVisible(true);
	    }
	}
        if (actionCommand.equals(ADD_TO_COMP_RESC)) {
	    if (myAddToCompResc == null) {
		myAddToCompResc = new mcatAddToCompResc (fileSystem);
	    } else {
		myAddToCompResc.setVisible(true);
	    }
	}
        if (actionCommand.equals(DEL_LOG_RESC)) {
	    if (myDelLogResc == null) {
		myDelLogResc = new mcatDelLogResc (fileSystem);
	    } else {
		myDelResc.setVisible(true);
	    }
	}
        if (actionCommand.equals(DEL_RESC)) {
	    if (myDelResc == null) {
		myDelResc = new mcatDelResc (fileSystem);
	    } else {
		myDelResc.setVisible(true);
	    }
	}
        if (actionCommand.equals(DEL_LOC)) {
	    if (myDelLoc == null) {
		myDelLoc = new mcatDelLoc (fileSystem);
	    } else {
		myDelLoc.setVisible(true);
	    }
	}
        if (actionCommand.equals(SHOW_TOKENS)) {
	    // Just create a new one with current information
	    myShowTokens = new mcatShowTokens (fileSystem, "");
	}
        if (actionCommand.equals(NEW_TOKEN)) {
	    if (myNewToken == null) {
		myNewToken = new mcatNewToken (fileSystem);
	    } else {
		myNewToken.setVisible(true);
	    }
	}
        if (actionCommand.equals(NEW_DOMAIN)) {
	    if (myNewDomain == null) {
		myNewDomain = new mcatNewDomain (fileSystem);
	    } else {
		myNewDomain.setVisible(true);
	    }
	}
    }

    public void actionPerformedHelp( ActionEvent event )
    {
        String All_Commands[] = {
           EXIT, HELP, CONNECT,
	   SHOW_ZONES, NEW_ZONES, MOD_ZONES,
	   SHOW_USERS, NEW_USERS, MOD_USERS, ADD_GROUP, DEL_USERS,
	   SHOW_RESCS, SHOW_LOCATIONS, NEW_LOC, NEW_RESC, NEW_LOG_RESC,
           ADD_TO_LOG_RESC, NEW_COMP_RESC,
	   ADD_TO_COMP_RESC, DEL_LOG_RESC, 
           DEL_RESC, DEL_LOC, SHOW_TOKENS, NEW_TOKEN, NEW_DOMAIN };
	String All_Help[] = {
    " - This will exit the program.  \nFor this help window, use the X box.\n",

    " - This normally brings up this Help window system, and here displays this general introduction.\n\nThis is the SRB/MCAT (http://www.npaci.edu/DICE/SRB) administration tool. You must be an SRB administrator (as defined in the MCAT database) to use it.  The SRB server (and MCAT) that you connect with is defined in your ~/.srb/.MdasEnv file (see other documentation for how to set these up).\n\nEach function in this tool brings up another window for input.  In many cases, this tool will list available choices for certain options based on the currently defined set within your MCAT database.  You then click Execute to send the request to the SRB/MCAT and either a success or error message will be displayed below.  Being an free-standing window, you can move and resize the window to anywhere on the screen.  You can close the window, and bring it back, and it will retain the settings and results from previous operations during the current SAT session (the object remains when closed).  The Clear/Refresh button can be used to again query the MCAT for current values (if you've changed something).\n\nThe normal sequence when bringing up a new MCAT is to initialize the database (with some predefined users), bring up the SRB server, and start this tool (using the predefined user name and password in ~/.srb/.MdasEnv and .MdasAuth).  Then you would create a new Admin user, reset your environment (.MdasEnv) to be that user, restart the SRB server so that it picks up its new userid (from your .MdasEnv), and then remove the pre-defined admin user used to 'boot' the system.\n\nAfter the above initialization (or using an existing MCAT) one would typically create a new Location (under Resources), and then create Physical, Logical, and/or Compound Resources.  And then add Users and Groups.  See the other windows for additional help.\n",

    " - Connect to an SRB Server/MCAT.  If the automatic connecting fails for any reason, you can enter the needed parameters in this panel.  Connections can fail if the server isn't running, or the .MdasEnv or .MdasAuth can't be found (in home/.srb), or if there is a problem with the contents of those files.\n\nYou can also use this panel to disconnect from the current SRB Server (and its MCAT) and connect to another one.\n\nYou can bypass the automatic attempt to connect by including 'noconnect' on the command line.\n\nIf there is no automatic connection at startup, or if the connection fails, the Connect panel will be brought up automatically.\n",

    " - Shows the currently defined Zones.  Clicking on a row brings up a detailed view of that Zone.\n",

    " - Creates one or more Zones; ie adds Zone information to the local MCAT.  These can be either local or remote.  Only one local zone can be created.  When you create a local Zone, be sure to reserve and register the name with the SRB Zone Authority to make it globally unique in the namespace, at http://www.npaci.edu/dice/srb/zoneAuthority.html\n\nFollow these steps to create a zone:\n\n1) Create the new Domain, using the name used at the remote site.\n2) Create the admin user for the new Domain, using the user name and Domain used at remote site; type is 'sysadmin'.\n3) Create a new Location for the remote site, using the admin user name and domain used at remote site.\n4) Create a remote Zone, using the admin user name, Domain, and Location used at the remote site.\n5) Modify the new admin user, changing their home Zone from yours to the remote Zone.\n\nBe sure to complete step 5 to prevent the remote admin from have full privileges in your zone.  The user account has to be defined for step 4, but then needs to be changed to be part of the remote zone as described in step 5.",

    " - Modifies one or more Zones attributes.  All the significant fields that are specified when a Zone is created can be changed, even including the name and the local/remote flag.  Be sure to register/reserve local zone names with the SRB Zone Authority at http://www.npaci.edu/dice/srb/zoneAuthority.html\n",

    " - Displays information concerning selected users.  Select a domain, and it will display the users that are part of that domain.  Then select a user, and information on that user will be displayed.\n",

    " - Adds a new SRB user to your system.\n\nYou enter a name and password for the user, and click on one of the available domains and user types.  Phone number, Email, and Address are optional.  The parent collection is always /home; this may be adjustable in the future.\n\nYou may want to add your own domain before adding users (Tokens/Domain).  Other user types can be also be added (Tokens/Add New Token).\n",

    " - Modifies attributes of SRB users.\n\nClicking on a domain will list the users that exist in that domain.  You then select an operation and it will display available choices.\n\nUsers always belong to two special  groups: 'public' and a group with the same name as their userid.  If you attempt to remove users from these groups an error will be returned (USER_GROUP_NOT_IN_CAT).  But for completeness, these special groups are included in the group list display.\n",

    " - Creates a new group.  Enter the information requested and click Execute.\n",

    " - Deletes users from the system.  Click on an existing domain (listed in the window), and the users in the domain will be listed.  Click on a user and then click Execute and a delete on that user will be attempted.\n\nThe error 'ACTION_NOT_ALLOWED' means that the delete is prevented because the user still owns something, typically the user has files stored.  In this case, the srbLog file will have more information.",

    " - Shows the currently defined Resources.  Clicking on a row brings up a detailed view of that Zone.\n",

    " - Shows the currently defined Locations.  This actually displays the Location tokens by bringing up a Display Tokens window with Locations selected.\n",

    " - Creates a new Location.  When you later create a Physical Resource, you will specify a Location that it belongs to.  You enter a name for the location, and host address (the full internet domain name), select a descriptive Parent Location (this is informational only), server user domain, and enter the server user ID.",

    " - Creates a new Physical Resource.  This is an underlying system that actually stores the files.  A simple one would be a Unix file system.  You enter a name, path string, pick a Location, Resource Type, and Resource Class.  You normally don't modify the subpath definition; it specifies how to generate the actual files names as they are stored.  The Resource Type tells the SRB which driver to use when talking to the resource; that is, whether this resource is a unix file system, Oracle DB Binary Objects, HPSS, etc.",

    " - Creates a new Logical Resource.",

    " - Adds another Physical Resource to a Logical Resource.",

    " - Creates a Compound Resource.  A Compound Resource has one (or more) cache resources and one tape or archival resource (more exactly, the Class indicates one is cache and another permanent).  These need to be under the same Location.  Users will see one copy of files stored.  The admin 'dumptape' command will move files from the cache to tape or archive.  This provides much of the functionality of a full archival storage system.",

    " - Adds another Physical Resource to a Compound Resource.",

    " - Deletes a Logical Resource, leaving in place the Physical Resources that make up the Logical Resource.",

    " - Deletes a Physical Resource. \n\nThe error 'ACTION_NOT_ALLOWED' means that the delete is prevented because the resource is in use, typically data has been stored into it.  This includes data moved to the trash (starting with SRB 3.0.2), so you may need to run Srmtrash before the resource will be empty.",

    " - Deletes a Location.\n\nThe error 'ACTION_NOT_ALLOWED' means that the delete is prevented because the location is associated with something, typically a resource has been created under it.",

    " - Displays Tokens. A Token is a descriptive item in the MCAT database.  This panel displays the main token types, and when you click on one, it will display the important values for those types of tokens as currently defined in your MCAT",

    " - Adds a new Token.  A Token is a descriptive item in the MCAT database.  There are quite a few predefined Tokens and you can add more via this if you want.  The Tokens can be either a ResourceType, DataType, UserType, Domain, or Action.  The predefined UserTypes include 'assistant manager', 'grad student', 'staff', etc.  But if you wanted another UserType, say 'slacker', you could add it via this.  In this UserType case, a Parent Value is required and it needs to be one of the existing UserTypes (the window used to create new users will show them).\n\nThe extenders and mimetypes apply only to Datatype tokens.\n\nThe extenders are like the Windows extension portion of file names, the part used to identify a file type, for example: .doc, .txt, .c, and .pl.  It is used by the SRB in finding what is the data type of the given file.  For example, if a user Sputs a file called 'foo.doc' it uses the extender info to give it the SRB data type 'MSWord Document' as its datatype.\n\nThe SRB mimetype is sent as a web mimetype so that the inQ and mySRB browsers can launch the correct application for each type of file that was downloaded.",

    " - Adds a new Domain.  A Domain is a string describing an administrative domain.  User names, for example, are combined with the Domain to be globally unique.  A Domain is a type of Token, and could be added via the Add New Token window, but is easier with this one.  You need to pick a parent for your domain name in the domain name hierarchy.  This is generally invisible and does not affect operations.  For many sites, a good choice is home (the root).  But if you want, you can denote to your site's relationship to other organizations by placing it under one of the others; SDSC under NPACI for example.  This is completely arbitrary though."

    };


        String actionCommand = event.getActionCommand();

	for ( int i=0 ; i< All_Commands.length; i++) {
	    if (actionCommand.equals(All_Commands[i])) {
		myText.setText(All_Commands[i]);
		if (i < All_Help.length) myText.append(All_Help[i]);
	    }
	}
    }

// This method is called by from the mcatConnect class
// after the user enters parameters and clicks execute
    public int returnSRBAccount(SRBAccount mySRBAccount)
    {
	String myHost;
	//	System.out.println("Retrying\n");
	myHost = mySRBAccount.getHost();
	if (myText == null) {
	    myText = new JTextArea();
	    myText.setLineWrap(true);
	    myText.setWrapStyleWord(true);
	    getContentPane().add( myText );
	}

	try {
	    fileSystem = new SRBFileSystem(mySRBAccount);
	    myText.setText(
               "Connection/authentication to " + myHost + " succeeded, other commands should work now.");
	    clearObjs();
	    return 1;
	}
        catch (SecurityException se) {
	    myText.setText("Connection/authenticaion failed, check your password.");
	    return 0;
	}
	catch (IOException e2) {
	    myText.setText("Connect/authenticate attempt failed.");
	    return 0;
	}
    }

    private void clearObjs() {
	// Clear out all the possibly existing sub-windows except myConnect.
	myNewZone=null;
	myShowZones=null;
	myModZone=null;
	myDelZone=null;
	myShowUsers=null;
    	myNewUser=null;
    	myModUser=null;
	myDelUser=null;
	myNewGroup=null;
	myShowRescs=null;
	myShowLocs=null;
	myNewLoc=null;
	myShowTokens=null;
    	myNewToken=null;
	myNewDomain=null;
    	myNewResc=null;
    	myNewLogResc=null;
    	myAddLogResc=null;
    	myRmFromLogResc=null;
    	myNewCompResc=null;
	myAddToCompResc=null;
	myDelLogResc=null;
	myDelResc=null;
	myDelLoc=null;
    }


    public static void main(String[] args) {

	mcatAdmin myMcatAdmin;
	if (args.length > 0) {
	    myMcatAdmin = new mcatAdmin(args[0]);
	}
	else {
	    myMcatAdmin = new mcatAdmin("");
	}
	myMcatAdmin.setVisible( true );
    }
}
