<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>
<%@ taglib uri="https://ajax4jsf.dev.java.net/ajax" prefix="a4j"%>
<h:form>
    <table style="margin-top:-20px" width="95%" border="0">
         <tbody>
            <tr>            
                <td>
                    <table style="margin-top:-20px" width="100%" border="0">
                         <tbody>
                            <tr>   
                                <td>    <h:messages globalOnly="true" errorClass="error" infoClass="info" /></td>
                            </tr>                               
                            
                        </tbody>   
                    </table>
                    
                    <h:form>
                        <h:panelGrid columns="3">
                            
                            <h:outputLabel>
                                <h:outputText styleClass="body" value="Search events:"/>
                            </h:outputLabel  >
                            
                            <h:selectOneMenu id="typeAccessj_id_1" validator="#{viewUserAdmin.validateType}" required="true" value="#{viewUserAdmin.type}" >
                                <f:selectItem itemValue="ONE" itemLabel="Select One" itemDisabled="true"/>
                                <f:selectItem itemValue="ALL" itemLabel="ALL"/>
                                <f:selectItem itemValue="LOG_ON" itemLabel="LOG ON"/>
                                <f:selectItem itemValue="LOG_OFF" itemLabel="LOG OFF"/>
                                <f:selectItem itemValue="BASIC_SEARCH" itemLabel="BASIC SEARCH"/>
                                <f:selectItem itemValue="DOWNLOAD" itemLabel="DOWNLOAD"/>
                                
                                
                            </h:selectOneMenu>
                            
                            
                            <h:message for="typeAccessj_id_1" styleClass="error" showSummary="false" showDetail="true" />
                            
                            <h:outputLabel>
                                <h:outputText styleClass="body" value="Search from:"/>
                            </h:outputLabel  >
                            
                            
                            <t:inputCalendar  binding="#{viewUserAdmin.calendarFirst}" id="thirdOnej_id_1"  monthYearRowClass="yearMonthHeader" weekRowClass="weekHeader" popupButtonStyleClass="standard_bold"
                                              currentDayCellClass="currentDayCell" value="#{viewUserAdmin.firstDate}" renderAsPopup="true"
                                              popupTodayString="Today is:"
                                              popupDateFormat="dd/MM/yyyy" popupWeekString="Wk"
                                              helpText="DD/MM/YYYY" />
                            
                            <h:message for="thirdOnej_id_1" styleClass="error" showSummary="false" showDetail="true" />
                            
                            
                            <h:outputLabel>
                                <h:outputText styleClass="body" value="Search to:"/>
                            </h:outputLabel  >
                            
                            
                            <t:inputCalendar binding="#{viewUserAdmin.calendarSecond}" id="forthOnej_id_1"  monthYearRowClass="yearMonthHeader" weekRowClass="weekHeader" popupButtonStyleClass="standard_bold"
                                             currentDayCellClass="currentDayCell" value="#{viewUserAdmin.secondDate}" renderAsPopup="true"
                                             popupTodayString="Today is:"
                                             popupDateFormat="dd/MM/yyyy" popupWeekString="Wk"
                                             helpText="DD/MM/YYYY" validator="#{viewUserAdmin.validateDate}"/>
                            
                            <h:message for="forthOnej_id_1" styleClass="error" showSummary="false" showDetail="true" />
                            
                            <%-- Does not work with rendering renerdre=false sections" <a4j:commandButton reRender="searchResults" actionListener="#{dataAuthorisationBean.search}" value="Search"/>--%>
                            <%-- Old button before ajax button--%>
                            <h:commandButton action="#{viewUserAdmin.search}" title="Search" value="Search"/>
                            
                        </h:panelGrid>
                    </h:form>
                    
                    <br />
                    <a4j:region   selfRendered="true" > 
                        
                        <t:dataTable rendered="#{viewUserAdmin.populated}" id="datatable" width="100%"
                                     styleClass="scrollerTable"
                                     headerClass="standardTable_Header"
                                     footerClass="standardTable_Header"
                                     rowClasses="standardTable_Row1,standardTable_Row2"
                                     columnClasses="standardTable_Column,standardTable_Column,standardTable_ColumnCentered, standardTable_ColumnCentered, standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered"
                                     var="data"
                                     value="#{viewUserAdmin.dataRefs}"
                                     preserveDataModel="true"
                                     rows="#{visit.userPreferences.resultsPerPage}"
                                     rowId="#{data.id}"
                                     binding="#{viewUserAdmin.table}"
                                     sortColumn="#{viewUserAdmin.sort}"
                                     sortAscending="#{viewUserAdmin.ascending}"
                                     preserveSort="true"
                                     varDetailToggler="detailToggler">                                
                            <f:facet name="header">
                                <h:outputText value="#{visit.adminData.searchUser} Event's Log" />
                            </f:facet>          
                            
                            <h:column>
                                <f:facet name="header"> <h:outputText escape="false" value="&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" /></f:facet>  
                                
                                <a4j:commandLink  reRender="datatable" action="#{detailToggler.toggleDetail}">
                                    <%--<h:commandLink rendered="#{data.dataset}" action="#{detailToggler.toggleDetail}">--%>
                                    <h:panelGroup>
                                        
                                        <t:graphicImage id="down" value="../../images/button_plus1.gif" rendered="#{!detailToggler.currentDetailExpanded}" border="0"/>
                                        <t:graphicImage id="down-f" value="../../images/blue-folder-closed.png" rendered="#{!detailToggler.currentDetailExpanded}" border="0"/>
                                        
                                        <t:graphicImage id="up" value="../../images/button_minus1.gif" rendered="#{detailToggler.currentDetailExpanded}" border="0"/>
                                        <t:graphicImage id="up-f" value="../../images/blue-folder-open.png" rendered="#{detailToggler.currentDetailExpanded}" border="0"/>
                                        
                                    </h:panelGroup>
                                    
                                </a4j:commandLink>
                                
                            </h:column>
                            
                            <h:column>
                                <f:facet name="header"> 
                                    <a4j:commandLink id="ff444ff4" reRender="datatable" actionListener="#{viewUserAdmin.sortColumn}">
                                        
                                        <h:outputText value="Event Type" />
                                        <f:param name="column" value="event"/>
                                        
                                        <t:graphicImage id="wy3" value="../../images/ascending-arrow.gif" rendered="#{viewUserAdmin.event}" border="0"/>
                                        <t:graphicImage id="w3y4" value="../../images/descending-arrow.gif" rendered="#{viewUserAdmin.notEvent}" border="0"/>
                                        
                                    </a4j:commandLink>
                                    
                                </f:facet>  
                                
                                <h:outputText value="#{data.event}" />                     
                                
                            </h:column>
                            <h:column>
                                <f:facet name="header"> 
                                    <a4j:commandLink id="detaghghgil" reRender="datatable" actionListener="#{viewUserAdmin.sortColumn}">
                                        
                                        <%--     <h:commandLink style="table-header" id="name" actionListener="#{datacenterBean.sortColumn}">--%>
                                        <h:outputText value="Details" />
                                        <f:param name="column" value="details"/>
                                        <%--<c:if test="${requestScope.datacenterBean.sort == 'name'}" >--%>
                                        <t:graphicImage id="acy3" value="../../images/ascending-arrow.gif" rendered="#{viewUserAdmin.details}" border="0"/>
                                        <t:graphicImage id="dey3" value="../../images/descending-arrow.gif" rendered="#{viewUserAdmin.notDetails}" border="0"/>
                                        <%--    </c:if>--%>
                                    </a4j:commandLink>
                                    
                                </f:facet>  
                                
                                <h:outputText value="#{data.details}" />                     
                                
                            </h:column>
                            
                            <h:column>
                                <f:facet name="header"> 
                                    <a4j:commandLink id="time" reRender="datatable" actionListener="#{viewUserAdmin.sortColumn}">
                                        
                                        <%--     <h:commandLink style="table-header" id="name" actionListener="#{datacenterBean.sortColumn}">--%>
                                        <h:outputText value="Time" />
                                        <f:param name="column" value="time"/>
                                        <%--<c:if test="${requestScope.datacenterBean.sort == 'name'}" >--%>
                                        <t:graphicImage id="ayc1" value="../../images/ascending-arrow.gif" rendered="#{viewUserAdmin.time}" border="0"/>
                                        <t:graphicImage id="dey1" value="../../images/descending-arrow.gif" rendered="#{viewUserAdmin.notTime}" border="0"/>
                                        <%--    </c:if>--%>
                                    </a4j:commandLink>
                                    
                                </f:facet>  
                                <h:outputText value="#{data.modTime}" >
                                    <f:convertDateTime pattern="HH:mm  dd.MM.yyyy "/>
                                </h:outputText>                                                
                            </h:column>
                            
                            <f:facet name="detailStamp">
                                 
                                
                                <t:dataTable preserveSort="true"  width="55%" id="detailsTable" columnClasses="standardTable_Column,standardTable_Column"
                                             styleClass="standardTable_Column" var="detailsList" value="#{data.eventLogDetails}">
                                   
                                    <h:outputText style="font-size: 10px" value="#{detailsList.details}" />v
                                   
                                </t:dataTable>
                                
                            </f:facet>
                            
                        </t:dataTable>
                        
                        
                        <h:panelGrid columns="1" rendered="#{viewUserAdmin.length}" styleClass="scrollerTable2" columnClasses="standardTable_ColumnCentered" >
                            <t:dataScroller id="scroll_11"
                                            for="datatable"
                                            fastStep="10"
                                            pageCountVar="pageCount"
                                            pageIndexVar="pageIndex"
                                            styleClass="scroller"
                                            paginator="true"
                                            paginatorMaxPages="9"
                                            paginatorTableClass="paginator"
                                            paginatorActiveColumnStyle="font-weight:bold;">
                                <f:actionListener type="uk.ac.dl.dp.web.navigation.DataScrollerActionListener"/>
                                
                                <%-- <a4j:ajaxListener type="uk.ac.dl.dp.web.navigation.DataScrollerActionListener"/>--%>
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
                        </h:panelGrid>
                        <h:panelGrid columns="1" styleClass="scrollerTable2" columnClasses="standardTable_ColumnCentered" >
                            
                            <t:dataScroller id="scroll_2"
                                            for="datatable"
                                            rowsCountVar="rowsCount"
                                            displayedRowsCountVar="displayedRowsCountVar"
                                            firstRowIndexVar="firstRowIndex"
                                            lastRowIndexVar="lastRowIndex"
                                            pageCountVar="pageCount"
                                            immediate="true"
                                            pageIndexVar="pageIndex"
                            >
                                <h:outputFormat value="{0} Events found, displaying {1}, from {2} to {3}. Page {4} / {5}" styleClass="standard" >
                                    <f:param value="#{rowsCount}" />
                                    <f:param value="#{displayedRowsCountVar}" />
                                    <f:param value="#{firstRowIndex}" />
                                    <f:param value="#{lastRowIndex}" />
                                    <f:param value="#{pageIndex}" />
                                    <f:param value="#{pageCount}" />
                                </h:outputFormat>
                            </t:dataScroller>
                        </h:panelGrid>
                        
                    </a4j:region>
                    
                    
                </td>
            </tr>
        </tbody>   
    </table>
</h:form>