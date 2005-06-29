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
package edu.sdsc.matrix.srb.operations;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.net.URL;

import edu.sdsc.matrix.srb.core.MatrixException;
import edu.sdsc.matrix.srb.core.SRBConnection;
import edu.sdsc.matrix.srb.core.flow.FlowThread;
import edu.sdsc.matrix.srb.parser.DataSet;
import edu.sdsc.matrix.srb.parser.StdDataSet;
import edu.sdsc.matrix.srb.parser.IngestDataSetOp;
import edu.sdsc.matrix.srb.parser.IngestDataSetParams;
import edu.sdsc.matrix.srb.parser.Step;
import edu.sdsc.matrix.srb.util.MatrixUtil;

import edu.sdsc.grid.io.local.LocalFile;
import edu.sdsc.grid.io.srb.SRBFile;
import edu.sdsc.grid.io.srb.SRBFileSystem;

// 14 Aug 2003 - Changes to use file cache - Reena
/**
	* This class is responsible for handling the SRB Operation of
	* ingesting a Data Set into SRB
	*
	* @author rmathew, Arun swaran Jagatheesan
	*
	*/
public class IngestDataSetOperation extends MatrixAgent{
			public IngestDataSetOperation(Step step, FlowThread parent,
					ThreadGroup group){
						super(step, parent, group);
			}

			/**
				* The method that executes the steps for Ingesting a Data Set into SRB
				*/
			public void typeSpecificExecute() throws Exception{
						IngestDataSetOp ingestOprn = getOperation().getIngestOp();
						IngestDataSetParams ingestParams = ingestOprn.getStdParams();
						DataSet dataSet = ingestParams.getDataDestination();
						StdDataSet ingestDataSet = this.resolvePossibleReference(dataSet);
						boolean overWrite = ingestParams.isOverWrite();
						SRBFile srbFile = null;
						SRBFileSystem srbFileSystem = null;
						byte[] buffer = new byte[100];
						LocalFile attachmentLocalFile = null;
						FileOutputStream attachment = null;
						String fileCacheLocation = MatrixUtil.getPropertyFromPropertiesManager(
								"jargon.filecache");
						String filePathForTargetFile = determineFilePath(ingestDataSet);
						toLog(
								"IngestDataSetOperation- srbCopyFromLocal() - filePathForTargetFile " +
								filePathForTargetFile, MatrixUtil.DEBUG_LOG);
						// Get from external URL
						if (ingestParams.getExternalURL() != null){
									toLog("External URL Ingest: " + ingestParams.getExternalURL(), 10);
									String urlValue = evaluateExpressionToString(ingestParams.
											getExternalURL());
									if (urlValue == null){
												throw new MatrixException("Cannot resolve variable for url");
									}
									// Create a temp filename
									String tempfile = urlValue;
									tempfile = tempfile.replace('.', 'd');
									tempfile = tempfile.replace(':', 'x');
									tempfile = tempfile.replace('/', 'x');
									tempfile = tempfile.replace('?', 'x');
									tempfile = tempfile.replace('&', 'x');
									tempfile = fileCacheLocation + File.separator + tempfile;
									// Create a stream to read the URL
									URL url = new URL(urlValue);
									BufferedInputStream bis = new BufferedInputStream(url.openStream());
									File file = new File(tempfile);
									FileOutputStream fos = new FileOutputStream(file);
									byte[] bytebuffer = new byte[4096];
									int length;
									while ((length = bis.read(bytebuffer)) != -1){
												fos.write(bytebuffer, 0, length);
									}
									bis.close();
									fos.close();
									toLog("External URL Ingest succesfully written to: " + tempfile, 10);
									SRBConnection srbConnection = (SRBConnection)
											retrieveGridConnection();
									srbFileSystem = srbConnection.getActiveConnection();
									toLog("IngestDataSetOperation-run() -USED FROM CONNECTION",
											MatrixUtil.DEBUG_LOG);
									srbFile = new SRBFile(srbFileSystem, filePathForTargetFile);
									if (srbFile.exists()){
												toLog(
														"IngestDataSetOperation-URL- TargetFile exists",
														MatrixUtil.DEBUG_LOG);
												if (overWrite){
															boolean returnValue = srbFile.delete();
															if (returnValue){
																		toLog(
																				"IngestDataSetOperation-URL- TargetFile existed and was deleted",
																				MatrixUtil.DEBUG_LOG);
																		srbFile.copyFrom(new LocalFile(tempfile), true);
															} else{
																		toLog(
																				"IngestDataSetOperation-URL- TargetFile could not be deleted",
																				MatrixUtil.ERROR_LOG);
																		srbFile.copyFrom(new LocalFile(tempfile), true);
															}
												} else{
												//TODO code here to say the file existed but was not overwritten
												}
									} else{
												srbFile.copyFrom(new LocalFile(tempfile), true);
									}
									file.delete();
									srbFileSystem = null; // For Performance
						}
						// Get from attachments
						else{
									// Changed by Reena Mathew on 27 August 2003
									// Added System.currentTimeMillis() to the name to ensure unique name
									String contentMIMEID = evaluateExpressionToString(ingestParams.
											getContentMIMEID());
									if (contentMIMEID == null){
												throw new MatrixException("Cannot resolve variable");
									}
									String filePathForSourceAttachmentFile = fileCacheLocation +
											File.separator + contentMIMEID +
											System.currentTimeMillis();
									toLog(
											"IngestDataSetOperation - srbCopyFrom() - filePathForSourceAttachmentFile" +
											filePathForSourceAttachmentFile, MatrixUtil.DEBUG_LOG);
									toLog(
											"IngestDataSetOperation - srbCopyFrom() - Content MIME ID" +
											contentMIMEID, MatrixUtil.DEBUG_LOG);
									// Write to target
									InputStream attachmentStream = (InputStream) getAttachment(
											contentMIMEID);
									if (attachmentStream != null){
												toLog(
														"IngestDataSetOperation -srbCopyFrom() - ATTACHMENT FOUND",
														MatrixUtil.DEBUG_LOG);
												attachmentLocalFile = new LocalFile(
														filePathForSourceAttachmentFile);
												attachment = new FileOutputStream(attachmentLocalFile.getFile());
												SRBConnection srbConnection = (SRBConnection)
														retrieveGridConnection();
												srbFileSystem = srbConnection.getActiveConnection();
												toLog("IngestDataSetOperation-run() -USED FROM CONNECTION",
														MatrixUtil.DEBUG_LOG);
												srbFile = new SRBFile(srbFileSystem, filePathForTargetFile);
												int c = 0;
												while ((c = attachmentStream.read(buffer)) != -1){
															attachment.write(buffer, 0, buffer.length);
												}
												toLog(
														"IngestDataSetOperation-srbCopyFromLocal() - file read and written to OS",
														MatrixUtil.DEBUG_LOG);
												if (srbFile.exists()){
															if (overWrite){
																		boolean returnValue = srbFile.delete();
																		if (returnValue){
																					toLog(
																							"IngestDataSetOperation-URL- TargetFile existed and was deleted",
																							MatrixUtil.DEBUG_LOG);
																					srbFile.copyFrom(attachmentLocalFile);
																		} else{
																					toLog(
																							"IngestDataSetOperation-URL- TargetFile could not be deleted",
																							MatrixUtil.ERROR_LOG);
																		}
															} else{
															//TODO code here to say the file existed but was not overwritten
															}
												} else{
															srbFile.copyFrom(attachmentLocalFile);
												}
												// This statement is required for the attachmentLocalFile to be deleted
												attachment.close();
												boolean isDeleted = attachmentLocalFile.delete();
												toLog("IngestDataSetOperation- srbCopyFrom() - isDeleted:" +
														isDeleted, MatrixUtil.DEBUG_LOG);
												srbFileSystem = null; // For Performance
									} else{
												throw new RuntimeException("No Attachment with content Id -" +
														contentMIMEID +
														" found for Operation");
									}
						}
						//setSRBResponse(new Object());
			}

			/**
				* Get Operation Name
				* @returns the name of the operation encapsulated by this agent
				*/
			public String getOperationName(){
						return "IngestDataSetOperation";
			}
}