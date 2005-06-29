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
	* MatrixConnection.java
	*
	* Modified Licence and formatting Dec 3, 2004 - Arun
	* Created on July 9, 2003, 6:58 AM
	*/
package edu.sdsc.matrix.srb.client;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.FileNotFoundException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Iterator;
// JAXB classes used
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;
import javax.xml.soap.AttachmentPart;
import javax.xml.soap.MessageFactory;
import javax.xml.soap.SOAPConnection;
import javax.xml.soap.SOAPConnectionFactory;
import javax.xml.soap.SOAPException;
import javax.xml.soap.SOAPMessage;
import javax.xml.soap.SOAPPart;
import javax.xml.transform.Source;
import javax.xml.transform.stream.StreamSource;

/**
	* <p> Matrix Connection Maker </p>
	* MatrixConnection allows a client to specify a matrix server to use, and to
	* send a DataGridRequest to that server.
	* It takes care of whatever JAXB and SOAP setup is needed when sending that request.
	* @author  Allen Ding - SDSC
	*/
public class MatrixConnection{
			private URL serverUrl;
			private SOAPConnection soapConnection;
			private SOAPMessage soapMsg;
			private SOAPPart soapPart;
			private static Marshaller marshaller;
			private static Unmarshaller unmarshaller;
			/**
				* Creates a MatrixConnection object using the specified server and port.
				* @param server Full Matrix server path to connect to.
				* The server path must be in this format "http://<host>:<port>/path" (e.g. http://localhost:8080/matrix)
				* @throws JAXBException If there was a problem creating a context of "edu.sdsc.matrix.srb.parser",
				* or it's associated marshaller/unmarshallers.
				* @throws MalformedURLException If the server path is not a valid URL.
				* @throws SOAPException If there was a problem setting up a soapConnection.
				*/
			public MatrixConnection(String server) throws JAXBException,
					MalformedURLException, SOAPException{
						setServer(server);
						soapConnection = SOAPConnectionFactory.newInstance().createConnection();
						soapMsg = MessageFactory.newInstance().createMessage();
						soapPart = soapMsg.getSOAPPart();
						JAXBContext jc = JAXBContext.newInstance("edu.sdsc.matrix.srb.parser");
						marshaller = jc.createMarshaller();
						unmarshaller = jc.createUnmarshaller();
			}

			/** Sets the server that the connection is talking to.
				* @param server The location of the matrix server
				* @throws MalformedURLException If the server path is not a valid URL.
				*/
			public void setServer(String server) throws MalformedURLException{
						serverUrl = new URL(server);
			}

			/**
				* Sends the DataGridRequest to the server the connection is associated to.
				* A DataGridResponse object is returned,
				* @param request The request to send.
				* @throws JAXBException If there was a problem related to marshallind the
				* DataGridRequest, or unmarshalling the DataGridResponse.
				* @throws SOAPException If there was a problem with SOAP (getting content, calling
				* the server, etc).
				* @return The DataGridResponse from the server.
				*/
			public DataGridResponse sendRequest(DataGridRequest request) throws
					JAXBException, SOAPException, FileNotFoundException{
						// Set up an input stream for SOAP
						ByteArrayOutputStream os = new ByteArrayOutputStream();
						marshaller.marshal(request, os);
						soapPart.setContent(new StreamSource(new ByteArrayInputStream(os.
								toByteArray())));
						// Add any attachments to the SOAP message if it was a Transaction
						if (request.getFlow() != null){
									ArrayList attachmentParts = request.getAttachments();
									// The ArrayList will never be null - see class client.Transaction
									for (Iterator i = attachmentParts.iterator(); i.hasNext(); ){
												soapMsg.addAttachmentPart((AttachmentPart) i.next());
									}
						}
						// Call the server with the SOAP wrapped request
						SOAPMessage soapReply = soapConnection.call(soapMsg, serverUrl);
						Source replyXML = soapReply.getSOAPPart().getContent();
						// Form the server response from reply Source
						DataGridResponse dgr =
								new DataGridResponse((edu.sdsc.matrix.srb.parser.DataGridResponse)
								unmarshaller.unmarshal(replyXML));
						return dgr;
			}
}