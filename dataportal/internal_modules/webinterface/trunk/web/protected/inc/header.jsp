<%@ taglib uri="http://myfaces.apache.org/sandbox" prefix="s"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsp/jstl/core" prefix="c"%>
<%-- style="width:947px"   works but makes it static on div --%>

<c:choose>
    <c:when test="${visit.width == 954}">
        <t:div styleClass="outerbar" style="width:947px" >
        <s:fishEyeNavigationMenu  itemWidth="50" itemHeight="50" itemMaxWidth="200"
            itemMaxHeight="200" orientation="horizontal" effectUnits="2"
            itemPadding="10" attachEdge="top" labelEdge="bottom">                    

            <t:navigationMenuItem icon="../../images/icon_browser.png"
            itemLabel="Web Browser"                            />
            <t:navigationMenuItem icon="../../images/icon_calendar.png"
            itemLabel="Calendar"  />
            <t:navigationMenuItem icon="../../images/icon_email.png" itemLabel="Email"                          />
            <t:navigationMenuItem icon="../../images/icon_texteditor.png" itemLabel="Text Editor"                            />
            <t:navigationMenuItem icon="../../images/icon_update.png"   itemLabel="Software Update"                          />
            <t:navigationMenuItem icon="../../images/icon_users.png" itemLabel="Users"                            />

        </s:fishEyeNavigationMenu>                    </t:div>
            
    </c:when>
    <c:when test="${visit.width == 1210}">
        <t:div styleClass="outerbar" style="width:1210px" >
        <s:fishEyeNavigationMenu  itemWidth="50" itemHeight="50" itemMaxWidth="200"
            itemMaxHeight="200" orientation="horizontal" effectUnits="2"
            itemPadding="10" attachEdge="top" labelEdge="bottom">                    

            <t:navigationMenuItem icon="../../images/icon_browser.png"
            itemLabel="Web Browser"                            />
            <t:navigationMenuItem icon="../../images/icon_calendar.png"
            itemLabel="Calendar"  />
            <t:navigationMenuItem icon="../../images/icon_email.png" itemLabel="Email"                          />
            <t:navigationMenuItem icon="../../images/icon_texteditor.png" itemLabel="Text Editor"                            />
            <t:navigationMenuItem icon="../../images/icon_update.png"   itemLabel="Software Update"                          />
            <t:navigationMenuItem icon="../../images/icon_users.png" itemLabel="Users"                            />

        </s:fishEyeNavigationMenu>                    </t:div>
            
    </c:when>
         
    <c:when test="${visit.width == 1530}">
        <t:div styleClass="outerbar" style="width:1530px" >
        <s:fishEyeNavigationMenu  itemWidth="50" itemHeight="50" itemMaxWidth="200"
            itemMaxHeight="200" orientation="horizontal" effectUnits="2"
            itemPadding="10" attachEdge="top" labelEdge="bottom">                    

            <t:navigationMenuItem icon="../../images/icon_browser.png"
            itemLabel="Web Browser"                            />
            <t:navigationMenuItem icon="../../images/icon_calendar.png"
            itemLabel="Calendar"  />
            <t:navigationMenuItem icon="../../images/icon_email.png" itemLabel="Email"                          />
            <t:navigationMenuItem icon="../../images/icon_texteditor.png" itemLabel="Text Editor"                            />
            <t:navigationMenuItem icon="../../images/icon_update.png"   itemLabel="Software Update"                          />
            <t:navigationMenuItem icon="../../images/icon_users.png" itemLabel="Users"                            />

        </s:fishEyeNavigationMenu>                    </t:div>
            
    </c:when>
</c:choose>     
            