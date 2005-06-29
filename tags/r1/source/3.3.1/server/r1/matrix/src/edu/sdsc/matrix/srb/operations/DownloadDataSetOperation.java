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
import javax.xml.soap.AttachmentPart;

import edu.sdsc.matrix.srb.core.SRBConnection;
import edu.sdsc.matrix.srb.core.flow.FlowThread;
import edu.sdsc.matrix.srb.parser.DownloadDataSetOp;
import edu.sdsc.matrix.srb.parser.DownloadDataSetParams;
import edu.sdsc.matrix.srb.parser.Step;
import edu.sdsc.matrix.srb.util.MatrixUtil;
import edu.sdsc.matrix.srb.parser.StdDataSet;

import edu.sdsc.grid.io.local.LocalFile;
import edu.sdsc.grid.io.srb.SRBFile;
import edu.sdsc.grid.io.srb.SRBFileSystem;
import org.globus.io.urlcopy.UrlCopy;
import org.globus.util.GlobusURL;

//05 Dec 2003 - Formating of code, external URL - Arun
//?? ??? 2003 - converted to 3.0 - Jon
//14 Aug 2003 - Changes to use file cache - Reena
/**
	* This class is responsible for handling the SRB Operation of
	* downloading a Data Set from SRB
	*
	* @author rmathew, jonw, Arun swaran Jagatheesan (arun@sdsc.edu)
	*
	*/
public class DownloadDataSetOperation extends MatrixAgent {
	public DownloadDataSetOperation(Step step, FlowThread parent,
									ThreadGroup group) {
		super(step, parent, group);
	}

	/**
		* The method that executes the steps for Downloading a data set from SRB
		*/
	public void typeSpecificExecute() throws Exception {
		//local variables
		DownloadDataSetOp downloadOprn = getOperation().getDownloadDataSetOp();
		DownloadDataSetParams downloadParams = downloadOprn.getStdParams();
		// Implementation for Operation
		String fileCacheLocation = MatrixUtil.getPropertyFromPropertiesManager(
					"jargon.filecache");
										StdDataSet stdDataSet = this.resolvePossibleReference(downloadParams.getDataSource());
		String filePathForSourceFile = determineFilePath(stdDataSet);
		String filePathForTargetAttachmentFile =
					evaluateExpressionToString(
					fileCacheLocation + File.separator + "tempfile" + getProgenitor().getMatrixThreadId() +
					getMatrixThreadId());
		SRBConnection srbConnection = (SRBConnection) retrieveGridConnection();
		SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
		toLog("DownloadDataSetOperation - run() -USED FROM CONNECTION",
					MatrixUtil.DEBUG_LOG);
		SRBFile srbFile = new SRBFile(srbFileSystem, filePathForSourceFile);
		LocalFile localFile = new LocalFile(filePathForTargetAttachmentFile);
		srbFile.copyTo(localFile, true);
		String downloadURL = downloadParams.getExternalURL();
		toLog("External URL for dowload= " + downloadURL, MatrixUtil.DEBUG_LOG);
		if (downloadURL == null) {
			String contentId = downloadParams.getContentMIMEID();
			AttachmentPart attachment = (AttachmentPart) createAttachment(
						"contentType", filePathForTargetAttachmentFile, contentId);
			getStore().saveResponseAttachment(getProgenitor().getMatrixThreadId(), attachment);
		} else {
			// check if this works for windows and unix, ..
			//@TODO add the ability to grab credetials and pass on
			GlobusURL from = new GlobusURL("file://" +
													filePathForTargetAttachmentFile);
			GlobusURL to = new GlobusURL(downloadURL);
			UrlCopy uc = new UrlCopy();
			uc.setSourceUrl(from);
			uc.setDestinationUrl(to);
			uc.copy();
			uc = null;
		}
		// clean-up code
		srbFileSystem = null; // For Performance
		//localFile.delete();
	} //run

	/**
		* Get Operation Name
		* @returns the name of the operation encapsulated by this agent
		*/
	public String getOperationName() {
		return "DownloadDataSetOperation";
	} //getOperationName
} //class
