/* ====================================================================
 * SDSC Matrix Software License, Version 1.0
 *
 * Copyright (c) 2003 San Diego Supercomputer Center (SDSC), San Diego, USA.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that ALL the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The end-user documentation included with the redistribution,
 *    if any, must include the following acknowledgment:
 *       "This product includes software developed by the
 *        SDSC Matrix Project (http://www.npaci.edu/DICE/SRB/matrix/)."
 *    Alternately, this acknowledgment may appear in the software itself,
 *    if and wherever such third-party acknowledgments normally appear.
 *
 * 4. The names "SDSC" and "SDSC Matrix" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact arun@sdsc.edu or srb-chat@sdsc.edu.
 *
 * 5. Products derived from this software may not be called "SDSC",
 *    nor may "SDSC" appear in their name, without prior written
 *    permission of the SDSC.
 *
 * 6. License is not granted for commercial resale, in whole or in part, 
 *	  without the prior written permission from SDSC.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL SDSC OR
 * ITS CONTRIBUTORS (SDSC Matrix Team) BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * ====================================================================
 *
 * This software is intended to consist of voluntary contributions made by many
 * individuals for the SDSC Matrix Project.
 *
 * SDSC Matrix.  
 * Designer & Architect: Arun swaran Jagatheesan (arun@sdsc.edu)
 *
 * Authors (Credits 02/SEP/2003):
 * Allen Ding (alding@sdsc.edu)
 * Arun Jagatheesan (arun@sdsc.edu)
 * Reena Mathew	(rmathew@sdsc.edu)
 * Erik Vandekieft (evk@sdsc.edu)
 */



package edu.sdsc.matrix.srb.operations;


import edu.sdsc.grid.io.srb.SRBException;
import edu.sdsc.grid.io.srb.SRBAccount;

import java.util.Calendar;

import edu.sdsc.matrix.srb.parser.*;
import edu.sdsc.matrix.srb.core.SRBConnection;
import edu.sdsc.matrix.srb.core.MatrixConnection;
import edu.sdsc.matrix.srb.core.MatrixConnectionPool;

import edu.sdsc.matrix.srb.util.MatrixUtil;

/**
 * This class is responsible for creating tickets that can be 
 * used by the Matrix Users
 *  
 * @author rmathew
 * 
 */
public class PrepareTicketOperation extends MatrixAgent
{
	boolean stepAbort = false;
	
	/**
	 * The method that executes the steps for preparing a ticket
	 */
	public void run()
	{
		try
		{
			toLog("PrepareTicketOperation() - run() -BEGIN",MatrixUtil.DEBUG_LOG);
			stepAbort = checkStepAbortFlag();
			if(!stepAbort)
			{			
				String ticketID = null;
				logStepBeginStatus();
						
				PrepareTicketOp prepTicketOprn = getOperation().getPrepareTicketOp();
				PrepareTicketParams prepTicketParams = prepTicketOprn.getStdParams();
				UserInfo ticketUser = prepTicketParams.getParentUser();
				VirtualOrganizationInfo voInfo = 	prepTicketParams.getGridInfo();		
				
				// Implementation for Operation
				
				//SRBAccount account = srbCreateSRBAccount();
				//SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
				//SRBAccount account = srbConnection.getSRBAccount();				
				toLog("PrepareTicketOperation- run() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
				ticketID = generateTicketId();
				MatrixConnection connForTicket = getMatrixTransactionHandler().createConnectionForMatrixAgent(ticketUser,voInfo,ticketID,"SRB");
								
				if(connForTicket!=null)
				{
					//	Return ticket to user
						 PrepareTicketResult ticketResult = getObjectFactory().createPrepareTicketResult();
						 GridTicketInfo gridTicket = getObjectFactory().createGridTicketInfo();
						 gridTicket.setTicketParent(ticketUser);
						 gridTicket.setStartTime(prepTicketParams.getStartTime());
					     gridTicket.setEndTime(prepTicketParams.getEndTime());
		
						 // Set Ticket String
						 TicketID srbTicketID = getObjectFactory().createTicketID();
						 TicketID.TicketStringType ticketStringType = getObjectFactory().createTicketIDTicketStringType();
						 ticketStringType.setValue(ticketID);
						 srbTicketID.setTicketString(ticketStringType);
		
						 gridTicket.setTicketID(srbTicketID);
					
						 ticketResult.setGridTicket(gridTicket);
						 toLog("PrepareTicketOperation() - run() -gridTicket:"+gridTicket,MatrixUtil.DEBUG_LOG);
		
		
						 //Place ticket in Ticket Store with TicketId as key
						 getSRBStore().saveTicket(ticketID,gridTicket);
						 MatrixConnectionPool.saveConnection(ticketID,connForTicket);
						 toLog("PrepareTicketOperation() - run() -saveTicketInStore",MatrixUtil.DEBUG_LOG);				
		
						 OpResult opResult = getObjectFactory().createOpResult();
						 opResult.setPrepareTicketResult(ticketResult);
						 setSRBResponse(opResult);							
				}	
				else
				{
					throw new Exception("Cannot create SRB account for the user.");
				}	
				
				toLog("PrepareTicketOperation() - run() -ticketID:"+ticketID,MatrixUtil.DEBUG_LOG);		
				
				
				
			}
			else
			{
				logStepAbortStatus();
			}	
			toLog("PrepareTicketOperation() - run() -END",MatrixUtil.DEBUG_LOG);		
		
		}
		catch(SRBException e)
		{
			String exceptionMessage = SRBException.getStandardMessage(e.getType());
			setSRBException(new Exception(exceptionMessage));	
		}
		catch(Exception e)
		{
			setSRBException(e);
		}
		finally
		{
			if(!stepAbort)
				logStepEndStatus();
		}
	}	
	
	/*
	 *	Generate Ticket Id
	 */	
	private String generateTicketId() throws Exception
	{
		//Let the current time be the ticket id
		String ticketId =(new Long(System.currentTimeMillis())).toString();
		return(ticketId);
	}	
	
}