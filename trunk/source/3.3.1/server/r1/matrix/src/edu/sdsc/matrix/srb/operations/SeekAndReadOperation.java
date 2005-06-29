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

import java.io.File;
import java.io.FileOutputStream;
import javax.xml.soap.AttachmentPart;

import edu.sdsc.matrix.srb.core.MatrixException;
import edu.sdsc.matrix.srb.core.SRBConnection;
import edu.sdsc.matrix.srb.core.flow.FlowThread;
import edu.sdsc.matrix.srb.parser.SeekNReadOp;
import edu.sdsc.matrix.srb.parser.SeekNReadParams;
import edu.sdsc.matrix.srb.parser.Step;
import edu.sdsc.matrix.srb.util.MatrixUtil;

import edu.sdsc.grid.io.local.LocalFile;
import edu.sdsc.grid.io.srb.SRBFile;
import edu.sdsc.grid.io.srb.SRBFileSystem;
import edu.sdsc.grid.io.srb.SRBRandomAccessFile;

//14 Aug 2003 - Changes to use file cache - Reena
/**
	* This class is responsible for handling the SRB Operation of
	* seeking and reading data from a Data Set in SRB
	*
	* @author rmathew
	*
	*/
public class SeekAndReadOperation extends MatrixAgent {
	public SeekAndReadOperation(Step step, FlowThread parent, ThreadGroup group) {
		super(step, parent, group);
	}

	/**
		* The method that executes the steps for seeking and reading data from a Data Set
		*/
	public void typeSpecificExecute() throws Exception {
		SeekNReadOp seekReadOprn = getOperation().getSeekNReadOp();
		SeekNReadParams seekReadParams = seekReadOprn.getStdParams();
		String filePath = determineFilePath(resolvePossibleReference(seekReadParams.getDataSet()));
		// get the offset
		Object offsetValue = evaluateExpressionToString(seekReadParams.getOffset());
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
		Object lengthValue = evaluateExpressionToString(seekReadParams.getOffset());
		int length = 0;
		if (lengthValue == null) {
			throw new MatrixException("Null Value for offset variable");
		}
		if (lengthValue instanceof String) {
			length = Integer.parseInt( (String) lengthValue);
		} else {
			length = ( (java.math.BigDecimal) offsetValue).intValue();
		}
		toLog("SeekAndReadOperation() - run() -offset:" + offset,
					MatrixUtil.DEBUG_LOG);
		toLog("SeekAndReadOperation() - run() -length:" + length,
					MatrixUtil.DEBUG_LOG);
		//SRBAccount account = srbCreateSRBAccount();
		//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
		SRBConnection srbConnection = (SRBConnection) retrieveGridConnection();
		SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
		toLog("SeekAndReadOperation - run() -USED FROM CONNECTION",
					MatrixUtil.DEBUG_LOG);
		SRBFile srbFile = new SRBFile(srbFileSystem, filePath);
		SRBRandomAccessFile srbAccessFile = new SRBRandomAccessFile(srbFile,
					"rw");
		// Create temp local File
		String fileCacheLocation = MatrixUtil.getPropertyFromPropertiesManager(
					"jargon.filecache");
		String filePathForTargetAttachmentFile = fileCacheLocation +
					File.separator + "tempfile" + getProgenitor().getMatrixThreadId() + getMatrixThreadId();
		LocalFile localFile = new LocalFile(filePathForTargetAttachmentFile);
		toLog("SeekAndReadOperation() - run() -localFile:" +
					filePathForTargetAttachmentFile, MatrixUtil.DEBUG_LOG);
		toLog("SeekAndReadOperation() - run() -srbFilePath:" + filePath,
					MatrixUtil.DEBUG_LOG);
		// Read From SRB file
		toLog("SeekAndReadOperation() - run() -read from SRB- BEGIN",
					MatrixUtil.DEBUG_LOG);
		byte[] buffer = new byte[length];
		srbAccessFile.seek(offset);
		int c;
		c = srbAccessFile.read(buffer, 0, length);
		toLog("SeekAndReadOperation() - run() -read  from SRB- END",
					MatrixUtil.DEBUG_LOG);
		// Write to Local File
		toLog("SeekAndReadOperation() - run() -write to Local - BEGIN",
					MatrixUtil.DEBUG_LOG);
		FileOutputStream localFileOS = new FileOutputStream(localFile.getFile());
		localFileOS.write(buffer);
		localFileOS.close();
		toLog("SeekAndReadOperation() - run() -write to Local - END",
					MatrixUtil.DEBUG_LOG);
		// Create and Store Attachment
		String contentId = evaluateExpressionToString(seekReadParams.getContentMIMEID());
		AttachmentPart attachment = (AttachmentPart) createAttachment(
					"contentType", filePathForTargetAttachmentFile, contentId);
		getStore().saveResponseAttachment(getProgenitor().getMatrixThreadId(), attachment);
		//localFile.delete();
		toLog("SeekAndReadOperation() - run() -Attachment Created - END",
					MatrixUtil.DEBUG_LOG);
		// Implementation for Operation
		//setSRBResponse(new Object());
		srbFileSystem = null; // For Performance
	}

	/**
		* Get Operation Name
		* @returns the name of the operation encapsulated by this agent
		*/
	public String getOperationName() {
		return "SeekAndReadOperation";
	}
}
