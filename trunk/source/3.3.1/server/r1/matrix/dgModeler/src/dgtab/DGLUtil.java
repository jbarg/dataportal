/*
	* DGLUtil.java
	* July 04

	*
	* Copyright (c) 2001-2004, SDSC All rights reserved.
	*
	* Redistribution and use in source and binary forms, with or without
	* modification, are permitted provided that the following conditions are met:
	*  - Redistributions of source code must retain the above copyright notice,
	* this list of conditions and the following disclaimer. - Redistributions in
	* binary form must reproduce the above copyright notice, this list of
	* conditions and the following disclaimer in the documentation and/or other
	* materials provided with the distribution. - Neither the name of JGraph nor
	* the names of its contributors may be used to endorse or promote products
	* derived from this software without specific prior written permission.
	*
	* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
	* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
	* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
	* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
	* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
	* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
	* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
	* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
	* POSSIBILITY OF SUCH DAMAGE.
	*
	*/



package dgtab;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;
import javax.xml.soap.MessageFactory;
import javax.xml.soap.SOAPConnection;
import javax.xml.soap.SOAPConnectionFactory;
import javax.xml.soap.SOAPException;
import javax.xml.soap.SOAPMessage;
import javax.xml.soap.SOAPPart;
import javax.xml.transform.Source;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;

import edu.sdsc.matrix.srb.client.AnyControlStructure;
import edu.sdsc.matrix.srb.client.DataGridRequest;
import edu.sdsc.matrix.srb.client.Flow;
import edu.sdsc.matrix.srb.client.FlowStatusQuery;

/**
	* Reads, saves and handles other utility methods involving DGL files.
	* @author Daniel Moore, San Diego Supercomputer Center
	* @since DGLModeler1.0
	*/

public class DGLUtil{

			public static final String MATRIX_URL =
					"http://localhost:8080/matrix/receiver";

			public static final int DEBUG = 10;

			private static URL connection_URL;

			/**
				* This method takes a DataGridRequest and a File and saves the DataGridRequest in the file.
				* @param dglFile The file to save to.
				* @param req The DataGridRequest to save.
				*/

			public static void save(File dglFile, DataGridRequest req){
						try{
									// Establish the context
									JAXBContext jc = JAXBContext.newInstance("edu.sdsc.matrix.srb.parser");
									// Create a Marshaller
									Marshaller marshaller = jc.createMarshaller();
									try{
												// Open a FileOutputStream to write to the file
												FileOutputStream fout = new FileOutputStream(dglFile);
												// Marshall that data!
												marshaller.marshal((edu.sdsc.matrix.srb.parser.DataGridRequest) req,
														fout);
									} catch (IOException e){
												System.out.println(e);
												e.printStackTrace();
									}
						} catch (JAXBException e){
									System.out.println(e);
									e.printStackTrace();
						}

			}

			/**
				* This method takes a DataGridResponse and a File and saves the DataGridResponse in the file.
				* @param dglFile The file to save to.
				* @param req The DataGridResponse to save.
				*/

			public static void save(File dglFile,
					edu.sdsc.matrix.srb.parser.DataGridResponse res){
						try{
									// Establish the context
									JAXBContext jc = JAXBContext.newInstance("edu.sdsc.matrix.srb.parser");
									// Create a Marshaller
									Marshaller marshaller = jc.createMarshaller();
									try{
												// Open a FileOutputStream to write to the file
												FileOutputStream fout = new FileOutputStream(dglFile);
												// Marshall that data!
												marshaller.marshal((edu.sdsc.matrix.srb.parser.DataGridResponse)
														res, fout);
									} catch (IOException e){
												System.out.println(e);
												e.printStackTrace();
									}
						} catch (JAXBException e){
									System.out.println(e);
									e.printStackTrace();
						}

			}

			/**
				* This method takes a file and returns a new DataGridRequest to represent the contents of the file.
				* @param dglFile The file to read from.
						* @return Returns a new DataGridRequest object that was created from the file.
				*/

			public static DataGridRequest load(File dglFile){
						// Create a new DataGridRequest object to stor the file data in
						DataGridRequest req = null;
						try{
									// Establish the context
									JAXBContext jc = JAXBContext.newInstance("edu.sdsc.matrix.srb.parser");
									// Get an unmarshaller appropriate to the context
									Unmarshaller unmarshaller = jc.createUnmarshaller();
									// Unmarshal the file to get the DataGridRequest
									req = new DataGridRequest((edu.sdsc.matrix.srb.parser.DataGridRequest)
											unmarshaller.unmarshal(dglFile));
						} catch (JAXBException e){
									System.err.println(e);
									e.printStackTrace();
						}
						// Create a new .client.DataGridRequest to return to the user
						return req;

			}

			/*
				*  This method sends a DataGridRequest object to the server specified in endpoint and returns the response.
				* This works by saving the request to a temporary file and calling the corresponding method.
				*/

			public static edu.sdsc.matrix.srb.parser.DataGridResponse sendRequest(
					DataGridRequest req, URL endpoint){
						File f;
						try{
									// Create a temporary file to save the request to for sending
									f = File.createTempFile("lastRequest", ".dgl");
									if (DEBUG >= 10){
												System.out.println(f);
									}
						} catch (IOException e){
									System.out.println(e);
									e.printStackTrace();
									System.out.println(
											"Could not create a temporary file to send, request was not sent.");
									return null;
						}
						// Save the DataGridRequest to the file
						save(f, req);
						// Send the request stored in the temporary file
						edu.sdsc.matrix.srb.parser.DataGridResponse res = sendRequest(f, endpoint);
						// Delete the temporary file
						f.delete();
						// Return the DataGridResponse
						return res;

			}

			/**
				* This method doesn't work at the moment, not sure why
				*/

			public static edu.sdsc.matrix.srb.parser.DataGridResponse queryStatus(
					DataGridRequest request, URL endpoint, String queryID){
						if (DEBUG >= 10){
									System.out.println("queryID = " + queryID);
						}
						// Create a new FlowStatusQuery with the specified ID
						FlowStatusQuery fsq = new FlowStatusQuery(queryID);
						// Print some debug message
						if (DEBUG >= 10){
									System.out.println("Flow Status Query = ");
									System.out.println(fsq.getFlowID());
									System.out.println();
						}
						// Clear out the old flow
						request.setFlow(null);
						// Same
						request.useFlow(null);
						// Set the DataGridRequest FlowStatusQuery
						request.setFlowStatusQuery(fsq);
						// Set the DataGridRequest to use the status query
						request.useFlowStatusQuery(fsq);
						// Send the request and return the response
						return sendRequest(request, endpoint);

			}

			/**
				* Loads the status query template and substitutes placeholder text with the actual queryID
				*/
			public static edu.sdsc.matrix.srb.parser.DataGridResponse queryStatus2(
					DataGridRequest request, URL endpoint, String queryID){
						try{
									SOAPConnectionFactory scf = SOAPConnectionFactory.newInstance();
									SOAPConnection connection = scf.createConnection();
									MessageFactory mf = MessageFactory.newInstance();
									SOAPMessage msg = mf.createMessage();
									SOAPPart soapPart = msg.getSOAPPart();
									Source source = null;
									if (DEBUG >= 10){
												System.out.println("queryID = " + queryID);
									}
									File f;
									try{
												// Create a temporary file to save the request to for sending
												f = File.createTempFile("lastRequest", ".dgl");
												if (DEBUG >= 10){
															System.out.println(f);
												}
									} catch (IOException e){
												System.out.println(e);
												e.printStackTrace();
												System.out.println(
														"Could not create a temporary file to send, request was not sent.");
												return null;
									}
									String fileString = readFile("C:/Documents and Settings/mixx/Desktop/Matrix/examples/Using-XMLeditor/dgl3-GetResponse.dgl");
									System.out.println(fileString);
									fileString = replacePlaceHolderWithValue(fileString, "_FLOW-ID_",
											queryID);
									System.out.println(fileString);
									FileWriter fw = new FileWriter(f);
									fw.write(fileString, 0, fileString.length());
									fw.close();
									source = new StreamSource(new FileInputStream(f.getAbsolutePath()));
									soapPart.setContent(source);
									System.out.println("\n\n");
									System.out.println("");
									System.out.println("Sending DGL message to: " + endpoint);
									printReply(msg);
									SOAPMessage reply = connection.call(msg, endpoint);
									System.out.println("\n\n");
									System.out.println("Received DGL Reply from: " +
											endpoint);
									printReply(reply);
									JAXBContext jc = JAXBContext.newInstance(
											"edu.sdsc.matrix.srb.parser");
									Unmarshaller unmarshaller = jc.createUnmarshaller();
									Source replyxml = reply.getSOAPPart().getContent();
									return (edu.sdsc.matrix.srb.parser.DataGridResponse) unmarshaller.
											unmarshal(replyxml);
						} catch (Exception e){
									System.out.println(e);
									e.printStackTrace();
						}
						return null;

			}

			public static edu.sdsc.matrix.srb.parser.DataGridResponse queryStatus(String
					queryID){
						try{
									SOAPConnectionFactory scf = SOAPConnectionFactory.newInstance();
									SOAPConnection connection = scf.createConnection();
									MessageFactory mf = MessageFactory.newInstance();
									SOAPMessage msg = mf.createMessage();
									SOAPPart soapPart = msg.getSOAPPart();
									Source source = null;
									if (DEBUG >= 10){
												System.out.println("queryID = " + queryID);
									}
									File f;
									try{
												// Create a temporary file to save the request to for sending
												f = File.createTempFile("lastRequest", ".dgl");
												if (DEBUG >= 10){
															System.out.println(f);
												}
									} catch (IOException e){
												System.out.println(e);
												e.printStackTrace();
												System.out.println(
														"Could not create a temporary file to send, request was not sent.");
												return null;
									}
									String fileString = readFile("C:/Documents and Settings/mixx/Desktop/Matrix/examples/Using-XMLeditor/dgl3-GetResponse.dgl");
									System.out.println(fileString);
									fileString = replacePlaceHolderWithValue(fileString, "_FLOW-ID_",
											queryID);
									System.out.println(fileString);
									FileWriter fw = new FileWriter(f);
									fw.write(fileString, 0, fileString.length());
									fw.close();
									source = new StreamSource(new FileInputStream(f.getAbsolutePath()));
									soapPart.setContent(source);
									System.out.println("\n\n");
									System.out.println("");
									System.out.println("Sending DGL message to: " + connection_URL);
									printReply(msg);
									SOAPMessage reply = connection.call(msg, connection_URL);
									System.out.println("\n\n");
									System.out.println("Received DGL Reply from: " + connection_URL);
									printReply(reply);
									JAXBContext jc = JAXBContext.newInstance(
											"edu.sdsc.matrix.srb.parser");
									Unmarshaller unmarshaller = jc.createUnmarshaller();
									Source replyxml = reply.getSOAPPart().getContent();
									return (edu.sdsc.matrix.srb.parser.DataGridResponse) unmarshaller.
											unmarshal(replyxml);
						} catch (Exception e){
									System.out.println(e);
									e.printStackTrace();
						}
						return null;

			}

			public static edu.sdsc.matrix.srb.parser.DataGridResponse sendRequest(File
					REQUEST_FILE, URL endpoint){
						// Some (un)marshalling utilities
						Unmarshaller unmarshaller;
						Marshaller marshaller;
						try{
									// Create a new instance of SOAPConnectionFactory
									SOAPConnectionFactory scf = SOAPConnectionFactory.newInstance();
									// Create a connection
									SOAPConnection connection = scf.createConnection();
									// Create a new instance of MessageFactory
									MessageFactory mf = MessageFactory.newInstance();
									// Create a SOAPMessage
									SOAPMessage msg = mf.createMessage();
									// Get the SOAPPart of the message
									SOAPPart soapPart = msg.getSOAPPart();
									// Get the source file
									Source source = new StreamSource(new FileInputStream(REQUEST_FILE));
									// Set the content of the SOAPPart of the message to the Stream
									soapPart.setContent(source);
									// Sends the message and stores the reply
									SOAPMessage reply = connection.call(msg, endpoint);
									// Create the unmarshaller
									JAXBContext jc = JAXBContext.newInstance("edu.sdsc.matrix.srb.parser");
									unmarshaller = jc.createUnmarshaller();
									// Unmarshall the reply XML source
									Source replyxml = reply.getSOAPPart().getContent();
									// Print some Debug message
									if (DEBUG >= 10){
												System.out.println("Reply XML = ");
												try{
															printReply(reply);
												} catch (Exception e){
															System.out.println(e);
															e.printStackTrace();
												}
									}
									// Unmarshall the XML to get the DataGridResponse
									return (edu.sdsc.matrix.srb.parser.DataGridResponse) unmarshaller.
											unmarshal(replyxml);
									// Catch some exceptions
						} catch (FileNotFoundException e){
									System.out.println(e);
									e.printStackTrace();
						} catch (SOAPException e){
									System.out.println(e);
									e.printStackTrace();
						} catch (JAXBException e){
									System.out.println(e);
									e.printStackTrace();
						}
						return null;

			}

			/**
				*  Arun's code from DGLSender, only prints to System.out as far as I know
				*/

			private static void printReply(SOAPMessage reply) throws Exception{
						TransformerFactory tFact = TransformerFactory.newInstance();
						Transformer transformer = tFact.newTransformer();
						Source src = reply.getSOAPPart().getContent();
						StreamResult result = new StreamResult(System.out);
						transformer.transform(src, result);
						System.out.println();

			}

			/**
				*  Arun's code from DGLSender
				*/

			public static String printFlowStatusResponse(edu.sdsc.matrix.srb.parser.
					FlowStatusResponse fsr, int level){
						String ret = "";
						for (int i = 0; i < level; i++)
									System.out.print("\t");
						ret += "Flow " + fsr.getID() + "\n";
						if (fsr.getFlowStatusResponse().size() == 0){
									if (fsr.getStepStatusResponse().size() == 0){
												for (int i = -1; i <= level; i++)
															System.out.print("\t");
												ret += "\t STEPS NOT STARTED or NOT FOUND" + "\n";
									}
									for (Iterator i2 = fsr.getStepStatusResponse().iterator(); i2.hasNext(); ){
												edu.sdsc.matrix.srb.parser.StepStatusResponse ssr = (edu.sdsc.
														matrix.srb.parser.StepStatusResponse)
														i2.next();
												for (int i = 0; i <= level; i++)
															ret += "\t";
												ret += "Step:" + ssr.getID() + "\t" + ssr.getStatusCode()
														+ "\t" + translateStatusCode(ssr.getStatusCode()) + "\n";
									}
									ret += "\n";
						} else{
									for (Iterator i1 = fsr.getFlowStatusResponse().iterator(); i1.hasNext(); ){
												edu.sdsc.matrix.srb.parser.FlowStatusResponse f2 = (edu.sdsc.matrix.
														srb.parser.FlowStatusResponse) i1.next();
												ret += printFlowStatusResponse(f2, level + 1);
									}
						}
						return ret;

			}

			/**
				* Deciphers status codes
				*/

			public static String translateStatusCode(int statusCode){
						// Status codes deciphered
						switch (statusCode){
									case (9000016):
												return "THREAD_NOT_FOUND_OR_NOT_STARTED";
									case (9000017):
												return "THREAD_STARTED";
									case (9000018):
												return "THREAD_COMPLETE";
									case (9000019):
												return "THREAD_ABORTED_DUE_TO_ERROR_IN_PREPROCESS";
									case (9000020):
												return "THREAD_ABORTED_DUE_TO_ERROR_IN_PROCESS";
									case (9000021):
												return "THREAD_ABORTED_DUE_TO_ERROR_IN_POSTPROCESS";
									case (9000022):
												return "THREAD_ABORTED_DUE_TO_REQUEST_FROM_USER";
									default:
												return "UNKNOWN THREAD STATE" +
														"\nPlease e-mail talk2matrix@sdsc.edu about this";
						}

			}

			/**
				* Testing functions
				* @param args File to load
				*/

			public static void main(String args[]){
						System.out.println("Testing DGLUtil functions");
						DataGridRequest req = null;
						if (args.length >= 1){
									try{
												req = load(new File(args[0]));
									} catch (Exception e){
												e.printStackTrace();
									}
						} else{
									try{
												req = load(new File(
														"C:/Documents and Settings/mixx/Desktop/DGL Tests/dgl3-Ingest.dgl"));
									} catch (Exception e){
												e.printStackTrace();
									}
						}
						if (req == null){
									System.out.println("DataGridRequest == null");
									System.out.println("No further testing");
						} else{
									Flow flow = new Flow(req.getFlow());
									System.out.println(flow);
									List flows = new ArrayList();
									flows = flow.getFlow();
									System.out.println("Flows: ");
									for (int i = 0; i < flows.size(); i++){
												System.out.println(flows.get(i));
									}
									flows = flow.getStep();
									System.out.println("Steps: ");
									for (int i = 0; i < flows.size(); i++){
												System.out.println(flows.get(i));
									}
									System.out.println("ACS: ");
									AnyControlStructure acs = new AnyControlStructure(flow.getFlowLogic());
									System.out.println(acs);
									System.out.println("Testing send:");
									URL matrixServer = null;
									try{
												matrixServer = new URL(MATRIX_URL);
									} catch (MalformedURLException e){
												System.out.println(e);
												e.printStackTrace();
									}
									File testFile = new File("C:/Documents and Settings/mixx/Desktop/Matrix/examples/Using-XMLeditor/dgl3-Ingest.dgl");
									if (matrixServer != null){
												edu.sdsc.matrix.srb.parser.DataGridResponse dgresponse =
														sendRequest(req, matrixServer);
												edu.sdsc.matrix.srb.parser.RequestAcknowledgment ack = dgresponse.
														getRequestAcknowledgement();
												if (ack != null){ //its a status response
															String flowId = ack.getRequestID();
															System.out.println("\nYour Flow ID : " + flowId);
															System.out.println(
																	"To Check Status use:\n java DGLSender status:" + flowId
																	+ " " + matrixServer);
															System.out.println("\nTesting queryStatus: ");
															dgresponse = queryStatus2(req, matrixServer, flowId);
															edu.sdsc.matrix.srb.parser.FlowStatusResponse fsr = dgresponse.
																	getFlowStatusResponse();
															if (fsr != null)
																		System.out.println(printFlowStatusResponse(fsr, 0));
												} else{
															System.out.println(
																	"\n\nResponse XML converted to viewable format:");
															edu.sdsc.matrix.srb.parser.FlowStatusResponse fsr = dgresponse.
																	getFlowStatusResponse();
															if (fsr != null)
																		System.out.println(printFlowStatusResponse(fsr, 0));
												}
									} else{
												System.out.println("MatrixServer = null");
									}
						}

			}

			/**
				*  Arun's code from DGLSender
				*/

			private static String replacePlaceHolderWithValue(String file,
					String placeHolder, String value){
						try{
									if (value == null)
												value = "";
									int index = file.indexOf("^" + placeHolder);
									while (index != -1){
												int carIndex = file.indexOf("^", index + 1);
												file = file.substring(0, index) + value +
														file.substring(carIndex + 1, file.length());
												index = file.indexOf("^" + placeHolder, index);
									}
									return file;
						} catch (Exception e){
									return file;
						}

			}

			/**
				*  Arun's code from DGLSender
				*/

			private static String readFile(String fileName){
						String fileString = null;
						if (fileName != null){
									BufferedInputStream br = null;
									try{
												File file = new File(fileName);
												int length = (int) file.length();
												br = new BufferedInputStream(new FileInputStream(file));
												byte b[] = new byte[length];
												br.read(b);
												String tempfileString = new String(b);
												//fileNameToString.put(fileName,tempfileString);
												return tempfileString;
									} catch (Exception e){
												e.printStackTrace();
												return null;
									} finally{
												try{
															if (br != null)
																		br.close();
												} catch (Exception e){
															System.err.println("Error reading/closing file " + fileName);
												}
									}
						} else{ //fileName = null
									return fileName;
						}

			}

			/**
				*  Returns the current URL to send Matrix connections to.
				*/

			public static URL getConnectionURL(){
						// If it's not initialized then initialize it
						if (connection_URL == null){
									setConnectionURL(MATRIX_URL);
						}
						// Return the URL
						return connection_URL;

			}

			/**
				* Sets the connection URL to the string specified
				*/

			public static void setConnectionURL(String url){
						try{
									// Set the URL
									connection_URL = new URL(url);
									// Catch some exceptions
						} catch (MalformedURLException e){
									try{
												System.out.println("Malformed URL, setting to default");
												connection_URL = new URL(MATRIX_URL);
									} catch (MalformedURLException m){
												// Print some errors
												System.out.println(e);
												e.printStackTrace();
									}
						}

			}

			/**
				* Sets the connection URL that the methods that don't accept an explicit URL use to connect to.
				*/

			public static void setConnectionURL(URL url){
						connection_URL = url;
			}

}