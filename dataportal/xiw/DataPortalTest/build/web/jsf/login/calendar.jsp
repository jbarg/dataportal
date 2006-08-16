<%--
<%@page contentType="text/html"%>
<%@page pageEncoding="UTF-8"%>
--%>
<%@ page session="false" contentType="text/html;charset=utf-8" %>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t" %>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">

   <f:view>
       <html>
           <head>
               <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
               <title>Data Portal Calendar</title>
           </head>
           <body>
               <%-- --%>
               <%@ include file="../../inc/head.inc" %>
               <%-- --%>
               <f:loadBundle basename="org.apache.myfaces.examples.resource.example_messages" var="example_messages" />
               <t:saveState value="#{calendarBean}" />
               <h:form id="calendarForm">
                   <%-- --%>
                   <t:inputCalendar monthYearRowClass="yearMonthHeader" weekRowClass="weekHeader" currentDayCellClass="currentDayCell" value="#{calendarBean.firstDate}" />
                   <%-- --%>
                   <%--     
                   <t:inputCalendar monthYearRowClass="yearMonthHeader" weekRowClass="weekHeader" currentDayCellClass="currentDayCell" value="#{calendarBean.secondDate}" renderAsPopup="true" popupTodayString="#{example_messages['popup_today_string']}" popupWeekString="#{example_messages['popup_week_string']}" renderPopupButtonAsImage="true" />
                   --%>
               </h:form>
           </body>
       </html>
   </f:view>
