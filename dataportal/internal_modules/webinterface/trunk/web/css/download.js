
       
        var win;
        function download(id,type,from){
        

//alert(BrowserDetect.browser +" "+BrowserDetect.version +" "+BrowserDetect.OS );

        //locat center
        var leftprop, topprop, screenX, screenY, cursorX, cursorY, padAmt

        if(BrowserDetect.browser == "Explorer")
        {
        screenY = document.body.offsetHeight
        screenX = document.body.offsetWidth
 //screenX = window.screen.availWidth
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
		
        if(BrowserDetect.browser == "Explorer")
        {
        leftprop = leftvar
        topprop = rightvar
        }
        else
        { // adjust Netscape coordinates for scrolling
        leftprop = leftvar 
        topprop = rightvar -50
        }

        
        if(BrowserDetect.browser == "Explorer"){

        // alert("micrsoft");
        if(win) win.close();
       
        //alert("../../servlet/DownloadServlet?url="+id+"&type="+type+"&from="+from+"");
        win = window.open("../../servlet/DownloadServlet?url="+id+"&type="+type+"&from="+from+"","Download","dependent=yes,toolbar=no,location=no,directories=no,status=no,menubar=no,width=300px,height=160px,left="+leftprop+",top="+topprop+"");

        }
        else if(BrowserDetect.browser == "Netscape" && BrowserDetect.version == "Moziila"){
        //alert("netsacpe 4");
        win = window.open("../../servlet/DownloadServlet?url="+id+"&type="+type+"&from="+from+"","Download Data","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=no,copyhistory=yes,width=300,height=150,left=0,top=0");
        }
          else if(BrowserDetect.browser == "Firefox"){
      
       // win = window.open("../../servlet/DownloadServlet?url="+id+"&type="+type+"&from="+from+"","Download Data","toolbar=no, width=300, height=160");
         win = window.open("../../servlet/DownloadServlet?url="+id+"&type="+type+"&from="+from+"","Download","toolbar=no,location=no,directories=no,status=no,menubar=no,width=300px,height=160px,left="+leftprop+",top="+topprop+"");

        }
        else {
             win = window.open("../../servlet/DownloadServlet?url="+id+"&type="+type+"&from="+from+"","Download","toolbar=no,location=no,directories=no,status=no,menubar=no,width=300px,height=160px,left="+leftprop+",top="+topprop+"");

        }
        }




           //        Detects which browser

           var BrowserDetect = {
	init: function () {
		this.browser = this.searchString(this.dataBrowser) || "An unknown browser";
		this.version = this.searchVersion(navigator.userAgent)
			|| this.searchVersion(navigator.appVersion)
			|| "an unknown version";
		this.OS = this.searchString(this.dataOS) || "an unknown OS";
	},
	searchString: function (data) {
		for (var i=0;i<data.length;i++)	{
			var dataString = data[i].string;
			var dataProp = data[i].prop;
			this.versionSearchString = data[i].versionSearch || data[i].identity;
			if (dataString) {
				if (dataString.indexOf(data[i].subString) != -1)
					return data[i].identity;
			}
			else if (dataProp)
				return data[i].identity;
		}
	},
	searchVersion: function (dataString) {
		var index = dataString.indexOf(this.versionSearchString);
		if (index == -1) return;
		return parseFloat(dataString.substring(index+this.versionSearchString.length+1));
	},
	dataBrowser: [
		{ 	string: navigator.userAgent,
			subString: "OmniWeb",
			versionSearch: "OmniWeb/",
			identity: "OmniWeb"
		},
		{
			string: navigator.vendor,
			subString: "Apple",
			identity: "Safari"
		},
		{
			prop: window.opera,
			identity: "Opera"
		},
		{
			string: navigator.vendor,
			subString: "iCab",
			identity: "iCab"
		},
		{
			string: navigator.vendor,
			subString: "KDE",
			identity: "Konqueror"
		},
		{
			string: navigator.userAgent,
			subString: "Firefox",
			identity: "Firefox"
		},
		{
			string: navigator.vendor,
			subString: "Camino",
			identity: "Camino"
		},
		{		// for newer Netscapes (6+)
			string: navigator.userAgent,
			subString: "Netscape",
			identity: "Netscape"
		},
		{
			string: navigator.userAgent,
			subString: "MSIE",
			identity: "Explorer",
			versionSearch: "MSIE"
		},
		{
			string: navigator.userAgent,
			subString: "Gecko",
			identity: "Mozilla",
			versionSearch: "rv"
		},
		{ 		// for older Netscapes (4-)
			string: navigator.userAgent,
			subString: "Mozilla",
			identity: "Netscape",
			versionSearch: "Mozilla"
		}
	],
	dataOS : [
		{
			string: navigator.platform,
			subString: "Win",
			identity: "Windows"
		},
		{
			string: navigator.platform,
			subString: "Mac",
			identity: "Mac"
		},
		{
			string: navigator.platform,
			subString: "Linux",
			identity: "Linux"
		}
	]

};
BrowserDetect.init();
        
       

   


  