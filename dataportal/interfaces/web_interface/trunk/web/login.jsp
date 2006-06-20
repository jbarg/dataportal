<%@page contentType="text/html"%>
<%@page pageEncoding="UTF-8"%>

<%@taglib prefix="f" uri="http://java.sun.com/jsf/core"%>
<%@taglib prefix="h" uri="http://java.sun.com/jsf/html"%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">
   
   <html>
       <head>
           <title>
               JSF in Action - Hello, world!
           </title>
       </head>
       <body>
           <f:view>
               <h:form id="welcomeForm">
                   <h:outputText id="welcomeOutput" value="Welcome to JavaServer Faces!"
                   style="font-family: Arial, sans-serif; font-size: 24; color: green;"/>
                   <p>
                       <h:message id="errors" for="helloInput" style="color: red"/>
                   </p>
                   <p>
                       <h:outputLabel for="helloInput">
                           <h:outputText id="helloInputLabel" value="Enter number of controls to display:"/>
                       </h:outputLabel>
                       <h:inputText id="helloInput" value="#{login.numControls}" required="true">
                           <f:validateLongRange minimum="1" maximum="500"/>
                       </h:inputText>
                   </p>
                   <p>
                       <h:panelGrid id="controlPanel" binding="#{login.controlPanel}" columns="20" border="1" cellspacing="0"/>
                   </p>
                   <h:commandButton id="redisplayCommand" type="submit" value="Redisplay" actionListener="#{login.addControls}"/>
                   <h:commandButton id="goodbyeCommand" type="submit" value="Goodbye" action="#{login.goodbye}" immediate="true"/>
               </h:form>
           </f:view>
       </body>
   </html>
