
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<%@ page import="java.util.*,java.io.*" %>
<% response.setHeader("pragma","no-cache");
 response.setHeader("cache-control","no-store");
 %>


<%
String name = request.getParameter("page");
session.removeAttribute("xmldocs");
%>
<html>
    <title>CLRC Data Portal/Estedi - Basic Search</title>
    <link rel="stylesheet" href="../style/main.css">

    <frameset border="0" frameborder="0" bgcolor="white" bordercolor="black" cols="150,*">
   	<frame name="menu"  src="menu.jsp" scrolling="no" target="footer">

        <frame name="footer" src="DataPortal_help.htm#<%=name%>"   scrolling="yes">
   </frameset>

</html> 