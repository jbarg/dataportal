package uk.ac.clrc.dataportal.srbcommands;

import java.io.*;

public class LaunchProcess {
    public static void runCommand(String arg) throws Exception {

        class Threader implements Runnable {
            BufferedReader br;
            PrintWriter pw;
            
            Threader(InputStream is) {
                br = new BufferedReader(new InputStreamReader(is));
            }
            
            public void run() {
                try{
                    String line;
                    while((line=br.readLine())!=null) {
                        // do nothing but read the output stream to stop the process from blocking
                    }
                }catch(Exception e){e.printStackTrace();}
            }
        }

        System.out.println("Executing command " + arg);
        
        Process p = Runtime.getRuntime().exec(arg);
        
        Thread o = new Thread(new Threader(p.getInputStream()));
        Thread e = new Thread(new Threader(p.getErrorStream()));
        
        o.start();
        e.start();
        p.waitFor();
    }
}
