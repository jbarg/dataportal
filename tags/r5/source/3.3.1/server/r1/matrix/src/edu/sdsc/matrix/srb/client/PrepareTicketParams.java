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
	* Developer Log: PrepareTicketParams.java
	*
	* Added more constructors, elaborate comments and format on Dec 3, 2004
	* Created on July 9, 2003, 3:40 AM
	*/
package edu.sdsc.matrix.srb.client;

/**
	* Parameters required to prepares or expire a login ticket to Matrix/SRB.
	*
	* Any one who has a Matrix ticketId can accesss Matrix and do operations as
	* though he was the real user who creatred the ticekeId.
	*
	* Ticket Ids could also be used to cache a connection for reuse. Every time a ticket
	* is created, a connection is made to the SRB server and the socket is kept open to
	* handle further requests. The next requests need waste time by opening a new
	* socket.  Care must be taken by the developers to close (expire) all the tickets, else
	* lot of open sockets might delay the SRB servers.
	*
	* This class contains methods to create PrepareTicketParams objects.
	* @author  Allen Ding, Arun swaran Jagatheesan
	*/
public class PrepareTicketParams extends edu.sdsc.matrix.srb.parser.impl.
		PrepareTicketParamsImpl{

			/**
				* Internal method used to wrap a parser interface PrepareTicketParams object into this
				* class' representation. This method should not normally be used by the developer.
				* @param ui The parser.PrepareTicketParams to wrap.
				*/
			protected PrepareTicketParams(edu.sdsc.matrix.srb.parser.PrepareTicketParams
					ptp){
						super.setParentUser(ptp.getParentUser());
						super.setGridInfo(ptp.getGridInfo());
						super.setStartTime(ptp.getStartTime());
						super.setEndTime(ptp.getEndTime());
						super.setUsageCount(ptp.getUsageCount());
			}

			/**
				* Creates an PrepareTicketParams object in DGL. This is used to prepare ticketID and
				* cache the connection for re-use. The ticket is set for infinite usage and with infinite
				* usage count.
				*
				* @param userID UserID used in data grid or digital library
				* @param organizationID The organization ID (e.g) SDSC
				* @param homeDirectory User's home directory
						* @param defaultStorageRes User's default starge resrouce (refer SRB documents)
						* @param password The password used int he challenge response protocol for SRB
				*
				*/
		public PrepareTicketParams(String userID, String organizationID,
					String homeDirectory, String defaultStorageRes, String password, String srbServerUrl, int port){
						this(new UserInfo(userID, organizationID, homeDirectory,
								defaultStorageRes, password), new VirtualOrganizationInfo( srbServerUrl ,port));
						super.setUsageCount(0);
			}


			/**
				* Creates a PrepareTicketParams object with the required
				* parentUser parameter.
				*
				* Warning:
				* @param parentUser The parent user.
				*/
			public PrepareTicketParams(UserInfo parentUser){
						super.setParentUser(parentUser);
			}

			public PrepareTicketParams(UserInfo parentUser, VirtualOrganizationInfo org){
						super.setParentUser(parentUser);
						super.setGridInfo(org);
			}

			public PrepareTicketParams(UserInfo parentUser, String 	srbServer, int port){
						this (parentUser, new VirtualOrganizationInfo( srbServer ,port));
			}

			public PrepareTicketParams(String ticketID){
						super.setParentUser(new UserInfo(ticketID));
						super.setUsageCount(0);
			}

			public void setUsageCount(int usage){
						super.setUsageCount(usage);
			}

}