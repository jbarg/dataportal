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
                                    <h:outputText rendered="#{!otherdatacenterBean.populated}" escape="false "value="<br />"  styleClass="info" />            
                                    <h:outputText rendered="#{!otherdatacenterBean.populated}" value="There are no items in #{visit.visitData.currentUserAuthDN}'s data references" styleClass="info" />
                                    <h:outputText rendered="#{!otherdatacenterBean.populated}" escape="false "value="<br /><br />"  styleClass="info" />
                                    
                                    <a4j:region   selfRendered="true" > 
                                        
                                        <t:dataTable rendered="#{otherdatacenterBean.populated}" id="datatable" width="100%"
                                                     styleClass="scrollerTable"
                                                     headerClass="standardTable_Header"
                                                     footerClass="standardTable_Header"
                                                     rowClasses="standardTable_Row1,standardTable_Row2"
                                                     columnClasses="standardTable_Column,standardTable_Column,standardTable_ColumnCentered, standardTable_ColumnCentered, standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered"
                                                     var="data"
                                                     value="#{otherdatacenterBean.dataRefs}"
                                                     preserveDataModel="true"
                                                     rows="#{visit.userPreferences.resultsPerPage}"
                                                     rowId="#{data.id}"
                                                     binding="#{otherdatacenterBean.table}"
                                                     sortColumn="#{otherdatacenterBean.sort}"
                                                     sortAscending="#{otherdatacenterBean.ascending}"
                                                     preserveSort="true" 
                                                     varDetailToggler="detailToggler">                                
                                            
                                            <f:facet name="header">
                                                <h:outputText value="#{visit.visitData.currentUserAuthDN}'s Data References" />
                                            </f:facet>
                                            
                                            <h:column>
                                                <f:facet name="header"></f:facet>  
                                                <a4j:commandLink  reRender="datatable" rendered="#{data.dataset && !data.datasetInFolder}" action="#{detailToggler.toggleDetail}">
                                                    <%--<h:commandLink rendered="#{data.dataset}" action="#{detailToggler.toggleDetail}">--%>
                                                    <h:panelGroup>
                                                        <t:graphicImage id="down" value="../../images/button_plus1.gif" rendered="#{!detailToggler.currentDetailExpanded}" border="0"/>
                                                        <t:graphicImage id="down-f" value="../../images/blue-folder-closed.png" rendered="#{!detailToggler.currentDetailExpanded}" border="0"/>
                                                        
                                                        <t:graphicImage id="up" value="../../images/button_minus1.gif" rendered="#{detailToggler.currentDetailExpanded}" border="0"/>
                                                        <t:graphicImage id="up-f" value="../../images/blue-folder-open.png" rendered="#{detailToggler.currentDetailExpanded}" border="0"/>
                                                    </h:panelGroup>
                                                </a4j:commandLink> 
                                                
                                                <%--   <h:panelGrid columns="4" rendered="#{data.dataset && data.datasetInFolder}"> 
                                                <h:commandLink  rendered="#{data.dataset && data.datasetInFolder}" disabled="true" style="color:black" id="view-1">
                                                    --%>
                                                <%--<h:commandLink rendered="#{data.dataset}" action="#{detailToggler.toggleDetail}">--%>
                                                <%-- <t:graphicImage id="down-1" value="../../images/button_plus1.gif" rendered="#{!detailToggler.currentDetailExpanded}" border="0"/>
                                                    <t:graphicImage id="down-f-1" value="../../images/blue-folder-closed.png" rendered="#{!detailToggler.currentDetailExpanded}" border="0"/>--%>
                                                <h:panelGroup rendered="#{data.dataset && data.datasetInFolder}">
                                                    <t:graphicImage id="up-1" value="../../images/button_minus1.gif"  border="0"/>
                                                    <t:graphicImage id="up-f-1" value="../../images/blue-folder-open.png"  border="0"/>
                                                </h:panelGroup>
                                                <%--  </h:commandLink>--%>
                                                
                                                <h:panelGrid columns="4" rendered="#{!data.dataset}">
                                                    
                                                    <h:outputText value="                  " />
                                                    <h:outputText value="                  " />
                                                    <h:outputText value="                  " />
                                                    
                                                    <h:commandLink  rendered="#{!data.dataset}" onclick="download('#{data.facility}-#{data.id}','#{data.typeOfReference}_IMAGEJ','DATA_CENTER'); return false;" style="color:black" id="view">                                                        
                                                        <t:graphicImage value="../../images/document.png" border="0"/>                                   
                                                    </h:commandLink>
                                                    
                                                </h:panelGrid>
                                                
                                            </h:column>
                                            <h:column>
                                                <f:facet name="header">
                                                    <a4j:commandLink id="name" reRender="datatable" actionListener="#{otherdatacenterBean.sortColumn}">
                                                        
                                                        <%--     <h:commandLink style="table-header" id="name" actionListener="#{datacenterBean.sortColumn}">--%>
                                                        <h:outputText value="Name" />
                                                        <f:param name="column" value="name"/>
                                                        <%--<c:if test="${requestScope.datacenterBean.sort == 'name'}" >--%>
                                                        <t:graphicImage id="ac1" value="../../images/ascending-arrow.gif" rendered="#{otherdatacenterBean.name}" border="0"/>
                                                        <t:graphicImage id="de1" value="../../images/descending-arrow.gif" rendered="#{otherdatacenterBean.notName}" border="0"/>
                                                        <%--    </c:if>--%>
                                                    </a4j:commandLink>
                                                    
                                                </f:facet>
                                                
                                                <h:panelGrid columns="3">   
                                                    <h:commandLink onclick="download('#{data.dpId}','#{data.typeOfReference}','DATA_CENTER'); return false;" style="color:black" id="downloadname" actionListener="#{datacenterBean.download}">
                                                        <h:outputText  value="#{data.name}" />
                                                        <f:param name="id" value="#{data.id}"/>               
                                                    </h:commandLink>      
                                                    <h:outputText escape="true" value="   " />
                                                    
                                                    
                                                    <h:selectBooleanCheckbox rendered="#{!data.dataset}"  value="#{data.download}" style="background-color:#D1E4E4" title="Add to download"  immediate="true" >                                                    
                                                        <a4j:support event="onclick" ajaxSingle="true" immediate="true" actionListener="#{otherdatacenterBean.setDataFileDownloadAction}">
                                                            <a4j:actionparam name="datafiles" value="#{data.dpId}" />
                                                            <a4j:actionparam name="DATA_FILE"  />
                                                        </a4j:support>
                                                    </h:selectBooleanCheckbox>
                                                </h:panelGrid>
                                            </h:column>
                                            <h:column>
                                                <f:facet name="header">
                                                    <a4j:commandLink reRender="datatable" style="table-header" id="type" actionListener="#{otherdatacenterBean.sortColumn}">
                                                        <%--<h:commandLink style="table-header" id="type" actionListener="#{datacenterBean.sortColumn}">--%>
                                                        <h:outputText value="Type" />
                                                        <f:param name="column" value="type"/>
                                                        <%-- <c:if test="${requestScope.datacenterBean.sort == 'type'}" >--%>
                                                        <t:graphicImage id="acty" value="../../images/ascending-arrow.gif" rendered="#{otherdatacenterBean.type}" border="0"/>
                                                        <t:graphicImage id="dety" value="../../images/descending-arrow.gif" rendered="#{otherdatacenterBean.notType}" border="0"/>
                                                        <%--   </c:if>--%>
               
                                                    </a4j:commandLink>
                                                </f:facet>
                                                
                                                
                                                <t:popup rendered="#{data.dataset}" styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                                                         closePopupOnExitingPopup="true"
                                                         displayAtDistanceX="20"
                                                         displayAtDistanceY="-40" >
                                                    <h:outputText  value="#{data.typeOfObject}" />
                                                    
                                                    <f:facet name="popup">
                                                        <h:panelGroup>
                                                            <h:panelGrid columns="1" >
                                                                <h:outputText value="List of files: (#{data.numberOfFiles})"/> 
                                                                
                                                                <h:outputText escape="false" value="#{data.printURLS}" />                                
                                                            </h:panelGrid>
                                                        </h:panelGroup>
                                                    </f:facet>
                                                </t:popup>
                                                <%-- <h:outputText rendered="#{!data.dataset}" value="#{data.typeOfReference}" />--%>
            
                                            </h:column>
                                            
                                            <h:column>
                                                <f:facet name="header">
                                                    <a4j:commandLink reRender="datatable" style="table-header" id="Facility" actionListener="#{otherdatacenterBean.sortColumn}">
                                                        <h:outputText value="Facility" />
                                                        <f:param name="column" value="facility"/>     
                                                        <t:graphicImage id="acf" value="../../images/ascending-arrow.gif" rendered="#{otherdatacenterBean.facility}" border="0"/>
                                                        <t:graphicImage id="def" value="../../images/descending-arrow.gif" rendered="#{otherdatacenterBean.notFacility}" border="0"/>
                                                        
                                                    </a4j:commandLink>
                                                </f:facet>
                                                <h:outputText value="#{data.facility}" />
                                            </h:column>        
                                            
                                            <h:column>
                                                <f:facet name="header">
                                                    <a4j:commandLink reRender="datatable" style="table-header" id="notes" actionListener="#{otherdatacenterBean.sortColumn}">
                                                        <h:outputText value="Notes" />
                                                        <f:param name="column" value="notes"/>    
                                                        <t:graphicImage id="acn" value="../../images/ascending-arrow.gif" rendered="#{otherdatacenterBean.notes}" border="0"/>
                                                        <t:graphicImage id="den" value="../../images/descending-arrow.gif" rendered="#{otherdatacenterBean.notNotes}" border="0"/>
                                                        
                                                    </a4j:commandLink>
                                                </f:facet>
                                                
                                                
                                                <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                                                         closePopupOnExitingPopup="true"
                                                         displayAtDistanceX="5"
                                                         displayAtDistanceY="-40" rendered="#{data.hasNote}">
                                                    <t:graphicImage id="view_button" rendered="#{data.hasNote}" value="../../images/toggle_view_s.gif"  border="0"/>
                                                    
                                                    <%-- <h:outputText  rendered="#{data.hasNote}" style="table-header; color : blue; font-size: 10px" value="view"/>--%>
                      
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
                                                
                                            </h:column>
                                            <h:column>
                                                <f:facet name="header">
                                                    <a4j:commandLink reRender="datatable" style="table-header" id="Time" actionListener="#{otherdatacenterBean.sortColumn}">
                                                        <h:outputText value="Time" />
                                                        <f:param name="column" value="time"/>   
                                                        
                                                        <t:graphicImage id="act" value="../../images/ascending-arrow.gif" rendered="#{otherdatacenterBean.time}" border="0"/>
                                                        <t:graphicImage id="det" value="../../images/descending-arrow.gif" rendered="#{otherdatacenterBean.notTime}" border="0"/>
                                                        
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
                                                <%-- <t:graphicImage id="goto_button"  value="../../images/goto.gif"  border="0"/>                               --%>
                  
                                                <h:commandLink style="table-header" id="view2" action="#{otherdatacenterBean.viewData}" rendered="#{data.dataset}">
                                                    <t:graphicImage id="goto_button"  value="../../images/goto.gif"  border="0"/>                               
                                                </h:commandLink>           
                                            </h:column>
                                            
                                            
                                            <f:facet name="detailStamp">
                                                
                                                <t:dataTable preserveSort="true" rendered="#{data.dataset}" width="95%" id="cities" styleClass="standardTable_Column" var="url" value="#{data.urls}">
                                                    <h:column>
                                                        &nbsp;&nbsp; &nbsp;&nbsp;
                                                    </h:column>    
                                                    <h:column>
                                                        
                                                        <t:graphicImage rendered="#{!url.imageJ}" id="doc_button_not"  value="../../images/document2.PNG"  border="0"/>
                                                        
                                                        <h:commandLink rendered="#{url.imageJ}" onclick="download('#{url.dpId}','DATA_FILE_IMAGEJ','DATA_CENTER'); return false;" style="color:black" id="view">
                                                            <t:graphicImage id="doc_button"  value="../../images/document.png"  border="0"/>
                                                            
                                                        </h:commandLink>
                                                        
                                                        <h:commandLink onclick="download('#{url.dpId}','DATA_FILE','DATA_CENTER'); return false;" style="color:black" id="downloadname" actionListener="#{datacenterBean.download}">
                                                            <h:outputText  value="#{url.name}" style="font-size: 10px"/>                         
                                                        </h:commandLink>
                                                        <h:outputText escape="false" value=" &nbsp;&nbsp;&nbsp;" />
                                                        
                                                        <h:selectBooleanCheckbox style="background-color:#D1E4E4" title="Add to download" value="#{url.download}" immediate="true" >
                                                            <f:param name="datafiles" value="#{url.dpId}"/>
                                                            <a4j:support event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{datacenterBean.setDataFileDownloadAction}">
                                                                <a4j:actionparam name="datafiles"  value="#{url.dpId}" />                                                                
                                                                
                                                            </a4j:support>
                                                        </h:selectBooleanCheckbox>
                                                    </h:column>               
                                                </t:dataTable>
                                                
                                            </f:facet>
                                        </t:dataTable>
                                        
                                        
                                        <h:panelGrid columns="1"  rendered="#{otherdatacenterBean.length}" styleClass="scrollerTable2" columnClasses="standardTable_ColumnCentered" >
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
                                        <h:panelGrid  rendered="#{datacenterBean.populated}" width="95%" columns="8">
                                            
                                            <h:commandButton onclick="download('#{node.identifier}','DOWNLOAD_MULTIPLE','DATA_CENTER'); return false;" action="#{datasetTree.select}" title="Download selections" value="Download selections"/>
                                            &nbsp;
                                            <h:selectBooleanCheckbox disabled="true" style="background-color:#D1E4E4" title="select_investigation" >
                                                
                                            </h:selectBooleanCheckbox> 
                                            <h:panelGroup/>
                                            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                            
                                            <h:panelGroup/>
                                            <h:panelGroup/>                                                                          
                                            
                                         
                                        </h:panelGrid>
                                    </a4j:region>
                                </td>
                            </tr>
                        </tbody>     
                    </table>
                </td>
            </tr>
        </tbody>   
    </table>
</h:form>