<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib prefix="tiles" uri="http://struts.apache.org/tags-tiles"%>

<h:form id="userform">
    
    <br />
    
    
    <%
    //invalidate session
    //  session.invalidate();
    %>
    
    <br />
    
    
        <h:outputText style="color:blue" styleClass="nodeFolder" value="Thank you for using the Data Portal."/>
        <br />  <br />
        
        <h:outputText  styleClass="nodeFolder" value="To log back on click "/> 
        <h:commandLink action="GOTO_LOGON" >
            <h:outputText style="color:blue" styleClass="nodeFolder" value="here."/>
        </h:commandLink> 
        
   
    
    <%-- <br />
    <h:outputText  styleClass="nodeFolder" value="Goto "/> 
    <h:commandLink  onclick="window.open('http://tiber.dl.ac.uk:8080/myproxy'); return false;" >
        <h:outputText style="color:blue" styleClass="nodeFolder" value="MyProxy Upload Home"/>
    </h:commandLink> 
    <h:outputText  styleClass="nodeFolder" value=" to upload credentials" />
   --%>
    
    
    
</h:form>
