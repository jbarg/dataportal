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

package edu.sdsc.matrix.srb.wrappers;

import java.io.FileOutputStream;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import javax.servlet.ServletConfig;
import javax.servlet.ServletException;
import javax.servlet.ServletOutputStream;
import javax.xml.bind.JAXBException;
import javax.xml.messaging.JAXMServlet;
import javax.xml.messaging.ReqRespListener;
import javax.xml.soap.AttachmentPart;
import javax.xml.soap.MessageFactory;
import javax.xml.soap.SOAPBody;
import javax.xml.soap.SOAPConnection;
import javax.xml.soap.SOAPConnectionFactory;
import javax.xml.soap.SOAPEnvelope;
import javax.xml.soap.SOAPException;
import javax.xml.soap.SOAPFault;
import javax.xml.soap.SOAPHeader;
import javax.xml.soap.SOAPMessage;
import javax.xml.soap.SOAPPart;
import javax.xml.transform.Source;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.stream.StreamResult;

import edu.sdsc.matrix.srb.core.MatrixRequestHandler;

/**
	*
	* @author Erik Vandekieft, Reena Mathew, Arun swaran Jagatheesan
	*
	*/
public class SRBJaxmReceivingServlet extends JAXMServlet implements
		ReqRespListener {
	//TODO change this class name to MatrixJaxmReceivingSevlet - arun
	static MessageFactory fac = null;
	static MatrixRequestHandler srbProcessor = null;
	static {
			print("Creating JAXM servlet .");
		try {
			print("Creating SOAP Factory..");
			SOAPConnectionFactory scf = SOAPConnectionFactory.newInstance();
			print("Creating SOAP Connection..");
			SOAPConnection connection = scf.createConnection();
			print("Creating Message Factory..");
			fac = MessageFactory.newInstance();
			print("Creating Matrix Request Handler...");
			srbProcessor = new MatrixRequestHandler();
		} catch (Exception ex) {
			print("FATAL ERROR IN Matrix Servlet Creation");
			ex.printStackTrace(System.out);
			ex.printStackTrace(System.err);
		}
	}
	//for debug - dont use toLog
	public static void print(String msg){
		System.out.println(msg);
	}

	public void init(ServletConfig servletConfig) throws ServletException {
		print("Starting Matrix");
		super.init(servletConfig);
		print("creating JAXM servlet succeded");
	}

	public SOAPMessage onMessage(SOAPMessage requestmsg) {
		try {
			// pull out xml document and attachments from SOAPMessage
			Source requestxml = requestmsg.getSOAPPart().getContent();
			List responseAttachments = new ArrayList();
			// process the request
			Source responsexml = srbProcessor.process(requestxml,
														requestmsg.getAttachments(),
														responseAttachments);
			if (responsexml == null) {
				return createReceiverSOAPFault("Matrix:Internal Server Error");
			}
			/*
				MimeHeaders mimeheader = new MimeHeaders();
				mimeheader.addHeader("Content-Type","text/xml");
				SOAPMessage responsemsg = fac.createMessage(mimeheader,null);
				*/
			if (fac==null){
						print("Creating SOAP Factory..");
						SOAPConnectionFactory scf = SOAPConnectionFactory.newInstance();
						print("Creating SOAP Connection..");
						SOAPConnection connection = scf.createConnection();
						print("Creating Message Factory..");
						fac = MessageFactory.newInstance();
			}
			SOAPMessage responsemsg = fac.createMessage();
			SOAPPart sp = responsemsg.getSOAPPart();
			if (sp != null) {
			// NOTE : COMMENTING THIS OUT MADE IT WORK IN TOMCAT
			// STRANGE??
			/*
					SOAPEnvelope env = sp.getEnvelope();
					System.out.println("***** SRBJaxmReceiving Servlet : onMessage - 3 COMPLETE-ENV-"+env);
					if(env!=null)
					{
				SOAPHeader hdr = env.getHeader();
				System.out.println("***** SRBJaxmReceiving Servlet : onMessage - 4 COMPLETE-ENVHEADER-"+hdr);
				SOAPBody bdy = env.getBody();
				System.out.println("***** SRBJaxmReceiving Servlet : onMessage - 5 COMPLETE-ENVBODY"+bdy);
					}
					else
					{
				System.out.println("***** SRBJaxmReceiving Servlet : onMessage -ENV null");
					}
				*/
			} else {
				//just leave this sys message for now
				System.err.println(
						"MATRIX: ***** SRBJaxmReceiving Servlet : onMessage -SP null");
			}
			sp.setContent(responsexml);
			Iterator it = responseAttachments.iterator();
			while (it.hasNext()) {
				responsemsg.addAttachmentPart( (AttachmentPart) it.next());
			}
			return responsemsg;
		} catch (JAXBException e) {
			// happens if client gave us a malformed request
			return createSenderSOAPFault(e.toString());
		} catch (SOAPException e) {
			e.printStackTrace();
			return null; // should never happen
		} catch (Exception e) {
			e.printStackTrace();
			return createSenderSOAPFault(e.toString());
		}
	}

	private SOAPMessage createSenderSOAPFault(String detail) {
		System.out.println("creating sender soap fault...");
		return createSOAPFault("SOAP-ENV:Sender", "Invalid Input", detail);
	}

	private SOAPMessage createReceiverSOAPFault(String detail) {
		System.out.println("creating receiver soap fault...");
		return createSOAPFault("SOAP-ENV:Receiver", "Internal Error", detail);
	}

	private SOAPMessage createSOAPFault(String faultCode, String faultString,
										String detail) {
		try {
			SOAPMessage responsemsg = fac.createMessage();
			SOAPPart sp = responsemsg.getSOAPPart();
			SOAPEnvelope env = sp.getEnvelope();
			SOAPHeader hdr = env.getHeader();
			SOAPBody bdy = env.getBody();
			SOAPFault sf = bdy.addFault();
			sf.setFaultCode(faultCode);
			sf.setFaultString(faultString + ": " + detail);
			responsemsg.saveChanges();
			return responsemsg;
		} catch (SOAPException e) {
			e.printStackTrace();
			return null; // should never happen
		}
	}

	public void doGet(javax.servlet.http.HttpServletRequest request,
						javax.servlet.http.HttpServletResponse response) throws
			ServletException, IOException {
		ServletOutputStream sOutputStream = response.getOutputStream();
		sOutputStream.println("<html><center><h1>You have successfully installed SDSC Matrix.</h1></center></html>");
	}

	private static void printReply(SOAPMessage reply, String outputFileName) throws
			Exception {
		TransformerFactory tFact = TransformerFactory.newInstance();
		Transformer transformer = tFact.newTransformer();
		Source src = reply.getSOAPPart().getContent();
		FileOutputStream tempReplyOS = new FileOutputStream(outputFileName);
		StreamResult result1 = new StreamResult(System.out);
		StreamResult result2 = new StreamResult(tempReplyOS);
		transformer.transform(src, result1);
		transformer.transform(src, result2);
		System.out.println();
	}
}
