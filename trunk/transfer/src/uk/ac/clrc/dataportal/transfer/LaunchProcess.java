package uk.ac.clrc.dataportal.transfer;

import java.io.*;

public class LaunchProcess {
    public static String runCommand(String arg) throws Exception {
        
        class Threader implements Runnable {
            BufferedReader br;
            PrintWriter pw;
            private boolean errorStream = false;
            
            Threader(InputStream is, boolean error) {
                br = new BufferedReader(new InputStreamReader(is));
                errorStream = error;
            }
            
            public boolean isError = false;
            public String error = "";
            public String getError(){
                return error;
            }
            
            public void run() {
                try{
                    String line;
                    StringBuffer buff = new StringBuffer();
                    if(errorStream) System.out.println("This is from error stream");
                    while((line=br.readLine())!=null) {
                        buff.append(line);
                        if(errorStream)System.out.println(line);
                        // do nothing but read the output stream to stop the process from blocking
                    }
                    if(!buff.equals("")) isError = true;
                    error = buff.toString();
                    
                }catch(Exception e){
                    error = e.getMessage();
                    isError = true;
                    e.printStackTrace();}
            }
        }
        
        System.out.println("Executing command " + arg);
        
        Process p = Runtime.getRuntime().exec(arg);
        
        //Thread o = new Thread(new Threader(p.getInputStream()));
        //Thread e = new Thread(new Threader(p.getErrorStream()));
        Threader o = new Threader(p.getInputStream(), false);
        Threader e = new Threader(p.getErrorStream(), true);
        
        Thread to = new Thread(o);
        Thread te = new Thread(e);
        to.start();
        te.start();
        p.waitFor();
        
        //check if there is a error
        boolean isError = e.isError;
        if(isError) return e.getError();
        else return "true";
    }
}
