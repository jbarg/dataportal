/*
 * MatrixStatusRequest.java
 *
 * Created on 21 July 2003, 12:02
 *
 * This creates a transaction used by DataGridRequest to return to the status of a transaction
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

/**
 *
 * @author  Mark Williams
 */

public class MatrixStatusRequest {
    private static ObjectFactory objFactory = new ObjectFactory();
    
    public  TransactionStatusQuery createTransactionStatusQuery(String transactionID)
    throws JAXBException {
        
        TransactionStatusQuery tsq = objFactory.createTransactionStatusQuery();
        FlowStatusQuery fsq = objFactory.createFlowStatusQuery();
        tsq.setTransactionID(transactionID);
        tsq.setFlowStatusQuery(fsq);
        
        return tsq;
    }
}
