/*
 * TestAll.java
 *
 * Created on 07 March 2007, 14:44
 *
 * JUNIT Test Suite that creates a connection to a specified ICAT3 JUNIT
 * test database schema and executes a number of scripts in order to
 * initialise the database for the JUNIT tests that are to be
 * subsequently called.  Add each JUNIT class (that is to be run) to the
 * @Suite.SuiteClasses annotation.
 *
 * @author df01
 * @version 1.0
 */
package uk.ac.dl.dp5;

import junit.framework.JUnit4TestAdapter;
import junit.framework.Test;
import org.junit.runner.RunWith;
import org.junit.runners.Suite;


@RunWith(Suite.class)
@Suite.SuiteClasses({
    
})
public class TestAll {
    
    
    public static Test suite() {
                
        //dp5 unit test
        ExecuteDatabaseScript script = new ExecuteDatabaseScript("jdbc:oracle:thin:@(DESCRIPTION=(ADDRESS=(HOST=elektra.dl.ac.uk)(PROTOCOL=tcp)(PORT=1521))(CONNECT_DATA=(SID=minerva2)))", "dp5_unit_test", "fl3nd1g");
        
        script.execute("database/drop.sql", ";");
        script.execute("database/create-pop.sql", ";");
        
        return new JUnit4TestAdapter(TestAll.class);
    }
}
