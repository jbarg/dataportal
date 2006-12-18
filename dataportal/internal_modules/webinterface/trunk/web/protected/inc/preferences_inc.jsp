<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>


<table width="90%">
    <tbody>
        <tr>
            <td width="30">&nbsp;</td>
            <td>
                <table width="90%" border="0">
                    <tbody>
                        <tr>
                            <tr> <h:messages globalOnly="true" errorClass="error" infoClass="info" />
                            </tr>                 
                        </tr>
                    </tbody>
                    
                    
                </table>
                
                <h:form>
                    <h:panelGrid columns="3">
                        
                        <h:outputLabel>
                            <h:outputText styleClass="body" value="Results Per Page"/>
                        </h:outputLabel  >
                        
                        <h:selectOneMenu id="resultsj_id_1"  required="true" value="#{userPreferencesBean.defaultresults}" >
                            <f:selectItems value="#{userPreferencesBean.results}"/>
                            <f:validateLength minimum="1" />
                            
                            
                        </h:selectOneMenu>
                        <h:message for="resultsj_id_1" styleClass="error" showSummary="false" showDetail="true" />
                        
                        <h:outputLabel>
                            <h:outputText styleClass="body" value="Default Facility"/>
                        </h:outputLabel  >
                        
                        <h:selectOneMenu id="defaultj_id_1" required="true" value="#{userPreferencesBean.defaultFacility}" >
                            <f:selectItems value="#{userPreferencesBean.facilities}"/>
                            <f:validateLength minimum="1" />
                            
                            
                        </h:selectOneMenu>
                        <h:message for="defaultj_id_1" styleClass="error" showSummary="false" showDetail="true" />
                        
                        <h:outputLabel>
                            <h:outputText styleClass="body" value="Screen Resolution"/>
                        </h:outputLabel>
                        
                        <h:selectOneMenu id="resj_id_1" required="true" value="#{userPreferencesBean.defaultResolution}" >
                            
                            <f:selectItems value="#{userPreferencesBean.resolution}" />
                            <f:validateLength minimum="1" />
                            
                        </h:selectOneMenu>
                        
                        <h:message for="resj_id_1" styleClass="error" showSummary="false" showDetail="true" />
                        
                        <h:outputLabel>
                            <h:outputText styleClass="body" value="Email Address"/>
                        </h:outputLabel>
                        
                        <h:inputText id="emailj_id_1" size="30" required="true" validator="#{userPreferencesBean.validateEmail}" value="#{userPreferencesBean.email}">
                            <f:validateLength minimum="5" maximum="160"/>
                        </h:inputText> 
                        
                        
                        <h:message for="emailj_id_1" styleClass="error" showSummary="false" showDetail="true" />
                        
                        <h:panelGroup/>
                        
                        <h:commandButton action="#{userPreferencesBean.save}" title="Search" value="Update"/>
                        <h:panelGroup/>
                        
                    </h:panelGrid>
                </h:form>
                
            </td>
        </tr>
    </tbody>
</table>

