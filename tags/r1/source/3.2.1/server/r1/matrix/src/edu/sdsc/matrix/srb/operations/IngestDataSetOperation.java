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

import edu.sdsc.grid.io.srb.SRBAccount;
import edu.sdsc.grid.io.srb.SRBFileSystem;
import edu.sdsc.grid.io.srb.SRBFile;
import edu.sdsc.grid.io.srb.SRBContainer;
import edu.sdsc.grid.io.srb.SRBException;
import edu.sdsc.grid.io.srb.SRBRandomAccessFile;
import edu.sdsc.grid.io.local.LocalFile;
import edu.sdsc.grid.io.GeneralFile;

import edu.sdsc.matrix.srb.parser.*;
import edu.sdsc.matrix.srb.core.SRBStore;
import edu.sdsc.matrix.srb.core.SRBConnection;

import java.io.*;
import java.util.Calendar;
import java.util.Iterator;
import java.util.ArrayList;
import javax.xml.soap.AttachmentPart;
import javax.xml.soap.SOAPMessage;
import javax.activation.DataHandler;
import javax.xml.soap.*;
import javax.activation.*;
import java.io.File;
import java.net.URL;

import edu.sdsc.matrix.srb.util.MatrixUtil;

// 14 Aug 2003 - Changes to use file cache - Reena 

/**
 * This class is responsible for handling the SRB Operation of  
 * ingesting a Data Set into SRB
 *  
 * @author rmathew
 * 
 */
public class IngestDataSetOperation extends MatrixAgent
{
	/**
	 * The method that executes the steps for Ingesting a Data Set into SRB
	 */
	public void run()
	{
		boolean stepAbort = false;
		try
		{
			
			toLog("IngestDataSetOperation - BEGIN",MatrixUtil.DEBUG_LOG);
			stepAbort = checkStepAbortFlag();
			if(!stepAbort)
			{		
				logStepBeginStatus();
						
				IngestDataSetOp ingestOprn = getOperation().getIngestOp();
				IngestDataSetParams ingestParams = ingestOprn.getStdParams();
				DataSet ingestDataSet = ingestParams.getDataDestination();
			
				SRBFile srbFile = null;
				SRBFileSystem srbFileSystem = null;
				byte[] buffer = new  byte[100];
				LocalFile attachmentLocalFile = null;
				FileOutputStream attachment = null; 
						
				String fileCacheLocation = MatrixUtil.getPropertyFromPropertiesManager("jargon.filecache");
						
				String filePathForTargetFile = determineFilePath(ingestDataSet);
				toLog("IngestDataSetOperation- srbCopyFromLocal() - filePathForTargetFile"+ filePathForTargetFile,MatrixUtil.DEBUG_LOG);
                                
                                // Get from external URL
                                if(ingestParams.getExternalURL() != null) {
                                    toLog("External URL Ingest: " + ingestParams.getExternalURL(), 3);

                                    // Create a temp filename
                                    String tempfile = ingestParams.getExternalURL();
                                    tempfile = tempfile.replace('.', 'd');
                                    tempfile = tempfile.replace(':', 'x');
                                    tempfile = tempfile.replace('/',  'x');
                                    tempfile = tempfile.replace('?', 'x');
                                    tempfile = tempfile.replace('&', 'x');
                                    tempfile = fileCacheLocation + File.separator + tempfile;

                                    // Create a stream to read the URL
                                    URL url = new URL(ingestParams.getExternalURL());
                                    BufferedInputStream bis = new BufferedInputStream(url.openStream());
                                    File file = new File(tempfile);
                                    FileOutputStream fos = new FileOutputStream(file);

                                    byte[] bytebuffer = new byte[4096];
                                    int length;

                                    while( (length = bis.read(bytebuffer)) != -1) {
                                        fos.write(bytebuffer,0, length);
                                    }
                                    bis.close();
                                    fos.close();

                                    toLog("External URL Ingest succesfully written to: " + tempfile, 3);
                                    
                                    SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
                                    srbFileSystem = srbConnection.getActiveConnection();
                                    toLog("IngestDataSetOperation-run() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);

                                    srbFile = new SRBFile( srbFileSystem, filePathForTargetFile );
                                    srbFile.copyFromLocal(tempfile);
                                    file.delete();
                                    srbFileSystem = null; // For Performance 
                                    
                                } 
                                // Get from attachments
                                else {
                                	
                                	// Changed by Reena Mathew on 27 August 2003
                                	// Added System.currentTimeMillis() to the name to ensure unique name
                                    String filePathForSourceAttachmentFile = fileCacheLocation + File.separator + ingestParams.getContentMIMEID()+System.currentTimeMillis();
                                    
									toLog("IngestDataSetOperation - srbCopyFromLocal() - filePathForSourceAttachmentFile"+ filePathForSourceAttachmentFile,MatrixUtil.DEBUG_LOG);
                                    toLog("IngestDataSetOperation - srbCopyFromLocal() - Content MIME ID"+ ingestParams.getContentMIMEID(),MatrixUtil.DEBUG_LOG);
                                    
                                    // Write to target
                                    String contId = ingestParams.getContentMIMEID();
                                    InputStream attachmentStream = (InputStream) getAttachment(contId);
                                    
                                    if(attachmentStream!=null) {
                                        toLog("IngestDataSetOperation -srbCopyFromLocal() - ATTACHMENT FOUND",MatrixUtil.DEBUG_LOG);
                                        attachmentLocalFile = new LocalFile(filePathForSourceAttachmentFile);
                                        attachment = new FileOutputStream(attachmentLocalFile.getFile());
                                        
                                        //SRBAccount account = srbCreateSRBAccount();
                                        //toLog("srbCopyFromLocal() - account"+ account,MatrixUtil.DEBUG_LOG);
                                        //srbFileSystem = new SRBFileSystem(account);
                                        
                                        SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
                                        srbFileSystem = srbConnection.getActiveConnection();
                                        toLog("IngestDataSetOperation-run() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
                                        
                                        
                                        srbFile = new SRBFile( srbFileSystem, filePathForTargetFile );
                                        
                                        int c=0;
                                        while((c= attachmentStream.read(buffer))!=-1) {
                                            attachment.write(buffer,0,buffer.length);
                                        }
                                        toLog("IngestDataSetOperation-srbCopyFromLocal() - file read and written to OS",MatrixUtil.DEBUG_LOG);
                                        
                                        
                                        srbFile.copyFromLocal(attachmentLocalFile);
                                        
                                        // This statement is required for the attachmentLocalFile to be deleted
                                        attachment.close();
                                        
                                        
                                        boolean isDeleted = attachmentLocalFile.delete();
                                        toLog("IngestDataSetOperation- srbCopyFromLocal() - isDeleted:"+isDeleted,MatrixUtil.DEBUG_LOG);
                                        srbFileSystem = null; // For Performance
                                    }
                                    else
                                        throw new Exception("No Attachment with content Id -"+contId+" found for Operation");
                                }
	
		
				//setSRBResponse(new Object());
			}
			else
			{
				logStepAbortStatus();
			}		
			toLog("IngestDataSetOperation - END",MatrixUtil.DEBUG_LOG);
			
		
		}
		catch(SRBException e)
		{
			String exceptionMessage = SRBException.getStandardMessage(e.getType());
			setSRBException(new Exception(exceptionMessage));	
		}
		catch(Exception e)
		{
			setSRBException(e);
		}
		finally
		{
			if(!stepAbort)
					logStepEndStatus();
		}
	}		
	
}