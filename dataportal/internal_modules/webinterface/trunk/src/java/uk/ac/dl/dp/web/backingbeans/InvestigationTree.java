/*
 * SearchBean.java
 *
 * Created on 08 August 2006, 14:33
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.web.backingbeans;


import java.util.Collection;
import java.util.*;
import org.apache.log4j.Logger;
import org.apache.myfaces.custom.tree2.HtmlTree;
import org.apache.myfaces.custom.tree2.TreeNode;
import org.apache.myfaces.custom.tree2.TreeNodeBase;
import org.apache.myfaces.custom.tree2.TreeModelBase;
import javax.faces.event.ActionEvent;
import javax.faces.component.UIComponent;
import javax.faces.component.*;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.exceptions.DataPortalException;
import uk.ac.dl.dp.web.navigation.NavigationConstants;
import uk.ac.dl.dp.web.util.AbstractRequestBean;
import uk.ac.dl.dp.web.util.WebConstants;
import uk.ac.dp.icatws.Datafile;
import uk.ac.dp.icatws.Dataset;
import uk.ac.dp.icatws.Investigation;

/**
 *
 * @author gjd37
 */
public class InvestigationTree extends AbstractRequestBean {
    
    private static Logger log = Logger.getLogger(InvestigationTree.class);
    
    private TreeModelBase  _treeModel;
    private HtmlTree   _tree;
    
    private TreeNode data;
    
    /** Creates a new instance of SearchBean */
    public InvestigationTree() {
    }
    
    
    public TreeModelBase getTreeModel() {
        return _treeModel;
    }
    
    public void setTreeModel(TreeModelBase treeModel) {
        this._treeModel = treeModel;
    }
    
    public HtmlTree getTree() {
        return _tree;
    }
    
    public void setTree(HtmlTree tree) {
        this._tree = tree;
    }
    
    
    public TreeNode getData() {
        
        if(data == null){
            //top level node of results, not show at moment in page
            data = new TreeNodeBase("foo-folder", "Search results", false);
            
            Collection<Investigation> invests = getVisitData().getSearchedInvestigations();
            
            //construct a list of returned facility results
            Collection<String> facs = new LinkedList<String>();
            for(Investigation invest :  invests){
                if(!facs.contains(invest.getFacility())){
                    facs.add(invest.getFacility().getFacilityShortName());
                }
            }
            
            //iterate of the list and add investiagtions that are asscoicated with it
            TreeNodeBase node = null;
            for(String fac : facs){
                
                //count how many from this fac
                int i = 0;
                for(Investigation invest: invests){
                    if(invest.getFacility().equals(fac)) i++;
                }
                //add count to (#) on investigation page
                node = new TreeNodeBase("foo-folder", fac, ""+i,false);
                int j = 0;
                
                //loop, if equals fac then add node
                for(Investigation invest : invests){
                    if(invest.getFacility().equals(fac)){
                        //add node, but add identifer as facility-id for uniqueness
                        //id of invest in ISIS could be same as DIAMOND
                        node.getChildren().add(new TreeNodeBase("foo1-folder", uk.ac.dl.dp.web.util.Util.escapeInvalidStrings(invest.getTitle()),invest.getFacility().getFacilityShortName()+"-"+invest.getId(), true));
                        
                        //only show 50??  shoule change
                        j++;
                        if(j == WebConstants.MAXIMIUM_INVESTIGATION_TREE_RESULTS){
                            node.getChildren().add(new TreeNodeBase("foo1-folder","more ...", true));
                            break;
                        }
                    }
                }
                //add  facility node to the top level node
                data.getChildren().add(node);
            }
            
            //now add the facs that have not returned yet.
            //maybe no search yet, coming from Data Center
            if(getVisitData() !=null && getVisitData().getQueryRequest() != null){
                Collection<String> searched = getVisitData().getQueryRequest().getFacilities();
                for(String searchedFac : searched){
                    boolean isCompleted = false;
                    for(String completedFac : facs){
                        if(completedFac.equals(searchedFac)){
                            //searched fac completed
                            log.trace(completedFac+ " finished");
                            isCompleted = true;
                            
                        }
                    }
                    //check now if searched is completed
                    if(!isCompleted){
                        log.trace("Not finished,timed out");
                        node = new TreeNodeBase("foo-folder", searchedFac, "Timed out",true);
                        data.getChildren().add(node);
                    }else if(isCompleted){
                        //count how many from this fac
                        int i = 0;
                        for(Investigation invest: invests){
                            if(invest.getFacility().equals(searchedFac)) i++;
                        }
                        if(i == 0){
                            node = new TreeNodeBase("foo-folder", searchedFac, "0",true);
                            data.getChildren().add(node);
                        }
                    }
                }
            }
        }
        
        return data;
    }
    
    
    private TreeNodeBase _node;
    
    public void setNodePath(TreeNodeBase nodePath) {
        _node = _node;
    }
    
    public TreeNodeBase getNodePath(){
        return _node;
    }
    
    //action event to set which current node was clicked last
    public void setNodeSelected(ActionEvent event) {
        
        log.trace("node clicked: ");
        UIComponent component = (UIComponent) event.getSource();
        while (!(component != null && component instanceof HtmlTree)) {
            component = component.getParent();
        }
        if (component != null) {
            HtmlTree tree = (HtmlTree) component;
            TreeNodeBase node = (TreeNodeBase) tree.getNode();
            _node = node;
            tree.setNodeSelected(event);
            log.trace("Hello: "+event);
            //_tree.setNodeSelected(event);
            
            log.trace(node.getIdentifier());
            log.trace(node.getDescription());
        }
    }
    
    //this takes the user to mext data set page and shows all the results from the facility (user clicked facility)
    //could have done same for investigatiosns but got javascritp exceptions no weired name like ';;'type100'
    public String minimise(){
        log.trace("Showing all investigations for "+getNodePath().getDescription());
        Collection<Investigation> investigations = new ArrayList<Investigation>();
        for(Investigation invest :  getVisitData().getSearchedInvestigations()){
            if(invest.getFacility().equals(getNodePath().getDescription())){
                investigations.add(invest);
                log.trace(invest.getId()+" is added");
            }
        }
        return viewDataSets(investigations);
    }
    
    //unclick all investigations
    public String selectone(){
        log.trace("selecting for "+getNodePath().getIdentifier());
        Collection<Investigation> investigations = new ArrayList<Investigation>();
        Investigation invest = getInvestigation(getNodePath().getIdentifier());
        investigations.add(invest);
        return viewDataSets(investigations);
    }
    
    //selection to load up all the data files and datasets from the list of investigations
    public String viewDataSets(Collection<Investigation> investigations){
        
        
        if(investigations.size() == 0 ){
            info("Please select atleast one investigation.");
            return null;
        }
        //canot display too many datsest, size of dowan too big for next page so limit
        if(investigations.size() > WebConstants.MAXIMIUM_DATASET_RESULTS ){
            warn("Please select less than "+WebConstants.MAXIMIUM_DATASET_RESULTS+" investigations to view.");
            return null;
        }
        
        
        Collection<Dataset> datasets = null;
        Collection<Datafile> datafiles = null;
        
        try{
            QueryDelegate qd = QueryDelegate.getInstance();
            log.debug("About to get datasets from: "+investigations.size());
            for(Investigation invest :  investigations){
                log.trace(invest);
            }
            
           /* datasets = qd.getDataSets(getVisit().getSid(),investigations);
            for(DataSet dataset : datasets){
                log.trace(dataset);
            }
            log.debug("Got datasets, getting datafiles");
            
            datafiles = qd.getDataFiles(getVisit().getSid(), datasets);
            log.debug("Got data files");
            for(DataFile datafile : datafiles){
                log.trace(datafile);
            }*/
            
            //set all of the data is visit for next page
            getVisitData().setCurrentInvestigations(investigations);
            getVisitData().setCurrentDatasets(datasets);
            getVisitData().setCurrentDatafiles(datafiles);
                if(true) throw new DataPortalException();
        } catch (DataPortalException ex) {
            error("Error:  Unable to search the information for the investigations.");
            log.fatal("Unable to create query user for: "+getVisit().getSid(),ex);
            return null;
        } catch (Exception ex) {
            error("Error:  Unable to search the information for the investigations.");
            log.fatal("exception : "+getVisit().getSid(),ex);
            return null;
        }
        
        return NavigationConstants.GET_DATASETS_SUCCESS;
    }
    
    public void setData(TreeNode data) {
        this.data = data;
    }
    
    //helper class that splits the param sent in to identify the current investigation
    private Investigation getInvestigation(String param) {
        String fac = param.split("-")[0];
        String id = param.split("-")[1];
        
        for(Investigation file : getVisitData().getCurrentInvestigations()){
            if(file.getId().equals(id)&& file.getFacility().getFacilityShortName().equals(fac)){
                log.debug("Found invest: "+file);
                return file;
            }
        }
        return null;
    }
    
    
}
