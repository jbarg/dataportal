
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
        <t:navigationMenuItem id="nav_2"  itemLabel="Results" action="investigations" rendered="#{visit.isInvestigations}" />
        <t:navigationMenuItem id="nav_3"  itemLabel="Data Sets" action="dataset_success" rendered="#{visit.isDatasets}" />
      
        <t:navigationMenuItem id="nav_4"  itemLabel="Data Center" >                  
            <t:navigationMenuItem id="nav_4_1"  itemLabel="Data References" action="addToDataCenter_success" />
            <t:navigationMenuItem id="nav_4_2"  itemLabel="Bookmarks" action="addToBookmark_success" />
                                         
        </t:navigationMenuItem>
    </t:jscookMenu>       
    <input type="hidden" name="jscook_action" />

</h:form> 