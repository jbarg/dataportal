<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>

    <br />
    <i><font size="-2">
    <b>User:</b>
    <h:outputText value="#{visit.dn}"/> 
    <br />
    <b>Expire time: </b>
     
    <h:outputText value="#{visit.session.expireTime}">
        <f:convertDateTime pattern="HH:mm a dd.MM.yyyy "/>
    </h:outputText>
         
         
    <%--<h:outputText value="#{visit.session.expireTime}">
    <f:convertDateTime timeStyle="short"/>
    </h:outputText>&nbsp;
    <h:outputText value="#{visit.session.expireTime}">
    <f:convertDateTime dateStyle="full"/>
    </h:outputText>--%>
    <!--minutes left--></font></i>
