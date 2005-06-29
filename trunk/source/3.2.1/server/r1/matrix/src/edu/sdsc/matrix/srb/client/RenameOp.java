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
 * RenameOp.java
 *
 * Created on July 8, 2003, 5:08 AM
 */

package edu.sdsc.matrix.srb.client;

/**
 * Part of the Matrix client API.
 * Contains methods to create RenameOp objects.
 *
 * A RenameOp object can use either StdParams or FlexParams,
 * but not both.
 * @author  Allen Ding - SDSC
 */
public class RenameOp extends edu.sdsc.matrix.srb.parser.impl.RenameOpImpl {

    /**
     * Make a RenameOp renaming the first collection name to a new one.
     * @param collection The collection to rename
     * @param newName The new name for the collection
     */
    public RenameOp(Collection collection, String newName) {
        super.setStdParams(new RenameParams(new StdDatagridObject(collection), newName));
    }

    /**
     * Make a RenameOp renaming the first dataset name to a new one.
     * @param dataset The dataset to rename
     * @param newName The new name for the collection
     */
    public RenameOp(DataSet dataset, String newName) {
        super.setStdParams(new RenameParams(new StdDatagridObject(dataset), newName));
    }

    /**
     * Creates a new RenameOp object using RenameParams.
     * @param stdParams Rename Parameters.
     */
    public RenameOp(RenameParams stdParams) {
        super.setStdParams(stdParams);
    }

    /**
     * Creates a new RenameOp object using flexParams.
     * @param flexParams Flex Parameters.
     */
    public RenameOp(ParamList flexParams) {
        super.setFlexParams(flexParams);
    }

    /**
     * Makes the RenameOp use the specified RenameParams. Using this
     * method clobbers any flexParams paremeter.
     * @param stdParams A RenameParams.
     */
    public void useStdParams(RenameParams stdParams) {
        super.setFlexParams(null);
        super.setStdParams(stdParams);
    }

    /**
     * Makes the RenameOp use the specified ParamList. Using this
     * method clobbers any stdParams parameter.
     * @param flexParams A ParamList.
     */   
    public void useFlexParams(ParamList flexParams) {
        super.setStdParams(null);
        super.setFlexParams(flexParams);
    }
    
}