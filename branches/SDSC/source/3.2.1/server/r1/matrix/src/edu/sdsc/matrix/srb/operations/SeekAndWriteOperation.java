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

import java.io.InputStream;

import edu.sdsc.matrix.srb.parser.*;
import edu.sdsc.matrix.srb.core.SRBConnection;

import edu.sdsc.matrix.srb.util.MatrixUtil;

/**
 * This class is responsible for handling the SRB Operation of  
 * seeking and writing data into a Data Set in SRB
 *  
 * @author rmathew
 * 
 */
public class SeekAndWriteOperation extends MatrixAgent
{
	/**
	 * The method that executes the steps for seeking and writing data into a Data Set
	 */
	public void run()
	{
		boolean stepAbort = false;
		try
		{
			logStepBeginStatus();
			toLog("SeekAndWriteOperation() - run() -BEGIN",MatrixUtil.DEBUG_LOG);	
			stepAbort = checkStepAbortFlag();
			if(!stepAbort)
			{		
				SeekNWriteOp seekWriteOprn = getOperation().getSeekNWriteOp();
				SeekNWriteParams seekWriteParams = seekWriteOprn.getStdParams();
				
				// Implementation for Operation
				String filePath = determineFilePath(seekWriteParams.getDataSet());
				long offset = seekWriteParams.getOffset().longValue();
				int length = seekWriteParams.getLength().intValue();
		
				toLog("SeekAndWriteOperation() - run() -offset:"+offset,MatrixUtil.DEBUG_LOG);
				toLog("SeekAndWriteOperation() - run() -length:"+length,MatrixUtil.DEBUG_LOG);
			
				//SRBAccount account = srbCreateSRBAccount();
				//SRBFileSystem srbFileSystem = new SRBFileSystem(account);
				
				SRBConnection srbConnection = (SRBConnection)retrieveGridConnection();
				SRBFileSystem srbFileSystem = srbConnection.getActiveConnection();
				toLog("SeekAndWriteOperation - run() -USED FROM CONNECTION",MatrixUtil.DEBUG_LOG);
					
				
				SRBFile srbFile = new SRBFile(srbFileSystem,filePath);
				SRBRandomAccessFile srbAccessFile =  new SRBRandomAccessFile(srbFile,"rw");
		
				// Create InputStream For Attachment 
				String contentId = seekWriteParams.getContentMIMEID();	
				InputStream attachmentStream = (InputStream) getAttachment(contentId);
				toLog("SeekAndWriteOperation() - run() -Attachment I/P Stream",MatrixUtil.DEBUG_LOG);
				
				// Read From Temp File 
				toLog("SeekAndWriteOperation() - run() -Read from Local File-BEGIN",MatrixUtil.DEBUG_LOG);
				byte[] buffer = new byte[length];
				int c = attachmentStream.read(buffer);
				toLog("SeekAndWriteOperation() - run() -Read from Local File-END",MatrixUtil.DEBUG_LOG);
				
				// Write to SRB File
				toLog("SeekAndWriteOperation() - run() -Write to SRB File-BEGIN",MatrixUtil.DEBUG_LOG);
				srbAccessFile.seek(offset);
				srbAccessFile.write(buffer);
				toLog("SeekAndWriteOperation() - run() -Write to SRB File-END:"+offset,MatrixUtil.DEBUG_LOG);
			
		
				//setSRBResponse(new Object());
				srbFileSystem = null; // For Performance 
			}	
			else
			{
				logStepAbortStatus();
			}	
			toLog("SeekAndWriteOperation() - run() -END",MatrixUtil.DEBUG_LOG);
		
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