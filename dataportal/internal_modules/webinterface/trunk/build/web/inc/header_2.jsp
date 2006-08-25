
<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>


 <h:form>
     <t:jscookMenu layout="hbr" theme="ThemeOffice" styleLocation="css/jscookmenu">
         <%/* Availaible jscookMenu themes: ThemeIE, ThemeMiniBlack, ThemeOffice, ThemePanel
             Availaible jscookMenu layout: hbr, hbl, hur, hul, vbr, vbl, vur, vul
             respect to Heng Yuan http://www.cs.ucla.edu/~heng/JSCookMenu
              */%>
         
        
              
         <t:navigationMenuItem id="nav_1"  itemLabel="Basic Search" itemValue="basicSearch" action="basicSearch"/>
         <t:navigationMenuItem id="nav_2"  itemLabel="Investigations" action="investigations" disabled="#{visit.investigations}"/>
         <t:navigationMenuItem id="nav_3"  itemLabel="User data" >                  
             <t:navigationMenuItem id="nav_3_1"  itemLabel="Data Center" action="go_sample2" />
             <t:navigationMenuItem id="nav_3_2"  itemLabel="Bookmarks" action="go_sample2" />
                                         
         </t:navigationMenuItem>
         <t:navigationMenuItem id="nav_4"  itemLabel="Transfer" action="http://myfaces.apache.org" />
     </t:jscookMenu>              
 </h:form>       