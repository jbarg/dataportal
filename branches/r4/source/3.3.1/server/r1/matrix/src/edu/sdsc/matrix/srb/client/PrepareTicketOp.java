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


/* Developer Log
	* PrepareTicketOp.java
	*
	* Created on July 9, 2003, 3:43 AM
	*/
package edu.sdsc.matrix.srb.client;

/**
	* Any one who has a Matrix ticketId can accesss Matrix and do operations as
	* though he was the real user who creatred the ticekeId.
	*
	* Ticket Ids could also be used to cache a connection for reuse. Every time a ticket
	* is created, a connection is made to the SRB server and the socket is kept open to
	* handle further requests. The next requests need waste time by opening a new
	* socket.  Care must be taken by the developers to close (expire) all the tickets, else
	* lot of open sockets might delay the SRB servers.
	*
	* A PrepareTicketOp object can use either StdParams or FlexParams,
	* but not both.
	* @author  Allen Ding - SDSC
	*/
public class PrepareTicketOp extends edu.sdsc.matrix.srb.parser.impl.
			PrepareTicketOpImpl {
//		protected PrepareTicketOp(edu.sdsc.matrix.srb.parser.PrepareTicketOp pto){
//		super.setFlexParams(pto.getFlexParams());
//		this.setStdParams(pto.getFlexParams());
//		}

	/**
		* Creates a new PrepareTicketOp using a UserInfo to identify the
		* parent/provider.
		*/
	public PrepareTicketOp(UserInfo ui) {
		super.setStdParams(new PrepareTicketParams(ui));
	}

	/**
		* Creates a new PrepareTicketOp object using PrepareTicketParams.
		* @param stdParams PrepareTicket Parameters.
		*/
	public PrepareTicketOp(PrepareTicketParams stdParams) {
		super.setStdParams(stdParams);
	}

	/**
		* Creates a new PrepareTicketOp object using flexParams.
		* @param flexParams Flex Parameters.
		*/
	public PrepareTicketOp(ParamList flexParams) {
		super.setFlexParams(flexParams);
	}

	/**
		* Makes the PrepareTicketOp use the specified PrepareTicketParams. Using this
		* method clobbers any flexParams paremeter.
		* @param stdParams A PrepareTicketParams.
		*/
	public void useStdParams(PrepareTicketParams stdParams) {
		super.setFlexParams(null);
		super.setStdParams(stdParams);
	}

	/**
		* Makes the PrepareTicketOp use the specified ParamList. Using this
		* method clobbers any stdParams parameter.
		* @param flexParams A ParamList.
		*/
	public void useFlexParams(ParamList flexParams) {
		super.setStdParams(null);
		super.setFlexParams(flexParams);
	}

	public void setUsageCount(int usage) throws MatrixClientException{
				if (this.getStdParams() != null){
							this.getStdParams().setUsageCount(usage);
				} else{
							throw new MatrixClientException(
									"Cannot update a stdParameter as FlexParms are used");
				}
	}
}