/* -*- mode: Java; c-basic-offset: 4; indent-tabs-mode: nil; -*-  //------100-columns-wide------>|*/

/*

 * Copyright (c) 2002 Extreme! Lab, Indiana University. All rights reserved.

 *

 * This software is open source. See the bottom of this file for the licence.

 *

 * $Id$

 */


package xportlets.reflect;

import java.lang.reflect.Method;
import java.lang.reflect.Modifier;
import java.util.Vector;
import java.util.Hashtable;
import soaprmi.server.RemoteRef;
import soaprmi.port.*;
import org.apache.log4j.*;

public class ReflectUtil
{
    /**
     * Method processInterface
     *
     * @param    c                   the class we will inspect.
     *
     * @return   a Vector containing the public methods implemented by the serivice
     *
     */
    private static Logger logger = Logger.getLogger(ReflectUtil.class);
    
    public static Vector processInterface(RemoteRef ref)
    {
        //first we get the port description
        
        Port port = ref.getSoapRMIPort();
        
        //we fetch the name of the interface that we implement
        
        String interfaceName = port.getPortType().getName();
        
        Class[] implementedInterfaces = ref.getClass().getInterfaces();
        
        
        
        //we use introspection on the method to derive the parameters
        
        Vector result = new Vector();
        
        for (int i = 0; i < implementedInterfaces.length; i++)
            
        {
            
            if (implementedInterfaces[i].getName().equals(interfaceName))
                
            {
                
                Method[] methods = implementedInterfaces[i].getMethods();
                
                for (int j = 0; j < methods.length; j++)
                    
                {
                    
                    int modifier = methods[j].getModifiers();
                    
                    if (Modifier.isPublic(modifier)) result.add(methods[j]);
                    
                }
                
            }
            
            
            
        }
        
        
        
        
        
        return result;
        
    }
    
    
    
    
    
    /**
    
     * Method processMethod
    
     *
    
     * @param    method              a  Method that we inspect creating a list of arguments that it uses
    
     *
    
     * @return   a Vector containing the @see Parameters the the class uses
    
     *
    
     * @throws   UnsupportedTypeException
    
     *
    
     */
    
    public static Vector processMethod(Method method)
        
        throws UnsupportedTypeException
        
    {
        
        Vector args = new Vector();
        
        int id = 0;
        
        
        
        Class[] params = method.getParameterTypes();
        
        for (int i = 0; i < params.length; i++)
            
        {
            
            args.add(new Parameter("parameter" + id, params[i]));
            
            id++;
            
        }
        
        
        
        return args;
        
    }
    
    
    
    
    
    /**
    
     * Method invokeService
    
     * Invokes a method using a remote reference. The parameters are in a vector
    
     *
    
     * @param    ref                 a  RemoteRef
    
     * @param    method              a  Method
    
     * @param    parameters          a  Vector
    
     *
    
     * @return   an Object
    
     *
    
     */
    
    public static Object invokeService(RemoteRef ref, Method method, Vector parameters)
        throws InvokationException
        
    {
        Object result = null;
        
        //construct the vector of values
        Vector parameterValuesVector = new Vector();
        for (int i = 0; i < parameters.size(); i++)
        {
            try
            {
                parameterValuesVector.add( ((Parameter) parameters.elementAt(i)).getValue());
            }
            catch (java.io.IOException e) { throw new InvokationException(e);}
            catch (org.gjt.xpp.XmlPullParserException e) {throw new InvokationException(e);}
        }
        
        Object[] parameterValues = new Object[parameterValuesVector.size()];
        
        for (int i = 0; i < parameterValuesVector.size(); i++)
        {
            parameterValues[i] = parameterValuesVector.elementAt(i);
        }
        
        
        
        try
        {
            //            Method m = ref.getClass().getMethod(method, parameterTypes);
            result = method.invoke(ref, parameterValues);
        }
        catch (SecurityException e)
        {
            e.printStackTrace();
            throw new InvokationException(e);
        }
        catch (IllegalArgumentException e)
        {
            e.printStackTrace();
            throw new InvokationException(e);
        }
        catch (java.lang.reflect.InvocationTargetException e)
        {
            e.printStackTrace();
            throw new InvokationException(e);
        }
        catch (IllegalAccessException e)
        {
            e.printStackTrace();
            throw new InvokationException(e);
        }
        
        return result;
    }
    
}

/*

 * Indiana University Extreme! Lab Software License, Version 1.2

 *

 * Copyright (C) 2002 The Trustees of Indiana University.

 * All rights reserved.

 *

 * Redistribution and use in source and binary forms, with or without

 * modification, are permitted provided that the following conditions are

 * met:

 *

 * 1) All redistributions of source code must retain the above

 *    copyright notice, the list of authors in the original source

 *    code, this list of conditions and the disclaimer listed in this

 *    license;

 *

 * 2) All redistributions in binary form must reproduce the above

 *    copyright notice, this list of conditions and the disclaimer

 *    listed in this license in the documentation and/or other

 *    materials provided with the distribution;

 *

 * 3) Any documentation included with all redistributions must include

 *    the following acknowledgement:

 *

 *      "This product includes software developed by the Indiana

 *      University Extreme! Lab.  For further information please visit

 *      http://www.extreme.indiana.edu/"

 *

 *    Alternatively, this acknowledgment may appear in the software

 *    itself, and wherever such third-party acknowledgments normally

 *    appear.

 *

 * 4) The name "Indiana Univeristy" or "Indiana Univeristy

 *    Extreme! Lab" shall not be used to endorse or promote

 *    products derived from this software without prior written

 *    permission from Indiana University.  For written permission,

 *    please contact http://www.extreme.indiana.edu/.

 *

 * 5) Products derived from this software may not use "Indiana

 *    Univeristy" name nor may "Indiana Univeristy" appear in their name,

 *    without prior written permission of the Indiana University.

 *

 * Indiana University provides no reassurances that the source code

 * provided does not infringe the patent or any other intellectual

 * property rights of any other entity.  Indiana University disclaims any

 * liability to any recipient for claims brought by any other entity

 * based on infringement of intellectual property rights or otherwise.

 *

 * LICENSEE UNDERSTANDS THAT SOFTWARE IS PROVIDED "AS IS" FOR WHICH

 * NO WARRANTIES AS TO CAPABILITIES OR ACCURACY ARE MADE. INDIANA

 * UNIVERSITY GIVES NO WARRANTIES AND MAKES NO REPRESENTATION THAT

 * SOFTWARE IS FREE OF INFRINGEMENT OF THIRD PARTY PATENT, COPYRIGHT, OR

 * OTHER PROPRIETARY RIGHTS.  INDIANA UNIVERSITY MAKES NO WARRANTIES THAT

 * SOFTWARE IS FREE FROM "BUGS", "VIRUSES", "TROJAN HORSES", "TRAP

 * DOORS", "WORMS", OR OTHER HARMFUL CODE.  LICENSEE ASSUMES THE ENTIRE

 * RISK AS TO THE PERFORMANCE OF SOFTWARE AND/OR ASSOCIATED MATERIALS,

 * AND TO THE PERFORMANCE AND VALIDITY OF INFORMATION GENERATED USING

 * SOFTWARE.

 */



