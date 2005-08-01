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

package edu.sdsc.matrix.srb.util;

import java.util.Calendar;
import javax.xml.bind.JAXBException;

import edu.sdsc.matrix.srb.parser.Collection;
import edu.sdsc.matrix.srb.parser.StdCollection;
import edu.sdsc.matrix.srb.parser.Container;
import edu.sdsc.matrix.srb.parser.CreateOp;
import edu.sdsc.matrix.srb.parser.CreateParams;
import edu.sdsc.matrix.srb.parser.StdDataSet;
import edu.sdsc.matrix.srb.parser.DataSet;
import edu.sdsc.matrix.srb.parser.DeleteOp;
import edu.sdsc.matrix.srb.parser.DeleteParams;
import edu.sdsc.matrix.srb.parser.DocMeta;
import edu.sdsc.matrix.srb.parser.GridTicketInfo;
import edu.sdsc.matrix.srb.parser.IngestDataSetOp;
import edu.sdsc.matrix.srb.parser.IngestDataSetParams;
import edu.sdsc.matrix.srb.parser.MetadataQueryOp;
import edu.sdsc.matrix.srb.parser.MetadataQueryParams;
import edu.sdsc.matrix.srb.parser.ObjectFactory;
import edu.sdsc.matrix.srb.parser.Operation;
import edu.sdsc.matrix.srb.parser.OrganizationInfo;
import edu.sdsc.matrix.srb.parser.PrepareTicketOp;
import edu.sdsc.matrix.srb.parser.PrepareTicketParams;
import edu.sdsc.matrix.srb.parser.RenameOp;
import edu.sdsc.matrix.srb.parser.RenameParams;
import edu.sdsc.matrix.srb.parser.StdDatagridObject;
import edu.sdsc.matrix.srb.parser.TicketID;
import edu.sdsc.matrix.srb.parser.UserInfo;
import edu.sdsc.matrix.srb.parser.VirtualOrganizationInfo;

public class JaxbUtil {
	private static ObjectFactory objFactory = new ObjectFactory();
	private static final String TERMINATION_HANDLERS_NAME = "faulthandler";
	private static final String DOC_AUTHOR = "Matrix Server";
	private static final String DOC_VERSION = "1.0";
	private static final String DOC_DOCUMENTATION = "Matrix";
	private static final String DEFAULT_ID = "Matrix-demo";
	private static final String DEFAULT_PASSWORD = "******";
	private static final String DEFAULT_ORGANIZATION = "sdsc";
	private static final String DEFAULT_HOMEDIRECTORY =
				"/home/Matrix-demo.sdsc";
	private static final String DEFAULT_DSR = "sdsc-unix";
	private static final String DEFAULT_SERVER = "srb.sdsc.edu";
	private static final int DEFAULT_PORT = 6824;
	public static DocMeta createDocMeta() throws JAXBException {
		DocMeta docMeta = objFactory.createDocMeta();
		docMeta.setDocAuthor(DOC_AUTHOR);
		docMeta.setDocOwner(createUserInfo());
		docMeta.setDocVersion(DOC_VERSION);
		//docMeta.setDocumentation(DOC_DOCUMENTATION);
		return docMeta;
	}

	public static UserInfo createUserInfo() throws JAXBException {
		UserInfo ui = objFactory.createUserInfo();
		ui.setUserID(DEFAULT_ID);
		ui.setChallengeResponse(DEFAULT_PASSWORD);
		ui.setHomeDirectory(DEFAULT_HOMEDIRECTORY);
		ui.setDefaultStorageResource(DEFAULT_DSR);
		ui.setOrganization(createOrganizationInfo());
		return ui;
	}

	public static UserInfo createUserInfoWithTicket(String ticketId) throws
				JAXBException {
		UserInfo ui = objFactory.createUserInfo();
		GridTicketInfo gtInfo = objFactory.createGridTicketInfo();
		TicketID ticketIDObj = objFactory.createTicketID();
//		TicketID.TicketStringType ticketString = objFactory.
//					createTicketIDTicketStringType();
//		ticketString.setValue(ticketId);
		ticketIDObj.setTicketString(ticketId);
		gtInfo.setTicketID(ticketIDObj);
		ui.setGridTicket(gtInfo);
		return ui;
	}

	public static UserInfo createUserInfo(String username,
												String password,
												String homeDirectory,
												String defaultStorageResource,
												String organization) throws
				JAXBException {
		UserInfo ui = objFactory.createUserInfo();
		ui.setUserID(username);
		ui.setChallengeResponse(password);
		ui.setHomeDirectory(homeDirectory);
		ui.setDefaultStorageResource(defaultStorageResource);
		ui.setOrganization(createOrganizationInfo(organization));
		return ui;
	}

	public static OrganizationInfo createOrganizationInfo() throws
				JAXBException {
		OrganizationInfo oi = objFactory.createOrganizationInfo();
		oi.setOrganizationName(DEFAULT_ORGANIZATION);
		return oi;
	}

	public static OrganizationInfo createOrganizationInfo(String organization) throws
				JAXBException {
		OrganizationInfo oi = objFactory.createOrganizationInfo();
		oi.setOrganizationName(organization);
		return oi;
	}

	public static VirtualOrganizationInfo createVirtualOrganizationInfo() throws
				JAXBException {
		VirtualOrganizationInfo voi = objFactory.createVirtualOrganizationInfo();
		voi.setServer(DEFAULT_SERVER);
		voi.setPort(DEFAULT_PORT);
		return voi;
	}

	public static VirtualOrganizationInfo createVirtualOrganizationInfo(String
				server, int port) throws JAXBException {
		VirtualOrganizationInfo voi =
					objFactory.createVirtualOrganizationInfo();
		voi.setServer(server);
		voi.setPort(port);
		return voi;
	}

	/* arun nove-16
		public static StepStatusResponse createErrorStepStatusResponse(String stepID)
		throws JAXBException {
		StepStatusResponse ssr = objFactory.createStepStatusResponse();
		ssr.setStepID(stepID);
		ssr.setStatusCode(MatrixCodes.STEP_NOT_FOUND_OR_NOT_STARTED);
		return ssr;
		}
		*/
	public static Operation createRenameOpForCollection(String collName,
														String newName) throws
				JAXBException {
		Operation operation = objFactory.createOperation();
		RenameOp renameOp = objFactory.createRenameOp();
		RenameParams renameParams = objFactory.createRenameParams();
		StdDatagridObject stdDatagridObject =
					createStdDatagridObjectForCollection(collName);
		renameParams.setOldName(stdDatagridObject);
		renameParams.setNewName(newName);
		renameOp.setStdParams(renameParams);
		operation.setRenameOp(renameOp);
		return (operation);
	}

	public static Operation createRenameOpForDataSet(String dataSetName,
														String logicalLocation,
														String newName) throws
				JAXBException {
		Operation operation = objFactory.createOperation();
		RenameOp renameOp = objFactory.createRenameOp();
		RenameParams renameParams = objFactory.createRenameParams();
		StdDatagridObject stdDatagridObject = createStdDatagridObjectForDataSet(
					dataSetName, logicalLocation);
		renameParams.setOldName(stdDatagridObject);
		renameParams.setNewName(newName);
		renameOp.setStdParams(renameParams);
		operation.setRenameOp(renameOp);
		return (operation);
	}

	public static Operation createDeleteOpForCollection(String collName) throws
				JAXBException {
		Operation operation = objFactory.createOperation();
		DeleteOp deleteOp = objFactory.createDeleteOp();
		DeleteParams deleteParams = objFactory.createDeleteParams();
		StdDatagridObject stdDatagridObject =
					createStdDatagridObjectForCollection(collName);
		deleteParams.setGridObject(stdDatagridObject);
		deleteOp.setStdParams(deleteParams);
		operation.setDeleteOp(deleteOp);
		return (operation);
	}

	public static Operation createDeleteOpForDataSet(String dataIdentifier,
														String logicalLocation) throws
				JAXBException {
		Operation operation = objFactory.createOperation();
		DeleteOp deleteOp = objFactory.createDeleteOp();
		DeleteParams deleteParams = objFactory.createDeleteParams();
		StdDatagridObject stdDatagridObject = createStdDatagridObjectForDataSet(
					dataIdentifier, logicalLocation);
		deleteParams.setGridObject(stdDatagridObject);
		deleteOp.setStdParams(deleteParams);
		operation.setDeleteOp(deleteOp);
		return (operation);
	}

	public static Operation createCreateOpForContainer(String contName) throws
				JAXBException {
		Operation operation = objFactory.createOperation();
		CreateOp createOp = objFactory.createCreateOp();
		CreateParams createParams = objFactory.createCreateParams();
		StdDatagridObject stdDatagridObject =
					createStdDatagridObjectForContainer(contName);
		createParams.setDatagridObject(stdDatagridObject);
		createOp.setStdParams(createParams);
		operation.setCreateOp(createOp);
		return (operation);
	}

	public static Operation createCreateOpForCollection(String collName) throws
				JAXBException {
		Operation operation = objFactory.createOperation();
		CreateOp createOp = objFactory.createCreateOp();
		CreateParams createParams = objFactory.createCreateParams();
		StdDatagridObject stdDatagridObject =
					createStdDatagridObjectForCollection(collName);
		createParams.setDatagridObject(stdDatagridObject);
		createOp.setStdParams(createParams);
		operation.setCreateOp(createOp);
		return (operation);
	}

	public static StdDatagridObject createStdDatagridObjectForContainer(String
				containerName) throws JAXBException {
		Container container = objFactory.createContainer();
		StdDatagridObject stdDatagridObject = objFactory.
					createStdDatagridObject();
		container.setContainerName(containerName);
		stdDatagridObject.setContainerNow(container);
		return (stdDatagridObject);
	}

	public static StdDatagridObject createStdDatagridObjectForCollection(String collectionName) throws JAXBException {
		Collection collection = objFactory.createCollection();

										StdCollection stdCollection = objFactory.createStdCollection();
		StdDatagridObject stdDatagridObject = objFactory.createStdDatagridObject();
		stdCollection.setCollectionName(collectionName);
		collection.setStdCollection(stdCollection);

		stdDatagridObject.setCollectionNow(collection);
		return (stdDatagridObject);
	}

	public static StdDatagridObject createStdDatagridObjectForDataSet(String
				dataIdentifier, String logicalLocation) throws JAXBException {
		StdDatagridObject stdDatagridObject = objFactory.createStdDatagridObject();
		StdDataSet dataSet = objFactory.createStdDataSet();
		dataSet.setDataIdentifier(dataIdentifier);
		dataSet.setLogicalLocation(logicalLocation);

										DataSet dataSetAbstract = objFactory.createDataSet();
										dataSetAbstract.setStdDataSet(dataSet);

		stdDatagridObject.setDataSetNow(dataSetAbstract);
		return (stdDatagridObject);
	}

	public static Operation createIngestOp(String dataIdentifier,
													String locigalLocation,
													String contentId) throws
				JAXBException {
		Operation operation = objFactory.createOperation();
		IngestDataSetOp ingestOp = objFactory.createIngestDataSetOp();
		IngestDataSetParams ingestParams = objFactory.createIngestDataSetParams();

		StdDataSet dataSet = objFactory.createStdDataSet();
		dataSet.setDataIdentifier(dataIdentifier);
		dataSet.setLogicalLocation(locigalLocation);

										DataSet dataSetAbstract = objFactory.createDataSet();
										dataSetAbstract.setStdDataSet(dataSet);

										ingestParams.setDataDestination(dataSetAbstract);
		ingestParams.setContentMIMEID(contentId);
		ingestOp.setStdParams(ingestParams);
		operation.setIngestOp(ingestOp);
		return (operation);
	}

	public static Operation createPrepareTicketOp(UserInfo userInfo,
														VirtualOrganizationInfo
														voInfo) throws JAXBException {
		Operation operation = objFactory.createOperation();
		PrepareTicketOp prepareOp = objFactory.createPrepareTicketOp();
		PrepareTicketParams prepareTicketParams = objFactory.
					createPrepareTicketParams();
		Calendar startTime = Calendar.getInstance();
//		Calendar endTime = Calendar.getInstance();
//		endTime.set(Calendar.YEAR, 2005);
		prepareTicketParams.setStartTime(startTime);
//		prepareTicketParams.setEndTime(endTime);
		prepareTicketParams.setGridInfo(voInfo);
		prepareTicketParams.setParentUser(userInfo);
		prepareOp.setStdParams(prepareTicketParams);
		operation.setPrepareTicketOp(prepareOp);
		return (operation);
	}

	public static Operation createMetatdataQueryOp(String queryString) throws
				JAXBException {
		Operation operation = objFactory.createOperation();
		MetadataQueryOp queryOp = objFactory.createMetadataQueryOp();
		MetadataQueryParams queryParams = objFactory.createMetadataQueryParams();
		queryParams.setQueryRequestString(queryString);
		queryOp.setStdParams(queryParams);
		operation.setMetadataQueryOp(queryOp);
		return (operation);
	}
}
