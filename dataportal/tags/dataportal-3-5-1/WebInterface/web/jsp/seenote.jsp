<%@page  import="java.util.*,org.apache.axis.client.*,javax.xml.rpc.ParameterMode,org.apache.axis.encoding.XMLType,javax.xml.namespace.QName,ac.dl.xml.*,org.apache.log4j.*,java.io.*,java.security.*,java.security.interfaces.*"%>
<%@ include file="loggedin.jsp"%>
<%@ include file="logger.jsp"%>
<%
String sid = (String)session.getAttribute("sessionid");
Properties prop = (Properties)session.getAttribute("props");
%>
<%-- Set the cart instance and pass through the sid, url and properties--%>
<jsp:useBean class="uk.ac.dl.beans.SeeNoteBean" id="cartNote" scope="request">
<jsp:setProperty name="cartNote" property="*" />
<jsp:setProperty name="cartNote" property="sid" value="<%=sid%>" />
<jsp:setProperty name="cartNote" property="properties" value="<%=prop%>" />
</jsp:useBean>
<%--  Call the web service and get user cart information --%>
<% 
   try{
        cartNote.getCart();
   }
   catch(Exception e){
        throw e;
   }
%>
<html>
<head><title>Notes</title>
<script>
function go(){          
    window.setTimeout("window.close();",1000);
}
</script></head>
<body onLoad="addnote.value.focus();">
<p align="center"><b><font size="4"><jsp:getProperty name="cartNote" property="item" /> </font></b><br />
Study:&nbsp;&nbsp;&nbsp;<b>
<jsp:getProperty name="cartNote" property="study" />
</b><br />
<u>Initial Query</u>:<br />
Facility(s):&nbsp;&nbsp;&nbsp;<b>
<jsp:getProperty name="cartNote" property="facilities" />
</b><br />
Discipline:&nbsp;&nbsp;&nbsp;<b>
<jsp:getProperty name="cartNote" property="discipline" />
</b><br />
Waiting time:&nbsp;&nbsp;&nbsp;<b>
<jsp:getProperty name="cartNote" property="time" /> 
&nbsp;seconds</b></p>
<form name="addnote" method="post" action="../servlet/AddNote">
<table>
<tr><td colspan="2">
<input type="hidden" value="<jsp:getProperty name="cartNote" property="url" />" name="url"/>
<textarea name="value" cols="42" rows="<jsp:getProperty name="cartNote" property="srows" />">
<jsp:getProperty name="cartNote" property="notes" />
</textarea>
</td></tr>
<tr><td colspan="2"></td><tr>
<td colspan="2" align="center">
<input type="submit" value="Add note">
<input type="button" value="Close" onClick="window.close();"/>
</td>
</table>
</form>
</body>
</html>
