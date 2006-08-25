<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>

<h:form>
   


    <table width="90%">
        <td width="30">&nbsp;</td>
        <td>
            <table>
                <tr> <h:messages globalOnly="true" errorClass="error" infoClass="info" />
                </tr>                 
                <td>    <h:outputText id="text31" value="Search Results from following facilities:" styleClass="nodeFolder"/>
          
                </td>
                <td align="left">&nbsp;&nbsp;&nbsp;
                    <t:popup styleClass="popup" closePopupOnExitingElement="true"
                        closePopupOnExitingPopup="true"
                        displayAtDistanceX="20"
                        displayAtDistanceY="-40" >

                        <h:outputText styleClass="nodeFolder" value="Help"/>&nbsp;&nbsp;&nbsp;
                        <f:facet name="popup">
                            <h:panelGroup>
                                <h:panelGrid columns="1" >
                                    <h:outputText value="This is going to show help tree diagram."/>
                        
                                </h:panelGrid>
                            </h:panelGroup>
                        </f:facet>
                    </t:popup>
                </td>
                <td height="10"><br />&nbsp;</td>
             
            </table>
           
            <t:tree2 id="tree" value="#{investigationTree.data}" var="node" varNodeToggler="t" showRootNode="false">>
    
                <f:facet name="foo-folder">
                    <h:panelGroup>
                        <f:facet name="expand">
                            <t:graphicImage id="gr4" value="../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                        </f:facet>
                        <f:facet name="collapse">
                            <t:graphicImage id="gr3" value="../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                        </f:facet>
                        <h:outputText id="text" value="#{node.description}" styleClass="nodeFolder"/>
                        <h:outputText id="text1" value=" (#{node.childCount})" styleClass="childCount" rendered="#{!empty node.children}"/>
                    </h:panelGroup>
                </f:facet>
                <f:facet name="foo1-folder">
                    <h:panelGroup>
                        <f:facet name="expand22">
                            <t:graphicImage id="gr42" value="../images/yellow-folder-open.png" rendered="#{t.nodeExpanded}" border="0"/>
                        </f:facet>
                        <f:facet name="collapse2">
                            <t:graphicImage id="gr32" value="../images/yellow-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                        </f:facet>
                        <h:outputText id="text2" value="#{node.description}" styleClass="nodeFolder"/>
                    </h:panelGroup>
                </f:facet>
                <f:facet name="bar-folder">
                    <h:panelGroup>
                        <f:facet name="expand2">
                            <t:graphicImage id="gr" value="../images/blue-folder-open.gif" rendered="#{t.nodeExpanded}" border="0"/>
                        </f:facet>
                        <f:facet name="collapse3">
                            <t:graphicImage id="gr1" value="../images/blue-folder-closed.png" rendered="#{!t.nodeExpanded}" border="0"/>
                        </f:facet>
                        <h:outputText id="text3" styleClass="document" value="#{node.description}" />
           
                    </h:panelGroup>
                </f:facet>
    
            </t:tree2>
        </td>
    </table>                        
</h:form>

<br />
<h:form>
    <t:dataTable id="data" width="90%"
        styleClass="scrollerTable"
        headerClass="standardTable_Header"
        footerClass="standardTable_Header"
        rowClasses="standardTable_Row1,standardTable_Row2"
        columnClasses="standardTable_Column,standardTable_ColumnCentered,standardTable_Column"
        var="invest"
        value="#{investigationBean.investigations}"
        preserveDataModel="true"
        rows="#{visit.userPreferences.resultsPerPage}"
        rowId="#{invest.id}"
        binding="#{investigationBean.table}"
        sortColumn="#{investigationBean.sort}"
        sortAscending="#{investigationBean.ascending}"
        preserveSort="true">
        <f:facet name="header">
				<h:outputText value="Investigations" />
			</f:facet>

        <h:column>
            <f:facet name="header"></f:facet>            
            <h:outputText value="#{invest.id}" />
              
        </h:column>
        <h:column>
            <f:facet name="header">
                <h:commandLink style="table-header" id="name" actionListener="#{investigationBean.sortColumn}">
                    <h:outputText value="Name" />
                    <f:param name="column" value="name"/>
                </h:commandLink>
            </f:facet>
            <h:outputText  value="#{invest.name}" />

        </h:column>
        <h:column>
            <f:facet name="header">
                <h:commandLink style="table-header" id="type" actionListener="#{investigationBean.sortColumn}">
                    <h:outputText value="Type" />
                    <f:param name="column" value="type"/>
               
                </h:commandLink>
            </f:facet>
            <h:outputText  value="#{invest.investigationType}" />

        </h:column>
        <h:column>
            <f:facet name="header">
                <h:commandLink style="table-header" id="abstract" actionListener="#{investigationBean.sortColumn}">
                    <h:outputText value="Abstract" />
                    <f:param name="column" value="abstract"/>
               
                </h:commandLink>
            </f:facet>
            <h:outputText  value="#{invest.investigationAbstract}" />

        </h:column>
        <h:column>
            <f:facet name="header">
                <h:commandLink style="table-header" id="facility" actionListener="#{investigationBean.sortColumn}">
                    <h:outputText value="Facility" />
                    <f:param name="column" value="facility"/>
              
                </h:commandLink>
            </f:facet>
            <h:outputText value="#{invest.facility}" />
        </h:column>
       
        <h:column>
            <f:facet name="header">
                <h:outputText value="" />
            </f:facet>
            <h:selectBooleanCheckbox title="select_investigation" valueChangeListener="#{investigationBean.listen}" value="#{invest.selected}" >
                <f:param name="id" value="#{invest.id}"/>
            </h:selectBooleanCheckbox>
        </h:column>

    </t:dataTable>

    <h:panelGrid columns="1" styleClass="scrollerTable2" columnClasses="standardTable_ColumnCentered" >
        <t:dataScroller id="scroll_1"
            for="data"
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
        <td align="right">
            Select:&nbsp; <h:commandButton action="#{investigationBean.selectall}" title="All" value=" All "/>
            &nbsp; 
            <h:commandButton action="#{investigationBean.selectnone}" title="None" value="None"/>
        </td>
    </table>
</h:form>