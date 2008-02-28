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
        
        
        <h:panelGrid  border="0" columns="4">      
            
            
            <h:outputLabel rendered="#{!visit.singleFacility}" for="facilitiesj_id_1">
                <h:outputText rendered="#{!visit.singleFacility}" value="Search: " style="font-size:14px"/>
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
            </h:outputLabel  >                      
            
            <h:panelGrid id="inputKeywordField" >   
                <ui:autoComplete rendered="#{sessionHistory.advancedSearchAutoComplete && sessionHistory.advancedSearchCaseSensitive}" styleClass="text" size="35" maxlength="60" id="keywordFieldj_id_1" 
                                 completionMethod="#{keyword.completeCaseSensitive}" 
                                 value="#{sessionHistory.advancedSearchHistoryBean.keywordForAdvancedSearch}" required="false"
                                 ondisplay="function(item) { return extractCity(item); }"
                                 onchoose="function(item) { return chooseCity(item); }"  validator="#{searchBean.validateKeyword}"/>
                
                <ui:autoComplete rendered="#{sessionHistory.advancedSearchAutoComplete && !sessionHistory.advancedSearchCaseSensitive}" styleClass="text" size="35" maxlength="60" id="keywordFieldj_id_2" 
                                 completionMethod="#{keyword.completeCaseInsensitive}" 
                                 value="#{sessionHistory.advancedSearchHistoryBean.keywordForAdvancedSearch}" required="false"
                                 ondisplay="function(item) { return extractCity(item); }"
                                 onchoose="function(item) { return chooseCity(item); }"  validator="#{searchBean.validateKeyword}"/>
                
                <h:inputText rendered="#{!sessionHistory.advancedSearchAutoComplete}" styleClass="text" size="35" maxlength="60" id="keywordFieldj_id_3"
                             value="#{sessionHistory.advancedSearchHistoryBean.keywordForAdvancedSearch}" required="false" validator="#{searchBean.validateKeyword}" >
                        <%--   <a4j:support id="noneAutoComplete" event="onkeyup" />--%>
                </h:inputText>
            </h:panelGrid> 
            
            <t:popup styleClass="popup" style="font-size: 14px; background-color: #D1E4E4;" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="15"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage url="../../images/help_16.png" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h3>Keyword(s)</h3>
                            <p> Use * to denote a wild card.  I.e.  *data, data* or *data*portal*<br />
                                <br />Searching ship* will match ships and shipping, when SHIP* will<br />
                            match SHIPS and shipping if Case Sensitive is turned off (unchecked)</p> 
                            
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
            
            <h:panelGrid columns="2" >
                
                <%--onchange="submit()" valueChangeListener="#{searchBean.autoCompleteEnabled}"--%>
                <h:selectBooleanCheckbox  style="font-size:12px" id="auto"   required="true" value="#{sessionHistory.advancedSearchAutoComplete}">                                       
                    <a4j:support id="other" event="onclick" immediate="true" actionListener="#{sessionHistory.autoComplete}" ajaxSingle="true" reRender="inputKeywordField" >
                        <a4j:actionparam name="advanced" /> 
                    </a4j:support>
                </h:selectBooleanCheckbox>         
                
                <h:outputLabel >            
                    <h:outputText  value="Auto Complete" style="font-size: 12px" />                   
                </h:outputLabel>
                
                
            </h:panelGrid>  
            
            <t:popup styleClass="popup" style="font-size: 14px; background-color: #D1E4E4;" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="15"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage url="../../images/help_16.png" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h3>Auto Complete</h3>
                            <p>'Yes' auto complete is enabled.  If enabled, the list is not a full <br />
                                list of all your possible keywords, only alpha numeric.<br /> 
                            'No' auto complete is disabled</p>
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup>
            
            <h:panelGroup/>
            
            <!--                End of toggle auto complete  -->  
            
            <!----------------     Start of Inv name  ---------------->
            <h:outputLabel>            
                <h:outputText value="Investigation name:" style="font-size: 14px" />                   
            </h:outputLabel>
            
            <h:inputText styleClass="text" id="invNamej_id_1" size="20" required="false" value="#{advancedSearchBean.invName}">
                
            </h:inputText>        
            
            <t:popup styleClass="popup" style="font-size: 14px; background-color: #D1E4E4;" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="15"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage url="../../images/help_16.png" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h3>Investigation name</h3>
                            <p>Seaches for the name/title of an investigation, use of wildcards (*) is allowed</p>                            
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup>
            
            <h:message for="invNamej_id_1" styleClass="error"/>
            
            <!----------------     End of Inv name ---------------->
            
            <!----------------     Start of Inv abstract ---------------->
            <h:outputLabel>            
                <h:outputText value="Investigation abstract:" style="font-size: 14px" />                   
            </h:outputLabel>
            
            <h:inputText styleClass="text" id="invAbstractj_id_1" size="20" required="false" value="#{advancedSearchBean.invAbstract}">
                
            </h:inputText>        
            
            <t:popup styleClass="popup" style="font-size: 14px; background-color: #D1E4E4;" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="15"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage url="../../images/help_16.png" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h3>Abstract</h3>
                            <p>Seaches for the abstract of an investigation, use of wildcards (*) is allowed</p>                            
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup>
            
            <h:message for="invAbstractj_id_1" styleClass="error"/>
            
            <!----------------     End of Inv abstract ---------------->
            
            <!----------------     Start of sample ---------------->
            <h:outputLabel>            
                <h:outputText value="Sample" style="font-size: 14px" />                   
            </h:outputLabel>
            
            <h:inputText styleClass="text" id="samplej_id_1" size="20" required="false" value="#{advancedSearchBean.sample}">
                
            </h:inputText>        
            
            <t:popup styleClass="popup" style="font-size: 14px; background-color: #D1E4E4;" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="15"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage url="../../images/help_16.png" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h3>Sample</h3>
                            <p>Seaches for a sample name used in investigation, use of wildcards (*) is allowed</p>                            
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup>
            
            <h:message for="samplej_id_1" styleClass="error"/>
            
            <!----------------     End of sample ---------------->
            
            <!----------------     Start of investigators ---------------->
            <h:outputLabel>            
                <h:outputText value="Investigator surname:" style="font-size: 14px" />                   
            </h:outputLabel>
            
            <h:inputText styleClass="text" id="investigatorsj_id_1" size="20" required="false" value="#{advancedSearchBean.investigator}">
                
            </h:inputText>        
            
            <t:popup styleClass="popup" style="font-size: 14px; background-color: #D1E4E4;" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="15"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage url="../../images/help_16.png" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h3>Investigator</h3>
                            <p>Seaches for a investigator surname asscoicated <br />
                            with an investigation, use of wildcards (*) is allowed</p>  
                            
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup>
            
            <h:message for="investigatorsj_id_1" styleClass="error"/>
            
            <!----------------     End of investigators ---------------->
            
            <!----------------     Start of Datafile name---------------->
            <h:outputLabel>            
                <h:outputText value="Datafile name:" style="font-size: 14px" />                   
            </h:outputLabel>
            
            <h:inputText styleClass="text" id="dfNamej_id_1" size="20" required="false" value="#{advancedSearchBean.datafileName}" />
            
            <t:popup styleClass="popup" style="font-size: 14px; background-color: #D1E4E4;" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="15"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage url="../../images/help_16.png" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h3>Datafile</h3>
                            <p>Seaches for a generated datafile that is associated <br />
                            with an investigation, use of wildcards (*) is allowed</p>                              
                            
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup>
            
            <h:message for="dfNamej_id_1" styleClass="error"/>
            
            <!----------------     End of Datafile name ---------------->
            
            <h:panelGroup/>
            
            <h:panelGrid columns="2" >
                
                
                <h:selectBooleanCheckbox style="font-size:12px" id="case" required="true" value="#{sessionHistory.advancedSearchCaseSensitive}">                                                           
                    <a4j:support id="other3" event="onclick" immediate="true" actionListener="#{sessionHistory.caseSensitive}" ajaxSingle="true" reRender="inputKeywordField">
                        <a4j:actionparam name="advanced" /> 
                    </a4j:support>
                </h:selectBooleanCheckbox> 
                
                <h:outputLabel>            
                    <h:outputText value="Case Sensitive" style="font-size: 12px" />                   
                </h:outputLabel>        
                
            </h:panelGrid>  
            
            <t:popup styleClass="popup" style="font-size: 14px; background-color: #D1E4E4;" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="15"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage url="../../images/help_16.png" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h3>Case Sensitive (for all columns above only)</h3>
                            <p> 'Yes' case is kept and searched<br />
                            'No' search is done regardless of case of input text (Case Insensitive), <br /> i.e SHIP will match ship and ShIp</p> 
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup>
            
            <h:panelGroup/>
            
            
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
                <t:popup styleClass="popup" style="font-size: 14px; background-color: #D1E4E4;" closePopupOnExitingElement="true"
                         closePopupOnExitingPopup="true"
                         displayAtDistanceX="15"
                         displayAtDistanceY="-40" >
                    
                    
                    <t:graphicImage url="../../images/help_16.png" border="0" />
                    <f:facet name="popup">
                        <h:panelGroup >
                            <h:panelGrid columns="1" >
                                <h3>Run number</h3>
                                <p>Seaches for a single or range of run numbers that is associated <br />
                                    with an investigation.<br />                                    
                                    <br />                                    
                                    Just enter the minimum and maximum values of a run number, <br />
                                    or enter minimum if you are only searching one run number and not a range.
                                </p>                
                                
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
            
            
            
            <!----------------     Start of Start Date ---------------->
            <h:outputLabel>            
                <h:outputText value="Start Date:" style="font-size: 14px" />                   
            </h:outputLabel>
            
            
            <t:inputCalendar size="15" styleClass="text" binding="#{advancedSearchBean.calendarFirst}" id="startDatej_id_1" required="false" monthYearRowClass="yearMonthHeader" weekRowClass="weekHeader" popupButtonStyleClass="standard_bold"
                             currentDayCellClass="currentDayCell" value="#{advancedSearchBean.firstDate}" renderAsPopup="true"
                             popupTodayString="Today is:"
                             popupDateFormat="dd/MM/yyyy" popupWeekString="Wk"
                             helpText="DD/MM/YYYY" validator="#{advancedSearchBean.validateDate}"/>  
            
            
            <t:popup styleClass="popup" style="font-size: 14px; background-color: #D1E4E4;" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="15"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage url="../../images/help_16.png" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h3>Start date</h3>
                            <p>Start date range for the investigation.<br />
                                <br />If this is left empty the search will have no start date, only a end date<br />
                                so it will return all the investigations before the end date
                            </p>
                            
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
            
            
            <t:popup styleClass="popup" style="font-size: 14px; background-color: #D1E4E4;" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="15"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage url="../../images/help_16.png" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h3>End date</h3>
                            <p>End date range for the investigation.<br />
                                <br />If this is left empty the search will have no end date, only a start date<br />
                                so it will return all the investigations after the start date
                            </p>
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup>
            
            <h:message for="endDatej_id_1" styleClass="error"/>
            
            
            <!----------------     End of End Date ---------------->
            
            <!----------------     Start of Visit Id ---------------->
            <h:outputLabel rendered="#{visit.customisation.visitIdVisible}">            
                <h:outputText rendered="#{visit.customisation.visitIdVisible}" value="Visit Id:" style="font-size: 14px" />                   
            </h:outputLabel>
            
            <h:inputText rendered="#{visit.customisation.visitIdVisible}" styleClass="text" id="visitIdj_id_1" size="20" required="false" value="#{advancedSearchBean.visitId}" />
            
            <t:popup rendered="#{visit.customisation.visitIdVisible}" styleClass="popup" style="font-size: 14px; background-color: #D1E4E4;" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="15"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage rendered="#{visit.customisation.visitIdVisible}" url="../../images/help_16.png" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h3>Visit Id</h3>
                            <p>Visit Id of the investigation.                            
                            </p>
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup>
            
            <h:message rendered="#{visit.customisation.visitIdVisible}" for="visitIdj_id_1" styleClass="error"/>
            
            <!----------------     End of Visit Id ---------------->
            
            <!----------------     Start of Grant Id ---------------->
            <h:outputLabel>            
                <h:outputText value="Grant Id:" style="font-size: 14px" />                   
            </h:outputLabel>
            
            <h:inputText styleClass="text" id="grantIdj_id_1" size="20" required="false" value="#{advancedSearchBean.grantId}">
                <f:validateDoubleRange minimum="0" maximum="9990000000" />
            </h:inputText>        
            
            <t:popup styleClass="popup" style="font-size: 14px; background-color: #D1E4E4;" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="15"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage url="../../images/help_16.png" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h3>Grant Id</h3>
                            <p>Grant Id of the investigation.                            
                            </p>
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup>
            
            <h:message for="grantIdj_id_1" showDetail="false" showSummary="true" styleClass="error"/>
            
            <!----------------     End of Grant Id ---------------->
            
            <!----------------     Start of Inv no ---------------->
            <h:outputLabel rendered="#{visit.customisation.invNumberVisible}">            
                <h:outputText rendered="#{visit.customisation.invNumberVisible}" value="Rb Number:" style="font-size: 14px" />                   
            </h:outputLabel>
            
            <h:inputText rendered="#{visit.customisation.invNumberVisible}" styleClass="text" id="invNumj_id_1" size="20" required="false" value="#{advancedSearchBean.invNumber}">
                
            </h:inputText>        
            
            <t:popup rendered="#{visit.customisation.invNumberVisible}" styleClass="popup" style="font-size: 14px; background-color: #D1E4E4;" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="15"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage rendered="#{visit.customisation.invNumberVisible}" url="../../images/help_16.png" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h3>Investigation number</h3>
                            <p>Investigation number of the investigation.                            
                            </p>
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup>
            
            <h:message rendered="#{visit.customisation.invNumberVisible}" for="invNumj_id_1" showDetail="false" showSummary="true" styleClass="error"/>
            
            <!----------------     End of Inv no ---------------->
            
            <h:panelGroup/>
            <h:panelGroup/>
            <h:panelGroup/>
            <h:panelGroup/>
            <h:panelGroup/>
            <h:panelGroup/>
            <h:panelGroup/>
            <h:panelGroup/>
            
            <!----------------     Start of Inv type ---------------->
            <h:outputLabel>            
                <h:outputText value="Investigation type:" style="font-size: 14px" />                   
            </h:outputLabel>
            
            <h:selectOneMenu id="invTypej_id_1" required="false" value="#{advancedSearchBean.invType}" >                           
                <f:selectItems value="#{visit.customisation.investigationTypeItems}" />                           
            </h:selectOneMenu>    
            
            <%--   <h:inputText styleClass="text" id="invTypej_id_1" size="20" required="false" value="#{advancedSearchBean.invType}">
            
            </h:inputText>      --%>  
            
            <t:popup styleClass="popup" style="font-size: 14px; background-color: #D1E4E4;" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="15"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage url="../../images/help_16.png" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h3>Investigation type</h3>
                            <p>Type of the investigation.                            
                            </p>
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup>
            
            <h:message for="invTypej_id_1" styleClass="error"/>
            
            <!----------------     End of Inv type ---------------->
            
            
            
            <!----------------     Start of instrument ---------------->
            <h:outputLabel>            
                <h:outputText value="Instrument: " style="font-size: 14px" />                   
            </h:outputLabel>
            
            <h:selectOneMenu id="instrumentj_id_1" required="false" value="#{advancedSearchBean.instrument}" >                           
                <f:selectItems value="#{visit.customisation.instrumentsItems}" />                           
            </h:selectOneMenu>           
            
            <%-- <h:inputText styleClass="text" id="instrumentj_id_1" size="20" required="false" value="#{advancedSearchBean.instrument}">
            
            </h:inputText>  --%>
            
            <t:popup styleClass="popup" style="font-size: 14px; background-color: #D1E4E4;" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="15"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage url="../../images/help_16.png" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >                                                            
                            <h3>Instrument</h3>
                            <p>Instrument used in investigation<br /><br />
                                If you do not select any it will search all intruments.
                            </p>
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup>
            
            <h:message for="instrumentj_id_1" styleClass="error"/>
            
            <!----------------     End of instrument ---------------->
            
            
            
            
            <h:panelGroup/>
            
            <h:panelGrid columns="3" >
                <h:commandButton id="f" styleClass="button" action="#{advancedSearchBean.searchAdvanced}" onclick="busyBox.Show();" title="Search" value="Search"/>
                &nbsp;
                <h:commandButton id="reset" type="reset" styleClass="button"  title="Reset" value="Reset"/>
            </h:panelGrid>
            
            <h:panelGroup/>
            <h:panelGroup/>
        </h:panelGrid>
        
        
    </h:form>
</a4j:region>