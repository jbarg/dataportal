<%@ taglib uri="http://java.sun.com/jsf/core" prefix="f"%>
<%@ taglib uri="http://myfaces.apache.org/tomahawk" prefix="t"%>
<%@ taglib uri="http://java.sun.com/jsf/html" prefix="h"%>


<h:form>

<t:jscookMenu layout="hbr" theme="ThemeOffice" styleLocation="../css/jscookmenu">
<%/* Availaible jscookMenu themes: ThemeIE, ThemeMiniBlack, ThemeOffice, ThemePanel
Availaible jscookMenu layout: hbr, hbl, hur, hul, vbr, vbl, vur, vul
respect to Heng Yuan http://www.cs.ucla.edu/~heng/JSCookMenu
*/%>
<t:navigationMenuItem id="nav_admin"  itemLabel="Admin" itemValue="gotAdmin" action="GOTO_ADMIN"  rendered="#{visit.admin}"  />    


<t:navigationMenuItem id="nav_0"  itemLabel="Search" itemValue="basicSearch" action="GOTO_BASIC_SEARCH">    
<t:navigationMenuItem id="nav_0_1"  itemLabel="Basic Search" itemValue="basicSearch" action="GOTO_BASIC_SEARCH" />
<t:navigationMenuItem id="nav_0_2"  itemLabel="My Data Search" itemValue="myDataSearch"  action="#{searchBean.searchOwnDataAll}" />
</t:navigationMenuItem>  

<t:navigationMenuItem id="nav_2"  itemLabel="Results" action="GOTO_INVESTIGATIONS" rendered="#{visit.visitData.investigations}" />
<t:navigationMenuItem id="nav_3"  itemLabel="Data Sets" action="GOTO_DATASETS" rendered="#{visit.visitData.datasets}" />

<t:navigationMenuItem id="nav_4"  itemLabel="Data Links" >                  
<t:navigationMenuItem id="nav_4_1"  itemLabel="Data References" action="GOTO_DATA_REFERENCES" />
<t:navigationMenuItem id="nav_4_2"  itemLabel="Bookmarks" action="GOTO_BOOKMARKS" />
<t:navigationMenuItem id="nav_4_3" rendered="#{visit.visitData.otherUserDn}"  itemLabel="Others" >                  
<t:navigationMenuItem id="nav_4_3_1" rendered="#{visit.visitData.datacenterEnabled}"  itemLabel="Data References" action="VIEW_OTHER_DATA_REFERENCES" />
<t:navigationMenuItem id="nav_4_3_2" rendered="#{visit.visitData.bookmarkEnabled}"  itemLabel="Bookmarks" action="VIEW_OTHER_BOOKMARKS" />

</t:navigationMenuItem>             
</t:navigationMenuItem>
<t:navigationMenuItem id="nav_5"  itemLabel="Log out" action="#{authorisationBean.logout}"  />

</t:jscookMenu>       
<input type="hidden" name="jscook_action" />

</h:form> 