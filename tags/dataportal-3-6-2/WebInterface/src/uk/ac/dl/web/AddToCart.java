package uk.ac.dl.web;
import java.io.*;
import java.util.*;
import javax.servlet.*;
import javax.servlet.http.*;
import org.jdom.*;
import ac.dl.taglib.*;

//log classes
import org.apache.log4j.*;
import uk.ac.dl.xml.*;
import uk.ac.dl.dn.*;
import java.util.Properties;
//web axis stuff
import javax.xml.rpc.ParameterMode;
import org.apache.axis.client.Call;
import org.apache.axis.encoding.XMLType;
import org.apache.axis.client.Service;
import javax.xml.namespace.QName;


/** This class takes the studies selected from the Expand class to decrease
 * the size of the xlm document.  It does this using a xsl. The servlet then
 * passes on the response to a jsp which displays the new xml
 * @author Glen Drinkwater
 * @date 04/07/02
 */
public class AddToCart extends HttpServlet{
    
    //set static log for the class
   Logger logger = Logger.getLogger(this.getClass().getName());
    
    
    
    public void doPost(HttpServletRequest request, HttpServletResponse response)
    throws IOException, ServletException {
        
        //get session
        HttpSession session = request.getSession(false);
        if(session == null){
           response.sendRedirect("../html/SessionTimedOut.html");
            return;
        }
        
        //get working directories
        String workingDir = (String)session.getAttribute("wd");
        
        String sid = (String )session.getAttribute("sessionid");
        Properties prop = (Properties)session.getAttribute("props");
        String url = prop.getProperty("CART");
        //System.out.println("shop cart url is "+url);
        try{
            RecordDB db = createDB(request);
            Document doc = createDoc(db,session);
            
            //Saver.save(doc,"c:/cart.xml");
            //System.out.println("document to pass to thingy is "+doc);
            //Saver.save(doc,"c:/File.xml");
            
            org.w3c.dom.Document docD = Converter.JDOMtoDOM(doc);
            
            addToCart(sid,docD,url);
            response.sendRedirect("../jsp/Checkout.jsp");
        }
        catch(Exception e){
            e.printStackTrace();
            //System.out.println(e);
            response.sendRedirect("../jsp/error.jsp");
        }
        
        
        
    }
    public void addToCart(String sid, org.w3c.dom.Document doc,String url){
        
        try{
            org.w3c.dom.Element el = doc.getDocumentElement();
            
            String endpoint = url;
            Service  service = new Service();
            Call     call    = (Call) service.createCall();
            
            call.setTargetEndpointAddress( new java.net.URL(endpoint) );
            call.setOperationName( "addToCart" );
            
            call.addParameter( "op1", XMLType.XSD_STRING, ParameterMode.IN );
            call.addParameter( "op2", XMLType.SOAP_ELEMENT, ParameterMode.IN );
            
            call.setReturnType( XMLType.XSD_BOOLEAN );
            
            
            Object[] ob = new Object[]{ sid,el};
            Boolean ret = (Boolean) call.invoke(ob );
        }
        catch(Exception e){
            logger.error("unable to add to cart",e);
            
        }
    }
    
    public Document createDoc(RecordDB db, HttpSession session){
        
        //iterate through the list, get the record from the db. and add to the cart
        Iterator it = db.iterator();
        String next = null;
        Record record = null;
        Document doc = new Document(new Element("shoppingCart"));
        Element root = doc.getRootElement();
        while(it.hasNext()){
            record = (Record)it.next();
            //get the record corresponding to the url passed
            //record = db.getRecord(next);
            String AssStudy = record.getName();
            String notes =record.getNotes();
            String url_1 = record.getUrl();
            
            Date date_1 = new Date();
            String dateadded  = date_1.toString();
            String type = record.getType();
            
            String name  = record.getStudyName();
            //gets the urls assciated with the dataset
            String urls=record.getFacility();
            
            //cannot have spaces in attributes, but some urls for the dataportal have
            // them  so remove
            //remove spaces out of the url
            int no1 = url_1.length();
            StringBuffer h1 = new StringBuffer();
            for(int i = 0; i<no1; i++){
                if( url_1.charAt(i) == ' ') h1.append("%20");
                else if(url_1.charAt(i) == '\\') h1.append("/");
                else h1.append(url_1.charAt(i));
            }
            url_1 = h1.toString();
            
            
            Element item = new Element("item");
            item.setAttribute("type", type);
            item.setAttribute("url", url_1);
            
            
            
            
            root.addContent(item);
            Element StudyName = new Element("name");
            StudyName.setText(name);
            Element url = new Element("url");
            url.setText(url_1);
            Element date = new Element("date");
            date.setText(dateadded);
            Element study = new Element("assStudy");
            study.setText(AssStudy);
            Element note = new Element("notes");
            note.setText(notes);
            item.addContent(note);
            item.addContent(study);
            item.addContent(date);
            item.addContent(StudyName);
            item.addContent(url);
            
            //add n to the shopping cart
            
            Object[] object = (Object[])session.getAttribute("query");
            String[] facs = (String[])object[1];
            String dis = (String)object[2];
            String max_wait  = String.valueOf(((Integer)object[3]).intValue());
            Element query = new Element("query");
            Element facility = new Element("facility");
            StringBuffer Sfacs = new StringBuffer();
            for(int i = 0;i <facs.length;i++){
                Sfacs.append(facs[i]);
                
            }
            // facility.setAttribute(new Attribute("facs",Sfacs.toString()));
            facility.addContent(Sfacs.toString());
            Element discipline =new Element("discipline");
            //discipline.setAttribute(new Attribute("dis",dis));
            discipline.addContent(dis);
            Element wait = new Element("wait");
            /// wait.setAttribute(new Attribute("wait",max_wait));
            wait.addContent(max_wait);
            query.addContent(facility);
            query.addContent(discipline);
            query.addContent(wait);
            item.addContent(query);
            
            //add all thestart and end dates on
            String startday = record.getStartDay();
            String startmonth = record.getStartMonth();
            String startyear = record.getStartYear();
            String endday = record.getEndDay();
            String endmonth = record.getEndMonth();
            String endyear = record.getEndYear();
            
            
            String minalt = record.getMinAlt();
            String minlat = record.getMinLat();
            String minlong = record.getMinLon();
            String maxalt = record.getMaxAlt();
            String maxlat = record.getMaxLat();
            String maxlong = record.getMaxLon();
            
            Element cera = new Element("cera");
            
            cera.setAttribute(new Attribute("startday",startday));
            cera.setAttribute(new Attribute("endday",endday));
            
            cera.setAttribute(new Attribute("startmonth",startmonth));
            cera.setAttribute(new Attribute("endmonth",endmonth));
            
            cera.setAttribute(new Attribute("startyear",startyear));
            cera.setAttribute(new Attribute("endyear",endyear));
            
            cera.setAttribute(new Attribute("minalt",minalt));
            cera.setAttribute(new Attribute("maxalt",maxalt));
            
            cera.setAttribute(new Attribute("minlon",minlong));
            cera.setAttribute(new Attribute("maxlon",maxlong));
            
            cera.setAttribute(new Attribute("minlat",minlat));
            cera.setAttribute(new Attribute("maxlat",maxlat));
            
            //add urls to document
            Element urlsd = new Element("urls");
            urlsd.addContent(urls);
            item.addContent(cera);
            item.addContent(urlsd);
            
        }
        return doc;
    }
    
    
    public RecordDB createDB(HttpServletRequest request){
        String[] values = null;
        String[] valuesS = null;
        String[] valuesF = null;
        int valuesLength = 0;
        int valuesSLength = 0;
        int valuesFLength = 0;
        //System.out.println("in the create db");
        //path to get to the items info in the xml
        String  xpath = "DataHoldingLocator/DataSetLocator[DataName='";
        String xpathDataSet=  "DataHoldingLocator/DataSetLocator[DataName='";
        String  xpath2 = "//URL";
        HttpSession session =  request.getSession();
        RecordDB db11 = new RecordDB();
        //get dataset user wishes to add to shopping basket
        values = request.getParameterValues("datasetdownload");
        if(values == null) {
            
            //set length of datasets to zero
            valuesLength = 0;
        }
        else{
            //get length of values
            valuesLength = values.length;
        }
        //get files user wishes to add to shopping basket
        valuesF = request.getParameterValues("filedownload");
        if(valuesF == null) {
            
            valuesFLength = 0;
        }
        else{
            //get length of values
            valuesFLength = valuesF.length;
        }
        
        
        //get files user wishes to add to shopping basket
        valuesS = request.getParameterValues("studydownload");
        if(valuesS == null) {
            logger.debug("No Data Sets added to shopping basket");
            valuesSLength = 0;
        }
        else{
            //get length of values
            valuesSLength = valuesS.length;
        }
        
        //get working directories
        String workingDir = (String)session.getAttribute("wd");
        String dn = Convert.removeSpaces((String )session.getAttribute("dn"));
        
        //create a xsl file to turn the large xml file a small one easily travered to get info out
        File temp2 = new File(workingDir+File.separator+"profiles"+File.separator+"res"+Math.random()+".xsl");
        FileWriter  d1 = null;
        File shop = null;
        try{
            d1 = new FileWriter(temp2);
            d1.write("<?xml version='1.0'?>\n");
            d1.write("<xsl:stylesheet xmlns:xsl='http://www.w3.org/1999/XSL/Transform' version='1.0'>\n");
            d1.write("<xsl:output method='xml' indent='yes'/>\n");
            d1.write("\t<xsl:template match=\"CLRCMetadata\">\n");
            d1.write("\t\t<xsl:element name=\"shoppingbasket\">\n");
            
            for(int i=0;i<valuesLength;i++){
                
                d1.write("\t\t\t<xsl:for-each select='MetadataRecord'>\n");
                d1.write("\t\t\t\t<xsl:if test=\""+xpath+""+values[i]+"']\">\n");
                d1.write("\t\t\t\t<xsl:element name='record'>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='name'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>"+values[i]+"</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='url'>\n");
                d1.write("\t\t\t\t\t\t<xsl:value-of select=\""+xpath+""+values[i]+"']"+xpath2+"\" />\n");
                
                //d1.write("<xsl:text>www.dl.ac.uk"+i+"</xsl:text>");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='assStudy'>\n");
                d1.write("\t\t\t\t\t\t<xsl:value-of select=\"*/StudyName\" />\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='type'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>dataset</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='startyear'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='startmonth'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='startday'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='endyear'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='endmonth'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='endday'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='minlon'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='maxlon'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='minlat'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='maxlat'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='minalt'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='maxalt'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                
                //add the url for the dataset
                d1.write("\t\t\t\t\t<xsl:attribute name='urls'>\n");
                d1.write("<xsl:for-each select=\""+xpath+values[i]+"']/../FileLocator\" >\n");
                d1.write(" <xsl:value-of select=\".//URL\"/><xsl:text>   </xsl:text>");
                d1.write("</xsl:for-each>\n");
                
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                
                d1.write("\t\t\t\t</xsl:element>\n");
                d1.write("\t\t\t</xsl:if>\n");
                d1.write("\t\t</xsl:for-each>\n");
            }
            
            for(int i=0;i<valuesSLength;i++){
                
                d1.write("\t\t\t<xsl:for-each select='MetadataRecord'>\n");
                d1.write("<xsl:if test=\"@metadataID='"+valuesS[i]+"'\">\n");
                d1.write("\t\t\t\t<xsl:element name='record'>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='name'>\n");
                d1.write("\t\t\t\t\t\t<xsl:value-of select=\"*/StudyName\" />\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='url'>\n");
                d1.write("<xsl:text>"+valuesS[i]+"</xsl:text>");
                
                //d1.write("<xsl:text>www.dl.ac.uk"+i+"</xsl:text>");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='assStudy'>\n");
                d1.write("\t\t\t\t\t\t<xsl:value-of select=\"*/StudyName\" />\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='type'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Study</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='startyear'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='startmonth'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='startday'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='endyear'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='endmonth'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='endday'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='minlon'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='maxlon'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='minlat'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='maxlat'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='minalt'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='maxalt'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>Not_needed</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t</xsl:element>\n");
                d1.write("\t\t\t</xsl:if>\n");
                d1.write("\t\t</xsl:for-each>\n");
            }
            for(int i=0;i<valuesFLength;i++){
                
                d1.write("\t\t\t<xsl:for-each select='MetadataRecord'>\n");
                d1.write("\t\t\t\t<xsl:if test=\"DataHolding/DataSet/File[URI='" + valuesF[i] + "']\">\n");
                d1.write("\t\t\t\t<xsl:element name='record'>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='name'>\n");
                
                d1.write("\t\t\t\t\t\t<xsl:value-of select=\"DataHolding/DataSet/File[URI='" + valuesF[i] + "']/./DataName\" />\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='url'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>"+valuesF[i]+"</xsl:text>\n");
                //d1.write("<xsl:text>www.dl.ac.uk"+i+"</xsl:text>");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='assStudy'>\n");
                d1.write("\t\t\t\t\t\t<xsl:value-of select=\"*/StudyName\" />\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='type'>\n");
                d1.write("\t\t\t\t\t\t<xsl:text>File</xsl:text>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='startyear'>\n");
                d1.write("\t\t\t\t\t\t<xsl:value-of select=\"DataHolding/DataSet/File[URI='" + valuesF[i] + "']/../LogicalDescription/Condition[ParamName='START_YEAR']/./ParamValue\"/>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='startmonth'>\n");
                d1.write("\t\t\t\t\t\t<xsl:value-of select=\"DataHolding/DataSet/File[URI='" + valuesF[i] + "']/../LogicalDescription/Condition[ParamName='START_MONTH']/./ParamValue\"/>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='startday'>\n");
                d1.write("\t\t\t\t\t\t<xsl:value-of select=\"DataHolding/DataSet/File[URI='" + valuesF[i] + "']/../LogicalDescription/Condition[ParamName='START_DAY']/./ParamValue\"/>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='endyear'>\n");
                d1.write("\t\t\t\t\t\t<xsl:value-of select=\"DataHolding/DataSet/File[URI='" + valuesF[i] + "']/../LogicalDescription/Condition[ParamName='STOP_YEAR']/./ParamValue\"/>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='endmonth'>\n");
                d1.write("\t\t\t\t\t\t<xsl:value-of select=\"DataHolding/DataSet/File[URI='" + valuesF[i] + "']/../LogicalDescription/Condition[ParamName='STOP_MONTH']/./ParamValue\"/>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='endday'>\n");
                d1.write("\t\t\t\t\t\t<xsl:value-of select=\"DataHolding/DataSet/File[URI='" + valuesF[i] + "']/../LogicalDescription/Condition[ParamName='STOP_DAY']/./ParamValue\"/>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                
                d1.write("\t\t\t\t\t<xsl:attribute name='minlon'>\n");
                d1.write("\t\t\t\t\t\t<xsl:value-of select=\"DataHolding/DataSet/File[URI='" + valuesF[i] + "']/../LogicalDescription/Condition[ParamName='MIN_LON']/./ParamValue\"/>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='maxlon'>\n");
                d1.write("\t\t\t\t\t\t<xsl:value-of select=\"DataHolding/DataSet/File[URI='" + valuesF[i] + "']/../LogicalDescription/Condition[ParamName='MAX_LON']/./ParamValue\"/>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='minlat'>\n");
                d1.write("\t\t\t\t\t\t<xsl:value-of select=\"DataHolding/DataSet/File[URI='" + valuesF[i] + "']/../LogicalDescription/Condition[ParamName='MIN_LAT']/./ParamValue\"/>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='maxlat'>\n");
                d1.write("\t\t\t\t\t\t<xsl:value-of select=\"DataHolding/DataSet/File[URI='" + valuesF[i] + "']/../LogicalDescription/Condition[ParamName='MAX_LAT']/./ParamValue\"/>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='minalt'>\n");
                d1.write("\t\t\t\t\t\t<xsl:value-of select=\"DataHolding/DataSet/File[URI='" + valuesF[i] + "']/../LogicalDescription/Condition[ParamName='MIN_ALTITUDE']/./ParamValue\"/>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t\t<xsl:attribute name='maxalt'>\n");
                d1.write("\t\t\t\t\t\t<xsl:value-of select=\"DataHolding/DataSet/File[URI='" + valuesF[i] + "']/../LogicalDescription/Condition[ParamName='MAX_ALTITUDE']/./ParamValue\"/>\n");
                d1.write("\t\t\t\t\t</xsl:attribute>\n");
                d1.write("\t\t\t\t</xsl:element>\n");
                d1.write("\t\t\t</xsl:if>\n");
                d1.write("\t\t</xsl:for-each>\n");
            }
            d1.write("\t</xsl:element>\n");
            d1.write("</xsl:template>\n");
            d1.write("</xsl:stylesheet>\n");
            d1.close();
            shop =new File(workingDir+File.separator+"profiles"+File.separator+"shop"+dn+".xml");
            XSLTransformer.transformFiletoFile(new File(workingDir+File.separator+"profiles"+File.separator+dn+"3.xml"),temp2,shop);
            
            org.w3c.dom.Document doc1 = XML_DOMBuilder.parse(shop);
            
            
            // Saver.save(n,new File(workingDirUser+File.separator+name+File.separator+"cera.xml"));
            
            org.w3c.dom.NodeList list = doc1.getElementsByTagName("record");
            int r = list.getLength();
            
            //for each element get all info from it
            for(int y=0;y<r;y++){
                org.w3c.dom.Element el = (org.w3c.dom.Element)list.item(y);
                String w = (String)el.getAttribute("name");
                String w1 = (String)el.getAttribute("url");
                String typeOfData = (String)el.getAttribute("type");
                String sy = (String)el.getAttribute("startyear");
                String sm = (String)el.getAttribute("startmonth");
                String sd = (String)el.getAttribute("startday");
                String ey = (String)el.getAttribute("endyear");
                String em = (String)el.getAttribute("endmonth");
                String ed = (String)el.getAttribute("endday");
                
                String minlon = (String)el.getAttribute("minlon");
                String maxlon = (String)el.getAttribute("maxlon");
                String minlat = (String)el.getAttribute("minlat");
                String maxlat = (String)el.getAttribute("maxlat");
                String minalt = (String)el.getAttribute("minalt");
                String maxalt = (String)el.getAttribute("maxalt");
                
                String study = (String)el.getAttribute("assStudy");
                
                String urls = (String)el.getAttribute("urls");
                
                //remove spaces out of the study
                int no = study.length();
                StringBuffer h = new StringBuffer();
                for(int i = 0; i<no; i++){
                    if( study.charAt(i) == ' ') h.append("%20");
                    else h.append(study.charAt(i));
                }
                study = h.toString();
                
                //remove spaces out of the url
                int no1 = w1.length();
                StringBuffer h1 = new StringBuffer();
                for(int i = 0; i<no1; i++){
                    if( w1.charAt(i) == ' ') h1.append("%20");
                    else if(w1.charAt(i) == '\\') h1.append("/");
                    else h1.append(w1.charAt(i));
                }
                w1 = h1.toString();
                
                
                Record u =new Record(w,w1,new Date().toString(),study,urls,typeOfData,sy,sm,sd,ey,em,ed, minlon,maxlon,minalt,maxalt,minlat,maxlat,"" );
                
                db11.add(w1,u);
                
            }
            //session.setAttribute("db", db11);
            d1.close();
            temp2.delete();
            shop.delete();
            
        } catch(Exception e){
            try{
                d1.close();
            }
            catch(Exception ignore){}
            temp2.delete();
            shop.delete();
            logger.warn("AddToCart exception, creating xml",e);
            
        }
        return db11;
    }
}
