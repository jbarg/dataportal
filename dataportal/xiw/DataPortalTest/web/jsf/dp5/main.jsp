<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>

<%@taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%> 

<html>
    <%--
    <%@ include file="../inc/head.inc" %>
    --%>
 

    <body>

        <f:view>
  
            <%-- --%>
            <table width="100%" border="0">
                <tr>
                    <td width="100%" height="100%">
                        <t:panelTabbedPane bgcolor="#FFFFF1" width="100%">
                            <t:panelTab id="home" label="Home" >
                                <f:verbatim>
                                    
                                    
                                    <table border="0">
                                        <tr>
                                            <td >                                        
                                                <t:panelNavigation id="my-home">
                                                    <t:commandNavigation value="Login" action="go_login"/>
                                                    <t:commandNavigation value="Logout"/>
                                                    <t:commandNavigation value="Help"/>
                                                    <t:commandNavigation value="Documentation"/>
                                                </t:panelNavigation>
                                            </td>
                                            <td width="100%">
                                            
                                            </td>
                                        </tr>
                                    </table>
                                </f:verbatim>
                                <%--                                                               
                                <t:selectOneRadio value="hhhhh">
                                <f:selectItem itemValue="0" itemLabel="First Choice" />
                                <f:selectItem itemValue="1" itemLabel="Second Choice" />
                                </t:selectOneRadio>
                                --%>                                
                                <                                                      
                                <%--
                                <%@ include file="./login/login.jsp" %>
                                --%>
                            </t:panelTab>
                            <t:panelTab id="search" label="Search" >
                                <%--                              <f:verbatim>    --%>
  
                                <%--
                                <table border="0">
                                <tr>
                                <td>                                        
                                <t:panelNavigation id="my-search">
                                <t:commandNavigation value="Basic Search" />
                                <t:commandNavigation value="Advanced Search"/>
                                </t:panelNavigation>
                                </td>
                                <td width="100%">
                                </td>
                                </tr>
                                </table>
                                --%>
                                    
                                <h:panelGrid columns="3">
                                    <%--                                        
                                    <frameset cols ="50%, *" border="0" frameborder="0">
                                    <frame noresize="true" src="./search/basic.jsp" scrolling="no" framespacing="0" >
   
                                    <frame noresize="true" src="./search/basic.jsp" scrolling="no" framespacing="0" name="searchfram" >
                                    </frameset>
                                    --%>


                                    <f:subview id="menu">
                                            
                                        <h:form>
                                            <h:panelGrid columns="1" border="0">
    
                                                <h:commandLink value="basic search" action="./search/basic.jsp"/>
                                                <h:commandLink value="advanced search" action="goFacesConfigNavRule2"/>
 
                     
                                            </h:panelGrid>
                                        </h:form>
                                    </f:subview>
                
                                    <h:panelGroup style="width: 50px;"/>
                
                                    <f:subview id="content">
                                        <c:import url="./search/basic.jsp"/>
                                    </f:subview>
                
                                    
                                </h:panelGrid>
                                <%--                                    
                                </f:verbatim>                                
                                --%>
                            </t:panelTab>
                            <t:panelTab id="expand" label="Expand" >
                            </t:panelTab>
                            <t:panelTab id="data" label="Data" >
                            </t:panelTab>
                        </t:panelTabbedPane>
                    </td>
                </tr>
            </table>
            <%-- --%>
      
            
            <%-- 
            <t:jscookMenu layout="hbr" theme="ThemeOffice">
            <t:navigationMenuItem id="home" itemLabel="Home" action="gohome">
            <t:navigationMenuItem id="login" itemLabel="Login" action="login"/>
            <t:navigationMenuItem id="logout" itemLabel="Logout" action="logout"/>
            </t:navigationMenuItem>
            <t:navigationMenuItem id="search" itemLabel="Search" action="search">
            <t:navigationMenuItem id="basic-search" itemLabel="Basic Search" action="basic-search"/>
            <t:navigationMenuItem id="advanced-search" itemLabel="Advanced Search" action="advanced-search" />
            </t:navigationMenuItem>
            <t:navigationMenuItem id="expand" itemLabel="Expand" action="expand"/>
            <t:navigationMenuItem id="data" itemLabel="Data" action="data"/>
            <t:navigationMenuItem id="bookmarks" itemLabel="Bookmarks" action="bookmarks"/>
            </t:jscookMenu>
            --%>  
 
            <%-- 
            <t:jscookMenu layout="hbr" theme="ThemeOffice">
            <t:navigationMenuItem id="home" itemLabel="Home" action="gohome">
            <t:navigationMenuItem id="login" itemLabel="Login" action="login"/>
            <t:navigationMenuItem id="logout" itemLabel="Logout" action="logout"/>
            </t:navigationMenuItem>
            <t:navigationMenuItem id="search" itemLabel="Search" action="search">
            <t:navigationMenuItem id="basic-search" itemLabel="Basic Search" action="basic-search"/>
            <t:navigationMenuItem id="advanced-search" itemLabel="Advanced Search" action="advanced-search" />
            </t:navigationMenuItem>
            <t:navigationMenuItem id="expand" itemLabel="Expand" action="expand"/>
            <t:navigationMenuItem id="data" itemLabel="Data" action="data"/>
            <t:navigationMenuItem id="bookmarks" itemLabel="Bookmarks" action="bookmarks"/>
            </t:jscookMenu>
            --%>            
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

 


