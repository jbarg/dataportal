/*
 * MatrixIngestOp.java
 *
 * Created on 21 July 2003, 15:40
 *
 * This creates a transaction used by DataGridRequest to ingest a dataset into a collection
 *
 */

/*
 * PLEASE NOTE!!
 *
 * There is a bug in the Matrix server Ingest operation which I have traced to the buffering process when writing to SRB
 * This may be fixed by them in future but if not make sure you apply the following fix if you install a new Matrix server
 *
 * In src/edu/sdsc/matrix/srb/operations/IngestDataSetOperation.java look for the line attachment.write(buffer,0,buffer.length) and change this to
 * attachment.write(buffer,0,c)
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
    
    public Transaction createIngestOpTransaction(String logicalLocation, String dataId) {
        // dataId is the data set name (i.e. filename) given to the file in SRB (does not have to be the same as the source file but would make more sense
        // logicalLocation is the Collection name and should start with /home (e.g. /home/srbadm.matrix)
        
        Transaction tx = null;
        
        try {
            DataSet dataSet = objFactory.createDataSet();
            dataSet.setDataIdentifier(dataId);
            dataSet.setLogicalLocation(logicalLocation);
            
            IngestDataSetParams ingestParams = objFactory.createIngestDataSetParams();
            ingestParams.setContentMIMEID("INGESTFILE"); // Hardcoded for now but should match MIMEIDs from each file attached by the client
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
