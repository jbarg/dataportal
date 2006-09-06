<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>
<h:form>
   


    <table width="90%" border="0">
        <td width="30">&nbsp;</td>
        <td>
            <table border="0" width="100%">
                <tr>
                    
                    <c:if test="${fn:length(sessionScope.visit.searchedInvestigations) > 500}" >
                        <h:outputText styleClass="info"  value="More than the 500 results shown was returned.  Please refine your query next time."/><br />
                    </c:if>
                    <h:messages globalOnly="true" errorClass="error" warnClass="info" infoClass="info" />
                </tr>                 
                <td width="60%">    &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
          
                </td>
                <td align="left">&nbsp;&nbsp;&nbsp;
                    <t:popup styleClass="popup"   closePopupOnExitingElement="true"
                        closePopupOnExitingPopup="true"
                        displayAtDistanceX="5"
                        displayAtDistanceY="-40" >

                         <t:graphicImage url="../../images/help.gif" border="0" />
              &nbsp;&nbsp;&nbsp;
                        <f:facet name="popup">
                            <h:panelGroup>
                                <h:panelGrid columns="1" >
                                    <%--  <f:subview id="helpl">
                                    <c:import url="/investigations_inc_help.jsp" />
                                    </f:subview>--%>
                                    <table width="300" border="0">
                                        <td>
                                            <t:tree2 id="tree166" value="#{investigationHelpTree.data}" var="node" varNodeToggler="t" showRootNode="false">
    
                                                <f:facet name="hfoo-folder">
                                                    <h:panelGroup>                
                                                        <t:graphicImage id="gr34h" value="../../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                                  
                                                        <t:graphicImage id="gr3h" value="../../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                        
                                                        <h:outputText id="text" value="#{node.description}" styleClass="nodeFolder"/>
                           
                       
                                                        <h:outputText id="t3ext1h" value=" (#{node.identifier})" styleClass="childCount" rendered="#{!empty node.children}"/>
                            
                                                    </h:panelGroup>
                                                </f:facet>
                                                <f:facet name="hfoo1-folder">
                                                    <h:panelGroup>
                                                        <f:facet name="expand22h">
                                                            <t:graphicImage id="gr43h2" value="../../../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                                                        </f:facet>
                                                        <f:facet name="collapse2h">
                                                            <t:graphicImage id="gr32h" value="../../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                                                        </f:facet>
                                                        <h:outputText id="text2" value="#{node.description}" styleClass="nodeFolder"/>
                       
                                                    </h:panelGroup>
                                                </f:facet>
               
    
                                            </t:tree2>
                                        </td>
                                    </table>
                                </h:panelGrid>
                            </h:panelGroup>
                        </f:facet>
                    </t:popup>
                </td>
             
            </table>
           
            <t:tree2 id="tree" value="#{investigationTree.data}" var="node" varNodeToggler="t" showRootNode="false">>
    
                <f:facet name="foo-folder">
                    <h:panelGroup>
                        <f:facet name="expand">
                            <t:graphicImage id="gr4" value="../../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                        </f:facet>
                        <f:facet name="collapse">
                            <t:graphicImage id="gr3" value="../../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                        </f:facet>
                      
                        <%-- <h:outputText id="text" value="#{node.description}" styleClass="nodeFolder"/>--%>
                        <h:commandLink  immediate="true" styleClass="nodeFolder" style="color:black" action="#{investigationTree.minimise}" actionListener="#{investigationTree.setNodeSelected}">
                            <h:outputText value="#{node.description}"  />
                            <f:param name="id" value="#{node.description}"/>
                        </h:commandLink>      
                        <h:outputText id="text1" value=" (#{node.identifier})" styleClass="childCount" rendered="#{!empty node.children}"/>
                    </h:panelGroup>
                </f:facet>
                <f:facet name="foo1-folder">
                    <h:panelGroup>
                        <f:facet name="expand22">
                            <t:graphicImage id="gr42" value="../../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                        </f:facet>
                        <f:facet name="collapse2">
                            <t:graphicImage id="gr32" value="../../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                        </f:facet>
                        <%--  <h:outputText id="text2" value="#{node.description}" styleClass="nodeFolder"/>--%>
                        <h:commandLink disabled="true" immediate="true" styleClass="nodeFolder" style="color:black" action="#{investigationTree.selectone}" actionListener="#{investigationTree.setNodeSelected}">
                            <h:outputText value="#{node.description}"  />
                            <f:param name="id" value="#{node.description}"/>
                        </h:commandLink>  
                    </h:panelGroup>
                </f:facet>
                <f:facet name="bar-folder">
                    <h:panelGroup>
                        <f:facet name="expand2">
                            <t:graphicImage id="gr" value="../../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                        </f:facet>
                        <f:facet name="collapse3">
                            <t:graphicImage id="gr1" value="../../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                        </f:facet>
                        <h:outputText id="text3" styleClass="document" value="#{node.description}" />
           
                    </h:panelGroup>
                </f:facet>
    
            </t:tree2>
        </td>
    </table>                        
</h:form>

<br />
<h:form>
    <t:dataTable id="data" width="90%"
        styleClass="scrollerTable"
        headerClass="standardTable_Header"
        footerClass="standardTable_Header"
        rowClasses="standardTable_Row1,standardTable_Row2"
        columnClasses="standardTable_Column,standardTable_ColumnCentered,standardTable_Column"
        var="invest"
        value="#{investigationBean.investigations}"
        preserveDataModel="true"
        rows="#{visit.userPreferences.resultsPerPage}"
        rowId="#{invest.id}"
        binding="#{investigationBean.table}"
        sortColumn="#{investigationBean.sort}"
        sortAscending="#{investigationBean.ascending}"
        preserveSort="true">
        <f:facet name="header">
            <h:outputText value="Investigations" />
        </f:facet>

        <h:column>
            <f:facet name="header"></f:facet>            
            <h:outputText value="#{invest.id}" />
              
        </h:column>
        <h:column>
            <f:facet name="header">
                <h:commandLink style="table-header" id="name" actionListener="#{investigationBean.sortColumn}">
                    <h:outputText value="Name" />
                    <f:param name="column" value="name"/>
                </h:commandLink>
            </f:facet>
            <h:outputText  value="#{invest.name}" />

        </h:column>
        <h:column>
            <f:facet name="header">
                <h:commandLink style="table-header" id="type" actionListener="#{investigationBean.sortColumn}">
                    <h:outputText value="Type" />
                    <f:param name="column" value="type"/>
               
                </h:commandLink>
            </f:facet>
            <h:outputText  value="#{invest.investigationType}" />

        </h:column>
        <h:column>
            <f:facet name="header">
                <h:commandLink style="table-header" id="abstract" actionListener="#{investigationBean.sortColumn}">
                    <h:outputText value="Abstract" />
                    <f:param name="column" value="abstract"/>
               
                </h:commandLink>
            </f:facet>
            <h:outputText  value="#{invest.investigationAbstract}" />

        </h:column>
        <h:column>
            <f:facet name="header">
                <h:commandLink style="table-header" id="facility" actionListener="#{investigationBean.sortColumn}">
                    <h:outputText value="Facility" />
                    <f:param name="column" value="facility"/>
              
                </h:commandLink>
            </f:facet>
            <h:outputText value="#{invest.facility}" />
        </h:column>
       
        <h:column>
            <f:facet name="header">
                <h:outputText value="" />
            </f:facet>
            <h:selectBooleanCheckbox title="select_investigation" valueChangeListener="#{investigationBean.listen}" value="#{invest.selected}" >
                <f:param name="id" value="#{invest.id}"/>
            </h:selectBooleanCheckbox>
        </h:column>

    </t:dataTable>

    <h:panelGrid columns="1" styleClass="scrollerTable2" columnClasses="standardTable_ColumnCentered" >
        <t:dataScroller id="scroll_1"
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
         <t:dataScroller id="scroll_2"
                    for="data"
                    rowsCountVar="rowsCount"
                    displayedRowsCountVar="displayedRowsCountVar"
                    firstRowIndexVar="firstRowIndex"
                    lastRowIndexVar="lastRowIndex"
                    pageCountVar="pageCount"
                    immediate="true"
                    pageIndexVar="pageIndex"
                    >
                <h:outputFormat value="{0} Investigations found, displaying {1}, from {2} to {3}. Page {4} / {5}" styleClass="standard" >
                    <f:param value="#{rowsCount}" />
                    <f:param value="#{displayedRowsCountVar}" />
                    <f:param value="#{firstRowIndex}" />
                    <f:param value="#{lastRowIndex}" />
                    <f:param value="#{pageIndex}" />
                    <f:param value="#{pageCount}" />
                </h:outputFormat>
            </t:dataScroller>
    </h:panelGrid>
    <br />

    <table width="90%" border="0">
        <td>
            <h:commandButton action="#{investigationBean.datasets}" title="View selections" value="View selection"/>
             
        </td>
        <td align="right">
            Select:&nbsp; <h:commandButton action="#{investigationBean.selectall}" title="All" value=" All "/>
            &nbsp; 
            <h:commandButton action="#{investigationBean.selectnone}" title="None" value="None"/>
        </td>
    </table>
</h:form>