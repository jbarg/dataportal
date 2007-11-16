<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>

<%@ taglib prefix="ui" uri="http://java.sun.com/blueprints/ui/14" %>
<%@ taglib uri="https://ajax4jsf.dev.java.net/ajax" prefix="a4j"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>

<script type="text/javascript">
    function extractCity(citystatezip) {
    var index = citystatezip.indexOf(',');
    var nextcity = citystatezip.substring(0, index+4);
    //var oldvalue = document.getElementById('body:autofillform:facilities_SELECTED').value;
    //alert(oldvalue);
    
    return citystatezip;
    }
    
    
    function chooseCity(city) {
    var textField = document.getElementById('body:autofillform:keywordFieldj_id_1');
    if(textField == null) textField = document.getElementById('body:autofillform:keywordFieldj_id_2');
    if(textField == null) textField = document.getElementById('body:autofillform:keywordFieldj_id_3');   
    
    var oldvalue = textField.value;
    
    var index = oldvalue.lastIndexOf(' ');
    
    var old = oldvalue.substring(0, index);
    
    
    if(old == ""){
    
    textField.value = city+" ";        
    
    }
    else {
    
    textField.value = old+" "+city+" ";
    
    }
    //var oldvalue = document.getElementById('body:autofillform:facilities_SELECTED').options[0].value;
    
    
    }
    
    
    
</script>


<f:loadBundle basename="uk.ac.dl.dp.web.messages.facility" var="facility_properties"  />


<a4j:region  selfRendered="true"> 
    
    <h:panelGrid border="0" columns="2"> 
        <h:messages globalOnly="true" errorClass="error" infoClass="info" />
        
    </h:panelGrid>
    
   <h5>Investigation Search</h5>
        <h:form id="autofillform">
            
            <h:panelGrid id="searchPanelGrid" border="0" columns="4">      
                
                
                <h:outputLabel  rendered="#{!visit.singleFacility}" for="facilitiesj_id_1">
                    <h:outputText  rendered="#{!visit.singleFacility}" value="Search: " style="font-size:14px"/>
                </h:outputLabel  >
                
                <h:panelGrid rendered="#{!visit.singleFacility}" id="nothing" >              
                    <%--    <h:selectManyListbox id="facilities" immediate="true" onchange="submit()" valueChangeListener="#{keyword.selectedFacilities}" value="#{visit.visitData.currentSelectedFacilities}" size="3" required="true" >--%>
                    <h:selectManyListbox rendered="#{!visit.singleFacility}" id="facilitiesj_id_1" immediate="true"  value="#{visit.visitData.currentSelectedFacilities}" size="#{fn:length(visit.facilities)}" required="true" >      
                        <a4j:support event="onchange" action="#{keyword.selectedFacilities}" ajaxSingle="true" reRender="facilityDisplay,radio" />
                        <f:selectItems value="#{visit.facilities}"/>
                        <f:validateLength minimum="1" />
                    </h:selectManyListbox>       
                </h:panelGrid>
                
                <h:outputText rendered="#{!visit.singleFacility}" id="facilityDisplay" value="#{visit.visitData.currentSelectedFacilities}" style="font-size:14px"/>
                
                <h:message rendered="#{!visit.singleFacility}" for="facilitiesj_id_1" styleClass="error"/>
                
                <h:outputLabel for="keywordFieldj_id_1">
                    <h:outputText value="Keyword(s): " style="font-size:14px"/>
                </h:outputLabel>                      
                
                <h:panelGrid id="inputKeywordField" >   
                    <ui:autoComplete rendered="#{sessionHistory.facilitySearchAutoComplete && sessionHistory.facilitySearchCaseSensitive}" styleClass="text" size="35" maxlength="60" id="keywordFieldj_id_1" 
                                     completionMethod="#{keyword.completeCityCaseSensitive}" 
                                     value="#{advancedSearchBean.keyword}" required="false"
                                     ondisplay="function(item) { return extractCity(item); }"
                                     onchoose="function(item) { return chooseCity(item); }"  validator="#{searchBean.validateKeyword}"/>
                    
                    <ui:autoComplete rendered="#{sessionHistory.facilitySearchAutoComplete && !sessionHistory.facilitySearchCaseSensitive}"  styleClass="text" size="35" maxlength="60" id="keywordFieldj_id_2" 
                                     completionMethod="#{keyword.completeCityCaseInsensitive}" 
                                     value="#{advancedSearchBean.keyword}" required="false"
                                     ondisplay="function(item) { return extractCity(item); }"
                                     onchoose="function(item) { return chooseCity(item); }"  validator="#{searchBean.validateKeyword}"/>
                    
                    <h:inputText rendered="#{!sessionHistory.facilitySearchAutoComplete}"  styleClass="text" size="35" maxlength="60" id="keywordFieldj_id_3"
                                 value="#{advancedSearchBean.keyword}" required="false" validator="#{searchBean.validateKeyword}" />
                </h:panelGrid> 
                
                <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                         closePopupOnExitingPopup="true"
                         displayAtDistanceX="5"
                         displayAtDistanceY="-40">
                    
                    <t:graphicImage url="../../images/help.gif" border="0" />
                    <f:facet name="popup">
                        <h:panelGroup>
                            <h:panelGrid columns="1" >
                                <%-- <h:outputText value="Exact keyword match only. Case insensititve."/>   --%>
                                
                                <h:outputText value="This list is not a full list of all your possible keywords, only alpha numeric."/>                                 
                                <h:outputText value="'AND' type searches returns all results"/>                                 
                                <h:outputText escape="true" value=" that contain all the keywords, i.e. default Google behaviour."/> 
                                <h:outputText value="Exact or Like match depending what you select below."/>                              
                            </h:panelGrid>
                        </h:panelGroup>
                    </f:facet>
                </t:popup> 
                
                
                <h:panelGrid  columns="3" >
                    <h:message for="keywordFieldj_id_1" styleClass="error"/>
                    <h:message for="keywordFieldj_id_2" styleClass="error"/>
                    <h:message  for="keywordFieldj_id_3" styleClass="error"/>                
                </h:panelGrid>    
                <!----------------     Start of auto complete toggle ---------------->
                
                <h:panelGroup/>
                
                <h:panelGrid columns="4" >
                    
                    <%--onchange="submit()" valueChangeListener="#{searchBean.autoCompleteEnabled}"--%>
                    <h:selectBooleanCheckbox  style="font-size:12px"  id="auto"   required="true" value="#{sessionHistory.facilitySearchAutoComplete}">                                       
                        <a4j:support id="other" event="onclick" immediate="true" actionListener="#{sessionHistory.autoComplete}" ajaxSingle="true" reRender="inputKeywordField" >
                            <a4j:actionparam name="facility" /> 
                        </a4j:support>
                    </h:selectBooleanCheckbox>         
                    
                    <h:outputLabel >            
                        <h:outputText  value="Auto Complete" style="font-size: 12px" />                   
                    </h:outputLabel>
                    
                    <h:selectBooleanCheckbox style="font-size:12px"  id="case" required="true" value="#{sessionHistory.facilitySearchCaseSensitive}">                                                           
                        <a4j:support id="other3" event="onclick" immediate="true" actionListener="#{sessionHistory.caseSensitive}" ajaxSingle="true" reRender="inputKeywordField">
                            <a4j:actionparam name="facility" /> 
                        </a4j:support>
                    </h:selectBooleanCheckbox> 
                    
                    <h:outputLabel>            
                        <h:outputText value="Case Sensitive" style="font-size: 12px" />                   
                    </h:outputLabel>   
                    
                </h:panelGrid>  
                
                <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                         closePopupOnExitingPopup="true"
                         displayAtDistanceX="5"
                         displayAtDistanceY="-40" >
                    
                    <t:graphicImage url="../../images/help.gif" border="0" />
                    <f:facet name="popup">
                        <h:panelGroup>
                            <h:panelGrid columns="1" >
                                <h:outputText escape="false" value="Auto Complete"/>
                                <h:outputText escape="false" value=" &nbsp; &nbsp;   'Yes' auto complete for keyword(s) is enabled"/>                                                             
                                <h:outputText escape="false" value="  &nbsp; &nbsp;  'No' auto complete for keyword(s) is disabled"/>   
                                
                                <h:outputText value="Case Sensitive "/>
                                <h:outputText escape="false" value=" &nbsp; &nbsp;  'Yes' search is done regardless of case of keyword"/>                                                             
                                <h:outputText escape="false" value=" &nbsp; &nbsp;  'No' keyword case is kept and searched"/>   
                                
                            </h:panelGrid>
                        </h:panelGroup>
                    </f:facet>
                </t:popup>
                
                <h:panelGroup/>
                
                <!--                End of toggle auto complete  -->  
                
                <!----------------     Start of Like ---------------->
                <h:outputLabel>            
                    <h:outputText value="Type:" style="font-size: 14px" />                   
                </h:outputLabel>
                
                <%--<h:selectBooleanCheckbox value="#{searchBean.logicalExpressionBoolean}"  />--%>
                <h:selectOneRadio style="font-size:12px"  id="like" value="#{advancedSearchBean.likeExpression}" >
                    <f:selectItem itemLabel="Exact"  itemValue="EXACT" /> 
                    <f:selectItem itemLabel="Like"  itemValue="LIKE" />
                </h:selectOneRadio>       
                
                <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                         closePopupOnExitingPopup="true"
                         displayAtDistanceX="5"
                         displayAtDistanceY="-40" >
                    
                    <t:graphicImage url="../../images/help.gif" border="0" />
                    <f:facet name="popup">
                        <h:panelGroup>
                            <h:panelGrid columns="1" >
                                <h:outputText value="Type of search " />
                                <h:outputText value="'Exact' type searches returns all results"/>                                 
                                <h:outputText escape="true" value=" that exactly match the search text."/> 
                                <h:outputText value="'Like' type searches returns all results"/>                             
                                <h:outputText  escape="true" value="that contain part of the search text."/>  
                                <h:outputText  escape="true" value="I.e. oxygen would return results with oxygens."/>              
                            </h:panelGrid>
                        </h:panelGroup>
                    </f:facet>
                </t:popup>
                
                
                <h:panelGroup/>
                <!----------------     End of Like ---------------->
                
                <h:panelGroup/>
                <h:panelGroup/>
                <h:panelGroup/>
                <h:panelGroup/>
                <h:panelGroup/>
                <h:panelGroup/>
                <h:panelGroup/>
                <h:panelGroup/>
                
                
                
                
                <!----------------     Start of Start Date ---------------->
                <h:outputLabel>            
                    <h:outputText value="Start Date:" style="font-size: 14px" />                   
                </h:outputLabel>
                
                
                <t:inputCalendar size="15" styleClass="text"  binding="#{advancedSearchBean.calendarFirst}" id="startDatej_id_1" required="false" monthYearRowClass="yearMonthHeader" weekRowClass="weekHeader" popupButtonStyleClass="standard_bold"
                                 currentDayCellClass="currentDayCell" value="#{advancedSearchBean.firstDate}" renderAsPopup="true"
                                 popupTodayString="Today is:"
                                 popupDateFormat="dd/MM/yyyy" popupWeekString="Wk"
                                 helpText="DD/MM/YYYY" validator="#{advancedSearchBean.validateDate}"/>  
                
                
                <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                         closePopupOnExitingPopup="true"
                         displayAtDistanceX="5"
                         displayAtDistanceY="-40" >
                    
                    <t:graphicImage url="../../images/help.gif" border="0" />
                    <f:facet name="popup">
                        <h:panelGroup>
                            <h:panelGrid columns="1" >
                                <h:outputText value="Start date range."/>                                 
                                
                            </h:panelGrid>
                        </h:panelGroup>
                    </f:facet>
                </t:popup>
                
                
                <h:message for="startDatej_id_1" styleClass="error"/>
                
                <!----------------     End of Start Date ---------------->
                
                <!----------------     Start of End Date ---------------->
                <h:outputLabel>            
                    <h:outputText value="End Date:" style="font-size: 14px" />                   
                </h:outputLabel>
                
                
                <t:inputCalendar size="15" styleClass="text" binding="#{advancedSearchBean.calendarSecond}" id="endDatej_id_1" required="false" monthYearRowClass="yearMonthHeader" weekRowClass="weekHeader" popupButtonStyleClass="standard_bold"
                                 currentDayCellClass="currentDayCell" value="#{advancedSearchBean.secondDate}" renderAsPopup="true"
                                 popupTodayString="Today is:"
                                 popupDateFormat="dd/MM/yyyy" popupWeekString="Wk"
                                 helpText="DD/MM/YYYY" validator="#{advancedSearchBean.validateDate}"/>  
                
                
                <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                         closePopupOnExitingPopup="true"
                         displayAtDistanceX="5"
                         displayAtDistanceY="-40" >
                    
                    <t:graphicImage url="../../images/help.gif" border="0" />
                    <f:facet name="popup">
                        <h:panelGroup>
                            <h:panelGrid columns="1" >
                                <h:outputText value="End date range."/>                                 
                                
                            </h:panelGrid>
                        </h:panelGroup>
                    </f:facet>
                </t:popup>
                
                <h:message for="endDatej_id_1" styleClass="error"/>
                
                
                <!----------------     End of End Date ---------------->
                
                
                <h:panelGroup/>
                <h:panelGroup/>
                <h:panelGroup/>
                <h:panelGroup/>
                <h:panelGroup/>
                <h:panelGroup/>
                <h:panelGroup/>
                <h:panelGroup/>
                
                <!----------------     Start of Run number ---------------->
                <h:outputLabel>            
                    <h:outputText value="Run Number (To - From):" style="font-size: 14px" />                   
                </h:outputLabel>
                
                <h:panelGroup>
                    
                    <h:inputText styleClass="text" id="runNumberMinj_id_1" size="8" required="false" binding="#{advancedSearchBean.runStartUI}" value="#{advancedSearchBean.runStart}" >
                        <f:validateDoubleRange minimum="0" maximum="90000000" />
                    </h:inputText>                
                    &nbsp;
                    <h:inputText styleClass="text" id="runNumberMaxj_id_1" size="8" required="false" validator="#{advancedSearchBean.validateRun}" value="#{advancedSearchBean.runEnd}" >
                        <f:validateDoubleRange minimum="0" maximum="90000000" />
                    </h:inputText>
                </h:panelGroup>
                
                <h:panelGroup>
                    <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                             closePopupOnExitingPopup="true"
                             displayAtDistanceX="5"
                             displayAtDistanceY="-40" 
                             
                             >
                        
                        <t:graphicImage url="../../images/help.gif" border="0" />
                        <f:facet name="popup">
                            <h:panelGroup >
                                <h:panelGrid columns="1" >
                                    <h:outputText value="Minimum and maximum values of a run number."/>                                 
                                    <h:outputText value="Just enter minimum if you are only search one run number and not a range."/> 
                                </h:panelGrid>
                            </h:panelGroup>
                        </f:facet>
                    </t:popup>
                </h:panelGroup>
                
                <h:panelGroup>
                    <h:message for="runNumberMinj_id_1" styleClass="error"/>&nbsp;&nbsp;
                    <h:message for="runNumberMaxj_id_1" styleClass="error"/>
                </h:panelGroup>
                <!----------------     End of Run number ---------------->
                
                <!----------------     Start of instrument ---------------->
                <h:outputLabel>            
                    <h:outputText value="Instrument: " style="font-size: 14px" />                   
                </h:outputLabel>
                
                <h:selectOneMenu id="instrumentj_id_1" required="false" value="#{advancedSearchBean.instrument}" >                           
                    <f:selectItems value="#{visit.customisation.instrumentsItems}" />                           
                </h:selectOneMenu>           
                
                <%-- <h:inputText styleClass="text" id="instrumentj_id_1" size="20" required="false" value="#{advancedSearchBean.instrument}">
                
                </h:inputText>  --%>
                
                <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                         closePopupOnExitingPopup="true"
                         displayAtDistanceX="5"
                         displayAtDistanceY="-40" >
                    
                    <t:graphicImage url="../../images/help.gif" border="0" />
                    <f:facet name="popup">
                        <h:panelGroup>
                            <h:panelGrid columns="1" >
                                <h:outputText value="Instrument used in investigation."/>                                 
                                
                            </h:panelGrid>
                        </h:panelGroup>
                    </f:facet>
                </t:popup>
                
                <h:message for="instrumentj_id_1" styleClass="error"/>
                
                <!----------------     End of instrument ---------------->
                
                
                
                <h:panelGroup/>
                
                <h:panelGrid columns="3" >
                    <h:commandButton id="f" styleClass="button" action="#{advancedSearchBean.searchAdvancedISIS}" onclick="busyBox.Show();" title="Search" value="Search"/>
                    &nbsp;
                    <h:commandButton id="reset" type="reset" styleClass="button"  title="Reset" value="Reset"/>
                </h:panelGrid>
                
                <h:panelGroup/>
                <h:panelGroup/>
            </h:panelGrid>
        </h:form>
        
        <h5>Data File Search</h5>
        <!--------------  DATAFILE SEARCH ------------------------>
        <h:form>
            <h:panelGrid id="searchPanelGridDF" border="0" columns="4">   
                <!----------------     Start of Run number ---------------->
                <h:outputLabel>            
                    <h:outputText value="Run Number (To - From):" style="font-size: 14px" />                   
                </h:outputLabel>
                
                <h:panelGroup>
                    
                    <h:inputText styleClass="text" id="runNumberMinDFj_id_1" size="8" required="true" binding="#{advancedSearchBean.runStartUIDF}" value="#{advancedSearchBean.runStartDF}" >
                        <f:validateDoubleRange minimum="0" maximum="90000000" />
                    </h:inputText>                
                    &nbsp;
                    <h:inputText styleClass="text" id="runNumberMaxDFj_id_1" size="8" required="false" validator="#{advancedSearchBean.validateRunDF}" value="#{advancedSearchBean.runEndDF}" >
                        <f:validateDoubleRange minimum="0" maximum="90000000" />
                    </h:inputText>
                </h:panelGroup>
                
                <h:panelGroup>
                    <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                             closePopupOnExitingPopup="true"
                             displayAtDistanceX="5"
                             displayAtDistanceY="-40" 
                             
                             >
                        
                        <t:graphicImage url="../../images/help.gif" border="0" />
                        <f:facet name="popup">
                            <h:panelGroup >
                                <h:panelGrid columns="1" >
                                    <h:outputText value="Minimum and maximum values of a run number."/>                                 
                                    <h:outputText value="Just enter minimum if you are only search one run number and not a range."/> 
                                </h:panelGrid>
                            </h:panelGroup>
                        </f:facet>
                    </t:popup>
                </h:panelGroup>
                
                <h:panelGroup>
                    <h:message for="runNumberMinDFj_id_1" styleClass="error"/>&nbsp;&nbsp;
                    <h:message for="runNumberMaxDFj_id_1" styleClass="error"/>
                </h:panelGroup>
                <!----------------     End of Run number ---------------->
                
                <!----------------     Start of instrument ---------------->
                <h:outputLabel>            
                    <h:outputText value="Instrument: " style="font-size: 14px" />                   
                </h:outputLabel>
                
                <h:selectOneMenu id="instrumentDFj_id_1" required="true" value="#{advancedSearchBean.instrumentDF}" >                           
                    <f:selectItems value="#{visit.customisation.instrumentsItems}" />                           
                </h:selectOneMenu>           
                
                
                <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                         closePopupOnExitingPopup="true"
                         displayAtDistanceX="5"
                         displayAtDistanceY="-40" >
                    
                    <t:graphicImage url="../../images/help.gif" border="0" />
                    <f:facet name="popup">
                        <h:panelGroup>
                            <h:panelGrid columns="1" >
                                <h:outputText value="Instrument used creating datafile."/>                                 
                                
                            </h:panelGrid>
                        </h:panelGroup>
                    </f:facet>
                </t:popup>
                
                <h:message for="instrumentDFj_id_1" styleClass="error"/>
                
                <!----------------     End of instrument ---------------->
                
                
                
                <h:panelGroup/>
                
                <h:panelGrid columns="3" >
                    <h:commandButton id="fDF" styleClass="button" action="#{advancedSearchBean.searchAdvancedISISDF}" onclick="busyBox.Show();" title="Search" value="Search"/>
                    &nbsp;
                    <h:commandButton id="resetDF" type="reset" styleClass="button"  title="Reset" value="Reset"/>
                </h:panelGrid>
                
                <h:panelGroup/>
                <h:panelGroup/>
            </h:panelGrid>
        </h:form>
    
    
    
</a4j:region>