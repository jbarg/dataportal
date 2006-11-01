/*
 * DataReference.java
 *
 * Created on 19 June 2006, 15:50
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.entity;

import java.io.Serializable;

import java.util.Date;
import javax.persistence.Column;
import javax.persistence.Entity;
import javax.persistence.EnumType;
import javax.persistence.Enumerated;
import javax.persistence.GeneratedValue;
import javax.persistence.GenerationType;
import javax.persistence.Id;
import javax.persistence.JoinColumn;
import javax.persistence.JoinTable;
import javax.persistence.ManyToMany;
import javax.persistence.ManyToOne;
import javax.persistence.NamedQueries;
import javax.persistence.NamedQuery;
import javax.persistence.PrePersist;
import javax.persistence.PreUpdate;
import javax.persistence.Table;
import javax.persistence.Temporal;
import javax.persistence.TemporalType;
import javax.persistence.Transient;
import uk.ac.dl.dp.coreutil.util.DPUrlRefType;
import javax.persistence.CascadeType;
import javax.persistence.OneToMany;

/**
 *
 * @author gjd37
 */
@Entity
@Table(name = "DP_DATA_REFERENCE")
@NamedQueries( {
    @NamedQuery(name = "DataReference.findById", query = "SELECT d FROM DataReference d WHERE d.id = :id"),
    @NamedQuery(name = "DataReference.findByName", query = "SELECT d FROM DataReference d WHERE d.name = :name"),
    @NamedQuery(name = "DataReference.findByNote", query = "SELECT d FROM DataReference d WHERE d.note = :note"),
    @NamedQuery(name = "DataReference.findByFacility", query = "SELECT d FROM DataReference d WHERE d.facility = :facility"),
    @NamedQuery(name = "DataReference.findByQuery", query = "SELECT d FROM DataReference d WHERE d.query = :query"),
    @NamedQuery(name = "DataReference.findByInvestigationId", query = "SELECT d FROM DataReference d WHERE d.investigationId = :investigationId"),
    @NamedQuery(name = "DataReference.findByTypeOfReference", query = "SELECT d FROM DataReference d WHERE d.typeOfReference = :typeOfReference"),
    @NamedQuery(name = "DataReference.findByReferenceId", query = "SELECT d FROM DataReference d WHERE d.referenceId = :referenceId"),
    @NamedQuery(name = "DataReference.findByTypeOfObject", query = "SELECT d FROM DataReference d WHERE d.typeOfObject = :typeOfObject"),
    @NamedQuery(name = "DataReference.findByModTime", query = "SELECT d FROM DataReference d WHERE d.modTime = :modTime"),
    @NamedQuery(name = "DataReference.findByUniqueKey", query = "SELECT d FROM DataReference d WHERE d.typeOfReference = :typeOfReference AND d.facility = :facility AND d.userId = :userId AND d.referenceId = :referenceId")}
)
public class DataReference implements Serializable {
    
    @Id
    @GeneratedValue(strategy=GenerationType.TABLE,generator="SEQ_GEN")
    @Column(name = "ID", nullable = false)
    private Integer id;
    
    @Column(name = "NAME", nullable = false)
    private String name;
    
    @Column(name = "NOTE")
    private String note;
    
    @Column(name = "FACILITY")
    private String facility;
    
    @Column(name = "QUERY")
    private String query;
    
    @Column(name = "TYPE_OF_REFERENCE")
    //  @Enumerated(EnumType.STRING)
    private String typeOfReference;
    
    @Column(name = "INVESTIGATION_ID", nullable = false)
    private Integer investigationId;
    
    @Column(name = "REFERENCE_ID")
    private Integer referenceId;
    
    @Column(name = "TYPE_OF_OBJECT")
    private String typeOfObject;
    
    @Column(name = "MOD_TIME")
    @Temporal(TemporalType.TIMESTAMP)
    private Date modTime;
    
    @JoinColumn(name = "USER_ID")
    @ManyToOne
    private User userId;
    
    @OneToMany(cascade = CascadeType.ALL, mappedBy = "dataRefId")
    private java.util.Collection <Url> urls;
    
    @Transient
    private boolean selected;
    
    @Transient
    private boolean dataset;
    
    @Transient
    private String printURLS;
    
    @Transient
    private boolean hasNote;
    
    @Transient
    private boolean download;
    
    @Transient
    private String dpId;
    
    @PrePersist
    @PreUpdate
    public void prePersist(){
        modTime = new Date();
    }
    
    /** Creates a new instance of DataReference */
    public DataReference() {
    }
    
    /*public DataReference(Integer id) {
        this.id = id;
    }*/
    
   /* public DataReference(Integer id, String name) {
        this.id = id;
        this.name = name;
    }*/
    
    public Integer getId() {
        return this.id;
    }
    
   /* public void setId(Integer id) {
        this.id = id;
    }*/
    
    public String getName() {
        return this.name;
    }
    
    public void setName(String name) {
        this.name = name;
    }
    
    public String getNote() {
        return this.note;
    }
    
    public void setNote(String note) {
        this.note = note;
    }
    
    public String getFacility() {
        return this.facility;
    }
    
    public void setFacility(String facility) {
        this.facility = facility;
    }
    
    public String getQuery() {
        return this.query;
    }
    
    public void setQuery(String query) {
        this.query = query;
    }
    
    public Integer getInvestigationId() {
        return this.investigationId;
    }
    
    public void setInvestigationId(Integer investigationId) {
        this.investigationId = investigationId;
    }
    
    public String getTypeOfReference() {
        return this.typeOfReference;
    }
    
    public void setTypeOfReference(String typeOfReference) {
        this.typeOfReference = typeOfReference;
    }
    
    public Integer getReferenceId() {
        return this.referenceId;
    }
    
    public void setReferenceId(Integer referenceId) {
        this.referenceId = referenceId;
    }
    
    public String getTypeOfObject() {
        return this.typeOfObject;
    }
    
    public void setTypeOfObject(String typeOfObject) {
        this.typeOfObject = typeOfObject;
    }
    
    public Date getModTime() {
        return this.modTime;
    }
    
   /* public void setModTime(Date modTime) {
        this.modTime = modTime;
    }*/
    
    public User getUserId() {
        return this.userId;
    }
    
    public void setUserId(User userId) {
        this.userId = userId;
    }
    
    
    
    public int hashCode() {
        int hash = 0;
        hash += (this.id != null ? this.id.hashCode() : 0);
        return hash;
    }
    
    public boolean equals(Object object) {
        if (object == null || !this.getClass().equals(object.getClass())) {
            return false;
        }
        DataReference other = (DataReference)object;
        if (this.id != other.id && (this.id == null || !this.id.equals(other.id))) return false;
        return true;
    }
    
    public String toString() {
        //TODO change toString() implementation to return a better display name
        return "" + this.id;
    }
    
    
    public boolean isSelected() {
        return selected;
    }
    
    public void setSelected(boolean selected) {
        this.selected = selected;
    }
    
    public String getPrintURLS() {
        StringBuilder builder = new StringBuilder();
        builder.append("<table>");
        int j = 0;
        for(Url url : urls){           
            builder.append("<tr><td>"+url.getName()+"</td></tr>");
            j++;
            if(j == 20 ) {
                builder.append("<tr><td>more ...</td></tr>");
                
                break;
            }
            
        }
        builder.append("</table>");
        return builder.toString();
    }
    
    public boolean isDataset() {
        if(getTypeOfReference().equals(DPUrlRefType.DATA_SET.toString())) return true;
        else if(getTypeOfReference().equals(DPUrlRefType.DATA_SET_FOLDER.toString())) return true;
        else return false;
    }
    
    public boolean isDatasetInFolder(){
        if(getTypeOfReference().equals(DPUrlRefType.DATA_SET_FOLDER.toString())) return true;
        else return false;
    }
    
    public void setDataset(boolean dataset) {
        this.dataset = dataset;
    }
    
    public java.util.Collection<Url> getUrls() {
        return urls;
    }
    
    public void setUrls(java.util.Collection<Url> urls) {
        this.urls = urls;
    }
    
    public boolean isHasNote() {
        if(getNote() == null || getNote().equals("")) return false;
        else return true;
    }
    
    public void setHasNote(boolean hasNote) {
        this.hasNote = hasNote;
    }
    
    public int getNumberOfFiles(){
        return urls.size();
    }
    
    public boolean isDownload() {
        return download;
    }
    
    public void setDownload(boolean download) {
        this.download = download;
    }
    
    public String getDpId(){
        return getFacility()+"-"+getId();
    }
    
    
}
