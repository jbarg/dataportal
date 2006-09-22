<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>


   <br />
   <br />
    <table width="90%">
        <td width="30">&nbsp;</td>
        <td>
            <table width="90%" border="0">
                <tr> <h:messages globalOnly="true" errorClass="error" infoClass="info" />
                </tr>                 
       
                        
            </table>

            <h:form>
                <h:panelGrid columns="3">
                                              
                    <h:outputLabel>
                        <h:outputText styleClass="body" value="Results Per Page"/>
                    </h:outputLabel  >
                                                                                           
                    <h:selectOneMenu id="results"  required="true" value="#{userPreferencesBean.defaultresults}" >
                        <f:selectItems value="#{userPreferencesBean.results}"/>
                        <f:validateLength minimum="1" />
                      
                       
                    </h:selectOneMenu>
                    <h:message for="results" styleClass="error" showSummary="false" showDetail="true" />

                    <h:outputLabel>
                        <h:outputText styleClass="body" value="Default Facility"/>
                    </h:outputLabel  >
                                                                                           
                    <h:selectOneMenu id="default" required="true" value="#{userPreferencesBean.defaultFacility}" >
                        <f:selectItems value="#{userPreferencesBean.facilities}"/>
                        <f:validateLength minimum="1" />
                      
                       
                    </h:selectOneMenu>
                    <h:message for="default" styleClass="error" showSummary="false" showDetail="true" />

                    <h:outputLabel>
                        <h:outputText styleClass="body" value="Screen Resolution"/>
                    </h:outputLabel>
                                                                           
                    <h:selectOneMenu id="res" required="true" value="#{userPreferencesBean.defaultResolution}" >
                        
                        <f:selectItems value="#{userPreferencesBean.resolution}" />
                        <f:validateLength minimum="1" />
                       
                    </h:selectOneMenu>
                    
                    <h:message for="res" styleClass="error" showSummary="false" showDetail="true" />

                    
                    <h:panelGroup/>

                    <h:commandButton action="#{userPreferencesBean.save}" title="Search" value="Update"/>
                    <h:panelGroup/>
                   
                </h:panelGrid>
            </h:form>


           
            <br />    
        </td>
    </table>
              