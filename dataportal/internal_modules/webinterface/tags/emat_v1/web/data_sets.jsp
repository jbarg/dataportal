
<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jstl/core" prefix="c"%>
<%@ taglib uri="http://myfaces.apache.org/sandbox" prefix="s"%>
<%@ page import="uk.ac.dl.dp.web.backingbeans.Visit" %>
<%@ page import="uk.ac.dl.dp.web.util.WebConstants" %>
<html>

   
    <head>
        <meta HTTP-EQUIV="Content-Type" CONTENT="text/html;charset=UTF-8" />
        <title>MyFaces - the free JSF Implementation</title>
        <link rel="stylesheet" type="text/css" href="css/basic.css" />
         
    </head>
    <body>
        <f:view>
            
          <table width="${sessionScope.visit.width}"  >  
                 <td width="100%">
            
        
            <table  border="0" width="100%" >
                        <td width="100%"  height="45" > 
                            <f:subview id="header">
                                <c:import url="/inc/header.jsp" />
                            </f:subview>
                        </td>
                    </table>   
           
              
           
            <table border="0" width="100%">
    
                <td width="100%" align="right">
                    <f:subview id="header_2">
                        <c:import url="/inc/header_2.jsp" />
                    </f:subview>
                </td>
                    
            </table>
            
            <table border="0" width="100%">
    
                <td valign="top" width="200">
                    <!-- does not work inside subview??? -->
                    <%--  <f:subview id="navigation">
                    <c:import url="/inc/header2.jsp" />
                    </f:subview>--%>
                    <img align="center" src="../images/cclrc.jpg" width="160" height="40" />
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
                                <c:import url="/inc/title.jsp" />
                            </f:subview>
                        </td>
                    </table>
                    <hr size="-1" width="100%"  />
                    <table border="0" width="100%" >    
                        <td width="7%">&nbsp;</td>
                        <td valign="top" width="93%" >
                            <f:subview id="body">
                                <c:import url="data_sets_inc.jsp" />
                            </f:subview>
                         
                        </td>
                    </table>
                    <br />
                    <br />
                    <hr size="-1" width="100%"  />
                    
                </td>
            </table>
            <br />

            <table border="0" align="right">
                 <f:subview id="footer">
                    <c:import url="/inc/footer.jsp" />
                </f:subview>    
            </table>
            
            </td>
            </table>
        </f:view>
       
    </body>

</html>
