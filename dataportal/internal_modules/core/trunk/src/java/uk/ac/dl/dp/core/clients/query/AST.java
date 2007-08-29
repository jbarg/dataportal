/*
 * AST.java
 *
 * Created on 29-Aug-2007, 15:25:37
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.core.clients.query;

import uk.ac.dl.dp.coreutil.util.AdvancedSearchDetailsDTO;
import uk.ac.dp.icatws.AdvancedSearchDetails;

/**
 *
 * @author gjd37
 */
public class AST {
    
    /** Creates a new instance of AST */
    public AST() {
        
        AdvancedSearchDetailsDTO dto =new AdvancedSearchDetailsDTO();
       
        dto.setFuzzy(true);
      
        AdvancedSearchDetails advancedSearchDetails = new AdvancedSearchDetails();
        dto.mergeTo(advancedSearchDetails);
        
        
        /*AdvancedSearchDetails advancedSearchDetails = new AdvancedSearchDetails();
        advancedSearchDetails.setFuzzy(true);
       
        
        AdvancedSearchDetailsDTO dto =new AdvancedSearchDetailsDTO();
        dto.mergeFrom(advancedSearchDetails);
        
        */
        
    } 
        
        /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
        new AST();
    }
    
}
