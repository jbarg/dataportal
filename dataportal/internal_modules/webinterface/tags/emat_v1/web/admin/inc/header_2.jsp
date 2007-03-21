<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>


<h:form>
 
    <t:jscookMenu layout="hbr" theme="ThemeOffice" styleLocation="../css/jscookmenu">
        <%/* Availaible jscookMenu themes: ThemeIE, ThemeMiniBlack, ThemeOffice, ThemePanel
            Availaible jscookMenu layout: hbr, hbl, hur, hul, vbr, vbl, vur, vul
            respect to Heng Yuan http://www.cs.ucla.edu/~heng/JSCookMenu
             */%>
        
        
              
        <t:navigationMenuItem id="nav_1"  itemLabel="Statistics" itemValue="Statistics" action="GOTO_ADMIN"/>
        <t:navigationMenuItem id="nav_2"  itemLabel="User stats " action="GOTO_VIEW_USER" rendered="#{visit.adminData.searched}" />
        <t:navigationMenuItem id="nav_3"  itemLabel="MyProxy Servers" action="#{adminBean.listMyProxyServers}"  />
      <t:navigationMenuItem id="nav_4"  itemLabel="Facilities" action="#{adminBean.listFacilities}"  />
      
       
       
    </t:jscookMenu>       
    <input type="hidden" name="jscook_action" />

</h:form> 