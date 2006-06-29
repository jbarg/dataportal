/*
 * DataURLDTO.java
 *
 * Created on 27 June 2006, 13:15
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp5.clients.dto;

import java.io.Serializable;
import java.math.BigDecimal;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import uk.ac.dl.dp5.entity.DataReference;
import uk.ac.dl.dp5.entity.Url;
import uk.ac.dl.dp5.util.DPUrlRefType;

/**
 *
 * @author gjd37
 */
public class DataUrlDTO implements Serializable{
    
    private String name;
    
    private String note;
    
    private String facility;
    
    private String query;
    
    private DPUrlRefType typeOfReference;
    
    private String typeOfObject;
    private int id;
    
    private Collection<String> urls;
    
    /** Creates a new instance of DataURLDTO */
    public DataUrlDTO() {
    }
    
    public DataUrlDTO(DataReference dref){
        this.id = dref.getId().intValue();
        this.facility = dref.getFacility();
        this.query = dref.getQuery();
        this.note = dref.getNote();
        this.name = dref.getName();
        this.typeOfObject = dref.getTypeOfObject();
        this.typeOfReference = DPUrlRefType.valueOf(dref.getTypeOfReference());
        urls = new ArrayList<String>();
        for(Url url : dref.getUrls()){
            urls.add(url.getUrl());
        }
    }
    
    
    public String getName() {
        return name;
    }
    
    public void setName(String name) {
        this.name = name;
    }
    
    public String getNote() {
        return note;
    }
    
    public void setNote(String note) {
        this.note = note;
    }
    
    public String getFacility() {
        return facility;
    }
    
    public void setFacility(String facility) {
        this.facility = facility;
    }
    
    public String getQuery() {
        return query;
    }
    
    public void setQuery(String query) {
        this.query = query;
    }
    
    public DPUrlRefType getTypeOfReference() {
        return typeOfReference;
    }
    
    public void setTypeOfReference(DPUrlRefType typeOfReference) {
        this.typeOfReference = typeOfReference;
    }
    
    public String getTypeOfObject() {
        return typeOfObject;
    }
    
    public void setTypeOfObject(String typeOfObject) {
        this.typeOfObject = typeOfObject;
    }
    
    public int getId() {
        return id;
    }
    
    public void setId(int id) {
        this.id = id;
    }
    
    public Collection<String> getUrl() {
        return urls;
    }
    
    public void setUrl(Collection<String> urls) {
        this.urls = urls;
    }
    
    public DataReference toDataReference(){
        DataReference dr = new DataReference();
        
        //if got id, then its an update, adding urls not allowed
        if(getId() != 0){
            dr.setId(new BigDecimal(getId()));
        } else{
            Collection<Url> dataUrls = new ArrayList<Url>();
            for(String surl : getUrl()){
                Url url = new Url();
                  url.setUrl(surl);
                dataUrls.add(url);
            }
            dr.setUrls(dataUrls);
        }
        dr.setFacility(getFacility());
        dr.setName(getName());
        dr.setNote(getNote());
        dr.setQuery(getQuery());
        dr.setTypeOfReference(getTypeOfReference().toString());
        dr.setTypeOfObject(getTypeOfObject());
         return dr;
    }
    
}
