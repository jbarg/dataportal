<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="c"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/functions" prefix="fn" %>


<h:form>
 


    <table border="0" >
        <td width="30">&nbsp;</td>
        <td>
        <table  border="0" >
            <tr> <h:messages globalOnly="true" errorClass="error" infoClass="info" />
            </tr>     
            <td>   <h:outputText id="text321" value="Investigations" styleClass="nodeFolder"/>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
            </td>           
            <td align="left">&nbsp;&nbsp;&nbsp;
                <t:popup styleClass="popup" closePopupOnExitingElement="true"
                    closePopupOnExitingPopup="true"
                    displayAtDistanceX="20"
                    displayAtDistanceY="-40" >

                    <h:outputText styleClass="nodeFolder" value="Help"/>&nbsp;&nbsp;&nbsp;
                    <f:facet name="popup">
                        <h:panelGroup>
                            <h:panelGrid columns="1" >
                                <h:outputText value="This is going to show help tree diagram."/>
                        
                            </h:panelGrid>
                        </h:panelGroup>
                    </f:facet>
                </t:popup>
            </td>
            <%--  <td> <h:commandLink value="Expand All" styleClass="nodeFolder" action="#{datasetTree.expandAll}"/></td>--%>
            <td height="10"><br />&nbsp;</td>
        </table>
        <t:tree2 id="tree" value="#{datasetTree.data}" var="node" varNodeToggler="t" showRootNode="false">
                
            <f:facet name="foo-folder">
                <h:panelGroup>
                    <f:facet name="expand">
                        <t:graphicImage id="gr99" value="../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                    </f:facet>
                    <f:facet name="collapse">
                        <t:graphicImage id="gr98" value="../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                    </f:facet>
                    <h:outputText id="text90" value="#{node.description}" styleClass="nodeFolder"/>
                    <h:outputText id="text891" value=" (#{node.childCount-3})" styleClass="childCount" rendered="#{!empty node.children}"/>
                    <h:selectBooleanCheckbox title="select_investigation" valueChangeListener="#{datasetTree.setSelected}">
                       <f:param name="datasets" value="#{node.identifier}"/>
                    </h:selectBooleanCheckbox>
                </h:panelGroup>
            </f:facet>
            <f:facet name="invest">
                <h:panelGroup>
                       
                    <t:graphicImage id="gr91f9" value="../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                     
                    <t:graphicImage id="gr9f18" value="../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                     
                    <h:outputText id="text9f10" value="#{node.description}" styleClass="nodeFolder"/>
                    <h:selectBooleanCheckbox title="select_investigation" valueChangeListener="#{datasetTree.setSelected}">
                       <f:param name="investigations" value="#{node.identifier}"/>
                    </h:selectBooleanCheckbox>
                </h:panelGroup>
            </f:facet>

            <f:facet name="foo1-folder">
                <h:panelGroup>
                      
                    <t:graphicImage id="gr991" value="../images/blue-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                       
                    <t:graphicImage id="gr918" value="../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                       
                    <h:outputText id="text190" value="#{node.description}" styleClass="nodeFolder"/>
                    <h:outputText id="text8ff91" value=" (#{node.childCount})" styleClass="childCount" rendered="#{!empty node.children}"/>
                       
                </h:panelGroup>
            </f:facet>
            <f:facet name="bar-folder">
                <h:panelGroup>
                    <f:facet name="expand3">
                        <t:graphicImage id="gr" value="../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                    </f:facet>
                    <f:facet name="collapse3">
                        <t:graphicImage id="gr1" value="../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                    </f:facet>
                    <h:outputText id="text786" styleClass="document" value="#{node.description}" />
           
                </h:panelGroup>
            </f:facet>
                
            <f:facet name="status-folder">
                <h:panelGroup>
                    <f:facet name="expand1">
                        <t:graphicImage id="gr17872" value="../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                    </f:facet>
                    <f:facet name="collapse1">
                        <t:graphicImage id="gr112" value="../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                    </f:facet>
                    <h:outputText id="text78" styleClass="nodeFolder" value="Status: " />
           
                    <h:outputText id="text728" styleClass="document" value="#{node.description}" />
           
                </h:panelGroup>
            </f:facet>
                
            <f:facet name="type-folder">
                <h:panelGroup>
                    <f:facet name="expand17">
                        <t:graphicImage id="gr1872" value="../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                    </f:facet>
                    <f:facet name="collapse71">
                        <t:graphicImage id="gr1712" value="../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                    </f:facet>
                    <h:outputText id="text778" styleClass="nodeFolder" value="Type " />
           
                    <h:outputText id="text7728" styleClass="document" value="#{node.description}" />
           
                </h:panelGroup>
            </f:facet>
                
            <f:facet name="file-folder">
                <h:panelGroup>
                    <f:facet name="expand163">
                        <t:graphicImage id="gr163" value="../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                    </f:facet>
                    <f:facet name="collapse163">
                        <t:graphicImage id="gr1613" value="../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                    </f:facet>
                    &nbsp;  
                    <h:commandLink immediate="true" styleClass="#{t.nodeSelected ? 'documentSelected':'document'}" actionListener="#{datasetTree.setNodeSelected}">
                        <t:graphicImage value="../images/document.png" border="0"/>
                        <h:outputText value="#{node.description}" styleClass="file" />
                        <f:param name="id" value="#{node.identifier}"/>
                    </h:commandLink>      
                    <h:selectBooleanCheckbox title="select_investigation" valueChangeListener="#{datasetTree.setSelected}">
                       <f:param name="datafiles" value="#{node.identifier}"/>
                    </h:selectBooleanCheckbox>
                </h:panelGroup>
            </f:facet>
                
            <f:facet name="fac-folder">
                <h:panelGroup>
                    <f:facet name="expand1f3">
                        <t:graphicImage id="grf13" value="../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                    </f:facet>
                    <f:facet name="collapse1f3">
                        <t:graphicImage id="gr1f13" value="../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                    </f:facet>
                    <h:outputText id="text7f38" styleClass="nodeFolder" value="Facility: " />
           
                    <h:outputText id="text73f8" styleClass="document" value="#{node.description}" />
           
                </h:panelGroup>
            </f:facet>
                
            <f:facet name="desc-folder">
                <h:panelGroup>
                    <f:facet name="expand1ff3">
                        <t:graphicImage id="grff13" value="../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                    </f:facet>
                    <f:facet name="collapse1ff3">
                        <t:graphicImage id="gr1ff13" value="../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                    </f:facet>
                    <h:outputText id="text7ff38" styleClass="nodeFolder" value="Description: " />
           
                    <h:outputText id="text73ff8" styleClass="document" value="#{node.description}" />
           
                </h:panelGroup>
            </f:facet>
    
    
        </t:tree2>
        <br />
        <table width="90%" border="0">
            <td align="left">
                <h:commandButton action="#{datasetTree.select}" title="Add selections" value="Add selections"/>
             
            </td>
           <%-- <td align="right">
                Select:&nbsp; <h:commandButton action="#{investigationBean.selectall}" title="All" value=" All "/>
                &nbsp; 
                <h:commandButton action="#{investigationBean.selectnone}" title="None" value="None"/>
            </td>--%>
        </table>
       
    </table>                        
</h:form>