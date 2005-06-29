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


import edu.sdsc.grid.io.srb.SRBException;
import edu.sdsc.grid.io.srb.SRBAccount;
import edu.sdsc.grid.io.srb.SRBFileSystem;
import edu.sdsc.grid.io.srb.SRBFile;
import edu.sdsc.grid.io.srb.SRBContainer;
import edu.sdsc.grid.io.srb.SRBRandomAccessFile;
import edu.sdsc.grid.io.local.LocalFile;

import edu.sdsc.matrix.srb.parser.*;
import edu.sdsc.matrix.srb.core.SRBConnection;

import javax.xml.soap.AttachmentPart;

import java.io.FileOutputStream;

import edu.sdsc.matrix.srb.util.MatrixUtil;
import java.io.File;

//14 Aug 2003 - Changes to use file cache - Reena

/**
 * This class is responsible for handling the SRB Operation of  
 * seeking and reading data from a Data Set in SRB
 *  
 * @author rmathew
 * 
 */
public class SeekAndReadOperation extends MatrixAgent
{
	/**
	 * The method that executes the steps for seeking and reading data from a Data Set
	 */
	public void run()
	{
		boolean stepAbort = false;
		try
		{
			toLog("SeekAndReadOperation() - run() -BEGIN",MatrixUtil.DEBUG_LOG);
			stepAbort = checkStepAbortFlag();
			if(!stepAbort)
			{	
				logStepBeginStatus();
						
				SeekNReadOp seekReadOprn = getOperation().getSeekNReadOp();
				SeekNReadParams seekReadParams = seekReadOprn.getStdParams();
				
				String filePath = determineFilePath(seekReadParams.getDataSet());
				long offset = seekReadParams.getOffset().longValue();
				int length = seekReadParams.getLength().intValue();
				
				toLog("SeekAndReadOperation() - run() -offset:"+offset,MatrixUtil.DEBUG_LOG);
				toLog("SeekAndReadOperation() - run() -length:"+length,MatrixUtil.DEBUG_LOG);
				
				
				//SRBAccount account = srbCreateSRBAccount();
				//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
				SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
				SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
				toLog("SeekAndReadOperation - run() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
				
				
				SRBFile srbFile = new SRBFile(srbFileSystem,filePath);
				SRBRandomAccessFile srbAccessFile =  new SRBRandomAccessFile(srbFile,"rw");
				
				// Create temp local File
				String fileCacheLocation = MatrixUtil.getPropertyFromPropertiesManager("jargon.filecache");
				String filePathForTargetAttachmentFile = fileCacheLocation + File.separator+"tempfile"+getTransactionId()+getStepId();
				LocalFile localFile = new LocalFile(filePathForTargetAttachmentFile);
				
				toLog("SeekAndReadOperation() - run() -localFile:"+filePathForTargetAttachmentFile,MatrixUtil.DEBUG_LOG);
				toLog("SeekAndReadOperation() - run() -srbFilePath:"+filePath,MatrixUtil.DEBUG_LOG);
				
				// Read From SRB file
				toLog("SeekAndReadOperation() - run() -read from SRB- BEGIN",MatrixUtil.DEBUG_LOG);
				byte[] buffer = new byte[length];
				srbAccessFile.seek(offset);
				int c;
				c = srbAccessFile.read(buffer,0,length); 
				toLog("SeekAndReadOperation() - run() -read  from SRB- END",MatrixUtil.DEBUG_LOG);
				
				// Write to Local File
				toLog("SeekAndReadOperation() - run() -write to Local - BEGIN",MatrixUtil.DEBUG_LOG);
				FileOutputStream localFileOS = new FileOutputStream(localFile.getFile());
				localFileOS.write(buffer);
				localFileOS.close();
				toLog("SeekAndReadOperation() - run() -write to Local - END",MatrixUtil.DEBUG_LOG);
				
				// Create and Store Attachment
				String contentId = seekReadParams.getContentMIMEID();			
				AttachmentPart attachment = (AttachmentPart)createAttachment("contentType",filePathForTargetAttachmentFile,contentId);
				getSRBStore().saveAttachment(getTransactionId(),attachment);
				//localFile.delete();
				toLog("SeekAndReadOperation() - run() -Attachment Created - END",MatrixUtil.DEBUG_LOG);
				
				// Implementation for Operation
		
				//setSRBResponse(new Object());
				srbFileSystem = null; // For Performance 
			}
			else
			{
				logStepAbortStatus();
			}	
			toLog("SeekAndReadOperation() - run() -END",MatrixUtil.DEBUG_LOG);
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