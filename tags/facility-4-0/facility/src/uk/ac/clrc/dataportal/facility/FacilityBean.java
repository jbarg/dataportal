/*
 * createFacility.java
 *
 * Created on 09 January 2003, 13:29
 */

package uk.ac.clrc.dataportal.facility;

import java.util.Vector;
import java.util.Iterator;

/**
 *
 * @author  Mark Williams
 */
public class FacilityBean implements java.io.Serializable
{
    
    private String facilityName = null;
    private String facilityDesc = null;
    private String dataPortalID = null;
    private Vector servicesVector = new Vector();
    
    public String getFacilityName()
    {
        return this.facilityName;
    }
    public void setFacilityName(String facilityName)
    {
        this.facilityName = facilityName;
    }
    
    public String getDataPortalID()
    {
        return this.dataPortalID;
    }
    public void setDataPortalID(String dataPortalID)
    {
        this.dataPortalID = dataPortalID;
    }
    
    public String getFacilityDesc()
    {
        return this.facilityDesc;
    }
    public void setFacilityDesc(String facilityDesc)
    {
        this.facilityDesc = facilityDesc;
    }
    
    public void addServiceBean(String serviceName, String serviceDesc, String serviceType, String serviceEndpoint, String serviceWSDL)
    {
        ServiceBean sb = new ServiceBean(serviceName, serviceDesc, serviceType, serviceEndpoint, serviceWSDL);
        servicesVector.add(sb);
    }
    
    public Vector getServiceBeanVector() {
        return servicesVector;
    }
    
    public int getServiceBeanLength()
    {
        return servicesVector.size();
    }
    
    public String toString()
    {
        String beanContents = "[" + getFacilityName() + ", " + getFacilityDesc() + "]";
        ServiceBean sb = new ServiceBean();
        
        Iterator i = getServiceBeanVector().iterator();
        while (i.hasNext()) {
            sb = (ServiceBean) i.next();
            beanContents += ", " + sb.toString();
        }
        return beanContents;
    }
}
