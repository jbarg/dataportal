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

import edu.sdsc.matrix.srb.parser.*;
import edu.sdsc.matrix.srb.core.SRBStore;
import edu.sdsc.matrix.srb.core.MatrixCodes;

import edu.sdsc.matrix.srb.core.MatrixConnection;
import edu.sdsc.matrix.srb.core.MatrixAccount;
import edu.sdsc.matrix.srb.core.GridConnection;
import edu.sdsc.matrix.srb.core.GridConnectionFactory;
import edu.sdsc.matrix.srb.core.MatrixConnectionPool;

import edu.sdsc.matrix.srb.util.MatrixUtil;

import java.util.List;
import java.util.ListIterator;
import java.util.Iterator;
import java.util.Calendar;
import java.util.Date;

/**
 * This class is responsible for managing a Matrix Transaction
 * which consists of multiple flow which in turn contain multiple 
 * steps/operations. 
 *  
 * @author rmathew
 * 
 */

public class MatrixTransactionHandler extends Thread
{
	/**
	 * Input Data Grid Request 
	 */
	private DataGridRequest srbRequestObject;
	
	/**
	 * List of Attachments to be used by this transaction
	 */
	private Iterator attachmentItr; 
	
	/**
	 * Transaction ID
	 */
	private String transactionId;
	
	/**
	 * Reference to Matrix Store to retrieve and store objects
	 */
	private SRBStore srbStore;
	
	/**
	 * Reference to JAXB Object Factory used for creating JAXB Response Objects
	 */
	private ObjectFactory objFactory;

	/**
	 * Logging Level
	 * NOT USED
	 *  Have to make lower filter level to see debug messages
	 */
	public static final int filterLevel = 0;
	
	/**
	 * Notes the current flow in execution
	 */
	private String currentFlowId = null;
	
	/**
	 * Tracks the number of threads complete for a flow
	 */ 
	private int currentFlowStepCompleteCount=0;
	
	/**
	 * Maintains the number of agents for current flow
	 */ 
	private int currentFlowStepCount=0;
	
	/**
	 * Tracks the number of flows complete for a transaction
	 */ 
	private int currentTransactionFlowCompleteCount =0;
	
	/**
	 * Maintains the number of flows for transaction
	 */
	private int currentTransactionFlowCount = 0;
	
	/**
	 * Ticket ID used by Transaction
	 */
	private String ticketIdForTransaction;
	
	/**
	 * Flag to indicate whether a flow execution is in progress
	 */
	private boolean flowInProgress = false;
	
	/**
	 * Flag to check if ticket is a temporary one or now
	 */
	private boolean isTempTicketForTransaction = false;
	
	/**
	 * Status Code for the current flow
	 */
	private int currentFlowStatusCode;
	
	/**
	 * Status Description for the current flow
	 */
	private String currentFlowStatusDescription;
	
	/**
	 * Status Code for transaction
	 */
	private int transactionStatusCode;
	
	/**
	 * Status Description for Transaction
	 */
	private String transactionStatusDescription;
	
	/**
	 * Temporary String for operation name of Matrix Agent
	 */
	private String currentOperationNameForMatrixAgentBuilt;
	
	/** 
	 * Default Constructor
	 */
	public MatrixTransactionHandler()
	{
		
	}

	/**
	 * Public Constructor
	 * @param srbRequest Input Datagrid Request
	 * @param txnId Transaction ID
	 * @param store Reference to Matrix Store
	 */
	public MatrixTransactionHandler(DataGridRequest srbRequest,String txnId,SRBStore store)
	{
		srbRequestObject = srbRequest;
		transactionId = txnId;
		srbStore = store;
	}

	/**
   	 * Set SRB Request Object
   	 * @param obj Datagrid Request Object
	 */  
	public void setSRBRequestObject(DataGridRequest obj)
	{
		srbRequestObject = obj;
	}

	/**
	 * Get SRB Request Object
	 * @return Data Grid Request
	 */  
	public DataGridRequest getSRBRequestObject()
	{
		return(srbRequestObject);
	}
	
	/**
	 * Set SRB Store
	 * @param obj Reference to Matrix Store
	 */  
	public void setSRBStore(SRBStore obj)
	{
		srbStore = obj;
	}

	/**
	 * Get SRB Store
	 * @return Reference to Matrix Store
	 */  
	public SRBStore getSRBStore()
	{
		return(srbStore);
	}
	
	/**
	 * Set Transaction ID
	 * @param obj Transaction ID
	 */  
	public void setTransactionId(String obj)
	{
		transactionId = obj;
	}

	/**
	 * Get Transaction ID
	 * @return Transaction ID
	 */  
	public String getTransactionId()
	{
		return(transactionId);
	}
	
	/**
	 * Set Attachment Iterator
	 * @param obj List of Input Attachments to be used by the Transaction
	 */  
	public void setAttachmentIterator(Iterator obj)
	{
		attachmentItr = obj;
	}

	/**
	 * Get Attachment Iterator
	 * @return List of Input Attachments
	 */  
	public Iterator getAttachmentIterator()
	{
		return(attachmentItr);
	}
	
	/**
	 * Set Ticket Id For Transaction
	 * @param tktId Ticket Id used by Transaction
	 */  
	public void setTicketIdForTransaction(String tktId)
	{
		ticketIdForTransaction = tktId;
	}

	/**
	 * Get Ticket Id For Transaction
	 * @return Ticket ID for transaction
	 */  
	public String getTicketIdForTransaction()
	{
		return(ticketIdForTransaction);
	}
	
	//	Arun change start
	 /**
	  * To log method by Arun
	  */		
	 public void toLog(String msg, int errLevel){
		MatrixUtil.toLog(getTransactionId(),msg,errLevel);	 
	 }
	
	 // End Arun change
	 
	 /**
	  * Check if Transaction to be aborted
	  * @param txnId Transaction ID
	  * @return Flag for Transaction Abort Status
	  */
	 private boolean checkTransactionAbortFlag(String txnId)
	 {
		toLog("MatrixTransactionHandler-checkTransactionAbortFlag-BEGIN", MatrixUtil.DEBUG_LOG);
	 	boolean abort = false;
	 	TransactionStatusResponse txnStatus =  (TransactionStatusResponse)
	 											srbStore.getTransactionStatus(txnId);
	 	if(txnStatus!=null)
			abort=txnStatus.isAbortRequested();
		toLog("MatrixTransactionHandler-checkTransactionAbortFlag-END", MatrixUtil.DEBUG_LOG);									
		return(abort); 
	 }
	 
	
	/**
	 * Check if Flow to be aborted
	 * @param txnId Transaction ID
	 * @param flowId Flow ID
	 * @return Flag for Flow Abort Status
	 */ 
	private boolean checkFlowAbortFlag(String txnId,String flowId)
		{
		   toLog("MatrixTransactionHandler-checkFlowAbortFlag-BEGIN", MatrixUtil.DEBUG_LOG);
		   boolean abort = false;
		   FlowStatusResponse flowStatus =  (FlowStatusResponse)
												   srbStore.getMatrixFlowStatus(txnId,flowId);
		   if(flowStatus!=null)
			   abort=flowStatus.isAbortRequested();
		   toLog("MatrixTransactionHandler-checkFlowAbortFlag-END", MatrixUtil.DEBUG_LOG);									
		   return(abort); 
		}
	 
	 
	 /**
	  * Log Status for Transaction Start
	  */
	 private void logTransactionBeginStatus() throws Exception
	 {
		toLog("MatrixTransactionHandler-logTransactionBeginStatus-BEGIN", MatrixUtil.DEBUG_LOG);
		TransactionStatusResponse txnStatus =  (TransactionStatusResponse)
																			srbStore.getTransactionStatus(transactionId);
		if(txnStatus==null)
		{
			objFactory = new ObjectFactory();
			txnStatus = objFactory.createTransactionStatusResponse();
		}											
		txnStatus.setStartTime(Calendar.getInstance());
		txnStatus.setStatusCode(transactionStatusCode);
		txnStatus.setTransactionID(transactionId);
		srbStore.setTransactionStatus(transactionId,txnStatus);
		toLog("MatrixTransactionHandler-logTransactionBeginStatus-END", MatrixUtil.DEBUG_LOG);
	 }
	
	/**
	 * Log Status for Transaction End
	 */ 
	private void logTransactionEndStatus() throws Exception
	{
		toLog("MatrixTransactionHandler-logTransactionEndStatus-BEGIN", MatrixUtil.DEBUG_LOG);	
	   TransactionStatusResponse txnStatus =  (TransactionStatusResponse)
																		   srbStore.getTransactionStatus(transactionId);
	  									
	   txnStatus.setEndTime(Calendar.getInstance());
	   txnStatus.setStatusCode(transactionStatusCode);
	   	  
	   srbStore.setTransactionStatus(transactionId,txnStatus);
	   toLog("MatrixTransactionHandler-logTransactionEndStatus-END", MatrixUtil.DEBUG_LOG);
	}
	
	/**
	 * Log Status for Transaction Abort
	 */
	private void logTransactionAbortStatus() throws Exception
	{
		toLog("MatrixTransactionHandler-logTransactionAbortStatus-BEGIN", MatrixUtil.DEBUG_LOG);
		TransactionStatusResponse txnStatus =  (TransactionStatusResponse)
													 srbStore.getTransactionStatus(transactionId);
		if(txnStatus==null)
		{
			objFactory = new ObjectFactory();
			txnStatus = objFactory.createTransactionStatusResponse();
		}
		
		if(txnStatus.getStartTime()==null)	
			txnStatus.setStartTime(Calendar.getInstance());
		txnStatus.setTransactionID(transactionId);
		txnStatus.setEndTime(Calendar.getInstance());
		txnStatus.setStatusCode(transactionStatusCode);
		srbStore.setTransactionStatus(transactionId,txnStatus);	
		toLog("MatrixTransactionHandler-logTransactionAbortStatus-END",MatrixUtil.DEBUG_LOG);
	}
	
	
	/**
	 * Log Status for Flow Start
	 */
	private void logFlowBeginStatus() throws Exception
		{
			toLog("MatrixTransactionHandler-logFlowBeginStatus-BEGIN", MatrixUtil.DEBUG_LOG);
		   FlowStatusResponse flowStatus =  (FlowStatusResponse)srbStore.getMatrixFlowStatus(transactionId,currentFlowId);
		   if(flowStatus==null)
		   {
			   objFactory = new ObjectFactory();
			flowStatus = objFactory.createFlowStatusResponse();
		   }											
		   flowStatus.setStartTime(Calendar.getInstance());
		   flowStatus.setStatusCode(MatrixCodes.FLOW_STARTED);
		   flowStatus.setFlowID(currentFlowId);
		   srbStore.setMatrixFlowStatus(transactionId,currentFlowId,flowStatus);
		   toLog("MatrixTransactionHandler-logFlowBeginStatus-END", MatrixUtil.DEBUG_LOG);
		}
	 
		/**
		 * Log Status for Flow End
		 */
	   private void logFlowEndStatus() throws Exception
	   {
		toLog("MatrixTransactionHandler-logFlowEndStatus-BEGIN", MatrixUtil.DEBUG_LOG);
			FlowStatusResponse flowStatus =  (FlowStatusResponse)srbStore.getMatrixFlowStatus(transactionId,currentFlowId);
			if(flowStatus==null)
			{
				objFactory = new ObjectFactory();
			 flowStatus = objFactory.createFlowStatusResponse();
			}											
			flowStatus.setEndTime(Calendar.getInstance());
			flowStatus.setStatusCode(MatrixCodes.FLOW_COMPLETE);
			flowStatus.setFlowID(currentFlowId);
			srbStore.setMatrixFlowStatus(transactionId,currentFlowId,flowStatus);
		toLog("MatrixTransactionHandler-logFlowEndStatus-END", MatrixUtil.DEBUG_LOG);
	   }
	
		/**
		 * Log Status for Flow Abort
		 */
	   private void logFlowAbortStatus() throws Exception
	   {
		toLog("MatrixTransactionHandler-logFlowAbortStatus-BEGIN", MatrixUtil.DEBUG_LOG);
			FlowStatusResponse flowStatus =  (FlowStatusResponse)srbStore.getMatrixFlowStatus(transactionId,currentFlowId);
		   if(flowStatus==null)
		   {
			   objFactory = new ObjectFactory();
			flowStatus = objFactory.createFlowStatusResponse();
		   }				
		   if(flowStatus.getStartTime()==null)							
		   	flowStatus.setStartTime(Calendar.getInstance());
		   flowStatus.setEndTime(Calendar.getInstance());
		   flowStatus.setStatusCode(MatrixCodes.FLOW_ABORTED_DUE_TO_REQUEST_FROM_USER);
		   flowStatus.setFlowID(currentFlowId);
		   srbStore.setMatrixFlowStatus(transactionId,currentFlowId,flowStatus);
		toLog("MatrixTransactionHandler-logFlowAbortStatus-END", MatrixUtil.DEBUG_LOG);
	   }
	   
	 
	/**
	 * Check is ticket is valid
	 * @param ticket Ticket to be checked
	 * @return Flag that indicate if ticket is valid or not
	 */  
	private boolean isTicketValid(GridTicketInfo ticket)
	{
		   toLog("MatrixTransactionHandler-isTicketValid-BEGIN", MatrixUtil.DEBUG_LOG);
		   Calendar currentTime = Calendar.getInstance();
		   Calendar ticketExpiryTime = ticket.getEndTime();
		   Date currentDate = currentTime.getTime();
		   Date ticketExpiryDate = ticketExpiryTime.getTime();
		   toLog("MatrixTransactionHandler-isTicketValid-valid-Curr before Expiry:"+currentDate.before(ticketExpiryDate), 5);
		   toLog("MatrixTransactionHandler-isTicketValid-END", MatrixUtil.DEBUG_LOG);
		   return(currentDate.before(ticketExpiryDate));
		
		   
	}

	// Added by Reena Mathew on 20 June 2003
	/**
	 * Retrieve Connection to be used by all Matrix Agents started by this transaction
	 * @return Matrix Connection to be used by all Matrix Agents started by this transaction
	 */
	private MatrixConnection getConnectionForMatrixAgents()
	{
		toLog("MatrixTransactionHandler-getConnectionForMatrixAgents-BEGIN", MatrixUtil.DEBUG_LOG);
		// ************************************************
		UserInfo gridUser = srbRequestObject.getGridUser();
		GridTicketInfo gridTicket = gridUser.getGridTicket();
		VirtualOrganizationInfo voInfo = srbRequestObject.getVOInfo();
		MatrixConnection matrixConnection =null;
		String gridType = "SRB"; // Has to be set in Schema or some other way
		if(gridTicket!=null)
		{
			toLog("MatrixTransactionHandler-getConnectionForMatrixAgents-TICKET AVAILABLE", MatrixUtil.DEBUG_LOG);
			String ticketId = null;
			TicketID ticketIdObject =  gridTicket.getTicketID();
			TicketID.TicketStringType ticketString = ticketIdObject.getTicketString();
			ticketId = ticketString.getValue();
			toLog("MatrixTransactionHandler-getConnectionForMatrixAgents-ticketId:"+ticketId, MatrixUtil.DEBUG_LOG);
			
			// Check for Ticket in Store
			GridTicketInfo gridTicketFromStore = (GridTicketInfo)getSRBStore().retrieveTicket(ticketId);
			if(gridTicketFromStore!=null)
			{
				if(isTicketValid(gridTicketFromStore))
				{
					// Check if Connection in Connection Pool
					matrixConnection = (MatrixConnection)MatrixConnectionPool.retrieveConnection(ticketId);
					
					// If not Create new Connection
					if(matrixConnection==null)
					{
						toLog("MatrixTransactionHandler-getConnectionForMatrixAgents-NO CONNECTION IN POOL", MatrixUtil.DEBUG_LOG);
						//Create Connection
						matrixConnection = createConnectionForMatrixAgent(gridUser,voInfo,ticketId,gridType);
					
						// Store Connection in Connection Pool
						MatrixConnectionPool.saveConnection(ticketId,matrixConnection);
					}
					setTicketIdForTransaction(ticketId);
					isTempTicketForTransaction = false;
				}
				else
				{
					toLog("MatrixTransactionHandler-getConnectionForMatrixAgents-TICKET INVALID:"+ticketId, MatrixUtil.DEBUG_LOG);	
				}	
			}
			else
			{
				toLog("MatrixTransactionHandler-getConnectionForMatrixAgents-TICKET NOT IN STORE:"+ticketId, MatrixUtil.DEBUG_LOG);	
			}
		}
		else
		{
			// Create temp Ticket Id
			String tempTicketID = MatrixUtil.generateUniqueTicketId();
			toLog("MatrixTransactionHandler-getConnectionForMatrixAgents-tempTicketID:"+tempTicketID, MatrixUtil.DEBUG_LOG);
			
			// Create Temp Connection
			matrixConnection = createConnectionForMatrixAgent(gridUser,voInfo,tempTicketID,gridType);
			
			// Store Connection in Connection Pool
			MatrixConnectionPool.saveConnection(tempTicketID,matrixConnection);
			setTicketIdForTransaction(tempTicketID);
			isTempTicketForTransaction = true;
		}
		
		
		toLog("MatrixTransactionHandler-getConnectionForMatrixAgents-END", MatrixUtil.DEBUG_LOG);
		return(matrixConnection);
	}
	
	/**
	 * Create Connection to be used by transaction
	 * @param uInfo User Information
	 * @param voInfo Virtual Organization Information
	 * @param ticketId Ticket ID
	 * @param gridType Grid Type Ex. SRB
	 * @return Grid Connection to be used by transaction
	 */
	public MatrixConnection createConnectionForMatrixAgent(UserInfo uInfo,
															VirtualOrganizationInfo voInfo,
															String ticketId,
															String gridType
															
															)
	{
		toLog("MatrixTransactionHandler-createConnectionForMatrixAgent-BEGIN", MatrixUtil.DEBUG_LOG);
		MatrixConnection matrixCxn = new MatrixConnection();
		MatrixAccount matrixAcct = new MatrixAccount(uInfo,voInfo);	
		GridConnection gridXn = GridConnectionFactory.createConnection(gridType,uInfo,voInfo);
		matrixCxn.setTicketId(ticketId);
		matrixCxn.setMatrixAccount(matrixAcct);
		matrixCxn.setGridConnection(gridXn);
		toLog("MatrixTransactionHandler-createConnectionForMatrixAgent-END", MatrixUtil.DEBUG_LOG);
		return(matrixCxn);
															
	}
	
	

	/**
	  *	Executes the transaction for the SRB Request
	  */
	public void run()
	{	
		toLog("MatrixTransactionHandler-run-BEGIN", MatrixUtil.DEBUG_LOG);
		
		boolean txnAbort = false;	
		try
		{	
			txnAbort = checkTransactionAbortFlag(transactionId);
			if(!txnAbort)
			{
				transactionStatusCode = MatrixCodes.TRANSACTION_STARTED;
				logTransactionBeginStatus();
				int opCount =0; 					// Number of Operations for the transaction
				String currentOperationName = null;		// Name of Current Operation
				MatrixAgent currentAgent = null;		// Thread for Current Operation
				MatrixAgent[] listOfSRBAgent = null; 	// List of Threads for all Operations of Transaction
				boolean isExecutionParallel = false;// Flag whether transaction operations are Parrallel or Sequential
				String currentAgentId = null;		// Id for each operation of the Transaction
				Operation currentOperation = null;
			
				// Process Flows
				edu.sdsc.matrix.srb.parser.Transaction srbTxn = srbRequestObject.getTransaction();
				List flowList = srbTxn.getFlow(); 
				ListIterator flowItr = flowList.listIterator();	
				currentTransactionFlowCount = flowList.size();
				
				// Get the Connection to be used by all Matrix Agents
				MatrixConnection matrixConnForAgent = getConnectionForMatrixAgents();
				if(matrixConnForAgent!=null)
				{
					//Log Transaction Flows intp Store
					String userId = getSRBUserID();
					String[]  listOfFlowIds =  new String[flowList.size()];
					for(int flowIndex=0;flowIndex<flowList.size();flowIndex++)
					{
						listOfFlowIds[flowIndex] = 	String.valueOf(flowIndex);			
					}	
					srbStore.setTransactionFlowIDs(transactionId,userId,listOfFlowIds);
					
					
					
					for(int flowIndex=0;flowIndex<flowList.size();flowIndex++)
					{
						// NOTE : Handle Matrix Flow will 
						//        return only after all steps of flow are complete
						handleMatrixFlow((Flow)flowItr.next(),String.valueOf(flowIndex),matrixConnForAgent);
						logFlowEndStatus();
					}
					transactionStatusCode = MatrixCodes.TRANSACTION_COMPLETE;
				}
				else
				{
					transactionStatusCode = MatrixCodes.TRANSACTION_ABORTED_DUE_TO_ERROR;
					toLog("MatrixTransactionHandler-run-Connection Not Available", MatrixUtil.DEBUG_LOG);
				}		
				cleanupTransaction();
				
				logTransactionEndStatus();			
				
			}
			else // Log Status For Response
			{
				transactionStatusCode = MatrixCodes.TRANSACTION_ABORTED_DUE_TO_REQUEST_FROM_USER;
				logTransactionAbortStatus();										
			}
			toLog("MatrixTransactionHandler-run-TRANSACTION "+getTransactionId()+" COMPLETE", MatrixUtil.DEBUG_LOG);
			toLog("MatrixTransactionHandler-run-END", MatrixUtil.DEBUG_LOG);
		}
		catch(Exception e)
		{
			transactionStatusCode = MatrixCodes.TRANSACTION_ABORTED_DUE_TO_ERROR;;
			System.out.println(e.toString());
			e.printStackTrace();
			try
			{
				logTransactionEndStatus();
			}
			catch(Exception e_1)
			{
				toLog("MatrixTransactionHandler-run-EXCEPTION while logging transaction End:"+e, MatrixUtil.ERROR_LOG);			
			}
		}
		
		//Arun
	}
	
	/**
	 * Cleans up any system objects after transaction is complete
	 */
	public void cleanupTransaction()
	{
		toLog("MatrixTransactionHandler-cleanupTransaction-BEGIN", MatrixUtil.DEBUG_LOG);
		
		// Cleanup if Temp Ticket Used
		if(isTempTicketForTransaction)
		{
			MatrixConnectionPool.removeConnection(getTicketIdForTransaction());
			toLog("MatrixTransactionHandler-cleanupTransaction-Connection Removed from POOL", MatrixUtil.DEBUG_LOG);
		}
		
		
		toLog("MatrixTransactionHandler-cleanupTransaction-END",MatrixUtil.DEBUG_LOG);
	}
	
	/**
	 * Updates Transaction Status<br>
	 * 	- Flow Count For Transaction<br>
	 * 	- Step Count for Flow<br>
	 * This method will be called by the MatrixAgents when they have completed execution
	 * @param flowId Flow Id of flow complete
	 * @param stepId Step Id of step complete
	 */
	public void updateMatrixTransactionStatusCount(String flowId,String stepId) throws Exception
	{
		toLog("MatrixTransactionHandler-updateMatrixTransactionStatusCount-BEGIN", MatrixUtil.DEBUG_LOG);
		currentFlowStepCompleteCount++;
		toLog("MatrixTransactionHandler-currentFlowStepCompleteCount:"+currentFlowStepCompleteCount, MatrixUtil.DEBUG_LOG);
		toLog("MatrixTransactionHandler-currentFlowStepCount:"+currentFlowStepCount, MatrixUtil.DEBUG_LOG);
		if(currentFlowStepCompleteCount==currentFlowStepCount) // Flow Complete
		{
			toLog("MatrixTransactionHandler-FLOW COMPLETE:"+flowId, MatrixUtil.DEBUG_LOG);
			flowInProgress = false;
			//logFlowEndStatus();
			currentTransactionFlowCompleteCount++;
			toLog("MatrixTransactionHandler-currentTransactionFlowCompleteCount:"+currentTransactionFlowCompleteCount, MatrixUtil.DEBUG_LOG);
			toLog("MatrixTransactionHandler-currentTransactionFlowCount:"+currentTransactionFlowCount, MatrixUtil.DEBUG_LOG);
			if(currentTransactionFlowCompleteCount==currentTransactionFlowCount) //Txn Complete
			{
				toLog("MatrixTransactionHandler-ALL FLOWS COMPLETE For TXN:"+getTransactionId(),MatrixUtil.DEBUG_LOG);		
			}
		}
		toLog("MatrixTransactionHandler-updateMatrixTransactionStatusCount-END", MatrixUtil.DEBUG_LOG);
	}

	/**
	  *	Executes the steps of the transaction flow in Parallel
	  * @param listOfAgent List of Matrix Agents to be executed for a flow 
	  */
	public void runParallel(MatrixAgent[] listOfAgent)
	{
		toLog("MatrixTransactionHandler-runParallel-BEGIN", MatrixUtil.DEBUG_LOG);
		for(int i=0;i<listOfAgent.length;i++)
		{
			listOfAgent[i].start();
		}
		toLog("MatrixTransactionHandler-runParallel-END", MatrixUtil.DEBUG_LOG);
	}

	/**
	  *	Executes the steps of the transaction flow in Sequential order
	  *
	  * @param listOfAgent List of Matrix Agents to be executed for a flow 
	  */
	public void runSequential(MatrixAgent[] listOfAgent)
	{
		toLog("MatrixTransactionHandler-runSequential-BEGIN", MatrixUtil.DEBUG_LOG);
		for(int i=0;i<listOfAgent.length;i++)
		{
			listOfAgent[i].run();
		}
		toLog("MatrixTransactionHandler-runSequential-END", MatrixUtil.DEBUG_LOG);
	}
	
	/**
	 * Return SRB User Id that initiated request
	 * @return SRB Used ID
	 */
	private String getSRBUserID()
	{
		// Extract parameters.....
		UserInfo srbUser = srbRequestObject.getGridUser();
		String ticketId = null;//srbUser.getTicketID(); // Have to update for new Ticket Code
		String userName = srbUser.getUserID();
		if(ticketId!=null)
			return(ticketId);
		else
			return(userName);	 
	}
	
	/**
	 * Create the MatrixAgent object for the operation passed
	 * @param opr Operation to be Executed
	 * @return MatrixAgent that will execute the operation
	 */
	private MatrixAgent getMatrixAgentForOperation(Operation opr)
	{
		MatrixAgent agent = null; 
		String operationName = null;
		toLog("MatrixTransactionHandler- getMatrixAgentForOperation()",MatrixUtil.DEBUG_LOG);
		
		if(opr.getChangePermissionOp()!=null)
		{
			agent = new ChangePermissionOperation();
			operationName = "ChangePermissionOperation";
		}	
		else if(opr.getCopyOp()!=null)
		{
			agent = new CopyOperation();
			operationName = "CopyOperation";
		}	
		else if(opr.getCreateOp()!=null)
		{
			agent = new CreateOperation();
			operationName = "CreateOperation";
		}	
		else if(opr.getDeleteOp()!=null)
		{
			agent = new DeleteOperation();
			operationName = "DeleteOperation";
		}		
		else if(opr.getDownloadDataSetOp()!=null)
		{
			agent = new DownloadDataSetOperation();
			operationName = "DownloadDataSetOperation";
		}
		else if(opr.getIngestOp()!=null)
		{
			agent = new IngestDataSetOperation();
			operationName = "IngestDataSetOperation";
		}	
		else if(opr.getListOp()!=null)
		{
			agent = new ListOperation();
			operationName = "ListOperation";
		}	
		else if(opr.getPrepareTicketOp()!=null)
		{
			agent = new PrepareTicketOperation();
			operationName = "PrepareTicketOperation";
		}	
		else if(opr.getRenameOp()!=null)
		{
			agent = new RenameOperation();
			operationName = "RenameOperation";
		}	
		else if(opr.getReplicateOp()!=null)
		{
			agent = new ReplicateOperation();
			operationName = "ReplicateOperation";
		}	
		else if(opr.getSeekNReadOp()!=null)
		{
			agent = new SeekAndReadOperation();
			operationName = "SeekAndReadOperation";
		}	
		else if(opr.getSeekNWriteOp()!=null)
		{
			agent = new SeekAndWriteOperation();
			operationName = "SeekAndWriteOperation";
		}
		else if(opr.getMetadataQueryOp()!=null)
		{
			agent = new MetaDataSetOperation();
			operationName = "MetaDataSetOperation";
		}
		else if(opr.getIngestMetadataOp()!=null)
		{
			agent = new IngestMetaDataOperation();
			operationName = "IngestMetaDataOperation";
		}				
		else
			throw new IllegalArgumentException(	"Invalid Operation requested.");
		currentOperationNameForMatrixAgentBuilt	= operationName;
		
		toLog("MatrixTransactionHandler- getMatrixAgentForOperation()- operationName:"+operationName,MatrixUtil.DEBUG_LOG);	
			return(agent);	
	}
	
	/**
	 * NOT USED
	 */
	private MatrixAgent getMatrixAgentForOperationOld(String operationName)
	{
		MatrixAgent agent = null; 
		toLog("MatrixTransactionHandler- getMatrixAgentForOperation()- operationName:"+operationName,MatrixUtil.DEBUG_LOG);
		if(operationName.equals("changePermissionOp"))
			agent = new ChangePermissionOperation();
		else if(operationName.equals("copyOp"))
			agent = new CopyOperation();
		else if(operationName.equals("createOp"))
			agent = new CreateOperation();
		else if(operationName.equals("deleteOp"))
			agent = new DeleteOperation();
		else if(operationName.equals("downloadDataSetOp"))
			agent = new DownloadDataSetOperation();
		else if(operationName.equals("ingestOp"))
			agent = new IngestDataSetOperation();
		else if(operationName.equals("listOp"))
			agent = new ListOperation();
		else if(operationName.equals("prepareTicketOp"))
			agent = new PrepareTicketOperation();
		else if(operationName.equals("renameOp"))
			agent = new RenameOperation();
		else if(operationName.equals("replicateOp"))
			agent = new ReplicateOperation();
		else if(operationName.equals("seekNReadOp"))
			agent = new SeekAndReadOperation();
		else if(operationName.equals("seekNWriteOp"))
			agent = new SeekAndWriteOperation();	
		else
			throw new IllegalArgumentException(	"Invalid Operation requested.");
		return(agent);	
	}

	public static void main(String[] args)
	{
		
	}
	
	/**
	 * Handles the execution of a flow in a transaction
	 * @param srbFlow Matrix Flow to be executed
	 * @param flowId Flow ID of flow to be executed
	 * @param matrixConnForAgent Matrix Connection to be used by all steps in the flow
	 */
	public void handleMatrixFlow(Flow srbFlow,String flowId,MatrixConnection matrixConnForAgent)
	{
		toLog("MatrixTransactionHandler-handleMatrixFlow-BEGIN:flowId"+flowId, MatrixUtil.DEBUG_LOG);
		flowInProgress = true;
		int opCount =0; 					// Number of Operations for the transaction
		String currentOperationName = null;		// Name of Current Operation
		MatrixAgent currentAgent = null;		// Thread for Current Operation
		MatrixAgent[] listOfSRBAgent = null; 	// List of Threads for all Operations of Transaction
		boolean isExecutionParallel = false;// Flag whether transaction operations are Parrallel or Sequential
		//String currentAgentId = null;		// Id for each operation of the Transaction
		Operation currentOperation = null;
		currentFlowId = flowId; 
		
		boolean flowAbort = false;	
		try
		{
			currentFlowStatusCode = 	MatrixCodes.FLOW_STARTED;
			String flowID = flowId;
			flowAbort = checkFlowAbortFlag(transactionId,flowID);
			if(!flowAbort)
			{
				logFlowBeginStatus();
				// Check if operations to be run in sequential or parallel
				if(srbFlow.getFlowType().equals("parallel"))
					 isExecutionParallel = true;
				else
					isExecutionParallel = false;
						
				// Initialize the list of SRB Agents
				List stepList = srbFlow.getSteps();
				opCount = stepList.size();
						
				listOfSRBAgent = new MatrixAgent[opCount];
				
				// Create the List of Threads for every operation
				Step currentStep = null;
				ListIterator itr = stepList.listIterator();		
				String[] txnStepNames = new String[opCount];
						
						
				
						
				// Intialize Track Count 
				 // Added by Reena on 19 June 2003
				 currentFlowId = flowId; //srbFlow.getFlowID(); // Has to change for multiple flows
				 currentFlowStepCompleteCount = 0;
				 currentFlowStepCount  = listOfSRBAgent.length;
						
						
				for(int i=0;i<listOfSRBAgent.length;i++)
				{
					int temp = 0;
							
					currentStep = (Step) itr.next();
	
					txnStepNames[i] =  currentStep.getStepID();//currentOperationName;
					currentOperation = currentStep.getOperation();
					//currentAgent = getMatrixAgentForOperation(currentOperationName);
					
					// Changed so that the user does not have to pass operationName
					currentAgent = getMatrixAgentForOperation(currentOperation); 
					currentOperationName =  currentOperationNameForMatrixAgentBuilt ; //currentStep.getStepName(); 
					toLog("MatrixTransactionHandler-handleMatrixFlow:currentAgent"+currentAgent, MatrixUtil.DEBUG_LOG);
					toLog("MatrixTransactionHandler-handleMatrixFlow:currentOperationNameForMatrixAgentBuilt : "+currentOperationNameForMatrixAgentBuilt, MatrixUtil.DEBUG_LOG);
											
					//new SRBAgent(currentOperationName,currentOperation, srbRequestObject,String.valueOf(i),transactionId,srbStore);
					// Setting Atrributes for Matrix Agent
					//toLog("MatrixTransactionHandler-handleMatrixFlow:getStepID"+currentStep.getStepID(), 5);
					currentAgent.setStepId(currentStep.getStepID());
					
					//toLog("MatrixTransactionHandler-handleMatrixFlow:currentOperationName"+currentOperationName, 5);
					currentAgent.setOperationName(currentOperationName);
					
					//toLog("MatrixTransactionHandler-handleMatrixFlow:currentOperation"+currentOperation, 5);
					currentAgent.setOperation(currentOperation);
					
					//toLog("MatrixTransactionHandler-handleMatrixFlow:srbRequestObject"+srbRequestObject, 5);
					currentAgent.setSRBRequestObject(srbRequestObject);
					
					//toLog("MatrixTransactionHandler-handleMatrixFlow:String.valueOf(i)"+String.valueOf(i), 5);
					currentAgent.setSrbAgentId(String.valueOf(i));
					
					//toLog("MatrixTransactionHandler-handleMatrixFlow:transactionId"+transactionId, 5);
					currentAgent.setTransactionId(transactionId);
					
					//toLog("MatrixTransactionHandler-handleMatrixFlow:srbStore"+srbStore, 5);
					currentAgent.setSRBStore(srbStore);				
					
					//toLog("MatrixTransactionHandler-handleMatrixFlow:attachmentItr"+attachmentItr, 5);
					currentAgent.setAttachmentIterator(attachmentItr);	
							
					// Added by Reena Mathew on 20 June 2003
					//toLog("MatrixTransactionHandler-handleMatrixFlow:currentFlowId"+currentFlowId, 5);
					currentAgent.setFlowId(currentFlowId);
					
					//toLog("MatrixTransactionHandler-handleMatrixFlow:this"+this, 5);
					currentAgent.setMatrixTransactionHandler(this);	
					
					//toLog("MatrixTransactionHandler-handleMatrixFlow:matrixConnForAgent"+matrixConnForAgent, 5);		
					currentAgent.setMatrixConnection(matrixConnForAgent);
					
					//toLog("MatrixTransactionHandler-currentAgent:this"+currentAgent, 5);			
					listOfSRBAgent[i] = currentAgent;	
					
				}
				toLog("MatrixTransactionHandler-handleMatrixFlow:FLOW_BUILT", MatrixUtil.DEBUG_LOG);		
						
				// Log Transaction Steps intp Store
				String userId = getSRBUserID();
				srbStore.setTransactionStepIDs(transactionId,currentFlowId,userId,txnStepNames);
						
				
						
				// Execute the steps of the Transaction
				if(isExecutionParallel)
					runParallel(listOfSRBAgent);
				else
					runSequential(listOfSRBAgent);
				
				toLog("MatrixTransactionHandler-handleMatrixFlow-CHECK START flowInProgress:"+flowInProgress, MatrixUtil.DEBUG_LOG);	
				while(flowInProgress)
				{
					//toLog("MatrixTransactionHandler-handleMatrixFlow-flowInProgress:"+flowInProgress, 5);	
				}
				currentFlowStatusCode = 	MatrixCodes.FLOW_COMPLETE;
				toLog("MatrixTransactionHandler-handleMatrixFlow-CHECK END flowInProgress:"+flowInProgress, MatrixUtil.DEBUG_LOG);
			}
			else
			{
				currentFlowStatusCode = MatrixCodes.FLOW_ABORTED_DUE_TO_REQUEST_FROM_USER;
				logFlowAbortStatus();
			}
			
			toLog("MatrixTransactionHandler-handleMatrixFlow-END", MatrixUtil.DEBUG_LOG);		
		}
		catch(Exception e)
		{
			toLog("MatrixTransactionHandler-handleMatrixFlow-EXCEPTION :"+e,MatrixUtil.ERROR_LOG);
			currentFlowStatusCode = MatrixCodes.FLOW_ABORTED_DUE_TO_ERROR;				
		}
		finally
		{
			
		}
		
	}


}
