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
 * Operation.java
 *
 * Created on July 8, 2003, 6:57 AM
 */

package edu.sdsc.matrix.srb.client;

import java.util.*;

/**
 * Part of the Matrix client API.
 * Contains methods to create Operation objects.
 * @author  Allen Ding - SDSC
 */
public class Operation extends edu.sdsc.matrix.srb.parser.impl.OperationImpl {
    
    /** 
     * Creates a new Operation object with a CopyOp
     */
    public Operation(CopyOp copyOp) {
        super.setCopyOp(copyOp);
    }

    /** 
     * Creates a new Operation object with a CreateOp
     */
    public Operation(CreateOp createOp) {
        super.setCreateOp(createOp);
    }

    /** 
     * Creates a new Operation object with a ChangePermissionOp
     */
    public Operation(ChangePermissionOp changePermissionOp) {
        super.setChangePermissionOp(changePermissionOp);
    }
    
    /** 
     * Creates a new Operation object with a DeleteOp
     */
    public Operation(DeleteOp deleteOp) {
        super.setDeleteOp(deleteOp);
    }
    
    /** 
     * Creates a new Operation object with a RenameOp
     */
    public Operation(RenameOp renameOp) {
        super.setRenameOp(renameOp);
    }    

    /** 
     * Creates a new Operation object with a IngestOp
     */
    public Operation(IngestDataSetOp ingestOp) {
        super.setIngestOp(ingestOp);
    }

    /** 
     * Creates a new Operation object with a PrepareTicketOp
     */
    public Operation(PrepareTicketOp ptOp) {
        super.setPrepareTicketOp(ptOp);
    }
    
    /** 
     * Creates a new Operation object with a DownloadDataSetOp
     */
    public Operation(DownloadDataSetOp ddsOp) {
        super.setDownloadDataSetOp(ddsOp);
    }
    
    /** 
     * Creates a new Operation object with a ReplicateOp
     */
    public Operation(ReplicateOp replicateOp) {
        super.setReplicateOp(replicateOp);
    }
    
    /** 
     * Creates a new Operation object with a ListOp
     */
    public Operation(ListOp listOp) {
        super.setListOp(listOp);
    }
    
    /** 
     * Creates a new Operation object with a SeekNReadOp
     */
    public Operation(SeekNReadOp snrOp) {
        super.setSeekNReadOp(snrOp);
    }
    
    /** 
     * Creates a new Operation object with a SeekNWriteOp
     */
    public Operation(SeekNWriteOp snwOp) {
        super.setSeekNWriteOp(snwOp);
    }
    
    /**
     * Creates a new Operation object with a MetadataQueryOp
     */
    public Operation(MetadataQueryOp mdqOp) {
        super.setMetadataQueryOp(mdqOp);
    }
    
    public Operation(IngestMetadataOp imOp) {
        super.setIngestMetadataOp(imOp);
    }
    
    protected ArrayList getAttachments() {
        //Which specific op is it that has attachments
        //find the op and return it
        if(super.getIngestOp() != null)
            return ((IngestDataSetOp)super.getIngestOp()).getAttachments();
        else if (super.getSeekNWriteOp() != null)
            return ((SeekNWriteOp)super.getSeekNWriteOp()).getAttachments();
        else
            return null;
    }
}