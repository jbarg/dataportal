<%--
<%@page contentType="text/html"%>
<%@page pageEncoding="UTF-8"%>
--%>

<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>

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
 
   <t:dataTable var="datatable" styleClass="standardTable" 
headerClass="standardTable_Header" 
footerClass="standardTanle_Header" 
rowClasses="standardTable_Row1,standardTable_Row2"
 columnClasses="standardTable_Column,standardTable_ColumnCentered,standardTable_Column"
 value="#{countryList.countries}"
 preserveDataModel="true">
       <h:column>
           <h:outputLabel value="Institure:"/>
           <h:outputLabel value="PI:"/>
           
       </h:column>
              <h:column>
           <h:outputLabel value="Institure:"/>
           <h:outputLabel value="PI:"/>
           
       </h:column>
   </t:dataTable>
               </h:form>
           </body>
       </html>
       
       <%--
   <</f:view>
   < --%>
   
