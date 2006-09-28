<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>

<h:form>
   
    <table width="95%">
         <tbody>
            <tr>
                <td width="20">&nbsp;</td>
                <td>
                    <table width="95%" border="0">
                         <tbody>
                            <tr>
                                <tr> <h:messages globalOnly="true" errorClass="error" infoClass="info" />
                                </tr>                 
        
                                <tr ><br />&nbsp;</tr>    
                            </tr>
                        </tbody>
                    </table>
                </td>
                <td>
                    <h:outputText rendered="#{!bookmarkBean.populated}" escape="false "value="<br />"  styleClass="info" />            
                    <h:outputText rendered="#{!bookmarkBean.populated}" value="There are no items in bookmarks" styleClass="info" />
                    <h:outputText rendered="#{!bookmarkBean.populated}" escape="false "value="<br /><br />"  styleClass="info" />
            
 
                    <t:dataTable rendered="#{bookmarkBean.populated}" id="datatable" width="95%"
                        styleClass="scrollerTable"
                        headerClass="standardTable_Header"
                        footerClass="standardTable_Header"
                        rowClasses="standardTable_Row1,standardTable_Row2"
                        columnClasses="standardTable_Column,standardTable_ColumnCentered, standardTable_ColumnCentered, standardTable_ColumnCentered,standardTable_ColumnCentered"
                        var="data"
                        value="#{bookmarkBean.dataRefs}"
                        preserveDataModel="true"
                        rows="#{visit.userPreferences.resultsPerPage}"
                        rowId="#{data.id}"
                        binding="#{bookmarkBean.table}"
                        sortColumn="#{bookmarkBean.sort}"
                        sortAscending="#{bookmarkBean.ascending}"
                        preserveSort="true">
                        <f:facet name="header">
                            <h:outputText value="Bookmarks" />
                        </f:facet>

                        <%-- <h:column>
                        <f:facet name="header"></f:facet>            
                        <h:outputText value="#{data.id}" />              
                        </h:column>--%>
                        <h:column >
                            <f:facet name="header">
                                <h:commandLink style="table-header" id="name" actionListener="#{bookmarkBean.sortColumn}">
                                    <h:outputText value="Name" />
                                    <f:param name="column" value="name"/>
                                    <c:if test="${requestScope.bookmarkBean.sort == 'name'}" >
                                        <t:graphicImage id="acn" value="../../images/ascending-arrow.gif" rendered="#{!bookmarkBean.ascending}" border="0"/>
                                        <t:graphicImage id="den" value="../../images/descending-arrow.gif" rendered="#{bookmarkBean.ascending}" border="0"/>
                                    </c:if>
                                </h:commandLink>
                            </f:facet>
                            <h:outputText  value="#{data.name}" />
                        </h:column>
       
      
                        <h:column>
                            <f:facet name="header">
                                <h:commandLink style="table-header" id="facility" actionListener="#{bookmarkBean.sortColumn}">
                                    <h:outputText value="Facility" />
                                    <f:param name="column" value="facility"/>  
                                    <c:if test="${requestScope.bookmarkBean.sort == 'facility'}" >
                                        <t:graphicImage id="acf" value="../../images/ascending-arrow.gif" rendered="#{!bookmarkBean.ascending}" border="0"/>
                                        <t:graphicImage id="def" value="../../images/descending-arrow.gif" rendered="#{bookmarkBean.ascending}" border="0"/>
                                    </c:if>
                                </h:commandLink>
                            </f:facet>
                            <h:outputText value="#{data.facility}" />
                        </h:column>
                        <h:column>
                            <f:facet name="header">
                                <h:commandLink style="table-header" id="notes" actionListener="#{bookmarkBean.sortColumn}">
                                    <h:outputText value="Notes" />
                                    <f:param name="column" value="notes"/>   
                                    <c:if test="${requestScope.bookmarkBean.sort == 'notes'}" >
                                        <t:graphicImage id="acnote" value="../../images/ascending-arrow.gif" rendered="#{!bookmarkBean.ascending}" border="0"/>
                                        <t:graphicImage id="denote" value="../../images/descending-arrow.gif" rendered="#{bookmarkBean.ascending}" border="0"/>
                                    </c:if>
                                </h:commandLink>
                            </f:facet>
                       
                            <t:popup styleClass="popup" closePopupOnExitingElement="true"
                                closePopupOnExitingPopup="true"
                                displayAtDistanceX="-165"
                                displayAtDistanceY="-20" >

                                <t:graphicImage id="add_button"  value="../../images/addButton.gif"  border="0"/>
                                <f:facet name="popup">
                                    <h:panelGroup>
                                        <h:panelGrid columns="1" >
                                            <h:inputTextarea value="#{data.note}" valueChangeListener="#{bookmarkBean.note}" id="note" rows="10" cols="15">
                                                <f:param name="note" value="#{data.id}"/>
                                            </h:inputTextarea>
                                   
                                            <h:commandButton action="#{bookmarkBean.addNote}" title="View selections" value="Add Note"/>
           
                                        </h:panelGrid>
                                    </h:panelGroup>
                                </f:facet>
                            </t:popup>
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
                                <h:commandLink style="table-header" id="time" actionListener="#{bookmarkBean.sortColumn}">
                                    <h:outputText value="Time" />
                                    <f:param name="column" value="time"/>   
                                    <c:if test="${requestScope.bookmarkBean.sort == 'time'}" >
                                        <t:graphicImage id="actime" value="../../images/ascending-arrow.gif" rendered="#{!bookmarkBean.ascending}" border="0"/>
                                        <t:graphicImage id="detime" value="../../images/descending-arrow.gif" rendered="#{bookmarkBean.ascending}" border="0"/>
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
                        
                            <h:commandLink style="table-header" id="goto" action="#{bookmarkBean.viewData}">
                                <t:graphicImage id="goto_button"  value="../../images/goto.gif"  border="0"/>
                             
                            </h:commandLink>
                        </h:column>
                        <h:column>
                            <f:facet name="header">
                                <h:outputText value="" />
                            </f:facet>
                            <h:selectBooleanCheckbox title="select_investigation" valueChangeListener="#{bookmarkBean.listen}" value="#{data.selected}" >
                                <f:param name="id" value="#{data.id}"/>
                            </h:selectBooleanCheckbox>
                        </h:column>
                    </t:dataTable>
          
           
                    <h:panelGrid  rendered="#{bookmarkBean.length}" columns="1" styleClass="scrollerTable2" columnClasses="standardTable_ColumnCentered" >
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
           
                    <br />
                    <h:panelGrid  rendered="#{bookmarkBean.populated}" width="95%" columns="5">
             
                        <h:commandButton action="#{bookmarkBean.removeBookmarks}" title="View selections" value="Delete selections"/>
                        <%-- Need panelGrid as need to check to render (cannot use HTML table, so no other waay of gettting butotn to the right??? --%>
                        &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                        &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                        &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                 
                        Select:&nbsp; <h:commandButton action="#{bookmarkBean.selectall}" title="All" value=" All "/>
                        &nbsp; 
                        <h:commandButton action="#{bookmarkBean.selectnone}" title="None" value="None"/>
                 
                    </h:panelGrid>
                    <%--  <table width="95%" border="0">
                    <td>
                    <h:commandButton action="#{bookmarkBean.removeBookmarks}" title="View selections" value="Delete selections"/>
             
                    </td>
                    <td align="right">
                    Select:&nbsp; <h:commandButton action="#{bookmarkBean.selectall}" title="All" value=" All "/>
                    &nbsp; 
                    <h:commandButton action="#{bookmarkBean.selectnone}" title="None" value="None"/>
                    </td>
                    </table>--%>
                </td>
            </tr>
        </tbody>
    </table>
</h:form>
