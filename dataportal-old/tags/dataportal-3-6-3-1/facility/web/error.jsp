<%@page contentType="text/html"%>
<%@page isErrorPage="true"%>
<html>
<head><title>JSP Page</title></head>
<body>
<font color=red><H1>ERROR!!</H1></font>
<p><%=exception.getMessage()%></p>
<p><button onclick='history.go(-1)'>Back</button>
</body>
</html>
