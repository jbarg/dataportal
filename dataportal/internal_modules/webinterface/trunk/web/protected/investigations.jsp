
<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jstl/core" prefix="c"%>

<%@ taglib uri="https://ajax4jsf.dev.java.net/ajax" prefix="a4j"%>

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
                                            --%> 
                                            <br />
                                            <br />
                                            <t:div id="subnavigation_outer">
                                                <t:div id="subnavigation">
                                                    <t:panelNavigation2 renderAll="false" id="nav1" layout="list" itemClass="mypage" activeItemClass="selected"
                                                                        disabledStyle="color:red;padding: 2px 20px 2px 25px">
                                                        <t:navigationMenuItems id="navitems" value="#{navigationMenu.panelNavigationItems}" />
                                                    </t:panelNavigation2>
                                                </t:div>
                                            </t:div>
                                            
                                           <t:div id="subnavigation_nav">
                                                <f:subview id="navigation_search">
                                                    <c:import url="inc/tabbed_panel_nav_inc.jsp" />
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
                                                            <h:outputText styleClass="header_top" value="#{visit.visitData.searchedTitle}"/> 
                                                            
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
                                                        <td width="3%">&nbsp;</td>
                                                        <td valign="top" width="97%" >
                                                            <f:subview id="body">
                                                                <c:import url="inc/investigations_inc.jsp" />
                                                            </f:subview>
                                                            
                                                            <a4j:form id="helpform">
                                                                
                                                                <t:collapsiblePanel  id="helpform" value="#{helpCollapsibleBean.collapsed}" title="testTitle" >
                                                                    <f:facet name="header">
                                                                        <t:div style="color: black; width:40px;">
                                                                            <h:panelGrid columns="4" >
                                                                                <h:outputText rendered="#{!helpCollapsibleBean.collapsed}" value="v " style="color:blue; "/>  <a4j:commandLink reRender="helpform" style="color: black;" styleClass="help" id="helpcj" action="#{helpCollapsibleBean.collapse}" rendered="#{!helpCollapsibleBean.collapsed}" immediate="true"   value="Help"/>
                                                                                
                                                                                <h:outputText rendered="#{helpCollapsibleBean.collapsed}" value="> " style="color:blue; "/>  <a4j:commandLink reRender="helpform" style="color: black;" styleClass="help" id="helpc" action="#{helpCollapsibleBean.collapse}" rendered="#{helpCollapsibleBean.collapsed}" immediate="true"   value="Help"/>
                                                                                
                                                                            </h:panelGrid>
                                                                        </t:div>
                                                                    </f:facet>
                                                                    
                                                                    <h:panelGrid width="600" style="font: 11px Verdana, Arial, Helvetica, sans-serif; background-color:#CCCCCC; ">
                                                                        <h:outputText value="This lists the investigations found either from your seach or ones associated with you."/>
                                                                        <h:outputText value="The tree gives a quick view, whilst the table holds more information.  Click on the facility name in the tree to view all the investigation's datasets from that facility."/>
                                                                        <h:outputText value="Each of the columns can be sorted by clicking on the header once or twice for reverse sort."/>
                                                                        <h:outputText value="Abstracts can be viewed by clicking the cross in the investigation row or all abstracts can be found by clicking the cross next to the abstract header.  To collapse click again." />
                                                                        <h:outputText value="The image picture of a head, if shown, means that there are users associated with the investigation.  Hover over the image and a list of the users will appear in a pop up box." />
                                                                        <h:outputText value="The key represents the keywords associated with the investigation, hover over it to view them." />
                                                                        <h:outputText value="Click on the check box for a particular investigation or click the cross on the top header to check all of the investigations, then press 'View Selection' to view their datasets." />
                                                                        <h:outputText value="If there are more investigations that are set in your preferences 'Results per page' then a scroller will appear to help you naviagate the investigations. To see more investigations on one page, click Preferences in the bottom left navigation tab bar and then choose the number of results per page in the drop down list." />
                                                                        
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
