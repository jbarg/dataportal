<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>

<h:form>
   
    <table width="95%">
        <td width="20">&nbsp;</td>
        <td>
            <table width="95%" border="0">
                <tr> <h:messages globalOnly="true" errorClass="error" infoClass="info" />
                </tr>                 
        
                <tr ><br />&nbsp;</tr>             
            </table>

   

            <t:dataTable id="datatable" width="95%"
                styleClass="scrollerTable"
                headerClass="standardTable_Header"
                footerClass="standardTable_Header"
                rowClasses="standardTable_Row1,standardTable_Row2"
                columnClasses="standardTable_Column,standardTable_ColumnCentered, standardTable_ColumnCentered, standardTable_ColumnCentered,standardTable_ColumnCentered"
                var="data"
                value="#{otherbookmarkBean.dataRefs}"
                preserveDataModel="true"
                rows="#{visit.userPreferences.resultsPerPage}"
                rowId="#{data.id}"
                binding="#{otherbookmarkBean.table}"
                sortColumn="#{otherbookmarkBean.sort}"
                sortAscending="#{otherbookmarkBean.ascending}"
                preserveSort="true">
                <f:facet name="header">
                    <h:outputText value="#{visit.currentUserAuthDN}'s Bookmarks" />
                </f:facet>

               <%-- <h:column>
                    <f:facet name="header"></f:facet>            
                    <h:outputText value="#{data.id}" />              
                </h:column>--%>
                <h:column>
                    <f:facet name="header">
                        <h:commandLink style="table-header" id="name" actionListener="#{otherbookmarkBean.sortColumn}">
                            <h:outputText value="Name" />
                            <f:param name="column" value="name"/>
                        </h:commandLink>
                    </f:facet>
                    <h:outputText  value="#{data.name}" />
                </h:column>
       
      
                <h:column>
                    <f:facet name="header">
                        <h:commandLink style="table-header" id="facility" actionListener="#{otherbookmarkBean.sortColumn}">
                            <h:outputText value="Facility" />
                            <f:param name="column" value="facility"/>              
                        </h:commandLink>
                    </f:facet>
                    <h:outputText value="#{data.facility}" />
                </h:column>
                <h:column>
                    <f:facet name="header">
                        <h:commandLink style="table-header" id="notes" actionListener="#{otherbookmarkBean.sortColumn}">
                            <h:outputText value="Notes" />
                            <f:param name="column" value="notes"/>               
                        </h:commandLink>
                    </f:facet>
                    &nbsp;                   
             
                    <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                        closePopupOnExitingPopup="true"
                        displayAtDistanceX="5"
                        displayAtDistanceY="-40" rendered="#{data.hasNote}">

                       <t:graphicImage id="view_button" rendered="#{data.hasNote}" value="../../images/toggle_view_s.gif"  border="0"/>
 <f:facet name="popup">
                            <h:panelGroup>
                                <h:panelGrid columns="1" >
                                    <table width="150" >
                                        <td nowrap="false" width="150">
                                            <h:outputText value="#{data.note}" />
                                        </td>
                                    </table>
                                </h:panelGrid>
                            </h:panelGroup>
                        </f:facet>
                    </t:popup>
                    &nbsp;
                </h:column>
        
                <h:column>
                    <f:facet name="header">
                        <h:commandLink style="table-header" id="time" actionListener="#{otherbookmarkBean.sortColumn}">
                            <h:outputText value="Time" />
                            <f:param name="column" value="time"/>              
                        </h:commandLink>
                    </f:facet>
                    <h:outputText value="#{data.modTime}" >
                        <f:convertDateTime pattern="HH:mm  dd.MM.yyyy "/>
                    </h:outputText>
                </h:column>  
                <h:column>
            
                    <f:facet name="header">
                            <h:outputText value="Goto" style="table-header; color:blue"/>
                   
                    </f:facet>
                     &nbsp; &nbsp; 
                    <h:commandLink style="table-header" id="goto" action="#{otherbookmarkBean.viewData}">
                         <t:graphicImage id="goto_button"  value="../../images/goto.gif"  border="0"/>
                      
                                
                    </h:commandLink>
                </h:column>
               
            </t:dataTable>
  <c:if test="${fn:length(requestScopeScope.otherbookmarkBean.dataRefs) > sessionScope.visit.userPreferences.resultsPerPage}" >
    
            <h:panelGrid columns="1" styleClass="scrollerTable2" columnClasses="standardTable_ColumnCentered" >
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
           
  </c:if>
            
        </td>
    </table>
</h:form>
