<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib prefix="ui" uri="http://java.sun.com/blueprints/ui/14" %>
<%@ taglib uri="https://ajax4jsf.dev.java.net/ajax" prefix="a4j"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>

<script type="text/javascript">
    function extractCityNav(citystatezip) {
        
        var index = citystatezip.indexOf(',');
        var nextcity = citystatezip.substring(0, index+4);   
        
        return citystatezip;
    }
    
    function extractCityNavAdv(citystatezip) {
        
        var index = citystatezip.indexOf(',');
        var nextcity = citystatezip.substring(0, index+4);   
        
        return citystatezip;
    }
    
    function extractCityNavISIS(citystatezip) {
        
        var index = citystatezip.indexOf(',');
        var nextcity = citystatezip.substring(0, index+4);   
        
        return citystatezip;
    }
    
    function chooseCityNavAdv(city) {
        
        //1.1.6
        var textField = document.getElementById('navigation_search:advancedSearch:autofillform_nav_adv:keywordField_nav_advj_id_1');  
        if(textField == null)  textField = document.getElementById('navigation_search:advancedSearch:autofillform_nav_adv:keywordField_nav_advj_id_2');  
        if(textField == null)  textField = document.getElementById('navigation_search:advancedSearch:autofillform_nav_adv:keywordField_nav_advj_id_3');  
        
        // 1.1.5 var textField = document.getElementById('navigation_search:autofillform_nav_adv:keywordField_nav_advj_id_1');
        //if(textField == null) textField = document.getElementById('navigation_search:autofillform_nav_adv:keywordField_nav_advj_id_2');
        //if(textField == null) textField = document.getElementById('navigation_search:autofillform_nav_adv:keywordField_nav_advj_id_3');   
        var oldvalue = textField.value;
        
        var index = oldvalue.lastIndexOf(' ');          
        var old = oldvalue.substring(0, index);     
        
        if(old == ""){       
            textField.value = city+" ";
        }
        else {     
            textField.value = old+" "+city+" ";
            
        }
    }
    
    function chooseCityNav(city) {
        // OLD way 1.1.6 
        var textField = document.getElementById('navigation_search:basicSearch:autofillform_nav:keywordField_navj_id_1');
        if(textField == null) textField = document.getElementById('navigation_search:basicSearch:autofillform_nav:keywordField_navj_id_2');
        if(textField == null) textField = document.getElementById('navigation_search:basicSearch:autofillform_nav:keywordField_navj_id_3');   
        
        
        //old way 1.1.5
        // var textField = document.getElementById('navigation_search:autofillform_nav:keywordField_navj_id_1');
        //if(textField == null) textField = document.getElementById('navigation_search:autofillform_nav:keywordField_navj_id_2');
        //if(textField == null) textField = document.getElementById('navigation_search:autofillform_nav:keywordField_navj_id_3');   
        
        var oldvalue = textField.value;  
        var index = oldvalue.lastIndexOf(' ');          
        var old = oldvalue.substring(0, index);     
        
        if(old == ""){       
            textField.value = city+" ";
            // OLD way 1.1.6  document.getElementById('navigation_search:basicSearch:autofillform_nav:keywordField_navj_id_1').value = city+" ";
        }
        else {     
            textField.value = old+" "+city+" ";
            // OLD way 1.1.6  document.getElementById('navigation_search:basicSearch:autofillform_nav:keywordField_navj_id_1').value = old+" "+city+" ";
            
        }
        
    }
    
    function chooseCityNavISIS(city) {
        //new wya
        var textField = document.getElementById('navigation_search:ISIS:autofillform_nav_isis:keywordField_nav_isisj_id_1');
        if(textField == null) textField = document.getElementById('navigation_search:ISIS:autofillform_nav_isis:keywordField_nav_isisj_id_2');
        if(textField == null) textField = document.getElementById('navigation_search:ISIS:autofillform_nav_isis:keywordField_nav_isisj_id_3');   
        
        
        // OLD way 1.1.5
        // var textField = document.getElementById('navigation_search:autofillform_nav_isis:keywordField_nav_isisj_id_1');
        //if(textField == null) textField = document.getElementById('navigation_search:autofillform_nav_isis:keywordField_nav_isisj_id_2');
        //if(textField == null) textField = document.getElementById('navigation_search:autofillform_nav_isis:keywordField_nav_isisj_id_3');   
        
        var oldvalue = textField.value;  
        var index = oldvalue.lastIndexOf(' ');          
        var old = oldvalue.substring(0, index);     
        
        if(old == ""){       
            textField.value = city+" ";
            // OLD way 1.1.6  document.getElementById('navigation_search:basicSearch:autofillform_nav:keywordField_navj_id_1').value = city+" ";
        }
        else {     
            textField.value = old+" "+city+" ";
            // OLD way 1.1.6  document.getElementById('navigation_search:basicSearch:autofillform_nav:keywordField_navj_id_1').value = old+" "+city+" ";
            
        }
        
    }
    
    
    
    
    </script>
<br />
<f:loadBundle basename="uk.ac.dl.dp.web.messages.facility" var="facility_properties"  />

<a4j:region  selfRendered="true"> 
    
    
    <t:panelTabbedPane selectedIndex="#{visit.tabIndex}" width="202px" bgcolor="#EAF4F4" serverSideTabSwitch="false" inactiveTabStyleClass="tabinactive" activeTabStyleClass="tabactive" >
        
        <t:panelTab id="basicSearch"  label="Keyword" >
            <h:form id="autofillform_nav">
                
                <h:panelGrid  border="0" columns="2"  >      
                    
                    <h:outputLabel rendered="#{!visit.singleFacility}" for="facilities_navj_id_1">
                        <h:outputText value="Search: " style="font-size:10px"/>
                    </h:outputLabel  >
                    
                    <h:panelGrid  rendered="#{!visit.singleFacility}">
                        <h:selectManyListbox id="facilities_navj_id_1" immediate="true"  value="#{visit.visitData.currentSelectedFacilities}" size="#{fn:length(visit.facilities)}" required="true" >      
                            <a4j:support event="onchange" action="#{keyword.selectedFacilities}" ajaxSingle="true" reRender="facilityDisplayNav,radioNav" />
                            <f:selectItems value="#{visit.facilities}"/>
                            <f:validateLength minimum="1" />
                        </h:selectManyListbox>
                    </h:panelGrid>
                    
                    <h:message  rendered="#{!visit.singleFacility}"  for="facilities_navj_id_1" styleClass="error"/>
                    <h:panelGroup  rendered="#{!visit.singleFacility}" />
                    
                    
                    
                    <h:outputLabel for="keywordField_navj_id_1">
                        <h:outputText value="Keyword(s): " style="font-size:10px"/>
                    </h:outputLabel  >                      
                    
                    <h:panelGrid id="inputKeywordField" >   
                        <ui:autoComplete rendered="#{sessionHistory.keywordSearchNavigationAutoComplete && sessionHistory.keywordSearchNavigationCaseSensitive}" styleClass="text" size="14" maxlength="60" id="keywordField_navj_id_1" 
                                         completionMethod="#{keyword.completeCityCaseSensitive}" 
                                         value="#{sessionHistory.basicSearchHistoryBean.keyword}" required="true"
                                         ondisplay="function(item) { return extractCityNav(item); }"
                                         onchoose="function(item) { return chooseCityNav(item); }"  validator="#{searchBean.validateKeyword}"/>
                        
                        <ui:autoComplete rendered="#{sessionHistory.keywordSearchNavigationAutoComplete && !sessionHistory.keywordSearchNavigationCaseSensitive}" styleClass="text" size="14" maxlength="60" id="keywordField_navj_id_2" 
                                         completionMethod="#{keyword.completeCityCaseInsensitive}" 
                                         value="#{sessionHistory.basicSearchHistoryBean.keyword}" required="true"
                                         ondisplay="function(item) { return extractCityNav(item); }"
                                         onchoose="function(item) { return chooseCityNav(item); }"  validator="#{searchBean.validateKeyword}"/>
                        
                        <h:inputText rendered="#{!sessionHistory.keywordSearchNavigationAutoComplete}" styleClass="text" size="14" maxlength="60" id="keywordField_navj_id_3"
                                     value="#{sessionHistory.basicSearchHistoryBean.keyword}" required="true" validator="#{searchBean.validateKeyword}" />
                    </h:panelGrid> 
                    
                    <h:panelGroup/>
                    
                    <h:panelGrid  columns="3" >
                        <h:message  for="keywordField_navj_id_1" styleClass="error"/>
                        <h:message  for="keywordField_navj_id_2" styleClass="error"/>
                        <h:message  for="keywordField_navj_id_3" styleClass="error"/>
                    </h:panelGrid>
                    
                    <h:panelGroup/>
                    
                    <h:panelGrid columns="4" >
                        
                        <h:selectBooleanCheckbox  style="font-size:12px" id="auto_nav"   required="true" value="#{sessionHistory.keywordSearchNavigationAutoComplete}">                                       
                            <a4j:support id="other" event="onclick" immediate="true" actionListener="#{sessionHistory.autoComplete}" ajaxSingle="true" reRender="inputKeywordField" >
                                <a4j:actionparam name="keyword_nav" /> 
                            </a4j:support>
                        </h:selectBooleanCheckbox>    
                        
                        
                        <h:outputLabel >            
                            <h:outputText  value="Auto " style="font-size: 10px" />                   
                        </h:outputLabel>
                        
                        <h:selectBooleanCheckbox style="font-size:12px" id="case" required="true" value="#{sessionHistory.keywordSearchNavigationCaseSensitive}">                                                           
                            <a4j:support id="other3" event="onclick" immediate="true" actionListener="#{sessionHistory.caseSensitive}" ajaxSingle="true" reRender="inputKeywordField">
                                <a4j:actionparam name="keyword_nav" /> 
                            </a4j:support>
                        </h:selectBooleanCheckbox> 
                        
                        
                        <h:outputLabel>            
                            <h:outputText value="Case " style="font-size: 10px" />                   
                        </h:outputLabel>        
                        
                    </h:panelGrid>  
                    
                    
                    
                    
                    <h:panelGroup/>
                    <h:panelGrid columns="2" >
                        <h:commandButton style="font-size:9px" id="f2" styleClass="button" action="#{searchBean.searchByKeywordNavigation}" onclick="busyBox.Show();" title="Search" value="Search"/>
                        
                        <h:commandButton style="font-size:9px" id="reset" type="reset" styleClass="button"  title="Reset" value="Reset"/>
                    </h:panelGrid>
                    
                </h:panelGrid>
            </h:form>
        </t:panelTab>
        
        <t:panelTab id="advancedSearch"  label="Advanced" >
            <h:form id="autofillform_nav_adv">
                
                <h:panelGrid  border="0" columns="2"  >      
                    
                    <h:outputLabel rendered="#{!visit.singleFacility}" for="facilities_nav_advj_id_1">
                        <h:outputText rendered="#{!visit.singleFacility}" value="Search: " style="font-size:10px"/>
                    </h:outputLabel  >
                    
                    <h:panelGrid rendered="#{!visit.singleFacility}">   
                        <h:selectManyListbox id="facilities_nav_advj_id_1" immediate="true"  value="#{sessionHistory.advancedSearchHistoryBean.selectedFacilities}" size="#{fn:length(visit.facilities)}" required="true" >      
                            <a4j:support event="onchange" action="#{keyword.selectedFacilities}" ajaxSingle="true" reRender="facilityDisplayNav,radioNav" />
                            <f:selectItems value="#{visit.facilities}"/>
                            <f:validateLength minimum="1" />
                        </h:selectManyListbox>
                    </h:panelGrid>   
                    
                    <h:message rendered="#{!visit.singleFacility}" for="facilities_nav_advj_id_1" styleClass="error"/>
                    <h:panelGroup rendered="#{!visit.singleFacility}" />
                    
                    <h:outputLabel for="keywordField_nav_advj_id_1">
                        <h:outputText value="Keyword(s): " style="font-size:10px"/>
                    </h:outputLabel>                      
                    
                    <h:panelGrid id="inputAdvancedKeywordField" >   
                        <ui:autoComplete rendered="#{sessionHistory.advancedSearchNavigationAutoComplete && sessionHistory.advancedSearchNavigationCaseSensitive}" styleClass="text" size="14" maxlength="60" id="keywordField_nav_advj_id_1" 
                                         completionMethod="#{keyword.completeCityCaseSensitive}" 
                                         value="#{sessionHistory.advancedSearchHistoryBean.keyword}" required="false"
                                         ondisplay="function(item) { return extractCityNavAdv(item); }"
                                         onchoose="function(item) { return chooseCityNavAdv(item); }"  validator="#{searchBean.validateKeyword}"/>
                        
                        <ui:autoComplete rendered="#{sessionHistory.advancedSearchNavigationAutoComplete && !sessionHistory.advancedSearchNavigationCaseSensitive}" styleClass="text" size="14" maxlength="60" id="keywordField_nav_advj_id_2" 
                                         completionMethod="#{keyword.completeCityCaseInsensitive}" 
                                         value="#{sessionHistory.advancedSearchHistoryBean.keyword}" required="false"
                                         ondisplay="function(item) { return extractCityNavAdv(item); }"
                                         onchoose="function(item) { return chooseCityNavAdv(item); }"  validator="#{searchBean.validateKeyword}"/>
                        
                        <h:inputText rendered="#{!sessionHistory.advancedSearchNavigationAutoComplete}" styleClass="text" size="14" maxlength="60" id="keywordField_nav_advj_id_3"
                                     value="#{sessionHistory.advancedSearchHistoryBean.keyword}" required="false" validator="#{searchBean.validateKeyword}" />
                    </h:panelGrid> 
                    
                    <h:panelGroup/>
                    
                    
                    <h:message rendered="#{sessionHistory.advancedSearchNavigationAutoComplete && sessionHistory.advancedSearchNavigationCaseSensitive}" for="keywordField_nav_advj_id_1" styleClass="error"/>
                    <h:message rendered="#{sessionHistory.advancedSearchNavigationAutoComplete && !sessionHistory.advancedSearchNavigationCaseSensitive}" for="keywordField_nav_advj_id_2" styleClass="error"/>
                    <h:message rendered="#{!sessionHistory.advancedSearchNavigationAutoComplete}" for="keywordField_nav_advj_id_3" styleClass="error"/>  
                    
                    <h:panelGroup/>
                    
                    <h:panelGrid columns="2" >
                        <h:selectBooleanCheckbox  style="font-size:12px" id="auto"   required="true" value="#{sessionHistory.advancedSearchNavigationAutoComplete}">                                       
                            <a4j:support id="other" event="onclick" immediate="true" actionListener="#{sessionHistory.autoComplete}" ajaxSingle="true" reRender="inputAdvancedKeywordField" >
                                <a4j:actionparam name="advanced_nav" /> 
                            </a4j:support>
                        </h:selectBooleanCheckbox>         
                        
                        <h:outputLabel >            
                            <h:outputText  value="Auto Complete" style="font-size: 10px" />                   
                        </h:outputLabel>
                    </h:panelGrid>
                    <!----------------     Start of Inv name  ---------------->
                    <h:outputLabel>            
                        <h:outputText value="Inv name:" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:inputText styleClass="text" id="invNamej_id_1" size="14" required="false" value="#{sessionHistory.advancedSearchHistoryBean.invName}">
                        
                    </h:inputText>        
                    
                    <h:panelGroup/>
                    <h:message for="invNamej_id_1" styleClass="error"/>
                    
                    <!----------------     End of Inv name ---------------->
                    
                    <!----------------     Start of Inv abstract ---------------->
                    <h:outputLabel>            
                        <h:outputText value="Inv abstract:" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:inputText styleClass="text" id="invAbstractj_id_1" size="14" required="false" value="#{sessionHistory.advancedSearchHistoryBean.invAbstract}">
                        
                    </h:inputText>        
                    
                    <h:panelGroup/>
                    <h:message for="invAbstractj_id_1" styleClass="error"/>
                    
                    <!----------------     End of Inv abstract ---------------->
                    
                    
                    
                    <!----------------     Start of sample ---------------->
                    <h:outputLabel>            
                        <h:outputText value="Sample" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:inputText styleClass="text" id="samplej_id_1" size="14" required="false" value="#{sessionHistory.advancedSearchHistoryBean.sample}">
                        
                    </h:inputText>        
                    
                    <h:panelGroup/>
                    <h:message for="samplej_id_1" styleClass="error"/>
                    
                    <!----------------     End of sample ---------------->
                    
                    <!----------------     Start of investigators ---------------->
                    <h:outputLabel>            
                        <h:outputText value="Surname:" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:inputText styleClass="text" id="investigatorsj_id_1" size="14" required="false" value="#{sessionHistory.advancedSearchHistoryBean.investigator}">
                        
                    </h:inputText>        
                    
                    <h:panelGroup/>
                    <h:message for="investigatorsj_id_1" styleClass="error"/>
                    
                    <!----------------     End of investigators ---------------->
                    
                    <!----------------     Start of Datafile name---------------->
                    <h:outputLabel>            
                        <h:outputText value="File name:" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:inputText styleClass="text" id="dfNamej_id_1" size="14" required="false" value="#{sessionHistory.advancedSearchHistoryBean.datafileName}" />
                    
                    <h:panelGroup/>
                    <h:message for="dfNamej_id_1" styleClass="error"/>
                    
                    <!----------------     End of Datafile name ---------------->
                    <h:panelGroup/>
                    
                    <h:panelGrid columns="2" >
                        <h:selectBooleanCheckbox style="font-size:12px" id="case" required="true" value="#{sessionHistory.advancedSearchNavigationCaseSensitive}">                                                           
                            <a4j:support id="other4" event="onclick" immediate="true" actionListener="#{sessionHistory.caseSensitive}" ajaxSingle="true" reRender="inputAdvancedKeywordField">
                                <a4j:actionparam name="advanced_nav" /> 
                            </a4j:support>
                        </h:selectBooleanCheckbox> 
                        
                        <h:outputLabel>            
                            <h:outputText value="Case Sensitive" style="font-size: 10px" />                   
                        </h:outputLabel>
                        
                    </h:panelGrid>                        
                    
                                       
                    <!----------------     Start of Run number ---------------->
                    <h:outputLabel>            
                        <h:outputText value="Run #" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    
                    <h:panelGroup >
                        
                        <h:inputText styleClass="text" id="runNumberMinj_id_1" size="5" required="false" binding="#{sessionHistory.advancedSearchHistoryBean.runStartUI}" value="#{sessionHistory.advancedSearchHistoryBean.runStart}" >
                            <f:validateDoubleRange minimum="0" maximum="90000000" />
                        </h:inputText>                
                        &nbsp;
                        <h:inputText styleClass="text" id="runNumberMaxj_id_1" size="5" required="false" validator="#{sessionHistory.advancedSearchHistoryBean.validateRun}" value="#{sessionHistory.advancedSearchHistoryBean.runEnd}" >
                            <f:validateDoubleRange minimum="0" maximum="90000000" />
                        </h:inputText>
                    </h:panelGroup>
                    
                    <h:panelGroup/>   
                    
                    <h:panelGroup>
                        <h:message for="runNumberMinj_id_1" styleClass="error"/>
                        <h:message for="runNumberMaxj_id_1" styleClass="error"/>
                    </h:panelGroup>
                    <!----------------     End of Run number ---------------->
                    
                    <!----------------     Start of Start Date ---------------->
                    
                    
                    <h:outputLabel>            
                        <h:outputText value="Date range:" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:panelGroup>
                        
                        <t:inputCalendar size="1" styleClass="text" binding="#{sessionHistory.advancedSearchHistoryBean.calendarFirst}" id="startDatej_id_1" required="false" monthYearRowClass="yearMonthHeader" weekRowClass="weekHeader" popupButtonStyleClass="standard_bold"
                                         currentDayCellClass="currentDayCell" value="#{sessionHistory.advancedSearchHistoryBean.firstDate}" renderAsPopup="true"
                                         popupTodayString="Today is:"
                                         popupDateFormat="dd/MM/yyyy" popupWeekString="Wk"
                                         helpText="" validator="#{sessionHistory.advancedSearchHistoryBean.validateDate}"/>  
                        
                        <t:inputCalendar size="1"  styleClass="text"  binding="#{sessionHistory.advancedSearchHistoryBean.calendarSecond}" id="endDatej_id_1" required="false" monthYearRowClass="yearMonthHeader" weekRowClass="weekHeader" popupButtonStyleClass="standard_bold"
                                         currentDayCellClass="currentDayCell" value="#{sessionHistory.advancedSearchHistoryBean.secondDate}" renderAsPopup="true"
                                         popupTodayString="Today is:" 
                                         popupDateFormat="dd/MM/yyyy" popupWeekString="Wk"
                                         helpText="" validator="#{sessionHistory.advancedSearchHistoryBean.validateDate}"/>  
                    </h:panelGroup>
                    
                    <h:panelGroup/>   
                    
                    <h:panelGroup>
                        <h:message for="startDatej_id_1" styleClass="error"/>
                        <h:message for="endDatej_id_1" styleClass="error"/>
                    </h:panelGroup>
                    <!----------------     End of Start Date ---------------->
                    
                    
                    <!----------------     Start of visit id ---------------->
                    <h:outputLabel>            
                        <h:outputText value="Visit Id" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:inputText styleClass="text" id="visitIdj_id_1" size="14" required="false" value="#{sessionHistory.advancedSearchHistoryBean.visitId}">
                        
                    </h:inputText>        
                    
                    <h:panelGroup/>
                    <h:message for="visitIdj_id_1" styleClass="error"/>
                    
                    <!----------------     End of visit id ---------------->
                    
                    <!----------------     Start of grant id ---------------->
                    <h:outputLabel>            
                        <h:outputText value="Grant Id" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:inputText styleClass="text" id="grantIdj_id_1" size="14" required="false" value="#{sessionHistory.advancedSearchHistoryBean.grantId}">
                        <f:validateDoubleRange minimum="0" maximum="90000000" />
                    </h:inputText>        
                    
                    <h:panelGroup/>
                    <h:message for="grantIdj_id_1" styleClass="error"/>
                    
                    <!----------------     End of grant id ---------------->
                    
                    <!----------------     Start of Inv no ---------------->
                    <h:outputLabel>            
                        <h:outputText value="Inv Number:" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:inputText styleClass="text" id="invNumj_id_1" size="14" required="false" value="#{sessionHistory.advancedSearchHistoryBean.invNumber}">                       
                    </h:inputText>        
                    
                    
                    <h:panelGroup/>
                    <h:message for="invNumj_id_1" showDetail="false" showSummary="true" styleClass="error"/>
                    
                    <!----------------     End of Inv no ---------------->
                    
                    <!----------------     Start of inv type---------------->
                    <h:outputLabel>            
                        <h:outputText value="Inv type" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:selectOneMenu id="invTypej_id_1" required="false" value="#{sessionHistory.advancedSearchHistoryBean.invType}" >                           
                        <f:selectItems value="#{visit.customisation.investigationTypeItems}" />                           
                    </h:selectOneMenu> 
                    
                    <%--        <h:inputText styleClass="text" id="invTypej_id_1" size="14" required="false" value="#{visit.visitData.advancedSearchBean.invType}">
                    
                    </h:inputText>        
                    --%>
                    
                    <h:panelGroup/>
                    <h:message for="invTypej_id_1" styleClass="error"/>
                    
                    <!----------------     End ofinv type---------------->
                    
                    <!----------------     Start of instruemnt ---------------->
                    <h:outputLabel>            
                        <h:outputText value="Instrument" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:selectOneMenu id="instrumentj_id_1" required="false" value="#{sessionHistory.advancedSearchHistoryBean.instrument}" >                           
                        <f:selectItems value="#{visit.customisation.instrumentsItems}" />                           
                    </h:selectOneMenu> 
                    
                    <%--  <h:inputText styleClass="text" id="instrumentj_id_1" size="14" required="false" value="#{visit.visitData.advancedSearchBean.instrument}">
                    
                    </h:inputText>        --%>
                    
                    <h:panelGroup/>
                    <h:message for="instrumentj_id_1" styleClass="error"/>
                    
                    <!----------------     End of instruemnt---------------->
                    
                    <h:panelGroup/>                  
                    
                    <h:panelGrid columns="2" >
                        <h:commandButton id="f28" style="font-size:9px" styleClass="button" action="#{advancedSearchBean.searchAdvancedNavigation}" onclick="busyBox.Show();" title="Search" value="Search"/>
                        
                        <h:commandButton style="font-size:9px" id="reset32" type="reset" styleClass="button"  title="Reset" value="Reset"/>
                    </h:panelGrid>
                    
                    
                </h:panelGrid>
            </h:form>
        </t:panelTab>
        
         <t:panelTab id="ISIS" label="ISIS" rendered="#{visit.customisation.facilitySearchPageVisible}">
            <h:form id="autofillform_nav_isis">
                
                <h:panelGrid  border="0" columns="2"  >      
                    
                    <h:outputLabel rendered="#{!visit.singleFacility}" for="facilities_nav_isisj_id_1">
                        <h:outputText rendered="#{!visit.singleFacility}" value="Search: " style="font-size:10px"/>
                    </h:outputLabel  >
                    
                    <h:panelGrid rendered="#{!visit.singleFacility}">   
                        <h:selectManyListbox id="facilities_nav_isisj_id_1" immediate="true"  value="#{sessionHistory.isisSearchHistoryBean.selectedFacilities}" size="#{fn:length(visit.facilities)}" required="true" >      
                            <a4j:support event="onchange" action="#{keyword.selectedFacilities}" ajaxSingle="true" reRender="facilityDisplayNav,radioNav" />
                            <f:selectItems value="#{visit.facilities}"/>
                            <f:validateLength minimum="1" />
                        </h:selectManyListbox>
                    </h:panelGrid>   
                    
                    <h:message rendered="#{!visit.singleFacility}" for="facilities_nav_isisj_id_1" styleClass="error"/>
                    <h:panelGroup rendered="#{!visit.singleFacility}" />
                    
                    <h:outputLabel for="keywordField_nav_isisj_id_1">
                        <h:outputText value="Keyword(s): " style="font-size:10px"/>
                    </h:outputLabel>                      
                    
                    <h:panelGrid id="inputisisKeywordField" >   
                        <ui:autoComplete rendered="#{sessionHistory.facilitySearchNavigationAutoComplete && sessionHistory.facilitySearchNavigationCaseSensitive}" styleClass="text" size="14" maxlength="60" id="keywordField_nav_isisj_id_1" 
                                         completionMethod="#{keyword.completeCityCaseSensitive}" 
                                         value="#{sessionHistory.isisSearchHistoryBean.keyword}" required="false"
                                         ondisplay="function(item) { return extractCityNavISIS(item); }"
                                         onchoose="function(item) { return chooseCityNavISIS(item); }"  validator="#{searchBean.validateKeyword}"/>
                        
                        <ui:autoComplete rendered="#{sessionHistory.facilitySearchNavigationAutoComplete && !sessionHistory.facilitySearchNavigationCaseSensitive}" styleClass="text" size="14" maxlength="60" id="keywordField_nav_isisj_id_2" 
                                         completionMethod="#{keyword.completeCityCaseInsensitive}" 
                                         value="#{sessionHistory.isisSearchHistoryBean.keyword}" required="false"
                                         ondisplay="function(item) { return extractCityNavISIS(item); }"
                                         onchoose="function(item) { return chooseCityNavISIS(item); }"  validator="#{searchBean.validateKeyword}"/>
                        
                        <h:inputText rendered="#{!sessionHistory.facilitySearchNavigationAutoComplete}" styleClass="text" size="14" maxlength="60" id="keywordField_nav_isisj_id_3"
                                     value="#{sessionHistory.isisSearchHistoryBean.keyword}" required="false" validator="#{searchBean.validateKeyword}" />
                    </h:panelGrid> 
                    
                    <h:panelGroup/>
                    
                    
                    <h:message rendered="#{sessionHistory.facilitySearchNavigationAutoComplete && sessionHistory.facilitySearchNavigationCaseSensitive}" for="keywordField_nav_isisj_id_1" styleClass="error"/>
                    <h:message rendered="#{sessionHistory.facilitySearchNavigationAutoComplete && !sessionHistory.facilitySearchNavigationCaseSensitive}" for="keywordField_nav_isisj_id_2" styleClass="error"/>
                    <h:message rendered="#{!sessionHistory.facilitySearchNavigationAutoComplete}" for="keywordField_nav_isisj_id_3" styleClass="error"/>  
                    
                    <h:panelGroup/>
                    
                    <h:panelGrid columns="4" >
                        <h:selectBooleanCheckbox  style="font-size:10px" id="auto"   required="true" value="#{sessionHistory.facilitySearchNavigationAutoComplete}">                                       
                            <a4j:support id="othedr" event="onclick" immediate="true" actionListener="#{sessionHistory.autoComplete}" ajaxSingle="true" reRender="inputisisKeywordField" >
                                <a4j:actionparam name="facility_nav" /> 
                            </a4j:support>
                        </h:selectBooleanCheckbox>         
                        
                        <h:outputLabel >            
                            <h:outputText  value="Auto" style="font-size: 10px" />                   
                        </h:outputLabel>
                        
                        <h:selectBooleanCheckbox style="font-size:12px" id="case" required="true" value="#{sessionHistory.facilitySearchNavigationCaseSensitive}">                                                           
                            <a4j:support id="otherd3" event="onclick" immediate="true" actionListener="#{sessionHistory.caseSensitive}" ajaxSingle="true" reRender="inputisisKeywordField">
                                <a4j:actionparam name="facility_nav" /> 
                            </a4j:support>
                        </h:selectBooleanCheckbox> 
                        
                        
                        <h:outputLabel>            
                            <h:outputText value="Case " style="font-size: 10px" />                   
                        </h:outputLabel> 
                    </h:panelGrid>                      
                    
                     <!----------------     Start of Start Date ---------------->
                                        
                    <h:outputLabel>            
                        <h:outputText value="Date range:" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:panelGroup>
                        
                        <t:inputCalendar size="1" styleClass="text" binding="#{sessionHistory.isisSearchHistoryBean.calendarFirst}" id="startDatej_id_1" required="false" monthYearRowClass="yearMonthHeader" weekRowClass="weekHeader" popupButtonStyleClass="standard_bold"
                                         currentDayCellClass="currentDayCell" value="#{sessionHistory.isisSearchHistoryBean.firstDate}" renderAsPopup="true"
                                         popupTodayString="Today is:"
                                         popupDateFormat="dd/MM/yyyy" popupWeekString="Wk"
                                         helpText="" validator="#{sessionHistory.isisSearchHistoryBean.validateDate}"/>  
                        
                        <t:inputCalendar size="1"  styleClass="text"  binding="#{sessionHistory.isisSearchHistoryBean.calendarSecond}" id="endDatej_id_1" required="false" monthYearRowClass="yearMonthHeader" weekRowClass="weekHeader" popupButtonStyleClass="standard_bold"
                                         currentDayCellClass="currentDayCell" value="#{sessionHistory.isisSearchHistoryBean.secondDate}" renderAsPopup="true"
                                         popupTodayString="Today is:" 
                                         popupDateFormat="dd/MM/yyyy" popupWeekString="Wk"
                                         helpText="" validator="#{sessionHistory.isisSearchHistoryBean.validateDate}"/>  
                    </h:panelGroup>
                    
                    <h:panelGroup/>   
                    
                    <h:panelGroup>
                        <h:message for="startDatej_id_1" styleClass="error"/>
                        <h:message for="endDatej_id_1" styleClass="error"/>
                    </h:panelGroup>
                    <!----------------     End of Start Date ---------------->                               
                                                           
                    <!----------------     Start of Run number ---------------->
                    <h:outputLabel>            
                        <h:outputText value="Run #" style="font-size: 10px" />                   
                    </h:outputLabel>
                                        
                    <h:panelGroup >
                        
                        <h:inputText styleClass="text" id="runNumberMinj_id_1" size="5" required="false" binding="#{sessionHistory.isisSearchHistoryBean.runStartUI}" value="#{sessionHistory.isisSearchHistoryBean.runStart}" >
                            <f:validateDoubleRange minimum="0" maximum="90000000" />
                        </h:inputText>                
                        &nbsp;
                        <h:inputText styleClass="text" id="runNumberMaxj_id_1" size="5" required="false" validator="#{sessionHistory.isisSearchHistoryBean.validateRun}" value="#{sessionHistory.isisSearchHistoryBean.runEnd}" >
                            <f:validateDoubleRange minimum="0" maximum="90000000" />
                        </h:inputText>
                    </h:panelGroup>
                    
                    <h:panelGroup/>   
                    
                    <h:panelGroup>
                        <h:message for="runNumberMinj_id_1" styleClass="error"/>
                        <h:message for="runNumberMaxj_id_1" styleClass="error"/>
                    </h:panelGroup>
                    <!----------------     End of Run number ---------------->
                    
                   
                    <!----------------     Start of instruemnt ---------------->
                    <h:outputLabel>            
                        <h:outputText value="Instrument" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:selectOneMenu id="instrumentj_id_1" required="false" value="#{sessionHistory.isisSearchHistoryBean.instrument}" >                           
                        <f:selectItems value="#{visit.customisation.instrumentsItems}" />                           
                    </h:selectOneMenu> 
                    
                    <%--  <h:inputText styleClass="text" id="instrumentj_id_1" size="14" required="false" value="#{visit.visitData.advancedSearchBean.instrument}">
                    
                    </h:inputText>        --%>
                    
                    <h:panelGroup/>
                    <h:message for="instrumentj_id_1" styleClass="error"/>
                    
                    <!----------------     End of instruemnt---------------->
                    
                    <h:panelGroup/>                  
                    
                    <h:panelGrid columns="2" >
                        <h:commandButton id="f54" style="font-size:9px" styleClass="button" action="#{advancedSearchBean.searchAdvancedNavigationISIS}" onclick="busyBox.Show();" title="Search" value="Search"/>
                        
                        <h:commandButton style="font-size:9px" id="reset4" type="reset" styleClass="button"  title="Reset" value="Reset"/>
                    </h:panelGrid>
                    
                    
                </h:panelGrid>
            </h:form>
            <br />
            
            <h:form>
                <h:panelGrid  border="0" columns="2"  >                      
                                   
                    <!----------------     Start of Run number ---------------->
                    <h:outputLabel>            
                        <h:outputText value="Run #" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:panelGroup >
                        
                        <h:inputText styleClass="text" id="runNumberMinDFj_id_1" size="5" required="false" binding="#{sessionHistory.isisSearchHistoryBean.runStartUIDF}" value="#{sessionHistory.isisSearchHistoryBean.runStartDF}" >
                            <f:validateDoubleRange minimum="0" maximum="90000000" />
                        </h:inputText>                
                        &nbsp;
                        <h:inputText styleClass="text" id="runNumberMaxDFj_id_1" size="5" required="false" validator="#{sessionHistory.isisSearchHistoryBean.validateRunDF}" value="#{sessionHistory.isisSearchHistoryBean.runEndDF}" >
                            <f:validateDoubleRange minimum="0" maximum="90000000" />
                        </h:inputText>
                    </h:panelGroup>
                    
                    <h:panelGroup/>   
                    
                    <h:panelGroup>
                        <h:message for="runNumberMinDFj_id_1" styleClass="error"/>
                        <h:message for="runNumberMaxDFj_id_1" styleClass="error"/>
                    </h:panelGroup>
                    <!----------------     End of Run number ---------------->
                    
                    
                    <!----------------     Start of instruemnt ---------------->
                    <h:outputLabel>            
                        <h:outputText value="Instrument " style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:selectOneMenu id="instrumentj_id_DF" required="false" value="#{sessionHistory.isisSearchHistoryBean.instrumentDF}" >                           
                        <f:selectItems value="#{visit.customisation.instrumentsItems}" />                           
                    </h:selectOneMenu> 
                    
                    <%--  <h:inputText styleClass="text" id="instrumentj_id_1" size="14" required="false" value="#{visit.visitData.advancedSearchBean.instrument}">
                    
                    </h:inputText>        --%>
                    
                    <h:panelGroup/>
                    <h:message for="instrumentj_id_DF" styleClass="error"/>
                    
                    <!----------------     End of instruemnt---------------->
                    
                    <h:panelGroup/>                  
                    
                    <h:panelGrid columns="2" >
                        <h:commandButton id="f54" style="font-size:9px" styleClass="button" action="#{advancedSearchBean.searchAdvancedNavigationISISDF}" onclick="busyBox.Show();" title="Search" value="Search"/>
                        
                        <h:commandButton style="font-size:9px" id="reset4" type="reset" styleClass="button"  title="Reset" value="Reset"/>
                    </h:panelGrid>
                    
                    
                </h:panelGrid>
            </h:form>
        </t:panelTab>      
        
        
    </t:panelTabbedPane>  
    
</a4j:region>

<iframe src="blank.html" id="BusyBoxIFrame" name="BusyBoxIFrame" frameBorder="0" scrolling="no" ondrop="return false;">
</iframe>
<SCRIPT>
    // Instantiate our BusyBox object
    var busyBox = new BusyBox("BusyBoxIFrame", "busyBox", 4, "../../images/gears_ani_", ".gif", 125, 147, 207);
    
    
    
    </SCRIPT>
