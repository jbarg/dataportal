<%--
<%@page contentType="text/html"%>
<%@page pageEncoding="UTF-8"%>
--%>

<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">
<%--
   <f:view>
       --%>
       <%--
       <html>
           <head>
               <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
               <title>Data Portal Basic Search Page</title>
           </head>
           <body>
           --%>
   
               
               <font face="Arial, sans-serif" size="4">
                   <h:outputText value="Basic Search Page" />
               </font>
               <hr>
               <h:form>
                   <table>
                       <tr>
                           <td>
                               <h:outputLabel for="facilities">
                                   <h:outputText value="Facilities:" />
                               </h:outputLabel>
                           </td>
                           <td>
                               <h:inputText id="facilities" size="20" maxlength="30"/>
                           </td>
                           <td/>
                       </tr>                              
                       <tr>
                           <td>
                               <h:outputLabel for="keywords">
                                   <h:outputText value="Keywords:" />
                               </h:outputLabel>
                           </td>
                           <td>
                               <h:inputText id="keywords" size="30" maxlength="50"/>
                           </td>
                           <td/>
                       </tr>  
                       <tr>
                           <td>
                               <h:outputLabel for="hits-displayed">
                                   <h:outputText value="Hits Displayed:" />
                               </h:outputLabel>
                           </td>
                           <td>
                               <h:selectOneMenu value="select-hits" id="hitsdisplayed" >
                                   <f:selectItem itemValue="10"/>
                                   <f:selectItem itemValue="15"/>
                                   <f:selectItem itemValue="30"/>
                                   <f:selectItem itemValue="50"/>
                               </h:selectOneMenu>
                           </td>
                           <td/>
                       </tr>  
                      
                       <tr>

                           <td>
                               <h:commandButton action="basicsearch" title="search" value="Search"/>
                           </td>
                           <td/>
                           <td/>
                       </tr>
                             
                   </table>
  
               </h:form>
        
               <%--
           </body>
       </html>
       --%>
       
       <%--
   </f:view> 
   --%>
   
