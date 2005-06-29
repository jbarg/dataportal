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
/*  WHAT:   SrbElement.java                                                  */
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

package edu.sdsc.SrbBrowser;

import java.awt.*;
// import com.sun.java.swing.ImageIcon;
import javax.swing.ImageIcon;

public class SrbElement
	extends Object
{
	private ImageIcon icon;
	private String    name;
	private Object    data;


	public SrbElement()
	{
	}

	public SrbElement( String name )
	{
		this.name = name;
	}

	public SrbElement( String name, ImageIcon icon )
	{
		this.name = name;
		this.icon = icon;
	}

	public SrbElement( String name, ImageIcon icon, Object data )
	{
		this.name = name;
		this.icon = icon;
		this.data = data;
	}


	public void setIcon( ImageIcon icon )
	{
		this.icon = icon;
	}

	public void setName( String name )
	{
		this.name = name;
	}

	public void setData( Object data )
	{
		this.data = data;
	}


	public ImageIcon getIcon()
	{
		return icon;
	}

	public String getName()
	{
		return name;
	}

	public Object getData()
	{
		return data;
	}

}

