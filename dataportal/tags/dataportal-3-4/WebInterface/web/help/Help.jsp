
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
    <title>CCLRC Data Portal - Help Pages</title>
    <link rel="stylesheet" href="../style/main.css">

    <frameset border="0" frameborder="0" bgcolor="white" bordercolor="black" cols="230,*">
   	<frame name="menu"  src="menu.jsp" scrolling="yes" target="footer">
       <!-- <frameset border="0" frameborder="0" bgcolor="white" bordercolor="black" rows="50%,*">
            <frame name="menu"  src="menu.jsp" scrolling="no" target="parent.menuBottom">
            <frame name="menuBottom"  src="menuBottom.jsp" scrolling="yes" target="footer">
        </frameset>-->


        <frame name="footer" src="Dataportal.html"   scrolling="yes">
   </frameset>

</html> 