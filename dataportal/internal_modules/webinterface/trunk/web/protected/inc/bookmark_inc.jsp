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
                                <td>&nbsp;</td>
                            </tr>   
                            <tr>   
                                <td>
                                    <h:outputText rendered="#{!bookmarkBean.populated}" escape="false "value="<br />"  styleClass="info" />            
                                    <h:outputText rendered="#{!bookmarkBean.populated}" value="There are no items in bookmarks" styleClass="info" />
                                    <h:outputText rendered="#{!bookmarkBean.populated}" escape="false "value="<br /><br />"  styleClass="info" />
                                    
                                    <a4j:region   selfRendered="true" >
                                        
                                        <t:dataTable rendered="#{bookmarkBean.populated}" id="datatable" width="100%"
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
                                                    <a4j:commandLink reRender="datatable" style="table-header" id="name" actionListener="#{bookmarkBean.sortColumn}">
                                                        <h:outputText value="Name" />
                                                        <f:param name="column" value="name"/>
                                                        <t:graphicImage id="acn1" value="../../images/ascending-arrow.gif" rendered="#{bookmarkBean.name}" border="0"/>
                                                        <t:graphicImage id="den1" value="../../images/descending-arrow.gif" rendered="#{bookmarkBean.notName}" border="0"/>
                                                        
                                                    </a4j:commandLink>
                                                </f:facet>
                                                <h:outputText  value="#{data.name}" />
                                            </h:column>
                                            
                                            
                                            <h:column>
                                                <f:facet name="header">
                                                    <a4j:commandLink reRender="datatable" style="table-header" id="facility" actionListener="#{bookmarkBean.sortColumn}">
                                                        <h:outputText value="Facility" />
                                                        <f:param name="column" value="facility"/>  
                                                        <t:graphicImage id="acn2" value="../../images/ascending-arrow.gif" rendered="#{bookmarkBean.facility}" border="0"/>
                                                        <t:graphicImage id="den2" value="../../images/descending-arrow.gif" rendered="#{bookmarkBean.notFacility}" border="0"/>
                                                        
                                                    </a4j:commandLink>
                                                </f:facet>
                                                <h:outputText value="#{data.facility}" />
                                            </h:column>
                                            <h:column>
                                                <f:facet name="header">
                                                    <a4j:commandLink reRender="datatable" style="table-header" id="notes" actionListener="#{bookmarkBean.sortColumn}">
                                                        <h:outputText value="Notes" />
                                                        <f:param name="column" value="notes"/>   
                                                        <t:graphicImage id="acn3" value="../../images/ascending-arrow.gif" rendered="#{bookmarkBean.notes}" border="0"/>
                                                        <t:graphicImage id="den4" value="../../images/descending-arrow.gif" rendered="#{bookmarkBean.notNotes}" border="0"/>
                                                        
                                                    </a4j:commandLink>
                                                </f:facet>
                                                
                                                <t:popup styleClass="popup" closePopupOnExitingElement="true"
                                                         closePopupOnExitingPopup="true"
                                                         displayAtDistanceX="-165"
                                                         displayAtDistanceY="-20" >
                                                    
                                                    <t:graphicImage id="add_button"  value="../../images/addButton.gif"  border="0"/>
                                                    <f:facet name="popup">
                                                        <h:panelGroup>
                                                            <h:panelGrid columns="1" >
                                                                <h:inputTextarea value="#{data.note}" valueChangeListener="#{bookmarkBean.note}" id="note" rows="5" cols="15">
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
                                                    <t:graphicImage id="view_button" rendered="#{data.hasNote}" value="../../images/toggle_view_s.gif" style="cursor: default" border="0"/>
                                                    
                                                    <f:facet name="popup">
                                                        <h:panelGroup>
                                                            <h:panelGrid columns="1" >
                                                                <table width="150" >
                                                                    <tr>
                                                                        <td nowrap="false" width="150">
                                                                            <h:outputText value="#{data.note}" />
                                                                        </td>
                                                                    </tr>
                                                                </table>
                                                            </h:panelGrid>
                                                        </h:panelGroup>
                                                    </f:facet>
                                                </t:popup>
                                                &nbsp;
                                            </h:column>
                                            
                                            <h:column>
                                                <f:facet name="header">                                                
                                                    <a4j:commandLink reRender="datatable" style="table-header" id="time" actionListener="#{bookmarkBean.sortColumn}">
                                                        <h:outputText value="Time" />
                                                        <f:param name="column" value="time"/>   
                                                        <t:graphicImage id="act" value="../../images/ascending-arrow.gif" rendered="#{bookmarkBean.time}" border="0"/>
                                                        <t:graphicImage id="det" value="../../images/descending-arrow.gif" rendered="#{bookmarkBean.notTime}" border="0"/>
                                                        
                                                    </a4j:commandLink>
                                                </f:facet>
                                                <h:outputText value="#{data.modTime}" >
                                                    <f:convertDateTime pattern="HH:mm  dd.MM.yyyy "/>
                                                </h:outputText>
                                            </h:column>  
                                            <h:column>
                                                
                                                <f:facet name="header">
                                                    <h:outputText value="Goto" style="table-header; color:blue"/>
                                                    
                                                </f:facet>
                                                
                                                
                                                <t:popup styleClass="popup"  closePopupOnExitingElement="true"
                                                         closePopupOnExitingPopup="true"
                                                         displayAtDistanceX="10"
                                                         displayAtDistanceY="-30" >
                                                    
                                                    <h:commandLink style="table-header" id="goto" action="#{bookmarkBean.viewData}">
                                                        <t:graphicImage id="goto_button"  value="../../images/goto.gif"  border="0"/>
                                                        
                                                    </h:commandLink> 
                                                    <f:facet name="popup">
                                                        <h:panelGrid columns="1" >
                                                            <h:commandLink action="#{bookmarkBean.viewData}" title="View investigation" value="Investigation"/>
                                                            <h:commandLink action="#{bookmarkBean.viewDataSets}" title="View datasets" value="Datasets"/>                                                            
                                                        </h:panelGrid>
                                                    </f:facet>
                                                </t:popup>
                                                
                                                
                                                
                                            </h:column>
                                            <h:column>
                                                <f:facet name="header">
                                                    <h:panelGrid columns="1">  
                                                        
                                                        <t:graphicImage id="delete_selected"  value="../../images/delete.jpg"  border="0"/>                                                                            
                                                        
                                                    </h:panelGrid>           
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
                                            
                                            
                                            
                                            <%-- <h:commandButton action="#{bookmarkBean.removeBookmarks}" title="View selections" value="Delete selections"/>--&>
                                        <%-- Need panelGrid as need to check to render (cannot use HTML table, so no other waay of gettting butotn to the right??? --%>
                                            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                            
                                            <h:commandButton action="#{bookmarkBean.removeBookmarks}" title="Delete selections" value="Delete selections"/>
                                            
                                            <%-- <a4j:commandButton action="#{bookmarkBean.removeBookmarks}" reRender="datatable" title="Delete selections" value="Delete selections"/>--%>
                                            &nbsp;
                                            
                                            <t:graphicImage id="delete_selected"  value="../../images/delete.jpg"  border="0"/>    
                                            
                                            
                                        </h:panelGrid>
                                        
                                    </a4j:region>
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
                </td>
            </tr>
        </tbody>   
    </table>
</h:form>
