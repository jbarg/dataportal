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

import java.io.InputStream;

import edu.sdsc.matrix.srb.core.MatrixException;
import edu.sdsc.matrix.srb.core.SRBConnection;
import edu.sdsc.matrix.srb.core.flow.FlowThread;
import edu.sdsc.matrix.srb.parser.SeekNWriteOp;
import edu.sdsc.matrix.srb.parser.SeekNWriteParams;
import edu.sdsc.matrix.srb.parser.Step;
import edu.sdsc.matrix.srb.util.MatrixUtil;

import edu.sdsc.grid.io.srb.SRBFile;
import edu.sdsc.grid.io.srb.SRBFileSystem;
import edu.sdsc.grid.io.srb.SRBRandomAccessFile;

/**
	* This class is responsible for handling the SRB Operation of
	* seeking and writing data into a Data Set in SRB
	*
	* @author rmathew
	*
	*/
public class SeekAndWriteOperation extends MatrixAgent {
	public SeekAndWriteOperation(Step step, FlowThread parent,
									ThreadGroup group) {
		super(step, parent, group);
	}

	/**
		* The method that executes the steps for seeking and writing data into a Data Set
		*/
	public void typeSpecificExecute() throws Exception {
		SeekNWriteOp seekWriteOprn = getOperation().getSeekNWriteOp();
		SeekNWriteParams seekWriteParams = seekWriteOprn.getStdParams();
		// Implementation for Operation
		String filePath = determineFilePath(this.resolvePossibleReference(seekWriteParams.getDataSet()));
		// get the offset
		Object offsetValue = evaluateExpressionToString(seekWriteParams.getOffset());
		long offset = 0;
		if (offsetValue == null) {
			throw new MatrixException("Null Value for offset variable");
		}
		if (offsetValue instanceof String) {
			offset = Long.parseLong( (String) offsetValue);
		} else {
			offset = ( (java.math.BigDecimal) offsetValue).longValue();
			// get the length
		}
		Object lengthValue = evaluateExpressionToString(seekWriteParams.getOffset());
		int length = 0;
		if (lengthValue == null) {
			throw new MatrixException("Null Value for offset variable");
		}
		if (lengthValue instanceof String) {
			length = Integer.parseInt( (String) lengthValue);
		} else {
			length = ( (java.math.BigDecimal) offsetValue).intValue();
		}
		toLog("SeekAndWriteOperation() - run() -offset:" + offset,
					MatrixUtil.DEBUG_LOG);
		toLog("SeekAndWriteOperation() - run() -length:" + length,
					MatrixUtil.DEBUG_LOG);
		//SRBAccount account = srbCreateSRBAccount();
		//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
		SRBConnection srbConnection = (SRBConnection) retrieveGridConnection();
		SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
		toLog("SeekAndWriteOperation - run() -USED FROM CONNECTION",
					MatrixUtil.DEBUG_LOG);
		SRBFile srbFile = new SRBFile(srbFileSystem, filePath);
		SRBRandomAccessFile srbAccessFile = new SRBRandomAccessFile(srbFile,
					"rw");
		// Create InputStream For Attachment
		String contentId = evaluateExpressionToString(seekWriteParams.
												getContentMIMEID());
		InputStream attachmentStream = (InputStream) getAttachment(contentId);
		toLog("SeekAndWriteOperation() - run() -Attachment I/P Stream",
					MatrixUtil.DEBUG_LOG);
		// Read From Temp File
		toLog("SeekAndWriteOperation() - run() -Read from Local File-BEGIN",
					MatrixUtil.DEBUG_LOG);
		byte[] buffer = new byte[length];
		int c = attachmentStream.read(buffer);
		toLog("SeekAndWriteOperation() - run() -Read from Local File-END",
					MatrixUtil.DEBUG_LOG);
		// Write to SRB File
		toLog("SeekAndWriteOperation() - run() -Write to SRB File-BEGIN",
					MatrixUtil.DEBUG_LOG);
		srbAccessFile.seek(offset);
		srbAccessFile.write(buffer);
		toLog("SeekAndWriteOperation() - run() -Write to SRB File-END:" +
					offset, MatrixUtil.DEBUG_LOG);
		//setSRBResponse(new Object());
		srbFileSystem = null; // For Performance
	}

	/**
		* Get Operation Name
		* @returns the name of the operation encapsulated by this agent
		*/
	public String getOperationName() {
		return "SeekAndWriteOperation";
	}
}
