//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v1.0 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2003.06.06 at 12:17:03 BST 
//


package edu.sdsc.matrix.srb.parser;


/**
 * Java content class for DownloadDataSetOp complex type.
 *  <p>The following schema fragment specifies the expected content contained within this java content object.
 * <p>
 * <pre>
 * &lt;complexType name="DownloadDataSetOp">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;choice>
 *         &lt;element name="StdParams" type="{http://www.teragrid.org/schemas/datagrid}DownloadDataSetParams"/>
 *         &lt;element name="FlexParams" type="{http://www.teragrid.org/schemas/datagrid}ParamList"/>
 *       &lt;/choice>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 */
public interface DownloadDataSetOp {


    edu.sdsc.matrix.srb.parser.ParamList getFlexParams();

    void setFlexParams(edu.sdsc.matrix.srb.parser.ParamList value);

    edu.sdsc.matrix.srb.parser.DownloadDataSetParams getStdParams();

    void setStdParams(edu.sdsc.matrix.srb.parser.DownloadDataSetParams value);

}
