//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v1.0 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2003.06.06 at 12:17:03 BST 
//


package edu.sdsc.matrix.srb.parser;


/**
 * Java content class for DataGridResponse element declaration.
 *  <p>The following schema fragment specifies the expected content contained within this java content object.
 * <p>
 * <pre>
 * &lt;element name="DataGridResponse">
 *   &lt;complexType>
 *     &lt;complexContent>
 *       &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *         &lt;sequence>
 *           &lt;element name="docMeta" type="{http://www.teragrid.org/schemas/datagrid}DocMeta"/>
 *           &lt;choice>
 *             &lt;element name="TransactionAcknowledgement" type="{http://www.teragrid.org/schemas/datagrid}TransactionAcknowledgment"/>
 *             &lt;element name="TransactionStatusResponse" type="{http://www.teragrid.org/schemas/datagrid}TransactionStatusResponse"/>
 *           &lt;/choice>
 *         &lt;/sequence>
 *       &lt;/restriction>
 *     &lt;/complexContent>
 *   &lt;/complexType>
 * &lt;/element>
 * </pre>
 * 
 */
public interface DataGridResponse
    extends javax.xml.bind.Element, edu.sdsc.matrix.srb.parser.DataGridResponseType
{


}
