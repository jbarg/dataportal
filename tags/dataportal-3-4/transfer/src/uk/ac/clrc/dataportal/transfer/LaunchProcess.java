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
            
            public boolean isError = true;
            public String error = null;
            public String getError(){
                return error;
            }
            
            public void run() {
                if(errorStream){
                    try{
                        String line;
                        StringBuffer buff = new StringBuffer();
                        System.out.println("This is from error stream");
                        while((line=br.readLine())!=null) {
                            
                            
                            buff.append(line);
                            System.out.println(line);
                            
                            // do nothing but read the output stream to stop the process from blocking
                        }
                        if(buff.toString().equals("")) isError = false;
                        if(!buff.toString().equals("")) isError = true;
                        error = buff.toString();
                        //System.out.println("This is the buff ="+buff+"= and the isError is "+isError+" and test "+buff.equals(""));
                        
                    }catch(Exception e){
                        error = e.getMessage();
                        isError = true;
                        e.printStackTrace();}
                }
                try{
                    br.close();
                }
                catch(Exception e){}
            }
        }
        
        System.out.println("Executing command from Launch process: " + arg);
        System.out.println("");
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
        try{
            Thread.sleep(2000);
        }
        catch(Exception r){}
        
        //check if there is a error
        boolean isError = e.isError;
        System.out.println("Error is "+isError);
        
        if(isError) {
            String error = e.getError();
            //System.out.println(e.getError());
            int i = 0;
            while(e.getError() == null){
                
                try{
                    Thread.sleep(1000);
                    System.out.println("waiting "+i);
                    i = i+1000;
                    error = e.getError();
                }
                catch(Exception r){}
                if(i > 4000){
                    error = "Unknown";
                    break;
                }
            }
            System.out.println("Returning value "+error);
            return error;
        }
        else return "true";
    }
}
