/**************************************************************************
Copyright Notice
All Rights Reserved
Please refer to files in COPYRIGHT directory under the SRB software directory 
for copyright, usage, liability and license information.
Please read these files before using,modifying or distributing SRB software.
**************************************************************************/


/*****************************************************************************/
/*                                                                           */
/*  WHO:    John L. Moreland  , Mike Wan                                     */
/*                                                                           */
/*  WHAT:   SrbAuthObj.java                                                  */
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

public class SrbAuthObj
	extends Object
{
	private String user;
	private String domain;
	private String pass;
	private String cert;


	public SrbAuthObj()
	{
		user = new String( "" );
		domain = new String( "" );
		pass = new String( "" );
		cert = new String( "Not Implemented" );
	}

	public SrbAuthObj( String un, String pw )
	{
		user = un;
		domain = new String( "" );
		pass = pw;
		cert = new String( "Not Implemented" );
	}

	public SrbAuthObj( String un, String dom, String pw )
	{
		user = un;
		domain = dom;
		pass = pw;
		cert = new String( "Not Implemented" );
	}


	public void setUser( String user )
	{
		this.user = user;
	}

	public void setDomain( String dom )
	{
		this.domain = dom;
	}

	public void setPass( String pass )
	{
		this.pass = pass;
	}

	public void setCert( String cert )
	{
		this.cert = cert;
	}


	public String getUser()
	{
		return user;
	}

	public String getDomain()
	{
		return domain;
	}

	public String getPass()
	{
		return pass;
	}

	public String getCert()
	{
		return cert;
	}

}

