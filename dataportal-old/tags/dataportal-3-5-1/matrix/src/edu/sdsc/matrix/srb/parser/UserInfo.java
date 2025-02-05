//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v1.0 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2003.06.06 at 12:17:03 BST 
//


package edu.sdsc.matrix.srb.parser;


/**
 * Java content class for UserInfo complex type.
 *  <p>The following schema fragment specifies the expected content contained within this java content object.
 * <p>
 * <pre>
 * &lt;complexType name="UserInfo">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;choice>
 *           &lt;element name="gridTicket" type="{http://www.teragrid.org/schemas/datagrid}GridTicketInfo"/>
 *           &lt;sequence>
 *             &lt;element name="userID" type="{http://www.w3.org/2001/XMLSchema}string"/>
 *             &lt;element name="organization" type="{http://www.teragrid.org/schemas/datagrid}OrganizationInfo"/>
 *             &lt;element name="challenge-Response" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *             &lt;element name="homeDirectory" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *             &lt;element name="defaultStorageResource" type="{http://www.w3.org/2001/XMLSchema}string" minOccurs="0"/>
 *           &lt;/sequence>
 *         &lt;/choice>
 *         &lt;element name="phoneNumber" type="{http://www.w3.org/2001/XMLSchema}int" minOccurs="0"/>
 *         &lt;element name="e-mail" type="{http://www.w3.org/2001/XMLSchema}anyURI" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 */
public interface UserInfo {


    java.lang.String getEMail();

    void setEMail(java.lang.String value);

    java.lang.String getHomeDirectory();

    void setHomeDirectory(java.lang.String value);

    int getPhoneNumber();

    void setPhoneNumber(int value);

    edu.sdsc.matrix.srb.parser.OrganizationInfo getOrganization();

    void setOrganization(edu.sdsc.matrix.srb.parser.OrganizationInfo value);

    java.lang.String getUserID();

    void setUserID(java.lang.String value);

    edu.sdsc.matrix.srb.parser.GridTicketInfo getGridTicket();

    void setGridTicket(edu.sdsc.matrix.srb.parser.GridTicketInfo value);

    java.lang.String getChallengeResponse();

    void setChallengeResponse(java.lang.String value);

    java.lang.String getDefaultStorageResource();

    void setDefaultStorageResource(java.lang.String value);

}
