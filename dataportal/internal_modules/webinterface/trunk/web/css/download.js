
       
        var win;
        function download(id,type,from){
        
        //locat center
        var leftprop, topprop, screenX, screenY, cursorX, cursorY, padAmt

        if(navigator.appName == "Microsoft Internet Explorer")
        {
        screenY = document.body.offsetHeight
        screenX = window.screen.availWidth
        }
        else
        { 
        screenY = window.outerHeight
        screenX = window.outerWidth

        // change made 3/16/01 to work with Netscape:
        //  screenY = screen.height;
        //  screenX = screen.width;
        }



        leftvar = (screenX - 300) / 2
        rightvar = (screenY - 40) / 2
		
        if(navigator.appName == "Microsoft Internet Explorer")
        {
        leftprop = leftvar
        topprop = rightvar
        }
        else
        { // adjust Netscape coordinates for scrolling
        leftprop = (leftvar + pageXOffset)
        topprop = (rightvar + pageYOffset)
        }

        
        if(navigator.appVersion.indexOf("MSIE") !=-1 ){

        // alert("micrsoft");
        if(win) win.close();
       
        //alert("../../servlet/DownloadServlet?url="+id+"&type="+type+"&from="+from+"");
        win = window.open("../../servlet/DownloadServlet?url="+id+"&type="+type+"&from="+from+"","Download","toolbar=no,location=no,directories=no,status=no,menubar=no,width=300px,height=160px,left="+leftprop+",top="+topprop+"");

        }
        else if(navigator.userAgent.indexOf("Mozilla/4") != -1 && navigator.appName.indexOf("Netscape") !=-1 && parseInt(navigator.appVersion) < 5){
        //alert("netsacpe 4");
        win = window.open("../../servlet/DownloadServlet?url="+id+"&type="+type+"","Download Data","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=no,copyhistory=yes,width=300,height=150,left=0,top=0");
        }
        else{
        win = window.open("../../servlet/DownloadServlet?url="+id+"&type="+type+"","Download Data","toolbar=no, width=300, height=160");
        }
        }
        
       

   


  