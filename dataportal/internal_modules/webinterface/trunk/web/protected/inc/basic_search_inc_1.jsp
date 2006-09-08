<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://myfaces.apache.org/sandbox" prefix="s"%>

<br />
<br />
   <h:form>
       <h:panelGrid border="0" columns="1"> 
           <h:messages globalOnly="true" errorClass="error" infoClass="info" />
         
       </h:panelGrid>
                   
       <h:panelGrid  border="0" columns="4">      
                 
           <h:outputLabel for="facilities">
               <h:outputText value="Search: " style="font-size:14px"/>
           </h:outputLabel  >
          
           <s:selectManyPicklist id="facilities" value="#{searchBean.facilities}" size="5" required="true" >
         
               <f:selectItems value="#{searchBean.facilityList}"/>
               <f:validateLength minimum="1" />
           </s:selectManyPicklist>
              
           <h:panelGroup/>
            
           <h:message for="facilities" styleClass="error"/>
               
           <h:outputLabel for="keyword">
               <h:outputText value="Keyword(s): " style="font-size:14px"/>
           </h:outputLabel  >                      
           
           <s:inputSuggestAjax required="true" suggestedItemsMethod="#{keyword.getSuggest}"
           id="keyword" value="#{searchBean.keyword}" charset="utf-8"/>
            
           <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
               closePopupOnExitingPopup="true"
               displayAtDistanceX="5"
               displayAtDistanceY="-40">

               <t:graphicImage url="../../images/help.gif" border="0" />
               <f:facet name="popup">
                   <h:panelGroup>
                       <h:panelGrid columns="1" >
                           <h:outputText value="Exact keyword match only. Case insensititve."/>                                 
                          
                       </h:panelGrid>
                   </h:panelGroup>
               </f:facet>
           </t:popup>       
           <%--<s:inputSuggest id="keyword" value="#{searchBean.keyword}" required="true">
           <f:selectItems value="#{keyword.keywords}"/>
           </s:inputSuggest>--%>
                   
           <h:message for="keyword" styleClass="error"/>
               
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