<%@page contentType="text/html"%>
<%@page pageEncoding="UTF-8"%>


<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"
   "http://www.w3.org/TR/html4/loose.dtd">

<html>
    <head>
        <meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
        <link rel="stylesheet" type="text/css" href="../css/basic.css" />
       
        <title>Session Timed Out </title>
                 
      
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
            body {
            background: #E8E8E8 !important;
            }

        </style>        
    </head>
    <body>
        <br />
        <br >
       
        <p align="center" class="error">Data Portal Session Timed Out.</p>
       <br >
        <table align="center">
            <td align="right"> <input type="button"  value="Close" onclick="window.close()"/>
            </td>
        </table>
        
    </body>
</html>