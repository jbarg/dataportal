<html>
<%@page contentType="text/html"%>
<head><title>JSP Page</title></head>
<link rel='stylesheet' href='default.css'>
<body>
<h1>Javascript vs. JSP/Servlets!</h1>
<h2>The JavaScript version</h2><P>
<TABLE border='1'><TR>
<SCRIPT>
<!-- -->
for(i=0; i<10; i++){
    document.write("<TD>" + i + "</TD>");
}
</SCRIPT>
</TR></TABLE>
<SCRIPT>
document.write("<B>Last modified:</B> " + document.lastModified + "<BR>");
now = new Date();
localTime = now.toString();
GMTtime = now.toGMTString();
document.write("<B>Local time:</B> " + localTime + "<BR>");
document.write("<B>UTC time:</B> " + GMTtime + "</P>");
document.write("<FONT size='+2'>");
document.write(now.getHours() + ":" + now.getMinutes() + ":" + now.getSeconds());
document.write("</FONT>");

//Scrolling message
var msg = "The quick brown fox jumps over the lazy dog         ";
var scrolling = true;
function changeScrollText(){
    msg = prompt("Enter the scroll text.");
    scrolling = false;
    window.status = "";
    if ( msg > "" ){
        scrolling = true;
        pos = 0;
        scrollMessage();
    }
}
var pos = 0;
function scrollMessage(){
    window.status = msg.substring( pos, msg.length ) + msg.substring( 0, pos );
    pos++;
    if ( pos > msg.length ) pos=0;
    if ( scrolling ) window.setTimeout("scrollMessage()",200);
}
scrollMessage();

function clickAlert(){
    alert( "You clicked." );
}

//Reload to update time
window.setTimeout("location.reload()",10000);

// -->
</SCRIPT>

<h2>And the JSP version</h2><p>
<TABLE border='1'><TR>
<%
for ( int i=0; i<10; i++ ){
    out.print("<TD>" + i + "</TD>");
}
%>
</TR></TABLE>
<%
java.util.Date now = new java.util.Date();
out.print("<FONT size='+2'>");
out.print(now.getHours() + ":" + now.getMinutes() + ":" + now.getSeconds());
out.print("</FONT><BR>");
out.print("End of JSP version.</p>");
%>
</P>

<%-- <jsp:useBean id="beanInstanceName" scope="session" class="package.class" /> --%>
<%-- <jsp:getProperty name="beanInstanceName"  property="propertyName" /> --%>

//Javascript history
<TABLE width='100%'><TR><TD align='CENTER'>
<A href='javascript:history.back()'>Back</A>
</TD><TD align='CENTER'>
<A href='javascript:history.forward()'>Forward</A>
</TD><TD align='CENTER'>
<A href='javascript:location.reload()'
   onmouseover='window.status = "Reload this page"; return true;'
   onmouseout='window.status = "";'>Reload</A>
</TD></TR>
<TR><TD align='CENTER'>
<A name='Change' href='#Change' onClick='changeScrollText();'>Change Scroll Text</A>
</TD><TD align='CENTER'>
<A name='Here' href='#Here' onClick='clickAlert();'>Click me!</A>
</TD></TR></TABLE>

</body>
</html>
