<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>
<%@ taglib uri="https://ajax4jsf.dev.java.net/ajax" prefix="a4j"%>
<%@ taglib prefix="fmt" uri="http://java.sun.com/jsp/jstl/fmt" %>

<script>
    
    function waitStatus(){
        
        var image =  document.getElementById("body:tableForm:status");
        image.src = "../images/connect_active.gif";
        document.body.style.cursor = 'wait';
        return true;
    }  
    
    function normalStatus(){
        
        var image =  document.getElementById("body:tableForm:status");
        image.src = "../images/connect_idle.gif";
        document.body.style.cursor = 'default';
        return true;
    }          
    
    </script>

<a4j:region   selfRendered="true" > 
    <a4j:form id="tableForm"> 
        <table width="95%" border="0" >
            <tbody>
                <tr>
                    <td width=20">&nbsp;</td>
                    <td>
                        
                        <table border="0" width="100%">
                            <tbody>
                                <tr>
                                    <td> <h:messages id="messages" globalOnly="true" warnClass="error" errorClass="error" infoClass="info" />
                                    </td>     
                                    <td align="right"> 
                                        <h:graphicImage id="status" url="../images/connect_idle.gif" />
                                    </td> 
                                </tr>
                            </tbody>      
                        </table>
                        
                    </td>
                </tr>
            </tbody>
        </table>
        <br />
        <table width="95%" border="0" >
            <tbody>
                <tr>             
                    <td width=20">&nbsp;</td>
                    <td>
                        
                        <h:panelGrid id="topgrid" border="0" width="97%" columns="2" styleClass="scrollerTable3" columnClasses="standardTable_ColumnCentered">
                            
                            <h:panelGroup  style="float: left" rendered="#{fn:length(sessionScope.visit.visitData.searchedInvestigations) > sessionScope.sessionHistory.numberOfResultsInvestigations}" >
                                
                                <t:dataScroller id="scroll_3"
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
                            </h:panelGroup>        
                            
                            <h:panelGroup rendered="#{fn:length(visit.visitData.searchedInvestigations) <= sessionScope.sessionHistory.numberOfResultsInvestigations}"  />
                            
                            <!---  Drop down list of length of table -->             
                            <h:panelGroup rendered="#{fn:length(visit.visitData.searchedInvestigations) > 5 }" style="float: right">
                                <h:outputText style="font-size:10px; color: black;  font-weight: bold;" value="Maximum displayed: " />
                                <h:selectOneMenu id="listj_id_2" required="false" immediate="true" value="#{sessionHistory.numberOfResultsInvestigationsString2}" >                           
                                    <a4j:support event="onchange" action="#{investigationBean.maxDisplay}" ajaxSingle="true" reRender="data, topgrid, bottomgrid, topgrid2" />            
                                    
                                    <f:selectItem itemLabel="5"  itemValue="5"/> 
                                    <f:selectItem itemLabel="10"  itemValue="10"/> 
                                    <f:selectItem itemLabel="20"  itemValue="20"/> 
                                    <f:selectItem itemLabel="50"  itemValue="50"/> 
                                    <f:selectItem itemLabel="100"  itemValue="100"/> 
                                </h:selectOneMenu>   
                            </h:panelGroup>
                            
                        </h:panelGrid>
                        
                        <t:dataTable renderedIfEmpty="false"
                                     id="data" width="97%"
                                     styleClass="scrollerTable"
                                     headerClass="standardTable_Header"
                                     footerClass="standardTable_Header"
                                     rowClasses="standardTable_Row1,standardTable_Row2"
                                     columnClasses="standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered,standardTable_ColumnCentered"
                                     var="invest"
                                     value="#{investigationBean.investigations}"
                                     preserveDataModel="true"
                                     rows="#{sessionHistory.numberOfResultsInvestigations}"       
                                     binding="#{investigationBean.table}"
                                     sortColumn="#{investigationBean.sort}"
                                     sortAscending="#{investigationBean.ascending}"
                                     preserveSort="true"
                                     varDetailToggler="detailToggler"       >     
                            <f:facet name="header">
                                <h:outputText value="Investigations" />
                            </f:facet>
                            
                            
                            <%-- <h:column>
                                            
                                            <f:facet name="header">
                                            <h:panelGrid columns="2" >
                                            <a4j:commandLink reRender="data,expand"  style="table-header" ajaxSingle="true" id="selectAll" rendered="#{visit.visitData.investigationsSelected}" actionListener="#{investigationBean.selectNone}">           
                                            <t:graphicImage  id="selectNonew" value="../../images/button_minus1.gif"  border="0"/>
                                            </a4j:commandLink>   
                                            
                                            <a4j:commandLink reRender="data,expand" style="table-header" ajaxSingle="true" id="selectNone" rendered="#{!visit.visitData.investigationsSelected}" actionListener="#{investigationBean.selectAll}">           
                                            <t:graphicImage  id="selectAllc" value="../../images/button_plus1.gif"  border="0"/>                 
                                            </a4j:commandLink> 
                                            </h:panelGrid>
                                            </f:facet>
                                            <h:selectBooleanCheckbox title="select_investigation" value="#{invest.selected}" >
                                            <f:param name="id" value="#{invest.id}"/>
                                            <a4j:support reRender="expand" event="onclick" ajaxSingle="true"  immediate="true" actionListener="#{investigationBean.listenAjax}">
                                            <a4j:actionparam name="id" value="#{invest.id}"/>   
                                            </a4j:support>
                                            </h:selectBooleanCheckbox>
                                            </h:column>--%>
                                            
                            <h:column>
                                <f:facet name="header">
                                    <h:panelGrid columns="" >
                                        
                                        
                                    </h:panelGrid>        
                                </f:facet>
                                
                                <h:panelGrid>                    
                                    <a4j:commandLink oncomplete="normalStatus();" onclick="waitStatus();"  reRender="data" style="table-header" ajaxSingle="true" id="abS" actionListener="#{investigationBean.getInvestigationInfo}" action="#{detailToggler.toggleDetail}">                        
                                        <t:graphicImage id="up" value="../../images/button_plus1.gif" rendered="#{!detailToggler.currentDetailExpanded}" border="0" />
                                        <t:graphicImage id="up-f" value="../../images/button_minus1.gif" rendered="#{detailToggler.currentDetailExpanded}" border="0" />                        
                                    </a4j:commandLink>  
                                </h:panelGrid>
                            </h:column>
                            
                            
                            <!--    Visit Id              -->
                            <h:column rendered="#{visit.customisation.visitIdVisible}">
                                <f:facet name="header">
                                    <a4j:commandLink reRender="data" style="table-header" ajaxSingle="true" id="visitId" actionListener="#{investigationBean.sortColumn}">
                                        
                                        <h:outputText value="Visit Id" />
                                        <f:param name="column" value="visitId"/>
                                        <%--   <c:if test="${requestScope.investigationBean.sort == 'type'}" >--%>
                                        <t:graphicImage id="acdhffty" value="../../images/ascending-arrow.gif" rendered="#{investigationBean.visitId}" border="0"/>
                                        <t:graphicImage id="dedffty" value="../../images/descending-arrow.gif" rendered="#{investigationBean.notVisitId}" border="0"/>
                                        <%--  </c:if>--%>
                                    </a4j:commandLink>
                                </f:facet>
                                <h:outputText  value="#{invest.visitId}" />                              
                                
                            </h:column>
                            
                            <!--    Experiment Number (Rb #)              -->                                            
                            <h:column rendered="#{visit.customisation.invNumberVisible}">
                                <f:facet name="header">
                                    <a4j:commandLink reRender="data" style="table-header" ajaxSingle="true" id="invNumber" actionListener="#{investigationBean.sortColumn}">
                                        
                                        <h:outputText value="Rb Number" />
                                        <f:param name="column" value="invNumber"/>
                                        <%--   <c:if test="${requestScope.investigationBean.sort == 'type'}" >--%>
                                        <t:graphicImage id="acdty" value="../../images/ascending-arrow.gif" rendered="#{investigationBean.invNumber}" border="0"/>
                                        <t:graphicImage id="dedty" value="../../images/descending-arrow.gif" rendered="#{investigationBean.notInvNumber}" border="0"/>
                                        <%--  </c:if>--%>
                                    </a4j:commandLink>
                                </f:facet>
                                <h:outputText  value="#{invest.invNumber}" />              
                                
                                <%--   <h:outputText  value="#{fn:toLowerCase(invest.invType.name)}" />               --%>
                            </h:column>
                            
                            <!-- Title -->
                            <h:column>
                                <f:facet name="header">
                                    <%--  <a4j:commandLink reRender="data" style="table-header" id="name" actionListener="#{investigationBean.sortColumn}">--%>
                                                    
                                    <a4j:commandLink reRender="data, props" style="table-header" id="name" actionListener="#{investigationBean.sortColumn}">
                                        <h:outputText value="Title" />
                                        <f:param name="column" value="name"/>
                                        <%--   <c:if test="${requestScope.investigationBean.sort == 'name'}" >--%>
                                        <t:graphicImage id="acname" value="../../images/ascending-arrow.gif" rendered="#{investigationBean.name}" border="0"/>
                                        <t:graphicImage id="dename" value="../../images/descending-arrow.gif" rendered="#{investigationBean.notName}" border="0"/>
                                        <%--  </c:if>--%>
                                    </a4j:commandLink>
                                </f:facet>
                                
                                <h:commandLink value="#{invest.title}" style="color: black; " styleClass="investigation" action="#{investigationBean.viewSingleInvestigation}" />
                                
                                
                            </h:column>
                            
                            <!--  Type -->
                            <h:column rendered="#{visit.customisation.invTypeVisible}">
                                <f:facet name="header">
                                    <a4j:commandLink reRender="data" style="table-header" ajaxSingle="true" id="invType" actionListener="#{investigationBean.sortColumn}">
                                        
                                        <h:outputText value="Type" />
                                        <f:param name="column" value="invType"/>
                                        <%--   <c:if test="${requestScope.investigationBean.sort == 'type'}" >--%>
                                        <t:graphicImage id="acty" value="../../images/ascending-arrow.gif" rendered="#{investigationBean.invType}" border="0"/>
                                        <t:graphicImage id="dety" value="../../images/descending-arrow.gif" rendered="#{investigationBean.notInvType}" border="0"/>
                                        <%--  </c:if>--%>
                                    </a4j:commandLink>
                                </f:facet>
                                <h:outputText  value="#{invest.invType}" />     
                            </h:column>                                                                                    
                            
                            <!--  Facility -->
                            <h:column rendered="#{!visit.singleFacility}">
                                <f:facet name="header">
                                    <%--<a4j:commandLink reRender="data" style="table-header" id="facility" actionListener="#{investigationBean.sortColumn}">--%>
                                                    
                                    <a4j:commandLink reRender="data" style="table-header" id="facility" ajaxSingle="true" actionListener="#{investigationBean.sortColumn}">
                                        <h:outputText value="Facility" />
                                        <f:param name="column" value="facility"/>
                                        <%-- <c:if test="${requestScope.investigationBean.sort == 'facility'}" >                            --%>
                                        <t:graphicImage id="acf" value="../../images/ascending-arrow.gif" rendered="#{investigationBean.facility}" border="0"/>
                                        <t:graphicImage id="def" value="../../images/descending-arrow.gif" rendered="#{investigationBean.notFacility}" border="0"/>
                                        <%--    </c:if>--%>
                                    </a4j:commandLink>
                                </f:facet>
                                <h:outputText value="#{invest.facility}" />
                                
                            </h:column>
                            
                            <!-- Instrument -->
                            <h:column>
                                <f:facet name="header">
                                    <a4j:commandLink reRender="data" style="table-header" ajaxSingle="true" id="instrument" actionListener="#{investigationBean.sortColumn}">
                                        
                                        <h:outputText value="#{visit.customisation.instrument}" />
                                        <f:param name="column" value="instrument"/>
                                        <%--   <c:if test="${requestScope.investigationBean.sort == 'type'}" >--%>
                                        <t:graphicImage id="acstddy" value="../../images/ascending-arrow.gif" rendered="#{investigationBean.instrument}" border="0"/>
                                        <t:graphicImage id="dedddty" value="../../images/descending-arrow.gif" rendered="#{investigationBean.notInstrument}" border="0"/>
                                        <%--  </c:if>--%>
                                    </a4j:commandLink>
                                </f:facet>
                                <h:outputText  value="#{invest.instrument}" />                              
                                
                            </h:column>
                            
                            <!--  ISIS Investigator -->
                            <h:column rendered="#{visit.customisation.bcatInvStr}">
                                <f:facet name="header">
                                    <a4j:commandLink reRender="data" style="table-header" ajaxSingle="true" id="bcat_inv_str" actionListener="#{investigationBean.sortColumn}">
                                        
                                        <h:outputText value="Investigator" />
                                        <f:param name="column" value="bcatInvStr"/>
                                        
                                        <t:graphicImage id="acbddty" value="../../images/ascending-arrow.gif" rendered="#{investigationBean.bcatInvStr}" border="0"/>
                                        <t:graphicImage id="dddety" value="../../images/descending-arrow.gif" rendered="#{investigationBean.notBcatInvStr}" border="0"/>
                                        
                                    </a4j:commandLink>
                                </f:facet>
                                <h:outputText  value="#{invest.bcatInvStr}" />              
                                
                            </h:column>
                            
                            <!--  Cycle -->
                            <h:column rendered="#{visit.customisation.cycleVisible}">
                                <f:facet name="header">
                                    
                                    <a4j:commandLink reRender="data" style="table-header" id="facilityCycle" ajaxSingle="true" actionListener="#{investigationBean.sortColumn}">
                                        <h:outputText value="Facility Cycle" />
                                        <f:param name="column" value="facilityCycle"/>
                                        <t:graphicImage id="agggcf" value="../../images/ascending-arrow.gif" rendered="#{investigationBean.facilityCycle}" border="0"/>
                                        <t:graphicImage id="gg" value="../../images/descending-arrow.gif" rendered="#{investigationBean.notFacilityCycle}" border="0"/>
                                        
                                    </a4j:commandLink>
                                </f:facet>
                                <h:outputText value="#{invest.facilityCycle.name}" />
                                
                            </h:column>
                            
                            <!--  Release Date -->
                            <h:column>
                                <f:facet name="header">
                                    
                                    <a4j:commandLink reRender="data" style="table-header" id="releaseDate" ajaxSingle="true" actionListener="#{investigationBean.sortColumn}">
                                        <h:outputText value="Release Date" />
                                        <f:param name="column" value="releaseDate"/>
                                        <t:graphicImage id="fff" value="../../images/ascending-arrow.gif" rendered="#{investigationBean.releaseDate}" border="0"/>
                                        <t:graphicImage id="gffg" value="../../images/descending-arrow.gif" rendered="#{investigationBean.notReleaseDate}" border="0"/>
                                        
                                    </a4j:commandLink>
                                </f:facet>
                                
                                <h:outputText value="#{fn:substringBefore(invest.releaseDate,\"T\")}">
                                    <f:convertDateTime pattern="HH:mm a dd.MM.yyyy "/>
                                </h:outputText>
                            </h:column>
                            
                            
                            <f:facet name="detailStamp">      
                                
                                
                                <t:dataTable preserveSort="true" 
                                             align="center" 
                                             styleClass="detailTable"
                                             headerClass="standardTable_Header"
                                             footerClass="standardTable_Header"
                                             rowClasses="standardTableDetail2_Row1,standardTableDetail2_Row1"
                                             columnClasses="standardTable_ColumnCenteredTop"
                                             
                                             width="100%" 
                                             id="invAbstract" 
                                             var="investigations"
                                             value="#{invest}">
                                    
                                    
                                    <h:column>
                                        <f:facet name="header">
                                            <h:outputText value="Abstract" style="font-size:12px; color: blue;  font-weight: bold;" />
                                        </f:facet>
                                        
                                        <t:dataTable  styleClass="detailTable"
                                                      headerClass="standardTable_Header"
                                                      footerClass="standardTable_Header"
                                                      rowClasses="standardTableDetail_Row1,standardTableDetail_Row2"
                                                      columnClasses="standardTable_ColumnCenteredTop"
                                                      style="valign:top; align:top;"
                                                      width="100%" 
                                                      id="abstract" 
                                                      var="abstract" 
                                                      value="#{invest}">                            
                                            
                                            
                                            <h:column>
                                                
                                                <h:column>                           
                                                    <h:outputText style="valign:top; font-size:12px;" value="#{abstract.invAbstract}" />
                                                </h:column>  
                                            </h:column>
                                            
                                        </t:dataTable>
                                    </h:column> 
                                    
                                    <h:column  rendered="#{visit.customisation.shiftVisible}">
                                        <f:facet name="header">
                                            <h:outputText value="Shifts" style="font-size:12px; color: blue;  font-weight: bold;" />
                                        </f:facet>
                                        
                                        <t:dataTable  styleClass="detailTable"
                                                      headerClass="standardTable_Header"
                                                      footerClass="standardTable_Header"
                                                      rowClasses="standardTableDetail_Row1,standardTableDetail_Row2"
                                                      columnClasses="standardTable_ColumnCenteredTop"
                                                      style="valign:top; align:top;"
                                                      width="100%" 
                                                      id="shifts" 
                                                      var="shifts" 
                                                      value="#{invest.shiftCollection}">                            
                                            
                                            
                                            <h:column>
                                                <f:facet name="header">
                                                    <h:outputText value="Start" style="font-size:12px; color: blue;  font-weight: bold;" />
                                                </f:facet>   
                                                <h:column>                           
                                                    <h:outputText  value="#{fn:substringBefore(fn:substringAfter(shifts.shiftPK.startDate,\"T\"),\".\")}  #{fn:substringBefore(shifts.shiftPK.startDate,\"T\")}" style="font-size:12px;" />                                  
                                                </h:column>  
                                            </h:column>
                                            
                                            <h:column>
                                                <f:facet name="header">
                                                    <h:outputText value="End" style="font-size:12px; color: blue;  font-weight: bold;" />
                                                </f:facet> 
                                                <h:column>
                                                    <h:outputText  value="#{fn:substringBefore(fn:substringAfter(shifts.shiftPK.endDate,\"T\"),\".\")}  #{fn:substringBefore(shifts.shiftPK.endDate,\"T\")}" style="font-size:12px;" />                                  
                                                </h:column>      
                                            </h:column>                    
                                        </t:dataTable>
                                    </h:column> 
                                    
                                    <h:column>
                                        <f:facet name="header">
                                            <h:outputText value="Investigators" style="font-size:12px; color: blue;  font-weight: bold;" />
                                        </f:facet>
                                        
                                        <t:dataTable  styleClass="detailTable"
                                                      headerClass="standardTable_Header"
                                                      footerClass="standardTable_Header"
                                                      rowClasses="standardTableDetail_Row1,standardTableDetail_Row2"
                                                      columnClasses="standardTable_ColumnCenteredTop"
                                                      style="valign:top; align:top;"
                                                      width="100%" 
                                                      id="investigators" 
                                                      var="investigators" 
                                                      value="#{invest.investigatorCollection}">                            
                                            
                                            <h:column>
                                                <f:facet name="header">
                                                    <h:outputText value="First Name" style="font-size:12px; color: blue;  font-weight: bold;" />
                                                </f:facet>   
                                                <h:outputText  value="#{investigators.facilityUser.firstName}" style="font-size:12px;"  />  
                                            </h:column>
                                            
                                            <h:column>
                                                <f:facet name="header">
                                                    <h:outputText value="Surname" style="font-size:12px; color: blue;  font-weight: bold;" />
                                                </f:facet> 
                                                <h:outputText  value="#{investigators.facilityUser.lastName}" style="font-size:12px;"  />  
                                            </h:column>                                 
                                        </t:dataTable>
                                    </h:column>   
                                    
                                    
                                    <h:column>
                                        <f:facet name="header">
                                            <h:outputText value="Samples" style="font-size:12px; color: blue;  font-weight: bold;" />
                                        </f:facet>
                                        <t:dataTable  styleClass="detailTable"
                                                      headerClass="standardTable_Header"
                                                      footerClass="standardTable_Header"
                                                      rowClasses="standardTableDetail_Row1,standardTableDetail_Row2"
                                                      columnClasses="standardTable_ColumnCentered"
                                                      
                                                      width="100%" 
                                                      id="samples" 
                                                      var="sample" 
                                                      value="#{invest.sampleCollection}">                            
                                            
                                            <h:column>
                                                <f:facet name="header">
                                                    <h:outputText value="Name" style="font-size:12px; color: blue;  font-weight: bold;" />
                                                </f:facet>   
                                                <h:outputText  value="#{sample.name}" style="font-size:12px;"  />  
                                            </h:column>
                                            
                                         <%--   <h:column>
                                                <f:facet name="header">
                                                    <h:outputText value="Instance" style="font-size:12px; color: blue;  font-weight: bold;" />
                                                </f:facet> 
                                                <h:outputText  value="#{sample.instance}" style="font-size:12px;"  />  
                                            </h:column>    --%>                        
                                        </t:dataTable>                           
                                    </h:column> 
                                    
                                </t:dataTable>                      
                                
                            </f:facet>              
                            
                        </t:dataTable>
                        
                        <h:panelGrid id="bottomgrid"  columns="2" styleClass="scrollerTable2" columnClasses="standardTable_ColumnCentered" border="0" width="97%">
                            <h:panelGroup style="float: left" rendered="#{fn:length(sessionScope.visit.visitData.searchedInvestigations) > sessionScope.sessionHistory.numberOfResultsInvestigations}" >            
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
                                
                            </h:panelGroup>
                            
                            <h:panelGroup rendered="#{fn:length(visit.visitData.searchedInvestigations) <= sessionHistory.numberOfResultsInvestigations}"  />
                            
                            <!---  Drop down list of length of table -->
                            <h:panelGroup rendered="#{fn:length(visit.visitData.searchedInvestigations) > 5 }" style="float: right">
                                <h:outputText style="font-size:10px; color: black;  font-weight: bold;" value="Maximum displayed: " />
                                <h:selectOneMenu id="listj_id_1" required="false" immediate="true" value="#{sessionHistory.numberOfResultsInvestigationsString}" >                           
                                    <a4j:support event="onchange" action="#{investigationBean.maxDisplay}" ajaxSingle="true" reRender="data, topgrid, bottomgrid" />            
                                    
                                    <f:selectItem itemLabel="5"  itemValue="5"/> 
                                    <f:selectItem itemLabel="10"  itemValue="10"/> 
                                    <f:selectItem itemLabel="20"  itemValue="20"/> 
                                    <f:selectItem itemLabel="50"  itemValue="50"/> 
                                    <f:selectItem itemLabel="100"  itemValue="100"/> 
                                </h:selectOneMenu>   
                            </h:panelGroup>
                            
                            <h:panelGroup style="float: left"  >            
                                
                                <t:dataScroller id="scroll_2"
                                                for="data"
                                                rowsCountVar="rowsCount"
                                                displayedRowsCountVar="displayedRowsCountVar"
                                                firstRowIndexVar="firstRowIndex"
                                                lastRowIndexVar="lastRowIndex"
                                                pageCountVar="pageCount"
                                                immediate="true"
                                                pageIndexVar="pageIndex"
                                                >
                                    <h:outputFormat value="{0} Investigations found, displaying {1}, from {2} to {3}. Page {4} / {5}" styleClass="standard" >
                                        <f:param value="#{rowsCount}" />
                                        <f:param value="#{displayedRowsCountVar}" />
                                        <f:param value="#{firstRowIndex}" />
                                        <f:param value="#{lastRowIndex}" />
                                        <f:param value="#{pageIndex}" />
                                        <f:param value="#{pageCount}" />
                                    </h:outputFormat>
                                </t:dataScroller>
                            </h:panelGroup>           
                        </h:panelGrid>
                        
                    </td>
                </tr>
            </tbody>     
        </table>
        
        
    </a4j:form>
    
</a4j:region>