/*
 * MatrixCreateCollectionOp.java
 *
 * Created on 25 July 2003, 11:14
 *
 * This creates a transaction used by DataGridRequest to create a collection
 *
 */

package uk.ac.clrc.dataportal.matrix;

import javax.xml.soap.*;
import javax.xml.transform.*;
import javax.xml.transform.stream.*;

import java.io.*;
import java.net.URL;

import edu.sdsc.matrix.srb.parser.*;

import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBException;

import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;

// Just for testing....
import java.io.FileOutputStream;

/**
 *
 * @author  Mark Williams
 */

public class MatrixCreateCollectionOp {
    
    private static ObjectFactory objFactory = new ObjectFactory();
    
    public Transaction createCreateCollectionOpTransaction(String collection) {
        // collection is the Collection name in SRB.  If it's not prefixed it will be create as a sub-directory of the home collection.
        // Use /home/collection.name to create a top level one
        Transaction tx = null;
        
        try {
            
            Collection.StdCollectionType collType = objFactory.createCollectionStdCollectionType();
            collType.setCollectionName(collection);
            
            Collection coll = objFactory.createCollection();
            coll.setStdCollection(collType);
            
            StdDatagridObject stdDGO = objFactory.createStdDatagridObject();
            stdDGO.setCollectionNow(coll);
            
            CreateParams createParams = objFactory.createCreateParams();
            createParams.setDatagridObject(stdDGO);
            
            CreateOp createOp = objFactory.createCreateOp();
            createOp.setStdParams(createParams);
            
            Operation op = objFactory.createOperation();
            op.setCreateOp(createOp);
            
            Step step = objFactory.createStep();
            step.setStepID("ID0001");
            step.setStepName("createOp"); // MUST BE "createOp"
            step.setOperation(op);
            
            Flow flow = objFactory.createFlow();
            flow.setFlowType("sequential");
            flow.getSteps().add(step);
            
            tx = objFactory.createTransaction();
            tx.setFlow(flow);
            
        }
        catch (Exception e) {
            e.printStackTrace();
        }
        
        return tx;
    }
}
