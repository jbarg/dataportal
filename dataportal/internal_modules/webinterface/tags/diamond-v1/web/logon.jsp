
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
        <title>Sci-Tech Data Portal </title>
        <link rel="stylesheet" type="text/css" href="css/basic.css" />
        
    </head>
    <body>
        <f:view>   
            <%
            // This is just if you want to kill the session for the current user so you can start fresh without restarting tomcat.
            // Good for when making changes to the component tree
            // if(!session.isNew()){
            //     session.invalidate();
            // }
            %>
            
            
            <table width="954" >
                 <tbody>
                    <tr>
                        <td width="100%">
                            
                            <table   border="0" width="100%" id="image_stfc" >
                                 <tbody>
                                    <tr>
                                        <td width="100%" > 
                                            <%--  <td id="nav_bar" width="100%"  > 
                                            <f:subview id="header">
                                                <c:import url="inc/header.jsp" />
                                            </f:subview>
                                             
                                        </td>--%>
                                            <f:subview id="header">
                                                <c:import url="inc/header_img.jsp" />
                                            </f:subview>
                                        </td>
                                    </tr>                                   
                                </tbody>                                
                            </table>
                            <table id="nav_bar_second"   border="0" width="100%" >
                                 <tbody>
                                    <tr>                         
                                        <td width="100%" align="right" >
                                            <%--     <f:subview id="header_2">
                                                <c:import url="inc/header_2.jsp" />
                                            </f:subview>--%>
                                            <br />
                                              <br />
                                        </td>
                                    </tr>                                    
                                </tbody>                                
                            </table>
                            
                            <table id="nav_bar_accross"  border="0" width="100%">
                                 <tbody>
                                    <tr>
                                        <td valign="top" width="200">
                                            <!-- does not work inside subview??? -->
                                            <%-- <f:subview id="navigation">
                                                <c:import url="inc/header_img.jsp" />
                                            </f:subview>--%>
                                            <%--
                                            <img align="center" src="../../images/diamond.gif" width=170" height="60" />
                                            --%>
                                            <br />
                                            <br />
                                            <%-- <t:div id="subnavigation_outer">
                                            <t:div id="subnavigation">
                                            <t:panelNavigation2 id="nav1" layout="list" itemClass="mypage" activeItemClass="selected"
                                            disabledStyle="color:red;padding: 2px 20px 2px 25px">
                                            <t:navigationMenuItems id="navitems" value="#{navigationMenu.panelNavigationItems}" />
                                            </t:panelNavigation2>
                                            </t:div>
                                            </t:div>--%>
                                        </td>
                                        <td width="*" valign="top" >
                                            <table border="0" width="100%" >  
                                                 <tbody>
                                                    <tr>
                                                        <td align="right">
                                                            <%-- <f:subview id="title">
                                                            <c:import url="/inc/title.jsp" />
                                                            </f:subview>--%>
                                                            <br />
                                                            <br />
                                                        </td>
                                                    </tr>
                                                </tbody>
                                            </table>
                                            <hr size="-1" width="100%"  />
                                            <table border="0" width="100%" >    
                                                 <tbody>
                                                    <tr>
                                                        <td width="7%">&nbsp;</td>
                                                        <td valign="top" width="93%" >
                                                            <f:subview id="body">
                                                                <c:import url="logon_inc.jsp" />
                                                            </f:subview>                         
                                                        </td>
                                                    </tr>
                                                </tbody>
                                            </table>
                                            <br />
                                            <br />
                                            <hr size="-1" width="100%"  />                    
                                        </td>
                                    </tr>
                                </tbody>
                            </table>
                            
                            <table id="footer_bar" border="0" width="100%" align="right">
                                
                                 <tbody>
                                    <tr>
                                        <f:subview id="footer">
                                            <c:import url="inc/footer.jsp" />
                                        </f:subview>  
                                    </tr>
                                </tbody>
                            </table>
                        </td>
                    </tr>
                </tbody>
            </table>
        </f:view>
        
    </body>
    
</html>
