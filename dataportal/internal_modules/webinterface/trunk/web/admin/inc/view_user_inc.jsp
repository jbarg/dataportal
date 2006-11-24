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
                            <tr>   
                                <td><h:panelGrid columns="6">
                                        <h:outputText value="Visits:  " />
                                        <h:outputText value="#{visit.adminData.visits}" />
                                        <h:outputText value="Downloads:  " />
                                        <h:outputText value="#{visit.adminData.downloads}" />
                                        <h:outputText value="Searches:  " />
                                        <h:outputText value="#{visit.adminData.searches}" />
                                    </h:panelGrid>
                                </td>
                            </tr>   
                            <tr>   
                                <td>                                    
                                    
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
                                                <f:facet name="header"> 
                                                    <a4j:commandLink id="ff444ff4" reRender="datatable" actionListener="#{viewUserAdmin.sortColumn}">
                                                        
                                                        <h:outputText value="Event Type" />
                                                        <f:param name="column" value="event"/>
                                                        
                                                        <t:graphicImage id="w3" value="../../images/ascending-arrow.gif" rendered="#{viewUserAdmin.event}" border="0"/>
                                                        <t:graphicImage id="w34" value="../../images/descending-arrow.gif" rendered="#{viewUserAdmin.notEvent}" border="0"/>
                                                        
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
                                                        <t:graphicImage id="ac3" value="../../images/ascending-arrow.gif" rendered="#{viewUserAdmin.details}" border="0"/>
                                                        <t:graphicImage id="de3" value="../../images/descending-arrow.gif" rendered="#{viewUserAdmin.notDetails}" border="0"/>
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
                                                        <t:graphicImage id="ac1" value="../../images/ascending-arrow.gif" rendered="#{viewUserAdmin.time}" border="0"/>
                                                        <t:graphicImage id="de1" value="../../images/descending-arrow.gif" rendered="#{viewUserAdmin.notTime}" border="0"/>
                                                        <%--    </c:if>--%>
                                                    </a4j:commandLink>
                                                    
                                                </f:facet>  
                                                <h:outputText value="#{data.modTime}" >
                                                    <f:convertDateTime pattern="HH:mm  dd.MM.yyyy "/>
                                                </h:outputText>                                                
                                            </h:column>
                                            
                                            
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
                                        
                                        
                                        
                                    </a4j:region>
                                    
                                    <%--   <h:panelGrid columns="2">
                                        <h:outputText value="Visits" />
                                        
                                        <h:outputText value="Downloads:" />
                                        
                                        <h:outputText value="Searches:" />
                                        
                                    </h:panelGrid>
                                    --%>
                                </td>
                            </tr>
                        </tbody>     
                    </table>
                </td>
            </tr>
        </tbody>   
    </table>
</h:form>