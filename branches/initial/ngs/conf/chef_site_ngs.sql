-----------------------------------------------------------------------------
-- CHEF_SITE
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS CHEF_SITE;

CREATE TABLE CHEF_SITE
(
    SITE_ID VARCHAR (255) NOT NULL,
    XML TEXT
);

CREATE UNIQUE INDEX CHEF_SITE_INDEX ON CHEF_SITE
(
    SITE_ID
);

INSERT INTO CHEF_SITE VALUES ('~anon', '<?xml version="1.0" encoding="UTF-8"?>
<site description-enc="VGhlIEdhdGV3YXkgU2l0ZQ==" id="~anon"
    joinable="false" status="2" title="Gateway">
    <properties>
        <property enc="BASE64" name="CHEF:modifiedby" value="YWRtaW4="/>
        <property enc="BASE64" name="DAV:getlastmodified" value="MjAwMzExMjYwMzQ1MjIwNjE="/>
    </properties>
        <pages>
            <page id="~anon-100" layout="0" title="Welcome">
                <properties/>
                <tools>
                    <tool id="~anon-110" title="Welcome!" toolId="chef.iframe">
                        <properties>
                            <property enc="BASE64" name="special" value="c2l0ZQ=="/>
                        </properties>
                    </tool>
                </tools>
            </page>
            <page id="~anon-200" layout="0" title="About">
                <properties/>
                <tools>
                    <tool id="~anon-210" title="About" toolId="chef.iframe">
                        <properties>
                            <property enc="BASE64" name="height" value="NTAwcHg="/>
                            <property enc="BASE64" name="source" value="http://www.ngs.ac.uk"/>
                        </properties>
                    </tool>
                </tools>
            </page>
        </pages>
    </site>
');

INSERT INTO CHEF_SITE VALUES ('~', '<?xml version="1.0" encoding="UTF-8"?>
<site id="~" title="My Workspace" description="MyWorkspace Site" joinable="false" status="2">
    <properties />
        <pages>
            <page id="~-100" layout="0" title="Home">
                <properties/>
                <tools>
                    <tool id="~-110" title="My Workspace Information" toolId="chef.iframe">
                        <properties>
                            <property enc="BASE64" name="special" value=""/>
                        </properties>
                    </tool>
                </tools>
            </page>
            <page id="~-200" layout="0" title="Membership">
                <properties/>
                <tools>
                    <tool id="~-210" title="Membership" toolId="chef.membership">
                        <properties/>
                    </tool>
                </tools>
            </page>
            <page id="~-300" layout="0" title="Schedule">
                <properties/>
                <tools>
                    <tool id="~-310" title="Schedule" toolId="chef.schedule">
                        <properties/>
                    </tool>
                </tools>
            </page>
            <page id="~-400" layout="0" title="Resources">
                <properties/>
                <tools>
                    <tool id="~-410" title="Resources" toolId="chef.resources">
                        <properties/>
                    </tool>
                </tools>
            </page>
            <page id="~-500" layout="0" title="Announcements">
                <properties/>
                <tools>
                    <tool id="~-510" title="Announcements" toolId="chef.announcements">
                        <properties/>
                    </tool>
                </tools>
            </page>
            <page id="~-600" layout="0" title="Chat">
                <properties/>
                <tools>
                    <tool id="~-610" title="Chat" toolId="chef.chat">
                        <properties/>
                    </tool>
                </tools>
            </page>
            <page id="~-700" layout="0" title="Account">
                <properties/>
                <tools>
                    <tool id="~-710" title="Account" toolId="chef.singleuser">
                        <properties>
                            <property enc="BASE64"
                                name="include-password" value="dHJ1ZQ=="/>
                        </properties>
                    </tool>
                </tools>
            </page>
            <page id="~-800" layout="0" title="Worksite Setup">
                <properties/>
                <tools>
                    <tool id="~-810" title="Worksite Setup" toolId="chef.sitesetup">
                        <properties/>
                    </tool>
                </tools>
            </page>
            <page id="~-900" layout="0" title="Proxy Manager">
                <properties/>
                <tools>
                    <tool id="~-901" title="Proxy Manager" toolId="xportlets:ProxyManager">
                       <properties/>
                    </tool>
                </tools>
            </page>
            <page id="~-910" layout="0" title="Job Launcher">
                <properties/>
                <tools>
                    <tool id="~-911" title="Job Launcher" toolId="xportlets:GramJobLauncher">
                        <properties/>
                    </tool>
                </tools>
            </page>
            <page id="~-920" layout="0" title="GridFTP">
                <properties/>
                <tools>
                    <tool id="~-921" title="GridFTP" toolId="xportlets:GridFTPClient">
                        <properties/>
                    </tool>
                </tools>
            </page>
            <page id="~-930" layout="0" title="Ldap Browser">
                <properties/>
                <tools>
                    <tool id="~-931" title="Ldap Browser" toolId="xportlets:LdapBrowser">
                        <properties/>
                    </tool>
                </tools>
            </page>
        </pages>
    </site>
');

INSERT INTO CHEF_SITE VALUES ('~admin', '<?xml version="1.0" encoding="UTF-8"?>
<site id="~admin" title="Administration Workspace" description="Administration Workspace" joinable="false" status="2">
    <properties />
    <pages>
        <page id="100" title="Home" layout="0">
            <properties />
            <tools>
                <tool id="110" toolId="chef.motd" title="Message of the Day">
                    <properties />
                </tool>
                <tool id="120" toolId="chef.iframe" title="My Workspace Information">
                    <properties>
                        <property name="special" value="workspace"/>
                    </properties>
                </tool>
            </tools>
        </page>
        <page id="200" title="Users" layout="0">
            <properties />
            <tools>
                <tool id="210" toolId="chef.users" title="Users">
                    <properties />
                </tool>
            </tools>
        </page>
        <page id="250" title="Aliases" layout="0">
            <properties />
            <tools>
                <tool id="260" toolId="chef.aliases" title="Aliases">
                    <properties />
                </tool>
            </tools>
        </page>
        <page id="300" title="Sites" layout="0">
            <properties />
            <tools>
                <tool id="310" toolId="chef.sites" title="Sites">
                    <properties />
                </tool>
            </tools>
        </page>
        <page id="350" title="Realms" layout="0">
            <properties />
            <tools>
                <tool id="355" toolId="chef.realms" title="Realms">
                    <properties />
                </tool>
            </tools>
        </page>
        <page id="360" title="Worksite Setup" layout="0">
            <properties />
            <tools>
                <tool id="365" toolId="chef.sitesetup" title="Worksite Setup">
                    <properties />
                </tool>
            </tools>
        </page>
        <page id="400" title="MOTD" layout="0">
            <properties />
            <tools>
                <tool id="410" toolId="chef.announcements" title="MOTD">
                    <properties>
                        <property name="channel" value="/announcement/channel/!site/motd"/>
                    </properties>
                </tool>
            </tools>
        </page>
        <page id="500" title="Resources" layout="0">
            <properties />
            <tools>
                <tool id="510" toolId="chef.resources" title="Resources">
                    <properties>
                        <property name="home" value="/"/>
                    </properties>
                </tool>
            </tools>
        </page>
        <page id="600" title="On-Line" layout="0">
            <properties />
            <tools>
                <tool id="610" toolId="chef.presence" title="On-Line">
                    <properties />
                </tool>
            </tools>
        </page>
        <page id="700" title="Memory" layout="0">
            <properties />
            <tools>
                <tool id="710" toolId="chef.memory" title="Memory">
                    <properties />
                </tool>
            </tools>
        </page>
        <page id="800" title="Email Archive" layout="0">
            <properties />
            <tools>
                <tool id="810" toolId="chef.mailbox" title="Email Archive">
                    <properties>
                        <property name="channel" value="/mailarchive/channel/!site/postmaster"/>
                    </properties>
                </tool>
            </tools>
        </page>
    </pages>
</site>
');

INSERT INTO CHEF_SITE VALUES ('!worksite', '<?xml version="1.0" encoding="UTF-8"?>
<site icon="/content/public/pig.gif" id="!worksite"
info="/content/public/site_home.html" joinable="false"
joiner-role="access" status="1" title="worksite">
<properties>
<property name="CHEF:creator" value="admin"/>
<property name="CHEF:modifiedby" value="admin"/>
<property name="DAV:getlastmodified" value="20030624121053597"/>
<property name="DAV:creationdate" value="20030624041508851"/>
</properties>
<pages>
<page id="100" layout="1" title="Home">
<properties/>
<tools>
<tool id="120" title="My Workspace Information" toolId="chef.iframe" layoutHints="0,0">
<properties>
<property name="height" value="100px"/>
<property name="special" value="workspace"/>
<property name="source" value=""/>
</properties>
</tool>
<tool id="105645501684435" title="Recent Announcements" toolId="chef.synoptic.announcement" layoutHints="0,1">
<properties />
</tool>
<tool id="105645503746936"
title="Recent Discussion Items" toolId="chef.synoptic.discussion" layoutHints="1,1">
<properties />
</tool>
<tool id="105645504631337" title="Recent Chat Messages" toolId="chef.synoptic.chat" layoutHints="2,1">
<properties />
</tool>
</tools>
</page>
<page id="300" layout="0" title="Schedule">
<properties/>
<tools>
<tool id="310" title="Schedule" toolId="chef.schedule">
<properties/>
</tool>
</tools>
</page>
<page id="10564282570317" layout="0" title="Announcements">
<properties/>
<tools>
<tool id="10564282644238" title="Announcements" toolId="chef.announcements">
<properties/>
</tool>
</tools>
</page>
<page id="400" layout="0" title="Resources">
<properties/>
<tools>
<tool id="410" title="Resources" toolId="chef.resources">
<properties/>
</tool>
</tools>
</page>
<page id="10564282094095" layout="0" title="Discussion">
<properties/>
<tools>
<tool id="10564282165356" title="Discussion" toolId="chef.discussion">
<properties>
<property name="category" value="false"/>
</properties>
</tool>
</tools>
</page>
<page id="105642833769811" layout="0" title="Assignments">
<properties/>
<tools>
<tool id="105642834551512" title="Assignments" toolId="chef.assignment">
<properties/>
</tool>
</tools>
</page>
<page id="10564282955649" layout="0" title="Drop Box">
<properties/>
<tools>
<tool id="105642830296610" title="Drop Box" toolId="chef.dropbox">
<properties>
<property name="resources_mode" value="dropbox"/>
</properties>
</tool>
</tools>
</page>
<page id="200" layout="0" title="Chat">
<properties/>
<tools>
<tool id="210" title="Chat" toolId="chef.chat">
<properties>
<property name="display-date" value="true"/>
<property name="filter-param" value="3"/>
<property name="display-time" value="true"/>
<property name="sound-alert" value="true"/>
<property name="filter-type" value="SelectMessagesByTime"/>
<property name="display-user" value="true"/>
</properties>
</tool>
</tools>
</page>
<page id="105642839703913" layout="0" title="Email Archive">
<properties/>
<tools>
<tool id="105642841315114" title="Email Archive" toolId="chef.mailbox">
<properties/>
</tool>
</tools>
</page>
</pages>
</site>
');
