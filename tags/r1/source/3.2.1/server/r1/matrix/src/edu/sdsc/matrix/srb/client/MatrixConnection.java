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


/*
 * MatrixConnection.java
 *
 * Created on July 9, 2003, 6:58 AM
 */

package edu.sdsc.matrix.srb.client;

import java.io.*;
import java.net.*;
import java.util.*;
import javax.xml.soap.*;
import javax.xml.transform.*;
import javax.xml.transform.stream.*;

// JAXB classes used
import javax.xml.bind.JAXBContext;
import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;
import javax.xml.bind.JAXBException;


/**
 * Part of the Matrix client API.
 * MatrixConnection allows a client to specify a matrix server to use, and to
 * send a DataGridRequest to that server.
 * It takes care of whatever JAXB and SOAP setup is needed when sending that request.
 * @author  Allen Ding - SDSC
 */
public class MatrixConnection {
    
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
    public MatrixConnection(String server) throws JAXBException, MalformedURLException, SOAPException {
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
    public void setServer(String server) throws MalformedURLException {
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
    public DataGridResponse sendRequest(DataGridRequest request) throws JAXBException, SOAPException, FileNotFoundException {
        // Set up an input stream for SOAP
        ByteArrayOutputStream os = new ByteArrayOutputStream();
        marshaller.marshal(request, os);
        soapPart.setContent(new StreamSource(new ByteArrayInputStream(os.toByteArray())));

        // Add any attachments to the SOAP message if it was a Transaction
        if(request.getTransaction() != null) {
            ArrayList attachmentParts = request.getAttachments();
            
            // The ArrayList will never be null - see class client.Transaction
            for(Iterator i = attachmentParts.iterator(); i.hasNext();) {
                soapMsg.addAttachmentPart((AttachmentPart)i.next());
            }
        }

        // Call the server with the SOAP wrapped request
        SOAPMessage soapReply = soapConnection.call(soapMsg, serverUrl);
        Source replyXML = soapReply.getSOAPPart().getContent();
        
        // Form the server response from reply Source
        DataGridResponse dgr =
            new DataGridResponse((edu.sdsc.matrix.srb.parser.DataGridResponse)unmarshaller.unmarshal(replyXML));
        
        return dgr;
    }
    
}
