<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">

<%@ page import="java.util.*,java.io.*" %>

<% response.setHeader("pragma","no-cache");

 response.setHeader("cache-control","no-store");

 %>

 <%@ include file="loggedin.jsp" %>

    <%

    //System.out.println("HELLLLLEEEEOOO");
    ArrayList facs = (ArrayList)session.getAttribute("facs");
    String facility = null;
    if(facs == null) facility = "No_Access";
    else{ facility = (String)facs.get(0);}
   

    //get the topic mana
%>



<html>



    <title>CCLRC Data Portal - Basic Search</title>

   <link rel="stylesheet" href="../style/mainNN4frame.css" type="text/css" /> 

<style type="text/css"> 

@import url(../style/mainframe.css); /*IE and NN6x styles*/

</style>

    <base target="_top" />

   <frameset border="0" frameborder="0" bgcolor="white" bordercolor="black" cols="220,*">

  	<frame name="qinetiq_logo"  src="menu.jsp" scrolling="no" target="parent" />
       <frameset border="0"  frameborder="0" bgcolor="white" bordercolor="black" rows="98,*">
        <frame name="header" src="../html/top.html"   scrolling="no" />
        <frame name="footer" src="dataxmlformemat<%=topics%>.jsp?list=<%=facility%>"   scrolling="no" />
        
        </frameset>
   </frameset>

 

</html>
