import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.OutputStream;
import java.net.URL;
import java.util.Iterator;

import javax.xml.bind.JAXBContext;
import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;
import javax.xml.soap.MessageFactory;
import javax.xml.soap.SOAPConnection;
import javax.xml.soap.SOAPConnectionFactory;
import javax.xml.soap.SOAPMessage;
import javax.xml.soap.SOAPPart;
import javax.xml.transform.Source;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;

import edu.sdsc.matrix.srb.parser.DataGridResponse;
import edu.sdsc.matrix.srb.parser.FlowStatusResponse;
import edu.sdsc.matrix.srb.parser.RequestAcknowledgment;
import edu.sdsc.matrix.srb.parser.StepStatusResponse;

	/**
	* <p>Title: DGL Sender</p>
	* <p>Description: The Matrix Client API can be used to programatically create DGL
	* documents and send it to the Matrix Server. Additionally DGL documents could be
	* created using XML editors like XMLSpy and this program can be used ot send the DGL
	* to the Matrix Server. It takes the DGL and makes a SOAP request to the server. This
	* program can also be used enquire the status of matrix workflows.</p>
	* <p>Copyright: Copyright (c) 2003 </p>
	* <p>Company: San Diego Supercomputer Center </p>
	* @author Arun Jagatheesan (arun@sdsc.edu)
	* @version 1.0
	*/

	/* Developer Log
		* Original code - (long time back) Arun
		* Multiple modifications - Arun
		* Added this developer log 1/31/05 Arun
		*/

public class DGLSender {

		private static Unmarshaller unmarshaller;
		private static  Marshaller marshaller;
		private static String endpoint = "http://localhost:8080/matrix/receiver";
		private static PropertiesManager propManager;
		private static String dglName = "lastRequest.dgl";


		private static final void usage(){
				toLog("\n\n");
				toLog("Usage: java DGLSender ( dgl file | 'status':flow-id) "+
							" [Matrix-server-url]");
				toLog("\n");
				toLog("arg1 is either the name of the dgl file or the word \"status\" ");
				toLog("dgl-file: dgl file made using an editor like XMLSpy to be sent to server");
				toLog("status:flow-id = whose status has to be enquired in the server.");
				toLog("The format to enquire status is: status:xxxxxxxxx, where xx is the flow-id");
				toLog("\n");
				toLog("arg2 is Matrix-server-url and is optional");
				toLog("URL where matrix is running");
				toLog("If not provided the default is looked up from the matrix.properties file that");
				toLog("is present in the conf directory. If the matrix.properties file could");
				toLog("be found,  the default is \"http://localhost:8080/matrix/receiver\" ");
				toLog("\n");
				toLog("Examples:");
				toLog("java DGLSender myDgl.dgl ");
				toLog("java DGLSender status:23299");
				toLog("java DGLSender status:34394 \"http://someserver.sdsc.edu:8080/matrix/receiver\" ");
				toLog("\n");
				return;

		}

		public static void main(String[] args) {
			String REQUEST_FILE = "c:\\matrix\\src\\test.xml";
			boolean getStatus = false;
			File tempFile = null;

			if (args.length > 0) {
					if (args[0].startsWith("status:")){
						//its a status request
						getStatus = true;
					}else{ //its a flow request
							REQUEST_FILE = args[0];
					}
					if (args.length > 1) {
						endpoint = args[1];
					}else{
							try {
									propManager = new PropertiesManager(
										"MATRIX_HOME",
										"conf" +
										File.separator
										+
										"matrix.properties", 1);
									endpoint = propManager.
										getPropertyWithDetail("server.url");
									if (endpoint == null)
											throw new FileNotFoundException();
							} catch (Exception e) {
									endpoint =
										"http://localhost:8080/matrix/receiver";
									toLog(
										"No matrix server url parameter given");
							} finally {
									toLog("Using matrix server: "+endpoint);
							}
					}
			}else{
					usage();
					return;
			}
			try {
				SOAPConnectionFactory scf = SOAPConnectionFactory.newInstance();
				SOAPConnection connection = scf.createConnection();
				MessageFactory mf = MessageFactory.newInstance();
				SOAPMessage msg = mf.createMessage();
				SOAPPart soapPart = msg.getSOAPPart();
				Source source = null;
				if (getStatus){
						String fileString = readFile( PropertiesManager.getEnvFirstTime("MATRIX_HOME")
						+File.separator+"examples"+File.separator+"Using-XMLeditor"+File.separator+
						"dgl3-GetResponse.dgl");
						if (fileString == null){
								toLog("Unable to read the template \"GetResponse.dgl\" to get status");
								toLog("exiting...");
								return;
						}else{
								//System.out.println("gimme a break. Yes I was here "+args[0].substring("status:".length()).trim());
								fileString = replacePlaceHolderWithValue(fileString,"_FLOW-ID_"
									,args[0].substring("status:".length()).trim());
								tempFile = new File("TempResponse.dgl");
								FileWriter fw = new FileWriter(tempFile);
								//source = new StreamSource(new StringReader(fileString));
								// for some reason the above didnot work. since i could not spend
								// time on the release week to explore tat, i had to store it in a file :(.
								fw.write(fileString,0,fileString.length());
								REQUEST_FILE = tempFile.getAbsolutePath();
								//System.out.println(REQUEST_FILE);
								//System.out.println(fileString);
								fw.close();
						}
				}
				source = new StreamSource(new FileInputStream(REQUEST_FILE));
				soapPart.setContent(source);
				System.out.println("\n\n");
				System.out.println("");
				System.out.println("Sending DGL message to: " + endpoint);
				printReply(msg);
				SOAPMessage reply = connection.call(msg, new URL(endpoint));
				System.out.println("\n\n");
				System.out.println("Received DGL Reply from: " +
										endpoint);

				printReply(reply);
				//if (tempFile != null) tempFile.delete();
				JAXBContext jc = JAXBContext.newInstance(
						"edu.sdsc.matrix.srb.parser");
				unmarshaller = jc.createUnmarshaller();
				Source replyxml = reply.getSOAPPart().getContent();
				DataGridResponse dgresponse = (DataGridResponse) unmarshaller.
						unmarshal(
						replyxml);

						// unnecessary marshalling here
					marshaller = jc.createMarshaller();
					System.out.println("\nWriting DGL file : " + dglName);
					OutputStream os = new FileOutputStream(dglName);
					marshaller.marshal(dgresponse, os);
					System.out.println("You may wish to browse the DGL request/respone file:"+ dglName);
				RequestAcknowledgment ack = dgresponse.getRequestAcknowledgement();
				if(ack != null) { //its a status response
						String flowId = ack.getRequestID();
						toLog("\n\nYour Flow ID : " + flowId);
						toLog("To Check Status use:\n java DGLSender status:"+flowId
									+" "+endpoint);
				}else{
						System.out.println("\n\nResponse XML converted to viewable format:");
						FlowStatusResponse fsr = dgresponse.getFlowStatusResponse();
						if (fsr != null) printResponse(fsr,0);
				}
			} catch (FileNotFoundException e) {
				e.printStackTrace();
				usage();
				return;
			} catch (Exception e) {
					System.out.println(e.toString());
				//e.printStackTrace();
				usage();
				return;
			}
		}

		private static void printReply(SOAPMessage reply) throws Exception {
			TransformerFactory tFact = TransformerFactory.newInstance();
			Transformer transformer = tFact.newTransformer();
			Source src = reply.getSOAPPart().getContent();
			StreamResult result = new StreamResult(System.out);
			transformer.transform(src, result);
			System.out.println();
		}

		private static void toLog(String msg){
				System.out.println(msg);
		}

		public static String readFile(String fileName) {
				String fileString = null; // = (String)fileNameToString.get(fileName);
				//System.err.println("The file size is ..."+fileString.length());
				if (fileString == null) {
						BufferedInputStream br = null;
						try {
								File file = new File(fileName);
								int length = (int) file.length();
								br = new BufferedInputStream(new
									FileInputStream(file));
								byte b[] = new byte[length];
								br.read(b);
								String tempfileString = new String(b);
								//fileNameToString.put(fileName,tempfileString);
								return tempfileString;
						} catch (Exception e) {
								e.printStackTrace();
								return null;
						} finally {
								try {
										if (br != null)	br.close();
								} catch (Exception e) {
								}
						}
				} else	return fileString;
		}

		public static String replacePlaceHolderWithValue(String file,
			String placeHolder, String value) {
				try {
						if (value == null)	value = "";
						int index = file.indexOf("^" + placeHolder);
						while (index != -1) {
								int carIndex = file.indexOf("^", index + 1);
								file = file.substring(0, index) + value +
									file.substring(carIndex + 1, file.length());
								index = file.indexOf("^" + placeHolder, index);
						}
						return file;
				} catch (Exception e) {
						return file;
				}
		}

		public static void printResponse(FlowStatusResponse fsr, int level){
						for(int i=0; i < level; i++) System.out.print("\t");
						toLog("Flow " + fsr.getID());
						if (fsr.getFlowStatusResponse().size() == 0){
								if (fsr.getStepStatusResponse().size() == 0) {
										for (int i = -1; i <= level; i++)											System.out.print("\t");
										toLog("\t STEPS NOT STARTED or NOT FOUND");
								}
								for (Iterator i2 = fsr.getStepStatusResponse().iterator();
										i2.hasNext(); ) {
										StepStatusResponse ssr = (StepStatusResponse)
											i2.next();
										for (int i = 0; i <= level; i++)
												System.out.print("\t");
										toLog("Step:"+ssr.getID() + "\t" +
													ssr.getStatusCode()
													+ "\t" +
													translateStatusCode(ssr.getStatusCode())
													);
								}
								toLog("\n");
						}else{
								for(Iterator i1 = fsr.getFlowStatusResponse().iterator(); i1.hasNext();){
										FlowStatusResponse f2 = (FlowStatusResponse) i1.next();
										printResponse(f2,level+1);
								}
						}
				}

		/**
			* Decipher those status codes
			*/
		public static String translateStatusCode(int statusCode) {
			switch(statusCode){
				case (9000016): return "THREAD_NOT_FOUND_OR_NOT_STARTED";
				case (9000017): return "THREAD_STARTED";
				case (9000018): return "THREAD_COMPLETE";
				case (9000019): return "THREAD_ABORTED_DUE_TO_ERROR_IN_PREPROCESS";
				case (9000020): return "THREAD_ABORTED_DUE_TO_ERROR_IN_PROCESS";
				case (9000021): return "THREAD_ABORTED_DUE_TO_ERROR_IN_POSTPROCESS";
				case (9000022): return "THREAD_ABORTED_DUE_TO_REQUEST_FROM_USER";
				default: return "UNKNOWN THREAD STATE" +
					"\nPlease e-mail talk2matrix@sdsc.edu about this"; //not returned

			}//switch
		}//translateStatusCode
}
