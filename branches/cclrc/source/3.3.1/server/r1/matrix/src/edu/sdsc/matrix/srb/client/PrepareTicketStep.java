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



/* Developer Log : PrepareTicketStep.java
	* Added more constructors Dec 7, 2004
	* Created on December 03,2004
	*/

package edu.sdsc.matrix.srb.client;

/**
	* <p>
	* Description: Creates a Prepare Ticket Step in DGL. The PrepareTicketSteop is
	* useful to create a matrix session. The TicketID from this step would be valid
	* based on the characteristics requested (for "n" times or till some point in time etc)
	*
	* Any one who has a Matrix ticketId can accesss Matrix and do operations as
	* though he was the real user who creatred the ticekeId.
	*
	* Ticket Ids could also be used to cache a connection for reuse. Every time a ticket
	* is created, a connection is made to the SRB server and the socket is kept open to
	* handle further requests. The next requests need waste time by opening a new
	* socket.  Care must be taken by the developers to close (expire) all the tickets, else
	* lot of open sockets might delay the SRB servers.
	* </p>
	* <p>LGPL licence</p>
	* <p>Company: SDSC </p>
	* @author Arun swaran Jagatheesan (arun@sdsc.edu)
	* @version 1.0
	*/

public class PrepareTicketStep extends Step{

				public PrepareTicketStep(String stepID, String userID, String organizationID,
					String homeDirectory, String defaultStorageRes, String password, String srbServerUrl, int srbPort){
						this(stepID, new UserInfo(userID, organizationID,
								homeDirectory, defaultStorageRes, password), new VirtualOrganizationInfo(srbServerUrl,srbPort));

			}

			public PrepareTicketStep(String stepID, String ticketID, boolean cancelTicket) throws MatrixClientException{
						this(stepID, new PrepareTicketParams(ticketID));
						if (cancelTicket)	this.setUsageCount(-1);
			}

			public PrepareTicketStep(String stepID, String ticketID){
						this(stepID, new PrepareTicketParams(ticketID));
			}

			public PrepareTicketStep(String stepID, GridTicketInfo gTicket){
						this(stepID, gTicket, null);
			}

			public PrepareTicketStep(String stepID, GridTicketInfo gTicket, VirtualOrganizationInfo vio){
						this(stepID, new PrepareTicketParams( new UserInfo(gTicket), vio));
			}

			public PrepareTicketStep(String stepID, UserInfo uio, VirtualOrganizationInfo vio){
						this(stepID, new PrepareTicketParams(uio, vio));
			}


			public PrepareTicketStep(String id, PrepareTicketParams stdParams){
						super(id, new Operation(new PrepareTicketOp(stdParams)));
			}

			public PrepareTicketStep(String id, ParamList flexParams){
						super(id, new Operation(new PrepareTicketOp(flexParams)));
			}

			public void setUsageCount(int usage) throws MatrixClientException{
						try{
									this.getOperation().getPrepareTicketOp().getStdParams().setUsageCount(
											usage);
						} catch (NullPointerException npe){
									throw new MatrixClientException(
											"Either StdParameters not initialized or FlexParams used.",
											npe);
						}
			}
}