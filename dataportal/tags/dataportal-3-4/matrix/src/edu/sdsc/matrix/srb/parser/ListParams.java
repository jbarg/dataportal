//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v1.0 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2003.06.06 at 12:17:03 BST 
//


package edu.sdsc.matrix.srb.parser;


/**
 * Java content class for ListParams complex type.
 *  <p>The following schema fragment specifies the expected content contained within this java content object.
 * <p>
 * <pre>
 * &lt;complexType name="ListParams">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="DataSource" type="{http://www.teragrid.org/schemas/datagrid}StdDatagridObject"/>
 *         &lt;element name="recursive" type="{http://www.w3.org/2001/XMLSchema}boolean" minOccurs="0"/>
 *         &lt;element name="extendedInfoLevel" type="{http://www.w3.org/2001/XMLSchema}int" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 */
public interface ListParams {


    edu.sdsc.matrix.srb.parser.StdDatagridObject getDataSource();

    void setDataSource(edu.sdsc.matrix.srb.parser.StdDatagridObject value);

    boolean isRecursive();

    void setRecursive(boolean value);

    int getExtendedInfoLevel();

    void setExtendedInfoLevel(int value);

}