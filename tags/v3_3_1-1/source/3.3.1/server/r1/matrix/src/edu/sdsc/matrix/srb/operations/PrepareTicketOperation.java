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

package edu.sdsc.matrix.srb.operations;

import edu.sdsc.matrix.srb.core.MatrixConnection;
import edu.sdsc.matrix.srb.core.MatrixConnectionArchive;
import edu.sdsc.matrix.srb.core.flow.FlowThread;
import edu.sdsc.matrix.srb.parser.GridTicketInfo;
import edu.sdsc.matrix.srb.parser.OpResult;
import edu.sdsc.matrix.srb.parser.PrepareTicketOp;
import edu.sdsc.matrix.srb.parser.PrepareTicketParams;
import edu.sdsc.matrix.srb.parser.PrepareTicketResult;
import edu.sdsc.matrix.srb.parser.Step;
import edu.sdsc.matrix.srb.parser.TicketID;
import edu.sdsc.matrix.srb.parser.UserInfo;
import edu.sdsc.matrix.srb.parser.VirtualOrganizationInfo;
import edu.sdsc.matrix.srb.util.MatrixUtil;
import edu.sdsc.matrix.srb.core.MatrixAccount;
import edu.sdsc.matrix.srb.core.GridConnectionFactory;
import edu.sdsc.matrix.srb.core.GridConnection;
import edu.sdsc.matrix.srb.core.MatrixException;

/**
	* This class is responsible for creating tickets that can be
	* used by the Matrix Users
	*
	* @author rmathew
	*
	*/
public class PrepareTicketOperation extends MatrixAgent{
				public PrepareTicketOperation(Step step, FlowThread parent,
						ThreadGroup group){
								super(step, parent, group);
				}

				/**
					* The method that executes the steps for preparing a ticket
					*/
				public void typeSpecificExecute() throws Exception{
							String ticketID = null;
							int reqUsage = 0;
							PrepareTicketOp prepTicketOprn = getOperation().getPrepareTicketOp();
							PrepareTicketParams prepTicketParams = prepTicketOprn.getStdParams();
							UserInfo ticketUser = prepTicketParams.getParentUser();
							VirtualOrganizationInfo voInfo = prepTicketParams.getGridInfo();
							PrepareTicketResult ticketResult = getObjectFactory().createPrepareTicketResult();
							reqUsage = prepTicketParams.getUsageCount();

							if (reqUsage < 0){ //request to remove a ticket
										if (ticketUser.getGridTicket() == null){
													// User is trying to setUsageCount < 0 for a user (not a ticket)
													// probably an accidental entry in the usage count
													toLog(
															"PrepareTicketOperation() -Invalid arguments",
															MatrixUtil.FATAL_LOG);
													throw new MatrixException(
															"Only matrix tickets can be set to no usage (usageCount less than 0)");
										} else {
																GridTicketInfo deleteTicket = ticketUser.getGridTicket();
																TicketID delTID = deleteTicket.getTicketID();
																//TicketID.TicketStringType ticketStringType = delTID.getTicketString();
																//assign the deleted Ticket to deleteTicket
																//deleteTicket = (GridTicketInfo) getStore().removeTicket(ticketStringType.getValue());
																deleteTicket = (GridTicketInfo) getStore().removeTicket(delTID.getTicketString());
																if (deleteTicket == null){
																			//MatrixConnectionArchive.removeConnection(ticketStringType.getValue());
																			MatrixConnectionArchive.removeConnection(delTID.getTicketString());
																			// the above line is a just in case - it wont be useful as there is no valid ticket
																			throw new MatrixException("Invalid Ticket");
																}else{ // now remove the live connection
																				MatrixConnectionArchive.removeConnection(delTID.getTicketString());
																}
										}
							} else{ // request to prepare a ticket
										toLog("PrepareTicketOperation- run() -USED FROM CONNECTION",
												MatrixUtil.DEBUG_LOG);
										ticketID = generateTicketId();
										MatrixConnection connForTicket = createConnection(ticketUser, voInfo,
												ticketID, "SRB");
										if (connForTicket != null){

													//	Return ticket to user
													GridTicketInfo gridTicket = getObjectFactory().createGridTicketInfo();
													gridTicket.setTicketParent(ticketUser);
													gridTicket.setStartTime(prepTicketParams.getStartTime());
													gridTicket.setEndTime(prepTicketParams.getEndTime());
													gridTicket.setUsageCount(reqUsage);

													// Set Ticket String
													TicketID srbTicketID = getObjectFactory().createTicketID();
													//TicketID.TicketStringType ticketStringType = getObjectFactory().
													//createTicketIDTicketStringType();
													//ticketStringType.setValue(ticketID);

													//srbTicketID.setTicketString(ticketStringType);
													srbTicketID.setTicketString(ticketID);
													gridTicket.setTicketID(srbTicketID);
													ticketResult.setGridTicket(gridTicket);
													toLog("PrepareTicketOperation() - run() -gridTicket:" + gridTicket,
															MatrixUtil.DEBUG_LOG);

													//Place ticket in Ticket Store with TicketId as key
													getStore().saveTicket(ticketID, gridTicket);
													// store connection in archive
													MatrixConnectionArchive.saveConnection(ticketID, connForTicket);
													toLog("PrepareTicketOperation() - run() -saveTicketInStore",	MatrixUtil.DEBUG_LOG);
													OpResult opResult = getObjectFactory().createOpResult();
													opResult.setPrepareTicketResult(ticketResult);
													setResponse(opResult);
										} else{
													toLog(
															"PrepareTicketOperation() - run() - unable to create connection",
															MatrixUtil.FATAL_LOG);
													throw new MatrixException(
															"Unable to create connection - Probably Invalid Login.");
										}
							}
				}

				/*
					*                             	Generate Ticket Id
					*/
				private String generateTicketId() throws Exception{
								//Let the current time be the ticket id
								String ticketId = (new Long(System.currentTimeMillis())).toString();
								return (ticketId);
				}

				public MatrixConnection createConnection(UserInfo uInfo,
						VirtualOrganizationInfo voInfo,
						String ticketId,
						String gridType){
								MatrixUtil.toLog("RequestManager-createConnectionForMatrixAgent-BEGIN",
										MatrixUtil.DEBUG_LOG);
								MatrixConnection matrixCxn = new MatrixConnection();
								MatrixAccount matrixAcct = new MatrixAccount(uInfo, voInfo);
								GridConnection gridXn = GridConnectionFactory.createConnection(gridType,
										uInfo, voInfo);
								matrixCxn.setTicketId(ticketId);
								matrixCxn.setMatrixAccount(matrixAcct);
								matrixCxn.setGridConnection(gridXn);
								MatrixUtil.toLog("RequestManager-createConnectionForMatrixAgent-END",
										MatrixUtil.DEBUG_LOG);
								return (matrixCxn);
				}

				/**
					* Get Operation Name
					* @returns the name of the operation encapsulated by this agent
					*/
				public String getOperationName(){
								return "PrepareTicketOperation";
				}
}