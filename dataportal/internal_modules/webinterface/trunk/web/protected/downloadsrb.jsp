<%@page contentType="text/html"%>
<%@page pageEncoding="UTF-8"%>
<%--
The taglib directive below imports the JSTL library. If you uncomment it,
you must also add the JSTL library to the project. The Add Library... action
on Libraries node in Projects view can be used to add the JSTL 1.1 library.
--%>
<%--
<%@taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%> 
--%>

<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">

<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
        <meta http-equiv="refresh" content="1">
        <title>Download</title>
        
        <script>
            function remove(){
            var close = <%= request.getParameter("close")%>;
               
            if(navigator.appName == "Microsoft Internet Explorer"){
            setTimeout("nothing()", 1);
            window.close();
            setTimeout("remove()", 100);
            }
                
                
            }
            
            function nothing(){
            }
            
            function Func1Delay()
            {
            //sleep two seconds
            setTimeout("remove()", 2000);
            }
            
        </script>
      
        <style>
            .bpui_progressbar_barAreaContainer {
            border-style: groove !important;
            border-width: medium !important;
            }
    
            .bpui_progressbar_portionComplete {
            background: #FF0000 !important;
            }

            .bpui_progressbar_portionRemaining {
            background: lightgrey !important;
            }    
            .bpui_progressbar_barAreaContainer {
            }

            .bpui_progressbar_barArea {
            height: 18px;
            }

            .bpui_progressbar_portionComplete {
            background: #00FF00;
            }

            .bpui_progressbar_portionRemaining {
            background: #D9E6EC;
            }

            .bpui_progressbar_percentageText {
            text-align: center;
            }

        </style>        
    </head>
    <body onload="Func1Delay();">

        <p><font size="3">Downloading</font>..   <%= request.getParameter("name") %></p>               
      
        <%
            String percent = request.getParameter("percentage") ;
            if(percent == null){
                percent = "0";
            }
        %>  
        
        <br />
        <table id="progressBar_barAreaContainer" class="bpui_progressbar_barAreaContainer" border="0" cellspacing="0" cellpadding="0" width="100%">
            <tr id="progressBar_barArea" class="bpui_progressbar_barArea">
                <td id="progressBar_portionIndeterminate" class="bpui_progressbar_portionRemaining" width="0%"></td>
                <td id="progressBar_portionComplete" class="bpui_progressbar_portionComplete" width="<%= percent %>%"></td>
                <td id="progressBar_portionRemaining" class="bpui_progressbar_portionRemaining" width="<%= (100-Integer.valueOf(percent)) %>%"></td>
            </tr>
        </table><div id="progressBar_percentageText" class="bpui_progressbar_percentageText"><%= percent %>%</div>

        <%-- <TABLE border=0 width='<%= percent %>%' bgcolor='red'>
        <TR><TD width='100%'> </TD></TR>
        </TABLE>--%>
    </body>
</html>
