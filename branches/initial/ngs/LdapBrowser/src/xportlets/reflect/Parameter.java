/* -*- mode: Java; c-basic-offset: 4; indent-tabs-mode: nil; -*-  //------100-columns-wide------>|*/
/*
 * Copyright (c) 2002 Extreme! Lab, Indiana University. All rights reserved.
 *
 * This software is open source. See the bottom of this file for the licence.
 *
 * $Id$
 */


package xportlets.reflect;

/**
 * This class wraps the parameter value and type
 * @version $Revision$  $Author$ $Date$ (GMT)
 * @author ochipara[mailto:ochipara@cs.indiana.edu]
 */

import java.io.StringReader;
import org.gjt.xpp.XmlNode;
import org.gjt.xpp.XmlPullParser;
import org.gjt.xpp.XmlPullParserFactory;

public class Parameter
{
    private String name;
    private String typeName;
    private Object value;
    
    
    /**
     * Method checkSupportedType
     * One of the current limitations of the invoking remote services via ldap
     * is that we do not allow the invokation of any method whose arguments are
     * more complex than the basic java types such as: int, double, long, etc.
     * This method imposes this requiremnt
     *
     * @param    type                a  String
     *
     * @return   a boolean that is true when the type is supported
     *
     */
    
    private boolean checkSupportedType(String type)
    {
        boolean supported = false;
        
        if ("java.lang.String".equals(type)) supported = true;
        else if ("int".equals(type)) supported = true;
        else if ("byte".equals(type)) supported = true;
        else if ("double".equals(type)) supported = true;
        else if ("float".equals(type)) supported = true;
        else if ("long".equals(type)) supported = true;
        else if ("short".equals(type)) supported = true;
        else if ("java.lang.Byte".equals(type)) supported = true;
        else if ("java.lang.Double".equals(type)) supported = true;
        else if ("java.lang.Float".equals(type)) supported = true;
        else if ("java.lang.Integer".equals(type)) supported = true;
        else if ("java.lang.Long".equals(type)) supported = true;
        else if ("java.lang.Short".equals(type)) supported = true;
        else if ("org.gjt.xpp.XmlNode".equals(type)) supported = true;
        
        return supported;
    }
    
    public Parameter(String name, Class type)
        throws UnsupportedTypeException
    {
        this.name = name;
        if (!checkSupportedType(type.getName())) throw new UnsupportedTypeException();
        this.typeName = type.getName();
    }
    
    
    
    /**
     * Method setName
     *
     * @param    name                a  String
     *
     */
    public void setName(String name)
    {
        this.name = name;
    }
    
    
    
    /**
     * Method getName
     *
     * @return   a String
     *
     */
    
    public String getName()
    {
        return name;
    }
    
    /**
     * Method setTypeName
     *
     * @param    typeName            a  String
     *
     */
    public void setTypeName(String typeName)
    {
        this.typeName = typeName;
    }
    
    
    
    /**
     * Method getTypeName
     *
     * @return   a String
     *
     */
    public String getTypeName()
    {
        return typeName;
    }
    
    
    
    /**
     * Method setValue
     * The method set value gets a String and converts it to the appropriate type
     * and stores it in the type object
     *
     * @param    value               a  String
     *
     */
    public void setValue(String value)
    {
        if ("java.lang.String".equals(typeName)) this.value = value;
        else if ("int".equals(typeName)) this.value = new Integer(value);
        else if ("byte".equals(typeName)) this.value = new Byte(value);
        else if ("double".equals(typeName)) this.value = new Double(value);
        else if ("float".equals(typeName)) this.value = new Float(value);
        else if ("long".equals(typeName)) this.value = new Long(value);
        else if ("short".equals(typeName)) this.value = new Short(value);
        else if ("java.lang.Byte".equals(typeName)) this.value = new Byte(value);
        else if ("java.lang.Double".equals(typeName)) this.value = new Double(value);
        else if ("java.lang.Float".equals(typeName)) this.value = new Float(value);
        else if ("java.lang.Integer".equals(typeName)) this.value = new Integer(value);
        else if ("java.lang.Long".equals(typeName)) this.value = new Long(value);
        else if ("java.lang.Short".equals(typeName)) this.value = new Short(value);
        else if ("org.gjt.xpp.XmlNode".equals(typeName)) this.value = new String(value);
    }
    
    
    
    /**
     * Method getValue
     * We are careful to do type casts for ints, bytes, floats ...
     * @return   an Object coutaining the value of the paramter
     *
     */
    public Object getValue()
        throws org.gjt.xpp.XmlPullParserException, java.io.IOException
    {
        if ("org.gjt.xpp.XmlNode".equals(typeName))
        {
            // invoke a find service data query
            XmlPullParserFactory xppFactory = XmlPullParserFactory.newInstance();
            XmlPullParser xpp0 = xppFactory.newPullParser();
            xpp0.setInput(new StringReader((String)value));
            xpp0.setNamespaceAware(true);
            xpp0.next();
            XmlNode node= xppFactory.newNode(xpp0);
            return node;
            
        }
        return value;
    }
    
    public String toString()
    {
        String s;
        
        s = this.name + "[" + this.typeName + "]=" + this.value;
        
        
        return s;
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


