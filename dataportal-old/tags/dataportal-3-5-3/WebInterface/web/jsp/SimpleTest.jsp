
<%@ page  import="java.io.*, java.util.*"%>
 


<% response.setHeader("pragma","no-cache");

 response.setHeader("cache-control","no-store"); %>


<html>
<head><title>JSP Page</title></head>
<body>
<%
    String result = "";
    //FileReader file = new FileReader("c:/TEST");
    //result=  file.

Properties prop = new Properties();
prop.load(new FileInputStream("c:/TEST1.txt"));
result = prop.getProperty("message");


%>
<form action="SimpleTest.jsp" method="post" nowrap>

<textarea>
<%
out.println("<p>"+result+"<p>");
%>
</textarea>
<%-- <jsp:useBean id="beanInstanceName" scope="session" class="package.class" /> --%>
<%-- <jsp:getProperty name="beanInstanceName"  property="propertyName" /> --%>
<input type="submit" value="submit" />
</form>
</body>
</html>
