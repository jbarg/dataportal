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
public class SeekNWriteOp extends edu.sdsc.matrix.srb.parser.impl.SeekNWriteOpImpl {

    ArrayList attachmentList;

    /**
     * Creates a new SeekNWriteOp object specifying the standard parameters
     * for the operation and a path to file to write from.
     * This constructor takes care of construction of an SeekNWriteParams.
     * @param stdParams SeekNWrite Parameters.
     */
    public SeekNWriteOp(DataSet dataSet, 
                        java.math.BigInteger offset, 
                        java.math.BigInteger length, 
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
                        java.math.BigInteger offset, 
                        java.math.BigInteger length, 
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