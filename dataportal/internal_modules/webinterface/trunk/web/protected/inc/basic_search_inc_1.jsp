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
                <%--<h:selectManyListbox id="facilities" immediate="true" onchange="submit()" valueChangeListener="#{keyword.selectedFacilities}" value="#{visit.visitData.currentSelectedFacilities}" size="3" required="true" >--%>
                <h:selectManyListbox id="facilitiesj_id_1" immediate="true"  value="#{visit.visitData.currentSelectedFacilities}" size="#{fn:length(visit.facilities)}" required="false" >      
                    <a4j:support id="facs" event="onchange" action="#{keyword.selectedFacilities}" ajaxSingle="true" reRender="facilityDisplay,radio" />
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
                <ui:autoComplete rendered="#{sessionHistory.keywordSearchAutoComplete && sessionHistory.keywordSearchCaseSensitive}" styleClass="text" size="35" maxlength="60" id="keywordFieldj_id_1" 
                                 completionMethod="#{keyword.completeCityCaseSensitive}" 
                                 value="#{searchBean.keyword}" required="true"
                                 ondisplay="function(item) { return extractCity(item); }"
                                 onchoose="function(item) { return chooseCity(item); }"  validator="#{searchBean.validateKeyword}"/>
                
                <ui:autoComplete rendered="#{sessionHistory.keywordSearchAutoComplete && !sessionHistory.keywordSearchCaseSensitive}" styleClass="text" size="35" maxlength="60" id="keywordFieldj_id_2" 
                                 completionMethod="#{keyword.completeCityCaseInsensitive}" 
                                 value="#{searchBean.keyword}" required="true"
                                 ondisplay="function(item) { return extractCity(item); }"
                                 onchoose="function(item) { return chooseCity(item); }"  validator="#{searchBean.validateKeyword}"/>
                
                <h:inputText rendered="#{!sessionHistory.keywordSearchAutoComplete}" styleClass="text" size="35" maxlength="60" id="keywordFieldj_id_3"
                             value="#{searchBean.keyword}" required="true" validator="#{searchBean.validateKeyword}" />
            </h:panelGrid> 
            
            
            
            <t:popup styleClass="popup" style="font-size: 14px; background-color: #D1E4E4;" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="15"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage url="../../images/help_16.png" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid  columns="1" >                            
                            <h3>Keyword(s)</h3>
                            <p> Use * to denote a wild card.  I.e.  *data, data* or *data*portal*<br />
                                <br />Searching ship* will match ships and shipping, when SHIP* will<br />
                            match SHIPS and shipping if Case Sensitive is turned off (unchecked)</p> 
                            
                            
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup> 
            
            
            <h:panelGrid >
                <h:message for="keywordFieldj_id_1" styleClass="error"/>
                <h:message for="keywordFieldj_id_2" styleClass="error"/>
                <h:message  for="keywordFieldj_id_3" styleClass="error"/>                
            </h:panelGrid>  
            <!----------------     Start of auto complete toggle ---------------->
            
            <h:panelGroup/>
            
            <h:panelGrid columns="4" >
                
                <%--onchange="submit()" valueChangeListener="#{searchBean.autoCompleteEnabled}"--%>
                <h:selectBooleanCheckbox  style="font-size:12px" id="auto"   required="true" value="#{sessionHistory.keywordSearchAutoComplete}">                                       
                    <a4j:support id="other" event="onclick" immediate="true" actionListener="#{sessionHistory.autoComplete}" ajaxSingle="true" reRender="inputKeywordField" >
                        <a4j:actionparam name="keyword" /> 
                    </a4j:support>
                </h:selectBooleanCheckbox>         
                
                <h:outputLabel >            
                    <h:outputText  value="Auto Complete" style="font-size: 12px" />                   
                </h:outputLabel>
                
                <h:selectBooleanCheckbox style="font-size:12px" id="case" required="true" value="#{sessionHistory.keywordSearchCaseSensitive}">                                                           
                    <a4j:support id="other3" event="onclick" immediate="true" actionListener="#{sessionHistory.caseSensitive}" ajaxSingle="true" reRender="inputKeywordField">
                        <a4j:actionparam name="keyword" /> 
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
                            <h3>Auto Complete</h3>
                            <p>'Yes' auto complete is enabled.  If enabled, the list is not a full <br />
                                list of all your possible keywords, only alpha numeric.<br /> 
                            'No' auto complete is disabled</p>   
                            
                            <h3>Case Sensitive</h3>
                            <p> 'Yes' keyword case is kept and searched<br />
                            'No' search is done regardless of case of keyword (Case Insensitive), <br />i.e SHIP will match ship and ShIp</p>   
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup>
            
            <h:panelGroup/>
            
            <!--                End of toggle auto complete  -->  
            
            
            <h:panelGroup/>
            
            <h:panelGrid columns="3" >
                <h:commandButton id="f" styleClass="button" action="#{searchBean.searchByKeyword}" onclick="busyBox.Show();" title="Search" value="Search"/>
                &nbsp;
                <h:commandButton id="reset" type="reset" styleClass="button"  title="Reset" value="Reset"/>
            </h:panelGrid>
            <h:panelGroup/>
            <h:panelGroup/>
        </h:panelGrid>
        
        <%--  <h:panelGrid  border="0" columns="2">   
        
        <h:panelGroup/> 
        <h:panelGroup><br /></h:panelGroup>
        
        
        <h:outputText value="To view your own data " style="font-size:14px"/>  <h:commandLink action="#{searchBean.searchOwnData}" onclick="busyBox.Show();" title="View Own Data" value="click here"/>
        
        </h:panelGrid>--%>
    </h:form>
</a4j:region>