

<%


	boolean loggedIn = false;

	// retrieve login info from user's session object...if available
	//Boolean status = (Boolean) session.getAttribute("LOGIN_STATUS");
	uk.ac.dl.web.ClockBean clock = (uk.ac.dl.web.ClockBean)session.getAttribute("clockbean");
        
        
	// if status is non-null then let's get the login status
	if (clock != null) {
                
		loggedIn = clock.isValid();
		
	}

	//FD	
	// if user is not logged in, then send them to error page
	if ( !loggedIn || clock == null) {
		throw new uk.ac.dl.web.SessionTimedOutException();
		//response.sendRedirect("../");

	}
%>