<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>

<br />
<br />
   <h:form>
       <h:panelGrid border="0" columns="1"> 
           <h:messages globalOnly="true" errorClass="error" infoClass="info" />
         
       </h:panelGrid>
                   
       <h:panelGrid border="0" columns="3">      
                 
           <h:outputLabel for="facilities">
               <h:outputText value="Search: "/>
           </h:outputLabel  >
          
           <h:selectManyListbox id="facilities" required="true" value="#{searchBean.facilities}">
               <f:selectItems value="#{visit.facilities}" />   
               <f:validateLength minimum="1" />
           </h:selectManyListbox>
              
           <h:message for="facilities" styleClass="error"/>
               
           <h:outputLabel for="keyword">
               <h:outputText value="Keyword(s): "/>
           </h:outputLabel  >
               
           <h:inputText id="keyword" required="true" value="#{searchBean.keyword}" >
               <f:validateLength minimum="1" maximum="30"/>
           </h:inputText>
               
           <h:message for="keyword" styleClass="error"/>
               
           <h:outputLabel> 
               <t:popup styleClass="popup" style="font-size: 14px" closePopupOnExitingElement="true"
                   closePopupOnExitingPopup="true"
                   displayAtDistanceX="20"
                   displayAtDistanceY="-40" >

                   <h:outputText value="'AND'"/>&nbsp;
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
               </t:popup><h:outputText value=" type search? "/>
              
           </h:outputLabel>

           <h:selectBooleanCheckbox value="#{searchBean.logicalExpressionBoolean}"  />
           <%--<h:selectOneRadio id="radio" value="#{searchBean.logicalExpression}" title="search">
           <f:selectItem itemLabel="AND" itemValue="AND" />
           <f:selectItem itemLabel="OR" itemValue="OR" />
           </h:selectOneRadio>--%>
           
           <h:panelGroup/>
           
           <h:panelGroup/>

           <h:commandButton action="#{searchBean.searchByKeyword}" title="search" value="Search"/>
           <h:panelGroup/>
       </h:panelGrid>
      
   </h:form>