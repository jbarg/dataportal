<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">



<%@ page errorPage="error.jsp" import="org.apache.log4j.*,ac.dl.xml.*,java.util.*, java.io.*,org.jdom.input.*,org.jdom.*" %>

<% response.setHeader("pragma","no-cache");

 response.setHeader("cache-control","no-store"); %>

<%@ include file="loggedin.jsp" %>

<%@ taglib uri="/tldweb" prefix="xtags" %>

<html>

<head>

<title>CCLRC Data Portal - History</title>

<script type="text/javascript">

<!--

function openwindow(page){

     // window.open("../help/Help.jsp?page="+page+"","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");
 window.open("../help/Help.jsp","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0");



  //  window.open("../style/help.html","Help", "width=900, height=600, toolbar=0, status=1, location=0,directories=0,menubar=1,resizable=1, scrollbars=1,left=0, top=0");

}

//-->

</script>
<!--<link rel="stylesheet" href="../style/mainNN4.css" type="text/css" /> -->

<style type="text/css"> 

@import url(../style/mainmenu.css); /*IE and NN6x styles*/

</style>

<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />

</head>

<body background="../img/redbg.gif">

<table width="100%" cellspacing="3" border="0">

<tr>

<td class="width" valign="top" width="20%" align="left">

<a href="http://www.clrc.ac.uk"><img border="0" src="../img/clrc.gif" alt="CLRC logo" /></a>

<br />&nbsp;<br /><br />



<!-- out puts the content if the browser is not netsacpe 4.  If so out puts

html/netscape.html file from the content root.-->
<xtags:menu netscapefile="html/netscape.html" iefile="html/ie.html">

</xtags:menu>
<!--<xtags:HPC /> -->
</td>

<td valign="top">

<h1>Data Portal History</h1>
<br/>
&nbsp;Home&nbsp;&nbsp;&nbsp;>&nbsp;<a href="history.jsp">History</a>

<hr />
<br />
<br />
<table border="1"  width="95%" cellspacing="0" cellpadding="1">
<tr bgcolor="#ADD8E6">
<td width="15%"><b>Facility(s)</b></td>
<td width="33%"><b>Discipline</b></td>
<td width="32%"><b>Date</b></td>
<td width="10%"><b>Waiting (seconds)</b></td>
<td width="15%"><b>Resend Query</td>
</tr>
<%
        //set static log for the class
        Logger logger = Logger.getLogger(this.getClass().getName());
        String wd = (String)session.getAttribute("wd");

        //locate the prop file.  Normal get this from web.xml file
        PropertyConfigurator.configure(wd+File.separator+"WEB-INF"+File.separator+"logger.properties");

        if(loggedIn){

            String sid = (String)session.getAttribute("sessionid");
            String workingDir = (String)session.getAttribute("wd");
            String dn = (String)session.getAttribute("dn");
            int n = dn.length();
            StringBuffer h = new StringBuffer();
            for(int i = 0; i<n; i++){
                if( dn.charAt(i) == '/') h.append("_");
                else h.append(dn.charAt(i));
            }
            dn = h.toString();
            File history = new File(wd+File.separator+"profiles"+File.separator+"history"+File.separator+dn);
            if(!history.exists()) history.createNewFile();
            FileReader read = new FileReader(history);
            BufferedReader buff = new BufferedReader(read);
            String str = "";
            int i = 0;
            while((str = buff.readLine()) != null){
                i++;
            }

            buff.close();
            
             FileReader read1 = new FileReader(history);
          
            BufferedReader buff2 = new BufferedReader(read1);
            String str1 = "";
            int j = 0;
            LinkedList list = new LinkedList();
            while((str1 = buff2.readLine()) != null){
            // while(jne /oragince crap on Mon Oct 13 13:23:43 BST 2003 waiting 15 seconds <a> href='../servlet/RedoBasicSearch?url=fdfd'>Resend Query</a></p>
                if((i-j) > 50){}
                else{
                    //out.println(str1);    
                    list.add(str1);
                }
                j++;

            }
            buff2.close();
            
            //out.print the lsit in revers order
            Collections.reverse(list);
            for(int k=0;k<list.size();k++){
                out.println(list.get(k));
            }

        }
        else{}
%>
</table>
<br />
<br />

<hr />

<p class="footer" align="right"><a href="http://validator.w3.org/check/referer"><img

         src="../img/vxhtml.gif"

        alt="Valid XHTML 1.0!"  border="0" height="31" width="88" /></a>

<br />

<br /><a href="http://www.clrc.ac.uk/Activity/ACTIVITY=Copyright;" target="_top">&copy;  2003 CLRC DataPortal</a></p>

</td>

</tr>

</table>

</body>

</html>


