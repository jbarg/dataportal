package uk.ac.dl.myproxy;


/** Provides a simple interface to a standard configuration wizard module.
 *
 */
public interface ConfigurationModule {

    public void saveSettings() ;

    public boolean verifySettings();
    
    public boolean verifyCertificate();
    
    public void download();

}

