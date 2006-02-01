/*
 * AttributeCertificate.java
 *
 * Created on July 16, 2003, 6:31 PM
 */

package uk.ac.cclrc.authorisation;

import org.jdom.*;
import org.jdom.input.*;
import org.jdom.output.*;
import java.io.*;
import java.util.*;
import ac.dl.xml.*;
import java.sql.*;
import org.apache.log4j.*;
import java.util.*;

/**
 *
 * @author  asm67
 *
 *This class reads XML String and converts to Attribute Certificate or can be used
 * to add data to generate Attribute Certificate
 */
public class AttributeList {
    
    /** Creates a new blank instance of AttributeCertificate */
    // public AttributeList() {
    // }
    
   /* public AttributeList(String userDn,String facilityName, String issuerDn, String notBefore, String  notAfter) throws ClassNotFoundException,SQLException {
        this.issuerName = facilityName;
        this.holder = userDn;
        this.issuer=issuerDn;
        this.validityNotAfter=notAfter;
        this.validityNotBefore=notBefore;
        this.loadFromDb(userDn,facilityName,issuerDn);
    }*/
    static Logger log = Logger.getLogger(AttributeList.class);
    
    // public AttributeList(String version, String userDn, String issuerDn, String issuerName, String signatureAlgorithm,
    // String issuerSerialNumber, int[] notBefore, int[] notAfter, String aDPView, String aWrapperGroup, String aDataAccessGroup) throws ClassNotFoundException,SQLException { NDB
    public AttributeList(String version, String userDn, String issuerDn, String issuerName, String signatureAlgorithm,
      String issuerSerialNumber, int[] notBefore, int[] notAfter, String aDataAccessGroup) throws ClassNotFoundException,SQLException {
       
    
        PropertyConfigurator.configure(Config.getContextPath()+"logger.properties");
        this.version = version;
        this.holder = userDn;
        this.issuer=issuerDn;
        this.issuerName = issuerName;
        this.signatureAlgorithm = signatureAlgorithm;
        this.issuerSerialNumber = issuerSerialNumber;
        
        this.validityNotAfter=notAfter;
        this.validityNotBefore=notBefore;
        // this.aDPView = aDPView; NDB
        // this.aWrapperGroup = aWrapperGroup; NDB
        this.aDataAccessGroup = aDataAccessGroup;
    }
    public AttributeList(org.jdom.Element acInfo) throws Exception {
        PropertyConfigurator.configure(Config.getContextPath()+"logger.properties");
        try {
            parseAttributeList(acInfo);
            
        } catch (Exception e) {
           
            throw e;
        }
    }
    
    public String getAcInfoAsXMLString() throws Exception{
        
        try {
            Document doc = this.getJDOMDocument();
            // Convert to XML String and Return
            XMLOutputter outputter = new XMLOutputter();
            return outputter.outputString(doc);
        } catch (Exception e) {
            log.error("Unable to create AttributeLost as String",e);
            throw e;
        }
    }
    
    /** This method is used to generate the ACInfo section of the authorisationToken
     *@param org.w3c.dom.Element  The resultSet Holding the one instance of the user
     *@exception    java.sql.SQLException
     */
    public org.w3c.dom.Element getAcInfoAsW3CElement() throws Exception{
        
        
        // Convert to XML String and Return
        //XMLOutputter outputter = new XMLOutputter();
        //return outputter.outputString(doc);
        try {
            Document doc = this.getJDOMDocument();
            org.w3c.dom.Document w3cDoc = Converter.JDOMtoDOM(doc);
            org.w3c.dom.Element el = w3cDoc.getDocumentElement();
            return el;
        } catch (Exception e) {
            log.error("Unable to create AttributeLost as Element",e);
            throw e;
        }
        
    }
    
    public Document getJDOMDocument() {

            Document doc = new Document();
            Element root = new Element("acInfo");
            
            
            //Build XML file
            doc.setRootElement(root);
            //set version
            Element versionElement = new Element("version");
            versionElement.setText(version);
            //set holder
            Element holderElement = new Element("holder");
            holderElement.setText(holder);
            //set issuer
            Element issuerElement = new Element("issuer");
            issuerElement.setText(issuer);
            //set issuer
            Element issuerNameElement = new Element("issuerName");
            issuerNameElement.setText(issuerName);
            //setissuerSerialNumber
            Element issuerSerialNumberElement = new Element("issuerSerialNumber");
            issuerSerialNumberElement.setText(issuerSerialNumber);
            //set SignatureAlgorithm
            Element signatureAlgorithmElement = new Element("signatureAlgorithm");
            signatureAlgorithmElement.setText(signatureAlgorithm);
            //set validity
            Element validityElement = new Element("validity");
            //set validity not before
            Element validityNotBeforeElement = new Element("notBefore");
            //turn array into string
            StringBuffer notBefore = new StringBuffer();
            for(int i = 0;i<validityNotBefore.length;i++){
                if(i == (validityNotBefore.length -1)) notBefore.append(validityNotBefore[i]);
                else notBefore.append(validityNotBefore[i]+" ");
            }
            
            validityNotBeforeElement.setText(notBefore.toString());
            //set validity not after
            Element validityNotAfterElement = new Element("notAfter");
            StringBuffer notAfter = new StringBuffer();
            for(int i = 0;i<validityNotAfter.length;i++){
                if(i == (validityNotAfter.length -1)) notAfter.append(validityNotAfter[i]);
                else notAfter.append(validityNotAfter[i]+" ");
            }
            
            validityNotAfterElement.setText(notAfter.toString());
            //set Attributes
            Element attributesElement = new Element("attributes");
            //set Attributes DP view
            // Element attributesDPViewElement = new Element("DPView"); NDB
            // attributesDPViewElement.setText(aDPView); NDB
            //set Attributes Wrapper Group
            // Element attributesWrapperGroupElement = new Element("wrapperGroup"); NDB
            // attributesWrapperGroupElement.setText(aWrapperGroup); NDB
            //set Attributes Data access Group
            Element attributesDataAccessGroupElement = new Element("dataAccessGroup");
            attributesDataAccessGroupElement.setText(aDataAccessGroup);
            
            //Create XML tree
            root.addContent(versionElement);
            root.addContent(holderElement);
            root.addContent(issuerElement);
            root.addContent(issuerNameElement);
            root.addContent(issuerSerialNumberElement);
            root.addContent(signatureAlgorithmElement);
            validityElement.addContent(validityNotBeforeElement);
            validityElement.addContent(validityNotAfterElement);
            root.addContent(validityElement);
            // attributesElement.addContent(attributesDPViewElement); NDB
            // attributesElement.addContent(attributesWrapperGroupElement); NDB
            attributesElement.addContent(attributesDataAccessGroupElement);
            root.addContent(attributesElement);
            return doc;
        
    }
    
    public boolean isValid()  {
        try {
            
            /*double creationTime = Double.parseDouble(validityNotBefore);
            double validTill = Double.parseDouble(validityNotAfter);
            System.out.println(validTill +" - "+creationTime);
            System.out.println(validTill -creationTime);
            System.out.println();
            System.out.println((validTill - creationTime));
            return (validTill - creationTime)/1000;*/
            
            GregorianCalendar notBeforeCal = new GregorianCalendar(validityNotBefore[0],validityNotBefore[1],validityNotBefore[2],validityNotBefore[3],validityNotBefore[4],validityNotBefore[5]);
            GregorianCalendar notAfterCal = new GregorianCalendar(validityNotAfter[0],validityNotAfter[1],validityNotAfter[2],validityNotAfter[3],validityNotAfter[4],validityNotAfter[5]);
            GregorianCalendar now = new GregorianCalendar();
            
            if(now.before(notBeforeCal)) return false;
            if(now.after(notAfterCal)) return false;
            return true;
            
            
        } catch (Exception e) {
            log.error("Unable to check validilty time",e);
            return false;
            
        }
    }
    public void setVersion(String version){
        this.version = version;
    }
    public String getVersion(){
        return this.version;
    }
    
    public void setHolder(String holder){
        this.holder = holder;
    }
    public String getHolder(){
        return this.holder;
    }
    public void setIssuer(String issuer){
        this.issuer = issuer;
    }
    public String getIssuer(){
        return issuer;
    }
    public void setIssuerName(String issuerName){
        this.issuerName=issuerName;
    }
    public String getIssuerName(){
        return this.issuerName;
    }
    public void setSignatureAlgorithm(String sigAglorithm){
        this.signatureAlgorithm=sigAglorithm;
    }
    public String getSignatureAlgorithm(){
        return signatureAlgorithm;
    }
    public void setIssuerSerialNumber(String serialNumber){
        this.issuerSerialNumber=serialNumber;
    }
    public String getIssuerSerialNumber(){
        return this.issuerSerialNumber;
    }
    public void setValidityNotBefore(int[] notBefore){
        this.validityNotBefore=notBefore;
    }
    public int[] getValidityNotBefore(){
        return this.validityNotBefore;
    }
    public void setValidityNotAfter(int[] notAfter){
        this.validityNotAfter=notAfter;
    }
    public int[] getValidityNotAfter(){
        return this.validityNotAfter;
    }
    /* NDB
    public void setDPView(String aDPView){
        this.aDPView=aDPView;
    }
    public String getDPView(){
        return this.aDPView;
    }
    public void setWrapperGroup(String aWrapperGroup){
        this.aWrapperGroup=aWrapperGroup;
    }
    public String getWrapperGroup(){
        return aWrapperGroup;
    }
     */
    public void setDataAccessGroup(String aDataAccessGroup){
        this.aDataAccessGroup=aDataAccessGroup;
    }
    public String getDataAccessGroup(){
        return aDataAccessGroup;
    }
    
    private void parseAttributeList(Element acInfo) throws Exception {
        try {
            
            List elementList = acInfo.getChildren();
            Iterator iterator = elementList.iterator();
            String elementName;
            while (iterator.hasNext()) {
                Element next = (Element) iterator.next();
                elementName=next.getName();
                
                if(elementName.equals("version")){
                    
                    this.version = next.getText().trim();
                }
                if(elementName.equals("holder")){
                    
                    this.holder = next.getText().trim();
                }
                if(elementName.equals("issuer")){ this.issuer = next.getText().trim(); }
                if(elementName.equals("issuerName")){this.issuerName = next.getText().trim() ;}
                if(elementName.equals("issuerSerialNumber")){ this.issuerSerialNumber = next.getText().trim();}
                if(elementName.equals("signatureAlgorithm")){ this.signatureAlgorithm = next.getText().trim();}
                if(elementName.equals("validity")){
                    
                    List validityList = next.getChildren();
                    Iterator validityIterator = validityList.iterator();
                    while (validityIterator.hasNext()) {
                        
                        Element validityNext = (Element) validityIterator.next();
                        String validityElementName = validityNext.getName();
                        
                        if(validityElementName.equals("notBefore")){
                            String notB = validityNext.getText().trim();
                            String[] notBS = notB.split(" ");
                            int[] notBI = new int[notBS.length];
                            for(int i= 0; i<notBS.length;i++){
                                notBI[i] = Integer.parseInt(notBS[i]);
                                
                            }
                            this.validityNotBefore = notBI;
                        }
                        if(validityElementName.equals("notAfter")){
                            String notA = validityNext.getText().trim();
                            String[] notAS = notA.split(" ");
                            int[] notAI = new int[notAS.length];
                            for(int i= 0; i<notAS.length;i++){
                                notAI[i] = Integer.parseInt(notAS[i]);
                                
                            }
                            this.validityNotAfter = notAI;
                            
                            
                        }
                        
                    }
                }
                
                if(elementName.equals("attributes")){
                    
                    List attributeList = next.getChildren();
                    Iterator attributeIterator = attributeList.iterator();
                    while (attributeIterator.hasNext()) {
                        
                        Element attribNext = (Element) attributeIterator.next();
                        String attribElementName = attribNext.getName();
                        /* NDB
                        if(attribElementName.equals("DPView")){
                            
                            this.aDPView = attribNext.getText().trim();
                        }
                        if(attribElementName.equals("wrapperGroup")){ this.aWrapperGroup = attribNext.getText().trim();}
                         */
                        if(attribElementName.equals("dataAccessGroup")){ this.aDataAccessGroup = attribNext.getText().trim();}
                        
                    }
                    ;
                    
                    
                }
            }
            
        } catch (Exception e) {
            log.error("Unable to parse AttibuteList",e);
            throw e;
        }
    }
    
    private String version;
    private String holder;
    private String issuer;
    private String issuerName;
    private String signatureAlgorithm;
    private String issuerSerialNumber;
    private int[] validityNotBefore;
    private int[] validityNotAfter;
    
    // private String aDPView; NDB
    // private String aWrapperGroup; NDB
    private String aDataAccessGroup;
    
    
}
