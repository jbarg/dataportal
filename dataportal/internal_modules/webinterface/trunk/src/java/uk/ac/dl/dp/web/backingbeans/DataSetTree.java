/*
 * SearchBean.java
 *
 * Created on 08 August 2006, 14:33
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */
package uk.ac.dl.dp.web.backingbeans;

import java.io.Serializable;
import java.net.MalformedURLException;
import java.text.DecimalFormat;
import java.util.Collection;
import java.util.*;
import javax.faces.event.ActionEvent;
import org.apache.log4j.Logger;
import org.apache.myfaces.custom.tree2.HtmlTree;
import org.apache.myfaces.custom.tree2.TreeNode;
import org.apache.myfaces.custom.tree2.TreeNodeBase;
import org.apache.myfaces.custom.tree2.*;
import uk.ac.dl.dp.coreutil.clients.dto.FacilityDTO;
import uk.ac.dl.dp.coreutil.delegates.DataCenterDelegate;
import uk.ac.dl.dp.coreutil.entity.Bookmark;
import uk.ac.dl.dp.coreutil.entity.DataReference;
import uk.ac.dl.dp.coreutil.entity.Url;
import uk.ac.dl.dp.coreutil.util.DPUrlRefType;
import uk.ac.dl.dp.coreutil.util.Util;
import uk.ac.dl.dp.web.navigation.NavigationConstants;
import uk.ac.dl.dp.web.util.AbstractRequestBean;
import uk.ac.dl.dp.web.util.WebConstants;
import javax.faces.component.*;
import uk.ac.dl.dp.coreutil.delegates.DownloadDelegate;
import uk.ac.dl.dp.coreutil.delegates.QueryDelegate;
import uk.ac.dl.dp.coreutil.exceptions.DataCenterException;
import uk.ac.dl.dp.coreutil.exceptions.SessionException;
import uk.ac.dl.dp.coreutil.util.SRBInfo;
import uk.ac.dp.icatws.Datafile;
import uk.ac.dp.icatws.Dataset;
import uk.ac.dp.icatws.DatasetInclude;
import uk.ac.dp.icatws.Investigation;

/**
 *
 * @author gjd37
 */
public class DataSetTree extends AbstractRequestBean implements Serializable {

    private static Logger log = Logger.getLogger(DataSetTree.class);
    private TreeModelBase _treeModel;
    private HtmlTree tree;
    private TreeNode data;
    /**
     * Disbales the top nav bar for datasets
     */
    private boolean datasetNav = true;

    public boolean isDatasetNav() {
        return datasetNav;
    }

    public boolean getSetFalse() {
        return false;
    }

    /** Creates a new instance of SearchBean */
    public DataSetTree() {
    }

    public TreeModelBase getTreeModel() {
        return _treeModel;
    }

    public void setTreeModel(TreeModelBase treeModel) {
        this._treeModel = treeModel;
    }

    public HtmlTree getTree() {
        return tree;
    }

    public void setTree(HtmlTree tree) {
        this.tree = tree;
    }

    public TreeNode getData() {

        //page going onto this, so disable the button
        datasetNav = false;

        //getVisitData().reinitailise();


        //if(getVisitData().getDataSetTree() == null){
        log.trace("Creating dataset tree");

        Collection<Investigation> investigations = getVisitData().getCurrentInvestigations();

        data = new TreeNodeBase("invest", "Investigations (" + investigations.size() + ")", false); //this not shown

        TreeNodeBase node = null;
        for (Investigation invest : investigations) {

            // log.debug("Adding investigations: "+invest.getName());
            node = new TreeNodeBase("invest", invest.getTitle(), invest.getFacility() + "-" + invest.getId(), false);

            if (invest.getVisitId() != null && getVisit().getCustomisation().isVisitIdVisible()) {
                node.getChildren().add(new TreeNodeBase("visitId-folder", invest.getVisitId(), true));
            }
            if (invest.getInvNumber() != null && getVisit().getCustomisation().isInvNumberVisible()) {
                node.getChildren().add(new TreeNodeBase("invNumber-folder", invest.getInvNumber(), true));
            }
            if (invest.getInstrument() != null) {
                node.getChildren().add(new TreeNodeBase("instrument-folder", invest.getInstrument(), true));
            }

            if (!getVisit().isSingleFacility()) {
                node.getChildren().add(new TreeNodeBase("fac-folder", invest.getFacility(), true));
            }

            TreeNodeBase datasetsNode = new TreeNodeBase("dataset-count-folder", "Datasets", invest.getFacility() + "-" + invest.getId() ,false);

            for (Dataset dataset : invest.getDatasetCollection()) {

                TreeNodeBase datasetNode = new TreeNodeBase("dataset-noread-folder", dataset.getName(), invest.getFacility() + "-" + invest.getId() + "-" + dataset.getId(), false);

                if (dataset.getDatasetStatus() != null) {
                    datasetNode.getChildren().add(new TreeNodeBase("status-folder", dataset.getDatasetStatus(), true));
                } else {
                    datasetNode.getChildren().add(new TreeNodeBase("status-folder", "", true));
                }

                if (dataset.getDatasetType() != null) {
                    datasetNode.getChildren().add(new TreeNodeBase("type-folder", dataset.getDatasetType(), true));
                } else {
                    datasetNode.getChildren().add(new TreeNodeBase("type-folder", "", true));
                }

                datasetNode.getChildren().add(new TreeNodeBase("desc-folder", uk.ac.dl.dp.web.util.Util.escapeInvalidStrings(dataset.getDescription()), true));

                /*for(Datafile datafile : dataset.getDatafileCollection()){
                    
                    boolean isImageJ = Util.isImageJ(datafile.getName());
                    log.trace(datafile.getName()+" is imageJ "+isImageJ);
                    
                    TreeNodeBase datafileNode = null;
                    
                    //check if file downloadable
                    if(datafile.getIcatRole().isActionDownload() && datafile.getLocation() !=null && !datafile.getLocation().equals("")){
                        datafileNode = new TreeNodeBase("file-folder", datafile.getName(),invest.getFacility()+"-"+invest.getId()+"-"+dataset.getId()+"-"+datafile.getId(),false);
                    } else {
                        datafileNode = new TreeNodeBase("file-noread-folder", datafile.getName(),invest.getFacility()+"-"+invest.getId()+"-"+dataset.getId()+"-"+datafile.getId(),false);
                    }
                    
                    if(datafile.getDatafileFormat() != null) datafileNode.getChildren().add(new TreeNodeBase("format-folder", datafile.getDatafileFormat().getDatafileFormatPK().getName(),false));
                    if(datafile.getDatafileFormat() != null) datafileNode.getChildren().add(new TreeNodeBase("format-version-folder", datafile.getDatafileFormat().getDatafileFormatPK().getVersion(),false));
                    if(datafile.getDatafileFormat() != null) datafileNode.getChildren().add(new TreeNodeBase("format-type-folder", datafile.getDatafileFormat().getFormatType(),false));
                    
                    if(datafile.getDatafileCreateTime() != null) datafileNode.getChildren().add(new TreeNodeBase("createTime-folder", ""+datafile.getDatafileCreateTime(),false));
                    if(datafile.getFileSize() != null) datafileNode.getChildren().add(new TreeNodeBase("size-folder", ""+df.format((datafile.getFileSize()/(1024f*1024f))) +" MB", false));
                    
                    if(isImageJ && datafile.getIcatRole().isActionDownload() && datafile.getLocation() !=null && !datafile.getLocation().equals("")){
                        datafileNode.getChildren().add(new TreeNodeBase("imageJ", "Launch ImageJ",invest.getFacility()+"-"+invest.getId()+"-"+dataset.getId()+"-"+datafile.getId(),false));
                    }
                    datasetNode.getChildren().add(datafileNode);
                }*/
                //datasetNode.getChildren().add(datafilesNode);
                datasetsNode.getChildren().add(datasetNode);
            }
            node.getChildren().add(datasetsNode);

            data.getChildren().add(node);
        }

        getVisitData().setDataSetTree(data);
        //expandAll();
        return data;
    // } else return getVisitData().getDataSetTree();

    }

    public void setData(TreeNode data) {
        this.data = data;
    }

    public TreeNode getTreeData() {
        return data;
    }

    public String setValueChangeListeners() {
        //this is a dummy method so that all the valuechangelisteners are called and then the
        //ajax4jsf calls a javascript function
        log.trace("Finished setting all valuelisteners.  No more action needed");
        return null;
    }

    public void setSelectedAjax(ActionEvent event) {

        List children = event.getComponent().getChildren();
        int i = 0;

        log.trace("selected checkbox ajax");
        for (Object ob : children) {
            if (ob instanceof UIParameter) {
                UIParameter current = (UIParameter) children.get(i);
                log.trace("Param name " + current.getName());
                if (current.getName().equals("datafiles") && current.getValue() != null) {
                    String param = current.getValue().toString();
                    // log.trace("selected: "+param+" for "+event.getNewValue());
                    Datafile df = getVisitData().getDataFileFromSearchedData(param);
                    df.setSelected(!df.isSelected());
                    log.trace("setting " + df.isSelected() + "  for DATAFILE: " + df.getId());
                    break;
                }
                if (current.getName().equals("datasets") && current.getValue() != null) {
                    String param = current.getValue().toString();
                    Dataset ds = getVisitData().getDataSetFromSearchedData(param);
                    ds.setSelected(!ds.isSelected());
                    log.trace("setting " + ds.isSelected() + "  for DATASET: " + ds.getId());
                    break;
                }
                if (current.getName().equals("investigations") && current.getValue() != null) {
                    String param = current.getValue().toString();
                    Investigation in = getVisitData().getInvestigationFromSearchedData(param);
                    //log.trace("? "+in.isSelected()+"  for INVESTIGATION: "+in.getId());
                    in.setSelected(!in.isSelected());
                    log.trace("setting " + in.isSelected() + "  for INVESTIGATION: " + in.getId());
                    break;
                }
            }
            i++;
        }
    }

    public void viewDataFiles(ActionEvent event) {
        List children = event.getComponent().getChildren();
        int i = 0;

        log.trace("viewing datafiles");
        for (Object ob : children) {
            if (ob instanceof UIParameter) {
                UIParameter current = (UIParameter) children.get(i);
                log.trace("Param name " + current.getName()+" and value "+current.getValue());

                if (current.getName().equals("datafiles") && current.getValue() != null) {
                    String param = current.getValue().toString();
                    
                    //set current dataset as param
                    getVisitData().setCurrentDatasetId(param);
                    
                    Dataset ds = getVisitData().getDataSetFromSearchedData(param);

                    log.trace("viewing datafiles for DATASET: " + ds.getId());

                    //check if already got datafiles
                    if (ds.getDatafileCollection() == null || ds.getDatafileCollection().size() == 0) {
                        log.trace("dataset " + ds.getId() + " is null or zero so searching for then");
                        String fac = param.split("-")[0];

                        try {
                            //now get the datafiles for the dataset
                            Dataset datasetReturned = QueryDelegate.getInstance().getDataset(getVisit().getSid(), ds, fac, DatasetInclude.DATASET_AND_DATAFILES_ONLY);
                            log.trace("Returned " + datasetReturned.getDatafileCollection().size() + " for " + ds.getId());
                            for (Investigation investigation : getVisitData().getCurrentInvestigations()) {
                                if (investigation.getId().equals(ds.getInvestigationId()) && investigation.getFacility().equals(fac)) {
                                    log.trace("Found that dataset: "+ds.getId()+" belongs to investigation: "+investigation.getId());
                                    Collection<Dataset> datasets = investigation.getDatasetCollection();
                                    for (Dataset dataset : datasets) {
                                        if (dataset.getId().equals(ds.getId())) {
                                            Collection<Datafile> datafilesSearched = dataset.getDatafileCollection(); //will return empty collection
                                            datafilesSearched.clear(); //make sure if clear though should be
                                            for(Datafile df : datasetReturned.getDatafileCollection()){
                                                                                               
                                                df.setUniqueId(fac);
                                                dataset.getDatafileCollection().add(df);
                                            }                                            
                                            log.trace("Setting datafiles for Investigation " + investigation.getId() + " and dataset " + dataset.getId());
                                            getVisitData().setCurrentDataset(dataset.getName());
                                        }
                                    }
                                }
                            }

                            //now set current datafiles                           
                            getVisitData().setCurrentDatafiles(datasetReturned.getDatafileCollection());
                        } catch (Throwable ex) {
                            log.error("Cannot view datafiles", ex);
                            error("Error: Cannot view datafiles");
                        }
                    } else {
                        //now set current datafiles
                        getVisitData().setCurrentDatafiles(ds.getDatafileCollection());
                    }
                    break;
                }
            }
            i++;
        }
    }

    public void viewDataSets(ActionEvent event) {
        List children = event.getComponent().getChildren();
        int i = 0;

        log.trace("viewing datasets");
        for (Object ob : children) {
            if (ob instanceof UIParameter) {
                UIParameter current = (UIParameter) children.get(i);
                log.trace("Param name " + current.getName()+" and value "+current.getValue());

                if (current.getName().equals("datasets") && current.getValue() != null) {
                    String param = current.getValue().toString();
                    Investigation investigation = getVisitData().getInvestigationFromSearchedData(param);
                    log.trace("viewing datasets for INVESTIGATION: " + investigation.getId());

                    //now set current datasets
                    getVisitData().setCurrentInvestigation(investigation.getTitle());
                    getVisitData().setCurrentDatasets(investigation.getDatasetCollection());
                }
                break;
            }
            i++;
        }

    }

    public String emailDownload(/*ActionEvent event*/) {
        log.trace("emailDownload: ");
        String[] srbFilesDownload = getVisitData().getAllSearchedSRBURLs();
        if (srbFilesDownload.length == 0) {
            error("Please select atleast one item to download.");
            return null;
        }

        try {
            SRBInfo info = new SRBInfo();
            info.setSid(getVisit().getSid());
            info.setSrbUrls(getVisitData().toSRBUrl(srbFilesDownload));
            DownloadDelegate.getInstance().downloadSRBFiles(getVisit().getSid(), info);
            info("Request sent for download");

        } catch (MalformedURLException mex) {
            log.error("Cannot download data via email, invalid URLS found.", mex);
            error("Error: Cannot download data via email");
        } catch (Throwable ex) {
            log.error("Cannot download data via email", ex);
            error("Error: Cannot download data via email");
        }

        return null;
    }

    public String select() {

        log.trace("Selected investigations, datasets, datafiles for addition :");

        Collection<Bookmark> toAddBookmarks = new ArrayList<Bookmark>();
        Collection<DataReference> toAddDataReference = new ArrayList<DataReference>();

        for (Investigation investigation : getVisitData().getCurrentInvestigations()) {

            //add all the investigations to the bookmarks
            if (investigation.isSelected()) {
                Bookmark bookmark = new Bookmark();
                bookmark.setFacility(investigation.getFacility());
                bookmark.setName(investigation.getTitle());
                bookmark.setNote("");
                bookmark.setStudyId(investigation.getId());
                bookmark.setQuery("N/A");
                toAddBookmarks.add(bookmark);
                log.trace("Adding this to bookmark" + investigation.getId());
            }

            //now add datasets

            for (Dataset dataset : investigation.getDatasetCollection()) {
                if (dataset.isSelected()) {
                    //log.trace(dataset);
                    DataReference ref = new DataReference();

                    ref.setFacility(investigation.getFacility());
                    ref.setName(dataset.getName());
                    ref.setNote("");
                    ref.setQuery("N/A");

                    boolean isDataInFolders = false;
                    for (FacilityDTO facs : getVisit().getSession().getFacilities()) {
                        //if data in folders then show no leaf in datatree
                        if (facs.getFacility().equals(investigation.getFacility()) && facs.isDataSetInFolders()) {
                            isDataInFolders = true;
                        }

                    }
                    if (isDataInFolders) {
                        ref.setTypeOfReference(DPUrlRefType.DATA_SET_FOLDER.toString());
                    } else {
                        ref.setTypeOfReference(DPUrlRefType.DATA_SET.toString());
                    }

                    ref.setTypeOfObject(dataset.getDatasetType());
                    ref.setReferenceId(dataset.getId());
                    ref.setInvestigationId(dataset.getInvestigationId());

                    Collection<Url> cs = new ArrayList<Url>();

                    for (Datafile df : dataset.getDatafileCollection()) {
                        Url url = new Url();
                        url.setDataRefId(ref);
                        url.setName(df.getName());
                        //TODO remove line, URL should neve be null
                        if (df.getLocation() == null) {
                            df.setLocation("dummy.raw");
                        }

                        url.setUrl(df.getLocation());
                        cs.add(url);
                        log.trace("Adding data file " + df.getId() + " to dataset: " + dataset.getId());

                    }

                    ref.setUrls(cs);
                    //check of the dataset has any data files
                    if (ref.getUrls().size() == 0) {
                        warn(ref.getName() + " not added because it's associated with no data files.");
                        log.warn("Not adding " + dataset.getId() + ", it has not data files associated");
                    } else {
                        toAddDataReference.add(ref);
                        log.trace(dataset.getId() + " with " + ref.getUrls().size() + " data files");
                    }

                }

                //now add datafiles
                for (Datafile datafile : dataset.getDatafileCollection()) {
                    if (datafile.isSelected()) {

                        DataReference ref = new DataReference();

                        ref.setFacility(investigation.getFacility());
                        ref.setName(datafile.getName());
                        ref.setNote("");
                        ref.setQuery("N/A");

                        //what type of dataset reference is this
                        boolean isDataInFolders = false;
                        for (FacilityDTO facs : getVisit().getSession().getFacilities()) {
                            //if data in folders then show no leaf in datatree
                            if (facs.getFacility().equals(investigation.getFacility()) && facs.isDataSetInFolders()) {
                                isDataInFolders = true;
                            }

                        }
                        if (isDataInFolders) {
                            ref.setTypeOfReference(DPUrlRefType.FILE_FOLDER.toString());
                        } else {
                            ref.setTypeOfReference(DPUrlRefType.FILE.toString());
                        }

                        if (datafile.getDatafileFormat() == null) {
                            ref.setTypeOfObject("N/A");
                        } else {
                            ref.setTypeOfObject(datafile.getDatafileFormat().getFormatType());
                        }

                        ref.setReferenceId(datafile.getId());
                        ref.setInvestigationId(investigation.getId());

                        Collection<Url> cs = new ArrayList<Url>();
                        Url url = new Url();
                        url.setDataRefId(ref);
                        //TODO remove line, URL should neve be null
                        if (datafile.getLocation() == null) {
                            datafile.setLocation(" ");
                        }

                        url.setUrl(datafile.getLocation());
                        cs.add(url);
                        log.trace("Adding: " + datafile.getId());

                        ref.setUrls(cs);
                        toAddDataReference.add(ref);
                    //log.trace(file);
                    }

                }
            }
        }

        String sid = getVisit().getSid();
        try {
            //check if anything selected
            if (toAddBookmarks.size() == 0 && toAddDataReference.size() == 0) {
                warn("Please select atleast item to addition to data links.");
                return null;
            }

            if (toAddBookmarks.size() != 0) {
                log.info("Adding bookmarks: " + toAddBookmarks.size());
                DataCenterDelegate.getInstance().addBookmark(sid, toAddBookmarks);
            }

            if (toAddDataReference.size() != 0) {
                log.info("Adding data references: " + toAddDataReference.size());
                DataCenterDelegate.getInstance().addDataReference(sid, toAddDataReference);
            }

            log.trace("Setting bookamrks and references to null");
            getVisitData().setCurrentBookmarks(null);
            getVisitData().setCurrentDataReferences(null);

        } catch (SessionException ex) {
            error("Session Exception: " + ex.getMessage());
            log.fatal("SessionException: " + sid, ex);
            return null;
        } catch (DataCenterException ex) {
            error("No access to data links: " + ex.getMessage());
            log.fatal("NoAccessToDataCenterException for user for: " + sid, ex);
            return null;
        }

        //remove all true selections

        for (Investigation investigation : getVisitData().getCurrentInvestigations()) {
            investigation.setSelected(false);
            for (Dataset dataset : investigation.getDatasetCollection()) {
                dataset.setSelected(false);
                for (Datafile datafile : dataset.getDatafileCollection()) {
                    datafile.setSelected(false);
                }

            }
        }

        if (toAddBookmarks.size() != 0 && toAddDataReference.size() == 0) {
            //add all stuff to datacenter
            return NavigationConstants.ADD_BOOKMARK_SUCCESS;
        } else {
            return NavigationConstants.ADD_DATA_REFERENCE_SUCCESS;
        }

    }

    public String expandAll() {
        if (tree != null) {
            log.trace("Expanding tree");
            tree.expandAll();
        }

        return null;
    }

    public String collapseAll() {
        if (tree != null) {
            tree.collapseAll();
        }

        return null;
    }
}
