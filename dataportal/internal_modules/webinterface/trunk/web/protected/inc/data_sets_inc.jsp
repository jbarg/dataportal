<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/functions" prefix="fn" %>
<%@ taglib uri="https://ajax4jsf.dev.java.net/ajax" prefix="a4j"%>

<h:form>
      <f:loadBundle basename="uk.ac.dl.dp.web.messages.facility" var="facility_properties" />
  
    
    <table width="90%" border="0" >
        <tbody>
            <tr>
                <td width="30">&nbsp;</td>
                <td>
                <a4j:region  selfRendered="true"> 
                    <table border="0" width="100%">
                        <tbody>
                            <tr>
                            <tr> <h:messages id="messages" globalOnly="true" warnClass="error" errorClass="error" infoClass="info" />
                                </tr>     
                                <td width="60%">   &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                </td>    
                                
                                <td align="left">&nbsp;&nbsp;&nbsp;
                                    <t:popup styleClass="popup" closePopupOnExitingElement="true"
                                             closePopupOnExitingPopup="true"
                                             displayAtDistanceX="5"
                                             displayAtDistanceY="-40" >
                                        
                                        <t:graphicImage url="../../images/help.gif" border="0" />
                                        &nbsp;&nbsp;&nbsp;
                                        <f:facet name="popup">
                                            <h:panelGroup>
                                                <h:panelGrid columns="1" >
                                                    <%-- <f:subview id="help">
                                                <c:import url="/data_sets_inc_help.jsp" />
                                                </f:subview>--%>
                                                    <table width="300" border="0">
                                                        <tbody>
                                                            <tr>
                                                                <td>
                                                                    <t:tree2 id="treehelp" value="#{datasetHelpTree.data}" var="node" varNodeToggler="t" showRootNode="false">
                                                                        
                                                                        <f:facet name="foo-folder">
                                                                            <h:panelGroup>
                                                                                
                                                                                <t:graphicImage id="gr99" value="../../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                                                                                
                                                                                <t:graphicImage id="gr98" value="../../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                                                                
                                                                                <h:outputText id="text90" value="#{node.description}" styleClass="nodeFolder"/>
                                                                                <h:outputText id="text891" value=" (no. of data files)" styleClass="childCount" rendered="#{!empty node.children}"/>
                                                                                <h:selectBooleanCheckbox title="select_investigation" >
                                                                                    <f:param name="datasets" value="#{node.identifier}"/>
                                                                                </h:selectBooleanCheckbox>
                                                                            </h:panelGroup>
                                                                        </f:facet>
                                                                        <f:facet name="invest">
                                                                            <h:panelGroup>
                                                                                
                                                                                <t:graphicImage id="gr91f9" value="../../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                                                                                
                                                                                <t:graphicImage id="gr9f18" value="../../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                                                                
                                                                                <h:outputText id="text9f10" value="#{node.description}" styleClass="nodeFolder"/>
                                                                                <h:selectBooleanCheckbox title="select_investigation" >
                                                                                    <f:param name="investigations" value="#{node.identifier}"/>
                                                                                </h:selectBooleanCheckbox>
                                                                            </h:panelGroup>
                                                                        </f:facet>
                                                                        
                                                                        <f:facet name="foo1-folder">
                                                                            <h:panelGroup>
                                                                                
                                                                                <t:graphicImage id="gr991" value="../../images/blue-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                                                                                
                                                                                <t:graphicImage id="gr918" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                                                                
                                                                                <h:outputText id="text190" value="#{node.description}" styleClass="nodeFolder"/>
                                                                                <h:outputText id="text8ff91" value=" (no. of data sets)" styleClass="childCount" rendered="#{!empty node.children}"/>
                                                                                
                                                                            </h:panelGroup>
                                                                        </f:facet>
                                                                        <f:facet name="bar-folder">
                                                                            <h:panelGroup>
                                                                                <f:facet name="expand3">
                                                                                    <t:graphicImage id="gr" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                                                                </f:facet>
                                                                                <f:facet name="collapse3">
                                                                                    <t:graphicImage id="gr1" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                                                                </f:facet>
                                                                                <h:outputText id="text786" styleClass="document" value="#{node.description}" />
                                                                                
                                                                            </h:panelGroup>
                                                                        </f:facet>
                                                                        
                                                                        <f:facet name="status-folder">
                                                                            <h:panelGroup>
                                                                                <f:facet name="expand1">
                                                                                    <t:graphicImage id="gr17872" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                                                                </f:facet>
                                                                                <f:facet name="collapse1">
                                                                                    <t:graphicImage id="gr112" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                                                                </f:facet>
                                                                                <h:outputText id="text7c8" styleClass="nodeFolder" value="Status: " />
                                                                                
                                                                                <h:outputText id="texft728" styleClass="document" value="#{node.description}" />
                                                                                
                                                                            </h:panelGroup>
                                                                        </f:facet>
                                                                        
                                                                        
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
                                                                        
                                                                        <f:facet name="file-folder">
                                                                            <h:panelGroup>
                                                                                <f:facet name="expand163">
                                                                                    <t:graphicImage id="gr163" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                                                                </f:facet>
                                                                                <f:facet name="collapse163">
                                                                                    <t:graphicImage id="gr1613" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                                                                </f:facet>
                                                                                &nbsp;  
                                                                                <h:commandLink disabled="true" immediate="true" styleClass="#{t.nodeSelected ? 'documentSelected':'document'}" >
                                                                                    <t:graphicImage value="../../images/document.png" border="0"/>
                                                                                    <h:outputText value="#{node.description}" styleClass="file" />
                                                                                    <f:param name="id" value="#{node.identifier}"/>
                                                                                </h:commandLink>      
                                                                                <h:selectBooleanCheckbox title="select_investigation" >
                                                                                    <f:param name="datafiles" value="#{node.identifier}"/>
                                                                                </h:selectBooleanCheckbox>
                                                                            </h:panelGroup>
                                                                        </f:facet>
                                                                        
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
                                                                        
                                                                        <f:facet name="desc-folder">
                                                                            <h:panelGroup>
                                                                                <f:facet name="expand1ff3">
                                                                                    <t:graphicImage id="grff13" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                                                                </f:facet>
                                                                                <f:facet name="collapse1ff3">
                                                                                    <t:graphicImage id="gr1ff13" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                                                                </f:facet>
                                                                                <h:outputText id="text7ff38" styleClass="nodeFolder" value="Description: " />
                                                                                
                                                                                <h:outputText id="text73ff8" styleClass="document" value="#{fn:replace(node.description, '~', 'approx')}" />
                                                                                
                                                                            </h:panelGroup>
                                                                        </f:facet>
                                                                        
                                                                        
                                                                    </t:tree2>
                                                                </td>
                                                            </tr>
                                                        </tbody>      
                                                    </table>
                                                </h:panelGrid>
                                            </h:panelGroup>
                                        </f:facet>
                                    </t:popup>
                                </td>
                            </tr>
                        </tbody>      
                        <%--  <td> <h:commandLink value="Expand All" styleClass="nodeFolder" action="#{datasetTree.expandAll}"/></td>--%>
                    </table>
                    
                    <%--  //TODO  maybe later for this, for now, allow dow nload only from bookmarns
                   <a4j:poll id="poll" actionListener="#{datasetTree.checkReadAccess}" interval="1000" reRender="checkbox, poll,tree" ajaxSingle="true"  enabled="#{!datasetTree.accessDone}" />
                   --%>
                   
                    <%-- <h:commandLink rendered="true" value="Expand All" action="#{datasetTree.expandAll}"/>&nbsp;&nbsp;
                    <h:commandLink rendered="true" value="Collapse All" action="#{datasetTree.collapseAll}"/>--%>
                    <br />
                    <br />
                    <t:tree2 binding="#{datasetTree.tree}" rendered="#{datasetTree.clientSide}" id="tree" value="#{datasetTree.data}" var="node" varNodeToggler="t" showRootNode="false" clientSideToggle="true">
                        
                        
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
                                
                                <h:outputText id="text891" value=" (#{node.childCount-3})" styleClass="childCount" rendered="#{!empty node.children}"/>
                                <h:outputText value="    " />
                                 
                                <h:graphicImage value="../../images/download.gif" width="19" height="14" />                                                             
                               
                                 <h:selectBooleanCheckbox style="background-color:#EAF4F4" title="Select Datasets">   
                                    <f:param name="datasets" value="#{node.identifier}"/>
                                    <a4j:support reRender="addSelection, downloademail, downloadnow" event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{datasetTree.setSelectedAjax}">
                                        <a4j:actionparam name="datasets" value="#{node.identifier}"/>   
                                    </a4j:support>
                                </h:selectBooleanCheckbox>
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  Data set no read branch (ie, no datafiles or no datafile which can read) ----%>
                        <f:facet name="dataset-noread-folder">
                            <h:panelGroup>
                               
                                    <t:graphicImage id="gr99noread" value="../../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                             
                                    <t:graphicImage id="gr98noread" value="../../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                               <h:outputText value=" " />
                                <%--    style is used cos IE7 hover does not worj with IE7 :  styleClass="nodeFolderLink"--%>
                                <h:outputText id="text90noread" value="#{node.description}" style="font-family: Verdana, Geneva, sans-serif; font-size: 10px;" />
                                
                                <h:outputText id="text891noread" value=" (#{node.childCount-3})" styleClass="childCount" rendered="#{!empty node.children}"/>
                                <h:outputText value="    " />
                                
                                <h:selectBooleanCheckbox style="background-color:#EAF4F4" title="Select Datasets">   
                                    <f:param name="datasets" value="#{node.identifier}"/>
                                    <a4j:support reRender="addSelection, downloademail" event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{datasetTree.setSelectedAjax}">
                                        <a4j:actionparam name="datasets" value="#{node.identifier}"/>   
                                    </a4j:support>
                                </h:selectBooleanCheckbox>
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
                                <h:selectBooleanCheckbox  style="background-color:#EAF4F4" title="Select Investigation" >
                                    <f:param name="investigations" value="#{node.identifier}"/>
                                    <a4j:support reRender="addSelection, downloademail, downloadnow" event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{datasetTree.setSelectedAjax}">
                                        <a4j:actionparam name="investigations" value="#{node.identifier}"/>   
                                    </a4j:support>
                                </h:selectBooleanCheckbox>
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----   Datasets, list number of datasets ----%>
                        <f:facet name="dataset-count-folder">
                            <h:panelGroup>
                                
                                <t:graphicImage id="gr9d91" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                
                                <t:graphicImage id="grd918" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                
                                <h:outputText id="textd190" value="#{node.description}" styleClass="nodeFolder"/>
                                <h:outputText id="texdt8ff91" value=" (#{node.childCount})" styleClass="childCount" rendered="#{!empty node.children}"/>
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----   Datafile, list number of datafile ----%>
                        <f:facet name="datafile-count-folder">
                            <h:panelGroup>
                                
                                <t:graphicImage id="gr991" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                
                                <t:graphicImage id="gr918" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                
                                <h:outputText id="text190" value="#{node.description}" styleClass="nodeFolder"/>
                                <h:outputText id="text8ff91" value=" (#{node.childCount})" styleClass="childCount" rendered="#{!empty node.children}"/>
                                
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
                        
                        <%----  Datafile size  ----%>
                        <f:facet name="size-folder">
                            <h:panelGroup>
                                <f:facet name="expandfg1">
                                    <t:graphicImage id="gr1fgfgg72" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="collapsfge1">
                                    <t:graphicImage id="grg112" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="tessxt78" styleClass="nodeFolder" value="Size : " />
                                
                                <h:outputText id="textf728" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%---- Datafile format   ----%>
                        <f:facet name="format-folder">
                            <h:panelGroup>
                                <f:facet name="expda1">
                                    <t:graphicImage id="gr1ddfgg72" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="collsapsge1">
                                    <t:graphicImage id="grgdd2" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="tedddxt8" styleClass="nodeFolder" value="Format: " />
                                
                                <h:outputText id="textfgfg28" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  Datafile format version   ----%>
                        <f:facet name="format-version-folder">
                            <h:panelGroup>
                                <f:facet name="expandf3gd1">
                                    <t:graphicImage id="gr1fgd3ffgg72" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="collaps3dfge1">
                                    <t:graphicImage id="grgfddd1312" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="textfddd4578" styleClass="nodeFolder" value="Format version: " />
                                
                                <h:outputText id="te3xt7ffff28" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  Datafile format type   ----%>
                        <f:facet name="format-type-folder">
                            <h:panelGroup>
                                <f:facet name="exnfgd1">
                                    <t:graphicImage id="gr1fg72" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="clasdfge1">
                                    <t:graphicImage id="grf112" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="te78" styleClass="nodeFolder" value="Format type: " />
                                
                                <h:outputText id="tex28" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  Datafile create time   ----%>
                        <f:facet name="createTime-folder">
                            <h:panelGroup>
                                <f:facet name="expand1">
                                    <t:graphicImage id="gr1fgfg72" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="collafge1">
                                    <t:graphicImage id="grgf2" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="texf78" styleClass="nodeFolder" value="Create Time: " />
                                
                                <h:outputText id="tex7f28" styleClass="document" value="#{node.description}" />
                                
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
                        
                        <%----  datafile imageJ  ----%>
                        <f:facet name="imageJ">
                            <h:panelGroup>                                
                                <h:commandLink  onclick="download('#{node.identifier}','DATA_FILE_IMAGEJ','DATA_SETS'); return false;" style="color:black" id="view">
                                    <h:outputText value="Launch via ImageJ" style="font-family: Verdana, Geneva, sans-serif; font-size: 10px;" />                                    
                                </h:commandLink>      
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
                                                                
                                <h:selectBooleanCheckbox  style="background-color:#EAF4F4" title="Select DataFile" >
                                    <f:param name="datafiles" value="#{node.identifier}"/>
                                    <a4j:support reRender="addSelection, downloademail, downloadnow" event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{datasetTree.setSelectedAjax}">
                                        <a4j:actionparam name="datafiles" value="#{node.identifier}"/>   
                                    </a4j:support>
                                </h:selectBooleanCheckbox>
                                
                                
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
                                
                                <h:selectBooleanCheckbox style="background-color:#EAF4F4" title="Select Datasets">   
                                    <f:param name="datafiles" value="#{node.identifier}"/>
                                    <a4j:support reRender="addSelection, downloademail, downloadnow" event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{datasetTree.setSelectedAjax}">
                                        <a4j:actionparam name="datafiles" value="#{node.identifier}"/>   
                                    </a4j:support>
                                </h:selectBooleanCheckbox>                                                                                      
                                
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
                                
                                <h:selectBooleanCheckbox  style="background-color:#EAF4F4" title="Select DataFile" >
                                    <f:param name="datafiles" value="#{node.identifier}"/>
                                    <a4j:support reRender="addSelection, downloademail, downloadnow" event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{datasetTree.setSelectedAjax}">
                                        <a4j:actionparam name="datafiles" value="#{node.identifier}"/>   
                                    </a4j:support>
                                </h:selectBooleanCheckbox>
                                
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
                    
                    <t:tree2 rendered="#{!datasetTree.clientSide}" id="tree2" value="#{datasetTree.data}" var="node" varNodeToggler="t" showRootNode="false" clientSideToggle="false">
                        
                        
                           <%----  Data set branch  ----%>
                        <f:facet name="dataset-folder">
                            <h:panelGroup>
                                <f:facet name="exdfpand">
                                    <t:graphicImage id="grdfrd99" value="../../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="coldfdlapse">
                                    <t:graphicImage id="gdfdfr98" value="../../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:commandLink onclick="download('#{node.identifier}','DATA_SET','DATA_SETS'); return false;" style="color:black" id="downloadname" actionListener="#{datacenterBean.download}">
                                    <%--    style is used cos IE7 hover does not worj with IE7 :  styleClass="nodeFolderLink"--%>
                                    <h:outputText id="textcvcvc90" value="#{node.description}" style="font-family: Verdana, Geneva, sans-serif; font-size: 10px;" />
                                    
                                    <f:param name="id" value="#{node.identifier}"/>             
                                </h:commandLink>
                                
                                <h:outputText id="textfdfff891" value=" (#{node.childCount-4})" styleClass="childCount" rendered="#{!empty node.children}"/>
                                <h:outputText value="    " />
                                 
                                <h:graphicImage value="../../images/download.gif" width="19" height="14" />                                                             
                               
                                 <h:selectBooleanCheckbox style="background-color:#EAF4F4" title="Select Datasets">   
                                    <f:param name="datasets" value="#{node.identifier}"/>
                                    <a4j:support reRender="addSelection, downloademail, downloadnow" event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{datasetTree.setSelectedAjax}">
                                        <a4j:actionparam name="datasets" value="#{node.identifier}"/>   
                                    </a4j:support>
                                </h:selectBooleanCheckbox>
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  Data set no read branch (ie, no datafiles or no datafile which can read) ----%>
                        <f:facet name="dataset-noread-folder">
                            <h:panelGroup>
                               
                                    <t:graphicImage id="gr99n23232oread" value="../../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                             
                                    <t:graphicImage id="gr232398noread" value="../../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                               <h:outputText value=" " />
                                <%--    style is used cos IE7 hover does not worj with IE7 :  styleClass="nodeFolderLink"--%>
                                <h:outputText id="text923230noread" value="#{node.description}" style="font-family: Verdana, Geneva, sans-serif; font-size: 10px;" />
                                
                                <h:outputText id="text8232391noread" value=" (#{node.childCount-3})" styleClass="childCount" rendered="#{!empty node.children}"/>
                                <h:outputText value="    " />
                                
                                <h:selectBooleanCheckbox style="background-color:#EAF4F4" title="Select Datasets">   
                                    <f:param name="datasets" value="#{node.identifier}"/>
                                    <a4j:support reRender="addSelection, downloademail" event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{datasetTree.setSelectedAjax}">
                                        <a4j:actionparam name="datasets" value="#{node.identifier}"/>   
                                    </a4j:support>
                                </h:selectBooleanCheckbox>
                            </h:panelGroup>
                        </f:facet>
                        
                        
                        <%----   Investigation branch ----%>
                        <f:facet name="invest">
                            <h:panelGroup>
                                
                                <t:graphicImage id="gr9231f9" value="../../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                                
                                <t:graphicImage id="gr239f18" value="../../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                
                                <h:outputText id="te232xt9f10" value="#{node.description}" styleClass="nodeFolder"/>
                                <h:outputText value="    " />
                                <%-- <h:selectBooleanCheckbox  style="background-color:#EAF4F4" title="select_investigation" valueChangeListener="#{datasetTree.setSelected}">
                                    <f:param name="investigations" value="#{node.identifier}"/>
                                </h:selectBooleanCheckbox>--%>
                                <h:selectBooleanCheckbox  style="background-color:#EAF4F4" title="Select Investigation" >
                                    <f:param name="investigations" value="#{node.identifier}"/>
                                    <a4j:support reRender="addSelection, downloademail, downloadnow" event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{datasetTree.setSelectedAjax}">
                                        <a4j:actionparam name="investigations" value="#{node.identifier}"/>   
                                    </a4j:support>
                                </h:selectBooleanCheckbox>
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----   Datasets, list number of datasets ----%>
                        <f:facet name="dataset-count-folder">
                            <h:panelGroup>
                                
                                <t:graphicImage id="gr9dsd91" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                
                                <t:graphicImage id="grd9sd18" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                
                                <h:outputText id="textd190" value="#{node.description}" styleClass="nodeFolder"/>
                                <h:outputText id="texdfdt8ff91" value=" (#{node.childCount})" styleClass="childCount" rendered="#{!empty node.children}"/>
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----   Datafile, list number of datafile ----%>
                        <f:facet name="datafile-count-folder">
                            <h:panelGroup>
                                
                                <t:graphicImage id="grdfd991" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                
                                <t:graphicImage id="fdf" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                
                                <h:outputText id="texjkt190" value="#{node.description}" styleClass="nodeFolder"/>
                                <h:outputText id="text8kjkff91" value=" (#{node.childCount})" styleClass="childCount" rendered="#{!empty node.children}"/>
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----   Dataset status  ----%>
                        <f:facet name="status-folder">
                            <h:panelGroup>
                                <f:facet name="expanrtd1">
                                    <t:graphicImage id="gr1787df2" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="colldfapse1">
                                    <t:graphicImage id="gr1vb12" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="texbvdsst78" styleClass="nodeFolder" value="Status: " />
                                
                                <h:outputText id="texsdht728" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  Dataset type  ----%>
                        <f:facet name="type-folder">
                            <h:panelGroup>
                                <f:facet name="expsdand17">
                                    <t:graphicImage id="grsd1872" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="collapsdse71">
                                    <t:graphicImage id="gr1d712" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="texnmst778" styleClass="nodeFolder" value="Type: " />
                                
                                <h:outputText id="text7nm728" styleClass="document" value="#{fn:toLowerCase(node.description)}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  Datafile size  ----%>
                        <f:facet name="size-folder">
                            <h:panelGroup>
                                <f:facet name="expanbvndfg1">
                                    <t:graphicImage id="gr1fsdgfgg72" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="collapssdfge1">
                                    <t:graphicImage id="grsdg112" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="tessdsxt78" styleClass="nodeFolder" value="Size : " />
                                
                                <h:outputText id="tesdsxtf728" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%---- Datafile format   ----%>
                        <f:facet name="format-folder">
                            <h:panelGroup>
                                <f:facet name="esdxpda1">
                                    <t:graphicImage id="gr1dddfgg72" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="collsadpsge1">
                                    <t:graphicImage id="grgdfdd2" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="tedfdddxt8" styleClass="nodeFolder" value="Format: " />
                                
                                <h:outputText id="tevxtfgfg28" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  Datafile format version   ----%>
                        <f:facet name="format-version-folder">
                            <h:panelGroup>
                                <f:facet name="expanvbdf3gd1">
                                    <t:graphicImage id="gr1fbvcgd3ffgg72" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="collapvbs3dfge1">
                                    <t:graphicImage id="grgfdfddd1312" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="textsfddd4578" styleClass="nodeFolder" value="Format version: " />
                                
                                <h:outputText id="te3xst7ffff28" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  Datafile format type   ----%>
                        <f:facet name="format-type-folder">
                            <h:panelGroup>
                                <f:facet name="exnfsgd1">
                                    <t:graphicImage id="gr1ssfg72" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="classdfge1">
                                    <t:graphicImage id="grfd112" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="ted78" styleClass="nodeFolder" value="Format type: " />
                                
                                <h:outputText id="texsd28" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  Datafile create time   ----%>
                        <f:facet name="createTime-folder">
                            <h:panelGroup>
                                <f:facet name="expfgand1">
                                    <t:graphicImage id="gr1fgfgfg72" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="collfgafge1">
                                    <t:graphicImage id="grfggf2" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="tewdffgxf78" styleClass="nodeFolder" value="Create Time: " />
                                
                                <h:outputText id="texew7f28" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  Investigation  instrument   ----%>
                        <f:facet name="instrument-folder">
                            <h:panelGroup>
                                <f:facet name="expa4nwewegd1">
                                    <t:graphicImage id="dwewe" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="colla4ffgwewee1">
                                    <t:graphicImage id="grddddgf2" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="te44xweeef7s8" styleClass="nodeFolder" value="#{facility_properties['instrument.name']}: " />
                                
                                <h:outputText id="te4xswewewe7f28" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                         <%----  Investigation  visit id   ----%>
                        <f:facet name="visitId-folder">
                            <h:panelGroup>
                                <f:facet name="expa4sd1">
                                    <t:graphicImage id="dddfg34d" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="colddfd1">
                                    <t:graphicImage id="grd44232" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="te4448" styleClass="nodeFolder" value="Visit Id: " />
                                
                                <h:outputText id="t43447f28" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  datafile imageJ  ----%>
                        <f:facet name="imageJ">
                            <h:panelGroup>                                
                                <h:commandLink  onclick="download('#{node.identifier}','DATA_FILE_IMAGEJ','DATA_SETS'); return false;" style="color:black" id="view">
                                    <h:outputText value="Launch via ImageJ" style="font-family: Verdana, Geneva, sans-serif; font-size: 10px;" />                                    
                                </h:commandLink>      
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  datafile  ----%>
                        <f:facet name="file-folder">
                            <h:panelGroup>
                                <f:facet name="expanggd6">
                                    <t:graphicImage id="grfgfg60" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="collaggpse6">
                                    <t:graphicImage id="grffg60u" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText value="         " />
                                
                                <t:graphicImage value="../../images/document.png" border="0"   />                                   
                                 <h:outputText value=" " />
                                <h:commandLink onclick="download('#{node.identifier}','DATA_FILE','DATA_SETS'); return false;" style="color:black" id="downloadname112">
                                    <%--    style is used cos IE7 hover does not worj with IE7 :  styleClass="file"--%>
                                    <h:outputText value="#{node.description}" style="font-family: Verdana, Geneva, sans-serif; font-size: 10px;" />
                                    <f:param name="id16" value="#{node.identifier}"/>              
                                </h:commandLink>
                                <h:outputText value="         " />
                                
                                <h:graphicImage value="../../images/download.gif" width="19" height="14" />                                                             
                                                                
                                <h:selectBooleanCheckbox  style="background-color:#EAF4F4" title="Select DataFile" >
                                    <f:param name="datafiles" value="#{node.identifier}"/>
                                    <a4j:support reRender="addSelection, downloademail, downloadnow" event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{datasetTree.setSelectedAjax}">
                                        <a4j:actionparam name="datafiles" value="#{node.identifier}"/>   
                                    </a4j:support>
                                </h:selectBooleanCheckbox>
                                
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  datafile (EMAT) in folder (show folder instead of file image  ----%>
                        <f:facet name="file-inFolder-folder">
                            <h:panelGroup>
                                
                                <t:graphicImage id="gr6rt0u1" value="../../images/yellow-folder-closed.png" border="0"/>
                                
                                <h:outputText value="         " />                                         
                                
                                <h:commandLink onclick="download('#{node.identifier}','DATA_FILE_IN_FOLDERS','DATA_SETS'); return false;" style="color:black" id="downloadname1182">
                                    <%--    style is used cos IE7 hover does not worj with IE7 :  styleClass="file"--%>
                                    <h:outputText value="#{node.description}" style="font-family: Verdana, Geneva, sans-serif; font-size: 10px;" />
                                    <f:param name="id2216" value="#{node.identifier}"/>              
                                </h:commandLink>
                                <h:outputText value="         " />
                                
                                <h:graphicImage value="../../images/download.gif" width="19" height="14" />                                                             
                                
                                <h:selectBooleanCheckbox style="background-color:#EAF4F4" title="Select Datasets">   
                                    <f:param name="datafiles" value="#{node.identifier}"/>
                                    <a4j:support reRender="addSelection, downloademail, downloadnow" event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{datasetTree.setSelectedAjax}">
                                        <a4j:actionparam name="datafiles" value="#{node.identifier}"/>   
                                    </a4j:support>
                                </h:selectBooleanCheckbox>                                                                                      
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  datafile no download  ----%>
                        <f:facet name="file-noread-folder">
                            <h:panelGroup>                   
                                <t:graphicImage value="../../images/document.png" border="0"/>                                   
                                <h:outputText value="         " />
                                
                                <%--    style is used cos IE7 hover does not worj with IE7 :  styleClass="file"--%>
                                <h:outputText value="#{node.description}" style="font-family: Verdana, Geneva, sans-serif; font-size: 10px;" />
                                
                                <h:outputText value="         " />
                                
                                <h:selectBooleanCheckbox  style="background-color:#EAF4F4" title="Select DataFile" >
                                    <f:param name="datafiles" value="#{node.identifier}"/>
                                    <a4j:support reRender="addSelection, downloademail, downloadnow" event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{datasetTree.setSelectedAjax}">
                                        <a4j:actionparam name="datafiles" value="#{node.identifier}"/>   
                                    </a4j:support>
                                </h:selectBooleanCheckbox>
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%---- Investigation facility  ----%>
                        <f:facet name="fac-folder">
                            <h:panelGroup>
                                <f:facet name="ex22pand1f3">
                                    <t:graphicImage id="grf2213" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="co22llapse1f3">
                                    <t:graphicImage id="gr1222f13" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="te2xt23" styleClass="nodeFolder" value="Facility: " />
                                
                                <h:outputText id="te2xt34" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <%----  dataset description  ----%>
                        <f:facet name="desc-folder">
                            <h:panelGroup>
                                <f:facet name="e22xpand1ff3">
                                    <t:graphicImage id="gr2" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="c22ollapse1ff3">
                                    <t:graphicImage id="gr1" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText id="text344" styleClass="nodeFolder" value="Description: " />
                                
                                <h:outputText id="text12" escape="false" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        
                    </t:tree2>
                    
                    
                    <br />
                    <table width="90%" border="0">
                         <tbody>
                            <tr>   
                                <td align="left">
                                    <%--<h:commandButton styleClass="button" id="addSelection" disabled="#{!visit.visitData.addSelectionable}" action="#{datasetTree.select}" title="Add selections" value="Add selections"/>--%>
                                   
                                    <h:commandButton disabled="#{!datasetTree.validSelection}" styleClass="button" id="addSelection" style="width: 130px" action="#{datasetTree.select}" title="Add to Data Links" value="Add to Data Links"/>
                                    &nbsp;
                                    <h:selectBooleanCheckbox disabled="true" style="background-color:#EAF4F4" title="select_investigation" >
                                        
                                    </h:selectBooleanCheckbox>
                                    
                                    
                                    &nbsp;
                                    <t:popup rendered="#{!visit.userPreferences.emailSet}"  styleClass="popup" style="font-size: 14px; cursor: default" closePopupOnExitingElement="true"
                                             closePopupOnExitingPopup="true"
                                             displayAtDistanceX="10"
                                             displayAtDistanceY="-40" >
                                        <t:graphicImage url="../../images/help.gif" border="0" /> 
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
                            </tr>
                            <tr>
                                <td align="left">
                                    <h:commandButton disabled="#{!datasetTree.validDownloadSelection}" styleClass="button" id="downloadnow" style="width: 130px"  onclick="download('DOWNLOAD_MULTIPLE','DOWNLOAD_MULTIPLE','DATA_SETS'); return false;"  title="Download selections now" value="Download now"/>
                                    &nbsp;
                                      <h:graphicImage value="../../images/download.gif" width="19" height="14" />                                                             
                             
                                    <h:selectBooleanCheckbox  disabled="true" style="background-color:#EAF4F4" title="select_investigation" >
                                        
                                    </h:selectBooleanCheckbox>
                                </td>                                
                                
                            </tr>
                            <tr>
                                <td align="left">
                                    
                                    <a4j:commandButton onclick="alert('Email has been sent to #{visit.userPreferences.email}');" immediate="true" reRender="messages" styleClass="button" id="downloademail" style="width: 130px" disabled="#{!datasetTree.validDownloadSelection}" rendered="#{visit.userPreferences.emailSet}" action="#{datasetTree.emailDownload}" title="Download via email" value="Email download"/>
                                    &nbsp;
                                      <h:graphicImage rendered="#{visit.userPreferences.emailSet}" value="../../images/download.gif" width="19" height="14" />                                                             
                             
                                    <h:selectBooleanCheckbox rendered="#{visit.userPreferences.emailSet}" disabled="true" style="background-color:#EAF4F4" title="select_investigation" >
                                        
                                    </h:selectBooleanCheckbox>
                                    
                                    
                                </td>
                            </tr>
                            
                        </tbody>      
                    </table>
                </a4j:region> 
            </tr>
        </tbody>      
        
    </table>                        
</h:form>