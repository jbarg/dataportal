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

import java.io.File;
import java.io.InputStream;
import java.util.List;
import javax.activation.DataHandler;
import javax.activation.FileDataSource;
import javax.xml.bind.JAXBException;
import javax.xml.soap.AttachmentPart;
import javax.xml.soap.MessageFactory;
import javax.xml.soap.SOAPException;
import javax.xml.soap.SOAPMessage;

import edu.sdsc.matrix.srb.core.GridConnection;
import edu.sdsc.matrix.srb.core.MatrixConnection;
import edu.sdsc.matrix.srb.core.MatrixDatagridObject;
import edu.sdsc.matrix.srb.core.MatrixException;
import edu.sdsc.matrix.srb.core.MatrixThread;
import edu.sdsc.matrix.srb.core.flow.FlowThread;
import edu.sdsc.matrix.srb.parser.Collection;
import edu.sdsc.matrix.srb.parser.Container;
import edu.sdsc.matrix.srb.parser.DataGridRequest;
import edu.sdsc.matrix.srb.parser.DataSet;
import edu.sdsc.matrix.srb.parser.ObjectFactory;
import edu.sdsc.matrix.srb.parser.OpResult;
import edu.sdsc.matrix.srb.parser.Operation;
import edu.sdsc.matrix.srb.parser.StdDatagridObject;
import edu.sdsc.matrix.srb.parser.Step;
import edu.sdsc.matrix.srb.parser.StepStatusResponse;
import edu.sdsc.matrix.srb.parser.TVariables;
import edu.sdsc.matrix.srb.util.MatrixUtil;
import edu.sdsc.matrix.srb.parser.StdDataSet;
import edu.sdsc.matrix.srb.parser.StdCollection;
import edu.sdsc.matrix.srb.parser.Status;

/**
	* This is the base class for all the classes that are responsible for handling
	* the an operation
	*
	* @author rmathew
	* @author Jon Weinberg
	*
	*/
public abstract class MatrixAgent extends MatrixThread {
	/**
		* Exception thrown while executing the agent
		*/
	private Exception exception;
	/**
		* Response for the operation executed
		*/
	private OpResult response;
	/**
		* Logging Level - NOT USED
		*/
	public static final int filterLevel = 0; // Have to make lower filter level to see debug messages
	/**
		* Constructor
		* @param the step this agent is to perform
		* @param parent the flow that has created this agent
		*/
	public MatrixAgent(Step step, FlowThread parent, ThreadGroup group) {
		super(step, parent, group);
	}

	public Step getStep() {
		return (Step)getWrappedFlowItem();
	}

	/**
		* Get Flow ID
		* @return Flow ID
		*/
	public String getFlowId() {
		return this.getParentThread().getMatrixThreadId();
	}

	/**
		* @return name assigned to this step by the request
		*/
	public String getThreadName() {
		return getStep().getStepID();
	}

	public Operation getOperation() {
		return getStep().getOperation();
	}

	/**
		* Set Exception
		* @param e Exception
		*/
	public void setSRBException(Exception e) {
		exception = e;
	}

	/**
		* Get Exception
		* @return Exception
		*/
	public Exception getSRBException() {
		return exception;
	}

	/**
		* Set Response
		* @param res Operation/Step Result
		*/
	public void setResponse(OpResult res) {

										Status status = this.getStore().getStatus(this.getMatrixThreadId());
										status.setResult(res);
										this.getStore().setStatus(this.getMatrixThreadId(), status);
	}

	/**
		* Get Response
		* @return Operation/Step Result
		*/
	public OpResult getResponse() {
		return this.getStore().getStatus(this.getMatrixThreadId()).getResult();
	}


	/**
		* Get ObjectFactory
		* @return Object Factory
		*/
	public ObjectFactory getObjectFactory() {
		return MatrixUtil.getObjectFactory();
	}

	/**
		* Get Matrix Connection
		* @return Matrix Connection
		*/
	public MatrixConnection getMatrixConnection() {
		return ( (FlowThread) (getProgenitor())).getMatrixConnection();
	}

	public TVariables getInputVariables() throws JAXBException {
		return this.getStep().getVariables();
	}

	public List getUserDefinedRules() {
		return getStep().getUserDefinedRules();
	}

					public MatrixDatagridObject determineFilePath(DataSet dataSet)
					{
			StdDataSet stdDataSet = resolvePossibleReference(dataSet);

			if (stdDataSet != null)
			{
									// get the filename
									String fileName = evaluateExpressionToString(stdDataSet.getDataIdentifier());

									// get the directory
									String logicalLocation = "";
									if (stdDataSet.getLogicalLocation() != null)
											logicalLocation = evaluateExpressionToString(stdDataSet.getLogicalLocation())+"/";

											// the filePath is the directory/fileName
									String filePath = logicalLocation+fileName;

									return new MatrixDatagridObject(MatrixDatagridObject.GRIDTYPE_DATASET, filePath);
			}

							return null;
					}

	/**
		* Determines the type of grid object:Container/Collection/Data Set
		* @param gridObject JAXB Object which represents a Container,Collection or Data Set
		* @return Matrix Datagrid Object which contains the datagrid object details
		*/
	public MatrixDatagridObject determineFilePath(StdDatagridObject gridObject)
	{
			int gridDataType = -1;
			String filePath = null;
			StdCollection collection = null;
			Collection collectionAbstract = gridObject.getCollectionNow();
			if (collectionAbstract != null){
					collection = resolvePossibleReference(collectionAbstract);
			}
			Container container = gridObject.getContainerNow();
			DataSet dataSet = gridObject.getDataSetNow();

			try
			{
					if (collection != null)
					{
			// get the value of the
			String collectionName = collection.getCollectionName();
			filePath = evaluateExpressionToString(collectionName);
			gridDataType = MatrixDatagridObject.GRIDTYPE_COLLECTION;
					}
					else if (container != null)
					{
			filePath = evaluateExpressionToString(container.getContainerName());
			gridDataType = MatrixDatagridObject.GRIDTYPE_CONTAINER;
					}
					else if (dataSet != null)
					{
			return determineFilePath(dataSet);
					}
			}
			catch (ClassCastException cce)
			{
					throw new MatrixException("variable not of expected type", cce);
			}
			return (new MatrixDatagridObject(gridDataType, filePath));
	}

	/**
		* Determine the file path for a data set
		* @param gridDataSet Data Set Details
		* @return File Path to Data Set
		*/
	public String determineFilePath(StdDataSet gridDataSet) {
		if (gridDataSet != null) {

										String value = gridDataSet.getDataIdentifier();
										if(gridDataSet.getLogicalLocation() != null)
												value = gridDataSet.getLogicalLocation()+"/"+value;

			return evaluateExpressionToString(value);
		} else {
			return (null);
		}
	}


	/**
		* Handle Attachments
		* Retrieves the attachment required for an agent
		* @param contentId Content ID for Attachment
		* @return Attachment
		*/
	public Object getAttachment(String contentId) throws Exception {
		AttachmentPart currentAttachment = null;
		InputStream attachmentContent = null;
		String contentType = null;
		DataHandler contentDataHandler = null;
		try {
			toLog("MatrixAgent - getAttachment() - txnid.contentId: " +
					getProgenitor().getMatrixThreadId() + "." + contentId, MatrixUtil.DEBUG_LOG);
			currentAttachment =
					(AttachmentPart) getStore().getRequestAttachment(
					getProgenitor().getMatrixThreadId(), contentId);
			if (currentAttachment != null) {
				toLog(
						"MatrixAgent - getAttachment() - currentAttachment not NULL " +
						getProgenitor().getMatrixThreadId() + "." + contentId, MatrixUtil.DEBUG_LOG);
				contentType = currentAttachment.getContentType();
				toLog("MatrixAgent - getAttachment() - ContentType : " +
						contentType + ",ContentID:" + contentId,
						MatrixUtil.DEBUG_LOG);
				toLog("MatrixAgent - getAttachment() - getContentId: " +
						currentAttachment.getContentId() + ",ContentID:" +
						contentId, MatrixUtil.DEBUG_LOG);
				toLog(
						"MatrixAgent - getAttachment() - Content Id matches ContentID:" +
						contentId, MatrixUtil.DEBUG_LOG);
				contentDataHandler = currentAttachment.getDataHandler();
				toLog("MatrixAgent - getAttachment() - DataHandler Retrieved" +
						",ContentID:" + contentId, MatrixUtil.DEBUG_LOG);
				attachmentContent = contentDataHandler.getInputStream();
				toLog("MatrixAgent - getAttachment() - InputStream Retrieved" +
						",ContentID:" + contentId, MatrixUtil.DEBUG_LOG);
			} else {
				toLog("getAttachment() - attachmentItr NULL " +
						",TXNID.ContentID:" + getProgenitor().getMatrixThreadId() + "." +
						contentId, MatrixUtil.DEBUG_LOG);
			}
			return (attachmentContent);
		} catch (Exception e) {
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
	public Object createAttachment(String contentType,
										String attachmentLocation, String contentId) throws
			Exception {
		try {
			AttachmentPart ap = null;
			MessageFactory mf = MessageFactory.newInstance();
			SOAPMessage msg = mf.createMessage();
			FileDataSource ds = new FileDataSource(attachmentLocation);
			DataHandler dh = new DataHandler(ds);
			ap = msg.createAttachmentPart(dh);
			ap.setContentId(contentId);
			return (ap);
		} catch (SOAPException se) {
			throw new Exception("Cannot create Attachment for get operations." +
								se.toString());
		} catch (Exception e) {
			throw new Exception("Cannot create Attachment for get operations." +
								e.toString());
		}
	}





	/**
		* To Retrieve Actual Connection for Agent
		* @return Grid Connection to be used by Agent
		*/
	public GridConnection retrieveGridConnection()
	{
							toLog("MatrixAgent() - retrieveGridConnection() -BEGIN", MatrixUtil.DEBUG_LOG);

			MatrixConnection connection = ( (FlowThread) getParentThread()).getMatrixConnection();
			GridConnection gridConn = connection.getActiveGridConnection();

			toLog("MatrixAgent() - retrieveGridConnection() -END", MatrixUtil.DEBUG_LOG);
			return (gridConn);
	}

}
