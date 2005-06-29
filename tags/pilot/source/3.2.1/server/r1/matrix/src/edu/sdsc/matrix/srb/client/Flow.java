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
 * Flow.java
 *
 * Created on July 8, 2003, 7:02 AM
 */

package edu.sdsc.matrix.srb.client;

import java.util.*;

/**
 * Part of the Matrix client API.
 * Contains methods to create Flow objects.
 * @author  Allen Ding - SDSC
 */
public class Flow extends edu.sdsc.matrix.srb.parser.impl.FlowImpl {

    /**
     * Creates a flow object with no initial step and flow type.
     * A step must be set later!
     */
    public Flow(String flowType) {
        super.setFlowType(flowType);
    }

    /**
     * Creates a Flow object with the specified flowType and initial
     * step. A flow may hold 1 - inf. steps.
     * @param flowType The type of flow.
     * @param step The initial step.
     */
    public Flow(String flowType, Step step) {
        super.setFlowType(flowType);
        super.getSteps().add(step);
    }
    
    /**
     * Adds a step to the flow object.
     * @param step The step to add.
     */
    public void addStep(Step step) {
        super.getSteps().add(step);
    }
    
    /**
     * Method for other classes in package to get a single level
     * ArrayList accumulation of SOAP AttachmentPart's from all the steps
     * that are in this Flow.
     */
    protected ArrayList getAttachments() {
        ArrayList returnList = new ArrayList();

        //search for attachments for all steps in this flow
        for(Iterator i = super.getSteps().iterator(); i.hasNext();) {
            Step step = (Step)i.next();
            
            //if the step has attachments, add them to the return list
            if(step.getAttachments() != null) {
                
                //extract each attachmentpart and store in new list
                for(Iterator x = step.getAttachments().iterator(); x.hasNext();) {
                    returnList.add(x.next());
                }      
            }
        }
        
        return returnList;
    }

}