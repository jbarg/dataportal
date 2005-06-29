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
 * Collection.java
 *
 * Created on July 7, 2003, 3:43 AM
 */

package edu.sdsc.matrix.srb.client;

/**
 * Class that represents a collection. 
 * This class provies information about the class' name, owner, annotation, 
 * timestamps, and other useful information. The class also provides methods 
 * to easily create new Collection objects.
 * Only the name of a Collection is a required field according to the schema.
 *
 * When creating a collection for a DataGridRequest, an example usage might look
 * like:<br/><br/>
 * Collection myCollection = new Collection(collectionName);<br/><br/>
 * When retrieving a collection somewhere from a DataGridResponse, a usage might
 * look like:<br/><br/>
 * Collection resultCollection = (Collection)iterator.next(), where iterator could be
 * a list of collections returned in a ListResult.
 *
 * TODO: Finish wrapping around all classes in Collection
 *
 * @author  Allen Ding - SDSC
 */
public class Collection extends edu.sdsc.matrix.srb.parser.impl.CollectionImpl {

    /**
     * Internal method used to wrap an auto generated Collection object into this
     * class' representation. This method should not normally be used by the user.
     * Instead the used would retrieve a parser.Collection from a higher level object
     * in a DataGridResponse object and then cast it into a client Collection.
     * @param collection The parser.Collection to wrap.
     */
    protected Collection(edu.sdsc.matrix.srb.parser.Collection collection) {
        StdCollectionType sct = new StdCollectionTypeImpl();
        sct.setCollectionName(collection.getStdCollection().getCollectionName());
        sct.setCollectionOwner(collection.getStdCollection().getCollectionOwner());
        sct.setMetaData(collection.getStdCollection().getMetaData());
        sct.setAnnotation(collection.getStdCollection().getAnnotation());
        sct.setComments(collection.getStdCollection().getComments());
        sct.setDefContainer(collection.getStdCollection().getDefContainer());
        sct.setCreateTimeStamp(collection.getStdCollection().getCreateTimeStamp());
        super.setStdCollection(sct);
    }

    /**
     * Returns the collection name directly. This method is a pseudonym for getStdCollection.getCollectionName().
     * @return The collection name.
     */
    public String getCollectionName() {
        return super.getStdCollection().getCollectionName();
    }

    /**
     * Creates a Collection with the specified collectionName. The
     * collectionName argument is used by the method to set the
     * collectionName parameter of the internal StdCollection object.
     * @param collectionName The collection name.
     */
    public Collection(String collectionName) {
        StdCollectionType sct = new StdCollectionTypeImpl();
        sct.setCollectionName(collectionName);
        super.setStdCollection(sct);
    }

    /**
     * Creates a Collection with the specified anyDirectory argument.
     * @param anyDirectory A ParamList.
     */
    public Collection(ParamList anyDirectory) {
        super.setAnyDirectory(anyDirectory);        
    }

    /**
     * Makes the Collection use a collectionName.
     * Using this method clobbers any AnyDirectory parameters previously
     * set.
     * @param collectionName The name of the collection.
     */
    public void useCollectionName(String collectionName) {
        super.setAnyDirectory(null);
        super.getStdCollection().setCollectionName(collectionName);
    }

    /**
     * Makes the Collection use an anyDirectory parameter.
     * Using this method clobbers any StdCollection parameter previously
     * set.
     * @param  anyDirectory The ParamList to use.
     */
    public void useAnyDirectory(ParamList anyDirectory) {
        super.setStdCollection(null);
        super.setAnyDirectory(anyDirectory);
    }

}
