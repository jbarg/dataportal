//*** 

This is the menu creation code - place it right after you body tag

Feel free to add this to a stand-alone js file and link it to your page.

**/



//Menu object creation

oCMenu=new makeCM("oCMenu") //Making the menu object. Argument: menuname



//Menu properties   

oCMenu.pxBetween=10

oCMenu.fromLeft=10 

oCMenu.fromTop=150   

oCMenu.rows=0 

oCMenu.menuPlacement=0



oCMenu.offlineRoot="file:///C|/Inetpub/wwwroot/dhtmlcentral/" 

oCMenu.onlineRoot="" 

oCMenu.resizeCheck=1 

oCMenu.wait=1000 

oCMenu.fillImg="cm_fill.gif"

oCMenu.zIndex=0



//Background bar properties

oCMenu.useBar=1

oCMenu.barWidth="menu"

oCMenu.barHeight="menu" 

oCMenu.barClass="clBar"

oCMenu.barX="menu"

oCMenu.barY="menu"

oCMenu.barBorderX=0

oCMenu.barBorderY=0

oCMenu.barBorderClass=""



//Level properties - ALL properties have to be spesified in level 0

oCMenu.level[0]=new cm_makeLevel() //Add this for each new level

oCMenu.level[0].width=80

oCMenu.level[0].height=40

oCMenu.level[0].regClass="clLevel0"

oCMenu.level[0].overClass="clLevel0over"

oCMenu.level[0].borderX=0 

oCMenu.level[0].borderY=0

oCMenu.level[0].borderClass=0

oCMenu.level[0].offsetX=0 

oCMenu.level[0].offsetY=0

oCMenu.level[0].rows=0

oCMenu.level[0].align="right" 





//EXAMPLE SUB LEVEL[1] PROPERTIES - You have to spesify the properties you want different from LEVEL[0] - If you want all items to look the same just remove this

oCMenu.level[1]=new cm_makeLevel() //Add this for each new level (adding one to the number)

oCMenu.level[1].width=oCMenu.level[0].width-15

oCMenu.level[1].height=22

oCMenu.level[1].regClass="clLevel1"

oCMenu.level[1].overClass="clLevel1over"

oCMenu.level[1].style=""

oCMenu.level[1].align="right" 

oCMenu.level[1].offsetX=0

oCMenu.level[1].offsetY=0

oCMenu.level[1].borderClass="clLevel1border"

oCMenu.level[1].borderX=1 

oCMenu.level[1].borderY=1

oCMenu.level[1].rows=0

oCMenu.level[1].align="bottom" 



/******************************************

Menu item creation:

myCoolMenu.makeMenu(name, parent_name, text, link, target, width, height, regImage, overImage, regClass, overClass , align, rows, nolink, onclick, onmouseover, onmouseout) 

*************************************/

oCMenu.makeMenu('top0','','<b>&nbsp;Search</b>')

	oCMenu.makeMenu('sub00','top0','Basic ','/BasicSearch.jsp','parent')

        oCMenu.makeMenu('sub01','top0','Advanced ','/AdvancedSearch.jsp','parent')

                //oCMenu.makeMenu('sub001','sub00','Advanced Search1','http://d')

		//oCMenu.makeMenu('sub002','sub00','Advanced Search2','http://d')

	//oCMenu.makeMenu('sub01','top0','Basic Search','http://www.teamtalk.com')



oCMenu.makeMenu('top1','','&nbsp;Cart','/Checkout.jsp','parent')

	//oCMenu.makeMenu('sub10','top1','View Shopping Cart','http://esc.dl.ac.uk')





oCMenu.makeMenu('top2','','&nbsp;Help','/menu.jsp','','','','','','','','','','',' window.open("../help/Help.jsp","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0")')

	//oCMenu.makeMenu('sub21','top2','View Help','http://esc.dl.ac.uk')



oCMenu.makeMenu('top3','','&nbsp;Log out','/../servlet/SaveCartServlet','parent')



//Leave this line - it constructs the menu

oCMenu.construct()	