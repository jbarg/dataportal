<%@page contentType="text/html"%>
<%@page errorPage="error.jsp" %>
<%@page import="org.apache.commons.fileupload.*"%>
<%@page import="org.apache.commons.beanutils.*"%>
<%@page import="java.util.*"%>
<%@page import="java.io.*"%>

<jsp:useBean id="admBean" class="uk.ac.clrc.dataportal.facility.client.FacilityAdminBean">
    <jsp:setProperty name="admBean" property="endPoint" value="http://escdmg.dl.ac.uk:8080/facility/services/FacilityAdminService"/>
</jsp:useBean>

<html>
<head><title>Facility Administration Client</title></head>
<body>
<h1>Facility Administration Web Client</h1>
<%
// Process the request
if (request.getMethod().equals("POST")) {
    FileUpload fu = new FileUpload();
    fu.setSizeMax(10240);  // Maximum file size to upload
    fu.setSizeThreshold(10240);
    fu.setRepositoryPath("C:/TEMP"); // System.getProperties("java.io.tmpdir"));

    List fileItems = fu.parseRequest(request);

    Iterator i = fileItems.iterator();

    while (i.hasNext())
    {
        FileItem fi = (FileItem) i.next();

        if (fi.isFormField() && fi.getFieldName().equals("endPoint")) {
            admBean.setEndPoint(fi.getString());
        } else if (fi.isFormField() && fi.getFieldName().equals("payLoad")) {
            admBean.setPayLoad(fi.getString());
        } else if (fi.isFormField()) {
            request.setAttribute(fi.getFieldName(), fi.getString());
        } else if (fi.getName().length() > 0) {
            request.setAttribute("XMLFile",fi.getString());
        }
        fi.delete(); // Don't really want to wait for garbage collection
    }
}
if (request.getMethod().equals("POST") && request.getAttribute("submit") != null) {
    out.print("<h2>Response</h2><textarea cols=80 rows=10>" + admBean.call() + "</textarea>");
%><BR/><A href="/FacilityAdmin.jsp">New Request</A><%
}
if (request.getMethod().equals("GET") || request.getAttribute("upload") != null) {
%>
<form action="FacilityAdmin.jsp" method="POST" ENCTYPE="multipart/form-data">
<table border='0'>
<tr>
<td>Service Endpoint:</td><td><input name="endPoint" type="text" size="100" value="<%=admBean.getEndPoint()%>"></td>
</tr><tr>
<td>Facility Admin XML:</td><td><textarea name="payLoad" cols="80" rows="10"><%if (request.getAttribute("XMLFile") != null) out.print(request.getAttribute("XMLFile"));%></textarea></td>
<tr>
<td>File upload:</td><td><input type="file" name="XMLFile"><input type="submit" name="upload" value="Upload"></td>
</tr>
<tr>
<td>&nbsp;</td><td><input type="submit" name="submit" value="Submit"></td>
</tr>
</table>
</form>
<%
}
%>
</body>
</html>
