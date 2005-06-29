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
	* IngestDataSetOp.java
	*
	* Created on July 8, 2003, 6:53 AM
	*/
package edu.sdsc.matrix.srb.client;

import java.io.*;
import java.util.*;
import javax.xml.soap.*;

/**
	* Part of the Matrix client API.
	* Contains methods to create IngestOp objects.
	*
	* A IngestOp object can use either StdParams or FlexParams,
	* but not both.
	* @author  Allen Ding - SDSC
	*/
public class IngestDataSetOp extends edu.sdsc.matrix.srb.parser.impl.IngestDataSetOpImpl {
	private ArrayList attachmentList = new ArrayList();

					public IngestDataSetOp(IngestDataSetParams stdParams)
					{
							this.setStdParams(stdParams);
							attachmentList.add(stdParams.getDataDestination());
					}

	/**
		* Creates a new IngestOp object and specifying a path to a file ingest.
		* This constructor takes care of construction of an IngestDataSetParams.
		* @param contentMIMEID MIMEID of the file to ingest.
		* @param destination Destination dataset.
		* @param file Path to file to ingest.
		*/
	public IngestDataSetOp(String contentMIMEID, DataSet destination,
								String filename) throws SOAPException {
		super.setStdParams(new IngestDataSetParams(contentMIMEID, destination));
		// Create a new file to be ingested
		File file = new File(filename);
		attachmentList.add(MatrixToolbox.createAttachment(file, contentMIMEID));
	}

	/**
		* Creates a new IngestOp object and specifying an array of bytes to
		* ingest.
		* This constructor takes care of construction of an IngestDataSetParams.
		* @param contentMIMEID MIMEID of the file to ingest.
		* @param destination Destination dataset.
		* @param bytes Bytes to ingest.
		*/
	public IngestDataSetOp(String contentMIMEID, DataSet destination,
								byte[] data) throws SOAPException, IOException {
		super.setStdParams(new IngestDataSetParams(contentMIMEID, destination));
		// Ingest the bytes using the Toolbox
		attachmentList.add(MatrixToolbox.createAttachment(data, contentMIMEID));
	}

	/**
		* Creates a new IngestOp object using IngestParams.
		* @param destination Destination dataset.
						* @param externalURL URL to ingest.
		*/
	public IngestDataSetOp(String externalURL, DataSet destination) {
		super.setStdParams(new IngestDataSetParams(destination, externalURL));
	}

	/**
		* Creates a new IngestOp object using flexParams.
		* @param flexParams Flex Parameters.
		*/
	public IngestDataSetOp(ParamList flexParams) {
		super.setFlexParams(flexParams);
	}

		/**
			* Allow the target file to be over written if already exists
			* @param overWrite true=overWrite;
			*/

	public void allowOverWrite(boolean overWrite){
		super.getStdParams().setOverWrite(overWrite);
	}

	/**
		* Makes the IngestOp use the specified IngestParams. Using this
		* method clobbers any flexParams paremeter.
		* @param stdParams A IngestParams.
		*/
	public void useStdParams(IngestDataSetParams stdParams) {
		super.setFlexParams(null);
		super.setStdParams(stdParams);
	}

	/**
		* Makes the IngestOp use the specified ParamList. Using this
		* method clobbers any stdParams parameter.
		* @param flexParams A ParamList.
		*/
	public void useFlexParams(ParamList flexParams) {
		super.setStdParams(null);
		super.setFlexParams(flexParams);
	}

	/**
		* Method for other classes in package to get a reference to the ArrayList of
		* attachments.
		*/
	protected ArrayList getAttachments() {
		return attachmentList;
	}
	/*protected void setAttachments(ArrayList*/
}
