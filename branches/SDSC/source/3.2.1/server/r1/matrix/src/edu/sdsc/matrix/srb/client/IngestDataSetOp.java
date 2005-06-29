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
public class IngestDataSetOp extends edu.sdsc.matrix.srb.parser.impl.IngestDataSetOpImpl  {

    private ArrayList attachmentList;

    /**
     * Creates a new IngestOp object and specifying a path to a file ingest.
     * This constructor takes care of construction of an IngestDataSetParams.
     * @param contentMIMEID MIMEID of the file to ingest.
     * @param destination Destination dataset.
     * @param file Path to file to ingest.
     */
    public IngestDataSetOp(String contentMIMEID, DataSet destination, String filename) 
    throws SOAPException {
        attachmentList = new ArrayList();
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
    public IngestDataSetOp(String contentMIMEID, DataSet destination, byte[] data) 
    throws SOAPException, IOException {
        attachmentList = new ArrayList();
        super.setStdParams(new IngestDataSetParams(contentMIMEID, destination));
        
        // Ingest the bytes using the Toolbox
        attachmentList.add(MatrixToolbox.createAttachment(data, contentMIMEID));
    }

    /**
     * Creates a new IngestOp object using IngestParams.
     * @param externalURL URL to ingest.
     * @param destination Destination dataset.
     */
    public IngestDataSetOp(java.net.URL externalURL, DataSet destination) {
        super.setStdParams(new IngestDataSetParams(externalURL, destination));
    }

    /**
     * Creates a new IngestOp object using flexParams.
     * @param flexParams Flex Parameters.
     */
    public IngestDataSetOp(ParamList flexParams) {
        super.setFlexParams(flexParams);
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