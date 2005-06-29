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

import edu.sdsc.grid.io.srb.SRBAccount;
import edu.sdsc.grid.io.srb.SRBFileSystem;
import edu.sdsc.grid.io.srb.SRBFile;
import edu.sdsc.grid.io.srb.SRBContainer;
import edu.sdsc.grid.io.srb.SRBException;
import edu.sdsc.grid.io.srb.SRBRandomAccessFile;
import edu.sdsc.grid.io.local.LocalFile;
import edu.sdsc.grid.io.GeneralFile;

import edu.sdsc.matrix.srb.util.MatrixUtil;
import edu.sdsc.matrix.srb.parser.*;
import edu.sdsc.matrix.srb.core.SRBStore;
import edu.sdsc.matrix.srb.core.MatrixCodes;
import edu.sdsc.matrix.srb.core.MatrixConnection;
import edu.sdsc.matrix.srb.core.GridConnection;

import java.io.*;
import java.util.Calendar;
import java.util.Iterator;
import java.util.ArrayList;
import javax.xml.soap.AttachmentPart;
import javax.xml.soap.SOAPMessage;
import javax.activation.DataHandler;
import javax.xml.soap.*;
import javax.activation.*;

/**
 * This is the base class for all the classes that are responsible for handling 
 * the SRB Operation of relating to a Container, Collection or Data Set.
 * This class contains the common methods that are required by all the operations
 * interacting with SRB.
 *  
 * @author rmathew
 * 
 */

public class MatrixAgent extends Thread
{
	/**
	 * Attribute for the operation the thread handles
	 */
	private String operationName; 		
	
	/**
	 * Attribute for the SRB Request Object
	 */ 
	private DataGridRequest srbRequestObject;
	
	/**
	 * Attribute for the SRB Agent ID
	 */ 	
	private String srbAgentId;
	
	/**
	 * Operation to be executed
	 */			
	private Operation srbOperation;
	
	/**
	 * Current Transaction ID
	 */
	private String transactionId;
	
	/**
	 * Current Step ID
	 */
	private String stepId;
	
	/**
	 * Reference to Matrix Store
	 */
	private SRBStore srbStore;
	
	/**
	 * JAXB Object used to create JAXB response objects
	 */
	private ObjectFactory objFactory;
	
	/**
	 * List of Input Attachments
	 */
	private Iterator attachmentItr;
	
	/**
	 *  Step's start status
	 */
	private StepStatusResponse startStatus;
	
	/**
	 * Step's end status
	 */
	private StepStatusResponse endStatus;
	
	/**
	 * Exception thrown while executing the agent
	 */
	private Exception srbException;
	
	/**
	 * Response for the operation executed
	 */
	private OpResult srbResponse;
		
	/**
	 * Logging Level - NOT USED
	 */	
	public static final int filterLevel = 0; // Have to make lower filter level to see debug messages
	
	// Added by Reena Mathew on 20 June 2003
	// For Implementing Transaction,Flow & Step Complete Check
	/**
	 * Current Flow ID
	 */
	private String flowId;
	
	/**
	 * Reference to Transaction Handler that started this agent
	 */
	private MatrixTransactionHandler agentTransaction;
	
	/**
	 * Matrix Connection to be used during the execution of the agent/operation
	 */
	private MatrixConnection matrixConnection;

	/**
	 * Default Constructor
	 */
	public MatrixAgent()
	{
		
	}
		
	/**
	 *  Constructor for the SRB Agent
	 *  Will be called by the Transaction Manager for each SRB operation
	 */
	public MatrixAgent(String opName,Operation opr,DataGridRequest srbObject,
					 String id,String txnId,SRBStore store)
	{
		int temp = 0;
		
		operationName = opName.trim();
		srbRequestObject = srbObject;
		srbAgentId = id;
		srbOperation = opr;
		transactionId = txnId;
		srbStore=store;
				
	}
		
		
	/**
	 *  Constructor for the SRB Agent with Step ID
	 *  Will be called by the Transaction Manager for each SRB operation
	 */
	public MatrixAgent(String opName,Operation opr,DataGridRequest srbObject,
					 String id,String txnId,String spId,SRBStore store)
	{
		int temp = 0;
		
		operationName = opName.trim();
		srbRequestObject = srbObject;
	    srbAgentId = id;
		srbOperation = opr;
		transactionId = txnId;
		stepId = spId;
		srbStore=store;		
	}

	// Arun change start
	/**
	 * To log method by Arun
	 */		
	public void toLog(String msg, int errLevel){
		MatrixUtil.toLog(getTransactionId(),msg,errLevel);
	}
	
	// End Arun change
        
	/**
	 * Set Operation Name
	 * @param s Operation Name
	 */         
	public void setOperationName(String s)
	{
		operationName = s;
	}
        
	/**
	 * Get Operation Name
	 * @return Operation Name
	 */  
	public String getOperationName()
	{
		return(operationName);
	}

	/**
	 * Set Operation
	 * @param o Operation
	 */         
	public void setOperation(Operation o)
	{
		srbOperation= o;
	}
        
	/**
	 * Get Operation
	 * @return Operation
	 */  
	public Operation getOperation()
	{
		return(srbOperation);
	}	

	/**
	 * Set SRB Request Object
	 * @param obj Input Request Object
	 */  
	public void setSRBRequestObject(DataGridRequest obj)
	{
		srbRequestObject = obj;
	}

	/**
	 * Get SRB Request Object
	 * @return Input Request Object
	 */  
	public DataGridRequest getSRBRequestObject()
	{
		return(srbRequestObject);
	}

	/**
	 * Set srbAgentId
	 * @param id Agent ID
	 */  
	public void setSrbAgentId(String id)
	{
		srbAgentId = id;
	}

	/**
	 * Get srbAgentId
	 * @return SRB Agent ID
	 */  
	public String getSrbAgentId()
	{
		return(srbAgentId);
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
	 * Set Flow Id
	 * @param obj Flow ID
	 */  
	public void setFlowId(String obj)
	{
		flowId = obj;
	}

	/**
	 * Get Flow ID
	 * @return Flow ID
	 */  
	public String getFlowId()
	{
		return(flowId);
	}
	
	
	/**
	 * Set stepId
	 * @param obj Step ID
	 */  
	public void setStepId(String obj)
	{
		stepId = obj;
	}

	/**
	 * Get stepId
	 * @return Step ID
	 */  
	public String getStepId()
	{
		return(stepId);
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
	 * Set Attachment Iterator
	 * @param obj List of Input Attachments
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
	 * Set Exception
	 * @param e Exception 
	 */  
	public void setSRBException(Exception e)
	{
		srbException = e;
	}

	/**
	 * Get Exception
	 * @return Exception 
	 */  
	public Exception getSRBException()
	{
		return(srbException);
	}
	
	/**
	 * Set Response
	 * @param res Operation/Step Result
	 */  
	public void setSRBResponse(OpResult res)
	{
		srbResponse = res;
	}

	/**
	 * Get Response
	 * @return Operation/Step Result
	 */  
	public OpResult getSRBResponse()
	{
		return(srbResponse);
	}
	
	/**
	 * Set Object Factory
	 * @param factory Reference to JAXB Object Factory
	 */  
	public void setObjectFactory(ObjectFactory factory)
	{
		objFactory = factory;
	}

	/**
	 * Get ObjectFactory
	 * @return Object Factory
	 */  
	public ObjectFactory getObjectFactory()
	{
		return(objFactory);
	}
	
	/**
	 * Set Transaction Handler
	 * @param txnHndl Transaction Handler
	 */  
	public void setMatrixTransactionHandler(MatrixTransactionHandler txnHndl)
	{
		agentTransaction = txnHndl;
	}

	/**
	 * Get Transaction Handler
	 * @return Transaction Handler
	 */  
	public MatrixTransactionHandler getMatrixTransactionHandler()
	{
		return(agentTransaction);
	}
	
	/**
	 * Set Matrix Connection
	 * @param conn Matrix Connection
	 */  
	public void setMatrixConnection(MatrixConnection conn)
	{
		matrixConnection = conn;
	}

	/**
	 * Get Matrix Connection
	 * @return Matrix Connection 
	 */  
	public MatrixConnection getMatrixConnection()
	{
		return(matrixConnection);
	}
	
	/**
	 * Check if Step has to be aborted
	 * @return Step Abort Check Flag 
	 */
	public boolean checkStepAbortFlag()
	{
		toLog("MatrixAgent() - checkStepAbortFlag() -BEGIN",MatrixUtil.DEBUG_LOG);
		boolean abortStep = false;
		StepStatusResponse stepStatus =  (StepStatusResponse)srbStore.getStepStatus(getTransactionId(),getFlowId(),getStepId());
		if(stepStatus!=null)
			abortStep=	stepStatus.isAbortRequested();								   
		toLog("MatrixAgent() - checkStepAbortFlag() -END",MatrixUtil.DEBUG_LOG);											   
	   return(abortStep); 
	}
	
	
	/**
	 * Logs Step Status for Step Abort to Matrix Store
	 */
	public void logStepAbortStatus() throws Exception
	{
		//	Log Step BEGIN into Operation Store
		toLog("MatrixAgent() - logStepAbortStatus() -BEGIN",MatrixUtil.DEBUG_LOG);
		objFactory = new ObjectFactory();
		StepStatusResponse abortStatus = objFactory.createStepStatusResponse();		
		abortStatus.setStepID(stepId);
		abortStatus.setStartTime(Calendar.getInstance());
		abortStatus.setEndTime(Calendar.getInstance());
		abortStatus.setStatusCode(MatrixCodes.STEP_ABORTED_DUE_TO_REQUEST_FROM_USER);
		abortStatus.setStatusDescription("Step Execution was aborted due to request from user.");
	
		toLog("MatrixAgent() - logStepAbortStatus() -startStatus:"+startStatus,MatrixUtil.DEBUG_LOG);		
	
		if(srbStore!=null)
			srbStore.setStepStatus(transactionId, flowId,stepId,abortStatus);
		else
			throw new Exception("Handle to persistence store not available.");
		toLog("MatrixAgent() - logStepAbortStatus() -END",MatrixUtil.DEBUG_LOG);		
	}
	
	
	
	/**
	 * Logs Step Status for Step Start to Matrix Store
	 */
	public void logStepBeginStatus() throws Exception
	{
		//	Log Step BEGIN into Operation Store
		toLog("MatrixAgent() - logStepBeginStatus() -BEGIN",MatrixUtil.DEBUG_LOG);
		objFactory = new ObjectFactory();
		startStatus = objFactory.createStepStatusResponse();
		endStatus = objFactory.createStepStatusResponse();
		startStatus.setStepID(stepId);
		startStatus.setStartTime(Calendar.getInstance());
		startStatus.setStatusCode(MatrixCodes.STEP_STARTED);
		
		
		toLog("MatrixAgent() - logStepBeginStatus() -startStatus:"+startStatus,MatrixUtil.DEBUG_LOG);		
		
		if(srbStore!=null)
		{
			toLog("MatrixAgent() -  transactionId-"+transactionId,MatrixUtil.DEBUG_LOG);
			toLog("MatrixAgent() -  stepId-"+stepId,MatrixUtil.DEBUG_LOG);
			srbStore.setStepStatus(transactionId, flowId,stepId,startStatus);
		}	
		else
			throw new Exception("Handle to persistence store not available.");
		toLog("MatrixAgent() - logStepBeginStatus() -END",MatrixUtil.DEBUG_LOG);		
	}
	
	/**
	 * Logs Step Status for Step End to Matrix Store
	 */
	public void logStepEndStatus()// throws Exception
	{
		toLog("MatrixAgent() - logStepEndStatus() -BEGIN",MatrixUtil.DEBUG_LOG);
		toLog("MatrixAgent() -  transactionId-"+transactionId,MatrixUtil.DEBUG_LOG);
		toLog("MatrixAgent() -  stepId-"+stepId,MatrixUtil.DEBUG_LOG);
		// Added by Reena Mathew on June 20 2003
		endStatus = srbStore.getStepStatus(transactionId, flowId,stepId);
		
		//Log Step END into Operation Store
		if(endStatus==null)
		{
			try
			{
				endStatus = objFactory.createStepStatusResponse();			
			}
			catch(Exception e)
			{	
				toLog("MatrixAgent() -  logStepEndStatus-Exception:"+e,MatrixUtil.ERROR_LOG);
			}
			endStatus.setStartTime(startStatus.getStartTime());
			endStatus.setStepID(stepId);
		}	
		endStatus.setEndTime(Calendar.getInstance());
	
		if(srbException!=null)
		{
			// Log Exception
			endStatus.setStatusCode(MatrixCodes.STEP_ABORTED_DUE_TO_ERROR);
			endStatus.setStatusDescription(srbException.getMessage());
			toLog("SRB Exception : "+srbException.toString(),MatrixUtil.ERROR_LOG);
		}
		else
		{
			 // Thread Execution successful
			endStatus.setStatusCode(MatrixCodes.STEP_COMPLETE);
			endStatus.setStatusDescription("Step Execution Successful");
			// Need to send back response some how. Keep it in Store ???
			endStatus.setResult((OpResult)getSRBResponse());
			toLog("Thread executed successfully for operation: "+operationName,MatrixUtil.DEBUG_LOG);                   
		}
		
		srbStore.setStepStatus(transactionId,flowId, stepId,endStatus);
		try
		{
			getMatrixTransactionHandler().updateMatrixTransactionStatusCount(getFlowId(),getStepId());
		}
		catch(Exception e)
		{
			toLog("MatrixAgent() - logStepEndStatus() Exception updating count:"+e,MatrixUtil.ERROR_LOG);	
		}
		toLog("MatrixAgent() - logStepEndStatus()-END ",MatrixUtil.DEBUG_LOG);
	}
	
	/**
	 * Executes a step of the SRB Transaction
	 * This method has to be overridden by all classes that extend this class. 
	 */
	public void run()
	{
	    System.out.println("Spanning a SRBAgent Thread");
		int methodIndex = 0;
		Exception srbException = null;
		StepStatusResponse startStatus;
		StepStatusResponse endStatus = null;
		Object response = null;
	                                 
		// Map Operation to method
		try
		{
			//	Log Step BEGIN into Operation Store
			objFactory = new ObjectFactory();
			startStatus = objFactory.createStepStatusResponse();
			endStatus = objFactory.createStepStatusResponse();
			startStatus.setStepID(stepId);
			startStatus.setStartTime(Calendar.getInstance());
			startStatus.setStatusCode(0);
			
			if(srbStore!=null)
				srbStore.setStepStatus(transactionId, flowId,operationName,startStatus);
			else
				throw new Exception("Handle to persistence store not available.");	
				 
			
				
		}
		catch(Exception e)
		{
			//System.out.println("Exception :"+e);
			srbException = e;
			
		}
		finally
		{
			//Log Step END into Operation Store
			endStatus.setStepID(stepId);
			endStatus.setEndTime(Calendar.getInstance());
			
			if(srbException!=null)
			{
				// Log Exception
				
				endStatus.setStatusCode(11111111);
				endStatus.setStatusDescription(srbException.getMessage());
				System.out.println("SRB Exception : "+srbException.toString());
			}
			else
			{
				 // Thread Execution successful
				endStatus.setStatusCode(0);
				endStatus.setStatusDescription("Step Execution Successful");
				// Need to send back response some how. Keep it in Store ???
				//endStatus.setResponse(response);
				System.out.println("Thread executed successfully for operation: "+operationName);                   
			}
			
			//srbStore.setStepStatus(transactionId, operationName,endStatus);	
		}
		
		
	}

	/**
	 * Determines the type of grid object:Container/Collection/Data Set
	 * @param gridObject JAXB Object which represents a Container,Collection or Data Set
	 * @return Matrix Datagrid Object which contains the datagrid object details
	 */
	public MatrixDatagridObject determineFilePath(StdDatagridObject gridObject)
	{
		String gridDataType = null;
		String filePath = null;
		Collection srbColl = gridObject.getCollectionNow();
		Container srbCont = gridObject.getContainerNow();
		DataSet srbDS = gridObject.getDataSetNow();
		MatrixDatagridObject matrixGridObject = null;
		
		if(srbColl!=null)
		{
			gridDataType = new String("Collection");
			filePath = srbColl.getStdCollection().getCollectionName();
			matrixGridObject = new MatrixDatagridObject(MatrixDatagridObject.GRIDTYPE_COLLECTION,filePath);		
		}
		else if(srbCont!=null)
		{
			gridDataType = new String("Container");
			filePath = srbCont.getContainerName();
			matrixGridObject = new MatrixDatagridObject(MatrixDatagridObject.GRIDTYPE_CONTAINER,filePath);
		}
		else if(srbDS!=null)
		{
			gridDataType = new String("DataSet");
			filePath = srbDS.getLogicalLocation() + File.separator + srbDS.getDataIdentifier();
			matrixGridObject = new MatrixDatagridObject(MatrixDatagridObject.GRIDTYPE_DATASET,filePath);
		}
		return(matrixGridObject);
		
	}
	
	/**
	 * Determine the file path for a data set
	 * @param gridDataSet Data Set Details 
	 * @return File Path to Data Set
	 */
	public String determineFilePath(DataSet gridDataSet)
	{
		String gridDataType = null;
		String filePath = null;		
		DataSet srbDS = gridDataSet;
	
		if(srbDS!=null)
		{
			gridDataType = new String("DataSet");
			filePath = srbDS.getLogicalLocation() + File.separator + srbDS.getDataIdentifier();
		}
		return(filePath);
	
	}

	
	 
	 

	/**
	 * NOT USER
	  *	Maps to the constructor SRBAccount(..) in class SRBFile
	  */	
	public SRBAccount srbCreateSRBAccountDepr() throws Exception
	{
		// Extract parameters.....
		UserInfo srbUser = srbRequestObject.getGridUser();
		VirtualOrganizationInfo virtualOrg = srbRequestObject.getVOInfo();

		SRBAccount account = null;
		String ticketId = null;
		
		// Check if user has a ticket
		// NOTE- Make changes for new ticket
		 
		GridTicketInfo gridTicket= srbUser.getGridTicket();
		if(gridTicket!=null)
		{
			toLog("MatrixAgent() - srbCreateSRBAccount() -gridTicket AVAILABLE",MatrixUtil.DEBUG_LOG);
			TicketID srbTicketID = gridTicket.getTicketID();
			TicketID.TicketStringType ticketString = srbTicketID.getTicketString();			
			ticketId = ticketString.getValue();
			toLog("MatrixAgent() - srbCreateSRBAccount() -ticketId:"+ ticketId,MatrixUtil.DEBUG_LOG);
		}	
			
		try
		{
		
			if(ticketId!=null)
			{
				// Check for ticket id in ticket store
				account = (SRBAccount)srbStore.retrieveTicket(ticketId);
				toLog("MatrixAgent() - srbCreateSRBAccount() -srbAccountForTicket:"+ account,MatrixUtil.DEBUG_LOG);
				 	
				if(account==null)
					throw new IllegalArgumentException("Invalid Ticket Id");
			
									
			}
			else
			{
				int numberOfParameters=7;
				String host = virtualOrg.getServer(); 
				int port = virtualOrg.getPort();
				String userName = srbUser.getUserID(); 
				String password = srbUser.getChallengeResponse();
				String homeDirectory = srbUser.getHomeDirectory();
				String mdasDomainName =  srbUser.getOrganization().getOrganizationName();
				String defaultStorageResource= srbUser.getDefaultStorageResource();
		
				if (numberOfParameters == 7)
				{
					//host, port, user name, password,
					//home directory, mdas domain home, default storage resource
					account = new SRBAccount(host, port, userName, password,homeDirectory, mdasDomainName, defaultStorageResource );
				}
				else if (numberOfParameters == 1)
				{
					//user info file
					account = new SRBAccount( host);
				}
				else if (numberOfParameters == 0)
				{
					//default
					account = new SRBAccount( );
				}
				else
				{
					throw new IllegalArgumentException(	"Wrong number of arguments to create SRB Account.");
				}
			
			}
		}
		catch(SRBException e)
		{
			String exceptionMessage = SRBException.getStandardMessage(e.getType());
			throw new Exception(exceptionMessage);			
		}
		catch(Exception e)
		{
			throw e; 
		}
		
		return(account);
	}
	
	
	/**
	 * NOT USED
	 * Handle Attachments
	 */
	 public synchronized Object getAttachmentDeprecated(String contentId) throws Exception
	 {
	 		Object attachment = null;
	 		String currentId = null;
	 		AttachmentPart currentAttachment = null;
	 		InputStream attachmentContent = null;
	 		String contentType = null;
	 		DataHandler contentDataHandler =null;
	 		try
	 		{
				toLog("MatrixAgent - getAttachment() - contentId: "+contentId,MatrixUtil.DEBUG_LOG);		
	 			if(attachmentItr!=null)
	 			{
					toLog("MatrixAgent - getAttachment() - attachmentItr not NULL "+contentId,MatrixUtil.DEBUG_LOG);	
	 				while(attachmentItr.hasNext())
		 			{
		 				currentAttachment = (AttachmentPart) attachmentItr.next();
		 				contentType = currentAttachment.getContentType();
						toLog("MatrixAgent - getAttachment() - ContentType : "+ contentType + ",ContentID:"+contentId,MatrixUtil.DEBUG_LOG);
						toLog("MatrixAgent - getAttachment() - getContentId: "+ currentAttachment.getContentId()+",ContentID:"+contentId,MatrixUtil.DEBUG_LOG);
		 				if(currentAttachment.getContentId().equals(contentId))
		 				{
							toLog("MatrixAgent - getAttachment() - Content Id matches ContentID:"+contentId,MatrixUtil.DEBUG_LOG);	
		 					contentDataHandler = currentAttachment.getDataHandler();
							toLog("MatrixAgent - getAttachment() - DataHandler Retrieved"+",ContentID:"+contentId,MatrixUtil.DEBUG_LOG);	
		 					attachmentContent = contentDataHandler.getInputStream();
							toLog("MatrixAgent - getAttachment() - InputStream Retrieved"+",ContentID:"+contentId,MatrixUtil.DEBUG_LOG);	
		 					break;
		 				}	
		 			}
	 			}
	 			else
					toLog("getAttachment() - attachmentItr NULL "+",ContentID:"+contentId,MatrixUtil.DEBUG_LOG);	
	 			return(attachmentContent);
	 		}
	 		catch(Exception e)
	 		{
	 			throw new Exception("Cannot extract attachment from message.");	
	 		}
	 }
	 
	/**
	 * Handle Attachments
	 * Retrieves the attachment required for an agent
	 * @param contentId Content ID for Attachment
	 * @return Attachment 
	 */
	 public Object getAttachment(String contentId) throws Exception
	 {
			Object attachment = null;
			String currentId = null;
			AttachmentPart currentAttachment = null;
			InputStream attachmentContent = null;
			String contentType = null;
			DataHandler contentDataHandler =null;
			try
			{
				toLog("MatrixAgent - getAttachment() - txnid.contentId: "+getTransactionId()+"."+contentId,MatrixUtil.DEBUG_LOG);
				currentAttachment = (AttachmentPart)srbStore.getInputAttachment(getTransactionId(),contentId);		
				if(currentAttachment!=null)
				{
					toLog("MatrixAgent - getAttachment() - currentAttachment not NULL "+getTransactionId()+"."+contentId,MatrixUtil.DEBUG_LOG);	
					contentType = currentAttachment.getContentType();
					toLog("MatrixAgent - getAttachment() - ContentType : "+ contentType + ",ContentID:"+contentId,MatrixUtil.DEBUG_LOG);
					toLog("MatrixAgent - getAttachment() - getContentId: "+ currentAttachment.getContentId()+",ContentID:"+contentId,MatrixUtil.DEBUG_LOG);
					toLog("MatrixAgent - getAttachment() - Content Id matches ContentID:"+contentId,MatrixUtil.DEBUG_LOG);	
					contentDataHandler = currentAttachment.getDataHandler();
					toLog("MatrixAgent - getAttachment() - DataHandler Retrieved"+",ContentID:"+contentId,MatrixUtil.DEBUG_LOG);	
					attachmentContent = contentDataHandler.getInputStream();
					toLog("MatrixAgent - getAttachment() - InputStream Retrieved"+",ContentID:"+contentId,MatrixUtil.DEBUG_LOG);	
				}	
				else
					toLog("getAttachment() - attachmentItr NULL "+",TXNID.ContentID:"+getTransactionId()+"."+contentId,MatrixUtil.DEBUG_LOG);	
				return(attachmentContent);
			}
			catch(Exception e)
			{
				throw new Exception("Cannot extract attachment from message.");	
			}
	 }
	 
	 
	 
	 /**
	  * Create Attachment
	  * @param contentType Content Type for Attachment
	  * @param attachmentLocation Location of Attachment
	  * @param contentId Content Id for Attachment
	  * @return Attachment
	  */
	public Object createAttachment(String contentType,String attachmentLocation,String contentId) throws Exception
	{
		try
		{
			AttachmentPart ap = null;
			MessageFactory mf = MessageFactory.newInstance();
			SOAPMessage msg = mf.createMessage();
			FileDataSource ds = new FileDataSource(attachmentLocation);
			DataHandler dh = new DataHandler(ds);
			ap = msg.createAttachmentPart(dh);		
			ap.setContentId(contentId);
			return(ap);
		}
		catch(SOAPException se)
		{
			throw new Exception("Cannot create Attachment for get operations."+ se.toString());
		}
		catch(Exception e)
		{
			throw new Exception("Cannot create Attachment for get operations."+ e.toString());
		}
	
	}
	
	
	/**
	 * To Retrieve Actual Connection for Agent
	 * @return Grid Connection to be used by Agent 
	 */
	public GridConnection retrieveGridConnection()
	{
		toLog("MatrixAgent() - retrieveGridConnection() -BEGIN",MatrixUtil.DEBUG_LOG);
		 Object gridConnection = null;
		 GridConnection gridConn = matrixConnection.getActiveGridConnection();
		toLog("MatrixAgent() - retrieveGridConnection() -END",MatrixUtil.DEBUG_LOG);
		 return(gridConn);
		
	}

	/**
	  *  Method to test the class
	  */
	public static void main(String[] args)
	{
		System.out.println("SRBAgent - main()");
		MatrixAgent agent = new MatrixAgent();		
		/*
		FileDataSource ds = new FileDataSource("test.xml");
		DataHandler dh = new DataHandler(ds);
		AttachmentPart attachment = msg.createAttachmentPart(dh);
		attachment.setContentId("attachcontentid");
		ArrayList attachmentList = new ArrayList();
		attachmentList.add(attachment);
		*/
		
		agent.setOperationName("copyToLocal");
		agent.start();
		
	}
	
		
}
