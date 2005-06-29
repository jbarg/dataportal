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



/* Developer Log: TicketID.java
	*
	* Licence anf format changed on Dec 3, 2004 - Arun
	* Created on July 2, 2003, 6:35 AM - Allen
	*/
package edu.sdsc.matrix.srb.client;

//import edu.sdsc.matrix.srb.parser.*;

/**
	* Part of the Matrix client API.
	* Contains methods to create TicketID objects.
	* A TicketID object may be created with either a ticketString or ticketID,
	* but they are exclusive.
	* @author  Allen Ding - SDSC
	*/
public class TicketID extends edu.sdsc.matrix.srb.parser.impl.TicketIDImpl{
			/**
				* Internal method used to wrap a parser interface TicketID object into this
						* class' representation. This method should not normally be used by the user.
				* @param tid The parser.TicketID to wrap.
				*/
			protected TicketID(edu.sdsc.matrix.srb.parser.TicketID tid){
						super.setTicketString(tid.getTicketString());
						super.setTicketType(tid.getTicketType());
			}

			/**
				* Creates a TicketID object using the specified ticketString.
				* @param ticketString The string representing the ticket.
				*/
			public TicketID(String myTicketStr){
					this(myTicketStr, "MATRIX");
			}

			public TicketID(String myTicketStr, String myTicType){
					super.setTicketString(myTicketStr);
					super.setTicketType(myTicType);
			}
}