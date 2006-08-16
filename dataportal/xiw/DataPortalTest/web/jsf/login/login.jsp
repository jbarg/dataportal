<%--
<%@page contentType="text/html"%>
<%@page pageEncoding="UTF-8"%>
--%>

<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">
<%-- <%--
   <<f:view>
<%--  --%>
       <html>
           <head>
               <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
               <title>Data Portal Login</title>
           </head>
           <body>
               <h:form>
                   <%-- 
                   <table border="2">
                   <tr>
                   <td>
                   <h:graphicImage value="./images/e-science.JPG" width="100" height="70" />
                   </td>
                                      
                   <td>
                   <table border="0">
                   <tr>
                   <td colspan="3">
                   <font face="Arial, sans-serif" size="5">
                   <h:outputText value="Data Portal Login" />
                   </font>
                   </td>
                   </tr>
                               
                   <tr>
                   <td>
                   <h:outputLabel for="userNameInput">
                   <h:outputText value="Enter your user name:" />
                   </h:outputLabel>
                   </td>
                   <td>
                   <h:inputText id="userNameInput" size="20" maxlength="30"/>
                   </td>
                   <td/>
                   </tr>
 
                   <tr> 
                   <td>
                   <h:outputLabel for="passwordInput" >
                   <h:outputText value="Password:"/>
                   </h:outputLabel>
                   </td>
                   <td>
                                 
                   <h:inputSecret id="passwordInput" size="20" maxlength="20" />
                                                          
                   </td>
                   </tr>
                             
                   <tr>
                   <td/>
                   <td>
                   <h:commandButton action="success" title="submit" value="Submit"/>
                   </td>
                   <td/>
                   </tr>
                             
                   </table>
                   </td>
                   
                   </tr>
                   
                   </table>
                   --%>
                   <%-- 
                   <h:dataTable var="table">
                   <h:column >
                   <h:outputLabel for="userNameInput">
                   <h:outputText value="Enter your user name:"/>
                   </h:outputLabel>
                   <h:outputLabel for="passwordInput">
                   <h:outputText value="password:"/>
                   </h:outputLabel>
                           
                   </h:column>
                   <h:column>
                           
                   <h:inputSecret id="passwordInput" size="20" maxlength="30" required="true">
                   <f:validateLength minimum="5" maximum="25"/>
                   </h:inputSecret>  
                   <h:inputText id="userNameInput" size="20" maxlength="30" required="true">
                   <f:validateLength minimum="3" maximum="30"/>
                   </h:inputText>
                   </h:column>
                      
                   </h:dataTable>
                   --%>
                   
                   
                   <%-- 
                   <h:panelGrid columns="1" width="100%">
                   <f:verbatim><h3><b>Data Portal Login</b></h3><hr></f:verbatim>

                   </h:panelGrid>
                   
                   <h:panelGrid columns="2">
                   <h:graphicImage url="./images/e-science.JPG" width="100" height="70" />
                   <h:panelGrid columns="1">

                   <h:panelGroup>
                   <h:outputLabel for="userNameInput">
                   <h:outputText value="Enter your user name:"/>
                   </h:outputLabel>
                   <h:inputText id="userNameInput" size="20" maxlength="30" required="true">
                   <f:validateLength minimum="3" maximum="30"/>
                   </h:inputText>
                   </h:panelGroup>
                   <h:panelGroup>
                   <h:outputLabel for="passwordInput">
                   <h:outputText value="password:"/>
                   </h:outputLabel>
                   <h:inputSecret id="passwordInput" size="20" maxlength="30" required="true">
                   <f:validateLength minimum="5" maximum="25"/>
                   </h:inputSecret>
                   </h:panelGroup>
                                                     
                   <h:panelGroup>
                   <h:commandButton title="Submit" value="Submit"/>                               
                   </h:panelGroup>
                   </h:panelGrid>
                   </h:panelGrid>
                   --%>
                   
                   
                   <h:panelGrid columns="1" width="100%">
                       <f:verbatim><h3><b>Data Portal Login</b></h3><hr></f:verbatim>
                   </h:panelGrid>                   
                   <h:panelGrid columns="2">
                       <h:graphicImage url="./images/e-science.JPG" width="100" height="70" />
                       <h:panelGrid columns="2">                           
                           <h:outputLabel for="userNameInput">
                               <h:outputText value="Enter your user name:"/>
                           </h:outputLabel>
                           <h:inputText id="userNameInput" size="20" maxlength="30" required="true">
                               <f:validateLength minimum="3" maximum="30"/>
                           </h:inputText>                                                      
                           <h:outputLabel for="passwordInput">
                               <h:outputText value="password:"/>
                           </h:outputLabel>
                           <h:inputSecret id="passwordInput" size="20" maxlength="30" required="true">
                               <f:validateLength minimum="5" maximum="25"/>
                           </h:inputSecret>                           
                           <f:verbatim>&nbsp;</f:verbatim>
                           <h:commandButton title="Submit" value="Submit"/>                                        
                       </h:panelGrid> 
                   </h:panelGrid>
                   
                   
                   <%-- 
                   <h:panelGrid columns="2">
                   <h:outputLabel value="first number:"/>
                   <h:inputText id="firstnumber" value="2" required="true"/>
                      
                       
                   <h:outputLabel value="second number"/>
                   <h:inputText id="secondnumber" value="3" required="true"/>
                       
                   </h:panelGrid>
                   --%>
                   
                   
               </h:form>
           </body>
       </html>
       
       <%--
   <</f:view>
   < --%>
   
