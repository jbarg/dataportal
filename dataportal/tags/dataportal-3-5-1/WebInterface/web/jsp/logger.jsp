<%@page contentType="text/html"%>
<%
 org.apache.log4j.Logger logger = org.apache.log4j.Logger.getLogger(this.getClass().getName());
 String wd = (String )session.getAttribute("wd");
 //locate the prop file.  Normal get this from web.xml file
 org.apache.log4j.PropertyConfigurator.configure(wd+java.io.File.separator+"WEB-INF"+java.io.File.separator+"logger.properties");
%>