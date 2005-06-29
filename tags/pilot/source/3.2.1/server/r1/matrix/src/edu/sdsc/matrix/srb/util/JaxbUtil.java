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



package edu.sdsc.matrix.srb.util;

import edu.sdsc.matrix.srb.core.*;
import javax.xml.bind.*;
import edu.sdsc.matrix.srb.parser.*;
import java.util.Calendar;

public class JaxbUtil {
    
    private static ObjectFactory objFactory = new ObjectFactory();
    private static final String TERMINATION_HANDLERS_NAME = "faulthandler";
    private static final String DOC_AUTHOR = "Matrix Server";
    private static final String DOC_VERSION = "1.0";
    private static final String DOC_DOCUMENTATION = "Matrix";
    private static final String DEFAULT_ID = "Matrix-demo";
    private static final String DEFAULT_PASSWORD = "******";
    private static final String DEFAULT_ORGANIZATION = "sdsc";
    private static final String DEFAULT_HOMEDIRECTORY = "/home/Matrix-demo.sdsc";
    private static final String DEFAULT_DSR = "sdsc-unix";
    private static final String DEFAULT_SERVER = "srb.sdsc.edu";
    private static final int DEFAULT_PORT = 6824;

    public static DocMeta createDocMeta() 
	throws JAXBException {
	DocMeta docMeta = objFactory.createDocMeta();
	docMeta.setDocAuthor(DOC_AUTHOR);
	docMeta.setDocOwner(createUserInfo());
	docMeta.setDocVersion(DOC_VERSION);
	docMeta.setDocumentation(DOC_DOCUMENTATION);
	return docMeta;
    }

    public static UserInfo createUserInfo() 
	throws JAXBException {
	UserInfo ui = objFactory.createUserInfo();
	ui.setUserID(DEFAULT_ID);
	ui.setChallengeResponse(DEFAULT_PASSWORD); // my userid is the same as my password... for now
	ui.setHomeDirectory(DEFAULT_HOMEDIRECTORY);
	ui.setDefaultStorageResource(DEFAULT_DSR);
	ui.setOrganization(createOrganizationInfo());
	return ui;
    }
    
	public static UserInfo createUserInfoWithTicket(String ticketId) 
	   throws JAXBException {
	   UserInfo ui = objFactory.createUserInfo();
	   GridTicketInfo gtInfo = objFactory.createGridTicketInfo();
	   TicketID ticketID = objFactory.createTicketID();
	   TicketID.TicketStringType ticketString = objFactory.createTicketIDTicketStringType();
	   
	   ticketString.setValue(ticketId);
	   ticketID.setTicketString(ticketString);
	   gtInfo.setTicketID(ticketID);
	   ui.setGridTicket(gtInfo);
	   return ui;
	   }
    
    
    public static UserInfo createUserInfo(String username, 
					  String password, 
					  String homeDirectory, 
					  String defaultStorageResource,
					  String organization)
	throws JAXBException {

	UserInfo ui = objFactory.createUserInfo();
	ui.setUserID(username);
	ui.setChallengeResponse(password);
	ui.setHomeDirectory(homeDirectory);
	ui.setDefaultStorageResource(defaultStorageResource);
	ui.setOrganization(createOrganizationInfo(organization));
	return ui;
    }

    public static OrganizationInfo createOrganizationInfo() 
	throws JAXBException {
	OrganizationInfo oi = objFactory.createOrganizationInfo();
	oi.setOrganizationName(DEFAULT_ORGANIZATION);
	return oi;
    }

    public static OrganizationInfo createOrganizationInfo(String organization) 
	throws JAXBException {
	OrganizationInfo oi = objFactory.createOrganizationInfo();
	oi.setOrganizationName(organization);
	return oi;
    }

    public static VirtualOrganizationInfo createVirtualOrganizationInfo() 
	throws JAXBException {
	VirtualOrganizationInfo voi = objFactory.createVirtualOrganizationInfo();
	voi.setServer(DEFAULT_SERVER);
	voi.setPort(DEFAULT_PORT);
	return voi;
    }

    public static VirtualOrganizationInfo createVirtualOrganizationInfo(String server, int port)
	throws JAXBException {

	VirtualOrganizationInfo voi = 
	    objFactory.createVirtualOrganizationInfo();
	voi.setServer(server);
	voi.setPort(port);
	return voi;
    }

    public static TerminationHandlers createTerminationHandlers() 
	throws JAXBException {
	TerminationHandlers fh = objFactory.createTerminationHandlers();
	fh.setName(TERMINATION_HANDLERS_NAME);
	fh.setTBA("TBA");
	return fh;
    }

    public static StepStatusResponse createErrorStepStatusResponse(String stepID)
	throws JAXBException {
	StepStatusResponse ssr = objFactory.createStepStatusResponse();
	ssr.setStepID(stepID);
	ssr.setStatusCode(MatrixCodes.STEP_NOT_FOUND_OR_NOT_STARTED);
	return ssr;
    }
    
	public static Operation createRenameOpForCollection(String collName,String newName)
	throws JAXBException
	{	
		Operation operation  = 	objFactory.createOperation();
		RenameOp renameOp	 = 	 objFactory.createRenameOp();
		RenameParams renameParams = objFactory.createRenameParams();

		StdDatagridObject stdDatagridObject = createStdDatagridObjectForCollection(collName);
		renameParams.setOldName(stdDatagridObject);
		renameParams.setNewName(newName);
		renameOp.setStdParams(renameParams);
		operation.setRenameOp(renameOp);

		return(operation);
	}
	
	public static Operation createRenameOpForDataSet(String dataSetName,String logicalLocation,String newName)
		throws JAXBException
	{	
		Operation operation  = 	objFactory.createOperation();
		RenameOp renameOp	 = 	 objFactory.createRenameOp();
		RenameParams renameParams = objFactory.createRenameParams();

		StdDatagridObject stdDatagridObject = createStdDatagridObjectForDataSet(dataSetName,logicalLocation);
		renameParams.setOldName(stdDatagridObject);
		renameParams.setNewName(newName);
		renameOp.setStdParams(renameParams);
		operation.setRenameOp(renameOp);

		return(operation);
	}
    
	public static Operation createDeleteOpForCollection(String collName)
	throws JAXBException
	{	
		Operation operation  = 	objFactory.createOperation();
		DeleteOp deleteOp	 = 	 objFactory.createDeleteOp();
		DeleteParams deleteParams = objFactory.createDeleteParams();
	
		StdDatagridObject stdDatagridObject = createStdDatagridObjectForCollection(collName);
		deleteParams.setGridObject(stdDatagridObject);
		deleteOp.setStdParams(deleteParams);
		operation.setDeleteOp(deleteOp);
	
		return(operation);
	}
	
	public static Operation createDeleteOpForDataSet(String dataIdentifier,String logicalLocation)
	throws JAXBException
	{	
		Operation operation  = 	objFactory.createOperation();
		DeleteOp deleteOp	 = 	 objFactory.createDeleteOp();
		DeleteParams deleteParams = objFactory.createDeleteParams();

		StdDatagridObject stdDatagridObject = createStdDatagridObjectForDataSet(dataIdentifier,logicalLocation);
		deleteParams.setGridObject(stdDatagridObject);
		deleteOp.setStdParams(deleteParams);
		operation.setDeleteOp(deleteOp);

		return(operation);
	}
	
    
	public static Operation createCreateOpForContainer(String contName)
	throws JAXBException
	{	
		Operation operation  = 	objFactory.createOperation();
		CreateOp createOp	 = 	 objFactory.createCreateOp();
		CreateParams createParams = objFactory.createCreateParams();
		
		StdDatagridObject stdDatagridObject = createStdDatagridObjectForContainer(contName);
		createParams.setDatagridObject(stdDatagridObject);
		createOp.setStdParams(createParams);
		operation.setCreateOp(createOp);
		
		return(operation);
	}
	
	public static Operation createCreateOpForCollection(String collName)
	throws JAXBException
	{	
		Operation operation  = 	objFactory.createOperation();
		CreateOp createOp	 = 	 objFactory.createCreateOp();
		CreateParams createParams = objFactory.createCreateParams();
	
		StdDatagridObject stdDatagridObject = createStdDatagridObjectForCollection(collName);
		createParams.setDatagridObject(stdDatagridObject);
		createOp.setStdParams(createParams);
		operation.setCreateOp(createOp);
	
		return(operation);
	}
	
	public static StdDatagridObject createStdDatagridObjectForContainer(String containerName)
	throws JAXBException
	{
		Container container = 	objFactory.createContainer();
		StdDatagridObject stdDatagridObject = objFactory.createStdDatagridObject();
		container.setContainerName(containerName);
		stdDatagridObject.setContainerNow(container);
		return(stdDatagridObject);
	}
	
	public static StdDatagridObject createStdDatagridObjectForCollection(String collectionName)
	throws JAXBException
	{
		Collection collection = 	objFactory.createCollection();
		Collection.StdCollectionType stdCollectionType = objFactory.createCollectionStdCollectionType();
		StdDatagridObject stdDatagridObject = objFactory.createStdDatagridObject();
		
		stdCollectionType.setCollectionName(collectionName);
		collection.setStdCollection(stdCollectionType);
		stdDatagridObject.setCollectionNow(collection);
		
		return(stdDatagridObject);
	}
	
	public static StdDatagridObject createStdDatagridObjectForDataSet(String dataIdentifier,String logicalLocation)
	throws JAXBException
	{
		StdDatagridObject stdDatagridObject = objFactory.createStdDatagridObject();	
		DataSet dataSet = objFactory.createDataSet();
		
		dataSet.setDataIdentifier(dataIdentifier);
		dataSet.setLogicalLocation(logicalLocation);
		stdDatagridObject.setDataSetNow(dataSet);
	
		return(stdDatagridObject);
	}
	
	public static Operation createIngestOp(String dataIdentifier,String locigalLocation,String contentId)
		throws JAXBException
	{	
		Operation operation  = 	objFactory.createOperation();
		IngestDataSetOp ingestOp = 	 objFactory.createIngestDataSetOp();
		IngestDataSetParams ingestParams = objFactory.createIngestDataSetParams();
		DataSet dataSet = objFactory.createDataSet();
		
		dataSet.setDataIdentifier(dataIdentifier);
		dataSet.setLogicalLocation(locigalLocation);
		ingestParams.setDataDestination(dataSet);
		ingestParams.setContentMIMEID(contentId);
			
		ingestOp.setStdParams(ingestParams);
		operation.setIngestOp(ingestOp);		
		return(operation);
	}
	
	
	public static Operation createPrepareTicketOp(UserInfo userInfo,VirtualOrganizationInfo voInfo)
			throws JAXBException
		{	
			Operation operation  = 	objFactory.createOperation();
			PrepareTicketOp prepareOp = objFactory.createPrepareTicketOp();
			PrepareTicketParams prepareTicketParams = objFactory.createPrepareTicketParams();
			
			Calendar startTime = Calendar.getInstance();
			Calendar endTime = Calendar.getInstance();
			
			endTime.set(Calendar.YEAR,2005);
			
			prepareTicketParams.setStartTime(startTime);
			prepareTicketParams.setEndTime(endTime);
			prepareTicketParams.setGridInfo(voInfo);
			prepareTicketParams.setParentUser(userInfo);
			prepareTicketParams.setRecursiveDistribution(false);
					
			prepareOp.setStdParams(prepareTicketParams);
			operation.setPrepareTicketOp(prepareOp);		
			return(operation);
		}
		
	public static Operation createMetatdataQueryOp(String queryString)
				throws JAXBException
			{	
				Operation operation  = 	objFactory.createOperation();
				MetadataQueryOp queryOp = objFactory.createMetadataQueryOp();
				MetadataQueryParams queryParams = objFactory.createMetadataQueryParams();  
				
			
				queryParams.setQueryRequestString(queryString);
					
				queryOp.setStdParams(queryParams);
				operation.setMetadataQueryOp(queryOp);		
				return(operation);
			}	
}
