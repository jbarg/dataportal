<%--
<%@ page import="org.apache.myfaces.custom.tree.DefaultMutableTreeNode,
                 org.apache.myfaces.custom.tree.model.DefaultTreeModel"%>
<%@ page session="true" contentType="text/html;charset=utf-8"%>
--%>

<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>

<%--
<%@ include file="./header.jsp" %>
--%>

<html>
    <%--
    <%@ include file="../inc/head.inc" %>
    --%>
    <!--
    /*
    * Copyright 2004 The Apache Software Foundation.
    *
    * Licensed under the Apache License, Version 2.0 (the "License");
    * you may not use this file except in compliance with the License.
    * You may obtain a copy of the License at
    *
    *      http://www.apache.org/licenses/LICENSE-2.0
    *
    * Unless required by applicable law or agreed to in writing, software
    * distributed under the License is distributed on an "AS IS" BASIS,
    * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    * See the License for the specific language governing permissions and
    * limitations under the License.
    */
    //-->




    <body>


        <f:view>
            <%--            
            <table width="100%" border="2">
            <tr>
            <td width="100%" height="100%">
            <t:panelTabbedPane bgcolor="#FFFFCC" width="100%">
            <t:panelTab id="home" label="Home" >
            <t:selectOneRadio value="hhhhh">
            <f:selectItem itemValue="0" itemLabel="First Choice" />
            <f:selectItem itemValue="1" itemLabel="Second Choice" />
            </t:selectOneRadio>

            </t:panelTab>
            <t:panelTab id="search" label="Search" >
            </t:panelTab>
            <t:panelTab id="expand" label="Expand" >
            </t:panelTab>
            <t:panelTab id="data" label="Data" >
            </t:panelTab>
            </t:panelTabbedPane>
            </td>
            </tr>
            </table>
            --%>
            
            <%--
            <t:panelTabbedPane bgcolor="#FFFFCC" >
            <t:panelTab id="tab1" label="lab1" >
            </t:panelTab>
            <t:panelTab id="tab2" label="lab2" >
            </t:panelTab>
            <t:panelTab id="tab3" label="lab3" >
            </t:panelTab>
            <t:panelTab id="tab4" label="lab4" >
            </t:panelTab>
            </t:panelTabbedPane>
            --%>    
            
            <%--            
            <table width="100%">
            <tr>
            <td align="left">
            <h:graphicImage value="../images/e-science.JPG" height="50" width="150" />
            </td>
            <td align="right">
            <h:graphicImage value="../images/help.gif" height="30" width="30" />
            </td>
            </tr>
            </table>    
            <hr>
            --%>
 
            <%-- --%>
            <table width="100%" border="0" bgcolor="#FFFFCC">
                <tr>
                    <td width="100%" height="100%">
                        <t:jscookMenu layout="hbr" theme="ThemeIE">
                            <t:navigationMenuItem id="home" itemLabel="Home" action="gohome">
                                <t:navigationMenuItem id="login" itemLabel="Login" action="login"/>
                                <t:navigationMenuItem id="logout" itemLabel="Logout" action="logout"/>
                                <t:navigationMenuItem id="documentation" itemLabel="Documentation" action="documentation"/>
                            </t:navigationMenuItem>
                            <t:navigationMenuItem id="search" itemLabel="Search" action="search">
                                <t:navigationMenuItem id="basic-search" itemLabel="Basic Search" action="basic-search"/>
                                <t:navigationMenuItem id="advanced-search" itemLabel="Advanced Search" action="advanced-search" />
                            </t:navigationMenuItem>
                            <t:navigationMenuItem id="expand" itemLabel="Expand" action="expand"/>
                            <t:navigationMenuItem id="data" itemLabel="Data" action="data"/>
                            <t:navigationMenuItem id="bookmarks" itemLabel="Bookmarks" action="bookmarks"/>
                        </t:jscookMenu> 
                    </td>     
                </tr>
                    
            </table>
   
            <%-- --%>            
            <%--    
            <t:div id="sub1">
            <t:div id="sub2">
            --%>
            <%--
            <t:panelNavigation2 id="nav1" layout="list" itemClass="mypage" activeItemClass="selected" openItemClass="selected" >
            --%>
            <%--
            <t:panelNavigation2 id="nav1" layout="list" >
            <t:commandNavigation2 value="c1" action="a1" >
                            
            <t:commandNavigation2 action="a1" >
            <f:verbatim>&#8250; </f:verbatim>
            <t:outputText value="va1"/>
            </t:commandNavigation2>
            <t:commandNavigation2 action="a1">
            <f:verbatim>&#8250; </f:verbatim>
            <t:outputText value="va2"/>
            </t:commandNavigation2>
          
            </t:commandNavigation2>
        
            </t:panelNavigation2>
            --%>
            <%--
            </t:div>
            </t:div>
            --%>


        </f:view>



    </body>

</html>
    
<%--
<%@ include file="./footer.jsp" %>
--%>    
    


