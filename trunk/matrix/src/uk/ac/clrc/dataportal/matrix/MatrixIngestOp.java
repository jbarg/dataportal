/*
 * MatrixIngestOp.java
 *
 * Created on 21 July 2003, 15:40
 *
 * This creates a transaction used by DataGridRequest to ingest a dataset into a collection
 *
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

public class MatrixIngestOp {
    
    private static ObjectFactory objFactory = new ObjectFactory();
    
    public Transaction createIngestOpTransaction() {
        
        Transaction tx = null;
        
        try {
            DataSet dataSet = objFactory.createDataSet();
            dataSet.setDataIdentifier("ingest.txt");
            dataSet.setLogicalLocation("/home/srbadm.matrix");
            
            IngestDataSetParams ingestParams = objFactory.createIngestDataSetParams();
            ingestParams.setContentMIMEID("INGESTFILE");
            ingestParams.setDataDestination(dataSet);
            
            IngestDataSetOp ingestOp = objFactory.createIngestDataSetOp();
            ingestOp.setStdParams(ingestParams);
            
            Operation op = objFactory.createOperation();
            op.setIngestOp(ingestOp);
            
            Step step = objFactory.createStep();
            step.setStepID("ID0001");
            step.setStepName("ingestOp"); // MUST BE "ingestOp"
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
