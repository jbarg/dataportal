<%@taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<h:form id="userform">
    
     <f:loadBundle basename="uk.ac.dl.dp.web.messages.facility" var="facility_properties" />
  
    <br />
    
    
    <%
    //invalidate session
    //  session.invalidate();
    %>
    <h:panelGrid columns="1" >
        <h:outputText styleClass="info" style="font-size : 16px" value="Log onto the Data Portal:"/>
    </h:panelGrid>  
    <br />
    <h:panelGrid columns="1" >
        <h:messages globalOnly="true" errorClass="error" infoClass="info" />
    </h:panelGrid>  
    <h:panelGrid columns="3">
        
        <h:outputLabel for="Usernamej_id_1">
            <%--<h:outputText styleClass="body" value="Federal ID: "/>--%>
            <h:outputText styleClass="body" value="#{facility_properties['logon.username']}: "/>
        </h:outputLabel  >
        
        <h:inputText styleClass="text" id="Usernamej_id_1" size="20" required="true" value="#{authorisationBean.username}">
            <f:validateLength minimum="1" />
        </h:inputText> 
        <h:message for="Usernamej_id_1" styleClass="error" showSummary="false" showDetail="true" />
        
        <h:outputLabel for="Passwordj_id_1">
            <h:outputText styleClass="body" value="Password: "/>
        </h:outputLabel  >
        
        <h:inputSecret styleClass="text" id="Passwordj_id_1"  size="20" required="true" value="#{authorisationBean.password}">
            <f:validateLength minimum="6" />
        </h:inputSecret> 
        <h:message for="Passwordj_id_1" styleClass="error"/>
        <h:panelGroup/>
        
        <h:commandButton styleClass="button" action="#{authorisationBean.login}" title="Login" value="Logon"/>
        <h:panelGroup/>
        
    </h:panelGrid>
    
    <br />
    <h:outputText rendered="#{facility_properties['myproxy.upload.visible']}" styleClass="nodeFolder" value="Goto "/> 
    <h:commandLink rendered="#{facility_properties['myproxy.upload.visible']}" onclick="window.open('http://tiber.dl.ac.uk:8080/myproxy'); return false;" target="_blank">
        <h:outputText style="color:blue" styleClass="nodeFolder" value="MyProxy Upload Home"/>
    </h:commandLink> 
    <h:outputText rendered="#{facility_properties['myproxy.upload.visible']}" styleClass="nodeFolder" value=" to upload credentials" />
    
    
    
    
</h:form>
