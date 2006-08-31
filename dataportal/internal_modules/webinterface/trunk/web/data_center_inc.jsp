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
        <td>    <h:outputText id="text31" value="Data Center" styleClass="nodeFolder"/>
          
        </td>
        
       
        <td height="10"><br />&nbsp;</td>
             
    </table>



    <br />

    <t:dataTable id="datatable" width="95%"
        styleClass="scrollerTable"
        headerClass="standardTable_Header"
        footerClass="standardTable_Header"
        rowClasses="standardTable_Row1,standardTable_Row2"
        columnClasses="standardTable_Column,standardTable_ColumnCentered,standardTable_Column"
        var="data"
        value="#{datacenterBean.dataRefs}"
        preserveDataModel="true"
        rows="#{visit.userPreferences.resultsPerPage}"
        rowId="#{data.id}"
        binding="#{datacenterBean.table}"
        sortColumn="#{datacenterBean.sort}"
        sortAscending="#{datacenterBean.ascending}"
        preserveSort="true">
        <f:facet name="header">
            <h:outputText value="Data References" />
        </f:facet>

       <%--<h:column>
            <f:facet name="header"></f:facet>            
            <h:outputText value="#{data.id}" />              
        </h:column>--%>
        <h:column>
            <f:facet name="header">
                <h:commandLink style="table-header" id="name" actionListener="#{datacenterBean.sortColumn}">
                    <h:outputText value="Name" />
                    <f:param name="column" value="name"/>
                </h:commandLink>
            </f:facet>
            <h:outputText  value="#{data.name}" />
        </h:column>
        <h:column>
            <f:facet name="header">
                <h:commandLink style="table-header" id="type" actionListener="#{datacenterBean.sortColumn}">
                    <h:outputText value="Type" />
                    <f:param name="column" value="type"/>
               
                </h:commandLink>
            </f:facet>
            <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                closePopupOnExitingPopup="true"
                displayAtDistanceX="20"
                displayAtDistanceY="-40" >

                <h:outputText  value="#{data.typeOfReference}" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h:outputText value="List of file(s):"/>                                 
                            <h:outputText escape="false" value="#{data.printURLS}" />                                
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup>
           
        </h:column>
       
        <h:column>
            <f:facet name="header">
                <h:commandLink style="table-header" id="facility" actionListener="#{datacenterBean.sortColumn}">
                    <h:outputText value="Facility" />
                    <f:param name="column" value="facility"/>              
                </h:commandLink>
            </f:facet>
            <h:outputText value="#{data.facility}" />
        </h:column>        
         
         <h:column>
            <f:facet name="header">
                <h:commandLink style="table-header" id="notes" actionListener="#{datacenterBean.sortColumn}">
                    <h:outputText value="Notes" />
                    <f:param name="column" value="notes"/>               
                </h:commandLink>
            </f:facet>
          &nbsp;
            <t:popup styleClass="popup" closePopupOnExitingElement="true"
                closePopupOnExitingPopup="true"
                displayAtDistanceX="-175"
                displayAtDistanceY="-40" >

                <h:outputText style="table-header; color : blue; font-size: 10px; align: left" value="add"/>
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h:inputTextarea value="#{data.note}" valueChangeListener="#{datacenterBean.note}" id="note" rows="10" cols="15">
                                 <f:param name="note" value="#{data.id}"/>
                            </h:inputTextarea>
                            <br />
                            <h:commandButton action="#{datacenterBean.addNote}" title="View selections" value="Add Note"/>
           
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup>
             &nbsp;&nbsp;
             
              <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                closePopupOnExitingPopup="true"
                displayAtDistanceX="5"
                displayAtDistanceY="-40" rendered="#{data.hasNote}">

                <h:outputText  rendered="#{data.hasNote}" style="table-header; color : blue; font-size: 10px" value="view"/>
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
           
        </h:column>
        <h:column>
            <f:facet name="header">
                <h:commandLink style="table-header" id="time" actionListener="#{datacenterBean.sortColumn}">
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
                <h:outputText value="" />
            </f:facet>          

            <h:commandLink style="table-header" id="view" action="#{datacenterBean.viewData}" rendered="#{data.dataset}">
                <h:outputText value=" goto " />
                                
            </h:commandLink>
           
        </h:column>
        <h:column>
            <f:facet name="header">
                <h:outputText value="" />
            </f:facet>
            <h:selectBooleanCheckbox title="select_investigation" valueChangeListener="#{datacenterBean.listen}" value="#{data.selected}" >
                <f:param name="id" value="#{data.id}"/>
            </h:selectBooleanCheckbox>
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

    <table width="95%" border="0">
        <td>
            <h:commandButton action="#{datacenterBean.removeDatasets}" title="View selections" value="Delete selections"/>
             
        </td>
        <td align="right">
            Select:&nbsp; <h:commandButton action="#{datacenterBean.selectall}" title="All" value=" All "/>
            &nbsp; 
            <h:commandButton action="#{datacenterBean.selectnone}" title="None" value="None"/>
        </td>
    </table>
</h:form>