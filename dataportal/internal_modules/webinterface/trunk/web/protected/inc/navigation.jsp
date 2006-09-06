<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>
<%@ taglib prefix="tiles" uri="http://struts.apache.org/tags-tiles"%>

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
