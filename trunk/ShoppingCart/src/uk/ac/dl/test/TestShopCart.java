/*
 * TestShopCart.java
 *
 * Created on 19 March 2003, 11:19
 */

package uk.ac.dl.test;

import org.apache.cactus.*;
import junit.framework.*;
import uk.ac.dl.cart.*;
import java.util.*;
import org.apache.cactus.util.HttpURLConnection;
import org.w3c.dom.*;
/**
 *
 * @author  gjd37
 */
public class TestShopCart extends TestCase{
    
    /** Creates a new instance of TestTransferServlet */
    public TestShopCart(String name) {
        super(name);
    }
    
    public static Test suite() {
        return new TestSuite(TestShopCart.class);
        
    }
    
    public void testGetCart(){
        try{
            ShopCart cart = new ShopCart("1");
            Element el = cart.getCart(new Integer(488));
            Assert.assertNotNull(el);
            System.out.println(el);
        }
        catch(Exception e){
            System.out.println(e);
        }
    }
}


