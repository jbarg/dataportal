<%@page contentType="text/html" %>
<%@page import="uk.ac.clrc.dataportal.facility.*, org.apache.log4j.*"%>
<% response.setHeader("pragma","no-cache");
 response.setHeader("cache-control","no-store");
 %>
<%@ taglib uri="http://java.sun.com/jstl/core_rt" prefix="c" %>
<%@ include file="loggedin.jsp" %>

<%@ include file="logger.jsp" %>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN">
<html>
<head>

<title>CCLRC Facility Manager</title>
<link rel="stylesheet" href="../style/mainmenu.css" type="text/css">


<script>
//checks that all fields are not nothing
function check(){


return true;

}

</script>
</head>
<body>
<a name="top" />
<table width="100%" cellspacing="3" border="0"><tr>
<td valign="top" width="180" align="left">
<!--<a href="http://www.clrc.ac.uk"><img border=0 src="../img/clrc.gif" alt="CLRC logo"></a>-->
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;
<br />&nbsp;<br /><br />
<br />&nbsp;<br /><br />
<table width="125" cellspacing="0" border="0">

<tr><td id="headmenu">Facility</td><tr><td>
<hr class="menuhr" />
<!--&nbsp;&nbsp;<a class="menu" href="create.jsp">Create Facility</a><br/>-->
&nbsp;&nbsp;<a class="menu" href="deleteFacility.jsp">List </a><br />
&nbsp;&nbsp;<a class="menu" href="logoff.jsp">Log Off</a><br />

&nbsp;</td></tr>



</table>

</td>
<td valign="top">
<p id="topheader">Facility list</p>
<br /><br /><br /><br />
<hr />
<div align="right"><font size="-1"><a align="right" href="#bottom">bottom</a></font></div>

<br />
<table border="1" cellpadding="1" cellspacing='0'>
<tr><td colspan="4" align="left" width="400"><b>Facility(s)</d></td></tr>
<tr><td colspan="4" width="3">&nbsp;</td></tr>
<%
    
    //List all the facilities with the Data Portal
    UDDIHelper uddi = new UDDIHelper();
    org.jdom.Element[] elements = uddi.findFacility("%");

    //check if dataportal already exists
    boolean isDataPortal = false;
    for(int i = 0; i < elements.length;i++){
        org.jdom.Element el = (org.jdom.Element)elements[i];
        String facilityName = el.getText();
       
        
        String facilityID = el.getAttribute("ID").getValue();
        out.println("<tr>");
        out.println("<td width=\"180\">"+facilityName+"</td>");
        if(!facilityName.equalsIgnoreCase("dataportal") && (elements.length != 1)){ 
             out.println("<td align=\"center\"><a onmouseover=\"window.status='Delete Facility';return true;\" onmouseout=\"window.status='';\" href=\"../servlet/deleteFacilityServlet?id="+facilityID+"\" >delete</a></td>");
        }  
        else if(facilityName.equalsIgnoreCase("dataportal") && (elements.length == 1)){ 
             out.println("<td align=\"center\"><a onmouseover=\"window.status='Delete Facility';return true;\" onmouseout=\"window.status='';\" href=\"../servlet/deleteFacilityServlet?id="+facilityID+"\" >delete</a></td>");
        }  
        else out.println("<td align=\"center\">&nbsp;</td>");
      
        out.println("<td align=\"center\"><a onmouseover=\"window.status='Edit Facility';return true;\" onmouseout=\"window.status='';\" onclick=\"alert('Not avaliable yet'); return false;\" href=\"../servlet/editFacilityServlet?id="+facilityID+"\" >edit</a></td>");
        if(facilityName.equalsIgnoreCase("dataportal")){ 
            isDataPortal = true;
            out.println("<td align=\"center\"><a onmouseover=\"window.status='Create Facility for the Data Portal';return true;\" onmouseout=\"window.status='';\" href=\"createFacility.jsp\" >add facility</a></td>");
        }
        else{
            out.println("<td align=\"center\">&nbsp;</td>");
        }
        out.println("</tr>");
        //out.println(facilityName+" &nbsp;&nbsp;&nbsp;&nbsp;       <a href=\"../servlet/deleteFacilityServlet?id=nothing\" >"+facilityID+"</a><br />");
        log.debug(facilityName+" : "+facilityID);
     }
%>

</table>
<br />
<%
    //check if dataportal exists    
    if(!isDataPortal) { %> <input type="button" value="Create Data Portal" onclick="location.href='createDataPortal.jsp'"/>&nbsp;&nbsp;&nbsp;&nbsp;<font color="red" size="3">A Data Portal instance needs to be created before you can add facilities</font><%}%>
    <c:if test="${param.error != null}">
        <br /><font color="red">${param.error}</font>
    </c:if>


<br />
<div align="right"><a name="bottom"/><font size="-1"><a align="right" href="#top">top</a></font></div>

<hr />
<%@ include file="../html/footer.html"%>
</td>
</table>

</body>
</html>