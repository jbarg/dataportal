
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%>
<%-- style="width:947px"   works but makes it static on div --%>

<c:choose>
    <c:when test="${visit.width == 954}">
        <%-- <t:div styleClass="outerbar" style="width:947px" >
           <s:fishEyeNavigationMenu  itemWidth="50" itemHeight="53" itemMaxWidth="200"
                                      itemMaxHeight="200" orientation="horizontal" effectUnits="2"
                                      itemPadding="10" attachEdge="top" labelEdge="bottom">                    
                
                <t:navigationMenuItem  icon="../../images/icon_browser.png"
                                       itemLabel="Web Browser"                            />
                <t:navigationMenuItem icon="../../images/icon_calendar.png"
                                      itemLabel="Calendar"  />
                <t:navigationMenuItem icon="../../images/icon_email.png" itemLabel="Email"                          />
                <t:navigationMenuItem icon="../../images/icon_texteditor.png" itemLabel="Force Test Fatal"      action="#{adminBean.forceTestFatal}"                       />
                <t:navigationMenuItem icon="../../images/icon_update.png"   itemLabel="Force Test Error"     action="#{adminBean.forceTestError}"                     />
                <t:navigationMenuItem icon="../../images/icon_users.png" action="GOTO_ADMIN" rendered="#{visit.admin}" itemLabel="Admin"                            />
                
            </s:fishEyeNavigationMenu>      
            
                             
            
        </t:div>     --%>
        <img  src="../../images/stfc_954px.jpg"  alt="Sci-Tech e-Science" border="0"  />
        
    </c:when>
    <c:when test="${visit.width == 1210}">
       <%--  <t:div styleClass="outerbar" style="width:1210px" >
            <s:fishEyeNavigationMenu  itemWidth="50" itemHeight="53" itemMaxWidth="200"
                                      itemMaxHeight="200" orientation="horizontal" effectUnits="2"
                                      itemPadding="10" attachEdge="top" labelEdge="bottom">                    
                
                <t:navigationMenuItem icon="../../images/icon_browser.png"
                                      itemLabel="Web Browser"                            />
                <t:navigationMenuItem icon="../../images/icon_calendar.png"
                                      itemLabel="Calendar"  />
                <t:navigationMenuItem icon="../../images/icon_email.png" itemLabel="Email"                          />
                <t:navigationMenuItem icon="../../images/icon_texteditor.png" itemLabel="Text Editor"                            />
                <t:navigationMenuItem icon="../../images/icon_update.png"   itemLabel="Software Update"                          />
                <t:navigationMenuItem icon="../../images/icon_users.png" action="GOTO_ADMIN" rendered="#{visit.admin}" itemLabel="Admin"                            />
                
        </s:fishEyeNavigationMenu>                    </t:div>--%>
           <img  src="../../images/stfc_1210px.jpg"  alt="Sci-Tech e-Science" border="0"  />
    </c:when>
    
    <c:when test="${visit.width == 1530}">
        <%-- <t:div styleClass="outerbar" style="width:1530px" >
            <s:fishEyeNavigationMenu  itemWidth="50" itemHeight="53" itemMaxWidth="200"
                                      itemMaxHeight="200" orientation="horizontal" effectUnits="2"
                                      itemPadding="10" attachEdge="top" labelEdge="bottom">                    
                
                <t:navigationMenuItem icon="../../images/icon_browser.png"
                                      itemLabel="Web Browser"                            />
                <t:navigationMenuItem icon="../../images/icon_calendar.png"
                                      itemLabel="Calendar"  />
                <t:navigationMenuItem icon="../../images/icon_email.png" itemLabel="Email"                          />
                <t:navigationMenuItem icon="../../images/icon_texteditor.png" itemLabel="Text Editor"                            />
                <t:navigationMenuItem icon="../../images/icon_update.png"   itemLabel="Software Update"                          />
                <t:navigationMenuItem icon="../../images/icon_users.png" action="GOTO_ADMIN" rendered="#{visit.admin}" itemLabel="Admin"                            />
                
        </s:fishEyeNavigationMenu>                    </t:div>--%>
           <img  src="../../images/stfc_1600px.jpg"  alt="Sci-Tech e-Science" border="0"  />
    </c:when>
</c:choose>     
