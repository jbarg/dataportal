<% 
        boolean loggedIn = false;

        // retrieve login info from user's session object...if available
        Boolean status = (Boolean) session.getAttribute("LOGIN_STATUS");

        // if status is non-null then let's get the login status
        if (status != null) {
                loggedIn = status.booleanValue();

        }

        //FD
        // if user is not logged in, then send them to error page
        if ( !loggedIn || status == null) {

                response.sendRedirect("../html/SessionTimedOut.html");
                return;
        }
%>