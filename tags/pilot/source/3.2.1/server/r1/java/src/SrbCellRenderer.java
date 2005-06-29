/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/
 


/*****************************************************************************/
/*                                                                           */
/*  WHO:    John L. Moreland, Mike Wan                                       */
/*                                                                           */
/*  WHAT:   SrbCellRenderer.java                                             */
/*                                                                           */
/*  WHY:    xxx                                                              */
/*                                                                           */
/*  WHERE:  San Diego Supercomputer Center (SDSC)                            */
/*                                                                           */
/*  WHEN:   Tue Mar 24 14:45:59 PST 1998                                     */
/*                                                                           */
/*  HOW:    JAVA                                                             */
/*                                                                           */
/*****************************************************************************/

// import com.sun.java.swing.*;
// import com.sun.java.swing.text.*;

package edu.sdsc.SrbBrowser;

import javax.swing.*;
import javax.swing.text.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import edu.sdsc.SrbBrowser.SrbElement;


/**
 * SrbCellRenderer
 *
 * @version 1.0 10/14/97
 * @author John Moreland
 */
public class SrbCellRenderer
	extends JLabel
	implements ListCellRenderer
{
	public Color highlightColor;

	public SrbCellRenderer()
	{
		setOpaque( true );
		// Use Courier because the width is fixed.
		setFont(new Font("Courier", Font.BOLD, 12));
		highlightColor = new Color( 0, 0, 128 );
	}

	public Component getListCellRendererComponent(
		JList list,
		Object value,
		int index,
		boolean isSelected,
		boolean cellHasFocus )
	{
		SrbElement se = (SrbElement) value;
		setText( se.getName() );

		if ( isSelected )
		{
			setBackground( highlightColor );
			setForeground( Color.white );
		}
		else
		{
			setBackground( Color.white );
			setForeground( Color.black );
		}

		setIcon( se.getIcon() );

		return this;
	}
}

