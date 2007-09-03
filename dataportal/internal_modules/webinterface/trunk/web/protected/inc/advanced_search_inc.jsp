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
      var oldvalue = document.getElementById('body:autofillform:keywordFieldj_id_1').value;
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
      document.getElementById('body:autofillform:keywordFieldj_id_1').value = city+" ";
      }
      else {
      // alert("is old");
      document.getElementById('body:autofillform:keywordFieldj_id_1').value = old+" "+city+" ";
      }
      var oldvalue = document.getElementById('body:autofillform:facilities_SELECTED').options[0].value;
      // alert(oldvalue);
            
      }
      
      function show(){
      alert("Hello");   
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
            
            <ui:autoComplete  styleClass="text" size="40" maxlength="60" id="keywordFieldj_id_1" 
                             completionMethod="#{keyword.completeCity}" 
                             value="#{advancedSearchBean.keyword}" required="false"
                             ondisplay="function(item) { return extractCity(item); }"
                             onchoose="function(item) { return chooseCity(item); }"  />
            
            
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
                            <h:outputText value="'AND' type searches returns all results"/>                                 
                            <h:outputText escape="true" value=" that contain all the keywords, i.e. default Google behaviour."/> 
                            
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup> 
            
            <h:message for="keywordFieldj_id_1" styleClass="error"/>
            
            
            <!----------------     Start of Inv name  ---------------->
            <h:outputLabel>            
                <h:outputText value="Investigation name:" style="font-size: 14px" />                   
            </h:outputLabel>
            
            <h:inputText styleClass="text" id="invNamej_id_1" size="20" required="false" value="#{advancedSearchBean.invName}">
               
            </h:inputText>        
            
            <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="5"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage url="../../images/help.gif" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h:outputText value="Name of investigation."/>                                 
                            
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
            
            <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="5"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage url="../../images/help.gif" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h:outputText value="Abstract of investigation."/>                                 
                            
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
            
            <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="5"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage url="../../images/help.gif" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h:outputText value="Sample used in investigation."/>                                 
                            
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
            
            <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="5"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage url="../../images/help.gif" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h:outputText value="Investigators used in investigation."/>                                 
                            
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup>
            
            <h:message for="investigatorsj_id_1" styleClass="error"/>
            
            <!----------------     End of investigators ---------------->
            
            <!----------------     Start of Like ---------------->
            <h:panelGroup/>
            
            <%--<h:selectBooleanCheckbox value="#{searchBean.logicalExpressionBoolean}"  />--%>
            <h:selectOneRadio style="font-size:12px" id="like" value="#{advancedSearchBean.likeExpression}" >
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
            
            <!----------------     Start of Visit Id ---------------->
            <h:outputLabel>            
                <h:outputText value="Visit Id:" style="font-size: 14px" />                   
            </h:outputLabel>
            
            <h:inputText styleClass="text" id="visitIdj_id_1" size="20" required="false" value="#{advancedSearchBean.visitId}" />
                        
            <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="5"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage url="../../images/help.gif" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h:outputText value="Visit Id of investigation."/>                                 
                            
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup>
            
            <h:message for="visitIdj_id_1" styleClass="error"/>
            
            <!----------------     End of Visit Id ---------------->
              
            <!----------------     Start of Grant Id ---------------->
            <h:outputLabel>            
                <h:outputText value="Grant Id:" style="font-size: 14px" />                   
            </h:outputLabel>
            
            <h:inputText styleClass="text" id="grantIdj_id_1" size="20" required="false" value="#{advancedSearchBean.grantId}">
                 <f:validateDoubleRange minimum="0" maximum="90000000" />
            </h:inputText>        
            
            <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="5"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage url="../../images/help.gif" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h:outputText value="Grant Id of investigation."/>                                 
                            
                        </h:panelGrid>
                    </h:panelGroup>
                </f:facet>
            </t:popup>
            
            <h:message for="grantIdj_id_1" styleClass="error"/>
            
            <!----------------     End of Grant Id ---------------->
            
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
            
            <h:inputText styleClass="text" id="invTypej_id_1" size="20" required="false" value="#{advancedSearchBean.invType}">
               
            </h:inputText>        
            
            <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                     closePopupOnExitingPopup="true"
                     displayAtDistanceX="5"
                     displayAtDistanceY="-40" >
                
                <t:graphicImage url="../../images/help.gif" border="0" />
                <f:facet name="popup">
                    <h:panelGroup>
                        <h:panelGrid columns="1" >
                            <h:outputText value="Type of investigation."/>                                 
                            
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
            
            <h:inputText styleClass="text" id="instrumentj_id_1" size="20" required="false" value="#{advancedSearchBean.instrument}">
               
            </h:inputText>        
            
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
            
            <h:commandButton id="f" styleClass="button" action="#{advancedSearchBean.searchAdvanced}" onclick="busyBox.Show();" title="Search" value="Search"/>
            
            
            <h:panelGroup/>
            <h:panelGroup/>
        </h:panelGrid>
        
        
    </h:form>
</a4j:region>