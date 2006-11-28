<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>
<%@ taglib uri="https://ajax4jsf.dev.java.net/ajax" prefix="a4j"%>

<table width="90%">
     <tbody>
        <tr>
           
            <td>
                <table style="margin-top:-20px" width="90%" border="0">
                     <tbody>
                        <tr>
                            <td> <h:messages id="message" globalOnly="true" errorClass="error" infoClass="info" />
                            </td>                 
                            
                        </tr>
                    </tbody>   
                </table>
                
                
                <a4j:region selfRendered="true"> 
                    <h:form>
                        <h:panelGrid columns="4">
                            
                            <h:outputLabel>
                                <h:outputText styleClass="body" value="Search DNs:"/>
                            </h:outputLabel  >
                            
                            <h:inputText id="dnj_id_1" size="30" required="true" value="#{visit.adminData.searchString}">
                                <f:validateLength minimum="1" maximum="60"/>
                            </h:inputText> 
                            
                            <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                                     closePopupOnExitingPopup="true"
                                     displayAtDistanceX="5"
                                     displayAtDistanceY="-40" >
                                
                                <t:graphicImage url="../../images/help.gif" border="0" />
                                <f:facet name="popup">
                                    <h:panelGroup>
                                        <h:panelGrid columns="1" >
                                            <table width="100" >
                                                <tr>
                                                    <td nowrap="false" width="150">
                                                        <h:outputText style="font-size: 14px" value="Use 'ALL' to search all users." />
                                                    </td>
                                                </tr>
                                            </table>
                                        </h:panelGrid>
                                    </h:panelGroup>
                                </f:facet>
                            </t:popup>
                            <h:message for="dnj_id_1" styleClass="error" showSummary="false" showDetail="true" />
                           
                            
                           <h:outputLabel>
                                <h:outputText styleClass="body" value="Search events:"/>
                            </h:outputLabel  >
                            
                            <h:selectOneMenu id="typeAccessj_id_1" validator="#{viewAllUserStatsAdmin.validateType}" required="true" value="#{viewUserAdmin.type}" >
                                <f:selectItem itemValue="ONE" itemLabel="Select One" itemDisabled="true"/>
                                <f:selectItem itemValue="LOG_ON" itemLabel="LOG ON"/>
                                <f:selectItem itemValue="LOG_OFF" itemLabel="LOG OFF"/>
                                <f:selectItem itemValue="BASIC_SEARCH" itemLabel="BASIC SEARCH"/>
                                <f:selectItem itemValue="DOWNLOAD" itemLabel="DOWNLOAD"/>
                                
                                
                            </h:selectOneMenu>
                            
                            
                            <h:message for="typeAccessj_id_1" styleClass="error" showSummary="false" showDetail="true" />
                            
                            <h:outputLabel>
                                <h:outputText styleClass="body" value="Search from:"/>
                            </h:outputLabel  >
                            
                            
                            <t:inputCalendar  binding="#{viewAllUserStatsAdmin.calendarFirst}" id="thirdOnej_id_1"  monthYearRowClass="yearMonthHeader" weekRowClass="weekHeader" popupButtonStyleClass="standard_bold"
                                              currentDayCellClass="currentDayCell" value="#{viewAllUserStatsAdmin.firstDate}" renderAsPopup="true"
                                              popupTodayString="Today is:"
                                              popupDateFormat="dd/MM/yyyy" popupWeekString="Wk"
                                              helpText="DD/MM/YYYY" />
                            
                            <h:message for="thirdOnej_id_1" styleClass="error" showSummary="false" showDetail="true" />
                            
                            <h:panelGroup/>
                            <h:outputLabel>
                                <h:outputText styleClass="body" value="Search to:"/>
                            </h:outputLabel  >
                            
                            
                            <t:inputCalendar binding="#{viewAllUserStatsAdmin.calendarSecond}" id="forthOnej_id_1"  monthYearRowClass="yearMonthHeader" weekRowClass="weekHeader" popupButtonStyleClass="standard_bold"
                                             currentDayCellClass="currentDayCell" value="#{viewAllUserStatsAdmin.secondDate}" renderAsPopup="true"
                                             popupTodayString="Today is:"
                                             popupDateFormat="dd/MM/yyyy" popupWeekString="Wk"
                                             helpText="DD/MM/YYYY" validator="#{viewAllUserStatsAdmin.validateDate}"/>
                            
                            <h:message for="forthOnej_id_1" styleClass="error" showSummary="false" showDetail="true" />
                            <h:panelGroup/>
                            
                            <h:panelGroup/>
                            <%-- Does not work with rendering renerdre=false sections" <a4j:commandButton reRender="searchResults" actionListener="#{dataAuthorisationBean.search}" value="Search"/>--%>
                            <%-- Old button before ajax button--%>
                            <h:commandButton actionListener="#{viewAllUserStatsAdmin.search}" title="Search" value="Search"/>
                            <h:panelGroup/>
                            <h:panelGroup/>
                        </h:panelGrid>
                    </h:form>
                    <br />
                    <h:form>
                        <h:panelGrid id="searchResults" rendered="#{visit.adminData.searched}" columns="3">
                            <a4j:region   selfRendered="true" > 
                                
                                <t:dataTable rendered="#{viewAllUserStatsAdmin.populated}" id="datatable" width="100%"
                                             styleClass="scrollerTable"
                                             headerClass="standardTable_Header"
                                             footerClass="standardTable_Header"
                                             rowClasses="standardTable_Row1,standardTable_Row2"
                                             columnClasses="standardTable_Column,standardTable_ColumnCentered,standardTable_ColumnCentered, standardTable_ColumnCentered, standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered"
                                             var="data"
                                             value="#{viewAllUserStatsAdmin.dataRefs}"
                                             preserveDataModel="true"
                                             rows="#{visit.userPreferences.resultsPerPage}"
                                             rowId="#{data.id}"
                                             binding="#{viewAllUserStatsAdmin.table}"
                                             sortColumn="#{viewAllUserStatsAdmin.sort}"
                                             sortAscending="#{viewAllUserStatsAdmin.ascending}"
                                             preserveSort="true"
                                             varDetailToggler="detailToggler">                                
                                    <f:facet name="header">
                                        <h:outputText value="User Statistics" />
                                    </f:facet>          
                                    
                                    
                                    <h:column>
                                        <f:facet name="header"> 
                                            <a4j:commandLink id="ff444ff4" reRender="datatable" actionListener="#{viewAllUserStatsAdmin.sortColumn}">
                                                
                                                <h:outputText value="DN" />
                                                <f:param name="column" value="dn"/>
                                                
                                                <t:graphicImage id="w3" value="../../images/ascending-arrow.gif" rendered="#{viewAllUserStatsAdmin.dn}" border="0"/>
                                                <t:graphicImage id="w34" value="../../images/descending-arrow.gif" rendered="#{viewAllUserStatsAdmin.notDN}" border="0"/>
                                                
                                            </a4j:commandLink>
                                            
                                        </f:facet>  
                                        <h:commandLink id="viewuserall" style="table-header; color: black"  action="#{viewAllUserStatsAdmin.viewUser}" >                                        
                                            <h:outputText value="#{data.dn}" />   
                                        </h:commandLink>
                                        
                                    </h:column>
                                    <h:column>
                                        <f:facet name="header"> 
                                            <a4j:commandLink id="detaghghgil" reRender="datatable" actionListener="#{viewAllUserStatsAdmin.sortColumn}">
                                                
                                                <%--     <h:commandLink style="table-header" id="name" actionListener="#{datacenterBean.sortColumn}">--%>
                                                <h:outputText value="Visit" />
                                                <f:param name="column" value="visits"/>
                                                <%--<c:if test="${requestScope.datacenterBean.sort == 'name'}" >--%>
                                                <t:graphicImage id="ac4" value="../../images/ascending-arrow.gif" rendered="#{viewAllUserStatsAdmin.visits}" border="0"/>
                                                <t:graphicImage id="de4" value="../../images/descending-arrow.gif" rendered="#{viewAllUserStatsAdmin.noVisit}" border="0"/>
                                                <%--    </c:if>--%>
                                            </a4j:commandLink>
                                            
                                        </f:facet>  
                                        <h:commandLink id="viewuserlogon" style="table-header; color: black"  action="#{viewAllUserStatsAdmin.viewUserLogon}" >                                        
                                            
                                            <h:outputText value="#{data.visits}" />                     
                                        </h:commandLink>
                                    </h:column>
                                    
                                    <h:column>
                                        <f:facet name="header"> 
                                            <a4j:commandLink id="searches" reRender="datatable" actionListener="#{viewAllUserStatsAdmin.sortColumn}">
                                                
                                                <%--     <h:commandLink style="table-header" id="name" actionListener="#{datacenterBean.sortColumn}">--%>
                                                <h:outputText value="Search" />
                                                <f:param name="column" value="searches"/>
                                                <%--<c:if test="${requestScope.datacenterBean.sort == 'name'}" >--%>
                                                <t:graphicImage id="ac3" value="../../images/ascending-arrow.gif" rendered="#{viewAllUserStatsAdmin.searches}" border="0"/>
                                                <t:graphicImage id="de3" value="../../images/descending-arrow.gif" rendered="#{viewAllUserStatsAdmin.notSearches}" border="0"/>
                                                <%--    </c:if>--%>
                                            </a4j:commandLink>
                                            
                                        </f:facet>  
                                        
                                        <h:commandLink id="viewusersearch" style="table-header; color: black"  action="#{viewAllUserStatsAdmin.viewUserSearch}" >                                        
                                            
                                            <h:outputText value="#{data.searches}" />
                                        </h:commandLink>
                                        
                                    </h:column>
                                    
                                    <h:column>
                                        <f:facet name="header"> 
                                            <a4j:commandLink id="downloads" reRender="datatable" actionListener="#{viewAllUserStatsAdmin.sortColumn}">
                                                
                                                <%--     <h:commandLink style="table-header" id="name" actionListener="#{datacenterBean.sortColumn}">--%>
                                                <h:outputText value="Download" />
                                                <f:param name="column" value="downloads"/>
                                                <%--<c:if test="${requestScope.datacenterBean.sort == 'name'}" >--%>
                                                <t:graphicImage id="ac2" value="../../images/ascending-arrow.gif" rendered="#{viewAllUserStatsAdmin.downloads}" border="0"/>
                                                <t:graphicImage id="de2" value="../../images/descending-arrow.gif" rendered="#{viewAllUserStatsAdmin.notDownloads}" border="0"/>
                                                <%--    </c:if>--%>
                                            </a4j:commandLink>
                                            
                                        </f:facet> 
                                        
                                        <h:commandLink id="viewdoanload" style="table-header; color: black"  action="#{viewAllUserStatsAdmin.viewUserDownload}" >                                        
                                            
                                            <h:outputText value="#{data.downloads}" />
                                        </h:commandLink>
                                        
                                    </h:column>
                                    
                                    <h:column>
                                        <f:facet name="header"> 
                                            <a4j:commandLink id="datarefs" reRender="datatable" actionListener="#{viewAllUserStatsAdmin.sortColumn}">
                                                
                                                <%--     <h:commandLink style="table-header" id="name" actionListener="#{datacenterBean.sortColumn}">--%>
                                                <h:outputText value="Data Refs" />
                                                <f:param name="column" value="datarefs"/>
                                                <%--<c:if test="${requestScope.datacenterBean.sort == 'name'}" >--%>
                                                <t:graphicImage id="ac1w" value="../../images/ascending-arrow.gif" rendered="#{viewAllUserStatsAdmin.dataReferences}" border="0"/>
                                                <t:graphicImage id="e1w" value="../../images/descending-arrow.gif" rendered="#{viewAllUserStatsAdmin.notDataReferences}" border="0"/>
                                                <%--    </c:if>--%>
                                            </a4j:commandLink>
                                            
                                        </f:facet>  
                                        <h:outputText value="#{data.dataReferences}" >
                                            
                                        </h:outputText>                                                
                                    </h:column>
                                    
                                    <h:column>
                                        <f:facet name="header"> 
                                            <a4j:commandLink id="bookmarks" reRender="datatable" actionListener="#{viewAllUserStatsAdmin.sortColumn}">
                                                
                                                <%--     <h:commandLink style="table-header" id="name" actionListener="#{datacenterBean.sortColumn}">--%>
                                                <h:outputText value="Bookmark" />
                                                <f:param name="column" value="bookmarks"/>
                                                <%--<c:if test="${requestScope.datacenterBean.sort == 'name'}" >--%>
                                                <t:graphicImage id="ac5" value="../../images/ascending-arrow.gif" rendered="#{viewAllUserStatsAdmin.bookmarks}" border="0"/>
                                                <t:graphicImage id="de5" value="../../images/descending-arrow.gif" rendered="#{viewAllUserStatsAdmin.notBookmarks}" border="0"/>
                                                <%--    </c:if>--%>
                                            </a4j:commandLink>
                                            
                                        </f:facet>  
                                        <h:outputText value="#{data.bookmarks}" >
                                            
                                        </h:outputText>                                                
                                    </h:column>
                                    
                                    <h:column>
                                        <f:facet name="header"> 
                                            <a4j:commandLink id="admin2" reRender="datatable" actionListener="#{viewAllUserStatsAdmin.sortColumn}">
                                                
                                                <%--     <h:commandLink style="table-header" id="name" actionListener="#{datacenterBean.sortColumn}">--%>
                                                <h:outputText value="Admin" />
                                                <f:param name="column" value="admin"/>
                                                <%--<c:if test="${requestScope.datacenterBean.sort == 'name'}" >--%>
                                                <t:graphicImage id="acd5" value="../../images/ascending-arrow.gif" rendered="#{viewAllUserStatsAdmin.admin}" border="0"/>
                                                <t:graphicImage id="ded5" value="../../images/descending-arrow.gif" rendered="#{viewAllUserStatsAdmin.notAdmin}" border="0"/>
                                                <%--    </c:if>--%>
                                            </a4j:commandLink>
                                            
                                        </f:facet>  
                                        <a4j:commandLink id="adminLink" reRender="datatable, message" actionListener="#{viewAllUserStatsAdmin.removeAdmin}">
                                            
                                            <t:graphicImage id="admin2w" value="../../images/correct.gif" height="13" width="13" rendered="#{data.admin}" border="0"/>
                                        </a4j:commandLink>
                                        <a4j:commandLink id="NotadminLink" reRender="datatable, message" actionListener="#{viewAllUserStatsAdmin.addAdmin}">
                                            
                                            <t:graphicImage id="notamdin" value="../../images/wrong.gif" height="13" width="13" rendered="#{!data.admin}" border="0"/>                                       
                                        </a4j:commandLink>
                                    </h:column>                                   
                                </t:dataTable>
                                
                                <h:panelGrid columns="1" rendered="#{viewAllUserStatsAdmin.length}" styleClass="scrollerTable2" columnClasses="standardTable_ColumnCentered" >
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
                                
                                
                                
                            </a4j:region>
                        </h:panelGrid> 
                        
                    </h:form>    
                </a4j:region>
            </td>
        </tr>
    </tbody>
</table>

