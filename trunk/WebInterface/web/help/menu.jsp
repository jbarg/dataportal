<%@page contentType="text/html" import="java.util.*"%>

  <html>
<head>

<title>CLRC Data Portal/Estedi - Basic database search</title>
<link rel="stylesheet" href="../style/main.css">

<script>
<!-- 
var message="That function is not available";
function click(e) {
	if (document.all) {
		if (event.button == 2) {
			alert(message);
		}
	}
	if (document.layers) {
		if (e.which == 3) {
			alert(message);
			return false;
		}
	}
}
	if (document.layers) {
		document.captureEvents(Event.MOUSEDOWN);
	}
document.onmousedown=click
//-->


</script>


</head>
<body background="../img/redbg.gif" onClick="click(event)">

<table width="140" cellspacing=3 border=0>
<tr>
<td valign=top width=140 align=left>
<a href="http://www.clrc.ac.uk"><img border=0 src="../img/clrc.gif" alt="CLRC logo"></a>

<br>&nbsp;<br><br>
<form action="">
<input type="button" class="button" value="Back to top" onClick="parent.footer.location.href='DataPortal_help.htm#Table_of_Contents'"  />

<br />
<br />
<input type="button" class="button" value="Close" onclick="parent.close();" />
</form>



<form name="search_help" action="../servlet/HelpSearchPage" target="footer" method="post">
<input type="button" class="button" value="Back to main" onclick="parent.footer.location.href='DataPortal_help.htm'" />
<br />
<br />
<font color="white">Search<!-- main page--></font><br />
<input type="text" size="15"  name="search_string"/>
</form>

<br />
<br />

<!--<form name="search" action="../servlet/HelpSearch" method="post">
<font color="white">Search indexed pages</font><br/>
<input type="text" size="15"  name="search_string"/>
<br />
<br />
<%/*
    ArrayList xml = (ArrayList)session.getAttribute("xmldocs");
    if(xml != null){
        
        for(int i = 0 ; i < xml.size(); i++){
            out.println("<a  href='../jsp/helpt.jsp?page="+xml.get(i)+".xml' target='footer' >"+xml.get(i)+"</a><br/>");
        }
    
    }   
*/
%>
</form>-->
<td>
</tr>


</table>
  </body>
</html>