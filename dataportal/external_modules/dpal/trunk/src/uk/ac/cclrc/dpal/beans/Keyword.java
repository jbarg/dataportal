package uk.ac.cclrc.dpal.beans;
import java.io.Serializable;
import java.util.* ;

public class Keyword extends Common implements Serializable
{
// just uses the superclass for now
// need investigation_id in some cases

    private String investigationId = null ;

    public String getInvestigationId() {
        return investigationId;
    }

    public void setInvestigationId(String inv_id) {
        this.investigation_id = inv_id;
    }


    public String toString() {
        //can't use variable names of superclass as there are not inherited

        return "[keyword details]:"+super.toString() +
                " [investigation id]:"+getInvestigationId() ;


    }


}


  
