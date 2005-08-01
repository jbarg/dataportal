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
	* SeekNWriteOp.java
	*
	* Created on July 9, 2003, 4:11 AM
	*/
package edu.sdsc.matrix.srb.client;

import java.io.*;
import java.util.*;
import javax.xml.soap.*;

/**
	* Part of the Matrix client API.
	* Contains methods to create SeekNWriteOp objects.
	*
	* A SeekNWriteOp object can use either StdParams or FlexParams,
	* but not both.
	* @author  Allen Ding - SDSC
	*/
public class SeekNWriteOp extends edu.sdsc.matrix.srb.parser.impl.
			SeekNWriteOpImpl {
	ArrayList attachmentList;
	/**
		* Creates a new SeekNWriteOp object specifying the standard parameters
		* for the operation and a path to file to write from.
		* This constructor takes care of construction of an SeekNWriteParams.
		* @param stdParams SeekNWrite Parameters.
		*/
	public SeekNWriteOp(DataSet dataSet,
						String offset,
						String length,
						String filename,
						String contentMIMEID) throws SOAPException {
		attachmentList = new ArrayList();
		super.setStdParams(new SeekNWriteParams(dataSet, offset, length));
		super.getStdParams().setContentMIMEID(contentMIMEID);
		File file = new File(filename);
		attachmentList.add(MatrixToolbox.createAttachment(file, contentMIMEID));
	}

	/**
		* Creates a new SeekNWriteOp object specifying the standard parameters
		* for the operation and an array of bytes to write.
		* This constructor takes care of construction of an SeekNWriteParams.
		* @param stdParams SeekNWrite Parameters.
		*/
	public SeekNWriteOp(DataSet dataSet,
						String offset,
						String length,
						byte[] data,
						String contentMIMEID) throws SOAPException, IOException {
		attachmentList = new ArrayList();
		// create attachment for the new file
		super.setStdParams(new SeekNWriteParams(dataSet, offset, length));
		super.getStdParams().setContentMIMEID(contentMIMEID);
		attachmentList.add(MatrixToolbox.createAttachment(data, contentMIMEID));
	}

	/**
		* Creates a new SeekNWriteOp object using flexParams.
		* @param flexParams Flex Parameters.
		*/
	public SeekNWriteOp(ParamList flexParams) {
		super.setFlexParams(flexParams);
	}

	/**
		* Makes the SeekNWriteOp use the specified SeekNWriteParams. Using this
		* method clobbers any flexParams paremeter.
		* @param stdParams A SeekNWriteParams.
		*/
	public void useStdParams(SeekNWriteParams stdParams) {
		super.setFlexParams(null);
		super.setStdParams(stdParams);
	}

	/**
		* Makes the SeekNWriteOp use the specified ParamList. Using this
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
}