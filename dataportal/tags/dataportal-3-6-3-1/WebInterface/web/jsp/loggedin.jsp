

<%
	boolean loggedIn = false;
	// retrieve login info from user's session object...if available
	//Boolean status = (Boolean) session.getAttribute("LOGIN_STATUS");
	uk.ac.dl.beans.ClockBean clock = (uk.ac.dl.beans.ClockBean)session.getAttribute("clockbean");
        
	// if status is non-null then let's get the login status
	if (clock != null) {
		loggedIn = clock.isValid();
	}
	// if clock is null then session already been invalidated
        if(clock == null){
                //section in web.xml file.  Any SessionTimedOutException() will 
                //go to logoff.jsp which will invalidate the session
                throw new uk.ac.dl.web.SessionTimedOutException();
        }
        // if get to here, then first time the user has got here, and certi has ran out
        // clean up and invalidate session.  Next time user will go to above
	else if ( !loggedIn ) {
                uk.ac.dl.beans.Cleanup.session(session);
		throw new uk.ac.dl.web.SessionTimedOutException();
	}
%>