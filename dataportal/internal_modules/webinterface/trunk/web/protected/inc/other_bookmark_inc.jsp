<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>

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
                                <td>&nbsp;</td>
                            </tr>   
                            <tr>   
                                <td>
                                    <h:outputText rendered="#{!otherbookmarkBean.populated}" escape="false "value="<br />"  styleClass="info" />            
                                    <h:outputText rendered="#{!otherbookmarkBean.populated}" value="There are no items in #{visit.currentUserAuthDN}'s bookmarks" styleClass="info" />
                                    <h:outputText rendered="#{!otherbookmarkBean.populated}" escape="false "value="<br /><br />"  styleClass="info" />
           
   

                                    <t:dataTable rendered="#{otherbookmarkBean.populated}" id="datatable" width="100%"
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
                                            <h:outputText value="#{visit.visitData.currentUserAuthDN}'s Bookmarks" />
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
                                                    <c:if test="${requestScope.otherbookmarkBean.sort == 'name'}" >
                                                        <t:graphicImage id="acn" value="../../images/ascending-arrow.gif" rendered="#{!otherbookmarkBean.ascending}" border="0"/>
                                                        <t:graphicImage id="den" value="../../images/descending-arrow.gif" rendered="#{otherbookmarkBean.ascending}" border="0"/>
                                                    </c:if>
                                                </h:commandLink>
                                            </f:facet>
                                            <h:outputText  value="#{data.name}" />
                                        </h:column>
       
      
                                        <h:column>
                                            <f:facet name="header">
                                                <h:commandLink style="table-header" id="facility" actionListener="#{otherbookmarkBean.sortColumn}">
                                                    <h:outputText value="Facility" />
                                                    <f:param name="column" value="facility"/> 
                                                    <c:if test="${requestScope.otherbookmarkBean.sort == 'facility'}" >
                                                        <t:graphicImage id="acf" value="../../images/ascending-arrow.gif" rendered="#{!otherbookmarkBean.ascending}" border="0"/>
                                                        <t:graphicImage id="def" value="../../images/descending-arrow.gif" rendered="#{otherbookmarkBean.ascending}" border="0"/>
                                                    </c:if>
                                                </h:commandLink>
                                            </f:facet>
                                            <h:outputText value="#{data.facility}" />
                                        </h:column>
                                        <h:column>
                                            <f:facet name="header">
                                                <h:commandLink style="table-header" id="notes" actionListener="#{otherbookmarkBean.sortColumn}">
                                                    <h:outputText value="Notes" />
                                                    <f:param name="column" value="notes"/> 
                                                    <c:if test="${requestScope.otherbookmarkBean.sort == 'notes'}" >
                                                        <t:graphicImage id="acno" value="../../images/ascending-arrow.gif" rendered="#{!otherbookmarkBean.ascending}" border="0"/>
                                                        <t:graphicImage id="deno" value="../../images/descending-arrow.gif" rendered="#{otherbookmarkBean.ascending}" border="0"/>
                                                    </c:if>
                                                </h:commandLink>
                                            </f:facet>
                                       
             
                                            <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                                                closePopupOnExitingPopup="true"
                                                displayAtDistanceX="5"
                                                displayAtDistanceY="-40" rendered="#{data.hasNote}">

                                                <t:graphicImage id="view_button" rendered="#{data.hasNote}" value="../../images/toggle_view_s.gif"  border="0"/>
                                                <f:facet name="popup">
                                                    <h:panelGroup>
                                                        <h:panelGrid columns="1" >
                                                            <table width="150" >
                                                                <tbody>
                                                                    <tr>
                                                                        <td nowrap="false" width="150">
                                                                            <h:outputText value="#{data.note}" />
                                                                        </td>
                                                                    </tr>
                                                                </tbody>
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
                                                    <c:if test="${requestScope.otherbookmarkBean.sort == 'time'}" >
                                                        <t:graphicImage id="actime" value="../../images/ascending-arrow.gif" rendered="#{!otherbookmarkBean.ascending}" border="0"/>
                                                        <t:graphicImage id="detime" value="../../images/descending-arrow.gif" rendered="#{otherbookmarkBean.ascending}" border="0"/>
                                                    </c:if>
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
    
                                    <h:panelGrid columns="1" rendered="#{otherbookmarkBean.length}" styleClass="scrollerTable2" columnClasses="standardTable_ColumnCentered" >
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
                                </td>
                            </tr>
                        </tbody>     
                    </table>
                </td>
            </tr>
        </tbody>   
    </table>
</h:form>
