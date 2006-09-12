<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://myfaces.apache.org/sandbox" prefix="s"%>
<%@taglib prefix="ui" uri="http://java.sun.com/blueprints/ui/14" %>

  <script type="text/javascript">
      function extractCity(citystatezip) {
      var index = citystatezip.indexOf(',');
      var nextcity = citystatezip.substring(0, index+4);
      //var oldvalue = document.getElementById('body:autofillform:facilities_SELECTED').value;
      //alert(oldvalue);
      
      return citystatezip;
      }
      

      function chooseCity(city) {
      var oldvalue = document.getElementById('body:autofillform:keywordField').value;
      // alert("--"+oldvalue+"--");
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
      document.getElementById('body:autofillform:keywordField').value = city+" ";
      }
      else {
      // alert("is old");
      document.getElementById('body:autofillform:keywordField').value = old+" "+city+" ";
      }
       var oldvalue = document.getElementById('body:autofillform:facilities_SELECTED').options[0].value;
     // alert(oldvalue);
            
      }
      
      function show(){
      alert("Hello");   
      }
         
      
       
  </script>
<br />
<br />
   <h:form id="autofillform">
       <h:panelGrid border="0" columns="1"> 
           <h:messages globalOnly="true" errorClass="error" infoClass="info" />
         
       </h:panelGrid>
                   
       <h:panelGrid  border="0" columns="4">      
                 
           <h:outputLabel for="facilities">
               <h:outputText value="Search: " style="font-size:14px"/>
           </h:outputLabel  >
          
         <%--  <s:selectManyPicklist id="facilities" immediate="true" onchange="submit()" valueChangeListener="#{keyword.selectedFacilities}" value="#{searchBean.facilities}" size="5" required="true" >
         
               <f:selectItems value="#{searchBean.facilityList}"/>
               <f:validateLength minimum="1" />
           </s:selectManyPicklist>--%>
           
            <h:selectManyListbox id="facilities" immediate="true" onchange="submit()" valueChangeListener="#{keyword.selectedFacilities}" value="#{visit.visitData.currentSelectedFacilities}" size="3" required="true" >
         
               <f:selectItems value="#{visit.facilities}"/>
               <f:validateLength minimum="1" />
           </h:selectManyListbox>
              
            <h:outputText value="#{visit.visitData.currentSelectedFacilities}" style="font-size:14px"/>
            
           <h:message for="facilities" styleClass="error"/>
               
           <h:outputLabel for="keyword">
               <h:outputText value="Keyword(s): " style="font-size:14px"/>
           </h:outputLabel  >                      
           
           <%--   <s:inputSuggestAjax required="true" suggestedItemsMethod="#{keyword.getSuggest}"
           id="keywordField" value="#{searchBean.keyword}" charset="utf-8"/>--%>
           
           <ui:autoComplete size="40" maxlength="60" id="keywordField" 
           completionMethod="#{keyword.completeCity}" 
           value="#{searchBean.keyword}" required="true"
           ondisplay="function(item) { return extractCity(item); }"
           onchoose="function(item) { return chooseCity(item); }" />
            
           <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
               closePopupOnExitingPopup="true"
               displayAtDistanceX="5"
               displayAtDistanceY="-40">

               <t:graphicImage url="../../images/help.gif" border="0" />
               <f:facet name="popup">
                   <h:panelGroup>
                       <h:panelGrid columns="1" >
                           <h:outputText value="Exact keyword match only. Case insensititve."/>                                 
                           <h:outputText value="Auto Complete enabled."/>                                 
                           <h:outputText value="This list is not a full list of all the possible keywords."/>                                 
                         
                       </h:panelGrid>
                   </h:panelGroup>
               </f:facet>
           </t:popup>       
           <%--<s:inputSuggest id="keyword" value="#{searchBean.keyword}" required="true">
           <f:selectItems value="#{keyword.keywords}"/>
           </s:inputSuggest>--%>
                   
           <h:message for="keywordField" styleClass="error"/>
               
           <h:outputLabel>            
               <h:outputText value="Type:" style="font-size: 14px" />                   
           </h:outputLabel>

           <%--<h:selectBooleanCheckbox value="#{searchBean.logicalExpressionBoolean}"  />--%>
           <h:selectOneRadio style="font-size:14px" id="radio" value="#{searchBean.logicalExpression}" title="search">
               <f:selectItem itemLabel="AND" itemValue="AND" />
               <f:selectItem itemLabel="OR" itemValue="OR" />
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
                           <h:outputText escape="true" value=" that contain all the keywords, i.e. google."/> 
                           <h:outputText value="'OR' type searches returns all results"/>                             
                           <h:outputText  escape="true" value="that contain atleast one of the keywords."/>  
                       </h:panelGrid>
                   </h:panelGroup>
               </f:facet>
           </t:popup>

           
           <h:panelGroup/>
           <h:panelGroup/>
           <h:commandButton action="#{searchBean.searchByKeyword}" title="search" value="Search"/>
          
           <h:panelGroup/>
       </h:panelGrid>
      
   </h:form>