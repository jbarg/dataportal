<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jstl/core" prefix="c"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>

<h:form>
   
    <table width="100%">
        <td width="5">&nbsp;</td>
        <td>
            <table width="100%" border="0">
                <tr> <h:messages globalOnly="true" errorClass="error" infoClass="info" />
                </tr>                 
        
                <tr ><br />&nbsp;</tr>             
            </table>

   

            <t:dataTable id="givendatatable" width="95%"
                styleClass="scrollerTable"
                headerClass="standardTable_Header"
                footerClass="standardTable_Header"
                rowClasses="standardTable_Row1,standardTable_Row2"
                columnClasses="standardTable_Column,standardTable_ColumnCentered,standardTable_Column"
                var="data"
                value="#{viewDataReceivedAuthorisationBean.receivedDataRefs}"
                preserveDataModel="true"
                rows="#{visit.userPreferences.resultsPerPage}"
                
                binding="#{viewDataReceivedAuthorisationBean.table}"
                sortColumn="#{viewDataReceivedAuthorisationBean.sort}"
                sortAscending="#{viewDataReceivedAuthorisationBean.ascending}"
                preserveSort="true">
                <f:facet name="header">
                    <h:outputText value="Received Authorisations" />
                </f:facet>

                -<h:column>
                    <f:facet name="header"></f:facet>  
        
                      <t:graphicImage id="up" value="../../images/person.png" rendered="#{data.selected}" border="0"/>
                      
                </h:column>
               
                <h:column>
                    <f:facet name="header">
                        <h:commandLink style="table-header" id="givenToUser" actionListener="#{viewDataReceivedAuthorisationBean.sortColumn}">
                            <h:outputText value="From" />
                            <f:param name="column" value="givenToUser"/>
                        </h:commandLink>
                    </f:facet>
                    <h:outputText  value="#{data.source_user.dn}" />
                     
                </h:column>
       
      
                <h:column>
                    <f:facet name="header">
                        <h:commandLink style="table-header" id="startDate" actionListener="#{viewDataReceivedAuthorisationBean.sortColumn}">
                            <h:outputText value="Start Date" />
                            <f:param name="column" value="startDate"/>              
                        </h:commandLink>
                          
                    </f:facet>
                    <h:outputText value="#{data.authStartDate}" >
                        <f:convertDateTime pattern="dd.MM.yy "/>
                    </h:outputText>
                </h:column>
                <h:column>
                    <f:facet name="header">
                        <h:commandLink style="table-header" id="endDate" actionListener="#{viewDataReceivedAuthorisationBean.sortColumn}">
                            <h:outputText value="End Date" />
                            <f:param name="column" value="endDate"/>               
                        </h:commandLink>
                    </f:facet>
                    <h:outputText value="#{data.authEndDate}" >
                        <f:convertDateTime pattern="dd.MM.yy "/>
                    </h:outputText>
                </h:column>
        
                <h:column>
                    <f:facet name="header">
                        <h:commandLink style="table-header" id="type" actionListener="#{viewDataReceivedAuthorisationBean.sortColumn}">
                            <h:outputText value="Type" />
                            <f:param name="column" value="type"/>              
                        </h:commandLink>
                    </f:facet>
                    <h:outputText value="#{data.authType}" />
                       
                </h:column>  
                <h:column>
            
                    <f:facet name="header">
                        <h:outputText value="" />
                    </f:facet>
                    <h:commandLink rendered="#{!data.selected}" style="table-header" id="goto" action="#{viewDataReceivedAuthorisationBean.viewData}">
                        <h:outputText value=" Set " />                                
                    </h:commandLink>
                    
                    <h:commandLink rendered="#{data.selected}" style="table-header" id="goto1" action="#{viewDataReceivedAuthorisationBean.unviewData}">
                        <h:outputText value=" Unset " />                                
                    </h:commandLink>
                </h:column>
               
            </t:dataTable>

            <h:panelGrid columns="1" styleClass="scrollerTable2" columnClasses="standardTable_ColumnCentered" >
                <t:dataScroller id="scroll_11"
                    for="givendatatable"
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
            <br />
            <t:dataTable id="receiveddatatable" width="95%"
                styleClass="scrollerTable"
                headerClass="standardTable_Header"
                footerClass="standardTable_Header"
                rowClasses="standardTable_Row1,standardTable_Row2"
                columnClasses="standardTable_Column,standardTable_ColumnCentered,standardTable_Column"
                var="data"
                value="#{viewDataGivenAuthorisationBean.givenDataRefs}"
                preserveDataModel="true"
                rows="#{visit.userPreferences.resultsPerPage}"
               
                binding="#{viewDataGivenAuthorisationBean.table}"
                sortColumn="#{viewDataGivenAuthorisationBean.sort}"
                sortAscending="#{viewDataGivenAuthorisationBean.ascending}"
                preserveSort="true">
                <f:facet name="header">
                    <h:outputText value="Authorisation access given from you to:" />
                </f:facet>

               
                <h:column>
                    <f:facet name="header">
                        <h:commandLink style="table-header" id="givenToUser" actionListener="#{viewDataGivenAuthorisationBean.sortColumn}">
                            <h:outputText value="To" />
                            <f:param name="column" value="givenToUser"/>
                        </h:commandLink>
                    </f:facet>
                    <h:outputText  value="#{data.user.dn}" />
                        
                </h:column>
       
      
                <h:column>
                    <f:facet name="header">
                        <h:commandLink style="table-header" id="startDate" actionListener="#{viewDataGivenAuthorisationBean.sortColumn}">
                            <h:outputText value="Start Date" />
                            <f:param name="column" value="startDate"/>              
                        </h:commandLink>
                    </f:facet>
                    <h:outputText value="#{data.authStartDate}" >
                        <f:convertDateTime pattern="dd.MM.yy "/>
                    </h:outputText>
                    
                </h:column>
                <h:column>
                    <f:facet name="header">
                        <h:commandLink style="table-header" id="endDate" actionListener="#{viewDataGivenAuthorisationBean.sortColumn}">
                            <h:outputText value="End Date" />
                            <f:param name="column" value="endDate"/>               
                        </h:commandLink>
                    </f:facet>
                    <h:outputText value="#{data.authEndDate}" >
                        <f:convertDateTime pattern="dd.MM.yy "/>
                    </h:outputText>
                </h:column>
        
                <h:column>
                    <f:facet name="header">
                        <h:commandLink style="table-header" id="type" actionListener="#{viewDataGivenAuthorisationBean.sortColumn}">
                            <h:outputText value="Type" />
                            <f:param name="column" value="type"/>              
                        </h:commandLink>
                    </f:facet>
                    <h:outputText value="#{data.authType}" />
                       
                </h:column>  
                <h:column>
            
                    <f:facet name="header">
                        <h:outputText value="" />
                    </f:facet>
                    <h:commandLink style="table-header" id="goto" action="#{viewDataGivenAuthorisationBean.remove}">
                        <h:outputText value=" remove " />
                                
                    </h:commandLink>
                </h:column>
               
            </t:dataTable>

            <h:panelGrid columns="1" styleClass="scrollerTable2" columnClasses="standardTable_ColumnCentered" >
                <t:dataScroller id="scroll_1"
                    for="givendatatable"
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
    </table>
</h:form>
