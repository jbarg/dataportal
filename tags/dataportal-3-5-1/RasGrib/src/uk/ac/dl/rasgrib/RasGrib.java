package uk.ac.dl.rasgrib;

/*
 * RasGribServlet.java
 *
 * Created on 24 May 2002, 09:36
 */

import java.io.*;
import java.util.* ;
import java.sql.*;
import rasj.*;
import rasj.odmg.*;
import org.odmg.*;
import org.w3c.dom.*;


/**
 *
 * @author  Glen Drinkwater.
 * @version
 */
public class RasGrib {

    //for rasdaman
    DBag resultBag = null;
    RasGMArray result = null;
    Transaction myTa = null;
    Database myDb = null;
    OQLQuery myQu = null;

    //for RasInfo
    Connection myConn = null;
    Statement myStmt = null;
    double[] rasInfo = new double[15];

    //* The machine on which the database resides
    public  static String s_hostName; 

    //* The TNS listener port
    public  static String s_portNumber; 

    //*The database name (SID)
    public  static String s_databaseSID;

    //* The database User ID
    public  static String s_userName; 

    //*The database user password
    public  static String s_password; 

    //public static String s_db_name;
    public static String s_rasdaman_port;
    public static String s_rasdaman_username;
    public static String s_rasdaman_password;

    // Initializes the servlet. and the connections to the databases.
    public String init(String max_lat,String min_lat,String max_lon,String min_lon,String max_alt,String min_alt,String min_year,String max_year,String min_month,String max_month,String min_day,String max_day,String collection,String operation_mode) throws Exception {


        try{
            Properties prop = new Properties();
            prop.load(new FileInputStream(Config.getContextPath()+"RasGrib.conf"));

            s_hostName = prop.getProperty("host_name");
            s_portNumber = prop.getProperty("jdbc_listener_port");
            s_databaseSID = prop.getProperty("oracle_sid");
            //s_db_name = prop.getProperty("rasdaman_db_name");
            s_rasdaman_port = prop.getProperty("rasdaman_port");
            s_rasdaman_username = prop.getProperty("rasdaman_username");
            s_rasdaman_password = prop.getProperty("rasdaman_password");
            s_userName = prop.getProperty("user_name(cera)");
            s_password = prop.getProperty("user_password(cera)");
        }
        catch(Exception e){
            throw new Exception("Configuration settings not found");
            //System.out.println("Using default settings");
        }



        try{
            //String param = request.getParameter("ras_collection");
            getRasInfoBounds(collection);

            Implementation myApp = new RasImplementation("http://"+s_hostName+":"+s_rasdaman_port );
            ((RasImplementation)myApp).setUserIdentification(s_rasdaman_username,s_rasdaman_password);
            myDb = myApp.newDatabase();

            //System.out.println("Opening database ...");
            myDb.open("RASBASE",Database.OPEN_READ_ONLY);

            //System.out.println("Starting transaction ...");
            myTa = myApp.newTransaction();
            myTa.begin();

           // System.out.println("Retrieving MDDs ...");
            myQu = myApp.newOQLQuery();

            //System.out.println("where am i ...");


            UserParameters user = new UserParameters( max_lat, min_lat, max_lon, min_lon, max_alt, min_alt, min_year, max_year, min_month, max_month, min_day, max_day, collection,  operation_mode);
            //System.out.println("passing min alt "+rasInfo[12]);
            user.calculateMinLatIndex(rasInfo[0],rasInfo[1], rasInfo[12]);
            user.calculateMaxLatIndex(rasInfo[0], rasInfo[1], rasInfo[12]);
            user.calculateMinLonIndex(rasInfo[2], rasInfo[3], rasInfo[13]);
            user.calculateMaxLonIndex(rasInfo[2], rasInfo[3], rasInfo[13]);
            user.calculateMinAltIndex(rasInfo[4], rasInfo[5], rasInfo[14]);
            user.calculateMaxAltIndex(rasInfo[4], rasInfo[5], rasInfo[14]);
            user.calculateMinTimeIndex(rasInfo[6], rasInfo[8], rasInfo[10]);
            user.calculateMaxTimeIndex(rasInfo[7],rasInfo[9],rasInfo[11]);
            //user.calculateIndexForLatLonAltTime(rasInfo[0],rasInfo[1],rasInfo[2],rasInfo[3],rasInfo[4],rasInfo[5],   rasInfo[6],rasInfo[7],rasInfo[8],rasInfo[9],rasInfo[10],rasInfo[11],rasInfo[12],rasInfo[13],rasInfo[14]);


            String s = new String("[" + (int)(user.getMinLatIndex()) + ":" + (int)(user.getMaxLatIndex()) + "," + (int)(user.getMinLonIndex()) + ":" + (int)(user.getMaxLonIndex()) + "," + (int)(user.getMinAltIndex()) + ":" + (int)(user.getMaxAltIndex()) + "," + (int)(user.getMinTimeIndex()) + ":" + (int)(user.getMaxTimeIndex()) + "]");
            //System.out.println(s);
            //System.out.println("request is "+user.getOperationModeRequest());

            //Document doc = new Document("
            String response = null;
            //add the if from which request it is.
            if(user.getOperationModeRequest().equals("minimum")){
                //System.out.println("minumim");

                myQu.create("select min_cells(img" + s + ") from "+user.getRasCollectionRequest()+" as img");
                //System.out.println("select min_cells(img" + s + ") from "+user.getRasCollectionRequest()+" as img");
                resultBag = (DBag)myQu.execute();
                if(resultBag != null) {
                    Iterator it = resultBag.iterator();
                    while(it.hasNext()){
                        response =  String.valueOf(it.next());
                    }
                }


            }
            else if(user.getOperationModeRequest().equals("maximum")){

                myQu.create("select max_cells(img" + s + ") from "+user.getRasCollectionRequest()+" as img");
                resultBag = (DBag)myQu.execute();
                if(resultBag != null) {
                    Iterator it = resultBag.iterator();
                    while(it.hasNext()){
                            response =  String.valueOf(it.next());
                    }
                }


            }
            else if(user.getOperationModeRequest().equals("average")){

                myQu.create("select avg_cells(img" + s + ") from "+user.getRasCollectionRequest()+" as img");
                resultBag = (DBag)myQu.execute();
                if(resultBag != null) {
                    Iterator it = resultBag.iterator();
                    while(it.hasNext()){
                              response =  String.valueOf(it.next());
                    }
                }


            }
           /* else if(user.getOperationModeRequest().equals("binary_data")){
                //response.setContentType("application/octet-stream");
                BufferedOutputStream strOut = new BufferedOutputStream(response.getOutputStream());
                //ServletOutputStream outS = response.getOutputStream();

                byte[] binary_data = null;
                myQu.create("select img" + s + " from "+user.getRasCollectionRequest()+" as img");
                resultBag = (DBag)myQu.execute();
                if (resultBag != null) {
                    Iterator iter = resultBag.iterator();
                    while (iter.hasNext()) {
                        result = (RasGMArray)iter.next();
                        strOut.write(result.getArray(),0,result.getArray().length);
                    }
                }
            }*/
            myTa.commit();
            myDb.close();
            return response;
        }
        catch(Exception e){
            if(myTa != null) myTa.abort();


            try {
                //System.out.println("Try to close the database ... because of error : "+e.toString());
                if(myDb != null) myDb.close();
            }
            catch ( org.odmg.ODMGException exp ) {
                //System.out.println("Could not close the database: " + exp.getMessage());

                //throw new RasGribException(new String("" + exp.getMessage() + "")) ;
            }

           // System.out.println("closing");
               throw e;
        }

    }

    

    public void getRasInfoBounds(String param) throws Exception{
        try{
            DriverManager.registerDriver(new oracle.jdbc.driver.OracleDriver());

            String l_dbConnectString = "(DESCRIPTION=(ADDRESS=(HOST="+s_hostName+")"+
            "(PROTOCOL=tcp)(PORT="+s_portNumber+"))"+
            "(CONNECT_DATA=(SID="+s_databaseSID+")))";

            // Step 2:  Get a connection
            myConn = DriverManager.getConnection(
            "jdbc:oracle:thin:@"+l_dbConnectString, s_userName, s_password);

            // Step 3:  Create and execute the SQL statement
            myStmt = myConn.createStatement();

            String s = "PARAM_130";
            String query = "select MIN_LAT,MAX_LAT,MIN_LON,MAX_LON,MIN_ALT,MAX_ALT,START_YEAR,STOP_YEAR,START_MONTH,STOP_MONTH,START_DAY,STOP_DAY,NUM_POINTS_LAT,NUM_POINTS_LON,NUM_POINTS_ALT from RAS_INFO WHERE RAS_COLLECTION  ='" + param + "'";
            ResultSet rs = myStmt.executeQuery(query);
            ResultSetMetaData rsmd =  rs.getMetaData();
           // System.out.println(rsmd.getColumnCount());
            //System.out.println(rs);
            while(rs.next()){
                int i = 1;
                rasInfo[0] = Double.valueOf(rs.getString(1)).doubleValue();
                rasInfo[1] = Double.valueOf(rs.getString(2)).doubleValue();
                rasInfo[2] = Double.valueOf(rs.getString(3)).doubleValue();
                rasInfo[3] = Double.valueOf(rs.getString(4)).doubleValue();
                rasInfo[4] = Double.valueOf(rs.getString(5)).doubleValue();
                rasInfo[5] = Double.valueOf(rs.getString(6)).doubleValue();
                rasInfo[6] = Double.valueOf(rs.getString(7)).doubleValue();
                rasInfo[7] = Double.valueOf(rs.getString(8)).doubleValue();
                rasInfo[8] = Double.valueOf(rs.getString(9)).doubleValue();
                rasInfo[9] = Double.valueOf(rs.getString(10)).doubleValue();
                rasInfo[10] = Double.valueOf(rs.getString(11)).doubleValue();
                rasInfo[11] = Double.valueOf(rs.getString(12)).doubleValue();
                rasInfo[12] = Double.valueOf(rs.getString(13)).doubleValue();
                rasInfo[13] = Double.valueOf(rs.getString(14)).doubleValue();
                rasInfo[14] = Double.valueOf(rs.getString(15)).doubleValue();
            }
            rs.close();
            myStmt.close();
            myConn.close();
        }
        catch(Exception e){
            //System.out.println(" from get bounds "+e.toString());
            throw new RasGribException("cannot open database");
        }
    }
}
