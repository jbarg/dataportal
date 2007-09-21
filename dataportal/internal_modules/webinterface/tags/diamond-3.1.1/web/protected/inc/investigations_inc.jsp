<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>
<%@ taglib uri="https://ajax4jsf.dev.java.net/ajax" prefix="a4j"%>



<a4j:region   selfRendered="true" > 
    <h:form>
        
        <%--  <h:outputText id="h" value="#{investigationBean.keywordDone}"  />--%>
        <a4j:poll id="poll" actionListener="#{investigationBean.getKeywordsForInvestigations}" interval="500" reRender="h,data, poll" ajaxSingle="true"  enabled="#{!investigationBean.keywordDone}" />
        
        <%--   <a4j:poll id="poll" actionListener="#{investigationBean.checkSearchComplete}" interval="1000" reRender="poll,scroll_1,process,searchedTree,data" ajaxSingle="true" enabled="#{!investigationBean.finished}" />
--%>
        <table style="margin-top:-90px" width="90%" border="0">
             <tbody>
                <tr>
                <td width="30">&nbsp;</td>
                <tr>
                    <td>
                        <table border="0" width="100%">
                             <tbody>
                                <tr>
                                    <tr>                   
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
                                                             <tbody>
                                                                <tr>
                                                                    <td>
                                                                        <t:tree2 id="helpTree" value="#{investigationHelpTree.data}" var="node" varNodeToggler="t" showRootNode="false">
                                                                            
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
                            
                        </table>
                        <%--  <t:graphicImage id="process" value="../../images/ajax_process.gif" rendered="#{!investigationBean.finished}" border="0"/>--%>
                            
                        <t:tree2 id="searchedTree" value="#{investigationTree.data}" var="node" varNodeToggler="t" showRootNode="false">>
                            
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
                                        <h:outputText escape="false" value="#{node.description}"  />
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
                                        <h:outputText escape="false" value="#{node.description}"  />
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
                                    <h:outputText id="text3" escape="false" styleClass="document" value="#{node.description}" />
                                    
                                </h:panelGroup>
                            </f:facet>
                            
                        </t:tree2>
                    </td>
                </tr>
            </tbody>
        </table>                        
    </h:form>
    
    <br />
    
    <a4j:form>
        
        
        <t:dataTable id="data" width="90%"
                     styleClass="scrollerTable"
                     headerClass="standardTable_Header"
                     footerClass="standardTable_Header"
                     rowClasses="standardTable_Row1,standardTable_Row2"
                     columnClasses="standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered"
                     var="invest"
                     value="#{investigationBean.investigations}"
                     preserveDataModel="true"
                     rows="#{visit.userPreferences.resultsPerPage}"       
                     binding="#{investigationBean.table}"
                     sortColumn="#{investigationBean.sort}"
                     sortAscending="#{investigationBean.ascending}"
                     preserveSort="true"
                     varDetailToggler="detailToggler"       >     
            <f:facet name="header">
                <h:outputText value="Investigations" />
            </f:facet>
            
            
            <%--<h:column>
                <f:facet name="header"></f:facet>            
                <h:outputText value="#{invest.id}" />
            
                </h:column>--%>
            <h:column>
                
                <f:facet name="header">
                    <h:panelGrid columns="2" >
                        <a4j:commandLink reRender="data,expand"  style="table-header" ajaxSingle="true" id="selectAll" rendered="#{visit.visitData.investigationsSelected}" actionListener="#{investigationBean.selectNone}">           
                            <%-- <h:commandLink id="expandAll" rendered="#{!visit.visitData.investigationExpanded}" actionListener="#{investigationBean.expandAll}">--%>
                            <t:graphicImage  id="selectNonew" value="../../images/button_minus1.gif"  border="0"/>
                        </a4j:commandLink>   
                        
                        <a4j:commandLink reRender="data,expand" style="table-header" ajaxSingle="true" id="selectNone" rendered="#{!visit.visitData.investigationsSelected}" actionListener="#{investigationBean.selectAll}">           
                            <%-- <h:commandLink id="collapseAll" rendered="#{visit.visitData.investigationExpanded}" actionListener="#{investigationBean.collapseAll}">--%>                       
                            <t:graphicImage  id="selectAllc" value="../../images/button_plus1.gif"  border="0"/>                 
                        </a4j:commandLink> 
                    </h:panelGrid>
                </f:facet>
                <h:selectBooleanCheckbox title="select_investigation" value="#{invest.selected}" >
                    <f:param name="id" value="#{invest.id}"/>
                    <a4j:support reRender="expand" event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{investigationBean.listenAjax}">
                        <a4j:actionparam name="id" value="#{invest.id}"/>   
                    </a4j:support>
                </h:selectBooleanCheckbox>
            </h:column>
            
            <h:column>
                <f:facet name="header">
                    <%--  <a4j:commandLink reRender="data" style="table-header" id="name" actionListener="#{investigationBean.sortColumn}">--%>
                                                
                    <a4j:commandLink reRender="data" style="table-header" id="name" actionListener="#{investigationBean.sortColumn}">
                        <h:outputText value="Name" />
                        <f:param name="column" value="name"/>
                        <%--   <c:if test="${requestScope.investigationBean.sort == 'name'}" >--%>
                        <t:graphicImage id="acname" value="../../images/ascending-arrow.gif" rendered="#{investigationBean.name}" border="0"/>
                        <t:graphicImage id="dename" value="../../images/descending-arrow.gif" rendered="#{investigationBean.notName}" border="0"/>
                        <%--  </c:if>--%>
                    </a4j:commandLink>
                </f:facet>
                
                <h:commandLink value="#{invest.name}" style=" color: black" styleClass="investigation" action="#{investigationBean.viewSingleInvestigation}" />
                <%-- <h:outputText  value="#{invest.name}" />--%>
                
            </h:column>
            
            <h:column>
                <f:facet name="header">
                    <a4j:commandLink reRender="data" style="table-header" ajaxSingle="true" id="visitId" actionListener="#{investigationBean.sortColumn}">
                        
                        <h:outputText value="Visit Id" />
                        <f:param name="column" value="visitId"/>
                        <%--   <c:if test="${requestScope.investigationBean.sort == 'type'}" >--%>
                        <t:graphicImage id="acdhffty" value="../../images/ascending-arrow.gif" rendered="#{investigationBean.visitId}" border="0"/>
                        <t:graphicImage id="dedffty" value="../../images/descending-arrow.gif" rendered="#{investigationBean.notVisitId}" border="0"/>
                        <%--  </c:if>--%>
                    </a4j:commandLink>
                </f:facet>
                <h:outputText  value="#{invest.visitId}" />                              
                
            </h:column>
            
            <h:column>
                <f:facet name="header">
                    <a4j:commandLink reRender="data" style="table-header" ajaxSingle="true" id="instrument" actionListener="#{investigationBean.sortColumn}">
                        
                        <h:outputText value="Instrument" />
                        <f:param name="column" value="instrument"/>
                        <%--   <c:if test="${requestScope.investigationBean.sort == 'type'}" >--%>
                        <t:graphicImage id="acsdtddy" value="../../images/ascending-arrow.gif" rendered="#{investigationBean.instrument}" border="0"/>
                        <t:graphicImage id="deddddty" value="../../images/descending-arrow.gif" rendered="#{investigationBean.notInstrument}" border="0"/>
                        <%--  </c:if>--%>
                    </a4j:commandLink>
                </f:facet>
                <h:outputText  value="#{invest.instrument}" />                              
                
            </h:column>
            
            <h:column>
                <f:facet name="header">
                    <a4j:commandLink reRender="data" style="table-header" ajaxSingle="true" id="type" actionListener="#{investigationBean.sortColumn}">
                        
                        <h:outputText value="Type" />
                        <f:param name="column" value="type"/>
                        <%--   <c:if test="${requestScope.investigationBean.sort == 'type'}" >--%>
                        <t:graphicImage id="acty" value="../../images/ascending-arrow.gif" rendered="#{investigationBean.type}" border="0"/>
                        <t:graphicImage id="dety" value="../../images/descending-arrow.gif" rendered="#{investigationBean.notType}" border="0"/>
                        <%--  </c:if>--%>
                    </a4j:commandLink>
                </f:facet>
                
                <h:outputText  value="#{fn:toLowerCase(invest.investigationType)}" />               
            </h:column>
            <h:column>
                <f:facet name="header">
                    <h:panelGrid columns="2" >
                        
                        <a4j:commandLink reRender="data"  id="abstract" ajaxSingle="true" actionListener="#{investigationBean.sortColumn}">
                            <h:outputText style="font-weight:bold; " value="Abstract" />
                            <f:param name="column" value="abstract"/>
                            <%-- <c:if test="${requestScope.investigationBean.sort == 'abstract'}" >--%>
                            <t:graphicImage id="acabstract" value="../../images/ascending-arrow.gif" rendered="#{investigationBean.abstract}" border="0"/>
                            <t:graphicImage id="deabstract" value="../../images/descending-arrow.gif" rendered="#{investigationBean.notAbstract}" border="0"/>
                            <%--   </c:if>--%>
                        </a4j:commandLink>  
                        
                        <a4j:commandLink reRender="data"  style="table-header" ajaxSingle="true" id="expandAll" rendered="#{!visit.visitData.investigationExpanded}" actionListener="#{investigationBean.expandAll}">
                            
                            <%-- <h:commandLink id="expandAll" rendered="#{!visit.visitData.investigationExpanded}" actionListener="#{investigationBean.expandAll}">--%>
                            <t:graphicImage  id="exp" value="../../images/button_plus1.gif"  border="0"/>
                        </a4j:commandLink>   
                        <a4j:commandLink reRender="data" style="table-header" ajaxSingle="true" id="collapseAll" rendered="#{visit.visitData.investigationExpanded}" actionListener="#{investigationBean.collapseAll}">
                            
                            <%-- <h:commandLink id="collapseAll" rendered="#{visit.visitData.investigationExpanded}" actionListener="#{investigationBean.collapseAll}">--%>
                            <t:graphicImage  id="coll" value="../../images/button_minus1.gif"  border="0"/>
                        </a4j:commandLink> 
                        
                    </h:panelGrid>        
                </f:facet>
                
                
                <a4j:commandLink rendered="#{!invest.abstractNull}" reRender="data" style="table-header" ajaxSingle="true" id="abS" action="#{detailToggler.toggleDetail}">
                    <%--  <h:commandLink  rendered="#{invest.abstractNull}" action="#{detailToggler.toggleDetail}">--%>
                    <t:graphicImage id="up" value="../../images/button_plus1.gif" rendered="#{!detailToggler.currentDetailExpanded}" border="0"/>
                    <t:graphicImage id="up-f" value="../../images/button_minus1.gif" rendered="#{detailToggler.currentDetailExpanded}" border="0"/>
                </a4j:commandLink>   
                
                
            </h:column>
            <h:column>
                <f:facet name="header">
                    <%--<a4j:commandLink reRender="data" style="table-header" id="facility" actionListener="#{investigationBean.sortColumn}">--%>
                   
                    <a4j:commandLink reRender="data" style="table-header" id="facility" ajaxSingle="true" actionListener="#{investigationBean.sortColumn}">
                        <h:outputText value="Facility" />
                        <f:param name="column" value="facility"/>
                        <%-- <c:if test="${requestScope.investigationBean.sort == 'facility'}" >                            --%>
                        <t:graphicImage id="acf" value="../../images/ascending-arrow.gif" rendered="#{investigationBean.facility}" border="0"/>
                        <t:graphicImage id="def" value="../../images/descending-arrow.gif" rendered="#{investigationBean.notFacility}" border="0"/>
                        <%--    </c:if>--%>
                    </a4j:commandLink>
                </f:facet>
                <h:outputText value="#{invest.facility}" />
                
            </h:column>
            
            <h:column>
                
                <f:facet name="header" />
                
                <t:popup  styleClass="popup"   closePopupOnExitingElement="true"
                          closePopupOnExitingPopup="true"
                          displayAtDistanceX="15"
                          displayAtDistanceY="-60" >
                    <t:graphicImage rendered="#{!invest.investigatorsNull}" url="../../images/person.png" border="0" />
                    
                    <f:facet name="popup">
                        
                        <t:dataTable  id="investigationPIs" styleClass="standardTable_ColumnCenter, standardTable_ColumnCenter,standardTable_ColumnCenter" var="investigatorspis" value="#{invest.listOfInvestigators}">
                            
                            <h:column>                    
                                <f:facet name="header">
                                    <h:outputText style="font-size: 10px" value="Investigators" />
                                </f:facet>
                                
                                <h:outputText style="font-size: 10px" value="#{investigatorspis.name}" />
                            </h:column>     
                            
                            <h:column>   
                                <f:facet name="header">
                                    <h:outputText style="font-size: 10px; " value="ID" />
                                </f:facet>
                                <h:outputText style="font-size: 10px" value="#{investigatorspis.id}" />
                            </h:column>     
                            
                            <h:column>    
                                <f:facet name="header">
                                    <h:outputText style="font-size: 10px" value="Role" />
                                </f:facet>
                                <h:outputText style="font-size: 10px" value="#{investigatorspis.role}" />
                            </h:column>     
                            
                        </t:dataTable> 
                        
                    </f:facet>
                </t:popup>
            </h:column>
            
            <h:column>
                
                <f:facet name="header" />
                
                <t:popup  styleClass="popup"   closePopupOnExitingElement="true"
                          closePopupOnExitingPopup="true"
                          displayAtDistanceX="15"
                          displayAtDistanceY="-60" >
                    <t:graphicImage  url="../../images/Key.jpg" border="0" />
                    
                    <f:facet name="popup">
                        
                        <t:dataTable id="investigationKeywords" styleClass="standardTable_ColumnCenter, standardTable_ColumnCenter,standardTable_ColumnCenter" var="investkeywords" value="#{invest.keywords}">
                            
                            <t:column colspan="4" >                    
                                <f:facet name="header">
                                    <h:outputText style="font-size: 10px" value="Keywords" />
                                </f:facet>                                
                                <h:outputText style="font-size: 10px" value="#{investkeywords}" />
                            </t:column>                               
                        </t:dataTable> 
                        
                    </f:facet>
                </t:popup>
            </h:column>
            
            
            
            
            <f:facet name="detailStamp">                
                <t:dataTable preserveSort="true" width="95%" id="investigationAbstract" styleClass="standardTable_Column" var="abstract" value="#{invest.investigationAbstract}">
                    
                    <h:column>
                        <h:outputText style="font-size:12px;" value="#{invest.investigationAbstract}" />
                    </h:column> 
                    <h:column>
                        <%--   <t:popup styleClass="popup"   closePopupOnExitingElement="true"
                                 closePopupOnExitingPopup="true"
                                 displayAtDistanceX="5"
                                 displayAtDistanceY="-40" >
                            <t:graphicImage url="../../images/help.gif" border="0" />
                            &nbsp;&nbsp;&nbsp;
                            <f:facet name="popup">
                                fgdfgdfgdfgd
                                 <t:dataTable preserveSort="true" width="95%" id="investigationPIs" styleClass="standardTable_Column" var="investigatorspis" value="#{invest.listOfInvestigators}">
                            <h:column>                    
                                
                                
                                <h:outputText style="font-size: 10px" value="#{investigatorspis.name}" />
                            </h:column>     
                            <h:column>                
                                <h:outputText style="font-size: 10px" value="#{investigatorspis.role}" />
                            </h:column>     
                            <h:column>                
                                <h:outputText style="font-size: 10px" value="#{investigatorspis.id}" />
                            </h:column>     
                        </t:dataTable> 
                        
                            </f:facet>
                        </t:popup>--%>
                    </h:column> 
                    
                </t:dataTable>   
                
                
            </f:facet>            
        </t:dataTable>
        
        
        
        <c:if test="${fn:length(sessionScope.visit.visitData.searchedInvestigations) > sessionScope.visit.userPreferences.resultsPerPage}" >
            
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
        </c:if>
        
        
        <table width="90%" border="0">
             <tbody>
                <tr>
                    <td align="left">
                        <h:commandButton styleClass="button" id="expand"  action="#{investigationBean.datasets}" title="View selections" value="View selection"/>
                        
                    </td>
                    <%--<td align="right">
                        <a4j:commandButton action="#{investigationBean.selectall}" reRender="datatable" title="Delete selections" value="Delete selections"/>
                        &nbsp;
                                        
                        <t:graphicImage id="delete_selected"  value="../../images/delete.jpg"  border="0"/>                                                                            
                                         
                        Select:&nbsp; <h:commandButton action="#{investigationBean.selectall}" title="All" value=" All "/>
                        &nbsp; 
                        <h:commandButton action="#{investigationBean.selectnone}" title="None" value="None"/>
                        </td>--%>
                </tr>
            </tbody>
        </table>
        
    </a4j:form>
    
</a4j:region>