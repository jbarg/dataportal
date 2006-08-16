<%--
<%@page contentType="text/html"%>
<%@page pageEncoding="UTF-8"%>
--%>

<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">
<html>
<%@ include file="../../inc/head.inc" %>
<body>
   <f:view>
<%--           
               <t:panelNavigation2 id="login" layout="list" >
                   <t:commandNavigation2 value="login" action="login"/>
                   <t:commandNavigation2 value="logout" action="logout"/>
               </t:panelNavigation2>
               --%>
               <t:panelNavigation id="qqq">
                   <t:commandNavigation value="qwe" />
                   <t:commandNavigation value="12234">
                       <t:outputText value="we"/>
                   </t:commandNavigation>
                       <t:commandNavigation value="qqq1">
                           <t:outputText value="are"/>
                       </t:commandNavigation>
                       <t:commandNavigation value="qqq2"/>
                       <t:commandNavigation value="qqq3"/>
                  
                   <t:commandNavigation value="qaw"/>
               </t:panelNavigation>
                      
 
   </f:view>
</body>
</html>
