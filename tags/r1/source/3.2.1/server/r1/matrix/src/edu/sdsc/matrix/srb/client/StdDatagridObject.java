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
 * StdDatagridObject.java
 *
 * Created on July 7, 2003, 5:03 AM
 */

package edu.sdsc.matrix.srb.client;

/**
 * Part of the Matrix client API.
 * Contains methods to create StdDataGrid objects.
 *
 * @author  Allen Ding - SDSC
 */
public class StdDatagridObject extends edu.sdsc.matrix.srb.parser.impl.StdDatagridObjectImpl {

    /**
     * Creates a StdDatagridObject using the specified Collection
     * object.
     * @param collectionNow The Collection object to use.
     */
    public StdDatagridObject(Collection collectionNow) {
        super.setCollectionNow(collectionNow);
    }

    /**
     * Creates a StdDatagridObject using the specified DataSet object.
     * @param datasetNow The DataSet to use.
     */
    public StdDatagridObject(DataSet datasetNow) {
        super.setDataSetNow(datasetNow);        
    }

    /**
     * Creates a StdDatagridObject using the specified Container object.
     * @param containerNow The container to use.
     */
    public StdDatagridObject(Container containerNow) {
        super.setContainerNow(containerNow);
    }

    /**
     * Creates a StdDatagridObject using the specified
     * ActiveDatagridCollection object.
     * @param adcNow The ActiveDatagridCollection to use.
     */
    public StdDatagridObject(ActiveDatagridCollection adcNow) {
        super.setADCnow(adcNow);
    }

    /**
     * Creates a StdDatagridObject using the specified
     * ActiveDatagridDataSet object.
     * @param activeDataSetNow The ActiveDatagridDataSet to use.
     */
    public StdDatagridObject(ActiveDatagridDataSet activeDataSetNow) {
        super.setActiveDataSetNow(activeDataSetNow);
    }

    /**
     * Makes the StdDatagridObject use the specified Collection.
     * This method clobbers any DataSet, Container, ActiveDatagridCollection,
     * and ActiveDatagridDataSet parameters.
     */
    public void useCollection(Collection collectionNow) {
        super.setDataSetNow(null);
        super.setContainerNow(null);
        super.setADCnow(null);
        super.setActiveDataSetNow(null);
        super.setCollectionNow(collectionNow);
    }
    
    /**
     * Makes the StdDatagridObject use the specified DataSet.
     * This method clobbers any Collection, Container, ActiveDatagridCollection,
     * and ActiveDatagridDataSet parameters.
     */
    public void useDataSet(DataSet dataSetNow) {
        super.setContainerNow(null);
        super.setADCnow(null);
        super.setActiveDataSetNow(null);
        super.setCollectionNow(null);
        super.setDataSetNow(dataSetNow);
    }
    
    /**
     * Makes the StdDatagridObject use the specified Container.
     * This method clobbers any DataSet, Collection, ActiveDatagridCollection,
     * and ActiveDatagridDataSet parameters.
     */
    public void useContainer(Container containerNow) {
        super.setDataSetNow(null);
        super.setADCnow(null);
        super.setActiveDataSetNow(null);
        super.setCollectionNow(null);
        super.setContainerNow(containerNow);
    }
    
    /**
     * Makes the StdDatagridObject use the specified ActiveDatagridCollection.
     * This method clobbers any DataSet, Container, Collection,
     * and ActiveDatagridDataSet parameters.
     */
    public void useActiveDatagridCollection(ActiveDatagridCollection adcNow) {
        super.setDataSetNow(null);
        super.setContainerNow(null);
        super.setActiveDataSetNow(null);
        super.setCollectionNow(null);
        super.setADCnow(adcNow);
    }
    
    /**
     * Makes the StdDatagridObject use the specified ActiveDatagridCollection.
     * This method clobbers any DataSet, Container, Collection,
     * and ActiveDatagridCollection parameters.
     */
    public void useActiveDatagridDataSet(ActiveDatagridDataSet activeDataSetNow) {
        super.setDataSetNow(null);
        super.setContainerNow(null);
        super.setADCnow(null);
        super.setCollectionNow(null);
        super.setActiveDataSetNow(activeDataSetNow);  
    }
}