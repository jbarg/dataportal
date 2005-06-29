
/**
 * <p>Title: WSDLgenerator </p>
 * <p>Description: Creates a WSDL for the Matrix installation </p>
 * <p>Copyright: Copyright (c) 2003</p>
 * <p>Company: </p>
 * @author Arun Jagatheesan (arun@sdsc.edu)
 * @version 1.0
 */
import java.io.BufferedInputStream;
import java.io.FileWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

public class WSDLgenerator {

		private static String getMatrixHome() throws IOException{

				return PropertiesManager.getEnvFirstTime("MATRIX_HOME");
		}

		public static void main(String args[]){
				int i=0;
				try{
						i = 1;
						String MatrixHome = getMatrixHome(); i++;
						System.out.println("Reading WSDX file: "+MatrixHome+File.separator+"schema"+File.separator+"matrix.wsdx");
						String fileString = readFile(MatrixHome+File.separator+"schema"+File.separator+"matrix.wsdx");
						System.out.println("Reading DGL Schema file: "+ MatrixHome+File.separator+"schema"+File.separator+"DGL.xsd");
						String schemaString = readFile(MatrixHome+File.separator+"schema"+File.separator+"DGL.xsd");
//						schemaString = schemaString.substring(
//											"<?xml version=\"1.0\" encoding=\"UTF-8\"?>".length,
//											schemaString.length() );
						int endPtr = schemaString.indexOf("?>")+2;
						schemaString = schemaString.substring(endPtr,schemaString.length());
						fileString = replacePlaceHolderWithValue(fileString, "_DGL.XSD_", schemaString);
						PropertiesManager propManager = new PropertiesManager("MATRIX_HOME",
										"conf" +File.separator+"matrix.properties", 1);
						String serverUrl = propManager.getPropertyWithDetail("server.url");
						if ( serverUrl == null || serverUrl.trim().equals(""))
								serverUrl = "http:////localhost:8080//matrix//receiver";
						fileString = replacePlaceHolderWithValue(fileString, "_MATRIX_SERVER_", serverUrl);
						System.out.println("Using Matrix server url "+serverUrl+ "from the matrix.properties file");
						System.out.println("\n\nWriting WSDL file into "+MatrixHome+"/matrix.wsdl");
						File file = new File(MatrixHome,"matrix.wsdl");
						FileWriter fw = new FileWriter(file);
						fw.write(fileString,0,fileString.length());
						fw.close();
				}catch(IOException ioe){
						ioe.printStackTrace();
						toLog("\n\n");
						if (i==1) toLog("Check if MATRIX_HOME property is set the directory where you have installed matrix."+
							"It is the directory where you will find \"build.xml\" ");
				}
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

		public static void toLog(String msg){

				System.out.println(msg);
		}

}