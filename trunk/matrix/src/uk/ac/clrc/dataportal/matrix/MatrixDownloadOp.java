/*
 * MatrixDownloadOp.java
 *
 * Created on 21 July 2003, 12:00
 *
 * This creates a transaction used by DataGridRequest to download a dataset from a collection
 *
 * It needs changing to allow a client to set certain parameters (like the filename etc).
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

public class MatrixDownloadOp {
    
    private static ObjectFactory objFactory = new ObjectFactory();
    
    public Transaction createDownloadOpTransaction() {
        
        Transaction tx = null;
        
        try {
            DataSet dataSet = objFactory.createDataSet();
            dataSet.setDataIdentifier("dbtf.pdf");
            dataSet.setLogicalLocation("/home/srbadm.matrix");
            
            DownloadDataSetParams downloadParams = objFactory.createDownloadDataSetParams();
            downloadParams.setDataSource(dataSet);
            downloadParams.setContentMIMEID("download_filename"); // Should be a parameter
            
            DownloadDataSetOp downloadOp = objFactory.createDownloadDataSetOp();
            downloadOp.setStdParams(downloadParams);
            
            Operation op = objFactory.createOperation();
            op.setDownloadDataSetOp(downloadOp);
            
            Step step = objFactory.createStep();
            step.setStepID("ID0001");
            step.setStepName("downloadDataSetOp"); // MUST BE "downloadDataSetOp"
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
