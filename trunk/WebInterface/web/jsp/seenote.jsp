<%@page contentType="text/html" import="java.util.*,org.apache.axis.client.*,javax.xml.rpc.ParameterMode,org.apache.axis.encoding.XMLType,javax.xml.namespace.QName,ac.dl.xml.*,org.apache.log4j.*,java.io.*,java.security.*,java.security.interfaces.*"%>

<%

//set static log for the class

 Logger logger = Logger.getLogger("seenote.jsp");

///locate the prop file.  Normal get this from web.xml file
String  wd = (String)session.getAttribute("wd");
PropertyConfigurator.configure(wd+File.separator+"WEB-INF"+File.separator+"logger.properties");
            



String dn = (String)session.getAttribute("sessionid");
//System.out.println("usenaem passed on by seenote is "+username);
//get url passed to see note
String url = request.getParameter("url");
//name of shopping item
String name = null;
//type of shopping item, ie file study
String type = null;
//number of rows for textarea
int rows = 9;
//notes added previously
String[] ret =  new String[5];

//add ws here
  try{
             Properties prop = (Properties)session.getAttribute("props");
            String url2 = prop.getProperty("CART");
            String endpoint = url2;

            Service  service = new Service();
            Call     call    = (Call) service.createCall();
           call.setTargetEndpointAddress( new java.net.URL(endpoint) );
            call.setOperationName( "getNote" );
            call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
            call.addParameter( "op2", XMLType.XSD_INT, ParameterMode.IN );
            call.setReturnType( XMLType.SOAP_ARRAY );

            Object[] ob = new Object[]{url,dn};
            ret = (String[]) call.invoke(ob );
            if(ret[0] == null) ret[0] = ""; //ret[0] is the notes

    }

    catch(Exception e){

            logger.error("Could not display notes for url "+url,e);

    }


        //if name has %20, replace with space
      
       /*StringBuffer buff = new StringBuffer();
        for(int i = 0; i<namel; i++){
        if( ret[1].charAt(i) == '%'){
            buff.delete(i,i+2);
            buff.append(" ");
           }
        else buff.append(ret[1].charAt(i));
        }*/
       // ret[1] = buff.toString();
        //if type is study, remove a row from the textarea to incease space
          int length = ret[3].length();
         rows=9;
         //type  = record.getType();
        if(length > 30) rows=8;
        else if(length > 60) rows=7;
        else if (length > 80) rows=6;
        else rows = 9;

        //get ass study lenght
        if(ret[5].length() > 30) rows--;
        String rowsS = String.valueOf(rows);
%>

<html>

<head><title>Notes</title>

<script>

function go(){
 
          
window.setTimeout("window.close();",1000);

}





</script></head>

<body onLoad="addnote.value.focus();">

<p align="center"><b><font size="4"><%=ret[1]%></font></b><br />
Study:&nbsp;&nbsp;&nbsp;<b>
<%
            if(ret[5] ==null) ret[5] =  "N/A";
            ret[5] = ret[5].replaceAll("%20"," ");
            out.println(ret[5]);

%></b><br />
<u>Initial Query</u>:<br />
Facility(s):&nbsp;&nbsp;&nbsp;<b><%

    if(ret[2] ==null) {
        out.println("N/A");
    ret[2] =  "N/A";
        }
        else{
String[] facs = ret[2].split(" ");
for(int i = 0;i<facs.length;i++){
out.print(facs[i]+" ");
}
}

%></b><br />
Discipline:&nbsp;&nbsp;&nbsp;<b><%
    if(ret[3] ==null) ret[3] =  "N/A";
out.print(ret[3]);
%></b><br />
Waiting time:&nbsp;&nbsp;&nbsp;<b><%

    if(ret[4] ==null) {
        ret[4] =  "N/A";
        out.print(ret[4]);
        }   
        else{
int int1 = Integer.parseInt(ret[4]);
int secs = int1/1000;
out.println(secs);
}

%> &nbsp;seconds</b></p>
<form name="addnote" method="post" action="../servlet/AddNote">

<table>

<tr><td colspan="2">

<input type="hidden" value="<%=url%>" name="url"/>

<textarea name="value" cols="42" rows="<%=rowsS%>"><%=ret[0]%>

</textarea>

</td>

</tr>

<tr><td colspan="2">



</td>

<tr>

<td colspan="2" align="center">



<input type="submit" value="Add note">

<input type="button" value="Close" onClick="window.close();"/>

</td>

</table>

</form>



</body>

</html>
