/*
	* 	SDSC Matrix. A Gridflow Management System for data grids and digtal libraries
	*  Copyright (C) 2004 SDSC Matrix Project
	*
	* This library is free software; you can redistribute it and/or
	* modify it under the terms of the GNU Lesser General Public
	* License as published by the Free Software Foundation; either
	* version 2.1 of the License, or (at your option) any later version.
	*
	* This library is distributed in the hope that it will be useful,
	*	but WITHOUT ANY WARRANTY; without even the implied warranty of
	*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	*	Lesser General Public License for more details.
	*
	*	You should have received a copy of the GNU Lesser General Public
	*	License along with this library; if not, write to the Free Software
	*	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
	*
	* ====================================================================
	*
	* This software consists of voluntary contributions from the developers of the SDSC
	* Matrix Project. We request that all redistribution and software made using this code
	* acknowledge their use of the “SDSC Matrix project” in their end-user documentation or
	* in their website.
	*
	* SDSC Matrix Project (list of developers as of Dec 2005)
	*	Designer & Architect: Arun swaran Jagatheesan (arun@sdsc.edu)
	* 9500 Gilman Drive, MC0505
	* San Diego Supercomputer Center
	* University of California
	* La Jolla CA 92093
	*
	* Allen Ding (alding@sdsc.edu)
	* Lucas Gilbert (iktome@sdsc.edu)
	* Arun Jagatheesan (arun@sdsc.edu)
	* Reena Mathew	(rmathew@sdsc.edu)
	* Daniel Moore (mixx@umail.ucsb.edu)
	* Erik Vandekieft (evk@sdsc.edu)
	* Jonathan Weinberg (jonw@sdsc.edu)
	*
	*/


/*
	* VirtualOrganizationInfo.java
	*
	* Checked for DGL 3.0 on Dec 15,2003
	* Created on July 3, 2003, 5:40 AM
	*/
package edu.sdsc.matrix.srb.client;

/**
	* Represents a SRB domain or VO info. Has infor for srb server and the port.
	*
	* Contains methods to create VirtualOrganizationInfo objects.
	* @author  Allen Ding - SDSC
	*/
public class VirtualOrganizationInfo extends edu.sdsc.matrix.srb.parser.impl.
			VirtualOrganizationInfoImpl {

	public VirtualOrganizationInfo(edu.sdsc.matrix.srb.parser.VirtualOrganizationInfo voInfo)
	{
			this.setDescription(voInfo.getDescription());
			this.setPort(voInfo.getPort());
			this.setServer(voInfo.getServer());
			this.setVoID(voInfo.getVoID());
	}

	/**
		* Creates a VirtualOrganizationInfo object with the voID.
		* @param voID A String.
		*/
	public VirtualOrganizationInfo(String voID) {
		super.setVoID(voID);
	}

	/**
		* Creates a VirtualOrganizationInfo object with the specified
		* server and port.
		* @param server Server to use.
		* @param port Port on the server.
		*/
	public VirtualOrganizationInfo(String server, int port) {
		super.setServer(server);
		super.setPort(port);
	}

	/**
		* Makes the VOI object use the specified voID. Using this method
		* clobbers any server and port information.
		* @param voID The voID to use.
		*/
	public void useVOID(String voID) {
		super.setServer(null);
		super.setPort(0);
		super.setVoID(voID);
	}

	/**
		* Makes the VOI object use the specified server and port. Using this
		* method clobbers any voID information.
		* @param server Server to use.
		* @param port Port on the server.
		*/
	public void useServerAndPort(String server, int port) {
		super.setServer(server);
		super.setPort(port);
	}
}
