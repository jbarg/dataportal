!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<%@ page  errorPage="error.jsp" import="org.apache.axis.client.*,javax.xml.rpc.ParameterMode,org.apache.axis.encoding.XMLType,javax.xml.namespace.QName,ac.dl.xml.*,org.apache.log4j.*,java.util.*, java.io.*,javax.xml.transform.*,javax.xml.transform.dom.*,javax.xml.transform.stream.*,javax.xml.parsers.*,org.jdom.*,org.xml.sax.*,java.security.*,java.security.interfaces.*,java.math.*" %>
 

<% response.setHeader("pragma","no-cache");

 response.setHeader("cache-control","no-store");

 %>

<html>
<head><title>Logon page</title></head>
<body>
<%
        String sessionid = request.getParameter("sessionid");
        
        Service  service = new Service();
        Call     call    = (Call) service.createCall();
        call.setTargetEndpointAddress( new java.net.URL(endpoint) );
        call.setOperationName( "getCart" );
        call.addParameter( "op1", XMLType.XSD_INT, ParameterMode.IN );
        cal<table width="90%" cellspacing="0" border="0">

<tr><td id="headmenu">Main</td><tr><td>
<hr class="menuhr" />
&nbsp;&nbsp;<a class="menu" href="BasicSearch.jsp">Basic Search&nbsp;&nbsp;</a><br/>
&nbsp;&nbsp;<a class="menu" href="Checkout.jsp">Shopping Cart&nbsp;&nbsp;</a><br />
&nbsp;</td></tr>

<tr><td id="headmenu">User</td><tr><td>
<hr  class="menuhr" />
&nbsp;&nbsp;<a class="menu" href="history.jsp">History&nbsp;&nbsp;</a><br />
&nbsp;&nbsp;<a class="menu" href="../servlet/SaveCartServlet">Log out&nbsp;&nbsp;</a><br />
&nbsp;</td></tr>

<tr><td id="headmenu">Information</td><tr><td>
<hr  class="menuhr" />
&nbsp;&nbsp;<a class="menu" href="#" onclick="openwindow('_Toc13298570');return false">Help&nbsp;&nbsp;</a><br />
&nbsp;&nbsp;<a class="menu" href="#" onclick="return false;">Documentation&nbsp;&nbsp;</a><br />
&nbsp;&nbsp;<a class="menu" href="http://www.e-science.clrc.ac.uk/web/projects/dataportal" >Project&nbsp;&nbsp;</a></td></tr>

</table>l.setReturnType( XMLType.SOAP_ELEMENT );
        //old way using invoke
        Object[] ob = new Object[]{dn};
        ret = (org.w3c.dom.Element) call.invoke(ob );


%>
</body>
</html>
