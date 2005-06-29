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
 * CollectionList.java
 *
 * Created on August 25, 2003, 11:06 AM
 */

package edu.sdsc.matrix.srb.client;

import java.util.*;

/**
 * Class that represents a List of the contents of a collection.
 * The CollectionList may contain:<br/>
 * <ul>
 * <li>More CollectionLists's</li>
 * <li>Collections</li>
 * <li>DataSetExtendedLists</li>
 * </ul><br/>
 *
 * These contents may be accessed using getXXX methods. An example of using a collection
 * list might be like this:<br/><br/>
 * <pre>
 * // dgResponse is a DataGridResponse returned by MatrixConnection.sendRequest()
 * TransactionStatusResponse tsResponse = (TransactionStatusResponse)dgResponse.getTransactionStatusResponse();
 * FlowStatusResponse fsResponse = (FlowStatusResponse)tsResponse.getFlowStatusResponse().get(0);
 * StepStatusResponse ssResponse = (StepStatusResponse)fsResponse.getStepStatusResponse().get(0);
 *
 * // If empty collection
 * if(ssResponse.getResult() == null || ssResponse.getResult().getListResult() == null) {
 *     out.println("Empty Result");
 * }
 * // Non-empty collection
 * else {
 * // Get the result's CollectionList
 * CollectionList = (CollectionList)ssResponse.getResult().getListResult().getListing();
 *
 * ...
 * </pre>
 * 
 * @author  Allen Ding
 */
public class CollectionList extends edu.sdsc.matrix.srb.parser.impl.CollectionListImpl {

    /**
     * Internal method used to wrap an auto generated CollectionList object into this
     * class' representation. This method should not normally be used by the user.
     * The user would retrieve a parser.CollectionList from a higher level object in a
     * DataGridResponse instead and cast it to a client CollectionList.
     * @param collectionList The CollectionList to wrap.
     */    
    protected CollectionList(edu.sdsc.matrix.srb.parser.CollectionList collectionList) {
        Iterator i = null;

        // Wrap any Collection List's in this Collection List
        for(i = collectionList.getCollectionsExtendedInfo().iterator(); i.hasNext();) {
            edu.sdsc.matrix.srb.parser.CollectionList tempCollectionList = 
                (edu.sdsc.matrix.srb.parser.CollectionList)i.next();
            CollectionList innerCollectionList = new CollectionList(tempCollectionList);
            super.getCollectionsExtendedInfo().add(innerCollectionList);
        }

        // Wrap any collections in this Collection List
        for(i = collectionList.getCollections().iterator(); i.hasNext();) {
            edu.sdsc.matrix.srb.parser.Collection tempCollection =
                (edu.sdsc.matrix.srb.parser.Collection)i.next();
            Collection collection = new Collection(tempCollection);
            super.getCollections().add(collection);
        }

        // Wrap any Data Set Extended Lists in this Collection List
        for(i = collectionList.getDataSets().iterator(); i.hasNext();) {
            edu.sdsc.matrix.srb.parser.DataSetExtendedList tempDataSetExtendedList =
                (edu.sdsc.matrix.srb.parser.DataSetExtendedList)i.next();
            DataSetExtendedList dsel = new DataSetExtendedList(tempDataSetExtendedList);
            super.getDataSets().add(dsel);
        }
    }
    
}
