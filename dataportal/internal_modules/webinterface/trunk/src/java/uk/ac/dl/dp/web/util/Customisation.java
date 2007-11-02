/*
 * Customisation.java
 *
 * Created on 03-Oct-2007, 14:54:42
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package uk.ac.dl.dp.web.util;

import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;
import javax.faces.model.SelectItem;
import org.apache.log4j.Logger;

/**
 *
 * @author gjd37
 */
public class Customisation {

    private static Logger log = Logger.getLogger(Customisation.class);
    private Properties props;
    private final static String DEFAULT_LOCATION = "/uk/ac/dl/dp/web/messages/facility.properties";
    /**
     * Name instrument, this is name instrument,  DLS = Beamline  ISIS = Instrument, CLF = Target Area
     */
    private String instrument;
    /**
     * Is inv type displayed
     */
    private boolean invTypeVisible;
    /**
     * Is inv type displayed
     */
    private boolean shiftVisible;   
     /**
     * Is inv number displayed
     */
    private boolean invNumberVisible;
    /**
     * Is bcatinvstr displayed
     */
    private boolean bcatInvStr;   
    /**
     * Is visit displayed
     */
    private boolean visitIdVisible;
    /**
     * Hardcoded values of instruments
     */
    private List<SelectItem> instrumentsItems = new ArrayList<SelectItem>();
    /**
     * Hardcoded values of instruments
     */
    private List<SelectItem> investigationTypeItems = new ArrayList<SelectItem>();

    public Customisation(String location) throws IOException {
        URL url = this.getClass().getResource(location);
        if (url != null) {
            props = new Properties();
            props.load(url.openStream());

            //load defaults
            instrument = getProperty("instrument.name", "Instrument");
            try {
                invTypeVisible = new Boolean(getProperty("inv.type.visible", "true")).booleanValue();
            } catch (Exception mre) {
                invTypeVisible = true;
            }
            
            try {
                invNumberVisible = new Boolean(getProperty("inv.number.visible", "true")).booleanValue();
            } catch (Exception mre) {
                invNumberVisible = true;
            }
             
            try {
                visitIdVisible = new Boolean(getProperty("visitId.visible", "true")).booleanValue();
            } catch (Exception mre) {
                visitIdVisible = true;
            }
            
             try {
                shiftVisible = new Boolean(getProperty("shift.visible", "true")).booleanValue();
            } catch (Exception mre) {
                shiftVisible = true;
            }
            
             try {
                bcatInvStr = new Boolean(getProperty("bcatinvstr.visible", "true")).booleanValue();
            } catch (Exception mre) {
                bcatInvStr = true;
            }
             
            //load up select items for advanced search
            //TODO hard code the instruments
            try {

                String instrumentList = getProperty("instrument.list");
                String[] instruments = instrumentList.split(",");

                instrumentsItems.add(new SelectItem("", ""));
                for (String instrument : instruments) {
                    //log.trace(instrument);
                    instrumentsItems.add(new SelectItem(instrument, instrument));
                }

            } catch (Exception mre) {
                log.warn("Unable to read in instruments", mre);
                instrumentsItems.add(new SelectItem("error", "error"));
            }

            //TODO hard code the inv type
            try {

                String investigationTypeList = getProperty("investigation.type.list");
                String[] types = investigationTypeList.split(",");

                investigationTypeItems.add(new SelectItem("", ""));
                for (String investigationType : types) {
                    // log.trace(investigationType); 
                    int index = investigationType.indexOf("_");
                    if (index != -1) {
                        investigationTypeItems.add(new SelectItem(investigationType, investigationType.substring(0, index)));
                    } else {
                        investigationTypeItems.add(new SelectItem(investigationType, investigationType));
                    }
                }

            } catch (Exception mre) {
                log.warn("Unable to read in investigation type", mre);
                instrumentsItems.add(new SelectItem("error", "error"));
            }
        } else {
            throw new FileNotFoundException(location + " not found");
        }
    }

    public Customisation() throws IOException {
        this(DEFAULT_LOCATION);
    }

    public String getProperty(String key, String defaultValue) {
        if (props == null) {
            return null;
        } else {
            return props.getProperty(key, defaultValue);
        }
    }

    public String getProperty(String key) {
        if (props == null) {
            return null;
        } else {
            return props.getProperty(key);
        }
    }

     public boolean isShiftVisible() {
        return shiftVisible;
    }

    public void setShiftVisible(boolean shiftVisible) {
        this.shiftVisible = shiftVisible;
    }
    
    public String getInstrument() {
        return instrument;
    }

    public void setInstrument(String instrument) {
        this.instrument = instrument;
    }

    public boolean isInvTypeVisible() {
        return invTypeVisible;
    }

    public void setInvTypeVisible(boolean invTypeVisible) {
        this.invTypeVisible = invTypeVisible;
    }
    
     public boolean isInvNumberVisible() {
        return invNumberVisible;
    }

    public void setInvNumberVisible(boolean invNumberVisible) {
        this.invNumberVisible = invNumberVisible;
    }

    public boolean isVisitIdVisible() {
        return visitIdVisible;
    }

    public void setVisitIdVisible(boolean visitIdVisible) {
        this.visitIdVisible = visitIdVisible;
    }

    public List<SelectItem> getInstrumentsItems() {
        return instrumentsItems;
    }

    public void setInstrumentsItems(List<SelectItem> instrumentsItems) {
        this.instrumentsItems = instrumentsItems;
    }

    public List<SelectItem> getInvestigationTypeItems() {
        return investigationTypeItems;
    }

    public void setInvestigationTypeItems(List<SelectItem> investigationTypeItems) {
        this.investigationTypeItems = investigationTypeItems;
    }
    
     public boolean isBcatInvStr() {
        return bcatInvStr;
    }

    public void setBcatInvStr(boolean bcatInvStr) {
        this.bcatInvStr = bcatInvStr;
    }
}


