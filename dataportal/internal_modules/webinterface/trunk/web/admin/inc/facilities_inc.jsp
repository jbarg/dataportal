<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>
<%@ taglib uri="https://ajax4jsf.dev.java.net/ajax" prefix="a4j"%>

<table width="90%" style="margin-top:-20px">
     <tbody>
        <tr>
            
            <td>
                <h:messages id="message" globalOnly="true" errorClass="error" infoClass="info" />
                <br/>
                <a4j:region   selfRendered="true" > 
                    <h:form>
                        
                        <h:panelGrid id="newFacility" columns="3" >
                            
                            <h:outputLabel>
                                <h:outputText styleClass="body" value="Name:"/>
                            </h:outputLabel  >
                            
                            <h:inputText id="namej_id_1" size="30" required="true" value="#{facilityAdminBean.name}">
                                <f:validateLength minimum="1" maximum="16"/>
                            </h:inputText> 
                            
                            <h:message for="namej_id_1" styleClass="error" showSummary="false" showDetail="true" />
                            
                            
                            <h:outputLabel>
                                <h:outputText styleClass="body" value="Type:"/>
                            </h:outputLabel  >
                            
                            <h:selectOneRadio style="font-size:14px" id="type" value="#{facilityAdminBean.type}" >
                                <f:selectItem itemLabel="Wrapper" itemValue="dpal"/>          
                                <f:selectItem itemLabel="Access and Control" itemValue="acm"/>    
                            </h:selectOneRadio>     
                            
                            <h:panelGroup/>
                            
                            <h:outputLabel>
                                <h:outputText styleClass="body" value="Data in folders:"/>
                            </h:outputLabel  >
                            
                            <h:selectOneRadio style="font-size:14px" id="folders" value="#{facilityAdminBean.folders}" >
                                <f:selectItem itemLabel="True" itemValue="Y"/>
                                <f:selectItem itemLabel="False" itemValue="N"/>
                            </h:selectOneRadio>     
                            
                            <h:panelGroup/>  
                            
                            <h:outputLabel>
                                <h:outputText styleClass="body" value="Active:"/>
                            </h:outputLabel  >
                            
                            <h:selectOneRadio style="font-size:14px" id="active" value="#{facilityAdminBean.activeFac}" >
                                <f:selectItem itemLabel="True" itemValue="Y"/>
                                <f:selectItem itemLabel="False" itemValue="N"/>
                            </h:selectOneRadio>     
                            
                            <h:panelGroup/>  
                            
                            <h:outputLabel>
                                <h:outputText styleClass="body" value="Topics:"/>
                            </h:outputLabel  >
                            
                            <h:selectOneRadio style="font-size:14px" id="keywords" value="#{facilityAdminBean.topicsFac}" >
                                <f:selectItem itemLabel="True" itemValue="Y"/>
                                <f:selectItem itemLabel="False" itemValue="N"/>
                                
                            </h:selectOneRadio>     
                            
                            <h:panelGroup/>  
                            
                            <h:outputLabel>
                                <h:outputText styleClass="body" value="All Keywords:"/>
                            </h:outputLabel  >
                            
                            <h:selectOneRadio style="font-size:14px" id="allKeywords" value="#{facilityAdminBean.allKeywords}" >
                                <f:selectItem itemLabel="True" itemValue="Y"/>
                                <f:selectItem itemLabel="False" itemValue="N"/>
                            </h:selectOneRadio>     
                            
                            <h:panelGroup/>  
                            
                            <h:commandButton actionListener="#{facilityAdminBean.createFacility}" title="Add" value="Add"/>
                            
                            <h:panelGroup/>
                        </h:panelGrid> 
                    </h:form>
                    
                    <h:form>
                        
                        
                        
                        <t:dataTable rendered="#{facilityAdminBean.populated}" id="datatable" width="100%"
                                     styleClass="scrollerTable"
                                     headerClass="standardTable_Header"
                                     footerClass="standardTable_Header"
                                     rowClasses="standardTable_Row1,standardTable_Row2"
                                     columnClasses="standardTable_Column,standardTable_ColumnCentered,standardTable_ColumnCentered, standardTable_ColumnCentered, standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered"
                                     var="data"
                                     value="#{facilityAdminBean.dataRefs}"
                                     preserveDataModel="true"
                                     rows="#{visit.userPreferences.resultsPerPage}"
                                     rowId="#{data.id}"
                                     binding="#{facilityAdminBean.table}"
                                     sortColumn="#{facilityAdminBean.sort}"
                                     sortAscending="#{facilityAdminBean.ascending}"
                                     preserveSort="true"
                                     varDetailToggler="detailToggler">                                
                            <f:facet name="header">
                                <h:outputText value="Facilities" />
                            </f:facet>          
                            
                            <h:column>
                                <f:facet name="header"> <h:outputText style="table-header; color:blue" escape="false" value="&nbsp;Edit&nbsp;" /></f:facet>  
                                
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
                                    <a4j:commandLink id="ff444ff4" reRender="datatable" actionListener="#{facilityAdminBean.sortColumn}">
                                        
                                        <h:outputText value="Name" />
                                        <f:param name="column" value="facility"/>
                                        
                                        <t:graphicImage id="w36" value="../../images/ascending-arrow.gif" rendered="#{facilityAdminBean.facility}" border="0"/>
                                        <t:graphicImage id="w3774" value="../../images/descending-arrow.gif" rendered="#{facilityAdminBean.notFacility}" border="0"/>
                                        
                                    </a4j:commandLink>
                                    
                                </f:facet>  
                                <h:outputText value="#{data.facility}" />   
                                
                                
                            </h:column>
                            
                            <h:column>
                                <f:facet name="header"> 
                                    <a4j:commandLink id="ff4d44ff4" reRender="datatable" actionListener="#{facilityAdminBean.sortColumn}">
                                        
                                        <h:outputText value="Active" />
                                        <f:param name="column" value="active"/>
                                        
                                        <t:graphicImage id="w35" value="../../images/ascending-arrow.gif" rendered="#{facilityAdminBean.activeSort}" border="0"/>
                                        <t:graphicImage id="w354" value="../../images/descending-arrow.gif" rendered="#{facilityAdminBean.notActiveSort}" border="0"/>
                                        
                                    </a4j:commandLink>
                                    
                                </f:facet>  
                                <h:outputText value="#{data.active}" />                              
                                
                            </h:column>
                            
                            <h:column>
                                <f:facet name="header"> 
                                    <a4j:commandLink id="frd" reRender="datatable" actionListener="#{facilityAdminBean.sortColumn}">
                                        
                                        <h:outputText value="Data In Folders" />
                                        <f:param name="column" value="folders"/>
                                        
                                        <t:graphicImage id="w353" value="../../images/ascending-arrow.gif" rendered="#{facilityAdminBean.dataFolders}" border="0"/>
                                        <t:graphicImage id="w343" value="../../images/descending-arrow.gif" rendered="#{facilityAdminBean.notDataFolders}" border="0"/>
                                        
                                    </a4j:commandLink>
                                    
                                </f:facet>  
                                <h:outputText value="#{data.dataInFolders}" />                             
                                
                            </h:column>
                            
                            <h:column>
                                <f:facet name="header"> 
                                    <a4j:commandLink id="f3rd" reRender="datatable" actionListener="#{facilityAdminBean.sortColumn}">
                                        
                                        <h:outputText value="All Keywords" />
                                        <f:param name="column" value="keywords"/>
                                        
                                        <t:graphicImage id="w3353" value="../../images/ascending-arrow.gif" rendered="#{facilityAdminBean.keywordsSort}" border="0"/>
                                        <t:graphicImage id="w3343" value="../../images/descending-arrow.gif" rendered="#{facilityAdminBean.notKeywordsSort}" border="0"/>
                                        
                                    </a4j:commandLink>
                                    
                                </f:facet>  
                                <h:outputText value="#{data.allKeywords}" />                             
                                
                            </h:column>
                            
                            <h:column>
                                <f:facet name="header"> 
                                    <a4j:commandLink id="f3rfd" reRender="datatable" actionListener="#{facilityAdminBean.sortColumn}">
                                        
                                        <h:outputText value="Topics" />
                                        <f:param name="column" value="topics"/>
                                        
                                        <t:graphicImage id="w3h353" value="../../images/ascending-arrow.gif" rendered="#{facilityAdminBean.topics}" border="0"/>
                                        <t:graphicImage id="w3h343" value="../../images/descending-arrow.gif" rendered="#{facilityAdminBean.notTopics}" border="0"/>
                                        
                                    </a4j:commandLink>
                                    
                                </f:facet>  
                                <h:outputText value="#{data.topics}" />                             
                                
                            </h:column>
                            
                            <h:column>
                                <f:facet name="header">
                                    
                                    <h:outputText style="table-header; color:blue" escape="false" value="Delete" />
                                    
                                </f:facet>
                                <a4j:commandLink id="deleteLink" reRender="datatable, message" actionListener="#{facilityAdminBean.delete}">                                    
                                    <t:graphicImage id="delete_selected4"  value="../../images/delete.jpg"  border="0"/> 
                                </a4j:commandLink>
                            </h:column>
                            
                            <f:facet name="detailStamp">
                                
                                <t:dataTable  var="upDate" value="#{data}" >
                                    <h:column>
                                        <h:panelGrid id="upDateFacility" columns="3" >
                                            
                                            <h:outputLabel>
                                                <h:outputText styleClass="body" value="Name:"/>
                                            </h:outputLabel  >
                                            
                                            <h:inputText id="upDatenamej_id_1" size="30" required="false" disabled="true" value="#{upDate.facility}">
                                                <f:validateLength minimum="1" maximum="16"/>
                                            </h:inputText> 
                                            
                                            <h:message for="upDatenamej_id_1" styleClass="error" showSummary="false" showDetail="true" />
                                            
                                            
                                            <h:outputLabel>
                                                <h:outputText styleClass="body" value="Type:"/>
                                            </h:outputLabel  >
                                            
                                            <h:selectOneRadio style="font-size:14px" id="upDatetype" value="#{upDate.moduleType}" >
                                                <f:selectItem itemLabel="Wrapper" itemValue="dpal"/>          
                                                <f:selectItem itemLabel="Access and Control" itemValue="acm"/>    
                                            </h:selectOneRadio>     
                                            
                                            <h:panelGroup/>
                                            
                                            <h:outputLabel>
                                                <h:outputText styleClass="body" value="Data in folders:"/>
                                            </h:outputLabel  >
                                            
                                            <h:selectOneRadio style="font-size:14px" id="upDatefolders" value="#{upDate.dataInFolders}" >
                                                <f:selectItem itemLabel="True" itemValue="Y"/>
                                                <f:selectItem itemLabel="False" itemValue="N"/>
                                            </h:selectOneRadio>     
                                            
                                            <h:panelGroup/>  
                                            
                                            <h:outputLabel>
                                                <h:outputText styleClass="body" value="Active:"/>
                                            </h:outputLabel  >
                                            
                                            <h:selectOneRadio style="font-size:14px" id="upDateactive" value="#{upDate.active}" >
                                                <f:selectItem itemLabel="True" itemValue="Y"/>
                                                <f:selectItem itemLabel="False" itemValue="N"/>
                                            </h:selectOneRadio>     
                                            
                                            <h:panelGroup/>  
                                            
                                            <h:outputLabel>
                                                <h:outputText styleClass="body" value="Topics:"/>
                                            </h:outputLabel  >
                                            
                                            <h:selectOneRadio style="font-size:14px" id="upDatekeywords" value="#{upDate.topics}" >
                                                <f:selectItem itemLabel="True" itemValue="Y"/>
                                                <f:selectItem itemLabel="False" itemValue="N"/>
                                            </h:selectOneRadio>     
                                            
                                            <h:panelGroup/>  
                                            
                                            <h:outputLabel>
                                                <h:outputText styleClass="body" value="All Keywords:"/>
                                            </h:outputLabel  >
                                            
                                            <h:selectOneRadio style="font-size:14px" id="upDateallKeywords" value="#{upDate.allKeywords}" >
                                                <f:selectItem itemLabel="True" itemValue="Y"/>
                                                <f:selectItem itemLabel="False" itemValue="N"/>
                                            </h:selectOneRadio>     
                                            
                                            <h:panelGroup/>  
                                            
                                            <h:commandButton action="#{facilityAdminBean.upDateFacility}" title="Update" value="Update"/>
                                            
                                            <h:commandButton action="#{facilityAdminBean.refreshPage}" immediate="true" value="Cancel" title="Cancel" />
                                            
                                            <h:panelGroup/>
                                            
                                        </h:panelGrid> 
                                    </h:column>
                                </t:dataTable>
                            </f:facet>
                            
                        </t:dataTable>
                        
                        <h:panelGrid columns="1" rendered="#{facilityAdminBean.length}" styleClass="scrollerTable2" columnClasses="standardTable_ColumnCentered" >
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
                        
                    </h:form>    
                    
                </a4j:region>     
            </td>
        </tr>
    </tbody>
</table>

