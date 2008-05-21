/*
 * Test.java
 *
 * Created on 08 November 2006, 15:12
 *
 * To change this template, choose Tools | Template Manager
 * and open the template in the editor.
 */

package uk.ac.dl.dp.coreutil.util;

import java.util.Arrays;

/**
 *
 * @author gjd37
 */
public class Test {
    
    /** Creates a new instance of Test */
    public Test() {
    }
    
    /**
     * @param args the command line arguments
     */
    public static void main(String[] args) {
        // TODO code application logic here
        int[] a = new int[]{1,2,3};
        System.out.println(Arrays.asList(a));
        Integer[] b= new Integer[]{new Integer(1),new Integer(2),new Integer(3)};
        System.out.println(Arrays.asList(b));
        System.out.println(Arrays.asList(b).getClass());
        
        for(Object f : Arrays.asList(b) ){
            System.out.println(f.getClass());
        }
    }
    
}
