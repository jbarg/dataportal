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
      var oldvalue = document.getElementById('body:autofillform:keywordFieldj_id_1').value;
   
      var index = oldvalue.lastIndexOf(' ');
          
      var old = oldvalue.substring(0, index);
     
      
      
      if(old == ""){
      
      document.getElementById('body:autofillform:keywordFieldj_id_1').value = city+" ";
      }
      else {
     
      document.getElementById('body:autofillform:keywordFieldj_id_1').value = old+" "+city+" ";
      }
      //var oldvalue = document.getElementById('body:autofillform:facilities_SELECTED').options[0].value;
     
            
      }
           
      
       
</script>
<br />

<a4j:region  selfRendered="true"> 
    <h:form id="autofillform">
        <h:panelGrid border="0" columns="2"> 
            <h:messages globalOnly="true" errorClass="error" infoClass="info" />
            
        </h:panelGrid>
        
        <h:panelGrid  border="0" columns="4">      
            
            
            <h:outputLabel for="facilitiesj_id_1">
                <h:outputText value="Search: " style="font-size:14px"/>
            </h:outputLabel  >
            
                                   
            <%--    <h:selectManyListbox id="facilities" immediate="true" onchange="submit()" valueChangeListener="#{keyword.selectedFacilities}" value="#{visit.visitData.currentSelectedFacilities}" size="3" required="true" >--%>
            <h:selectManyListbox id="facilitiesj_id_1" immediate="true"  value="#{visit.visitData.currentSelectedFacilities}" size="#{fn:length(visit.facilities)}" required="true" >      
                <a4j:support event="onchange" action="#{keyword.selectedFacilities}" ajaxSingle="true" reRender="facilityDisplay,radio" />
                <f:selectItems value="#{visit.facilities}"/>
                <f:validateLength minimum="1" />
            </h:selectManyListbox>
            
            
            <h:outputText id="facilityDisplay" value="#{visit.visitData.currentSelectedFacilities}" style="font-size:14px"/>
            
            <h:message for="facilitiesj_id_1" styleClass="error"/>
            
            <h:outputLabel for="keywordFieldj_id_1">
                <h:outputText value="Keyword(s): " style="font-size:14px"/>
            </h:outputLabel  >                      
                        
            <ui:autoComplete styleClass="text" size="40" maxlength="60" id="keywordFieldj_id_1" 
                              completionMethod="#{keyword.completeCity}" 
                              value="#{searchBean.keyword}" required="true"
                              ondisplay="function(item) { return extractCity(item); }"
                              onchoose="function(item) { return chooseCity(item); }"  validator="#{searchBean.validateKeyword}"/>
            
            
            <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="5"
                     displayAtDistanceY="-40">
                
                <t:graphicImage url="../../images/help.gif" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <%-- <h:outputText value="Exact keyword match only. Case insensititve."/>   --%>
                            <h:outputText value="Auto Complete enabled."/>                                 
                            <h:outputText value="This list is not a full list of all the possible keywords."/>                                 
                            
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup> 
                                        
            <h:message for="keywordFieldj_id_1" styleClass="error"/>
            
            <h:outputLabel>            
                <h:outputText value="Type:" style="font-size: 14px" />                   
            </h:outputLabel>
            
            <%--    <h:selectBooleanCheckbox value="#{searchBean.logicalExpressionBoolean}"  />--%>
            <h:selectOneRadio style="font-size:12px" id="radio" value="#{searchBean.logicalExpression}" >
                <f:selectItems value="#{searchBean.logicalExpressions}"/>              
            </h:selectOneRadio>         
            
            <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="5"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage url="../../images/help.gif" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h:outputText value="'AND' type searches returns all results"/>                                 
                            <h:outputText escape="true" value=" that contain all the keywords, i.e. default Google behaviour."/> 
                            <h:outputText value="'OR' type searches returns all results"/>                             
                            <h:outputText  escape="true" value="that contain atleast one of the keywords."/>  
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup>
            
            
            <h:panelGroup/>
            
            <h:panelGroup/>
            
            <%--<h:selectBooleanCheckbox value="#{searchBean.logicalExpressionBoolean}"  />--%>
            <h:selectOneRadio style="font-size:12px" id="like" value="#{searchBean.likeExpression}" >
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
                                     <h:outputText value="'Exact' type searches returns all results"/>                                 
                            <h:outputText escape="true" value=" that exactly match the keyword."/> 
                            <h:outputText value="'Like' type searches returns all results"/>                             
                            <h:outputText  escape="true" value="that contain part of the keywords."/>  
                            <h:outputText  escape="true" value="I.e. oxygen would return results with oxygens as a keyword."/>              
                   </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup>
            
            
            <h:panelGroup/>
            
            
            <h:panelGroup/>
            <h:panelGrid columns="3">
                <h:commandButton id="f" styleClass="button" action="#{searchBean.searchByKeyword}" onclick="busyBox.Show();" title="Search" value="Search"/>
                <h:panelGroup>&nbsp;</h:panelGroup>
                <h:commandButton id="own" styleClass="button" action="#{searchBean.searchOwnData}" immediate="true" onclick="busyBox.Show();" title="View my data" value="View my data"/>
                
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