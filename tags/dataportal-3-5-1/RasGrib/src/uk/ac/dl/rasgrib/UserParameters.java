package uk.ac.dl.rasgrib;
import javax.servlet.*;
import javax.servlet.http.*;
import java.sql.*;
import java.io.*;
import java.util.* ;

class UserParameters {
    
    
    int MONTHTAB[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31,30, 31};
    int BINARY_DATA_LENGTH_TYPE;
    int HOURS_PER_DAY;
    int REFERENCE_YEAR;
    int REFERENCE_MONTH;
    int REFERENCE_DAY;
    double min_lat_content;
    double max_lat_content;
    double min_lon_content;
    double max_lon_content;
    double min_alt_content;
    double max_alt_content;
    double min_year_content;
    double max_year_content;
    double min_month_content;
    double max_month_content;
    double min_day_content;
    double max_day_content;
    double min_hour_content;
    double max_hour_content;
    double users[] = new double[15];
    String operation_mode_content;
    String binary_data_content;
    String minimum_content;
    String maximum_content;
    String average_content;
    String ras_collection_content;
    String TAKE_MIN_VALUE_FROM_RAS_INFO;
    String TAKE_MAX_VALUE_FROM_RAS_INFO;
    double TAKE_FROM_RAS_INFO;
    double min_lat_index;
    double max_lat_index;
    double min_lon_index;
    double max_lon_index;
    double min_alt_index;
    double max_alt_index;
    double min_time_index;
    double max_time_index;
    double total_points_lat;
    double total_points_lon;
    double diff_user_min_lat;
    double diff_user_max_lat;
    double diff_user_min_lon;
    double diff_user_max_lon;
    double points_division_latitude;
    double points_division_longitude;
    
    /*public double  getMinLatRequest() {
        return users[0];
    }
     
    public double getMaxLatRequest() {
        return users[1];
    }
     
    public double getMinLonRequest() {
        return users[2];
    }
     
    public double getMaxLonRequest() {
        return users[3];
    }
     
    public double getMinAltRequest() {
        return users[4];
    }
     
    public double getMaxAltRequest() {
        return users[5];
    }
     
    public double getMinYearRequest() {
        return users[6];
    }
     
    public double getMaxYearRequest() {
        return users[7];
    }
     
    public double getMinMonthRequest() {
        return users[8];
    }
    public double getMaxMonthRequest() {
        return users[9];
    }
     
    public double getMinDayRequest() {
        return users[10];
    }
     
    public double getMaxDayRequest() {
        return users[11];
    }*/
    
    public String getOperationModeRequest() {
        return operation_mode_content;
    }
    
    public String getBinaryDataRequest() {
        return binary_data_content;
    }
    
    public String getMinimumRequest() {
        return minimum_content;
    }
    
    public String getMaximumRequest() {
        return maximum_content;
    }
    
    public String getAverageRequest() {
        return average_content;
    }
    
    public String getRasCollectionRequest() {
        return ras_collection_content;
    }
    
    public UserParameters(String max_lat,String min_lat,String max_lon,String min_lon,String max_alt,String min_alt,String min_year,String max_year,String min_month,String max_month,String min_day,String max_day,String collection, String operation_mode) throws Exception {
        
        BINARY_DATA_LENGTH_TYPE = 4;
        HOURS_PER_DAY = 24;
        REFERENCE_YEAR = 1;
        REFERENCE_MONTH = 1;
        REFERENCE_DAY = 1;
        min_lat_content = 0;
        max_lat_content = 0;
        min_lon_content = 0;
        max_lon_content = 0;
        min_alt_content = 0;
        max_alt_content = 0;
        min_year_content = 0;
        max_year_content = 0;
        min_month_content = 0;
        max_month_content = 0;
        min_day_content = 0;
        max_day_content = 0;
        min_hour_content = 0;
        max_hour_content = 6;
        operation_mode_content = "";
        binary_data_content = "";
        minimum_content = "";
        maximum_content = "";
        average_content = "";
        ras_collection_content = "";
        TAKE_MIN_VALUE_FROM_RAS_INFO = "This parameter will take the minimum value from its spatial domain when is used.";
        TAKE_MAX_VALUE_FROM_RAS_INFO = "This parameter will take the maximum value from its spatial domain when is used.";
        TAKE_FROM_RAS_INFO = 0x3b9ac9ff;
        min_lat_index = 0;
        max_lat_index = 0;
        min_lon_index = 0;
        max_lon_index = 0;
        min_alt_index = 0;
        max_alt_index = 0;
        min_time_index = 0;
        max_time_index = 0;
        total_points_lat = 0;
        total_points_lon = 0;
        diff_user_min_lat = 0;
        diff_user_max_lat = 0;
        diff_user_min_lon = 0;
        diff_user_max_lon = 0;
        points_division_latitude = 1.0F;
        points_division_longitude = 1.0F;
        //System.out.println("in userparam...");
        //parameter names
        String[] start = {"min_lat","max_lat","min_lon","max_lon","min_alt","max_alt","min_year","max_year","min_month","max_month","min_day","max_day","min_hour","max_hour"};
        //double[] user = {"min_lat_content","max_lat_content","min_lon_centent","max_lon_content","min_alt_content","max_alt_content","start_year_content","end_year_content","start_month_content","end_month_content","start_day_content","end_day_content"};
        
        
        
        try {
            min_lat_content = Double.parseDouble(min_lat);
        }
        catch(NullPointerException e){
            min_lat_content = TAKE_FROM_RAS_INFO;
        }
        catch(NumberFormatException e){
            throw new RasGribException("Invalid number for minimum latitude");
        }
        try{
            max_lat_content = Double.parseDouble(max_lat);
        }
        catch(NullPointerException e){
            max_lat_content = TAKE_FROM_RAS_INFO;
        }
        catch(NumberFormatException e){
            throw new RasGribException("Invalid number for maximum latitude");
        }
        try{
            min_lon_content = Double.parseDouble(min_lon);
        }
        catch(NullPointerException e){
            min_lon_content = TAKE_FROM_RAS_INFO;
        }
        catch(NumberFormatException e){
            throw new RasGribException("Invalid number for minimum longitude");
        }
        try{
            max_lon_content = Double.parseDouble(max_lon);
        }
        catch(NullPointerException e){
            max_lon_content = TAKE_FROM_RAS_INFO;
        }
        catch(NumberFormatException e){
            throw new RasGribException("Invalid number for maximum longitude");
        }
        try{
            min_alt_content = Double.parseDouble(min_alt);
        }
        catch(NullPointerException e){
            min_alt_content = TAKE_FROM_RAS_INFO;
        }
        catch(NumberFormatException e){
            throw new RasGribException("Invalid number for minimum longitude");
        }
        try{
            max_alt_content = Double.parseDouble(max_alt);
        }
        catch(NullPointerException e){
            max_alt_content = TAKE_FROM_RAS_INFO;
        }
        catch(NumberFormatException e){
            throw new RasGribException("Invalid number for minimum longitude");
        }
        try{
            min_year_content = Double.parseDouble(min_year);
        }
        catch(NullPointerException e){
            min_year_content = TAKE_FROM_RAS_INFO;
        }
        catch(NumberFormatException e){
            throw new RasGribException("Invalid number for minimum longitude");
        }
        try{
            max_year_content = Double.parseDouble(max_year);
        }
        
        catch(NullPointerException e){
            max_year_content = TAKE_FROM_RAS_INFO;
        }
        catch(NumberFormatException e){
            throw new RasGribException("Invalid number for minimum longitude");
        }
        try{
            min_month_content = Double.parseDouble(min_month);
        }
        catch(NullPointerException e){
            min_month_content = TAKE_FROM_RAS_INFO;
        }
        catch(NumberFormatException e){
            throw new RasGribException("Invalid number for minimum longitude");
        }
        try{
            max_month_content = Double.parseDouble(max_month);
        }
        catch(NullPointerException e){
            max_month_content = TAKE_FROM_RAS_INFO;
        }
        catch(NumberFormatException e){
            throw new RasGribException("Invalid number for minimum longitude");
        }
        try{
            min_day_content = Double.parseDouble(min_day);
        }
        catch(NullPointerException e){
            min_day_content = TAKE_FROM_RAS_INFO;
        }
        catch(NumberFormatException e){
            throw new RasGribException("Invalid number for minimum longitude");
        }
        try{
            max_day_content = Double.parseDouble(max_day);
        }
        catch(NullPointerException e){
            max_day_content = TAKE_FROM_RAS_INFO;
        }
        catch(NumberFormatException e){
            throw new RasGribException("Invalid number for minimum longitude");
        }
        try{
            operation_mode_content = operation_mode;
            if(operation_mode_content.equals("")){
                throw new RasGribException("No value present for 'operation_mode' parameter");
            }
            else if(!operation_mode_content.equals("minimum") && !operation_mode_content.equals("minimum") && !operation_mode_content.equals("maximum") && !operation_mode_content.equals("average") &&!operation_mode_content.equals("binary_data")){
                throw new RasGribException("Invalid operation mode");
            }
        }
        catch(NullPointerException nullpointerexception){
            throw new RasGribException("No operation mode present");
        }
        
        try{
            ras_collection_content = collection;
            if(ras_collection_content.equals("")){
                throw new RasGribException("No value present for 'operation_mode' parameter");
            }
        }
        catch(NullPointerException nullpointerexception){
            throw new RasGribException("No operation mode present");
        }
        
        
        
    }
    
    public double  getMinLatIndex() {
        return min_lat_index;
    }
    
    public double getMaxLatIndex() {
        return max_lat_index;
    }
    
    public double getMinLonIndex() {
        return min_lon_index;
    }
    
    public double getMaxLonIndex() {
        return max_lon_index;
    }
    
    public double getMinAltIndex() {
        return min_alt_index;
    }
    
    public double getMaxAltIndex() {
        return max_alt_index;
    }
    
    public double getMinTimeIndex() {
        return min_time_index;
    }
    
    public double getMaxTimeIndex() {
        return max_time_index;
    }
    
    
       /* public boolean doNotDownloadThisBinaryData() throws RasGribException {
            boolean flag = false;
            int i = ((max_lat_index - min_lat_index) + 1) * ((max_lon_index - min_lon_index) + 1) * ((max_alt_index - min_alt_index) + 1) * ((max_time_index - min_time_index) + 1) * BINARY_DATA_LENGTH_TYPE;
            int j = getIntValueFromString(requestInitializedInputParameter.getMaximumDownloadSizeValue());
            System.out.println("Download size from config file: " + j);
            System.out.println("Download size from client: " + i);
            if(i > j)
                flag = true;
            return flag;
        }*/
    
    
    
    public void calculateMinLatIndex(double i, double j, double k) throws RasGribException {
        //System.out.println("i "+i+", j "+j+" , k "+k);
        if(min_lat_content == TAKE_FROM_RAS_INFO)
            min_lat_content = i;
        else
            if(min_lat_content < i)
                throw new RasGribException("Out of bounds - min_lat cannot be less than " + i + "");
        //calculate the index
        // i is min from rasinfo
        // j is max from ras info
        // k is number of points  in range
        //min lat content is users input. interface the range upwards
        total_points_lat = j -i;
        points_division_latitude = total_points_lat / k;
        
        diff_user_min_lat = min_lat_content - i;
        min_lat_index = (double)((float)diff_user_min_lat / points_division_latitude);
        //System.out.println("min lat index "+min_lat_index);
    }
    
    public void calculateMaxLatIndex(double i, double j, double k) throws RasGribException {
        if(max_lat_content == TAKE_FROM_RAS_INFO)
            max_lat_content = j;
        else
            if(max_lat_content > j)
                throw new RasGribException(new String("Out of bounds - max_lat cannot be more than " + j + ""));
        
        points_division_latitude = total_points_lat / k;
        diff_user_max_lat = max_lat_content - i;
        
        max_lat_index = (double)((float)diff_user_max_lat / points_division_latitude);
        //System.out.println("max lat index "+max_lat_index);
    }
    
    public void calculateMinLonIndex(double i, double j, double k) throws RasGribException {
        if(min_lon_content == TAKE_FROM_RAS_INFO)
            min_lon_content = i;
        else
            if(min_lon_content < i)
                throw new RasGribException(new String("Out of bounds - min_lon cannot be less than " + i + ""));
        
        total_points_lon = j - i;
        
        points_division_longitude = total_points_lon / k;
        
        diff_user_min_lon = min_lon_content - i;
        
        min_lon_index = (double)((float)diff_user_min_lon / points_division_longitude);
        //System.out.println("min lon index "+min_lon_index);
        
    }
    
    public void calculateMaxLonIndex(double i, double j, double k) throws RasGribException {
        if(max_lon_content == TAKE_FROM_RAS_INFO)
            max_lon_content = j;
        else
            if(max_lon_content > j)
                throw new RasGribException(new String("Out of bounds - max_lon cannot be more than " + j + ""));
        
        points_division_longitude = total_points_lon / k;
        diff_user_max_lon = max_lon_content - i;
        
        max_lon_index = (double)((float)diff_user_max_lon / points_division_longitude);
        //System.out.println("max lon index "+max_lon_index);
        
    }
    
    public void calculateMinAltIndex(double i, double j, double k) throws RasGribException {
        //System.out.println("from calculateMinAltIndex; i="+i+" ,j="+j+", k="+k);
        //System.out.println("min alt from user is "+min_alt_content);
        if(min_alt_content == TAKE_FROM_RAS_INFO)
            min_alt_content = i;
        else
            if(min_alt_content < i)
                throw new RasGribException(new String("Out of bounds - min_alt cannot be less than " + i + ""));
        
        min_alt_index = getAltitudeIndexFromRasAltitudes(min_alt_content);
        
    }
    
    public void calculateMaxAltIndex(double i, double j, double k) throws RasGribException {
        if(max_alt_content == TAKE_FROM_RAS_INFO)
            max_alt_content = j;
        else
            if(max_alt_content > j)
                throw new RasGribException(new String("Out of bounds - max_alt cannot be more than " + j + ""));
        
        max_alt_index = getAltitudeIndexFromRasAltitudes(max_alt_content);
        
    }
    
    public double getAltitudeIndexFromRasAltitudes(double i) throws RasGribException {
        double j = 0;
        Connection myConn = null;
        try {
            DriverManager.registerDriver(new oracle.jdbc.driver.OracleDriver());
            
            
            
            // Step 2:  Get a connection
            myConn = DriverManager.getConnection(
            "jdbc:oracle:thin:@twister.badc.rl.ac.uk:1521:RAS805","cera","BaumannP");
            
            
            
            Statement statement = myConn.createStatement();
            String s1 = "SELECT MAX(ALT_INDEX) FROM RAS_ALTITUDES WHERE ALT_LEVEL <= " + i + " AND RAS_COLLECTION  ='" + ras_collection_content + "'";
            
            ResultSet resultset = statement.executeQuery(s1);
            //System.out.println(resultset);
            int k = 1;
            if(resultset.next()){
                
                //System.out.println("from alt "+(String)resultset);
                j = resultset.getDouble(k++);
                //System.out.println("alt  "+j);
            }
            resultset.close();
            statement.close();
            
            myConn.close();
        }
        catch(SQLException sqlexception) {
            
            throw new RasGribException("RequestUserParametersClass(): SQLException - " + sqlexception.getMessage());
        }
        catch(Exception classnotfoundexception) {
            
            throw new RasGribException("excpetion from getaltind "+classnotfoundexception.getMessage());
        }
        return j;
    }
    
    public boolean isShiftYear(double i) {
        boolean flag = false;
        if(i % 4 == 0) {
            if(i % 100 == 0 && i % 400 != 0)
                flag = false;
            else
                flag = true;
        } else {
            flag = false;
        }
        return flag;
    }
    
    public int M( int i, int j) {
        int k = 0;
        for(int l = 0; l < j - 1; l++) {
            k += MONTHTAB[l];
            if(isShiftYear(i) && l == 1)
                k++;
        }
        
        return k;
    }
    
    public int myDate(int i, int j, int k) {
        //System.out.println("date in is  "+i+", "+j+" ,"+k);
        int l = 0;
        int i1 = i;
        l = (i - 1) * 365 + M(i, j) + k;
        l += i1 / 4;
        l -= i1 / 100;
        l += i1 / 400;
        //System.out.println("returned value is "+ l);
        return l;
        
    }
    
    public int diffNrDays(double i, double j, double k, double l, double i1, double j1) {
        
        int k1 = 0;
        int l1 = 0;
        int i2 = 0;
        l1 = myDate((int)i, (int)j,(int)k);
        i2 = myDate((int)l, (int)i1, (int)j1);
        k1 = l1 - i2;
        if(k1 < 0)
            k1 *= -1;
        int s = ++k1;
        //System.out.println("diff number of days" +s);
        return s;
    }
    
    public int getInt(double d){
        int t = (int)(d);
        return t;
        
    }
    
    public void calculateMinTimeIndex(double i, double j, double k) throws RasGribException {
        
        if(min_year_content == TAKE_FROM_RAS_INFO)
            min_year_content = i;
        else
            if(min_year_content < i)
                throw new RasGribException(new String("Out of bounds - min_year cannot be less than " + i + ""));
        if(min_month_content == TAKE_FROM_RAS_INFO)
            min_month_content = j;
        else
            if(min_month_content < j)
                throw new RasGribException(new String("Out of bounds - min_month cannot be less than " + j + ""));
        if(min_day_content == TAKE_FROM_RAS_INFO)
            min_day_content = k;
        else
            if(min_day_content < k)
                throw new RasGribException(new String("Out of bounds - min_day cannot be less than " + k + ""));
        int diff = diffNrDays(min_year_content, min_month_content, min_day_content, REFERENCE_YEAR, REFERENCE_MONTH, REFERENCE_DAY);
        //System.out.println("");
        //System.out.println("diff is "+diff);
        //System.out.println("diff*24 "+(diff)*24);
        
        min_time_index = (double)(diffNrDays(min_year_content, min_month_content, min_day_content, REFERENCE_YEAR, REFERENCE_MONTH, REFERENCE_DAY) * HOURS_PER_DAY);
        //System.out.println("min_time_index is " +min_time_index);
        //min_time_index += min_hour_content;
    }
    
    public void calculateMaxTimeIndex(double i, double j, double k) throws RasGribException {
        
        if(max_year_content == TAKE_FROM_RAS_INFO)
            max_year_content = i;
        else
            if(max_year_content > i)
                throw new RasGribException(new String("Out of bounds - max_year cannot be more than " + i + ""));
        if(max_month_content == TAKE_FROM_RAS_INFO)
            max_month_content = j;
        else
            if(max_month_content > j)
                throw new RasGribException(new String("Out of bounds - max_month cannot be more than " + j + ""));
        if(max_day_content == TAKE_FROM_RAS_INFO)
            max_day_content = k;
        else
            if(max_day_content > k)
                throw new RasGribException(new String("Out of bounds - max_day cannot be more than " + k + ""));
        max_time_index = diffNrDays(max_year_content, max_month_content, max_day_content, REFERENCE_YEAR, REFERENCE_MONTH, REFERENCE_DAY) * HOURS_PER_DAY;
        //System.out.println("max_time_index is "+max_time_index);
        //max_time_index += max_hour_content;
    }
    
    public void calculateIndexForLatLonAltTime(double i, double j, double k, double l, double i1, double j1, double k1,
    double l1, double i2, double j2, double k2, double l2, double i3, double j3,
    double k3) throws RasGribException {
        
        calculateMinLatIndex(i, j, i3);
        calculateMaxLatIndex(i, j, i3);
        calculateMinLonIndex(k, l, j3);
        calculateMaxLonIndex(k, l, j3);
        calculateMinAltIndex(i1, j1, k3);
        calculateMaxAltIndex(i1, j1, k3);
        calculateMinTimeIndex(k1, i2, k2);
        calculateMaxTimeIndex(l1, j2, l2);
        
    }
}
