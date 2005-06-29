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
 * MetaData.java
 *
 * Created on July 7, 2003, 3:19 AM
 */

package edu.sdsc.matrix.srb.client;

import java.util.*;

/**
 * Part of the Matrix client API.
 * Contains methods to create MetaData objects. 
 * MetaData objects contain descriptive information  about other objects
 * in the DGL.
 *
 * Non optional-parameter changes to the object should be made using 
 * useXXX methods that are available. A bad configuration of set fields
 * may result in an invalid content tree.
 * @author  Allen Ding - SDSC
 */
public class MetaData extends edu.sdsc.matrix.srb.parser.impl.MetaDataImpl {

    /**
     * Internal method used to wrap a parser interface MetaData object into this
     * class' representation. This method should not normally be used by the user.
     * @param metadata The parser.MetaData to wrap.
     */
    protected MetaData(edu.sdsc.matrix.srb.parser.MetaData metadata) {
        for(Iterator i = metadata.getAVUPairs().iterator(); i.hasNext();) {
            edu.sdsc.matrix.srb.parser.AVUpair tempAVUpair =
                (edu.sdsc.matrix.srb.parser.AVUpair)i.next();
            AVUpair avupair = new AVUpair(tempAVUpair);
            super.getAVUPairs().add(avupair);
        }
        
        if(metadata.getUserDefined() != null)
            super.setUserDefined(new UserDefinedSchema(metadata.getUserDefined()));
        
        if(metadata.getSystemMeta() != null)
            super.setSystemMeta(new SystemMetaData(metadata.getSystemMeta()));
    }
    
    /**
     * Default do nothing constructor for a MetaData Object.
     */
    public MetaData() {
        super();
    }

    /**
     * Constructor that takes an initial attribute and value for the
     * Metadata object.
     * @param attribute Initial attribute.
     * @param value Initial value of the attribute.
     */
    public MetaData(String attribute, String value) {
        addAVUPair(attribute, value);
    }
    
    /**
     * Adds an attribute value pair to the MetaData object.
     * @param attribute The attribute to add.
     * @param value The value of the attribute being added.
     */
    public void addAVUPair(String attribute, String value) {
        super.getAVUPairs().add(new AVUpair(attribute, value));
    }
    
    /**
     * Constructs a MetaData object with an initial AVUpair object.
     * A MetaData object can hold an unlimited sequence of AVU pair objects.
     * @param avuPair The initial AVUpair element.
     */
    public MetaData(AVUpair avuPair) {
        super.getAVUPairs().add(avuPair);
    }
    
    /**
     * Constructs a MetaData object with an AnyType parameter.
     * @param userDefined Any AnyType object?
     */
    public MetaData(AnyType userDefined) {
        //super.setUserDefined(userDefined);
    }

    /**
     * Clears the objects AVUpair list and restarts with a new
     * one if for some reason you need it.
     * Using this method clobbers any previously set UserDefined
     * parameters.
     * @param avuPair The AVUpair to restart with.
     */
    public void useAVUPair(AVUpair avuPair) {
        super.setUserDefined(null);
        super.getAVUPairs().clear();
        super.getAVUPairs().add(avuPair);
    }

    /**
     * Adds an AVUPair to an existing MetaData object.
     * Using this method clobbers any previously set UserDefined
     * parameters.
     * @param avuPair The AVUpair to add.
     */
    public void addAVUPair(AVUpair avuPair) {
        super.setUserDefined(null);
        super.getAVUPairs().add(avuPair);
    }

    /**
     * Makes the MetaData object use an specified userDefined information.
     * Using this method clears the objects AVUPair list.
     * @param userDefined A user defined anyType.
     */
    public void useUserDefined(AnyType userDefined) {
        super.getAVUPairs().clear();
        //super.setUserDefined(userDefined);
    }

}
