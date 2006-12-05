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
                        
                        <h:panelGrid id="newProxy" columns="3" >
                            
                            <h:outputLabel>
                                <h:outputText styleClass="body" value="Address:"/>
                            </h:outputLabel  >
                            
                            <h:inputText id="addressnewj_id_1" size="30" required="true" value="#{myProxyServerAdminBean.proxyServerAddress}">
                                <f:validateLength minimum="5" maximum="60"/>
                            </h:inputText> 
                            
                            
                            <h:message for="addressnewj_id_1" styleClass="error" showSummary="false" showDetail="true" />
                            
                            <h:outputLabel>
                                <h:outputText styleClass="body" value="CA DN:"/>
                            </h:outputLabel  >
                            
                            <h:inputText id="canewj_id_1" size="30" required="true" value="#{myProxyServerAdminBean.caRootCertificate}">
                                <f:validateLength minimum="5" maximum="160"/>
                            </h:inputText> 
                            
                            
                            <h:message for="canewj_id_1" styleClass="error" showSummary="false" showDetail="true" />
                            
                            <h:outputLabel>
                                <h:outputText styleClass="body" value="Port:"/>
                            </h:outputLabel  >
                            
                            <h:inputText id="portnewj_id_1" size="30" required="true" value="#{myProxyServerAdminBean.portNumber}">
                                <f:validateLongRange minimum="1"  />
                            </h:inputText> 
                            
                            
                            <h:message for="portnewj_id_1" styleClass="error" showSummary="false" showDetail="true" />
                            
                            <h:commandButton actionListener="#{myProxyServerAdminBean.newMyProxyServer}" title="Add" value="Add"/>
                            
                            <h:panelGroup/>
                        </h:panelGrid> 
                    </h:form>
                    <h:form>
                        
                        
                        
                        <t:dataTable rendered="#{myProxyServerAdminBean.populated}" id="datatable" width="100%"
                                     styleClass="scrollerTable"
                                     headerClass="standardTable_Header"
                                     footerClass="standardTable_Header"
                                     rowClasses="standardTable_Row1,standardTable_Row2"
                                     columnClasses="standardTable_Column,standardTable_ColumnCentered,standardTable_ColumnCentered, standardTable_ColumnCentered, standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered"
                                     var="data"
                                     value="#{myProxyServerAdminBean.dataRefs}"
                                     preserveDataModel="true"
                                     rows="#{visit.userPreferences.resultsPerPage}"
                                     rowId="#{data.id}"
                                     binding="#{myProxyServerAdminBean.table}"
                                     sortColumn="#{myProxyServerAdminBean.sort}"
                                     sortAscending="#{myProxyServerAdminBean.ascending}"
                                     preserveSort="true"
                                     varDetailToggler="detailToggler">                                
                            <f:facet name="header">
                                <h:outputText value="MyProxy Servers" />
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
                                    <a4j:commandLink id="ff444ff4" reRender="datatable" actionListener="#{myProxyServerAdminBean.sortColumn}">
                                        
                                        <h:outputText value="Address" />
                                        <f:param name="column" value="name"/>
                                        
                                        <t:graphicImage id="w3" value="../../images/ascending-arrow.gif" rendered="#{myProxyServerAdminBean.address}" border="0"/>
                                        <t:graphicImage id="w34" value="../../images/descending-arrow.gif" rendered="#{myProxyServerAdminBean.notAddress}" border="0"/>
                                        
                                    </a4j:commandLink>
                                    
                                </f:facet>  
                                <h:outputText value="#{data.proxyServerAddress}" />   
                                
                                
                            </h:column>
                            <h:column>
                                <f:facet name="header"> 
                                    <a4j:commandLink id="detaghghgil" reRender="datatable" actionListener="#{myProxyServerAdminBean.sortColumn}">
                                        
                                        <%--     <h:commandLink style="table-header" id="name" actionListener="#{datacenterBean.sortColumn}">--%>
                                        <h:outputText value="Port" />
                                        <f:param name="column" value="port"/>
                                        <%--<c:if test="${requestScope.datacenterBean.sort == 'name'}" >--%>
                                        <t:graphicImage id="ac4" value="../../images/ascending-arrow.gif" rendered="#{myProxyServerAdminBean.port}" border="0"/>
                                        <t:graphicImage id="de4" value="../../images/descending-arrow.gif" rendered="#{myProxyServerAdminBean.notPort}" border="0"/>
                                        <%--    </c:if>--%>
                                    </a4j:commandLink>
                                    
                                </f:facet>  
                                
                                <h:outputText value="#{data.portNumber}" />                     
                                
                            </h:column>
                            
                            <h:column >
                                <f:facet name="header"> 
                                    <a4j:commandLink id="searches" reRender="datatable" actionListener="#{myProxyServerAdminBean.sortColumn}">
                                        
                                        <%--     <h:commandLink style="table-header" id="name" actionListener="#{datacenterBean.sortColumn}">--%>
                                        <h:outputText value="CA DN" />
                                        <f:param name="column" value="ca"/>
                                        <%--<c:if test="${requestScope.datacenterBean.sort == 'name'}" >--%>
                                        <t:graphicImage id="ac3" value="../../images/ascending-arrow.gif" rendered="#{myProxyServerAdminBean.ca}" border="0"/>
                                        <t:graphicImage id="de3" value="../../images/descending-arrow.gif" rendered="#{myProxyServerAdminBean.notCa}" border="0"/>
                                        <%--    </c:if>--%>
                                    </a4j:commandLink>
                                    
                                </f:facet>  
                                
                                
                                <h:outputText  value="#{data.caRootCertificate}" />
                                
                                
                            </h:column>
                            
                            <h:column>
                                <f:facet name="header"> 
                                    <a4j:commandLink id="downloads" reRender="datatable" actionListener="#{myProxyServerAdminBean.sortColumn}">
                                        
                                        <%--     <h:commandLink style="table-header" id="name" actionListener="#{datacenterBean.sortColumn}">--%>
                                        <h:outputText value="Default" />
                                        <f:param name="column" value="active"/>
                                        <%--<c:if test="${requestScope.datacenterBean.sort == 'name'}" >--%>
                                        <t:graphicImage id="ac2" value="../../images/ascending-arrow.gif" rendered="#{myProxyServerAdminBean.active}" border="0"/>
                                        <t:graphicImage id="de2" value="../../images/descending-arrow.gif" rendered="#{myProxyServerAdminBean.notActive}" border="0"/>
                                        <%--    </c:if>--%>
                                    </a4j:commandLink>
                                    
                                </f:facet> 
                                <a4j:commandLink id="activeLink" reRender="datatable, message" actionListener="#{myProxyServerAdminBean.setActive}">
                                    
                                    <t:graphicImage id="admin2w" value="../../images/correct.gif" height="13" width="13" rendered="#{data.active}" border="0"/>
                                </a4j:commandLink>
                                <a4j:commandLink id="NotactiveLink" reRender="datatable, message" actionListener="#{myProxyServerAdminBean.setActive}">
                                    
                                    <t:graphicImage id="notamdin" value="../../images/wrong.gif" height="13" width="13" rendered="#{!data.active}" border="0"/>                                       
                                </a4j:commandLink>
                                
                            </h:column>
                            
                            <h:column>
                                <f:facet name="header">
                                  
                                        <h:outputText style="table-header; color:blue" escape="false" value="Delete" />
                                           
                                </f:facet>
                                <a4j:commandLink id="deleteLink" reRender="datatable, message" actionListener="#{myProxyServerAdminBean.delete}">                                    
                                    <t:graphicImage id="delete_selected4"  value="../../images/delete.jpg"  border="0"/> 
                                </a4j:commandLink>
                            </h:column>
                            
                            <f:facet name="detailStamp">
                                
                                <t:dataTable  var="upDate" value="#{data}" >
                                    <h:column>
                                        <h:panelGrid columns="3">
                                            
                                            <h:outputLabel>
                                                <h:outputText styleClass="body" value="Address:"/>
                                            </h:outputLabel  >
                                            
                                            <h:inputText id="addressj_id_1" size="30" required="true" value="#{upDate.proxyServerAddress}">
                                                <f:validateLength minimum="5" maximum="60"/>
                                            </h:inputText> 
                                            
                                            
                                            <h:message for="addressj_id_1" styleClass="error" showSummary="false" showDetail="true" />
                                            
                                            <h:outputLabel>
                                                <h:outputText styleClass="body" value="CA DN:"/>
                                            </h:outputLabel  >
                                            
                                            <h:inputText id="caj_id_1" size="30" required="true" value="#{upDate.caRootCertificate}">
                                                <f:validateLength minimum="5" maximum="160"/>
                                            </h:inputText> 
                                            
                                            
                                            <h:message for="caj_id_1" styleClass="error" showSummary="false" showDetail="true" />
                                            
                                            <h:outputLabel>
                                                <h:outputText styleClass="body" value="Port:"/>
                                            </h:outputLabel  >
                                            
                                            <h:inputText id="portj_id_1" size="30" required="true" value="#{upDate.portNumber}">
                                                <f:validateLongRange minimum="1"  />
                                            </h:inputText> 
                                            
                                            
                                            <h:message for="portj_id_1" styleClass="error" showSummary="false" showDetail="true" />
                                            
                                            <h:commandButton action="#{myProxyServerAdminBean.upDateMyProxyServer}" title="Update" value="Update"/>
                                            <h:commandButton action="#{myProxyServerAdminBean.refreshPage}" immediate="true" value="Cancel" title="Cancel" />
                                            <h:panelGroup/>
                                        </h:panelGrid> 
                                    </h:column>
                                </t:dataTable>
                            </f:facet>
                            
                        </t:dataTable>
                        
                        <h:panelGrid columns="1" rendered="#{myProxyServerAdminBean.length}" styleClass="scrollerTable2" columnClasses="standardTable_ColumnCentered" >
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

