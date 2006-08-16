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
                   <table border="2">
                       <tr>
                           <td>
                               <h:graphicImage value="../../images/e-science.JPG" width="100" height="70" />
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
               </h:form>
           </body>
       </html>
       
       <%--
   <</f:view>
   < --%>
   
