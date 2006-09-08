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
        <link rel="stylesheet" type="text/css" href="../css/basic.css" />
       
        <title>Download Error </title>
                 
      
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
    <body>
        <br />
        <p align="center"><font size="2">Error downloading:</font>   <%= request.getParameter("name") %></p>               
      
      
        <br />
        <p align="center" class="error"><% 

            String error = request.getParameter("message");
            if(error == null){
                error = "Unexpected error downloading data.";
            }
            out.print(error);

        %></p>
        <br />
        <table align="center">
            <td align="right"> <input type="button"  value="Close" onclick="window.close()"/>
            </td>
        </table>
        
    </body>
</html>