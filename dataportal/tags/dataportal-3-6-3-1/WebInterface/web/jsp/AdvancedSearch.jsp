<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<%@ page import="java.util.*,java.io.*"  errorPage="error.jsp"%>
<% response.setHeader("pragma","no-cache");
 response.setHeader("cache-control","no-store");
 %>
 
 <%@ include file="loggedin.jsp" %>





<html>
<head>  
<script type="javascript">



function checking(){
alert("we are checking");
   /* if(document.study.StudyName.length < 1){
        alert("Please enter a value in the Study Name");
        return false;
    }
else return true;*/
}
</script>
 <title>CCLRC Data Portal - Advanced Search</title>
    <link rel="stylesheet" href="style/main.css" />
</head>
   
    <base target="_top" />
    <frameset border="0" frameborder="0" bgcolor="white" bordercolor="black" cols="18%,*">
  	<frame name="qinetiq_logo"  src="menuAdvanced.jsp" scrolling="no" target="parent">
  	   		
        <frame name="footer" src="AdvancedSearch1.jsp?level=1&facility=BADC" target="main" scrolling="yes">
   </frameset>



</html>
