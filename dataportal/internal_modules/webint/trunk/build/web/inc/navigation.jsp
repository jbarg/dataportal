 <img align="center" src="images/cclrc.jpg" width="160" height="40" />
    <br />
    <br />
   <f:loadBundle
		basename="uk.example_messages"
		var="example_messages" />
    <t:div id="subnavigation_outer">
        <t:div id="subnavigation">
            <t:panelNavigation2 id="nav1" layout="list" itemClass="mypage" activeItemClass="selected"
                disabledStyle="color:red;padding: 2px 20px 2px 25px">
                <t:commandNavigation2 value="#{example_messages['panelnav_contact']}" style="padding-left: 0px;">

                </t:commandNavigation2>
                <t:commandNavigation2 value="#{example_messages['panelnav_contact']}" externalLink="http://www.yahoo.com"
                target="_blank"/>
                <t:commandNavigation2 value="#{example_messages['panelnav_contact']}" style="padding-left: 150px;">

                </t:commandNavigation2>
                <t:commandNavigation2 value="#{example_messages['panelnav_contact']}" externalLink="http://mail.yahoo.com"
                target="_blank"/>
            </t:panelNavigation2>  

        </t:div>
    </t:div>