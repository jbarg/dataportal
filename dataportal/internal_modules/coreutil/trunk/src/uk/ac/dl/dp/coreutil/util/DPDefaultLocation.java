/*
 * DPRole.java
 *
 * Created on 23 June 2006, 08:28
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package uk.ac.dl.dp.coreutil.util;

/**
 *
 * @author gjd37
 */
public enum DPDefaultLocation {

    MY_DATA("MY_DATA"),
    ADVANCED_SEARCH("ADVANCED_SEARCH"),
    DATA_REFERENCES("DATA_REFERENCES"),
    BOOKMARKS("BOOKMARKS"),
    BASIC_SEARCH("BASIC_SEARCH"),
    ISIS_FACILITY_SEARCH("ISIS_FACILITY_SEARCH"),
    DIAMOND_FACILITY_SEARCH("DIAMOND_FACILITY_SEARCH"),
    CLF_FACILITY_SEARCH("CLF_FACILITY_SEARCH");

    DPDefaultLocation(String value) {
        this.value = value;
    }
    private final String value;

    public String toString() {
        return value;
    }
}
