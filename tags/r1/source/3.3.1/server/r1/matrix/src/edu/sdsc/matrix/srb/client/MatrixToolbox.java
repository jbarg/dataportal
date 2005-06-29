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


/*
	* MatrixToolbox.java
	*
	* Created on July 10, 2003, 7:05 AM
	*/
package edu.sdsc.matrix.srb.client;

import java.io.File;
import javax.activation.DataHandler;
import javax.activation.DataSource;
import javax.activation.FileDataSource;
import javax.xml.soap.AttachmentPart;
import javax.xml.soap.MessageFactory;
import javax.xml.soap.SOAPException;
import javax.xml.soap.SOAPMessage;

/**
	* A few classes to help with attachments and stuff
	* @author  Allen Ding
	*/
public class MatrixToolbox {
	/**
		* Creates an attachment from a file.
		* Actually just creates a DataSource and gives the job to
		* constructAttachment.
		* Originally from reena's matrixagent :)
		*/
	protected static AttachmentPart createAttachment(File file,
														String contentID) throws
				SOAPException {
		FileDataSource fds = new FileDataSource(file);
		return constructAttachment(fds, contentID);
	}

	protected static AttachmentPart createAttachment(byte[] data,
														String contentID) throws
				SOAPException {
		ByteArrayDataSource bads = new ByteArrayDataSource(data, contentID);
		return constructAttachment(bads, contentID);
	}

	protected static AttachmentPart constructAttachment(DataSource ds,
														String contentID) throws
				SOAPException {
		MessageFactory mf = MessageFactory.newInstance();
		SOAPMessage soapMessage = mf.createMessage();
		DataHandler dataHandler = new DataHandler(ds);
		AttachmentPart ap = soapMessage.createAttachmentPart(dataHandler);
		ap.setContentId(contentID);
		return ap;
	}
}
