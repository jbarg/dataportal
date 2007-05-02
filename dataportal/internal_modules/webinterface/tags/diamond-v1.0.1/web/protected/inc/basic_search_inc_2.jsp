<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://myfaces.apache.org/sandbox" prefix="s"%>
<%@ taglib prefix="ui" uri="http://java.sun.com/blueprints/ui/14" %>
<%@ taglib uri="https://ajax4jsf.dev.java.net/ajax" prefix="a4j"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>

<script type="text/javascript">
      function extractCityNav(citystatezip) {
    
      var index = citystatezip.indexOf(',');
      var nextcity = citystatezip.substring(0, index+4);
     // var oldvalue = document.getElementById('body:autofillform_nav:facilities_SELECTED').value;
      //alert(oldvalue);
       
      return citystatezip;
      }
      

      function chooseCityNav(city) {
     // alert("22");
      var oldvalue = document.getElementById('navigation_search:autofillform_nav:keywordField_navj_id_1').value;
     //  alert("--"+oldvalue+"--");
      var index = oldvalue.lastIndexOf(' ');
          
      var old = oldvalue.substring(0, index);
      //alert(old);
      //alert("data Received =" + city + " - " + state + " - " + zip);
      
      
      //var index2 = city.lastIndexOf('-');
      //if(index2 != -1){
      //  city  = city.substring(0, index2-1);
      //var index3 = city.lastIndexOf('-');
      //if(index3 != -1){
      //  city  = city.substring(0, index3-3);
      //}
      //}
      
      
      if(old == ""){
      // alert("no old");
      document.getElementById('navigation_search:autofillform_nav:keywordField_navj_id_1').value = city+" ";
      }
      else {
      // alert("is old");
      document.getElementById('navigation_search:autofillform_nav:keywordField_navj_id_1').value = old+" "+city+" ";
      }
      var oldvalue = document.getElementById('navigation_search:autofillform_nav:facilities_SELECTED').options[0].value;
      // alert(oldvalue);
            
      }
      
      function show(){
      alert("Hello");   
      }
         
      
       
</script>
<br />

<a4j:region  selfRendered="true"> 
    
    
    
    <t:panelTabbedPane  width="202px" bgcolor="#D1E4E4" serverSideTabSwitch="false" inactiveTabStyleClass="tabinactive" activeTabStyleClass="tabactive" >
        <t:panelTab id="basicSearch"  label="Basic Search" >
            <h:form id="autofillform_nav">
                
                <h:panelGrid  border="0" columns="2"  >      
                    
                    
                    <h:outputLabel for="facilities_navj_id_1">
                        <h:outputText value="Search: " style="font-size:12px"/>
                    </h:outputLabel  >
                    
                    <h:selectManyListbox id="facilities_navj_id_1" immediate="true"  value="#{visit.visitData.currentSelectedFacilities}" size="#{fn:length(visit.facilities)}" required="true" >      
                        <a4j:support event="onchange" action="#{keyword.selectedFacilities}" ajaxSingle="true" reRender="facilityDisplayNav,radioNav" />
                        <f:selectItems value="#{visit.facilities}"/>
                        <f:validateLength minimum="1" />
                    </h:selectManyListbox>
                    
                    
                    
                    <h:message  for="facilities_navj_id_1" styleClass="error"/>
                    <h:panelGroup/>
                    
                    
                    
                    <h:outputLabel for="keywordField_navj_id_1">
                        <h:outputText value="Keyword(s): " style="font-size:12px"/>
                    </h:outputLabel  >                      
                    
                    <ui:autoComplete  styleClass="text" size="14" maxlength="60" id="keywordField_navj_id_1" 
                                      completionMethod="#{keyword.completeCity}" 
                                      value="#{visit.visitData.basicSearchBean.keyword}" required="true"
                                      ondisplay="function(item) { return extractCityNav(item); }"
                                      onchoose="function(item) { return chooseCityNav(item); }"  validator="#{searchBean.validateKeyword}"/>
                    
                    
                     <h:panelGroup/>
                    <h:message for="keywordField_navj_id_1" styleClass="error"/>
                   
                    
                    
                    
                    <h:outputLabel>            
                        <h:outputText escape="false" value="Type:" style="font-size: 12px" />                   
                    </h:outputLabel>
                    
                    <h:selectOneRadio style="font-size:10px" id="radioNav" value="#{visit.visitData.basicSearchBean.logicalExpression}" >
                        <f:selectItems value="#{searchBean.logicalExpressions}"/>              
                    </h:selectOneRadio>         
                    
                    
                    <h:panelGroup/>
                    
                    <%--<h:selectBooleanCheckbox value="#{searchBean.logicalExpressionBoolean}"  />--%>
                    <h:selectOneRadio style="font-size:10px" id="likeNav" value="#{visit.visitData.basicSearchBean.likeExpression}" >
                        <f:selectItem itemLabel="Exact"  itemValue="EXACT" /> 
                        <f:selectItem itemLabel="Like"  itemValue="LIKE" />
                    </h:selectOneRadio>       
                    
                    <h:panelGroup/>           
                    <h:panelGroup/>
                    
                    
                    <h:panelGroup/>
                    <h:commandButton id="f2" styleClass="button" action="#{searchBean.searchByKeywordNavigation}" onclick="busyBox.Show();" title="Search" value="Search"/>
                    
                    
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
