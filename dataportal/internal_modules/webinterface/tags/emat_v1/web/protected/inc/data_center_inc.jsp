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
                    <a4j:region   selfRendered="true" > 
                        <table style="margin-top:-20px" width="100%" border="0">
                             <tbody>
                                <tr>   
                                    <td>    <h:messages id="message" globalOnly="true" warnClass="info" errorClass="error" infoClass="info" /></td>
                                </tr>                                
                                <tr>   
                                    <td>&nbsp;</td>
                                </tr>   
                                <tr>   
                                    <td>
                                        
                                        <h:outputText rendered="#{!datacenterBean.populated}" escape="false "value="<br />"  styleClass="info" />            
                                        <h:outputText rendered="#{!datacenterBean.populated}" value="There are no items in data center" styleClass="info" />
                                        <h:outputText rendered="#{!datacenterBean.populated}" escape="false "value="<br /><br />"  styleClass="info" />
                                        
                                        
                                        
                                        <t:dataTable rendered="#{datacenterBean.populated}" id="datatable" width="100%"
                                                     styleClass="scrollerTable"
                                                     headerClass="standardTable_Header"
                                                     footerClass="standardTable_Header"
                                                     rowClasses="standardTable_Row1,standardTable_Row2"
                                                     columnClasses="standardTable_Column,standardTable_Column,standardTable_ColumnCentered, standardTable_ColumnCentered, standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered"
                                                     var="data"
                                                     value="#{datacenterBean.dataRefs}"
                                                     preserveDataModel="true"
                                                     rows="#{visit.userPreferences.resultsPerPage}"
                                                     rowId="#{data.id}"
                                                     binding="#{datacenterBean.table}"
                                                     sortColumn="#{datacenterBean.sort}"
                                                     sortAscending="#{datacenterBean.ascending}"
                                                     preserveSort="true" 
                                                     varDetailToggler="detailToggler">                                
                                            <f:facet name="header">
                                                <h:outputText value="Data References" />
                                            </f:facet>          
                                            <%-- <f:facet name="header">
                                            <h:outputText value="Data References" />
                                            </f:facet>--%>

                                            <h:column>
                                                <f:facet name="header"> 
                                                    <h:outputText escape="false" value="&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;" />
                                                </f:facet>  
                                                <a4j:commandLink  reRender="datatable" rendered="#{data.dataset}" action="#{detailToggler.toggleDetail}">
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
                                                <%--    <h:panelGroup rendered="#{data.dataset && data.datasetInFolder}"> --%>
                                                <h:panelGroup rendered="false">
                                                    <t:graphicImage id="up-1" value="../../images/button_minus1.gif"  border="0"/>
                                                    <t:graphicImage id="up-f-1" value="../../images/blue-folder-open.png"  border="0"/>
                                                </h:panelGroup>
                                                <%--  </h:commandLink>--%>
                                                
                                                <h:panelGrid columns="4" rendered="#{!data.dataset}">
                                                    
                                                    <h:outputText value="                  " />
                                                    <h:outputText value="                  " />
                                                    <h:outputText value="                  " />
                                                    
                                                    <t:graphicImage rendered="#{!data.imageJ && !data.datafileInFolder}" id="doc_button_not"  value="../../images/document2.PNG"  border="0"/>
                                                    <t:graphicImage id="doc_button2" rendered="#{data.datafileInFolder}" value="../../images/blue-folder-closed.png"  border="0"/>
                                                    
                                                    
                                                    <h:commandLink title="Download via ImageJ" rendered="#{data.imageJ}" onclick="download('#{data.facility}-#{data.id}','#{data.typeOfReference}_IMAGEJ','DATA_CENTER'); return false;" style="color:black" id="view">                                                        
                                                        <t:graphicImage title="Download via ImageJ" value="../../images/document.png" border="0"/>                                   
                                                    </h:commandLink>
                                                    
                                                </h:panelGrid>
                                                
                                            </h:column>
                                            <h:column>
                                                <f:facet name="header">
                                                    <a4j:commandLink id="name" reRender="datatable" actionListener="#{datacenterBean.sortColumn}">
                                                        
                                                        <%--     <h:commandLink style="table-header" id="name" actionListener="#{datacenterBean.sortColumn}">--%>
                                                        <h:outputText value="Name" />
                                                        <f:param name="column" value="name"/>
                                                        <%--<c:if test="${requestScope.datacenterBean.sort == 'name'}" >--%>
                                                        <t:graphicImage id="ac1" value="../../images/ascending-arrow.gif" rendered="#{datacenterBean.name}" border="0"/>
                                                        <t:graphicImage id="de1" value="../../images/descending-arrow.gif" rendered="#{datacenterBean.notName}" border="0"/>
                                                        <%--    </c:if>--%>
                                                    </a4j:commandLink>
                                                    
                                                </f:facet>
                                                
                                                <h:panelGrid columns="3">   
                                                    <h:commandLink onclick="download('#{data.dpId}','#{data.typeOfReference}','DATA_CENTER'); return false;" style="color:black" id="downloadname" actionListener="#{datacenterBean.download}">
                                                        <h:outputText  value="#{data.name}" />
                                                        <f:param name="id" value="#{data.id}"/>               
                                                    </h:commandLink>      
                                                    <h:outputText escape="true" value="   " />
                                                    
                                                    
                                                    <%--<h:selectBooleanCheckbox rendered="#{!data.dataset}"  value="#{data.download}" style="background-color:#D1E4E4" title="Add to download"  immediate="true" >                                                    
                                                        <a4j:support reRender="message, datatable" event="onclick" ajaxSingle="true" immediate="true" actionListener="#{datacenterBean.setDataFileDownloadAction}">
                                                            <a4j:actionparam name="datafiles" value="#{data.dpId}" />
                                                            <a4j:actionparam name="DATA_FILE"  />
                                                        </a4j:support>
                                                    </h:selectBooleanCheckbox>--%>
                                                </h:panelGrid>
                                            </h:column>
                                            
                                            <h:column>
                                                <f:facet name="header">
                                                    <t:popup styleClass="popup" style="font-size: 14px; cursor: default" closePopupOnExitingElement="true"
                                                             closePopupOnExitingPopup="true"
                                                             displayAtDistanceX="10"
                                                             displayAtDistanceY="-40" >
                                                        <t:graphicImage id="adownloads1" value="../../images/download.gif"  border="0"/>
                                                        <f:facet name="popup">
                                                            <h:panelGroup>
                                                                <h:panelGrid columns="1" >
                                                                    <h:outputText value="If you specify an email address, you can have the data downloaded link emailed to you."/>    
                                                                    <h:outputText value="To do this, add your email to the bottom left preferences tab."/>    
                                                                    
                                                                </h:panelGrid>
                                                            </h:panelGroup>
                                                        </f:facet>
                                                    </t:popup>
                                                    
                                                    
                                                    
                                                </f:facet>
                                                <h:selectBooleanCheckbox  value="#{data.download}" style="background-color:#D1E4E4" title="Add to download"  immediate="true" >                                                    
                                                    <a4j:support reRender="message,downloademail,downloadnow, notdownloadnow" event="onclick" ajaxSingle="true" immediate="true" actionListener="#{datacenterBean.setDataFileDownloadAction}">
                                                        <a4j:actionparam name="datafiles" value="#{data.dpId}" />
                                                        <a4j:actionparam name="DATA_FILE"  />
                                                    </a4j:support>
                                                </h:selectBooleanCheckbox>
                                            </h:column>
                                            
                                            <h:column>
                                                <f:facet name="header">
                                                    <a4j:commandLink reRender="datatable" style="table-header" id="type" actionListener="#{datacenterBean.sortColumn}">
                                                        <%--<h:commandLink style="table-header" id="type" actionListener="#{datacenterBean.sortColumn}">--%>
                                                        <h:outputText value="Type" />
                                                        <f:param name="column" value="type"/>
                                                        <%-- <c:if test="${requestScope.datacenterBean.sort == 'type'}" >--%>
                                                        <t:graphicImage id="acty" value="../../images/ascending-arrow.gif" rendered="#{datacenterBean.type}" border="0"/>
                                                        <t:graphicImage id="dety" value="../../images/descending-arrow.gif" rendered="#{datacenterBean.notType}" border="0"/>
                                                        <%--   </c:if>--%>               
                                                    </a4j:commandLink>
                                                </f:facet>
                                                
                                                <t:popup rendered="#{data.dataset}"  styleClass="popup" style="font-size: 14px; cursor: default" closePopupOnExitingElement="true"
                                                         closePopupOnExitingPopup="true"
                                                         displayAtDistanceX="20"
                                                         displayAtDistanceY="-40" >
                                                    <h:outputText style="cursor: default" value="#{fn:toLowerCase(data.typeOfObject)}" />
                                                    
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
                                                    <a4j:commandLink reRender="datatable" style="table-header" id="Facility" actionListener="#{datacenterBean.sortColumn}">
                                                        <h:outputText value="Facility" />
                                                        <f:param name="column" value="facility"/>     
                                                        <t:graphicImage id="acf" value="../../images/ascending-arrow.gif" rendered="#{datacenterBean.facility}" border="0"/>
                                                        <t:graphicImage id="def" value="../../images/descending-arrow.gif" rendered="#{datacenterBean.notFacility}" border="0"/>
                                                        
                                                    </a4j:commandLink>
                                                </f:facet>
                                                <h:outputText value="#{data.facility}" />
                                            </h:column>        
                                            
                                            <h:column>
                                                <f:facet name="header">
                                                    <a4j:commandLink reRender="datatable" style="table-header" id="notes" actionListener="#{datacenterBean.sortColumn}">
                                                        <h:outputText value="Notes" />
                                                        <f:param name="column" value="notes"/>    
                                                        <t:graphicImage id="acn" value="../../images/ascending-arrow.gif" rendered="#{datacenterBean.notes}" border="0"/>
                                                        <t:graphicImage id="den" value="../../images/descending-arrow.gif" rendered="#{datacenterBean.notNotes}" border="0"/>                                                        
                                                    </a4j:commandLink>
                                                </f:facet>
                                                
                                                <t:popup styleClass="popup" closePopupOnExitingElement="true"
                                                         closePopupOnExitingPopup="true"
                                                         displayAtDistanceX="-145"
                                                         displayAtDistanceY="-20" >                                                    
                                                    <t:graphicImage id="add_button"  value="../../images/addButton.gif"  border="0"/>
                                                    <f:facet name="popup">
                                                        <h:panelGroup>
                                                            <h:panelGrid columns="1" >
                                                                <h:inputTextarea styleClass="text" value="#{data.note}" valueChangeListener="#{datacenterBean.note}"  id="note" rows="5" cols="17">
                                                                    <f:param name="note" value="#{data.id}"/>
                                                                </h:inputTextarea>
                                                                
                                                                <h:commandButton  styleClass="button" action="#{datacenterBean.addNote}" title="View selections" value="Add Note"/>
                                                                
                                                            </h:panelGrid>
                                                        </h:panelGroup>
                                                    </f:facet>
                                                </t:popup>
                                                &nbsp;&nbsp;
                                                
                                                <t:popup styleClass="popup" style="font-size: 14px ; cursor: default;" closePopupOnExitingElement="true"
                                                         closePopupOnExitingPopup="true"
                                                         displayAtDistanceX="5"
                                                         displayAtDistanceY="-40" rendered="#{data.hasNote}">
                                                    <t:graphicImage style="cursor: default" id="view_button" rendered="#{data.hasNote}" value="../../images/toggle_view_s.gif"  border="0"/>
                                                    
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
                                                    <a4j:commandLink reRender="datatable" style="table-header" id="Time" actionListener="#{datacenterBean.sortColumn}">
                                                        <h:outputText value="Time" />
                                                        <f:param name="column" value="time"/>   
                                                        
                                                        <t:graphicImage id="act" value="../../images/ascending-arrow.gif" rendered="#{datacenterBean.time}" border="0"/>
                                                        <t:graphicImage id="det" value="../../images/descending-arrow.gif" rendered="#{datacenterBean.notTime}" border="0"/>
                                                        
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
                  
                                                
                                                <t:popup styleClass="popup"  closePopupOnExitingElement="true"
                                                         closePopupOnExitingPopup="true"
                                                         displayAtDistanceX="10"
                                                         displayAtDistanceY="-30" >
                                                    
                                                    <h:commandLink style="table-header" id="view2" action="#{datacenterBean.viewData}" rendered="#{data.dataset}">
                                                        <t:graphicImage id="goto_button"  value="../../images/goto.gif"  border="0"/>                               
                                                    </h:commandLink>    
                                                    
                                                    <f:facet name="popup">
                                                        <h:panelGrid columns="1" >
                                                            <h:commandLink action="#{datacenterBean.viewData}" title="View investigation" value="Investigation"/>
                                                            <h:commandLink action="#{datacenterBean.viewDataSets}" title="View datasets" value="Datasets"/>                                                            
                                                        </h:panelGrid>
                                                    </f:facet>
                                                </t:popup>
                                                
                                                
                                                
                                            </h:column>
                                            <h:column>
                                                <f:facet name="header">
                                                    <h:panelGrid columns="3">  
                                                        <%--  <a4j:commandLink reRender="datatable" style="table-header" ajaxSingle="true" id="collapseAll" action="#{datacenterBean.selectnone}">           
                                                            <%-- <h:commandLink id="collapseAll" rendered="#{visit.visitData.investigationExpanded}" actionListener="#{investigationBean.collapseAll}">--%>
                                                        <%--    <t:graphicImage  id="coll" value="../../images/button_minus1.gif"  border="0"/>
                                                        </a4j:commandLink>  --%>
                                                        
                                                        <t:graphicImage id="delete_selected"  value="../../images/delete.jpg"  border="0"/>                                                                            
                                                        
                                                        <%--   <a4j:commandLink reRender="datatable" style="table-header" ajaxSingle="true" id="expandAll" action="#{datacenterBean.selectall}">           
                                                            <%-- <h:commandLink id="expandAll" rendered="#{!visit.visitData.investigationExpanded}" actionListener="#{investigationBean.expandAll}">--%>
                                                        <%--       <t:graphicImage  id="exp" value="../../images/button_plus1.gif"  border="0"/>
                                                        </a4j:commandLink> --%>
                                                    </h:panelGrid>           
                                                </f:facet>
                                                <h:selectBooleanCheckbox title="select_investigation" valueChangeListener="#{datacenterBean.listen}" value="#{data.selected}" >
                                                    <f:param name="id" value="#{data.id}"/>
                                                </h:selectBooleanCheckbox>
                                            </h:column>
                                            
                                            <f:facet name="detailStamp">
                                                
                                                <t:dataTable preserveSort="true" rendered="#{data.dataset}" width="55%" id="cities" columnClasses="standardTable_Column,standardTable_Column"
                                                             styleClass="standardTable_Column" var="url" value="#{data.urls}">
                                                    <h:column>
                                                        &nbsp;&nbsp; &nbsp;&nbsp;
                                                    </h:column>    
                                                    <h:column>                                                        
                                                        <t:graphicImage rendered="#{!url.imageJ && !data.datasetInFolder}" id="doc_button_not"  value="../../images/document2.PNG"  border="0"/>
                                                        <t:graphicImage id="doc_button2" rendered="#{data.datasetInFolder}" value="../../images/blue-folder-closed.png"  border="0"/>
                                                        
                                                        <h:commandLink rendered="#{url.imageJ}" onclick="download('#{url.dpId}','DATA_FILE_IMAGEJ','DATA_CENTER'); return false;" style="color:black" id="view">
                                                            <t:graphicImage id="doc_button"  value="../../images/document.png"  border="0"/>
                                                        </h:commandLink>
                                                        
                                                        <h:outputText escape="false" value=" &nbsp;" />
                                                        
                                                        <h:commandLink rendered="#{data.datasetInFolder}" onclick="download('#{url.dpId}','DATA_FILE_IN_FOLDERS','DATA_CENTER'); return false;" style="color:black" id="downloadname12" actionListener="#{datacenterBean.download}">
                                                            <h:outputText  value="#{url.name}" style="font-size: 10px"/>                         
                                                        </h:commandLink>
                                                        
                                                        <h:commandLink rendered="#{!data.datasetInFolder}" onclick="download('#{url.dpId}','DATA_FILE','DATA_CENTER'); return false;" style="color:black" id="downloadname" actionListener="#{datacenterBean.download}">
                                                            <h:outputText  value="#{url.name}" style="font-size: 10px"/>                         
                                                        </h:commandLink>
                                                        
                                                        <h:outputText escape="false" value=" &nbsp;&nbsp;&nbsp;" />
                                                        
                                                        <h:selectBooleanCheckbox style="background-color:#D1E4E4" title="Add to download" value="#{url.download}" immediate="true" >
                                                            <f:param name="datafiles" value="#{url.dpId}"/>
                                                            <a4j:support reRender="message, datatable,downloademail,downloadnow, notdownloadnow" event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{datacenterBean.setDataFileDownloadAction}">
                                                                <a4j:actionparam name="datafiles"  value="#{url.dpId}" />                                                                
                                                                
                                                            </a4j:support>
                                                        </h:selectBooleanCheckbox>
                                                    </h:column>   
                                                    
                                                </t:dataTable>
                                            </f:facet>                                           
                                            
                                        </t:dataTable>
                                        
                                        
                                        <h:panelGrid columns="1" rendered="#{datacenterBean.length}" styleClass="scrollerTable2" columnClasses="standardTable_ColumnCentered" >
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
                                        
                                        
                                        <br />
                                        <h:panelGrid border="0" rendered="#{datacenterBean.populated}" width="95%" columns="12">
                                            
                                            <h:commandButton styleClass="button" id="downloadnow" style="width: 130px"  onclick="download('DOWNLOAD_MULTIPLE','DOWNLOAD_MULTIPLE','DATA_CENTER'); return false;"  title="Download selections now" value="Download now"/>
                                            &nbsp;
                                            <t:graphicImage id="downlaodaci3" value="../../images/download.gif" border="0"/>
                                            
                                          
                                            <t:popup rendered="#{!visit.userPreferences.emailSet}"  styleClass="popup" style="font-size: 14px; cursor: default" closePopupOnExitingElement="true"
                                                     closePopupOnExitingPopup="true"
                                                     displayAtDistanceX="10"
                                                     displayAtDistanceY="-40" >
                                                <t:graphicImage url="../../images/help.gif" border="0" /> 
                                                <f:facet name="popup">
                                                    <h:panelGroup>
                                                        <h:panelGrid columns="1" >
                                                            <h:outputText value="If you specify an email address, you can have the data downloaded link emailed to you."/>    
                                                            <h:inputText styleClass="text" value="#{visit.userPreferences.email}"  id="email">
                                                                
                                                            </h:inputText>
                                                            
                                                            <h:commandButton styleClass="button" action="#{datacenterBean.addEmail}" title="Add Email" value="Add Email"/>
                                                            
                                                        </h:panelGrid>
                                                    </h:panelGroup>
                                                </f:facet>
                                            </t:popup>
                                            
                                            <h:panelGroup />
                                            <h:panelGroup /><h:panelGroup />
                                            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                            
                                            <h:commandButton styleClass="button" action="#{datacenterBean.removeDatasets}" title="Delete selections" value="Delete selections"/>
                                            
                                            &nbsp;
                                            
                                            <t:graphicImage id="delete_selected"  value="../../images/delete.jpg"  border="0"/>                                                                            
                                            
                                            
                                            
                                            
                                        </h:panelGrid>
                                        <h:panelGrid border="0" rendered="#{datacenterBean.populated}" width="95%" columns="13">
                                            
                                            <h:commandButton styleClass="button" id="downloademail" style="width: 130px" rendered="#{visit.userPreferences.emailSet}" actionListener="#{datacenterBean.emailDownload}" title="Download via email" value="Email download"/>
                                            &nbsp;
                                            
                                            <t:graphicImage rendered="#{visit.userPreferences.emailSet}" id="downlaodac3" value="../../images/download.gif" border="0"/>
                                            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                            &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
                                            <h:panelGroup /> <h:panelGroup /> <h:panelGroup />
                                            <h:panelGroup />
                                        </h:panelGrid>
                                    </td>
                                </tr>
                            </tbody>     
                        </table>
                    </a4j:region>
                </td>
            </tr>
        </tbody>   
    </table>
</h:form>