<%@page contentType="text/html" import="java.util.*,org.apache.lucene.search.*"%>

  <html>
<head>

<title>CLRC Data Portal - Help Pages</title>
<!--<link rel="stylesheet" href="../style/main.css">-->
<style type="text/css"> 

.menuhr { color: grey ; height: 2px; }


.menu:link, .menu:visited {
  color: blue;
  text-decoration: none;
  font-family: tahoma, verdana, sans-serif;
  font-size: 14px;
}
.menu:hover {
  color: blue;
text-decoration: none;
  font-family: tahoma, verdana, sans-serif;
  font-size: 15px;
}

#headmenu {
font-family: times,tahoma, verdana, sans-serif;
  font-size: 17px;
color: black;

}

#headmenu2 {
font-family: times,tahoma, verdana, sans-serif;
  font-size: 17px;


}

a:link 		{ color: BLUE; text-decoration: none; }



a:visited 	{ color: #0000FF; text-decoration: none;  }



a:hover		{ color: #999999; text-decoration: underline;  }



a:active 	{ color: #9999CC; text-decoration: underline;  }


</style>

<script>
function check(){
   // alert(document.search.search_string.value);
    if(document.search.search_string.value == null) {
        alert("Please enter a search string");
        return false;
    }
    else if(document.search.search_string.value == "") {
        alert("Please enter a search string");
        return false;
    }
    else{
       // alert("true");
        return true;
        }

}


<!-- 
var message="That function is not available";
/*function click(e) {
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
document.onmousedown=click*/
//-->


</script>


</head>
<!--onClick="click(event)"-->
<body onload="document.forms['search'].search_string.focus();">



<a href="http://www.clrc.ac.uk"><img border="0" src="../img/clrc.gif"  alt="CCLRC logo" border="0"></a>

<br />&nbsp;<br /><br />

<!--<form action="">
<input type="button" class="button" value="Back to top" onClick="parent.footer.location.href='DataPortal_help.htm#Table_of_Contents'"  />

<br />
<br />
<input type="button" class="button" value="Close" onclick="parent.close();" />
</form>-->
<a href="Dataportal.html" target="footer" id="headmenu2" > Help</a>
<br />
<a href="#" onclick="parent.close();" id="headmenu2"/>Close</a> 

<!--
<form name="search_help" action="../servlet/HelpSearch1" target="footer" method="post">
<input type="button" class="button" value="Back to main" onclick="parent.footer.location.href='DataPortal_help.htm'" />
<br />
<br />
<font color="white">Search--><!--</font><br />
<input type="text" size="15"  name="search_string"/>
</form>alert("false1");
-->
<!--
<br />
<br />
-->
<!--
<form name="search" action="../servlet/HelpSearch1" method="post">
<font color="white" size="4"> Search</font><br/>
<input type="text" size="15"  name="search_string"/>
</form>
-->
<hr />
<br />
<form name="search" method="post" onsubmit="return check();" action="../servlet/HelpSearch1">
<span id="headmenu">Type in keyword(s)</span>
<br />
 <%
 String text = (String)session.getAttribute("text");

 if(text == null) text = "";
text = text.replaceAll("\"","'");
%>
<input name="search_string" type="text" value="<%=text%>" size="23"/>
<br />
&nbsp;
&nbsp;
&nbsp;
&nbsp;&nbsp;&nbsp;
&nbsp;
&nbsp;
&nbsp;
&nbsp;
<input type="submit" value="List topics" />
</form>
<%
    Hits hits = (Hits)session.getAttribute("hits");
  Searcher search = (Searcher)session.getAttribute("searcher");
    
    if(hits == null){}
    else if(hits.length()!=0){
    System.out.println(hits.length());
%>        
<hr />
<br />
<span id="headmenu">Select topic to display:</span>
<br />
<br />
<table>
<tr>
   <td width="100" id="headmemu"><u>Title</u></td>
       <td id="headmemu"><u>Rank</u></td>
	
</tr>
<%

 for(int i = 0 ; i < hits.length(); i++){

            //added section to find out if xml or html document
           
           String name = hits.doc(i).get("name");
             String path = hits.doc(i).get("path");
            float rank = hits.score(i);
            String floatS = String.valueOf(rank);
           // System.out.println("floatS "+floatS);
            String number = "0";
            if(floatS.length() < 4){
                 if(floatS.equals("1.0")) number = "10";
                 else if(floatS.length() == 3){
                    number = floatS.substring(2,3);
                }
                else if(floatS.length()< 3){
                    if(floatS.equals("1.0")) number = "10";
                    else number = "1";
                }
            }
            else{
                floatS = floatS.substring(0,4);
                rank    = Float.parseFloat(floatS);
                number = floatS.substring(2,3);
            }
            //System.out.println("Score is "+rank);
           /// System.out.println("number is "+number);
           // System.out.println("name is "+name);

            //add %20 to spaces
             int no1 = name.length();
            StringBuffer h1 = new StringBuffer();
            for(int j = 0; j<no1; j++){
                if( name.charAt(j) == ' ') h1.append("%20");
                
                else h1.append(name.charAt(j));
            }
           String name2 = h1.toString();
            
            
            out.println("<tr><td>");
            if(hits.doc(i).get("type").equals("xml")){
                out.println("<a class=\"menu\" href='../jsp/help2.jsp?page="+name2+"' target='footer'>"+name+"</a></td>");
                out.println("<td><img src=\"../img/"+number+"0.gif\" width=\"40\" height=\"10\"/></td></tr>");
            }
            else{
                out.println("<a class=\"menu\" href='xml/"+name2+".html' target='footer'>"+name+"</a></td>");
                out.println("<td><img src=\"../img/"+number+"0.gif\" width=\"40\" height=\"10\"/></td></tr>");
           }
        }
out.println("</table>");
}
if(search != null){
search.close();
}
search = null;
hits = null; 
%>




    

  </body>
</html>