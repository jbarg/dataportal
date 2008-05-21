/*
 * EventLogCount.java
 *
 * Created on 24 November 2006, 13:08
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.entity;

import java.io.Serializable;

/**
 *
 * @author gjd37
 */
public class EventLogCount implements Serializable{
    
    private String dn;
    private Integer visits = 0;
    private Integer searches= 0;
    private Integer downloads =0;
    private Integer bookmarks =0;
    private Integer dataReferences=0;
    private Integer id;
    private boolean admin;

    public String getDn() {
        return dn;
    }

    public void setDn(String dn) {
        this.dn = dn;
    }

    public Integer getVisits() {
        return visits;
    }

    public void setVisits(Integer visits) {
        this.visits = visits;
    }

    public Integer getSearches() {
        return searches;
    }

    public void setSearches(Integer searches) {
        this.searches = searches;
    }

    public Integer getDownloads() {
        return downloads;
    }

    public void setDownloads(Integer downloads) {
        this.downloads = downloads;
    }

    public Integer getBookmarks() {
        return bookmarks;
    }

    public void setBookmarks(Integer bookmarks) {
        this.bookmarks = bookmarks;
    }

    public Integer getDataReferences() {
        return dataReferences;
    }

    public void setDataReferences(Integer dataReferences) {
        this.dataReferences = dataReferences;
    }

    public Integer getId() {
        return id;
    }

    public void setId(Integer id) {
        this.id = id;
    }

    public boolean isAdmin() {
        return admin;
    }

    public void setAdmin(boolean admin) {
        this.admin = admin;
    }
    
}
