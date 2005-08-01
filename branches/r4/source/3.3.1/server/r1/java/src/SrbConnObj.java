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
/*  WHAT:   SrbConnObj.java                                                  */
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

// import java.awt.*;

public class SrbConnObj
	extends Object
{
	private String host;
	private String port;
	private String stat;
	private String srvs;


	public SrbConnObj()
	{
		host = new String( "" );
		port = new String( "" );
		stat = new String( "Not Authenticated" );
		srvs = new String( "srb" );
	}

	public SrbConnObj( String hn, String pt )
	{
		host = hn;
		port = pt;
		stat = new String( "Not Authenticated" );
		srvs = new String( "srb" );
	}


	public void setHost( String host )
	{
		this.host = host;
	}

	public void setPort( String port )
	{
		this.port = port;
	}

	public void setStat( String stat )
	{
		this.stat = stat;
	}

	public void setSrvs( String srvs )
	{
		this.srvs = srvs;
	}


	public String getHost()
	{
		return host;
	}

	public String getPort()
	{
		return port;
	}

	public String getStat()
	{
		return stat;
	}

	public String getSrvs()
	{
		return srvs;
	}
}

