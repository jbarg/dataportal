/*
 * MatrixListOp.java
 *
 * Created on 18 July 2003, 12:02
 *
 * This creates a transaction used by DataGridRequest to list the contents of a collection
 *
 * Some parameters MUST be set as specified below as the SDSC Matrix code seems to get some
 * parameters from the wrong places!!  eg. OperationName taken from StepName
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

public class MatrixListOp {
    
    private static ObjectFactory objFactory = new ObjectFactory();
    
    public Transaction createListOpTransaction(String collection) {
        
        Transaction tx = null;
        
        try {
            
            Collection.StdCollectionType collType = objFactory.createCollectionStdCollectionType();
            collType.setCollectionName(collection);
            
            Collection coll = objFactory.createCollection();
            coll.setStdCollection(collType);
            
            StdDatagridObject stdDGO = objFactory.createStdDatagridObject();
            stdDGO.setCollectionNow(coll);
            
            ListParams listParams = objFactory.createListParams();
            listParams.setDataSource(stdDGO);
            listParams.setExtendedInfoLevel(0);
            listParams.setRecursive(true); // MUST BE SET TO TRUE
            
            ListOp listOp = objFactory.createListOp();
            listOp.setStdParams(listParams);
            
            Operation op = objFactory.createOperation();
            op.setListOp(listOp);
            
            Step step = objFactory.createStep();
            step.setStepID("ID0001");
            step.setStepName("listOp"); // MUST BE "listOp"
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
