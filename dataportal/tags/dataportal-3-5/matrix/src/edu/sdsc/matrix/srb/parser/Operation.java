//
// This file was generated by the JavaTM Architecture for XML Binding(JAXB) Reference Implementation, v1.0 
// See <a href="http://java.sun.com/xml/jaxb">http://java.sun.com/xml/jaxb</a> 
// Any modifications to this file will be lost upon recompilation of the source schema. 
// Generated on: 2003.06.06 at 12:17:03 BST 
//


package edu.sdsc.matrix.srb.parser;


/**
 * Java content class for Operation complex type.
 *  <p>The following schema fragment specifies the expected content contained within this java content object.
 * <p>
 * <pre>
 * &lt;complexType name="Operation">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;choice>
 *         &lt;element name="CopyOp" type="{http://www.teragrid.org/schemas/datagrid}CopyOp"/>
 *         &lt;element name="ChangePermissionOp" type="{http://www.teragrid.org/schemas/datagrid}ChangePermissionOp"/>
 *         &lt;element name="CreateOp" type="{http://www.teragrid.org/schemas/datagrid}CreateOp"/>
 *         &lt;element name="DeleteOp" type="{http://www.teragrid.org/schemas/datagrid}DeleteOp"/>
 *         &lt;element name="RenameOp" type="{http://www.teragrid.org/schemas/datagrid}RenameOp"/>
 *         &lt;element name="IngestOp" type="{http://www.teragrid.org/schemas/datagrid}IngestDataSetOp"/>
 *         &lt;element name="PrepareTicketOp" type="{http://www.teragrid.org/schemas/datagrid}PrepareTicketOp"/>
 *         &lt;element name="DownloadDataSetOp" type="{http://www.teragrid.org/schemas/datagrid}DownloadDataSetOp"/>
 *         &lt;element name="ReplicateOp" type="{http://www.teragrid.org/schemas/datagrid}ReplicateOp"/>
 *         &lt;element name="ListOp" type="{http://www.teragrid.org/schemas/datagrid}ListOp"/>
 *         &lt;element name="SeekNReadOp" type="{http://www.teragrid.org/schemas/datagrid}SeekNReadOp"/>
 *         &lt;element name="SeekNWriteOp" type="{http://www.teragrid.org/schemas/datagrid}SeekNWriteOp"/>
 *       &lt;/choice>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 */
public interface Operation {


    edu.sdsc.matrix.srb.parser.CopyOp getCopyOp();

    void setCopyOp(edu.sdsc.matrix.srb.parser.CopyOp value);

    edu.sdsc.matrix.srb.parser.SeekNReadOp getSeekNReadOp();

    void setSeekNReadOp(edu.sdsc.matrix.srb.parser.SeekNReadOp value);

    edu.sdsc.matrix.srb.parser.DownloadDataSetOp getDownloadDataSetOp();

    void setDownloadDataSetOp(edu.sdsc.matrix.srb.parser.DownloadDataSetOp value);

    edu.sdsc.matrix.srb.parser.CreateOp getCreateOp();

    void setCreateOp(edu.sdsc.matrix.srb.parser.CreateOp value);

    edu.sdsc.matrix.srb.parser.RenameOp getRenameOp();

    void setRenameOp(edu.sdsc.matrix.srb.parser.RenameOp value);

    edu.sdsc.matrix.srb.parser.ListOp getListOp();

    void setListOp(edu.sdsc.matrix.srb.parser.ListOp value);

    edu.sdsc.matrix.srb.parser.SeekNWriteOp getSeekNWriteOp();

    void setSeekNWriteOp(edu.sdsc.matrix.srb.parser.SeekNWriteOp value);

    edu.sdsc.matrix.srb.parser.DeleteOp getDeleteOp();

    void setDeleteOp(edu.sdsc.matrix.srb.parser.DeleteOp value);

    edu.sdsc.matrix.srb.parser.PrepareTicketOp getPrepareTicketOp();

    void setPrepareTicketOp(edu.sdsc.matrix.srb.parser.PrepareTicketOp value);

    edu.sdsc.matrix.srb.parser.IngestDataSetOp getIngestOp();

    void setIngestOp(edu.sdsc.matrix.srb.parser.IngestDataSetOp value);

    edu.sdsc.matrix.srb.parser.ChangePermissionOp getChangePermissionOp();

    void setChangePermissionOp(edu.sdsc.matrix.srb.parser.ChangePermissionOp value);

    edu.sdsc.matrix.srb.parser.ReplicateOp getReplicateOp();

    void setReplicateOp(edu.sdsc.matrix.srb.parser.ReplicateOp value);

}