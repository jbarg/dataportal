package uk.ac.dl.cart;
import org.apache.axis.AxisFault;
import org.apache.axis.Handler;
import org.apache.axis.MessageContext;
import org.apache.axis.handlers.BasicHandler;

import java.io.FileOutputStream;
import java.io.PrintWriter;
import java.util.Date;
import java.io.*;

public class LogHandler extends BasicHandler {
    public void invoke(MessageContext msgContext) throws AxisFault
    {
        /** Log an access each time we get invoked.
         */
        try {
            Handler serviceHandler = msgContext.getService();
            //String filename = (String)getOption("filename");
            String filename = System.getProperty("catalina.home")+File.separator+"logs"+File.separator+"Shoppingcart.log";
            if ((filename == null) || (filename.equals("")))
                throw new AxisFault("Server.NoLogFile",
                                 "No log file configured for the LogHandler!",
                                    null, null);
            FileOutputStream fos = new FileOutputStream(filename, true);

            PrintWriter writer = new PrintWriter(fos);

            Integer numAccesses =
                             (Integer)serviceHandler.getOption("accesses");
            if (numAccesses == null)
                numAccesses = new Integer(0);

            numAccesses = new Integer(numAccesses.intValue() + 1);

            Date date = new Date();
	
			String address  = (String)msgContext.getProperty("remoteaddr");
			
            String result = date + ": service " +
                            msgContext.getTargetService() +
                            " accessed " + numAccesses + " time(s) from IP address "+address;
            serviceHandler.setOption("accesses", numAccesses);

            writer.println(result);

            writer.close();
        } catch (Exception e) {
            throw AxisFault.makeFault(e);
        }
    }
}
