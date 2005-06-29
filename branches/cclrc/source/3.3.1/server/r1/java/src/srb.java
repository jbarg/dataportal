/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 
// import com.sun.java.swing.*;
// import com.sun.java.swing.text.*;
// import com.sun.java.swing.border.*;
import javax.swing.*;
import javax.swing.text.*;
import javax.swing.border.*;

import java.awt.*;
import java.awt.event.*;
import java.applet.*;
import java.net.*;

import SrbTool;


public class srb
	extends Applet
{
	JFrame  srbToolFrame;
	SrbTool srbTool;
	int width  = 720;
	int height = 300;

	// Enable class to run as application or applet
//XXXXX	public static void main( String[] args )
//	{
//		new AppletFrame( new srb(), width, height );
//	}

      public static void main( String[] args )
      {
		srb mySrb;

//              new AppletFrame( new srb(), width, height );
	        mySrb = new srb();
		mySrb.init ();
      }

	public void init()
	{
		JButton srbToolButton = new JButton( "SrbTool" );
		add( srbToolButton );
		ActionListener srbToolButtonListener = new ActionListener()
		{
			public void actionPerformed( ActionEvent e )
			{
				openTool();
			}
		};
		srbToolButton.addActionListener( srbToolButtonListener );

		openTool();
	}

	public void openTool()
	{
		if ( srbToolFrame == null )
		{
			srbToolFrame = new JFrame( "SrbTool" );
			srbToolFrame.setSize( width, height );
			srbToolFrame.getContentPane().setLayout( new BorderLayout() );  // HACK
/*
			srbToolFrame.setLayout( new BorderLayout() );  // HACK
*/
		}
		srbToolFrame.setVisible( true );
		if ( srbTool == null )
		{
			srbTool = new SrbTool();
			srbToolFrame.getContentPane().add( srbTool );
			srbToolFrame.setSize( width+1, height ); // HACK to force repaint
		}
	}
}

