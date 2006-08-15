
<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib prefix="tiles" uri="http://struts.apache.org/tags-tiles"%>
<html>

   
    <head>
        <meta HTTP-EQUIV="Content-Type" CONTENT="text/html;charset=UTF-8" />
        <title>MyFaces - the free JSF Implementation</title>
        <link rel="stylesheet" type="text/css" href="css/basic.css" />
        <link rel="stylesheet" type="text/css" href="../css/basic.css" />
      
        <style>

            .dojoHtmlFisheyeListBar {
            margin: 0 auto;
            text-align: center;
            }

            .outerbar {
            background-color: #666;
            text-align: center;
            position: absolute;
            left: 0px;
            top: 0px;
            width: 100%;
            }

            body {
            font-family: Arial, Helvetica, sans-serif;
            padding: 0;
            margin: 0;
            }

            .page {
            padding: 60px 20px 20px 20px;
            }            
            
            a:link 		{ color: black; font-size: -1; text-decoration: none; }

            a:visited 	{ color: #0000FF; text-decoration: none;  }

            a:hover		{ color: #999999; text-decoration: underline;  }

            a:active 	{ color: #9999CC; text-decoration: underline;  }

        </style>
    </head>
    <body>
        <f:view>
            
            <f:loadBundle
            basename="uk.example_messages"
            var="example_messages" />
            
            <table  border="0" width="950" bgcolor="#666">
                <td width="950"  height="50" > 
                    <f:subview id="header">
                        <tiles:insert attribute="header" flush="false" />
                    </f:subview>
                </td>
            </table>   
              
            <!--  <br />
            <br /><br />
            <br />-->
              
           
            <table border="0" width="950">
    
                <td width="100%" align="right">
                    <f:subview id="header_2">
                        <tiles:insert attribute="header_2" flush="false" />
                    </f:subview>
                </td>
                    
            </table>
            
            <table border="0" width="950">
    
                <td valign="top" width="200">
                    <!-- does not work inside subview??? -->
                    <!--<f:subview id="navigation">
                    <tiles:insert attribute="navigation" flush="false" />
                    </f:subview>-->
                    <img align="center" src="images/cclrc.jpg" width="160" height="40" />
                    <br />
                    <br />
                    <t:div id="subnavigation_outer">
                        <t:div id="subnavigation">
                            <t:panelNavigation2 id="nav1" layout="list" itemClass="mypage" activeItemClass="selected"
                                disabledStyle="color:red;padding: 2px 20px 2px 25px">
                                <t:navigationMenuItems id="navitems" value="#{navigationMenu.panelNavigationItems}" />
                            </t:panelNavigation2>
                        </t:div>
                    </t:div>
                </td>
                <td width="*" valign="top" >
                    <table border="0" width="100%" >  
                        <td align="right">
                        <f:subview id="title">
                            <tiles:insert attribute="title" flush="false" />
                        </f:subview>
                        </td>
                    </table>
                    <hr size="-1" width="100%"  />
                    <table border="0" width="100%" >    
                        <td width="7%">&nbsp;</td>
                        <td valign="top" width="93%" >
                            <f:subview id="body">
                                <tiles:insert attribute="body" flush="false" />
                            </f:subview>
                        
                        </td>
                    </table>
                    <br />
                    <br />
                    <hr size="-1" width="100%"  />
                    
                </td>
            </table>
            <br />

            <table border="0" >
                <f:subview id="footer">
                    <tiles:insert attribute="footer" flush="false" />
                </f:subview>
            </table>
        </f:view>
       
    </body>

</html>
