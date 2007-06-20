
<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jstl/core" prefix="c"%>


<%@ taglib prefix="ui" uri="http://java.sun.com/blueprints/ui/14" %>
<%@ taglib uri="https://ajax4jsf.dev.java.net/ajax" prefix="a4j"%>
<%@ taglib prefix="fn" uri="http://java.sun.com/jsp/jstl/functions" %>
<%@ page import="uk.ac.dl.dp.web.backingbeans.Visit" %>
<%@ page import="uk.ac.dl.dp.web.util.WebConstants" %>
<html>
    
    
    <head>
        <meta HTTP-EQUIV="Content-Type" CONTENT="text/html;charset=UTF-8" />
        <title>Sci-Tech Data Portal </title>
        <link rel="stylesheet" type="text/css" href="../css/basic.css" />
        <script language="javascript" src="../css/CastleBusyBox.js"></script>
    </head>
    <body>
        <f:view>
            <table width="${sessionScope.visit.width}" id="top_table" > 
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
                                                <c:import url="inc/header.jsp" />
                                            </f:subview>
                                        </td>
                                    </tr>                                   
                                </tbody>                                
                            </table>
                            <table id="nav_bar_second"   border="0" width="100%" >
                                 <tbody>
                                    <tr>                         
                                        <td width="100%" align="right" >
                                            <f:subview id="header_2">
                                                <c:import url="inc/header_2.jsp" />
                                            </f:subview>
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
                                            --%>    <br />
                                            
                                            <br />
                                            <t:div id="subnavigation_outer">
                                                <t:div id="subnavigation">
                                                    <t:panelNavigation2 renderAll="false" id="nav1" layout="list" 
                                                                        disabledStyle="color:red;padding: 2px 20px 2px 25px">
                                                        <t:navigationMenuItems  id="navitems" value="#{navigationMenu.panelNavigationItems}" />
                                                    </t:panelNavigation2>
                                                </t:div>
                                            </t:div>
                                            
                                            <t:div id="subnavigation_nav">
                                                <f:subview id="navigation_search">
                                                    <c:import url="inc/basic_search_inc_2.jsp" />
                                                </f:subview>
                                            </t:div>
                                        </td>
                                        <td width="*" valign="top" >
                                            <table border="0" width="100%" >  
                                                 <tbody>
                                                    <tr>
                                                        <td align="right" width="130">
                                                            &nbsp;
                                                        </td>
                                                        <td align="left" >
                                                            <h:outputText styleClass="header_top" value="Bookmarks"/> 
                                                            
                                                        </td>
                                                        <td align="right">
                                                            <f:subview id="title">
                                                                <c:import url="inc/title.jsp" />
                                                            </f:subview>
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
                                                                <c:import url="inc/bookmark_inc.jsp" />
                                                            </f:subview>
                                                            
                                                            <a4j:form id="helpform">
                                                                
                                                                <t:collapsiblePanel  id="help" value="#{helpCollapsibleBean.collapsed}" title="testTitle" >
                                                                    <f:facet name="header">
                                                                        <t:div style="color: black; width:40px;">
                                                                            <h:panelGrid columns="4" >
                                                                                <h:outputText rendered="#{!helpCollapsibleBean.collapsed}" value="v " style="color:blue; "/>  <a4j:commandLink reRender="helpform" style="color: black;" styleClass="help" id="helpcj" action="#{helpCollapsibleBean.collapse}" rendered="#{!helpCollapsibleBean.collapsed}" immediate="true"   value="Help"/>
                                                                                
                                                                                <h:outputText rendered="#{helpCollapsibleBean.collapsed}" value="> " style="color:blue; "/>  <a4j:commandLink reRender="helpform" style="color: black;" styleClass="help" id="helpc" action="#{helpCollapsibleBean.collapse}" rendered="#{helpCollapsibleBean.collapsed}" immediate="true"   value="Help"/>
                                                                                
                                                                            </h:panelGrid>
                                                                        </t:div>
                                                                    </f:facet>
                                                                    <h:panelGrid width="600" style="font: 11px Verdana, Arial, Helvetica, sans-serif; background-color:#CCCCCC; ">
                                                                        <h:outputText value="Bookmarks, a persistant place of investigations you have found and are interested in."/>
                                                                        <h:outputText value="Each of the columns can be sorted by clicking on the header once or twice for reverse sort."/>
                                                                        
                                                                        <h:outputText value="Here you can add and view a notes that you want to give to a investigation, or by checking a box and clicking 'Delete Selection' remove a investigation from the list."/>
                                                                        <h:outputText value="Also, to quickly go back to view the parent investigation, click on the hand arrow image and it will take you to the investigations page, or hover over the arrow and then click on datasets to take you straight to the Data Sets page and bypass the investigation page for this investigation."/>
                                                                        
                                                                    </h:panelGrid>
                                                                </t:collapsiblePanel>
                                                            </a4j:form>
                                                            
                                                        </td>
                                                    </tr>
                                                </tbody>
                                            </table>
                                            <br />
                                            
                                            <hr size="-1" width="100%"  />
                                            <table id="footer_bar" border="0" width="100%" align="right">
                                                 <tbody>
                                                    <tr>
                                                        <f:subview id="footer2">
                                                            <c:import url="inc/footer.jsp" />
                                                        </f:subview> 
                                                    </tr>
                                                </tbody>
                                            </table>
                                        </td>
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
