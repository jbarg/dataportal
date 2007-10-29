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
      
       function chooseCityNavAdv(city) {
    
        var oldvalue = document.getElementById('navigation_search:autofillform_nav_adv:keywordField_nav_advj_id_1').value;  
     
         // OLD way 1.1.6   var oldvalue = document.getElementById('navigation_search:advancedSearch:autofillform_nav_adv:keywordField_nav_advj_id_1').value;  
        var index = oldvalue.lastIndexOf(' ');          
        var old = oldvalue.substring(0, index);     
      
        if(old == ""){       
            document.getElementById('navigation_search:autofillform_nav_adv:keywordField_nav_advj_id_1').value = city+" ";
            // OLD way 1.1.6 document.getElementById('navigation_search:advancedSearch:autofillform_nav_adv:keywordField_nav_advj_id_1').value = city+" ";
        }
        else {     
           document.getElementById('navigation_search:autofillform_nav_adv:keywordField_nav_advj_id_1').value = old+" "+city+" ";
            // OLD way 1.1.6 document.getElementById('navigation_search:advancedSearch:autofillform_nav_adv:keywordField_nav_advj_id_1').value = old+" "+city+" ";
        
            }
        }

      function chooseCityNav(city) {
                
      // OLD way 1.1.6  var oldvalue = document.getElementById('navigation_search:basicSearch:autofillform_nav:keywordField_navj_id_1').value;         
        var oldvalue = document.getElementById('navigation_search:autofillform_nav:keywordField_navj_id_1').value;  
        var index = oldvalue.lastIndexOf(' ');          
        var old = oldvalue.substring(0, index);     
   
        if(old == ""){       
           document.getElementById('navigation_search:autofillform_nav:keywordField_navj_id_1').value = city+" ";
            // OLD way 1.1.6  document.getElementById('navigation_search:basicSearch:autofillform_nav:keywordField_navj_id_1').value = city+" ";
        }
        else {     
            document.getElementById('navigation_search:autofillform_nav:keywordField_navj_id_1').value = old+" "+city+" ";
           // OLD way 1.1.6  document.getElementById('navigation_search:basicSearch:autofillform_nav:keywordField_navj_id_1').value = old+" "+city+" ";
      
        }
                     
      }
                   
       
</script>
<br />
<f:loadBundle basename="uk.ac.dl.dp.web.messages.facility" var="facility_properties"  />

<a4j:region  selfRendered="true"> 
    
    
    <t:panelTabbedPane selectedIndex="#{visit.tabIndex}" width="202px" bgcolor="#D1E4E4" serverSideTabSwitch="false" inactiveTabStyleClass="tabinactive" activeTabStyleClass="tabactive" >
        <t:panelTab id="basicSearch"  label="Basic" >
            <h:form id="autofillform_nav">
                
                <h:panelGrid  border="0" columns="2"  >      
                    
                    <h:outputLabel for="facilities_navj_id_1">
                        <h:outputText value="Search: " style="font-size:10px"/>
                    </h:outputLabel  >
                    
                    <h:selectManyListbox id="facilities_navj_id_1" immediate="true"  value="#{visit.visitData.currentSelectedFacilities}" size="#{fn:length(visit.facilities)}" required="true" >      
                        <a4j:support event="onchange" action="#{keyword.selectedFacilities}" ajaxSingle="true" reRender="facilityDisplayNav,radioNav" />
                        <f:selectItems value="#{visit.facilities}"/>
                        <f:validateLength minimum="1" />
                    </h:selectManyListbox>
                    
                    
                    <h:message  for="facilities_navj_id_1" styleClass="error"/>
                    <h:panelGroup/>
                    
                    
                    
                    <h:outputLabel for="keywordField_navj_id_1">
                        <h:outputText value="Keyword(s): " style="font-size:10px"/>
                    </h:outputLabel  >                      
                    
                    <ui:autoComplete  styleClass="text" size="14" maxlength="60" id="keywordField_navj_id_1" 
                                      completionMethod="#{keyword.completeCity}" 
                                      value="#{visit.visitData.basicSearchBean.keyword}" required="true"
                                      ondisplay="function(item) { return extractCityNav(item); }"
                                      onchoose="function(item) { return chooseCityNav(item); }"  validator="#{searchBean.validateKeyword}"/>
                    
                    
                    <h:panelGroup/>
                    <h:message for="keywordField_navj_id_1" styleClass="error"/>
                    
                    
                    <h:outputLabel>            
                        <h:outputText escape="false" value="Type:" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:selectOneRadio  style="font-size:10px" id="radioNav" value="#{visit.visitData.basicSearchBean.logicalExpression}" >
                        <f:selectItems value="#{searchBean.logicalExpressions}"/> 
                        <f:validateLength minimum="1" />
                    </h:selectOneRadio>         
                    
                    
                    <h:panelGroup/>
                    
                    <%--<h:selectBooleanCheckbox value="#{searchBean.logicalExpressionBoolean}"  />--%>
                    <h:selectOneRadio style="font-size:10px" id="likeNav" value="#{visit.visitData.basicSearchBean.likeExpression}" >
                        <f:selectItem itemLabel="Exact"  itemValue="EXACT" />                         
                        <f:selectItem itemLabel="Like"  itemValue="LIKE" />
                        <f:validateLength minimum="1" />
                    </h:selectOneRadio>       
                    
                    <h:panelGroup/>           
                    <h:panelGroup/>
                    
                    
                    <h:panelGroup/>
                    <h:commandButton id="f2" styleClass="button" action="#{searchBean.searchByKeywordNavigation}" onclick="busyBox.Show();" title="Search" value="Search"/>
                    
                    
                </h:panelGrid>
            </h:form>
        </t:panelTab>
        
        <t:panelTab id="advancedSearch"  label="Advanced" >
            <h:form id="autofillform_nav_adv">
                
                <h:panelGrid  border="0" columns="2"  >      
                    
                    <h:outputLabel for="facilities_nav_advj_id_1">
                        <h:outputText value="Search: " style="font-size:10px"/>
                    </h:outputLabel  >
                    
                    <h:selectManyListbox id="facilities_nav_advj_id_1" immediate="true"  value="#{visit.visitData.currentSelectedFacilities}" size="#{fn:length(visit.facilities)}" required="true" >      
                        <a4j:support event="onchange" action="#{keyword.selectedFacilities}" ajaxSingle="true" reRender="facilityDisplayNav,radioNav" />
                        <f:selectItems value="#{visit.facilities}"/>
                        <f:validateLength minimum="1" />
                    </h:selectManyListbox>
                    
                    <h:message  for="facilities_nav_advj_id_1" styleClass="error"/>
                    <h:panelGroup/>
                    
                    
                    <h:outputLabel for="keywordField_nav_advj_id_1">
                        <h:outputText value="Keyword(s): " style="font-size:10px"/>
                    </h:outputLabel>                      
                    
                    <ui:autoComplete  styleClass="text" size="14" maxlength="60" id="keywordField_nav_advj_id_1" 
                                      completionMethod="#{keyword.completeCity}" 
                                      value="#{visit.visitData.advancedSearchBean.keyword}" required="false"
                                      ondisplay="function(item) { return extractCityNavAdv(item); }"
                                      onchoose="function(item) { return chooseCityNavAdv(item); }" />
                    
                    <h:panelGroup/>
                    <h:message for="keywordField_nav_advj_id_1" styleClass="error"/>
                    
                    
                    <!----------------     Start of Inv name  ---------------->
                    <h:outputLabel>            
                        <h:outputText value="Inv name:" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:inputText styleClass="text" id="invNamej_id_1" size="14" required="false" value="#{visit.visitData.advancedSearchBean.invName}">
                        
                    </h:inputText>        
                    
                    <h:panelGroup/>
                    <h:message for="invNamej_id_1" styleClass="error"/>
                    
                    <!----------------     End of Inv name ---------------->
              
                    <!----------------     Start of Inv abstract ---------------->
                    <h:outputLabel>            
                        <h:outputText value="Inv abstract:" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:inputText styleClass="text" id="invAbstractj_id_1" size="14" required="false" value="#{visit.visitData.advancedSearchBean.invAbstract}">
                        
                    </h:inputText>        
                    
                    <h:panelGroup/>
                    <h:message for="invAbstractj_id_1" styleClass="error"/>
                    
                    <!----------------     End of Inv abstract ---------------->
            
                        
              
                    <!----------------     Start of sample ---------------->
                    <h:outputLabel>            
                        <h:outputText value="Sample" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:inputText styleClass="text" id="samplej_id_1" size="14" required="false" value="#{visit.visitData.advancedSearchBean.sample}">
                        
                    </h:inputText>        
                    
                    <h:panelGroup/>
                    <h:message for="samplej_id_1" styleClass="error"/>
                    
                    <!----------------     End of sample ---------------->
              
                    <!----------------     Start of investigators ---------------->
                    <h:outputLabel>            
                        <h:outputText value="Surname:" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:inputText styleClass="text" id="investigatorsj_id_1" size="14" required="false" value="#{visit.visitData.advancedSearchBean.investigator}">
                        
                    </h:inputText>        
                    
                    <h:panelGroup/>
                    <h:message for="investigatorsj_id_1" styleClass="error"/>
                    
                    <!----------------     End of investigators ---------------->
            
                    <!----------------     Start of Datafile name---------------->
                    <h:outputLabel>            
                        <h:outputText value="File name:" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:inputText styleClass="text" id="dfNamej_id_1" size="14" required="false" value="#{visit.visitData.advancedSearchBean.datafileName}" />
                    
                    <h:panelGroup/>
                    <h:message for="dfNamej_id_1" styleClass="error"/>
                    
                    <!----------------     End of Datafile name ---------------->
            
                    <!----------------     Start of Like ---------------->
                    <h:outputLabel>            
                        <h:outputText escape="false" value="Type:" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:selectOneRadio style="font-size:12px" id="like" value="#{visit.visitData.advancedSearchBean.likeExpression}" >
                        <f:selectItem itemLabel="Exact"  itemValue="EXACT" /> 
                        <f:selectItem itemLabel="Like"  itemValue="LIKE" />
                        <f:validateLength minimum="1" />
                    </h:selectOneRadio>       
                    
                    <h:panelGroup/>
                    <h:panelGroup/>
                    <!----------------     End of Like ---------------->
                    
                    <!----------------     Start of Run number ---------------->
                    <h:outputLabel>            
                        <h:outputText value="Run #" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    
                    <h:panelGroup >
                        
                        <h:inputText styleClass="text" id="runNumberMinj_id_1" size="5" required="false" binding="#{visit.visitData.advancedSearchBean.runStartUI}" value="#{visit.visitData.advancedSearchBean.runStart}" >
                            <f:validateDoubleRange minimum="0" maximum="90000000" />
                        </h:inputText>                
                        &nbsp;
                        <h:inputText styleClass="text" id="runNumberMaxj_id_1" size="5" required="false" validator="#{visit.visitData.advancedSearchBean.validateRun}" value="#{visit.visitData.advancedSearchBean.runEnd}" >
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
                        
                        <t:inputCalendar size="1" styleClass="text" binding="#{visit.visitData.advancedSearchBean.calendarFirst}" id="startDatej_id_1" required="false" monthYearRowClass="yearMonthHeader" weekRowClass="weekHeader" popupButtonStyleClass="standard_bold"
                                         currentDayCellClass="currentDayCell" value="#{visit.visitData.advancedSearchBean.firstDate}" renderAsPopup="true"
                                         popupTodayString="Today is:"
                                         popupDateFormat="dd/MM/yyyy" popupWeekString="Wk"
                                         helpText="" validator="#{visit.visitData.advancedSearchBean.validateDate}"/>  
                        
                        <t:inputCalendar size="1"  styleClass="text"  binding="#{visit.visitData.advancedSearchBean.calendarSecond}" id="endDatej_id_1" required="false" monthYearRowClass="yearMonthHeader" weekRowClass="weekHeader" popupButtonStyleClass="standard_bold"
                                         currentDayCellClass="currentDayCell" value="#{visit.visitData.advancedSearchBean.secondDate}" renderAsPopup="true"
                                         popupTodayString="Today is:" 
                                         popupDateFormat="dd/MM/yyyy" popupWeekString="Wk"
                                         helpText="" validator="#{visit.visitData.advancedSearchBean.validateDate}"/>  
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
                    
                    <h:inputText styleClass="text" id="visitIdj_id_1" size="14" required="false" value="#{visit.visitData.advancedSearchBean.visitId}">
                        
                    </h:inputText>        
                    
                    <h:panelGroup/>
                    <h:message for="visitIdj_id_1" styleClass="error"/>
                    
                    <!----------------     End of visit id ---------------->
                    
                    <!----------------     Start of grant id ---------------->
                    <h:outputLabel>            
                        <h:outputText value="Grant Id" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:inputText styleClass="text" id="grantIdj_id_1" size="14" required="false" value="#{visit.visitData.advancedSearchBean.grantId}">
                        <f:validateDoubleRange minimum="0" maximum="90000000" />
                    </h:inputText>        
                    
                    <h:panelGroup/>
                    <h:message for="grantIdj_id_1" styleClass="error"/>
                    
                    <!----------------     End of grant id ---------------->

                    <!----------------     Start of Inv no ---------------->
                    <h:outputLabel>            
                        <h:outputText value="Inv Number:" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:inputText styleClass="text" id="invNumj_id_1" size="14" required="false" value="#{visit.visitData.advancedSearchBean.invNumber}">
                        <f:validateDoubleRange minimum="0" maximum="9990000000" />
                    </h:inputText>        
                    
                    
                    <h:panelGroup/>
                    <h:message for="invNumj_id_1" showDetail="false" showSummary="true" styleClass="error"/>
                    
                    <!----------------     End of Inv no ---------------->
                    
                    <!----------------     Start of inv type---------------->
                    <h:outputLabel>            
                        <h:outputText value="Inv type" style="font-size: 10px" />                   
                    </h:outputLabel>
                    
                    <h:selectOneMenu id="invTypej_id_1" required="false" value="#{visit.visitData.advancedSearchBean.invType}" >                           
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
                    
                    <h:selectOneMenu id="instrumentj_id_1" required="false" value="#{visit.visitData.advancedSearchBean.instrument}" >                           
                        <f:selectItems value="#{visit.customisation.instrumentsItems}" />                           
                    </h:selectOneMenu> 
                    
                    <%--  <h:inputText styleClass="text" id="instrumentj_id_1" size="14" required="false" value="#{visit.visitData.advancedSearchBean.instrument}">
        
    </h:inputText>        --%>
    
                    <h:panelGroup/>
                    <h:message for="instrumentj_id_1" styleClass="error"/>
                    
                    <!----------------     End of instruemnt---------------->
                    
                    <h:panelGroup/>                  
                    
                    
                    <h:commandButton id="f24" styleClass="button" action="#{advancedSearchBean.searchAdvancedNavigation}" onclick="busyBox.Show();" title="Search" value="Search"/>
                    
                    
                </h:panelGrid>
            </h:form>
        </t:panelTab>
        
        <t:panelTab id="Preferences" label="Preferences" >
            <h:form id="preferencesform">
                <h:panelGrid columns="2">
                    
                    <h:outputLabel>
                        <h:outputText styleClass="body" style="font-size:10px" value="Results Per Page"/>
                    </h:outputLabel  >
                    
                    <h:selectOneMenu id="results_navj_id_1"  style="font-size:7pt" required="true" value="#{userPreferencesBean.defaultresults}" >
                        <f:selectItems value="#{userPreferencesBean.results}"/>
                        <f:validateLength minimum="1" />
                        
                        
                    </h:selectOneMenu>
                    
                    <h:outputLabel>
                        <h:outputText styleClass="body" style="font-size:10px" value="Default Facility"/>
                    </h:outputLabel  >
                    
                    <h:selectOneMenu id="default_navj_id_1" style="font-size:7pt" required="true" value="#{userPreferencesBean.defaultFacility}" >
                        <f:selectItems value="#{userPreferencesBean.facilities}"/>
                        <f:validateLength minimum="1" />
                        
                        
                    </h:selectOneMenu>
                    
                    <h:outputLabel>
                        <h:outputText styleClass="body" style="font-size:10px" value="Screen Resolution"/>
                    </h:outputLabel>
                    
                    <h:selectOneMenu id="res_navj_id_1" style="font-size:7pt" required="true" value="#{userPreferencesBean.defaultResolution}" >
                        
                        <f:selectItems value="#{userPreferencesBean.resolution}" />
                        <f:validateLength minimum="1" />
                        
                    </h:selectOneMenu>
                    
                    
                    <h:outputLabel>
                        <h:outputText styleClass="body" style="font-size:10px" value="Default Location"/>
                    </h:outputLabel>
                    
                    <h:selectOneMenu id="location_navj_id_1" style="font-size:7pt" required="true" value="#{userPreferencesBean.defaultLocation}" >
                        
                        <f:selectItems  value="#{userPreferencesBean.location}" />
                        <f:validateLength minimum="1" />
                        
                    </h:selectOneMenu>
                    
                    
                    <h:outputLabel>
                        <h:outputText styleClass="body" style="font-size:10px" value="Email Address"/>
                    </h:outputLabel>
                    
                    <h:inputText styleClass="text" id="email_navj_id_1" size="15" validator="#{userPreferencesBean.validateEmail}" value="#{userPreferencesBean.email}">
                        <f:validateLength minimum="5" maximum="160"/>
                    </h:inputText> 
                    <h:panelGroup/>
                    
                    <h:message for="email_navj_id_1" styleClass="error" showSummary="false" showDetail="true" />
                    
                    <h:panelGroup/>
                    
                    <h:commandButton styleClass="button" action="#{userPreferencesBean.save}" title="Search" value="Update"/>
                    
                    
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
