/*
 * CreateTModels.java
 *
 * Created on 04 February 2003, 09:13
 */

package uk.ac.clrc.dataportal.uddisetup;

import java.util.Vector;
import java.util.Enumeration;

import org.uddi4j.*;
import org.uddi4j.client.*;
import org.uddi4j.datatype.*;
import org.uddi4j.datatype.binding.*;
import org.uddi4j.datatype.business.*;
import org.uddi4j.datatype.service.*;
import org.uddi4j.datatype.tmodel.*;
import org.uddi4j.request.*;
import org.uddi4j.response.*;
import org.uddi4j.util.*;
import org.uddi4j.transport.TransportFactory;

import java.util.Properties;
import java.io.*;
/**
 *
 * @author  maw24
 */
public class CreateTModels
{
    
    public static void main(String[] args) throws Exception
    {
        if (args.length != 2)
        {
            throw new Exception("Syntax:  CreateTmodels <serviceTypes properties filename> <TModel keys properties filename>");
        }
        
        try
        {
            Properties servTypeProps = new Properties();
            /*
             * Just in case we need to run without a properties file!
            servTypeProps.setProperty("ACL","Access Control List");
            servTypeProps.setProperty("XMLW","XML Wrapper");
            servTypeProps.setProperty("QNR","Query and Reply");
            servTypeProps.setProperty("AUTH","Authentication");
            servTypeProps.setProperty("LOOKUP","Service Lookup");
            servTypeProps.setProperty("FACADM","Facilities Administation");
            servTypeProps.setProperty("CART","Shopping Cart");
            servTypeProps.setProperty("RASGRIB","RasDaMan");
            servTypeProps.setProperty("DTS","Data Transfer");
            */
            
            FileInputStream in = new FileInputStream(args[0]);
            servTypeProps.load(in);
            in.close();
            
            UDDIHelper uh = new UDDIHelper();
            uh.createTModels(servTypeProps);
            Properties tmKeyProps = uh.getTModelKeyProps();
            
            FileOutputStream out = new FileOutputStream(args[1]);
            
            tmKeyProps.store(out, "UDDI TModel Keys for DataPortal Service Types");
            out.close();
        }
        catch (FileNotFoundException fnfe)
        {
            System.err.println(fnfe.getMessage());
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }
}
