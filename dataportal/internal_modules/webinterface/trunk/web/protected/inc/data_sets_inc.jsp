<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/functions" prefix="fn" %>
<%@ taglib uri="https://ajax4jsf.dev.java.net/ajax" prefix="a4j"%>

<h:form>
    
    
    <table width="90%" border="0" >
        <tbody>
            <tr>
                <td width="30">&nbsp;</td>
                <td>
                <table border="0" width="100%">
                    <tbody>
                        <tr>
                            <tr> <h:messages globalOnly="true" errorClass="error" infoClass="info" />
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
                                                                            <h:outputText id="text78" styleClass="nodeFolder" value="Status: " />
                                                                            
                                                                            <h:outputText id="text728" styleClass="document" value="#{node.description}" />
                                                                            
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
                                                                            
                                                                            <h:outputText id="text7728" styleClass="document" value="#{node.description}" />
                                                                            
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
                                                                            
                                                                            <h:outputText id="text73ff8" styleClass="document" value="#{node.description}" />
                                                                            
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
                <a4j:region selfRendered="true"> 
                    <%--  //TODO  maybe later for this, for now, allow dow nload only from bookmarns
                   <a4j:poll id="poll" actionListener="#{datasetTree.checkReadAccess}" interval="1000" reRender="checkbox, poll,tree" ajaxSingle="true"  enabled="#{!datasetTree.accessDone}" />
                   --%>
                   
                    <%-- <h:commandLink rendered="true" value="Expand All" action="#{datasetTree.expandAll}"/>&nbsp;&nbsp;
                    <h:commandLink rendered="true" value="Collapse All" action="#{datasetTree.collapseAll}"/>--%>
                    <br />
                    <br />
                    <t:tree2 binding="#{datasetTree.tree}" rendered="#{datasetTree.clientSide}" id="tree" value="#{datasetTree.data}" var="node" varNodeToggler="t" showRootNode="false" clientSideToggle="true">
                        
                        
                        
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
                                <h:selectBooleanCheckbox style="background-color:#EAF4F4" title="select_investigation" valueChangeListener="#{datasetTree.setSelected}">
                                    <f:param name="datasets" value="#{node.identifier}"/>
                                </h:selectBooleanCheckbox>
                            </h:panelGroup>
                        </f:facet>
                        
                        <f:facet name="dataset-inFolder-folder">
                            <h:panelGroup>
                                
                                <t:graphicImage id="gr-1" value="../../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                                
                                <t:graphicImage id="gr-12" value="../../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                
                                <h:commandLink onclick="download('#{node.identifier}','DATA_SET','DATA_SETS'); return false;" style="color:black" id="downloadname-10" actionListener="#{datacenterBean.download}">
                                    <%--    style is used cos IE7 hover does not worj with IE7 :  styleClass="nodeFolderLink"--%>
                                    <h:outputText id="text-10" value="#{node.description}" style="font-family: Verdana, Geneva, sans-serif; font-size: 10px;" />
                                    
                                    <f:param name="id" value="#{node.identifier}"/>               
                                </h:commandLink>
                                <h:outputText value="    " />
                                <h:selectBooleanCheckbox style="background-color:#EAF4F4" title="select_investigation" valueChangeListener="#{datasetTree.setSelected}">
                                    <f:param name="datasets" value="#{node.identifier}"/>
                                </h:selectBooleanCheckbox>
                            </h:panelGroup>
                        </f:facet>
                        
                        <f:facet name="invest">
                            <h:panelGroup>
                                
                                <t:graphicImage id="gr91f9" value="../../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                                
                                <t:graphicImage id="gr9f18" value="../../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                
                                <h:outputText id="text9f10" value="#{node.description}" styleClass="nodeFolder"/>
                                <h:outputText value="    " />
                                <h:selectBooleanCheckbox  style="background-color:#EAF4F4" title="select_investigation" valueChangeListener="#{datasetTree.setSelected}">
                                    <f:param name="investigations" value="#{node.identifier}"/>
                                </h:selectBooleanCheckbox>
                            </h:panelGroup>
                        </f:facet>
                        
                        <f:facet name="foo1-folder">
                            <h:panelGroup>
                                
                                <t:graphicImage id="gr991" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                
                                <t:graphicImage id="gr918" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                
                                <h:outputText id="text190" value="#{node.description}" styleClass="nodeFolder"/>
                                <h:outputText id="text8ff91" value=" (#{node.childCount})" styleClass="childCount" rendered="#{!empty node.children}"/>
                                
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
                                <h:outputText id="text78" styleClass="nodeFolder" value="Status: " />
                                
                                <h:outputText id="text728" styleClass="document" value="#{node.description}" />
                                
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
                                
                                <h:outputText id="text7728" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <f:facet name="imageJ">
                            <h:panelGroup>
                                
                                <h:commandLink  onclick="download('#{node.identifier}','DATA_FILE_IMAGEJ','DATA_SETS'); return false;" style="color:black" id="view">
                                    <h:outputText value="Launch via ImageJ" style="font-family: Verdana, Geneva, sans-serif; font-size: 10px;" />
                                    
                                </h:commandLink>                           
                                
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <f:facet name="file-folder">
                            <h:panelGroup>
                                <f:facet name="expand6">
                                    <t:graphicImage id="gr60" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="collapse6">
                                    <t:graphicImage id="gr60u" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText value="         " />
                                <%-- <h:commandLink immediate="true" styleClass="#{t.nodeSelected ? 'documentSelected':'document'}" actionListener="#{datasetTree.setNodeSelected}">
                                <t:graphicImage value="../images/document.png" border="0"/>
                                <h:outputText value="#{node.description}" styleClass="file" />
                                <f:param name="id" value="#{node.identifier}"/>
                                </h:commandLink>      --%>
                             
                                <%--  <t:graphicImage value="../../images/document.png" border="0"/>  --%>     
                   
                                <t:graphicImage value="../../images/document.png" border="0"/>                                   
                                
                                <%--<h:commandLink rendered="#{node.leaf}" onclick="download('#{node.identifier}','FILE_IMAGEJ','DATA_SETS'); return false;" style="color:black" id="view">
                                <t:graphicImage value="../../images/document.png" border="0"/>                                   
                                </h:commandLink>--%>
                        
                                <h:commandLink onclick="download('#{node.identifier}','DATA_FILE','DATA_SETS'); return false;" style="color:black" id="downloadname12">
                                    <%--    style is used cos IE7 hover does not worj with IE7 :  styleClass="file"--%>
                                    <h:outputText value="#{node.description}" style="font-family: Verdana, Geneva, sans-serif; font-size: 10px;" />
                                    <f:param name="id16" value="#{node.identifier}"/>              
                                </h:commandLink>
                                <h:outputText value="         " />
                                <h:selectBooleanCheckbox style="background-color:#EAF4F4" title="Add to Data Center" valueChangeListener="#{datasetTree.setSelected}">
                                    <f:param name="datafiles" value="#{node.identifier}"/>
                                </h:selectBooleanCheckbox>
                                
                                <%--
                               //TOTO for now allow single download only from bookmarks etc
                               <h:selectBooleanCheckbox  id="checkbox" style="background-color:#D1E4E4" title="Add to download" valueChangeListener="#{datasetTree.setDataFileDownload}">
                                    <f:param name="datafiles" value="#{node.identifier}"/>
                                    <%--   <a4j:support  immediate="true" event="onclick" ajaxSingle="true"  actionListener="#{datasetTree.setDataFileDownloadAction}">
                                    <a4j:actionparam name="datafiles" value="#{node.identifier}" />
                                    </a4j:support>--%>
                                    
                                <%--</h:selectBooleanCheckbox>--%>
                                
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <f:facet name="file-noread-folder">
                            <h:panelGroup>                   
                                <t:graphicImage value="../../images/document.png" border="0"/>                                   
                                <h:outputText value="         " />
                                
                                <h:commandLink disabled="true"  style="color:black" id="downloadname1">
                                    <%--    style is used cos IE7 hover does not worj with IE7 :  styleClass="file"--%>
                                    <h:outputText value="#{node.description}" style="font-family: Verdana, Geneva, sans-serif; font-size: 10px;" />
                                    <f:param name="id1" value="#{node.identifier}"/>              
                                </h:commandLink>
                                <h:outputText value="         " />
                                <h:selectBooleanCheckbox style="background-color:#EAF4F4" title="Add to Data Center" valueChangeListener="#{datasetTree.setSelected}">
                                    <f:param name="datafiles" value="#{node.identifier}"/>
                                </h:selectBooleanCheckbox>
                                
                                <%--  <h:selectBooleanCheckbox  disabled="true" id="checkbox1" style="background-color:#D1E4E4" title="No access to data" >
                                    
                                </h:selectBooleanCheckbox>--%>
                               
                              
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
                                
                                <h:outputText id="text73ff8" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        
                    </t:tree2>
                    
                    <t:tree2 rendered="#{!datasetTree.clientSide}" id="tree2" value="#{datasetTree.data}" var="node" varNodeToggler="t" showRootNode="false" clientSideToggle="false">
                        
                        
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
                                <h:selectBooleanCheckbox style="background-color:#EAF4F4" title="select_investigation" valueChangeListener="#{datasetTree.setSelected}">
                                    <f:param name="datasets" value="#{node.identifier}"/>
                                </h:selectBooleanCheckbox>
                            </h:panelGroup>
                        </f:facet>
                        
                        <f:facet name="dataset-inFolder-folder">
                            <h:panelGroup>
                                
                                <t:graphicImage id="gr-1" value="../../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                                
                                <t:graphicImage id="gr-12" value="../../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                
                                <h:commandLink onclick="download('#{node.identifier}','DATA_SET','DATA_SETS'); return false;" style="color:black" id="downloadname-10" actionListener="#{datacenterBean.download}">
                                    <%--    style is used cos IE7 hover does not worj with IE7 :  styleClass="nodeFolderLink"--%>
                                    <h:outputText id="text-10" value="#{node.description}" style="font-family: Verdana, Geneva, sans-serif; font-size: 10px;" />
                                    
                                    <f:param name="id" value="#{node.identifier}"/>               
                                </h:commandLink>
                                <h:outputText value="    " />
                                <h:selectBooleanCheckbox style="background-color:#EAF4F4" title="select_investigation" valueChangeListener="#{datasetTree.setSelected}">
                                    <f:param name="datasets" value="#{node.identifier}"/>
                                </h:selectBooleanCheckbox>
                            </h:panelGroup>
                        </f:facet>
                        
                        <f:facet name="invest">
                            <h:panelGroup>
                                
                                <t:graphicImage id="gr91f9" value="../../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                                
                                <t:graphicImage id="gr9f18" value="../../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                
                                <h:outputText id="text9f10" value="#{node.description}" styleClass="nodeFolder"/>
                                <h:outputText value="    " />
                                <h:selectBooleanCheckbox  style="background-color:#EAF4F4" title="select_investigation" valueChangeListener="#{datasetTree.setSelected}">
                                    <f:param name="investigations" value="#{node.identifier}"/>
                                </h:selectBooleanCheckbox>
                            </h:panelGroup>
                        </f:facet>
                        
                        <f:facet name="foo1-folder">
                            <h:panelGroup>
                                
                                <t:graphicImage id="gr991" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                
                                <t:graphicImage id="gr918" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                
                                <h:outputText id="text190" value="#{node.description}" styleClass="nodeFolder"/>
                                <h:outputText id="text8ff91" value=" (#{node.childCount})" styleClass="childCount" rendered="#{!empty node.children}"/>
                                
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
                                <h:outputText id="text78" styleClass="nodeFolder" value="Status: " />
                                
                                <h:outputText id="text728" styleClass="document" value="#{node.description}" />
                                
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
                                
                                <h:outputText id="text7728" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <f:facet name="imageJ">
                            <h:panelGroup>
                                
                                <h:commandLink  onclick="download('#{node.identifier}','DATA_FILE_IMAGEJ','DATA_SETS'); return false;" style="color:black" id="view">
                                    <h:outputText value="Launch via ImageJ" style="font-family: Verdana, Geneva, sans-serif; font-size: 10px;" />
                                    
                                </h:commandLink>                           
                                
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <f:facet name="file-folder">
                            <h:panelGroup>
                                <f:facet name="expand6">
                                    <t:graphicImage id="gr60" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <f:facet name="collapse6">
                                    <t:graphicImage id="gr60u" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                </f:facet>
                                <h:outputText value="         " />
                                <%-- <h:commandLink immediate="true" styleClass="#{t.nodeSelected ? 'documentSelected':'document'}" actionListener="#{datasetTree.setNodeSelected}">
                                <t:graphicImage value="../images/document.png" border="0"/>
                                <h:outputText value="#{node.description}" styleClass="file" />
                                <f:param name="id" value="#{node.identifier}"/>
                                </h:commandLink>      --%>
                             
                                <%--  <t:graphicImage value="../../images/document.png" border="0"/>  --%>     
                   
                                <t:graphicImage value="../../images/document.png" border="0"/>                                   
                                
                                <%--<h:commandLink rendered="#{node.leaf}" onclick="download('#{node.identifier}','FILE_IMAGEJ','DATA_SETS'); return false;" style="color:black" id="view">
                                <t:graphicImage value="../../images/document.png" border="0"/>                                   
                                </h:commandLink>--%>
                        
                                <h:commandLink onclick="download('#{node.identifier}','DATA_FILE','DATA_SETS'); return false;" style="color:black" id="downloadname12">
                                    <%--    style is used cos IE7 hover does not worj with IE7 :  styleClass="file"--%>
                                    <h:outputText value="#{node.description}" style="font-family: Verdana, Geneva, sans-serif; font-size: 10px;" />
                                    <f:param name="id16" value="#{node.identifier}"/>              
                                </h:commandLink>
                                <h:outputText value="         " />
                                <h:selectBooleanCheckbox style="background-color:#EAF4F4" title="Add to Data Center" valueChangeListener="#{datasetTree.setSelected}">
                                    <f:param name="datafiles" value="#{node.identifier}"/>
                                </h:selectBooleanCheckbox>
                                
                                <%--
                               //TOTO for now allow single download only from bookmarks etc
                               <h:selectBooleanCheckbox  id="checkbox" style="background-color:#D1E4E4" title="Add to download" valueChangeListener="#{datasetTree.setDataFileDownload}">
                                    <f:param name="datafiles" value="#{node.identifier}"/>
                                    <%--   <a4j:support  immediate="true" event="onclick" ajaxSingle="true"  actionListener="#{datasetTree.setDataFileDownloadAction}">
                                    <a4j:actionparam name="datafiles" value="#{node.identifier}" />
                                    </a4j:support>--%>
                                    
                                <%--</h:selectBooleanCheckbox>--%>
                                
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        <f:facet name="file-noread-folder">
                            <h:panelGroup>                   
                                <t:graphicImage value="../../images/document.png" border="0"/>                                   
                                <h:outputText value="         " />
                                
                                <h:commandLink disabled="true"  style="color:black" id="downloadname1">
                                    <%--    style is used cos IE7 hover does not worj with IE7 :  styleClass="file"--%>
                                    <h:outputText value="#{node.description}" style="font-family: Verdana, Geneva, sans-serif; font-size: 10px;" />
                                    <f:param name="id1" value="#{node.identifier}"/>              
                                </h:commandLink>
                                <h:outputText value="         " />
                                <h:selectBooleanCheckbox style="background-color:#EAF4F4" title="Add to Data Center" valueChangeListener="#{datasetTree.setSelected}">
                                    <f:param name="datafiles" value="#{node.identifier}"/>
                                </h:selectBooleanCheckbox>
                                
                                <%--  <h:selectBooleanCheckbox  disabled="true" id="checkbox1" style="background-color:#D1E4E4" title="No access to data" >
                                    
                                </h:selectBooleanCheckbox>--%>
                               
                              
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
                                
                                <h:outputText id="text73ff8" styleClass="document" value="#{node.description}" />
                                
                            </h:panelGroup>
                        </f:facet>
                        
                        
                    </t:tree2>
                </a4j:region> 
                
                <br />
                <table width="90%" border="0">
                     <tbody>
                        <tr>   
                            <td align="left">
                                
                                <h:commandButton action="#{datasetTree.select}" title="Add selections" value="Add selections"/>
                                &nbsp;
                                <h:selectBooleanCheckbox disabled="true" style="background-color:#EAF4F4" title="select_investigation" >
                                    
                                </h:selectBooleanCheckbox>
                            </td>
                            <%-- <td align="left">
                                <a4j:commandButton oncomplete="download('#{node.identifier}','DOWNLOAD','DATA_SETS'); return false;" action="#{datasetTree.setValueChangeListeners}" title="Download selections" value="Download selections"/>
                               
                                <%-- <h:commandButton onclick="download('#{node.identifier}','DOWNLOAD','DATA_SETS'); return false;" action="#{datasetTree.select}" title="Download selections" value="Download selections"/>--%>
                            <%-- &nbsp;
                                <h:selectBooleanCheckbox disabled="true" style="background-color:#D1E4E4" title="select_investigation" >
                               
                                </h:selectBooleanCheckbox>
                            </td>--%>
                            <%--<td align="right">
                            Select:&nbsp; <h:commandButton action="#{datasetTree.selectall}" title="All" value=" All "/>
                            &nbsp; 
                            <h:commandButton action="#{datasetTree.selectnone}" title="None" value="None"/>
                            </td>--%>
                        </tr>
                    </tbody>      
                </table>
            </tr>
        </tbody>      
        
    </table>                        
</h:form>