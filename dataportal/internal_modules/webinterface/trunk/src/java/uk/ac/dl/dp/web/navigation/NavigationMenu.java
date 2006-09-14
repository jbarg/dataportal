package uk.ac.dl.dp.web.navigation;
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
import org.apache.myfaces.custom.navmenu.NavigationMenuItem;
import org.apache.myfaces.custom.navmenu.jscookmenu.HtmlCommandJSCookMenu;
import org.apache.myfaces.custom.navmenu.htmlnavmenu.HtmlCommandNavigationItem;


import org.apache.commons.logging.Log;
import org.apache.commons.logging.LogFactory;

import javax.faces.event.ActionEvent;
import java.util.List;
import java.util.ArrayList;

/**
 * @author Thomas Spiegl (latest modification by $Author: tomsp $)
 * @version $Revision: 419187 $ $Date: 2006-07-05 08:11:08 +0000 (Wed, 05 Jul 2006) $
 */
public class NavigationMenu {
    private static final Log log = LogFactory.getLog(NavigationMenu.class);
    
    public List getPanelNavigationItems() {
        List menu = new ArrayList();
        
        // Main
        NavigationMenuItem main = getMenuNaviagtionItem("Main", null);
        menu.add(main);
        main.add(getMenuNaviagtionItem("Basic Search", "GOTO_BASIC_SEARCH"));
        
        NavigationMenuItem advanced = getMenuNaviagtionItem("Advanced Search", null);
        advanced.setOpen(true);
        main.add(advanced);
        
        NavigationMenuItem nullm = getMenuNaviagtionItem("", null);
        menu.add(nullm);
        
        
        // Main
        NavigationMenuItem data = getMenuNaviagtionItem("Data Center", null);
        menu.add(data);
        data.add(getMenuNaviagtionItem("Data References", "GOTO_DATA_REFERENCES"));
        NavigationMenuItem bookmark = getMenuNaviagtionItem("Bookmarks", "GOTO_BOOKMARKS");
        
        //bookmark.setOpen(true);
        data.add(bookmark);
        /*NavigationMenuItem item = getMenuNaviagtionItem("Help", "#{navigationMenu.getAction2}");
        item.setActive(true);
        item.setOpen(true);
        item.setSplit(true);
        item.setTarget("_blank");*
        products.add(item);*/
         data.add(getMenuNaviagtionItem("Grant Authorisation", "GIVE_DATA_AUTH"));
         data.add(getMenuNaviagtionItem("View Authorisations", "GOTO_DATA_AUTH"));
       
         NavigationMenuItem nullm1 = getMenuNaviagtionItem("", null);
        menu.add(nullm1);
        
        
        //User
        NavigationMenuItem user = getMenuNaviagtionItem("User", null);
        menu.add(user);
        user.add(getMenuNaviagtionItem("User Preferences", "GOTO_USER_PREFS"));
        
        user.add(getMenuNaviagtionItem("History", "GOTO_HISTORY"));
        user.add(getMenuNaviagtionItem("Logout", "#{authorisationBean.logout}"));
        
        NavigationMenuItem nullm2 = getMenuNaviagtionItem("", null);
        menu.add(nullm2);
        
        // Information
        NavigationMenuItem info = getMenuNaviagtionItem("Documentation", null);
        menu.add(info);
        NavigationMenuItem help = getMenuNaviagtionItem("Help", null);
        help.setActive(true);
        //help.setOpen(true);
        help.setSplit(true);
        help.setTarget("_blank");
        info.add(help);
        
        NavigationMenuItem documentation = getMenuNaviagtionItem("Documentation", null);
        documentation.setActive(true);
        //documentation.setOpen(true);
        documentation.setSplit(true);
        documentation.setTarget("_blank");
        info.add(documentation);
        
        NavigationMenuItem dp = getMenuNaviagtionItem("Data Portal", null);
        dp.setActive(true);
        //dp.setOpen(true);
        dp.setSplit(true);
        dp.setTarget("_blank");
        dp.setExternalLink("http://www.bbc.co.uk");
        info.add(dp);
        
        
        return menu;
    }
    
    private static NavigationMenuItem getMenuNaviagtionItem(String label, String action) {
        NavigationMenuItem item = new NavigationMenuItem(label, action);
        item.setActionListener("#{navigationMenu.actionListener}");
        item.setValue(label);
        return item;
    }
    
    public String getAction1() {
        return "go_panelnavigation_1";
    }
    
    public String actionListener(ActionEvent event) {
        if (event.getComponent() instanceof HtmlCommandNavigationItem) {
            log.info("ActionListener: " + ((HtmlCommandNavigationItem) event.getComponent()).getValue());
            return getAction1();
        } else {
            String outcome = (String) ((HtmlCommandJSCookMenu) event.getComponent()).getValue();
            log.info("ActionListener: " + outcome);
            return outcome;
        }
    }    
    
    public boolean getDisabled() {
        return true;
    }
}
