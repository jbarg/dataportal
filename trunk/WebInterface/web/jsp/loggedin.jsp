

<%
	boolean loggedIn = false;
	// retrieve login info from user's session object...if available
	//Boolean status = (Boolean) session.getAttribute("LOGIN_STATUS");
	uk.ac.dl.web.ClockBean clock = (uk.ac.dl.web.ClockBean)session.getAttribute("clockbean");
        
	// if status is non-null then let's get the login status
	if (clock != null) {
		loggedIn = clock.isValid();
	}
	// if clock is null then session already been invalidated
        if(clock == null){
                throw new uk.ac.dl.web.SessionTimedOutException();
        }
        // if get to here, then first time user has get here, and certi has ran out
	else if ( !loggedIn ) {
                uk.ac.dl.web.Cleanup.session(session.getAttribute("sessionid"));
		throw new uk.ac.dl.web.SessionTimedOutException();
	}
%>