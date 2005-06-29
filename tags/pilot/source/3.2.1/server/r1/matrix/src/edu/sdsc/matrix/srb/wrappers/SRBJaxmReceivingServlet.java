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



package edu.sdsc.matrix.srb.wrappers;

import javax.xml.soap.*;
import javax.xml.messaging.*;
import javax.servlet.*;
import javax.servlet.http.*;
import javax.xml.transform.*;
import javax.naming.*;
import org.apache.commons.logging.*;
import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;
import java.io.*;
import java.awt.image.BufferedImage;
import javax.activation.*;
import javax.xml.bind.JAXBException;
import edu.sdsc.matrix.srb.core.SRBProcessor;

import javax.xml.transform.stream.*;

public class SRBJaxmReceivingServlet extends JAXMServlet implements ReqRespListener 
{

    static MessageFactory fac = null;
    static SRBProcessor srbProcessor = null;

    static 
    {
        try 
        {
            fac = MessageFactory.newInstance();
	    	srbProcessor = new SRBProcessor();
        } 
        catch (Exception ex) 
        {
            ex.printStackTrace();
        }
    }

    public void init(ServletConfig servletConfig) throws ServletException 
    {
        super.init(servletConfig);
    }

    public SOAPMessage onMessage(SOAPMessage requestmsg) 
    {
		try 
		{
	    	// pull out xml document and attachments from SOAPMessage
	   
	    	Source requestxml = requestmsg.getSOAPPart().getContent();
	    
	    	List responseAttachments = new ArrayList();
	    	Source responsexml = srbProcessor.process(requestxml,requestmsg.getAttachments(),responseAttachments);
								      
	    	if(responsexml == null) 
	    		return createReceiverSOAPFault("Internal server error");
		
			/*MimeHeaders mimeheader = new MimeHeaders();
			mimeheader.addHeader("Content-Type","text/xml");
			SOAPMessage responsemsg = fac.createMessage(mimeheader,null);
			*/

        	SOAPMessage responsemsg = fac.createMessage();
			SOAPPart sp = responsemsg.getSOAPPart();
				
			if(sp!=null)
			{
				// NOTE : COMMENTING THIS OUT MADE IT WORK IN TOMCAT
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
			}
			else
			{
				System.out.println("***** SRBJaxmReceiving Servlet : onMessage -SP null");
			}
			
			/*
			System.out.println("***** SRBJaxmReceiving Servlet : onMessage - 3 COMPLETE-ENV-"+sp.getEnvelope());    
	    	SOAPEnvelope env = sp.getEnvelope();
			System.out.println("***** SRBJaxmReceiving Servlet : onMessage - 4 COMPLETE-ENVHEADER-"+env.getHeader());
	    	SOAPHeader hdr = env.getHeader();
			System.out.println("***** SRBJaxmReceiving Servlet : onMessage - 5 COMPLETE-ENVBODY"+env.getBody());
	    	SOAPBody bdy = env.getBody();
			System.out.println("***** SRBJaxmReceiving Servlet : onMessage - 6 COMPLETE");
	    	*/
	    
	    	/*
	    	env.getBody()
		    .addChildElement(env.createName("NOW_IT_WORKS"))
		    .addTextNode("now_it_works");

	    	*/

		
	    	sp.setContent(responsexml);
		
	    	Iterator it = responseAttachments.iterator();
	    	while(it.hasNext()) 
	    	{
				responsemsg.addAttachmentPart((AttachmentPart)it.next());
	    	}
		
	    	return responsemsg;
    	} 
		catch(JAXBException e) 
		{
			// happens if client gave us a malformed request 
	    	return createSenderSOAPFault(e.toString()); 
		}
		catch(SOAPException e) 
		{
	    	e.printStackTrace(); 
	    	return null; // should never happen
		}
		catch(Exception e)
		{
			e.printStackTrace();
			return createSenderSOAPFault(e.toString());
		}
    }

    private SOAPMessage createSenderSOAPFault(String detail) 
    {
		System.out.println("creating sender soap fault...");
		return createSOAPFault("SOAP-ENV:Sender", "Invalid Input", detail);
    }
    
    private SOAPMessage createReceiverSOAPFault(String detail) 
    {
		System.out.println("creating receiver soap fault...");
		return createSOAPFault("SOAP-ENV:Receiver", "Internal Error", detail);
    }

    private SOAPMessage createSOAPFault(String faultCode, String faultString, String detail) 
    {
		try 
		{
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
		}
		catch(SOAPException e) 
		{
		    e.printStackTrace(); 
	    	return null; // should never happen
		}
    }

    public void doGet(javax.servlet.http.HttpServletRequest request, 
		              javax.servlet.http.HttpServletResponse response) 
	throws ServletException, IOException 
	{

		ServletOutputStream sOutputStream = response.getOutputStream();
		sOutputStream.println("<h1>You have successfully installed SRB Matrix.</h1>");
    }


    /*
    public void doPost(javax.servlet.http.HttpServletRequest request, 
		               javax.servlet.http.HttpServletResponse response) 
    throws ServletException, IOException 
	{

		System.out.println("post called");
		try 
		{
	    	//GetHeaders is defined in javax.xml.messaging.JAXMServlet to retrieve mimeHeaders from HttpServletRequest	
	    	//MimeHeaders include any SOAPAction, Content-Location, and Content-Type headers.	
	    	MimeHeaders mimeHeaders = getHeaders(request);        
	    	ServletInputStream sInputStream = request.getInputStream();        
	    	//Create the SOAPMessage from the reply        
	    	//msgFactory is a protected field of JAXMServlet        
	    	SOAPMessage soapMessage = msgFactory.createMessage(mimeHeaders,sInputStream);

	    	SOAPMessage soapMessageReply = null;	
	    	System.out.println("about to call onMessage");
	    	soapMessageReply = this.onMessage(soapMessage);        
	    	System.out.println("called onMessage. have soapMessageReply");
	    	if(soapMessageReply != null)
	    	{
				System.out.println("soapMessageReply is not null");
				if(soapMessageReply.saveRequired())
				{
		    		System.out.println("about to save changes");
		    		soapMessageReply.saveChanges();
				}
				System.out.println("about to check for presense of soap fault");
				
				//Check to see if presence of SOAPFault                
				if(soapMessageReply.getSOAPPart().getEnvelope().getBody().hasFault())
				{
		    		System.out.println("Setting HTTP status: 500");
		    		response.setStatus(500);                
				} 
				else 
				{
		    		System.out.println("Setting HTTP status: 200");
		    		response.setStatus(200);
				}
				System.out.println("done checking for soap faults");
	    	}

	    	//Send the response back to the sender        
	    	//by placing the mime headers into the response, and externalizing        
	    	//the soapmessage onto the response object.	
	    	//PutHeaders is a method from JAXMServlet.
	    	System.out.println("about to put on headers");
	    	putHeaders(soapMessageReply.getMimeHeaders(), response);        
	    	System.out.println("put on headers");
	    	ServletOutputStream sOutputStream = response.getOutputStream();
	    	System.out.println("got outputstream");
	    	soapMessageReply.writeTo(sOutputStream);	
	    	System.out.println("wrote to outputstream");
	    	sOutputStream.flush();
	    	System.out.println("flushed.");
		}
		catch(SOAPException e) 
		{
	    	System.out.println("Exception thrown");
	    	e.printStackTrace();
	    	throw new ServletException("JAXMServlet POST Failed: " + e.getMessage());
		}
    }
    */
    
	private static void printReply(SOAPMessage reply,String outputFileName) throws Exception 
	{
	
		TransformerFactory tFact = TransformerFactory.newInstance();
		Transformer transformer = tFact.newTransformer();
		Source src = reply.getSOAPPart().getContent();
		FileOutputStream tempReplyOS = new FileOutputStream(outputFileName);
		StreamResult result1 = new StreamResult( System.out );
		StreamResult result2 = new StreamResult( tempReplyOS );
		transformer.transform(src, result1);
		transformer.transform(src, result2);
		System.out.println();
	}
	
}

