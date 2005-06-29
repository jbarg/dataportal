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
 * ExampleClean.java
 * Large Matrix Client API DGRequest test example.
 *
 * Created on June 30, 2003, 5:26 AM
 */

import edu.sdsc.matrix.srb.client.*;
import javax.xml.soap.*;
import javax.xml.bind.*;
import java.io.*;
import javax.xml.transform.*;
import javax.xml.transform.stream.*;
import java.net.*;

/**
 * A Class that removes all data created by DatagridRequestExample.
 *
 * Look in "docs/Client-Readme" and the client API javadoc for more in depth explanations.
 *
 * @author  Allen Ding - SDSC
 */
public class ExampleClean {
    private static BufferedReader console = new BufferedReader(new InputStreamReader(System.in));

    public static void main(String args[]) {
        try {
            
            /*****************************************************
             * Here are some default values for the user.
             ****************************************************/
            String userid = "Matrix-demo";
            String password = "******";
            String homedir = "/home/Matrix-demo.sdsc";
            String organization = "sdsc";
            String srbserver = "srb.sdsc.edu";
            int srbport = 6824;
            // If the below is not available, you can change it to
            // http://arun-pc.sdsc.edu:8080/matrix/receiver
            String matrixserver = "http://localhost:8080/matrix/receiver";
            
            /**********************************************************
             * Now that we have login and server configuration
             * information, set up the required DocMeta, UserInfo, 
             * VirtualOrganizationInfo, and TerminationHandler objects.
             *
             * We will create the Transaction object later.
             *********************************************************/
            // Both UserInfo and VirtualOrgInfo need OrgInfo so set that up first
            OrganizationInfo oi = new OrganizationInfo(organization);
            
            // Set Up UserInfo object
            UserInfo ui = new UserInfo(userid, oi);
            ui.setChallengeResponse(password);          //  stuff that is optional,
            ui.setHomeDirectory(homedir);               //  but is probably needed
                                                        //  most of the time.
            
            ui.setDefaultStorageResource("sdsc-unix");  // the resource to store in

            // Set up VirtualOrganizationInfo object
            VirtualOrganizationInfo voi = new VirtualOrganizationInfo(srbserver, srbport);
            
            // Set up DocMeta with some description that doesn't matter
            DocMeta dm = new DocMeta("Charkueyteow");
            
            // Set up a Dummy TerminationHandler which doesn't do anything for now
            // but is part of the DGL schema.
            TerminationHandler th = new TerminationHandler("MyTH", "tba");


            /***************************************************
             * Now we get to creating the Transaction part of a 
             * DataGridRequest.
             *
             * This example creates a transaction that removes
             * all data created by DatagridRequestExample
             **************************************************/
            
            // Setup some names for the urls and datasets in the transaction
            String NewCollectionName = "ClientTestCollection";
            String NewContainerName = "ClientTestContainer";
            String NonExistentDataset = "IDONTEXISTXYZABC";
            
            DataSet HttpURL1dataset = new DataSet("ClientTestcactus.tar.gz");
            HttpURL1dataset.setLogicalLocation(homedir);
            
            DataSet HttpURL2dataset = new DataSet("ClientTestjames.zip");
            HttpURL2dataset.setLogicalLocation(homedir);
            
            DataSet FtpURL1dataset = new DataSet("ClientTestvelocity.zip");
            FtpURL1dataset.setLogicalLocation(homedir);
            
            DataSet localfiledataset = new DataSet("ClientTestdummyfile.txt");
            localfiledataset.setLogicalLocation(homedir);

            /*************************************
             * Now Create flow 1
             ************************************/
            Flow flow_one = new Flow("sequential");
            
            DeleteOp deleteop1 = new DeleteOp(new DeleteParams(new StdDatagridObject(HttpURL1dataset)));
            flow_one.addStep(new Step("Step 1_1", new Operation(deleteop1)));

            DeleteOp deleteop2 = new DeleteOp(new DeleteParams(new StdDatagridObject(HttpURL2dataset)));
            flow_one.addStep(new Step("Step 1_2", new Operation(deleteop2)));

            DeleteOp deleteop3 = new DeleteOp(new DeleteParams(new StdDatagridObject(FtpURL1dataset)));
            flow_one.addStep(new Step("Step 1_3", new Operation(deleteop3)));
            
            DeleteOp deleteop4 = new DeleteOp(new DeleteParams(new StdDatagridObject(localfiledataset)));
            flow_one.addStep(new Step("Step 1_4", new Operation(deleteop4)));            
            
            DeleteOp deleteop5 = new DeleteOp(new DeleteParams(new StdDatagridObject(new Collection(NewCollectionName))));
            flow_one.addStep(new Step("Step 1_5", new Operation(deleteop5)));  

            DeleteOp deleteop6 = new DeleteOp(new DeleteParams(new StdDatagridObject(new Container(NewContainerName))));
            flow_one.addStep(new Step("Step 1_6", new Operation(deleteop6)));  
            
            /***************************************************
             * Now we can create the actual Transaction and also
             * the DataGridRequest.
             **************************************************/
            Transaction transaction = new Transaction();
            transaction.addFlow(flow_one);
            
            DataGridRequest dgreq = new DataGridRequest(dm, ui, voi, transaction, th);

            /*******************************************************************
             * Now that we have a DataGridRequest, we can make a connection to a
             * Matrix server to send the request.
             ******************************************************************/
            print("Sending request to: " + matrixserver + "\n");
            MatrixConnection mConnect = new MatrixConnection(matrixserver);
            
            /*************************************************************
             * Send the request and get back a DataGridResponse which will
             * contain the transaction ID which can be used to query the
             * status of the request later.
             ************************************************************/
            DataGridResponse dgres = mConnect.sendRequest(dgreq);

            /*****************************************************************
             * Output some information about the DataGridResponse we received.
             ****************************************************************/
            System.out.println("Request State: " + dgres.getRequestState());
            System.out.println("Trans ID: " + dgres.getTransactionID());
            
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    
    public static void print(String s) {
        System.out.println(s);
    }

}