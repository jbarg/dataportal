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
       
       <html>
           <head>
               <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
               <title>Data Portal Basic Search Page</title>
           </head>
           <body>
       
 
               <h:form>
                   <%--                   
                   <h:panelGrid columns="1" width="100%">
                   <h:panelGrid columns="1" width="100%">
                   <f:verbatim><h2><b>Basic Search</b></h2><hr></f:verbatim>
                   </h:panelGrid>
                       
                   <h:panelGrid columns="2">
                   <h:panelGrid columns="1">
                   <h:outputLabel value="Facilities:"/>
                   <h:outputLabel value="Keywords:"/>
                   <h:outputLabel value="Hits Dsiplayed:"/>
                   <h:outputLabel value="Click to search:" />
                   </h:panelGrid>

                   <h:panelGrid columns="1">
                   <h:inputText id="facilities" size="20" maxlength="30"/>
                   <h:inputText id="keywords" size="30" maxlength="50"/>
                   <h:selectOneMenu value="select-hits" id="hitsdisplayed" >
                   <f:selectItem itemValue="10"/>
                   <f:selectItem itemValue="15"/>
                   <f:selectItem itemValue="30"/>
                   <f:selectItem itemValue="50"/>
                   </h:selectOneMenu>
                   <h:commandButton action="basicsearch" title="search" value="Search"/>
                   </h:panelGrid>
                   </h:panelGrid>
                       
                   </h:panelGrid>
                   
                   --%>
                   <h:panelGrid columns="1" width="100%">
                       <h:panelGrid columns="1" width="100%">
                           <f:verbatim><h2><b>Basic Search</b></h2><hr></f:verbatim>
                       </h:panelGrid>
                       
                       <h:panelGrid columns="2">

                           <h:outputLabel value="Facilities:"/>
                           <h:inputText id="facilities" size="20" maxlength="30"/>
                           <h:outputLabel value="Keywords:"/>
                           <h:inputText id="keywords" size="30" maxlength="50"/>
                           <h:outputLabel value="Hits Dsiplayed:"/>
                           <h:selectOneMenu value="select-hits" id="hitsdisplayed" >
                               <f:selectItem itemValue="10"/>
                               <f:selectItem itemValue="15"/>
                               <f:selectItem itemValue="30"/>
                               <f:selectItem itemValue="50"/>
                           </h:selectOneMenu>
                           <f:verbatim>&nbsp;</f:verbatim>
                           <h:commandButton action="basicsearch" title="search" value="Search"/>

                       </h:panelGrid>
                       
                   </h:panelGrid>
                   

               </h:form>
        
               
           </body>
       </html>
       
       
       <%--
   </f:view> 
   --%>
   
