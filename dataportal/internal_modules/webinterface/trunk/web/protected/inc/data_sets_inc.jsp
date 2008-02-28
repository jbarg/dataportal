<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/functions" prefix="fn" %>
<%@ taglib uri="https://ajax4jsf.dev.java.net/ajax" prefix="a4j"%>

<script>
    
    function waitStatus(){
        
        var image =  document.getElementById("body:tableForm:status");
        image.src = "../images/connect_active.gif";
        document.body.style.cursor = 'wait';
        return true;
    }  
    
    function normalStatus(){
        
        var image =  document.getElementById("body:tableForm:status");
        image.src = "../images/connect_idle.gif";
        document.body.style.cursor = 'default';
        return true;
    }          
    
    </script>
<a4j:region  selfRendered="true"> 
    
    <h:form id="tableForm">
        <f:loadBundle basename="uk.ac.dl.dp.web.messages.facility" var="facility_properties" />
        
        <table width="95%" border="0" >
            <tbody>
                <tr>
                    <td width=20">&nbsp;</td>
                    <td>
                    
                    <table border="0" width="100%">
                        <tbody>
                            <tr>
                                <td> <h:messages id="messages" globalOnly="true" warnClass="error" errorClass="error" infoClass="info" />
                                </td>     
                                <td align="right"> 
                                    <h:graphicImage id="status" url="../images/connect_idle.gif" />
                                </td> 
                            </tr>
                        </tbody>      
                    </table>
                    
                    <br />
                    
                    <t:tree2 rendered="#{visit.visitData.investigatonTreeVisable}" binding="#{datasetTree.tree}" id="tree" value="#{datasetTree.data}" var="node" varNodeToggler="t" showRootNode="false" clientSideToggle="true">
                        
                        <%----  Data set branch  ----%>
                        <f:facet name="dataset-folder">
                            <h:panelGroup>
                                <f:facet name="expand">
                                    <t:graphicImage id="gr99" value="../../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="collapse">
                                    <t:graphicImage id="gr98" value="../../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                
                                <h:commandLink onclick="download('#{node.identifier}','DATA_SET','DATA_SETS'); return false;" style="color:black" id="downloadname" actionListener="#{datacenterBean.download}">
                                    <%--    style is used cos IE7 hover does not worj with IE7 :  styleClass="nodeFolderLink"--%>
                                    <h:outputText id="text90" value="#{node.description}" style="font-family: Verdana, Geneva, sans-serif; font-size: 10px;" />
                                    
                                    <f:param name="id" value="#{node.identifier}"/>             
                                </h:commandLink>
                                
                                <%--  <h:outputText id="text891" value=" (#{node.childCount-4})" styleClass="childCount" rendered="#{!empty node.children}"/>--%>
                                <h:outputText value="    " />
                                
                                <h:graphicImage value="../../images/download.gif" width="19" height="14" />                                                             
                                
                                <%--  <h:selectBooleanCheckbox style="background-color:#EAF4F4" title="Select Datasets">   
                                <f:param name="datasets" value="#{node.identifier}"/>
                                <a4j:support reRender="addSelection, downloademail, downloadnow" event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{datasetTree.setSelectedAjax}">
                                <a4j:actionparam name="datasets" value="#{node.identifier}"/>   
                                </a4j:support>
                                </h:selectBooleanCheckbox>--%>
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  Data set no read branch (ie, no datafiles or no datafile which can read) ----%>
                        <f:facet name="dataset-noread-folder">
                            <h:panelGroup>
                                
                                <t:graphicImage id="gr99noread" value="../../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                                
                                <t:graphicImage id="gr98noread" value="../../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                <h:outputText value=" " />
                                <%--    style is used cos IE7 hover does not worj with IE7 :  styleClass="nodeFolderLink"--%>
                                
                                <a4j:commandLink oncomplete="normalStatus();" onclick="waitStatus();"  reRender="tableGrid, status" styleClass="investigation" style="color: black; " id="viewFiles" actionListener="#{datasetTree.viewDataFiles}">
                                    <%--    style is used cos IE7 hover does not worj with IE7 :  styleClass="nodeFolderLink"--%>
                                    <h:outputText id="text934" value="#{node.description}" style="font-family: Verdana, Geneva, sans-serif; font-size: 10px;" />
                                    
                                    <f:param name="datafiles" value="#{node.identifier}"/>             
                                </a4j:commandLink>
                                <%-- <h:selectBooleanCheckbox style="background-color:#EAF4F4" title="Select Datasets">   
                                <f:param name="datasets" value="#{node.identifier}"/>
                                <a4j:support reRender="addSelection, downloademail" event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{datasetTree.setSelectedAjax}">
                                <a4j:actionparam name="datasets" value="#{node.identifier}"/>   
                                </a4j:support>
                                </h:selectBooleanCheckbox>--%>
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----   Investigation branch ----%>
                        <f:facet name="invest">
                            <h:panelGroup>
                                
                                <t:graphicImage id="gr91f9" value="../../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                                
                                <t:graphicImage id="gr9f18" value="../../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                
                                <h:outputText id="text9f10" value="#{node.description}" styleClass="nodeFolder"/>
                                <h:outputText value="    " />
                                <%-- <h:selectBooleanCheckbox  style="background-color:#EAF4F4" title="select_investigation" valueChangeListener="#{datasetTree.setSelected}">
                                <f:param name="investigations" value="#{node.identifier}"/>
                                </h:selectBooleanCheckbox>--%>
                                <%-- <h:selectBooleanCheckbox  style="background-color:#EAF4F4" title="Select Investigation" >
                                <f:param name="investigations" value="#{node.identifier}"/>
                                <a4j:support reRender="addSelection, downloademail, downloadnow" event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{datasetTree.setSelectedAjax}">
                                <a4j:actionparam name="investigations" value="#{node.identifier}"/>   
                                </a4j:support>
                                </h:selectBooleanCheckbox>--%>
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----   Datasets, list number of datasets ----%>
                        <f:facet name="dataset-count-folder">
                            <h:panelGroup>
                                
                                <t:graphicImage id="gr9d91" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                
                                <t:graphicImage id="grd918" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                
                                <a4j:commandLink  oncomplete="normalStatus();" onclick="waitStatus();"  reRender="tableGrid, status" style="color: black; " styleClass="investigation" id="viewSets" actionListener="#{datasetTree.viewDataSets}">
                                    <%--    style is used cos IE7 hover does not worj with IE7 :  styleClass="nodeFolderLink"--%>
                                    <h:outputText id="textd19g0" value="#{node.description}" style="font-family: Verdana, Geneva, sans-serif; font-size: 10px;" />
                                    
                                    <f:param name="datasets" value="#{node.identifier}"/>             
                                </a4j:commandLink>
                            </h:panelGroup>
                        </f:facet>                        
                        
                        <%----   Dataset status  ----%>
                        <f:facet name="status-folder">
                            <h:panelGroup>
                                <f:facet name="expand1">
                                    <t:graphicImage id="gr17872" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="collapse1">
                                    <t:graphicImage id="gr112" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="texdsst78" styleClass="nodeFolder" value="Status: " />
                                
                                <h:outputText id="texht728" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  Investigation number  ----%>
                        <f:facet name="invNumber-folder">
                            <h:panelGroup>
                                <f:facet name="expander17">
                                    <t:graphicImage id="gr1er872" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="collapseer71">
                                    <t:graphicImage id="gr1er712" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="text7er78" styleClass="nodeFolder" value="Rb number: " />
                                
                                <h:outputText id="text77er28" styleClass="document" value="#{fn:toLowerCase(node.description)}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  Dataset type  ----%>
                        <f:facet name="type-folder">
                            <h:panelGroup>
                                <f:facet name="expand17">
                                    <t:graphicImage id="gr1872" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="collapse71">
                                    <t:graphicImage id="gr1712" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="text778" styleClass="nodeFolder" value="Type: " />
                                
                                <h:outputText id="text7728" styleClass="document" value="#{fn:toLowerCase(node.description)}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  Investigation  instrument   ----%>
                        <f:facet name="instrument-folder">
                            <h:panelGroup>
                                <f:facet name="expa4ngd1">
                                    <t:graphicImage id="d" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="colla4ffge1">
                                    <t:graphicImage id="grddddgf2" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="te44xf7s8" styleClass="nodeFolder" value="#{facility_properties['instrument.name']}: " />
                                
                                <h:outputText id="te4xs7f28" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  Investigation  visit id   ----%>
                        <f:facet name="visitId-folder">
                            <h:panelGroup>
                                <f:facet name="expa4551">
                                    <t:graphicImage id="dsfdfd" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="colddffge1">
                                    <t:graphicImage id="grddd4232" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="te44ggr8" styleClass="nodeFolder" value="Visit Id: " />
                                
                                <h:outputText id="t43fgs7f28" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        
                        <%----  datafile  ----%>
                        <f:facet name="file-folder">
                            <h:panelGroup>
                                <f:facet name="expand6">
                                    <t:graphicImage id="gr60" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="collapse6">
                                    <t:graphicImage id="gr60u" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText value="           " />
                                
                                <t:graphicImage value="../../images/document.png" border="0"   />                                   
                                <h:outputText value=" " />
                                <h:commandLink onclick="download('#{node.identifier}','DATA_FILE','DATA_SETS'); return false;" style="color:black" id="downloadname112">
                                    <%--    style is used cos IE7 hover does not worj with IE7 :  styleClass="file"--%>
                                    <h:outputText value=" #{node.description}" style="font-family: Verdana, Geneva, sans-serif; font-size: 10px;" />
                                    <f:param name="id16" value="#{node.identifier}"/>              
                                </h:commandLink>
                                <h:outputText value="         " />
                                
                                <h:graphicImage value="../../images/download.gif" width="19" height="14" />                                                             
                                
                                <%--  <h:selectBooleanCheckbox  style="background-color:#EAF4F4" title="Select DataFile" >
                                <f:param name="datafiles" value="#{node.identifier}"/>
                                <a4j:support reRender="addSelection, downloademail, downloadnow" event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{datasetTree.setSelectedAjax}">
                                <a4j:actionparam name="datafiles" value="#{node.identifier}"/>   
                                </a4j:support>
                                </h:selectBooleanCheckbox>--%>
                                
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  datafile (EMAT) in folder (show folder instead of file image  ----%>
                        <f:facet name="file-inFolder-folder">
                            <h:panelGroup>
                                
                                <t:graphicImage id="gr60u1" value="../../images/yellow-folder-closed.png" border="0"/>
                                
                                <h:outputText value="          " />                                         
                                
                                <h:commandLink onclick="download('#{node.identifier}','DATA_FILE_IN_FOLDERS','DATA_SETS'); return false;" style="color:black" id="downloadname1182">
                                    <%--    style is used cos IE7 hover does not worj with IE7 :  styleClass="file"--%>
                                    <h:outputText value="#{node.description}" style="font-family: Verdana, Geneva, sans-serif; font-size: 10px;" />
                                    <f:param name="id16" value="#{node.identifier}"/>              
                                </h:commandLink>
                                <h:outputText value="         " />
                                
                                <h:graphicImage value="../../images/download.gif" width="19" height="14" />                                                             
                                
                                <%--   <h:selectBooleanCheckbox style="background-color:#EAF4F4" title="Select Datasets">   
                                <f:param name="datafiles" value="#{node.identifier}"/>
                                <a4j:support reRender="addSelection, downloademail, downloadnow" event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{datasetTree.setSelectedAjax}">
                                <a4j:actionparam name="datafiles" value="#{node.identifier}"/>   
                                </a4j:support>
                                </h:selectBooleanCheckbox>         --%>                                                                             
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  datafile no download  ----%>
                        <f:facet name="file-noread-folder">
                            <h:panelGroup>                   
                                <t:graphicImage value="../../images/document.png" border="0"/>                                   
                                <h:outputText value="        " />
                                
                                <%--    style is used cos IE7 hover does not worj with IE7 :  styleClass="file"--%>
                                <h:outputText value="#{node.description}" style="font-family: Verdana, Geneva, sans-serif; font-size: 10px;" />
                                
                                <h:outputText value="         " />
                                
                                <%--   <h:selectBooleanCheckbox  style="background-color:#EAF4F4" title="Select DataFile" >
                                <f:param name="datafiles" value="#{node.identifier}"/>
                                <a4j:support reRender="addSelection, downloademail, downloadnow" event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{datasetTree.setSelectedAjax}">
                                <a4j:actionparam name="datafiles" value="#{node.identifier}"/>   
                                </a4j:support>
                                </h:selectBooleanCheckbox>--%>
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%---- Investigation facility  ----%>
                        <f:facet name="fac-folder">
                            <h:panelGroup>
                                <f:facet name="expand1f3">
                                    <t:graphicImage id="grf13" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="collapse1f3">
                                    <t:graphicImage id="gr1f13" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="text7f38" styleClass="nodeFolder" value="Facility: " />
                                
                                <h:outputText id="text73f8" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  dataset description  ----%>
                        <f:facet name="desc-folder">
                            <h:panelGroup>
                                <f:facet name="expand1ff3">
                                    <t:graphicImage id="grff13" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="collapse1ff3">
                                    <t:graphicImage id="gr1ff13" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="text7ff38" styleClass="nodeFolder" value="Description: " />
                                
                                <h:outputText id="text73ff8" escape="false" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                    </t:tree2>
                    
                </tr>
            </tbody>      
            
        </table>                        
    </h:form>
    
    <br />
    <a4j:form>
        <h:panelGrid width="100%" border="0" id="tableGrid" columns="1" styleClass="scrollerTable2" columnClasses="standardTable_ColumnCentered" >
            
            <%---  ///////////  DataFile Table /////////        -----%>
            <h:panelGrid rendered="#{visit.visitData.datafileTableVisable}" id="topgrid" columns="2" styleClass="scrollerTable2" columnClasses="standardTable_ColumnCentered" border="0" width="100%">
                <h:panelGroup style="float: left" rendered="#{fn:length(sessionScope.visit.visitData.currentDatafiles) > sessionScope.sessionHistory.numberOfResultsDatafiles}" >            
                    <t:dataScroller id="scroll_4_datafile"
                                    for="data"
                                    fastStep="10"
                                    pageCountVar="pageCount"
                                    pageIndexVar="pageIndex"
                                    styleClass="scroller"
                                    paginator="true"
                                    paginatorMaxPages="9"
                                    paginatorTableClass="paginator"
                                    paginatorActiveColumnStyle="font-weight:bold;">
                        <f:actionListener type="uk.ac.dl.dp.web.navigation.DataScrollerActionListener"/>
                        <f:facet name="first" >
                            <t:graphicImage url="../../images/arrow-first.gif" border="1" />
                        </f:facet>
                        <f:facet name="last">
                            <t:graphicImage url="../../images/arrow-last.gif" border="1" />
                        </f:facet>
                        <f:facet name="previous">
                            <t:graphicImage url="../../images/arrow-previous.gif" border="1" />
                        </f:facet>
                        <f:facet name="next">
                            <t:graphicImage url="../../images/arrow-next.gif" border="1" />
                        </f:facet>
                        <f:facet name="fastforward">
                            <t:graphicImage url="../../images/arrow-ff.gif" border="1" />
                        </f:facet>
                        <f:facet name="fastrewind">
                            <t:graphicImage url="../../images/arrow-fr.gif" border="1" />
                        </f:facet>
                    </t:dataScroller>
                    
                </h:panelGroup>
                
                <h:panelGroup rendered="#{fn:length(visit.visitData.currentDatafiles) <= sessionHistory.numberOfResultsDatafiles}"  />
                
                <!---  Drop down list of length of table -->
                <h:panelGroup rendered="#{fn:length(visit.visitData.currentDatafiles) > 5 }" style="float: right">
                    <h:outputText style="font-size:10px; color: black;  font-weight: bold;" value="Maximum displayed: " />
                    <h:selectOneMenu id="listj_id_3_datafile" required="false" immediate="false" value="#{sessionHistory.numberOfResultsDatafilesString2}" >                           
                        <a4j:support event="onchange" action="#{datafileBean.maxDisplay}" ajaxSingle="true" reRender="data, topgrid, bottomgrid" />            
                        
                        <f:selectItem itemLabel="5"  itemValue="5"/> 
                        <f:selectItem itemLabel="10"  itemValue="10"/> 
                        <f:selectItem itemLabel="20"  itemValue="20"/> 
                        <f:selectItem itemLabel="50"  itemValue="50"/> 
                        <f:selectItem itemLabel="100"  itemValue="100"/> 
                    </h:selectOneMenu>   
                </h:panelGroup>                            
                
            </h:panelGrid> 
            
            
            
            <!---  Actual Table -->
            <t:dataTable id="data" width="100%"
                         styleClass="scrollerTable"
                         headerClass="standardTable_Header"
                         footerClass="standardTable_Header"
                         rowClasses="standardTable_Row1,standardTable_Row2"
                         columnClasses="standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered"
                         var="dataFile"
                         rowIndexVar="rowIndex"
                         value="#{datafileBean.datafiles}"
                         preserveDataModel="true"
                         rows="#{sessionHistory.numberOfResultsDatafiles}"       
                         binding="#{datafileBean.table}"
                         sortColumn="#{datafileBean.sort}"
                         sortAscending="#{datafileBean.ascending}"
                         preserveSort="true"
                         varDetailToggler="detailToggler"  
                         rendered="#{visit.visitData.datafileTableVisable}"  >
                <f:facet name="header">
                    <h:outputText value="#{visit.visitData.currentDataset}'s datafiles" />
                </f:facet>
                
                <!--    Number     -->  
                <h:column>
                    <f:facet name="header">
                        <h:outputText style="table-header" value="#" />       
                    </f:facet>
                    
                    <h:outputText styleClass="investigation" value="#{rowIndex+1}" />
                    
                </h:column>
                
                <!--  Expand -->
                <h:column>
                    <f:facet name="header">
                        <h:panelGrid columns="" >
                            
                            <%-- <a4j:commandLink reRender="data"  style="table-header" ajaxSingle="true" id="expandAll" rendered="#{!visit.visitData.datafileExpanded}" actionListener="#{datafileBean.expandAll}">
                                
                                <t:graphicImage   id="exp" value="../../images/button_plus1.gif"  border="0"/>
                            </a4j:commandLink>   
                            <a4j:commandLink reRender="data" style="table-header" ajaxSingle="true" id="collapseAll" rendered="#{visit.visitData.datafileExpanded}" actionListener="#{datafileBean.collapseAll}">
                                
                                <t:graphicImage  id="coll" value="../../images/button_minus1.gif"  border="0"/>
                            </a4j:commandLink> --%>
                            
                        </h:panelGrid>        
                    </f:facet>
                    
                    <h:panelGrid columns="" >
                        <a4j:commandLink oncomplete="normalStatus();" onclick="waitStatus();"  reRender="data" style="table-header" ajaxSingle="true" id="abS" actionListener="#{datafileBean.getDatafileParameters}" action="#{detailToggler.toggleDetail}">
                            
                            <t:graphicImage id="up" value="../../images/button_plus1.gif" rendered="#{!detailToggler.currentDetailExpanded}" border="0"/>
                            <t:graphicImage id="up-f" value="../../images/button_minus1.gif" rendered="#{detailToggler.currentDetailExpanded}" border="0"/>
                        </a4j:commandLink>  
                    </h:panelGrid>
                    
                </h:column>
                
                <!--  Name -->
                <h:column>
                    <f:facet name="header">
                        
                        <a4j:commandLink reRender="data, props" style="table-header" id="name" actionListener="#{datafileBean.sortColumn}">
                            <h:outputText value="Name" />
                            <f:param name="column" value="name"/>
                            <t:graphicImage id="acname" value="../../images/ascending-arrow.gif" rendered="#{datafileBean.name}" border="0"/>
                            <t:graphicImage id="dename" value="../../images/descending-arrow.gif" rendered="#{datafileBean.notName}" border="0"/>
                            
                        </a4j:commandLink>
                    </f:facet>
                    
                    <!--  Download -->
                    <h:commandLink rendered="#{dataFile.icatRole.actionDownload && dataFile.location != null}" onclick="download('#{dataFile.id}','DATA_FILE','DATA_SETS'); return false;" style="color: black; " styleClass="investigation" id="downloadname">                    
                        <h:outputText value="#{dataFile.name}" />
                    </h:commandLink> 
                    
                    <!-- No download -->
                    <h:outputText rendered="#{!dataFile.icatRole.actionDownload || dataFile.location == null}" value="#{dataFile.name}" />
                    
                </h:column>
                
                <!--  File Size -->
                <h:column>
                    <f:facet name="header">
                        
                        <a4j:commandLink reRender="data, props" style="table-header" id="size" actionListener="#{datafileBean.sortColumn}">
                            <h:outputText value="File Size (B)" />
                            <f:param name="column" value="size"/>
                            <t:graphicImage id="afilesize" value="../../images/ascending-arrow.gif" rendered="#{datafileBean.fileSize}" border="0"/>
                            <t:graphicImage id="dfilesize" value="../../images/descending-arrow.gif" rendered="#{datafileBean.notFileSize}" border="0"/>
                            
                        </a4j:commandLink>
                    </f:facet>
                    
                    <h:outputText value="#{dataFile.fileSize}" />
                    
                </h:column>
                
                <!--  Format -->
                <h:column>
                    <f:facet name="header">
                        
                        <a4j:commandLink reRender="data, props" style="table-header" id="format" actionListener="#{datafileBean.sortColumn}">
                            <h:outputText value="Format" />
                            <f:param name="column" value="format"/>
                            <t:graphicImage id="aformat" value="../../images/ascending-arrow.gif" rendered="#{datafileBean.format}" border="0"/>
                            <t:graphicImage id="dformat" value="../../images/descending-arrow.gif" rendered="#{datafileBean.notFormat}" border="0"/>
                            
                        </a4j:commandLink>
                    </f:facet>
                    
                    <h:outputText value="#{dataFile.datafileFormat.datafileFormatPK.name}" />
                </h:column>
                
                
                <!--  Version -->
                <h:column>
                    <f:facet name="header">
                        
                        <a4j:commandLink reRender="data, props" style="table-header" id="version" actionListener="#{datafileBean.sortColumn}">
                            <h:outputText value="Format Version" />
                            <f:param name="column" value="formatVersion"/>
                            <t:graphicImage id="aversion" value="../../images/ascending-arrow.gif" rendered="#{datafileBean.formatVersion}" border="0"/>
                            <t:graphicImage id="dversion" value="../../images/descending-arrow.gif" rendered="#{datafileBean.notFormatVersion}" border="0"/>
                            
                        </a4j:commandLink>
                    </f:facet>
                    
                    <h:outputText value="#{dataFile.datafileFormat.datafileFormatPK.version}" />
                </h:column>
                
                
                <!-- Format Type -->
                <h:column>
                    <f:facet name="header">
                        
                        <a4j:commandLink reRender="data, props" style="table-header" id="formatType" actionListener="#{datafileBean.sortColumn}">
                            <h:outputText value="Format Type" />
                            <f:param name="column" value="formatType"/>
                            <t:graphicImage id="aformatType" value="../../images/ascending-arrow.gif" rendered="#{datafileBean.formatType}" border="0"/>
                            <t:graphicImage id="dformatType" value="../../images/descending-arrow.gif" rendered="#{datafileBean.notFormatType}" border="0"/>
                            
                        </a4j:commandLink>
                    </f:facet>
                    
                    <h:outputText value="#{dataFile.datafileFormat.formatType}" />
                </h:column>
                
                <!-- Create Time -->
                <h:column>
                    <f:facet name="header">
                        
                        <a4j:commandLink reRender="data, props" style="table-header" id="createTime" actionListener="#{datafileBean.sortColumn}">
                            <h:outputText value="Create Time" />
                            <f:param name="column" value="createTime"/>
                            <t:graphicImage id="acreateTime" value="../../images/ascending-arrow.gif" rendered="#{datafileBean.createTime}" border="0"/>
                            <t:graphicImage id="dcreateTime" value="../../images/descending-arrow.gif" rendered="#{datafileBean.notCreateTime}" border="0"/>
                            
                        </a4j:commandLink>
                    </f:facet>
                    
                    <h:outputText value="#{fn:substringBefore(fn:substringAfter(dataFile.datafileCreateTime,\"T\"),\"+\")}  #{fn:substringBefore(dataFile.datafileCreateTime,\"T\")}" />
                </h:column>
                
                <!-- Download -->
                <h:column>
                    <f:facet name="header">
                        
                        <t:graphicImage id="download" value="../../images/download.gif" border="0"/>
                        
                    </f:facet>
                    
                    <h:selectBooleanCheckbox title="select datafile" disabled="#{!(dataFile.icatRole.actionDownload && dataFile.location != null)}" value="#{dataFile.selected}" >
                        <f:param name="id" value="#{dataFile.id}"/>
                        <a4j:support reRender="data" event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{datafileBean.listenAjax}">
                            <a4j:actionparam name="id" value="#{dataFile.id}"/>   
                        </a4j:support>
                    </h:selectBooleanCheckbox>   
                </h:column>
                
                
                
                <f:facet name="detailStamp">      
                    
                    
                    <t:dataTable preserveSort="true" 
                                 align="center" 
                                 styleClass="detailTable"
                                 headerClass="standardTable_Header"
                                 footerClass="standardTable_Header"
                                 rowClasses="standardTableDetail_Row1,standardTableDetail_Row1"
                                 columnClasses="standardTable_ColumnCenteredTop"
                                 
                                 width="100%" 
                                 id="parameter" 
                                 var="parameter"
                                 value="#{dataFile.datafileParameterCollection}">
                                 
                        <!-- Name -->
                        <h:column>
                            <f:facet name="header">
                                <h:outputText value="Name" style="font-size:12px; color: blue;  font-weight: bold;" />
                            </f:facet>
                            
                            <h:outputText style="valign:top; font-size:12px;" value="#{parameter.datafileParameterPK.name}" />
                            
                        </h:column> 
                        
                        <!-- Units -->
                        <h:column>
                            <f:facet name="header">
                                <h:outputText value="Units" style="font-size:12px; color: blue;  font-weight: bold;" />
                            </f:facet>
                            
                            <h:outputText style="valign:top; font-size:12px;" value="#{parameter.datafileParameterPK.units}" />
                            
                        </h:column> 
                        
                        <!-- Value -->
                        <h:column>
                            <f:facet name="header">
                                <h:outputText value="Value" style="font-size:12px; color: blue;  font-weight: bold;" />
                            </f:facet>
                            
                            <h:outputText rendered="#{parameter.numeric}" style="valign:top; font-size:12px;" value="#{parameter.numericValue}" />
                            <h:outputText rendered="#{!parameter.numeric}" style="valign:top; font-size:12px;" value="#{parameter.stringValue}" />
                            
                        </h:column> 
                        
                    </t:dataTable>                      
                    
                </f:facet>              
                
            </t:dataTable>
            
            <h:panelGrid rendered="#{visit.visitData.datafileTableVisable}" id="bottomgrid" columns="2" styleClass="scrollerTable2" columnClasses="standardTable_ColumnCentered" border="0" width="100%">
                <h:panelGroup style="float: left" rendered="#{fn:length(visit.visitData.currentDatafiles) > sessionHistory.numberOfResultsDatafiles}" >            
                    <t:dataScroller id="scroll_2_datafile"
                                    for="data"
                                    fastStep="10"
                                    pageCountVar="pageCount"
                                    pageIndexVar="pageIndex"
                                    styleClass="scroller"
                                    paginator="true"
                                    paginatorMaxPages="9"
                                    paginatorTableClass="paginator"
                                    paginatorActiveColumnStyle="font-weight:bold;">
                        <f:actionListener type="uk.ac.dl.dp.web.navigation.DataScrollerActionListener"/>
                        <f:facet name="first" >
                            <t:graphicImage url="../../images/arrow-first.gif" border="1" />
                        </f:facet>
                        <f:facet name="last">
                            <t:graphicImage url="../../images/arrow-last.gif" border="1" />
                        </f:facet>
                        <f:facet name="previous">
                            <t:graphicImage url="../../images/arrow-previous.gif" border="1" />
                        </f:facet>
                        <f:facet name="next">
                            <t:graphicImage url="../../images/arrow-next.gif" border="1" />
                        </f:facet>
                        <f:facet name="fastforward">
                            <t:graphicImage url="../../images/arrow-ff.gif" border="1" />
                        </f:facet>
                        <f:facet name="fastrewind">
                            <t:graphicImage url="../../images/arrow-fr.gif" border="1" />
                        </f:facet>
                    </t:dataScroller>
                    
                </h:panelGroup>
                
                <h:panelGroup rendered="#{fn:length(visit.visitData.currentDatafiles) <= sessionHistory.numberOfResultsDatafiles}"  />
                
                <!---  Drop down list of length of table -->
                <h:panelGroup rendered="#{fn:length(visit.visitData.currentDatafiles) > 5 }" style="float: right">
                    <h:outputText style="font-size:10px; color: black;  font-weight: bold;" value="Maximum displayed: " />
                    <h:selectOneMenu id="listj_id_1_datafile" required="false" immediate="false" value="#{sessionHistory.numberOfResultsDatafilesString}" >                           
                        <a4j:support event="onchange" action="#{datafileBean.maxDisplay}" ajaxSingle="true" reRender="data, topgrid, bottomgrid" />            
                        
                        <f:selectItem itemLabel="5"  itemValue="5"/> 
                        <f:selectItem itemLabel="10"  itemValue="10"/> 
                        <f:selectItem itemLabel="20"  itemValue="20"/> 
                        <f:selectItem itemLabel="50"  itemValue="50"/> 
                        <f:selectItem itemLabel="100"  itemValue="100"/> 
                    </h:selectOneMenu>   
                </h:panelGroup>
                
                <h:panelGroup style="float: left" rendered="#{fn:length(visit.visitData.currentDatafiles) > 0 }">            
                    
                    <t:dataScroller id="scroll_1_datafile"
                                    for="data"
                                    rowsCountVar="rowsCount"
                                    displayedRowsCountVar="displayedRowsCountVar"
                                    firstRowIndexVar="firstRowIndex"
                                    lastRowIndexVar="lastRowIndex"
                                    pageCountVar="pageCount"
                                    immediate="true"
                                    pageIndexVar="pageIndex"
                                    >
                        <h:outputFormat value="{0} Datafiles found, displaying {1}, from {2} to {3}. Page {4} / {5}" styleClass="standard" >
                            <f:param value="#{rowsCount}" />
                            <f:param value="#{displayedRowsCountVar}" />
                            <f:param value="#{firstRowIndex}" />
                            <f:param value="#{lastRowIndex}" />
                            <f:param value="#{pageIndex}" />
                            <f:param value="#{pageCount}" />
                        </h:outputFormat>
                    </t:dataScroller>
                </h:panelGroup>           
            </h:panelGrid>                                        
            <%---  ///////////  End of DataFile Table /////////        -----%>
            
            <%---  ///////////  DataSet Table /////////        -----%>
            <h:panelGrid rendered="#{visit.visitData.datasetTableVisable}" id="topgridds" columns="2" styleClass="scrollerTable2" columnClasses="standardTable_ColumnCentered" border="0" width="100%">
                <h:panelGroup style="float: left" rendered="#{fn:length(sessionScope.visit.visitData.currentDatasets) > sessionScope.sessionHistory.numberOfResultsDatasets}" >            
                    <t:dataScroller id="scroll_5_datafile"
                                    for="dataset"
                                    fastStep="10"
                                    pageCountVar="pageCount"
                                    pageIndexVar="pageIndex"
                                    styleClass="scroller"
                                    paginator="true"
                                    paginatorMaxPages="9"
                                    paginatorTableClass="paginator"
                                    paginatorActiveColumnStyle="font-weight:bold;">
                        <f:actionListener type="uk.ac.dl.dp.web.navigation.DataScrollerActionListener"/>
                        <f:facet name="first" >
                            <t:graphicImage url="../../images/arrow-first.gif" border="1" />
                        </f:facet>
                        <f:facet name="last">
                            <t:graphicImage url="../../images/arrow-last.gif" border="1" />
                        </f:facet>
                        <f:facet name="previous">
                            <t:graphicImage url="../../images/arrow-previous.gif" border="1" />
                        </f:facet>
                        <f:facet name="next">
                            <t:graphicImage url="../../images/arrow-next.gif" border="1" />
                        </f:facet>
                        <f:facet name="fastforward">
                            <t:graphicImage url="../../images/arrow-ff.gif" border="1" />
                        </f:facet>
                        <f:facet name="fastrewind">
                            <t:graphicImage url="../../images/arrow-fr.gif" border="1" />
                        </f:facet>
                    </t:dataScroller>
                    
                </h:panelGroup>
                
                <h:panelGroup rendered="#{fn:length(visit.visitData.currentDatasets) <= sessionHistory.numberOfResultsDatasets}"  />
                
                <!---  Drop down list of length of table -->
                <h:panelGroup rendered="#{fn:length(visit.visitData.currentDatasets) > 5 }" style="float: right">
                    <h:outputText style="font-size:10px; color: black;  font-weight: bold;" value="Maximum displayed: " />
                    <h:selectOneMenu id="listj_id_6_dataset" required="false" immediate="false" value="#{sessionHistory.numberOfResultsDatasetsString2}" >                           
                        <a4j:support event="onchange" action="#{datasetBean.maxDisplay}" ajaxSingle="true" reRender="dataset, topgridds, bottomgridds" />            
                        
                        <f:selectItem itemLabel="5"  itemValue="5"/> 
                        <f:selectItem itemLabel="10"  itemValue="10"/> 
                        <f:selectItem itemLabel="20"  itemValue="20"/> 
                        <f:selectItem itemLabel="50"  itemValue="50"/> 
                        <f:selectItem itemLabel="100"  itemValue="100"/> 
                    </h:selectOneMenu>   
                </h:panelGroup>                            
                
            </h:panelGrid> 
            
            <!-- Dataset table -->                        
            <t:dataTable id="dataset" width="100%"
                         styleClass="scrollerTable"
                         headerClass="standardTable_Header"
                         footerClass="standardTable_Header"
                         rowClasses="standardTable_Row1,standardTable_Row2"
                         columnClasses="standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered"
                         var="dataSet"
                         value="#{datasetBean.datasets}"
                         preserveDataModel="true"
                          rowIndexVar="rowIndex"
                         rows="#{sessionHistory.numberOfResultsDatasets}"       
                         binding="#{datasetBean.table}"
                         sortColumn="#{datasetBean.sort}"
                         sortAscending="#{datasetBean.ascending}"
                         preserveSort="true"
                         varDetailToggler="detailToggler"  
                         rendered="#{visit.visitData.datasetTableVisable}"  >
                             
                <f:facet name="header">
                    <h:outputText value="#{visit.visitData.currentInvestigation}'s datasets" />
                </f:facet>
                
                <!--    Number     -->  
                <h:column>
                    <f:facet name="header">
                        <h:outputText style="table-header" value="#" />       
                    </f:facet>
                    
                    <h:outputText styleClass="investigation" value="#{rowIndex+1}" />
                    
                </h:column>
                
                <!--  Expand -->
                <h:column>
                    <f:facet name="header">
                        <h:panelGrid columns="" >
                            
                            
                            <%--  <a4j:commandLink reRender="dataset"  style="table-header" ajaxSingle="true" id="expandAll" rendered="#{!visit.visitData.datasetExpanded}" actionListener="#{datasetBean.expandAll}">
                                
                                <t:graphicImage  id="expds" value="../../images/button_plus1.gif"  border="0"/>
                            </a4j:commandLink>   
                            <a4j:commandLink reRender="data" style="table-header" ajaxSingle="true" id="collapseAll" rendered="#{visit.visitData.datasetExpanded}" actionListener="#{datasetBean.collapseAll}">
                                
                                <t:graphicImage  id="collds" value="../../images/button_minus1.gif"  border="0"/>
                            </a4j:commandLink> --%>
                            
                        </h:panelGrid>        
                    </f:facet>
                    
                    <h:panelGrid  columns="" >
                        <a4j:commandLink actionListener="#{datasetBean.getDatasetParameters}" oncomplete="normalStatus();" onclick="waitStatus();"  reRender="dataset" style="table-header" ajaxSingle="true" id="abS" action="#{detailToggler.toggleDetail}">
                            
                            <t:graphicImage id="upds" value="../../images/button_plus1.gif" rendered="#{!detailToggler.currentDetailExpanded}" border="0"/>
                            <t:graphicImage id="up-fds" value="../../images/button_minus1.gif" rendered="#{detailToggler.currentDetailExpanded}" border="0"/>
                        </a4j:commandLink>  
                    </h:panelGrid>
                </h:column>
                
                <!--  Name -->
                <h:column>
                    <f:facet name="header">
                        
                        <a4j:commandLink reRender="dataset, props" style="table-header" id="name" actionListener="#{datasetBean.sortColumn}">
                            <h:outputText value="Name" />
                            <f:param name="column" value="name"/>
                            <%--   <c:if test="${requestScope.investigationBean.sort == 'name'}" >--%>
                            <t:graphicImage id="acname" value="../../images/ascending-arrow.gif" rendered="#{datasetBean.name}" border="0"/>
                            <t:graphicImage id="dename" value="../../images/descending-arrow.gif" rendered="#{datasetBean.notName}" border="0"/>
                            <%--  </c:if>--%>
                        </a4j:commandLink>
                    </f:facet>
                    
                    <a4j:commandLink oncomplete="normalStatus();" onclick="waitStatus();"  reRender="status, tableGrid" style="color:black" id="viewFilesTable" actionListener="#{datasetTree.viewDataFiles}">
                        <%--    style is used cos IE7 hover does not worj with IE7 :  styleClass="nodeFolderLink"--%>
                        <h:outputText id="textfgg4" value="#{dataSet.name}" style="font-family: Verdana, Geneva, sans-serif; font-size: 12px;" />
                        
                        <f:param name="datafiles" value="#{dataSet.uniqueId}}"/>             
                        
                    </a4j:commandLink>
                    
                </h:column>
                
                <!-- Status -->
                <h:column>
                    <f:facet name="header">
                        
                        <a4j:commandLink reRender="dataset, props" style="table-header" id="status" actionListener="#{datasetBean.sortColumn}">
                            <h:outputText value="Status" />
                            <f:param name="column" value="status"/>
                            <%--   <c:if test="${requestScope.investigationBean.sort == 'name'}" >--%>
                            <t:graphicImage id="afilesizeds" value="../../images/ascending-arrow.gif" rendered="#{datasetBean.status}" border="0"/>
                            <t:graphicImage id="dfilesizeds" value="../../images/descending-arrow.gif" rendered="#{datasetBean.notStatus}" border="0"/>
                            <%--  </c:if>--%>
                        </a4j:commandLink>
                    </f:facet>
                    <h:outputText value="#{dataSet.datasetStatus}" />
                    
                </h:column>
                
                <!--  Type -->
                <h:column>
                    <f:facet name="header">
                        
                        <a4j:commandLink reRender="dataset, props" style="table-header" id="type" actionListener="#{datasetBean.sortColumn}">
                            <h:outputText value="Type" />
                            <f:param name="column" value="type"/>
                            <%--   <c:if test="${requestScope.investigationBean.sort == 'name'}" >--%>
                            <t:graphicImage id="aformatds" value="../../images/ascending-arrow.gif" rendered="#{datasetBean.type}" border="0"/>
                            <t:graphicImage id="dformatds" value="../../images/descending-arrow.gif" rendered="#{datasetBean.notType}" border="0"/>
                            <%--  </c:if>--%>
                        </a4j:commandLink>
                    </f:facet>
                    
                    <h:outputText value="#{dataSet.datasetType}" />
                    
                </h:column>
                
                
                <!--  Description -->
                <h:column>
                    <f:facet name="header">
                        
                        <a4j:commandLink reRender="dataset, props" style="table-header" id="description" actionListener="#{datasetBean.sortColumn}">
                            <h:outputText value="Description" />
                            <f:param name="column" value="description"/>
                            <%--   <c:if test="${requestScope.investigationBean.sort == 'name'}" >--%>
                            <t:graphicImage id="aversionds" value="../../images/ascending-arrow.gif" rendered="#{datasetBean.description}" border="0"/>
                            <t:graphicImage id="dversionds" value="../../images/descending-arrow.gif" rendered="#{datasetBean.notDescription}" border="0"/>
                            <%--  </c:if>--%>
                        </a4j:commandLink>
                    </f:facet>
                    
                    <h:outputText value="#{dataSet.description}" />
                    
                </h:column>
                
                
                <!-- Sample -->
                <h:column>
                    <f:facet name="header">
                        
                        <a4j:commandLink reRender="dataset, props" style="table-header" id="sample" actionListener="#{datasetBean.sortColumn}">
                            <h:outputText value="Sample" />
                            <f:param name="column" value="sample"/>
                            <%--   <c:if test="${requestScope.investigationBean.sort == 'name'}" >--%>
                            <t:graphicImage id="aformatTypeds" value="../../images/ascending-arrow.gif" rendered="#{datasetBean.sample}" border="0"/>
                            <t:graphicImage id="dformatTypeds" value="../../images/descending-arrow.gif" rendered="#{datasetBean.notSample}" border="0"/>
                            <%--  </c:if>--%>
                        </a4j:commandLink>
                    </f:facet>
                    
                    <h:outputText value="#{fn:substringAfter(dataSet.uniqueId,\":\")}" />
                    
                </h:column>
                
                
                <f:facet name="detailStamp">    
                    
                    <t:dataTable preserveSort="true" 
                                 align="center" 
                                 styleClass="detailTable"
                                 headerClass="standardTable_Header"
                                 footerClass="standardTable_Header"
                                 rowClasses="standardTableDetail_Row1,standardTableDetail_Row1"
                                 columnClasses="standardTable_ColumnCenteredTop"
                                 
                                 width="100%" 
                                 id="datasetParameter" 
                                 var="datasetParameter"
                                 value="#{dataSet.datasetParameterCollection}">
                                 
                        <!-- Name -->
                        <h:column>
                            <f:facet name="header">
                                <h:outputText value="Name" style="font-size:12px; color: blue;  font-weight: bold;" />
                            </f:facet>
                            
                            <h:outputText style="valign:top; font-size:12px;" value="#{datasetParameter.datasetParameterPK.name}" />
                            
                        </h:column> 
                        
                        <!-- Units -->
                        <h:column>
                            <f:facet name="header">
                                <h:outputText value="Units" style="font-size:12px; color: blue;  font-weight: bold;" />
                            </f:facet>
                            
                            <h:outputText style="valign:top; font-size:12px;" value="#{datasetParameter.datasetParameterPK.units}" />
                            
                        </h:column> 
                        
                        <!-- Value -->
                        <h:column>
                            <f:facet name="header">
                                <h:outputText value="Value" style="font-size:12px; color: blue;  font-weight: bold;" />
                            </f:facet>
                            
                            <h:outputText rendered="#{datasetParameter.numeric}" style="valign:top; font-size:12px;" value="#{datasetParameter.numericValue}" />
                            <h:outputText rendered="#{!datasetParameter.numeric}" style="valign:top; font-size:12px;" value="#{datasetParameter.stringValue}" />
                            
                        </h:column> 
                        
                    </t:dataTable>                      
                    
                </f:facet>              
                
            </t:dataTable>
            
            
            <h:panelGrid rendered="#{visit.visitData.datasetTableVisable}" id="bottomgridds" columns="2" styleClass="scrollerTable2" columnClasses="standardTable_ColumnCentered" border="0" width="100%">
                <h:panelGroup style="float: left" rendered="#{fn:length(visit.visitData.currentDatasets) > sessionHistory.numberOfResultsDatasets}" >            
                    <t:dataScroller id="scroll_6_dataset"
                                    for="dataset"
                                    fastStep="10"
                                    pageCountVar="pageCount"
                                    pageIndexVar="pageIndex"
                                    styleClass="scroller"
                                    paginator="true"
                                    paginatorMaxPages="9"
                                    paginatorTableClass="paginator"
                                    paginatorActiveColumnStyle="font-weight:bold;">
                        <f:actionListener type="uk.ac.dl.dp.web.navigation.DataScrollerActionListener"/>
                        <f:facet name="first" >
                            <t:graphicImage url="../../images/arrow-first.gif" border="1" />
                        </f:facet>
                        <f:facet name="last">
                            <t:graphicImage url="../../images/arrow-last.gif" border="1" />
                        </f:facet>
                        <f:facet name="previous">
                            <t:graphicImage url="../../images/arrow-previous.gif" border="1" />
                        </f:facet>
                        <f:facet name="next">
                            <t:graphicImage url="../../images/arrow-next.gif" border="1" />
                        </f:facet>
                        <f:facet name="fastforward">
                            <t:graphicImage url="../../images/arrow-ff.gif" border="1" />
                        </f:facet>
                        <f:facet name="fastrewind">
                            <t:graphicImage url="../../images/arrow-fr.gif" border="1" />
                        </f:facet>
                    </t:dataScroller>
                    
                </h:panelGroup>
                
                <h:panelGroup rendered="#{fn:length(visit.visitData.currentDatasets) <= sessionHistory.numberOfResultsDatasets}"  />
                
                <!---  Drop down list of length of table -->
                <h:panelGroup rendered="#{fn:length(visit.visitData.currentDatasets) > 5 }" style="float: right">
                    <h:outputText style="font-size:10px; color: black;  font-weight: bold;" value="Maximum displayed: " />
                    <h:selectOneMenu id="listj_id_dataset" required="false" immediate="false" value="#{sessionHistory.numberOfResultsDatasetsString}" >                           
                        <a4j:support event="onchange" action="#{datasetBean.maxDisplay}" ajaxSingle="true" reRender="dataset, topgridds, bottomgridds" />            
                        
                        <f:selectItem itemLabel="5"  itemValue="5"/> 
                        <f:selectItem itemLabel="10"  itemValue="10"/> 
                        <f:selectItem itemLabel="20"  itemValue="20"/> 
                        <f:selectItem itemLabel="50"  itemValue="50"/> 
                        <f:selectItem itemLabel="100"  itemValue="100"/> 
                    </h:selectOneMenu>   
                </h:panelGroup>
                
                <h:panelGroup style="float: left"  rendered="#{fn:length(visit.visitData.currentDatasets) > 0 }">            
                    
                    <t:dataScroller id="scroll_1_datasets"
                                    for="dataset"
                                    rowsCountVar="rowsCount"
                                    displayedRowsCountVar="displayedRowsCountVar"
                                    firstRowIndexVar="firstRowIndex"
                                    lastRowIndexVar="lastRowIndex"
                                    pageCountVar="pageCount"
                                    immediate="true"
                                    pageIndexVar="pageIndex"
                                    >
                        <h:outputFormat value="{0} Datasets found, displaying {1}, from {2} to {3}. Page {4} / {5}" styleClass="standard" >
                            <f:param value="#{rowsCount}" />
                            <f:param value="#{displayedRowsCountVar}" />
                            <f:param value="#{firstRowIndex}" />
                            <f:param value="#{lastRowIndex}" />
                            <f:param value="#{pageIndex}" />
                            <f:param value="#{pageCount}" />
                        </h:outputFormat>
                    </t:dataScroller>
                </h:panelGroup>           
            </h:panelGrid>   
            
            
            <h:panelGrid  width="100%" border="0" columns="1" style="align: right" rendered="#{visit.visitData.datafileTableVisable}">
                
                
                <%-- <tr>   
                <td align="left">
                
                <h:commandButton disabled="#{!datasetTree.validSelection}" styleClass="button" id="addSelection" style="width: 130px" action="#{datasetTree.select}" title="Add to Data Links" value="Add to Data Links"/>
                &nbsp;
                <h:selectBooleanCheckbox disabled="true" style="background-color:#EAF4F4" title="select_investigation" >
                
                </h:selectBooleanCheckbox>
                
                
                
                &nbsp;
                <t:popup rendered="#{!visit.userPreferences.emailSet}"  styleClass="popup" style="font-size: 14px; cursor: default" closePopupOnExitingElement="true"
                closePopupOnExitingPopup="true"
                displayAtDistanceX="10"
                displayAtDistanceY="-40" >
                <t:graphicImage url="../../images/help_16.png" border="0" /> 
                <f:facet name="popup">
                <h:panelGroup>
                <h:panelGrid columns="1" >
                <h:outputText value="If you specify an email address, you can have the data downloaded link emailed to you."/>    
                <h:inputText styleClass="text" value="#{visit.userPreferences.email}" validator="#{userPreferencesBean.validateEmail}"  id="email">
                
                </h:inputText>
                
                <h:commandButton styleClass="button" action="#{datacenterBean.addEmail}" title="Add Email" value="Add Email"/>
                
                </h:panelGrid>
                </h:panelGroup>
                </f:facet>
                </t:popup>
                </td>
                
                </tr>--%>
                <h:panelGrid columns="3" style="float: right" >
                    <h:commandButton disabled="#{!visit.visitData.dataFilesDownloadable}" styleClass="button" id="downloadnow" style="width: 130px"  onclick="download('DOWNLOAD_MULTIPLE','DOWNLOAD_MULTIPLE','DATA_SETS'); return false;"  title="Download selections" value="Download selection"/>
                    <h:panelGroup/>
                    <h:graphicImage value="../../images/download.gif" width="19" height="14" />                                                             
                </h:panelGrid>
                
                <h:panelGrid columns="3" style="float: right" >
                    <h:commandButton disabled="#{!visit.visitData.dataFilesDownloadable}" styleClass="button" id="downloaddataset" style="width: 130px"  onclick="download('#{visit.visitData.currentDatasetId}','DATA_SET','DATA_SETS'); return false;"  title="Download All" value="Download All"/>
                    <h:panelGroup/>
                    <h:graphicImage value="../../images/download.gif" width="19" height="14" />                                                             
                </h:panelGrid>
                
                <%--<h:panelGroup style="float: right" >  
                <a4j:commandButton onclick="alert('Email has been sent to #{visit.userPreferences.email}');" immediate="true" reRender="messages" styleClass="button" id="downloademail" style="width: 130px" rendered="#{visit.userPreferences.emailSet}" action="#{datasetTree.emailDownload}" title="Download via email" value="Email download"/>
                &nbsp;
                <h:graphicImage rendered="#{visit.userPreferences.emailSet}" value="../../images/download.gif" width="19" height="14" />                                                             
                &nbsp;
                <h:selectBooleanCheckbox rendered="#{visit.userPreferences.emailSet}" disabled="true" style="background-color:#EAF4F4" title="select_investigation" >
                
                </h:selectBooleanCheckbox>
                </h:panelGroup>    --%>
                
                
            </h:panelGrid>
        </h:panelGrid>
    </a4j:form>
    </a4j:region> 