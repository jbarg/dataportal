package hello;

import java.net.URL;
import java.util.Vector;
/*import org.apache.soap.SOAPException;
import org.apache.soap.Constants;
import org.apache.soap.Fault;
import org.apache.soap.rpc.Call;
import org.apache.soap.rpc.Parameter;
import org.apache.soap.rpc.Response;
*/
import org.apache.soap.*;
import org.apache.soap.rpc.*;

public class Client{
	public static void main( String args[] ) throws Exception{
		if ( args.length == 0 ){
			System.err.println( "Usage: java hello.Client [SOAP-router-URL]" );
			System.exit(1);
		}
		try{
			URL url = null;
			String name = null;
			if ( args.length == 2 ){
				url = new URL( args[0] );
				name = args[1];
			} else {
				url = new URL( "http://localhost:8080/apache-soap/servlet/rpcrouter" );
				name = args[0];
			}

			//Build the call
			Call call = new Call();
			call.setTargetObjectURI( "urn:Hello" );
			call.setMethodName( "sayHelloTo" );
			call.setEncodingStyleURI( Constants.NS_URI_SOAP_ENC );
			Vector params = new Vector();
			params.addElement( new Parameter( "name", String.class, name, null ) );
			call.setParams( params );

			//Invoke the call
			Response resp = null;

			try{
				resp = call.invoke( url, "" );
			} catch( SOAPException e ){
				System.err.println( "Caught SOAPException (" +
						    e.getFaultCode() + "): " + e.getMessage() );
				System.exit(-1);
			}

			//Check the response
			if ( !resp.generatedFault() ){
				Parameter ret = resp.getReturnValue();
				Object value = ret.getValue();
				System.out.println( value );
			} else {
				Fault fault = resp.getFault();
				System.err.println( "Generated fault: " );
				System.out.println( "Fault Code = " + fault.getFaultCode() );
				System.out.println( "Fault String = " + fault.getFaultString() );
			}
		} catch( Exception e ){
			e.printStackTrace();
		}
	}
}
