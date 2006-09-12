<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib prefix="tiles" uri="http://struts.apache.org/tags-tiles"%>

<h:form>
 
    <br />
    <br />
    <br />
    <%
        //invalidate session
        //  session.invalidate();
    %>
    <h:panelGrid columns="1" >
        <h:messages globalOnly="true" styleClass="error" style="color : red" />
    </h:panelGrid>  
    <h:panelGrid columns="3">
                                              
        <h:outputLabel for="usernameinput">
            <h:outputText styleClass="body" value="Username: "/>
        </h:outputLabel  >
                                                                                           
        <h:inputText id="Username" size="20" required="true" value="#{authorisationBean.username}">
            <f:validateLength minimum="1" maximum="10"/>
        </h:inputText> 
        <h:message for="Username" styleClass="error" showSummary="false" showDetail="true" />

        <h:outputLabel for="passwordinput">
            <h:outputText styleClass="body" value="Password: "/>
        </h:outputLabel  >

        <h:inputSecret id="Password"  size="20" required="true" value="#{authorisationBean.password}">
            <f:validateLength minimum="0" maximum="10"/>
        </h:inputSecret> 
        <h:message for="Password" styleClass="error"/>
        <h:panelGroup/>

        <h:commandButton action="#{authorisationBean.login}" title="Login" value="Logon"/>
        <h:panelGroup/>
                  
    </h:panelGrid>

    <br />
    <br />
    <h:outputText  styleClass="nodeFolder" value="Goto "/> 
    <h:commandLink  onclick="window.open('http://tiber.dl.ac.uk:8080/myproxy')" target="_blank">
        <h:outputText  styleClass="nodeFolder" value="MyProxy Upload Home"/>
    </h:commandLink> 
    <h:outputText  styleClass="nodeFolder" value=" to upload credentials" />
   
             


</h:form>
