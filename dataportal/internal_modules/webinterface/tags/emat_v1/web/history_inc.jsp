<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jstl/core" prefix="c"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>

<h:form>
   
    <table width="90%">
    <td width="30">&nbsp;</td>
    <td>
    <table width="90%" border="0">
        <tr> <h:messages globalOnly="true" errorClass="error" infoClass="info" />
        </tr>                 
        <td>    <h:outputText id="text31" value="Search History" styleClass="nodeFolder"/>         
        </td>
        <td height="10"><br />&nbsp;</td>             
    </table>

    <br />

    <t:dataTable id="datatable" width="90%"
        styleClass="scrollerTable"
        headerClass="standardTable_Header"
        footerClass="standardTable_Header"
        rowClasses="standardTable_Row1,standardTable_Row2"
        columnClasses="standardTable_Column,standardTable_ColumnCentered,standardTable_Column"
        var="data"
        value="#{historyBean.history}"
        preserveDataModel="true"
        rows="#{visit.userPreferences.resultsPerPage}"
        
        binding="#{historyBean.table}"
        sortColumn="#{historyBean.sort}"
        sortAscending="#{historyBean.ascending}"
        preserveSort="true">
        <f:facet name="header">
            <h:outputText value="Previous Searches" />
        </f:facet>

        <h:column>
            <f:facet name="header"></f:facet>            
            <h:outputText value="1" />              
        </h:column>
        <h:column>
            <f:facet name="header">
                <h:commandLink style="table-header" id="facilities" actionListener="#{historyBean.sortColumn}">
                    <h:outputText value="Facility(s)" />
                    <f:param name="column" value="facilities"/>              
                </h:commandLink>
            </f:facet>
            <h:outputText value="#{data.facilities}" />
        </h:column>
        <h:column>
            <f:facet name="header">
                <h:commandLink style="table-header" id="sent" actionListener="#{historyBean.sortColumn}">
                    <h:outputText value="Sent" />
                    <f:param name="column" value="sent"/>              
                </h:commandLink>
            </f:facet>
            <h:outputText value="#{data.sent}" >
                <f:convertDateTime pattern="HH:mm  dd.MM.yyyy "/>
            </h:outputText>
        </h:column>      
        <h:column>
            <f:facet name="header">
                <h:commandLink style="table-header" id="processed" actionListener="#{historyBean.sortColumn}">
                    <h:outputText value="Processed" />
                    <f:param name="column" value="processed"/>              
                </h:commandLink>
            </f:facet>
            <h:outputText value="#{data.processed}" >
                <f:convertDateTime pattern="HH:mm  dd.MM.yyyy "/>
            </h:outputText>
        </h:column>      
        <h:column>
            
            <f:facet name="header">
                <h:outputText value="" />
            </f:facet>
            <h:commandLink style="table-header" id="view" action="#{historyBean.viewData}">
                    <h:outputText value="view" />
                                
                </h:commandLink>
        </h:column>
    </t:dataTable>

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
                <t:graphicImage url="../images/arrow-first.gif" border="1" />
            </f:facet>
            <f:facet name="last">
                <t:graphicImage url="../images/arrow-last.gif" border="1" />
            </f:facet>
            <f:facet name="previous">
                <t:graphicImage url="../images/arrow-previous.gif" border="1" />
            </f:facet>
            <f:facet name="next">
                <t:graphicImage url="../images/arrow-next.gif" border="1" />
            </f:facet>
            <f:facet name="fastforward">
                <t:graphicImage url="../images/arrow-ff.gif" border="1" />
            </f:facet>
            <f:facet name="fastrewind">
                <t:graphicImage url="../images/arrow-fr.gif" border="1" />
            </f:facet>
        </t:dataScroller>
    </h:panelGrid>
    <br />

    <table width="90%" border="0">
        <td>
            <h:commandButton action="#{investigationBean.datasets}" title="View selections" value="View selection"/>
             
        </td>
       
    </table>
</h:form>                    
