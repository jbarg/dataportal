-----------------------------------------------------------------------------
-- CHEF_ALIAS
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS CHEF_ALIAS;

CREATE TABLE CHEF_ALIAS
(
    ALIAS_ID VARCHAR (255) NOT NULL,
    XML TEXT
);

CREATE UNIQUE INDEX CHEF_ALIAS_INDEX ON CHEF_ALIAS
(
	ALIAS_ID
);
-----------------------------------------------------------------------------
-- ANNOUNCEMENT_CHANNEL
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS ANNOUNCEMENT_CHANNEL;

CREATE TABLE ANNOUNCEMENT_CHANNEL
(
    CHANNEL_ID VARCHAR (99) NOT NULL,
	NEXT_ID INT,
    XML TEXT
);

CREATE UNIQUE INDEX ANNOUNCEMENT_CHANNEL_INDEX ON ANNOUNCEMENT_CHANNEL
(
	CHANNEL_ID
)
;

-----------------------------------------------------------------------------
-- ANNOUNCEMENT_MESSABE
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS ANNOUNCEMENT_MESSAGE;

CREATE TABLE ANNOUNCEMENT_MESSAGE
(
    CHANNEL_ID VARCHAR (99) NOT NULL,
	MESSAGE_ID VARCHAR (32) NOT NULL,
	MESSAGE_DATE TIMESTAMP NOT NULL,
    XML TEXT
);

CREATE INDEX ANNOUNCEMENT_MESSAGE_INDEX ON ANNOUNCEMENT_MESSAGE
(
	CHANNEL_ID
)
;

-----------------------------------------------------------------------------
-- ASSIGNMENT_ASSIGNMENT
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS ASSIGNMENT_ASSIGNMENT;

CREATE TABLE ASSIGNMENT_ASSIGNMENT
(
    ASSIGNMENT_ID VARCHAR (99) NOT NULL,
    XML TEXT
);

CREATE UNIQUE INDEX ASSIGNMENT_ASSIGNMENT_INDEX ON ASSIGNMENT_ASSIGNMENT
(
	ASSIGNMENT_ID
)
;

-----------------------------------------------------------------------------
-- ASSIGNMENT_CONTENT
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS ASSIGNMENT_CONTENT;

CREATE TABLE ASSIGNMENT_CONTENT
(
    CONTENT_ID VARCHAR (99) NOT NULL,
    XML TEXT
);

CREATE UNIQUE INDEX ASSIGNMENT_CONTENT_INDEX ON ASSIGNMENT_CONTENT
(
	CONTENT_ID
)
;

-----------------------------------------------------------------------------
-- ASSIGNMENT_SUBMISSION
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS ASSIGNMENT_SUBMISSION;

CREATE TABLE ASSIGNMENT_SUBMISSION
(
    SUBMISSION_ID VARCHAR (99) NOT NULL,
    XML TEXT
);

CREATE UNIQUE INDEX ASSIGNMENT_SUBMISSION_INDEX ON ASSIGNMENT_SUBMISSION
(
	SUBMISSION_ID
)
;
-----------------------------------------------------------------------------
-- CALENDAR_CALENDAR
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS CALENDAR_CALENDAR;

CREATE TABLE CALENDAR_CALENDAR
(
    CALENDAR_ID VARCHAR (99) NOT NULL,
	NEXT_ID INT,
    XML TEXT
);

CREATE UNIQUE INDEX CALENDAR_CALENDAR_INDEX ON CALENDAR_CALENDAR
(
	CALENDAR_ID
)
;

-----------------------------------------------------------------------------
-- CALENDAR_EVENT
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS CALENDAR_EVENT;

CREATE TABLE CALENDAR_EVENT
(
    CALENDAR_ID VARCHAR (99) NOT NULL,
	EVENT_ID VARCHAR (32) NOT NULL,
	EVENT_START TIMESTAMP NOT NULL,
	EVENT_END TIMESTAMP NOT NULL,
    XML TEXT
);

CREATE INDEX CALENDAR_EVENT_INDEX ON CALENDAR_EVENT
(
	CALENDAR_ID
)
;
-----------------------------------------------------------------------------
-- CHAT_CHANNEL
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS CHAT_CHANNEL;

CREATE TABLE CHAT_CHANNEL
(
    CHANNEL_ID VARCHAR (99) NOT NULL,
	NEXT_ID INT,
    XML TEXT
);

CREATE UNIQUE INDEX CHAT_CHANNEL_INDEX ON CHAT_CHANNEL
(
	CHANNEL_ID
)
;

-----------------------------------------------------------------------------
-- CHAT_MESSABE
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS CHAT_MESSAGE;

CREATE TABLE CHAT_MESSAGE
(
    CHANNEL_ID VARCHAR (99) NOT NULL,
	MESSAGE_ID VARCHAR (32) NOT NULL,
	MESSAGE_DATE TIMESTAMP NOT NULL,
    XML TEXT
);

CREATE INDEX CHAT_MESSAGE_INDEX ON CHAT_MESSAGE
(
	CHANNEL_ID
)
;

-----------------------------------------------------------------------------
-- CHEF_CLUSTER
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS CHEF_CLUSTER;

CREATE TABLE CHEF_CLUSTER
(
	SERVER_ID VARCHAR (64),
	START_TIME TIMESTAMP,
	END_TIME TIMESTAMP,
	UPDATE_TIME TIMESTAMP
);

CREATE UNIQUE INDEX CHEF_CLUSTER_INDEX ON CHEF_CLUSTER
(
	SERVER_ID
);
-----------------------------------------------------------------------------
-- CONTENT_COLLECTION
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS CONTENT_COLLECTION;

CREATE TABLE CONTENT_COLLECTION
(
    COLLECTION_ID VARCHAR (255) NOT NULL,
	IN_COLLECTION VARCHAR (255) NOT NULL,
    XML TEXT
);

CREATE UNIQUE INDEX CONTENT_COLLECTION_INDEX ON CONTENT_COLLECTION
(
	COLLECTION_ID
);

CREATE INDEX CONTENT_IN_COLLECTION_INDEX ON CONTENT_COLLECTION
(
	IN_COLLECTION
);

INSERT INTO CONTENT_COLLECTION VALUES ('/',
'',
'<?xml version="1.0" encoding="UTF-8"?>
<collection id="/">
	<properties>
		<property name="CHEF:creator" value="admin"/>
		<property name="CHEF:is-collection" value="true"/>
		<property name="DAV:displayname" value="root"/>
		<property name="CHEF:modifiedby" value="admin"/>
		<property name="DAV:getlastmodified" value="20020401000000000"/>
		<property name="DAV:creationdate" value="20020401000000000"/>
	</properties>
</collection>
');

INSERT INTO CONTENT_COLLECTION VALUES ('/group/',
'/',
'<?xml version="1.0" encoding="UTF-8"?>
<collection id="/group/">
	<properties>
		<property name="CHEF:creator" value="admin"/>
		<property name="CHEF:is-collection" value="true"/>
		<property name="DAV:displayname" value="group"/>
		<property name="CHEF:modifiedby" value="admin"/>
		<property name="DAV:getlastmodified" value="20020401000000000"/>
		<property name="DAV:creationdate" value="20020401000000000"/>
	</properties>
</collection>
');


INSERT INTO CONTENT_COLLECTION VALUES ('/public/',
'/',
'<?xml version="1.0" encoding="UTF-8"?>
<collection id="/public/">
	<properties>
		<property name="CHEF:creator" value="admin"/>
		<property name="CHEF:is-collection" value="true"/>
		<property name="DAV:displayname" value="public"/>
		<property name="CHEF:modifiedby" value="admin"/>
		<property name="DAV:getlastmodified" value="20020401000000000"/>
		<property name="DAV:creationdate" value="20020401000000000"/>
	</properties>
</collection>
');

INSERT INTO CONTENT_COLLECTION VALUES ('/attachment/',
'/',
'<?xml version="1.0" encoding="UTF-8"?>
<collection id="/attachment/">
	<properties>
		<property name="CHEF:creator" value="admin"/>
		<property name="CHEF:is-collection" value="true"/>
		<property name="DAV:displayname" value="attachment"/>
		<property name="CHEF:modifiedby" value="admin"/>
		<property name="DAV:getlastmodified" value="20020401000000000"/>
		<property name="DAV:creationdate" value="20020401000000000"/>
	</properties>
</collection>
');

INSERT INTO CONTENT_COLLECTION VALUES ('/user/',
'/',
'<?xml version="1.0" encoding="UTF-8"?>
<collection id="/user/">
	<properties>
		<property name="CHEF:creator" value="admin"/>
		<property name="CHEF:is-collection" value="true"/>
		<property name="DAV:displayname" value="user"/>
		<property name="CHEF:modifiedby" value="admin"/>
		<property name="DAV:getlastmodified" value="20020401000000000"/>
		<property name="DAV:creationdate" value="20020401000000000"/>
	</properties>
</collection>
');

INSERT INTO CONTENT_COLLECTION VALUES ('/group-user/',
'/',
'<?xml version="1.0" encoding="UTF-8"?>
	<collection id="/group-user/">
		<properties>
			<property name="CHEF:creator" value="admin"/>
			<property name="CHEF:is-collection" value="true"/>
			<property name="DAV:displayname" value="group-user"/>
			<property name="CHEF:modifiedby" value="admin"/>
			<property name="DAV:getlastmodified" value="20020401000000000"/>
			<property name="DAV:creationdate" value="20020401000000000"/>
		</properties>
	</collection>
');

-----------------------------------------------------------------------------
-- CONTENT_RESOURCE
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS CONTENT_RESOURCE;

CREATE TABLE CONTENT_RESOURCE
(
    RESOURCE_ID VARCHAR (255) NOT NULL,
	IN_COLLECTION VARCHAR (255) NOT NULL,
    XML TEXT
-- for BLOB body, add BODY BLOB -- and drop the content_resource_body_binary tables -ggolden
);

CREATE UNIQUE INDEX CONTENT_RESOURCE_INDEX ON CONTENT_RESOURCE
(
	RESOURCE_ID
);

CREATE INDEX CONTENT_IN_RESOURCE_INDEX ON CONTENT_RESOURCE
(
	IN_COLLECTION
);

-----------------------------------------------------------------------------
-- CONTENT_RESOURCE_BODY_BINARY
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS CONTENT_RESOURCE_BODY_BINARY;

CREATE TABLE CONTENT_RESOURCE_BODY_BINARY
(
    RESOURCE_ID VARCHAR (255) NOT NULL,
    BODY TEXT
);

CREATE UNIQUE INDEX CONTENT_RESOURCE_BB_INDEX ON CONTENT_RESOURCE_BODY_BINARY
(
	RESOURCE_ID
);
-----------------------------------------------------------------------------
-- DISCUSSION_CHANNEL
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS DISCUSSION_CHANNEL;

CREATE TABLE DISCUSSION_CHANNEL
(
    CHANNEL_ID VARCHAR (99) NOT NULL,
	NEXT_ID INT,
    XML TEXT
);

CREATE UNIQUE INDEX DISCUSSION_CHANNEL_INDEX ON DISCUSSION_CHANNEL
(
	CHANNEL_ID
)
;

-----------------------------------------------------------------------------
-- DISCUSSION_MESSABE
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS DISCUSSION_MESSAGE;

CREATE TABLE DISCUSSION_MESSAGE
(
    CHANNEL_ID VARCHAR (99) NOT NULL,
	MESSAGE_ID VARCHAR (32) NOT NULL,
	MESSAGE_DATE TIMESTAMP NOT NULL,
    XML TEXT
);

CREATE INDEX DISCUSSION_MESSAGE_INDEX ON DISCUSSION_MESSAGE
(
	CHANNEL_ID
)
;
-----------------------------------------------------------------------------
-- DISSERTATION_DISSERTATION
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS DISSERTATION_DISSERTATION;

CREATE TABLE DISSERTATION_DISSERTATION
(
    DISSERTATION_ID VARCHAR (99) NOT NULL,
    XML TEXT
);

CREATE UNIQUE INDEX DISSERTATION_DISS_INDEX ON DISSERTATION_DISSERTATION
(
	DISSERTATION_ID
)
;


-----------------------------------------------------------------------------
-- DISSERTATION_STEP
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS DISSERTATION_STEP;

CREATE TABLE DISSERTATION_STEP
(
    STEP_ID VARCHAR (99) NOT NULL,
    XML TEXT
);

CREATE UNIQUE INDEX DISSERTATION_STEP_INDEX ON DISSERTATION_STEP
(
	STEP_ID
)
;

-----------------------------------------------------------------------------
-- DISSERTATION_PATH
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS DISSERTATION_PATH;

CREATE TABLE DISSERTATION_PATH
(
    PATH_ID VARCHAR (99) NOT NULL,
    XML TEXT
);

CREATE UNIQUE INDEX DISSERTATION_PATH_INDEX ON DISSERTATION_PATH
(
	PATH_ID
)
;


-----------------------------------------------------------------------------
-- DISSERTATION_STATUS
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS DISSERTATION_STATUS;

CREATE TABLE DISSERTATION_STATUS
(
    STATUS_ID VARCHAR (99) NOT NULL,
    XML TEXT
);

CREATE UNIQUE INDEX DISSERTATION_STATUS_INDEX ON DISSERTATION_STATUS
(
	STATUS_ID
)
;


-----------------------------------------------------------------------------
-- DISSERTATION_CANDIDATEINFO
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS DISSERTATION_CANDIDATEINFO;

CREATE TABLE DISSERTATION_CANDIDATEINFO
(
    INFO_ID VARCHAR (99) NOT NULL,
    XML TEXT
);

CREATE UNIQUE INDEX DISSERTATION_INFO_INDEX ON DISSERTATION_CANDIDATEINFO
(
	INFO_ID
)
;

-----------------------------------------------------------------------------
-- CHEF_EVENT
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS CHEF_EVENT;

CREATE TABLE CHEF_EVENT
(
	EVENT_ID SERIAL,
	EVENT_DATE TIMESTAMP,
	EVENT VARCHAR (32),
	REF VARCHAR (255),
	SESSION_ID VARCHAR (32),
	EVENT_CODE VARCHAR (1)
);

CREATE UNIQUE INDEX CHEF_EVENT_INDEX ON CHEF_EVENT
(
	EVENT_ID
);

-- Not sure mysql does sequence... -ggolden

-- DROP SEQUENCE CHEF_EVENT_SEQ;

--CREATE SEQUENCE CHEF_EVENT_SEQ;
-----------------------------------------------------------------------------
-- CHEF_ID (SEQ)
-----------------------------------------------------------------------------

-- Not sure that mysql does sequences -ggolden

-- DROP SEQUENCE CHEF_ID_SEQ;

--CREATE SEQUENCE CHEF_ID_SEQ;
-----------------------------------------------------------------------------
-- MAILARCHIVE_CHANNEL
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS MAILARCHIVE_CHANNEL;

CREATE TABLE MAILARCHIVE_CHANNEL
(
    CHANNEL_ID VARCHAR (99) NOT NULL,
	NEXT_ID INT,
    XML TEXT
);

CREATE UNIQUE INDEX MAILARCHIVE_CHANNEL_INDEX ON MAILARCHIVE_CHANNEL
(
	CHANNEL_ID
)
;

-----------------------------------------------------------------------------
-- MAILARCHIVE_MESSAGE
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS MAILARCHIVE_MESSAGE;

CREATE TABLE MAILARCHIVE_MESSAGE
(
    CHANNEL_ID VARCHAR (99) NOT NULL,
	MESSAGE_ID VARCHAR (32) NOT NULL,
	MESSAGE_DATE TIMESTAMP NOT NULL,
    XML TEXT
);

CREATE INDEX MAILARCHIVE_MESSAGE_INDEX ON MAILARCHIVE_MESSAGE
(
	CHANNEL_ID
);

INSERT INTO MAILARCHIVE_CHANNEL VALUES ('/mailarchive/channel/!site/postmaster', 1, '<?xml version="1.0" encoding="UTF-8"?>
<channel context="!site" id="postmaster" next-message-id="1">
	<properties/>
</channel>
');
-----------------------------------------------------------------------------
-- CHEF_NOTIFICATION
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS CHEF_NOTIFICATION;

CREATE TABLE CHEF_NOTIFICATION
(
    NOTIFICATION_ID VARCHAR (99) NOT NULL,
    XML TEXT
);

CREATE UNIQUE INDEX CHEF_NOTIFICATION_INDEX ON CHEF_NOTIFICATION
(
	NOTIFICATION_ID
)
;
-----------------------------------------------------------------------------
-- CHEF_PRESENCE
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS CHEF_PRESENCE;

CREATE TABLE CHEF_PRESENCE
(
	SESSION_ID VARCHAR (32),
	LOCATION_ID VARCHAR (255)
);

CREATE INDEX CHEF_PRESENCE_SESSION_INDEX ON CHEF_PRESENCE
(
	SESSION_ID
);

CREATE INDEX CHEF_PRESENCE_LOCATION_INDEX ON CHEF_PRESENCE
(
	LOCATION_ID
);
-----------------------------------------------------------------------------
-- CHEF_REALM
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS CHEF_REALM;

CREATE TABLE CHEF_REALM
(
    REALM_ID VARCHAR (99) NOT NULL,
    XML TEXT
);

CREATE UNIQUE INDEX CHEF_REALM_INDEX ON CHEF_REALM
(
	REALM_ID
);

INSERT INTO CHEF_REALM VALUES ('/content/public/', '<?xml version="1.0" encoding="UTF-8"?>
<realm id="/content/public/">
	<ability auth="auth" lock="content.read"/>
	<ability anon="anon" lock="content.read"/>
</realm>
');

INSERT INTO CHEF_REALM VALUES ('/realm/', '<?xml version="1.0" encoding="UTF-8"?>
<realm id="/realm/">
	<ability auth="auth" lock="realm.add"/>
	<ability auth="auth" lock="realm.del"/>
	<ability auth="auth" lock="realm.upd"/>
</realm>
');

INSERT INTO CHEF_REALM VALUES ('/site/', '<?xml version="1.0" encoding="UTF-8"?>
<realm id="/site/">
	<ability auth="auth" lock="site.add"/>
</realm>
');

INSERT INTO CHEF_REALM VALUES ('/content/attachment/', '<?xml version="1.0" encoding="UTF-8"?>
<realm id="/content/attachment/">
	<ability auth="auth" lock="content.new"/>
	<ability auth="auth" lock="content.read"/>
	<ability auth="auth" lock="content.revise"/>
	<ability auth="auth" lock="content.delete"/>
</realm>
');

INSERT INTO CHEF_REALM VALUES ('/announcement/channel/!site/motd', '<?xml version="1.0" encoding="UTF-8"?>
<realm id="/announcement/channel/!site/motd">
	<ability auth="auth" lock="annc.read"/>
	<ability anon="anon" lock="annc.read"/>
</realm>
');

INSERT INTO CHEF_REALM VALUES ('/dissertation/s/rackham', '<?xml version="1.0" encoding="UTF-8"?>
<realm id="/dissertation/s/rackham">
	<ability auth="auth" lock="dis.step.read"/>
	<ability auth="auth" lock="dis.step.upd"/>
	<ability auth="auth" lock="dis.step.add"/>
	<ability auth="auth" lock="dis.step.del"/>
</realm>
');

INSERT INTO CHEF_REALM VALUES ('/dissertation/d/rackham', '<?xml version="1.0" encoding="UTF-8"?>
<realm id="/dissertation/d/rackham">
	<ability auth="auth" lock="dis.dis.upd"/>
</realm>
');

INSERT INTO CHEF_REALM VALUES ('/dissertation/p/rackham', '<?xml version="1.0" encoding="UTF-8"?>
<realm id="/dissertation/p/rackham">
	<ability auth="auth" lock="dis.path.upd"/>
</realm>
');

INSERT INTO CHEF_REALM VALUES ('/dissertation/ss/rackham', '<?xml version="1.0" encoding="UTF-8"?>
<realm id="/dissertation/ss/rackham">
	<ability auth="auth" lock="dis.status.upd"/>
	<ability auth="auth" lock="dis.status.add"/>
	<ability auth="auth" lock="dis.status.del"/>
	<ability auth="auth" lock="dis.status.read"/>
</realm>
');

INSERT INTO CHEF_REALM VALUES ('/dissertation/i/rackham', '<?xml version="1.0" encoding="UTF-8"?>
<realm id="/dissertation/i/rackham">
	<ability auth="auth" lock="dis.info.upd"/>
	<ability auth="auth" lock="dis.info.add"/>
	<ability auth="auth" lock="dis.info.del"/>
	<ability auth="auth" lock="dis.info.read"/>
</realm>
');

INSERT INTO CHEF_REALM VALUES ('~', '<?xml version="1.0" encoding="UTF-8"?>
<realm id="~">
<role id="maintain">
<ability lock="calendar.new"/>
<ability lock="calendar.read"/>
<ability lock="calendar.revise"/>
<ability lock="calendar.delete"/>
<ability lock="content.new"/>
<ability lock="content.read"/>
<ability lock="content.revise"/>
<ability lock="content.delete"/>
<ability lock="annc.new"/>
<ability lock="annc.read"/>
<ability lock="annc.revise.any"/>
<ability lock="annc.revise.own"/>
<ability lock="annc.delete.any"/>
<ability lock="annc.delete.own"/>
<ability lock="chat.new"/>
<ability lock="chat.read"/>
<ability lock="chat.revise.any"/>
<ability lock="chat.revise.own"/>
<ability lock="chat.delete.any"/>
<ability lock="chat.delete.own"/>
<ability lock="disc.new"/>
<ability lock="disc.read"/>
<ability lock="disc.revise.any"/>
<ability lock="disc.revise.own"/>
<ability lock="disc.delete.any"/>
<ability lock="disc.delete.own"/>
<ability lock="disc.new.topic"/>
<ability lock="mail.read"/>
<ability lock="mail.revise.any"/>
<ability lock="mail.revise.own"/>
<ability lock="mail.delete.any"/>
<ability lock="mail.delete.own"/>
<ability lock="site.visit"/>
<ability lock="site.visit.unp"/>
<ability lock="user.add"/>
<ability lock="user.upd.own"/>
</role>
<ability auth="auth" role="maintain"/>
<ability anon="anon" lock="user.add"/>
</realm>
');

INSERT INTO CHEF_REALM VALUES ('@', '<?xml version="1.0" encoding="UTF-8"?>
<realm id="@">
<role id="access">
<ability lock="dis.status.add"/>
<ability lock="dis.step.read"/>
<ability lock="dis.path.upd"/>
<ability lock="dis.status.del"/>
<ability lock="dis.status.read"/>
<ability lock="dis.status.upd"/>
<ability lock="dis.dis.read"/>
<ability lock="asn.read"/>
<ability lock="asn.submit"/>
<ability lock="chat.new"/>
<ability lock="chat.read"/>
<ability lock="disc.new"/>
<ability lock="disc.read"/>
<ability lock="disc.revise.own"/>
<ability lock="mail.read"/>
<ability lock="calendar.read"/>
<ability lock="annc.read"/>
<ability lock="site.visit"/>
<ability lock="dropbox.own"/>
<ability lock="content.read"/>
</role>
<role id="maintain">
<ability lock="asn.new"/>
<ability lock="asn.read"/>
<ability lock="asn.revise"/>
<ability lock="asn.delete"/>
<ability lock="chat.new"/>
<ability lock="chat.read"/>
<ability lock="chat.revise.any"/>
<ability lock="chat.revise.own"/>
<ability lock="chat.delete.any"/>
<ability lock="chat.delete.own"/>
<ability lock="mail.read"/>
<ability lock="mail.revise.any"/>
<ability lock="mail.revise.own"/>
<ability lock="mail.delete.any"/>
<ability lock="mail.delete.own"/>
<ability lock="annc.new"/>
<ability lock="annc.read"/>
<ability lock="annc.revise.any"/>
<ability lock="annc.revise.own"/>
<ability lock="annc.delete.any"/>
<ability lock="annc.delete.own"/>
<ability lock="annc.read.draft"/>
<ability lock="calendar.new"/>
<ability lock="calendar.delete"/>
<ability lock="calendar.read"/>
<ability lock="calendar.revise"/>
<ability lock="disc.new"/>
<ability lock="disc.read"/>
<ability lock="disc.revise.any"/>
<ability lock="disc.revise.own"/>
<ability lock="disc.delete.any"/>
<ability lock="disc.delete.own"/>
<ability lock="disc.read.draft"/>
<ability lock="disc.new.topic"/>
<ability lock="content.read"/>
<ability lock="content.revise"/>
<ability lock="content.delete"/>
<ability lock="content.new"/>
<ability lock="site.visit"/>
<ability lock="site.visit.unp"/>
<ability lock="site.upd"/>
<ability lock="site.del"/>
<ability lock="dis.step.upd"/>
<ability lock="dis.step.add"/>
<ability lock="dis.step.read"/>
<ability lock="dis.status.add"/>
<ability lock="dis.path.del"/>
<ability lock="dis.step.del"/>
<ability lock="dis.dis.del"/>
<ability lock="dis.dis.upd"/>
<ability lock="dis.path.add"/>
<ability lock="dis.status.del"/>
<ability lock="dis.status.read"/>
<ability lock="dis.status.upd"/>
<ability lock="dis.dis.add"/>
<ability lock="dis.path.read"/>
<ability lock="dis.dis.read"/>
</role>
</realm>
');
-----------------------------------------------------------------------------
-- CHEF_SESSION
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS CHEF_SESSION;

CREATE TABLE CHEF_SESSION
(
	SESSION_ID VARCHAR (32),
	SESSION_SERVER VARCHAR (64),
	"SESSION_USER" VARCHAR (99),
	SESSION_IP VARCHAR (16),
	SESSION_USER_AGENT VARCHAR (255),
	SESSION_START TIMESTAMP,
	SESSION_END TIMESTAMP
);

CREATE UNIQUE INDEX CHEF_SESSION_INDEX ON CHEF_SESSION
(
	SESSION_ID
);

CREATE INDEX CHEF_SESSION_SERVER_INDEX ON CHEF_SESSION
(
	SESSION_SERVER
);
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
<site id="~anon" title="Welcome" description="The anon page" joinable="false" status="2">
	<properties />
	<pages>
		<page id="100" title="main" layout="0">
			<properties />
			<tools>
				<tool id="110" toolId="chef.synoptic.announcement" title="Message of the day">
					<properties>
						<property name="channel" value="/announcement/channel/!site/motd"/>
						<property name="days" value="100"/>
						<property name="items" value="1"/>
						<property name="length" value="50000"/>
						<property name="show-subject" value="false"/>
					</properties>
				</tool>
				<tool id="120" toolId="chef.iframe" title="Welcome!">
					<properties>
						<property name="special" value="site"/>
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
		<page id="100" title="Home" layout="0">
			<properties />
			<tools>
				<tool id="110" toolId="chef.synoptic.announcement" title="Message of the Day">
					<properties>
						<property name="channel" value="/announcement/channel/!site/motd"/>
						<property name="days" value="100"/>
						<property name="items" value="1"/>
						<property name="length" value="50000"/>
						<property name="show-subject" value="false"/>
					</properties>
				</tool>
				<tool id="120" toolId="chef.iframe" title="My Workspace Information">
					<properties>
						<property name="special" value="workspace"/>
					</properties>
				</tool>
			</tools>
		</page>
		<page id="200" title="Membership" layout="0">
			<properties />
			<tools>
				<tool id="210" toolId="chef.membership" title="Membership">
					<properties />
				</tool>
			</tools>
		</page>
		<page id="300" title="Schedule" layout="0">
			<properties />
			<tools>
				<tool id="310" toolId="chef.schedule" title="Schedule">
					<properties />
				</tool>
			</tools>
		</page>
		<page id="400" title="Resources" layout="0">
			<properties />
			<tools>
				<tool id="410" toolId="chef.resources" title="Resources">
					<properties />
				</tool>
			</tools>
		</page>
		<page id="500" title="Worksite Setup" layout="0">
			<properties />
			<tools>
				<tool id="510" toolId="chef.sitesetup" title="Worksite Setup">
					<properties />
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
				<tool id="110" toolId="chef.synoptic.announcement" title="Message of the Day">
					<properties>
						<property name="channel" value="/announcement/channel/!site/motd"/>
						<property name="days" value="100"/>
						<property name="items" value="1"/>
						<property name="length" value="50000"/>
						<property name="show-subject" value="false"/>
					</properties>
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
		<page id="400" title="Announcements" layout="0">
			<properties />
			<tools>
				<tool id="410" toolId="chef.announcements" title="Announcements">
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
<property name="CHEF:site-email-notification-id" value="105642846272215"/>
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
<properties>
<property name="items" value="3"/>
<property name="show-subject" value="true"/>
<property name="length" value="50"/>
<property name="message-service" value="org.chefproject.service.generic.GenericAnnouncementService"/>
<property name="days" value="10"/>
</properties>
</tool>
<tool id="105645503746936"
title="Recent Discussion Items" toolId="chef.synoptic.discussion" layoutHints="1,1">
<properties>
<property name="items" value="3"/>
<property name="show-subject" value="true"/>
<property name="length" value="50"/>
<property name="message-service" value="org.chefproject.service.generic.GenericDiscussionService"/>
<property name="days" value="10"/>
</properties>
</tool>
<tool id="105645504631337" title="Recent Chat Messages" toolId="chef.synoptic.chat" layoutHints="2,1">
<properties>
<property name="items" value="3"/>
<property name="show-subject" value="false"/>
<property name="length" value="50"/>
<property name="message-service" value="org.chefproject.service.generic.GenericChatService"/>
<property name="days" value="10"/>
</properties>
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
<tool id="10564282165356" title="Threaded Discussion" toolId="chef.threadeddiscussion">
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
-----------------------------------------------------------------------------
-- CHEF_USER
-----------------------------------------------------------------------------

-- DROP TABLE IF EXISTS CHEF_USER;

CREATE TABLE CHEF_USER
(
    USER_ID VARCHAR (99) NOT NULL,
    XML TEXT
);

CREATE UNIQUE INDEX CHEF_USER_INDEX ON CHEF_USER
(
	USER_ID
)
;

INSERT INTO CHEF_USER VALUES ('admin', '<?xml version="1.0" encoding="UTF-8"?>
<user id="admin" first-name="CHEF" last-name="Administrator" email="chef-support@umich.edu" pw="ISMvKXpXpadDiUoOSoAf">
	<properties>
		<property name="CHEF:creator" value="admin"/>
		<property name="CHEF:modifiedby" value="admin"/>
		<property name="DAV:getlastmodified" value="20020823003527581"/>
		<property name="DAV:creationdate" value="20020823003506140"/>
	</properties>
</user>
');

INSERT INTO CHEF_USER VALUES ('postmaster', '<?xml version="1.0" encoding="UTF-8"?>
<user id="postmaster" first-name="CHEF" last-name="Postmaster" email="chef-support@umich.edu" pw="1B2M2Y8AsgTpgAmY7PhC">
	<properties>
		<property name="CHEF:creator" value="admin"/>
		<property name="CHEF:modifiedby" value="admin"/>
		<property name="DAV:getlastmodified" value="20020823003527581"/>
		<property name="DAV:creationdate" value="20020823003506140"/>
	</properties>
</user>
');
