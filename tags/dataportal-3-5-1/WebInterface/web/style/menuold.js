

/***

This is the menu creation code - place it right after you body tag

Feel free to add this to a stand-alone js file and link it to your page.

**/



//Extra code to find position:

function findPos(){

  if(bw.ns4){   //Netscape 4

    x = document.layers.layerMenu.pageX

    y = document.layers.layerMenu.pageY

  }else{ //other browsers

    x=0; y=0; var el,temp

    el = bw.ie4?document.all["divMenu"]:document.getElementById("divMenu");

    if(el.offsetParent){

      temp = el

      while(temp.offsetParent){ //Looping parent elements to get the offset of them as well

        temp=temp.offsetParent;

        x+=temp.offsetLeft

        y+=temp.offsetTop;

      }

    }

    x+=el.offsetLeft

    y+=el.offsetTop

  }

  //Returning the x and y as an array

  return [x,y]

}



pos = findPos()



//Menu object creation

oCMenu=new makeCM("oCMenu") //Making the menu object. Argument: menuname



//Menu properties

//**** this is the pixel height between buttons ****/

oCMenu.pxBetween=10

//*************************************************/

//Using the cm_page object to place the menu ----

oCMenu.fromLeft=pos[0]

oCMenu.fromTop=pos[1]

//We also need to "re place" the menu on resize. So:

oCMenu.onresize="pos = findPos(); oCMenu.fromLeft=pos[0]; oCMenu.fromTop=pos[1]"



oCMenu.rows=0

oCMenu.menuPlacement=0







oCMenu.offlineRoot="."

oCMenu.onlineRoot="."

oCMenu.resizeCheck=1

oCMenu.wait=200

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

oCMenu.level[0].width=75

oCMenu.level[0].height=40

oCMenu.level[0].regClass="clLevel0"

oCMenu.level[0].overClass="clLevel0over"

oCMenu.level[0].borderX=1

oCMenu.level[0].borderY=1

oCMenu.level[0].borderClass="clLevel0border"

oCMenu.level[0].offsetX=0

oCMenu.level[0].offsetY=0

oCMenu.level[0].rows=0

oCMenu.level[0].arrow=0

oCMenu.level[0].arrowWidth=0

oCMenu.level[0].arrowHeight=0

oCMenu.level[0].align="right"





//EXAMPLE SUB LEVEL[1] PROPERTIES - You have to specify the properties you want different from LEVEL[0] - If you want all items to look the same just remove this

oCMenu.level[1]=new cm_makeLevel() //Add this for each new level (adding one to the number)

oCMenu.level[1].width=oCMenu.level[0].width-2

oCMenu.level[1].height=22

oCMenu.level[1].width=65

oCMenu.level[1].regClass="clLevel1"

oCMenu.level[1].overClass="clLevel1over"

oCMenu.level[1].borderX=1

oCMenu.level[1].borderY=1

oCMenu.level[1].align="right"

oCMenu.level[1].offsetX=-(oCMenu.level[0].width-2)/2+20

oCMenu.level[1].offsetY=0

oCMenu.level[1].borderClass="clLevel1border"





//EXAMPLE SUB LEVEL[2] PROPERTIES - You have to spesify the properties you want different from LEVEL[1] OR LEVEL[0] - If you want all items to look the same just remove this

oCMenu.level[2]=new cm_makeLevel() //Add this for each new level (adding one to the number)

oCMenu.level[2].width=150

oCMenu.level[2].height=20

oCMenu.level[2].offsetX=0

oCMenu.level[2].offsetY=0

oCMenu.level[2].regClass="clLevel2"

oCMenu.level[2].overClass="clLevel2over"

oCMenu.level[2].borderClass="clLevel2border"





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



oCMenu.makeMenu('top1','','&nbsp;Shopping &nbsp;Cart','/Checkout.jsp','parent')

	//oCMenu.makeMenu('sub10','top1','View Shopping Cart','http://esc.dl.ac.uk')





oCMenu.makeMenu('top2','','&nbsp;Help','/menu.jsp','','','','','','','','','','',' window.open("../help/Help.jsp","Help","toolbar=no,location=no,directories=no,status=no,menubar=no,scrollbars=yes,resizable=yes,copyhistory=yes,width=900,height=600,left=0,top=0")')

	//oCMenu.makeMenu('sub21','top2','View Help','http://esc.dl.ac.uk')



oCMenu.makeMenu('top3','','&nbsp;Log out','/../servlet/SaveCartServlet','parent')

	///oCMenu.makeMenu('sub30','top3','General','/forums/forum.asp?FORUM_ID=6&CAT_ID=1&Forum_Title=General+DHTML+issues')

	//oCMenu.makeMenu('sub31','top3','Scripts','/forums/forum.asp?FORUM_ID=4&CAT_ID=1&Forum_Title=DHTML+Scripts')

	//oCMenu.makeMenu('sub32','top3','Crossbrowser','/forums/forum.asp?FORUM_ID=3&CAT_ID=1&Forum_Title=Crossbrowser+DHTML')

	//oCMenu.makeMenu('sub33','top3','CoolMenus','/forums/forum.asp?FORUM_ID=2&CAT_ID=1&Forum_Title=CoolMenus')

	//oCMenu.makeMenu('sub34','top3','dhtmlcentral.com','/forums/forum.asp?FORUM_ID=5&CAT_ID=1&Forum_Title=dhtmlcentral%2Ecom')

	//oCMenu.makeMenu('sub35','top3','Cool sites','/forums/forum.asp?FORUM_ID=1&CAT_ID=1&Forum_Title=Cool+sites')









//Leave this line - it constructs the menu

oCMenu.construct()

