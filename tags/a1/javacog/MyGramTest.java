package javacog;

import org.globus.gram.*;
import org.globus.util.deactivator.*;
import org.globus.security.*;
import org.globus.io.gass.server.*;

import java.security.*;
import java.security.cert.*;
import java.io.*;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.border.*;

/** Class to demonstate the GRAM implementation of the COG kit
 *
 * @author daniel
 * @created January 8, 2001
 */
public class MyGramTest {
    //Java representation of standard Globus proxy file
    GlobusProxy gp;
    MyGramTest mgt;

    /** When called from the command line
     * @param args The parameters passed from the command line.
     */    
    public static void main(String args[]) {
        new MyGramTest();
        /**
        if ( args.length < 3 ){
            new MyGramTest();
            args = new String[2];
            args[0] = getInput( "Enter contact string:" );
            args[1] = getInput( "Enter executable:" );
            if ( args[0].length() == 0 || args[1].length() == 0 ){
                System.out.println( "Usage: java MyGramTest <contact string> <executable>" );
                System.exit(0);
            }
        }
	new MyGramTest( args[0], args[1] );
        */
        System.exit(0);
    }
    
    /** Exit with error
     * @param e Error message
     */    
    public void exitWithError( String e ){
        System.err.println( e );
        System.exit(1);
    }
        
    GassServer gs = null;
    /** Return a Gass server
     * @return The GassServer object
     *
     */    
    public GassServer getGassServer(){
        try{
            gs = new GassServer();
            myOut.println( "Using GASS URL:" + gs.getURL() );
            Deactivator.registerDeactivation( new DeactivationHandler(){
                public void deactivate(){
                    if ( gs != null ) gs.shutdown();
                }
            } );
            gs.registerJobOutputStream( "out", myOut );
            gs.registerJobOutputStream( "err", myOut );
            myOut.println("Registered cout and cerr");
        }
        catch( Exception e ){
            myOut.println( e.toString() );
            e.printStackTrace();
            Deactivator.deactivateAll();
            System.exit(1);
        }
        return gs;
    }

    PrintStream myOut;
    JTextArea ota;
    /** Constructor
     */    
    public MyGramTest(){
	mgt = this;
        myOut = new PrintStream( new OutputStream(){
            public void close(){}
            public void flush(){}
            public void write( byte b[] ){
                ota.append( new String ( b ) );
            }
            public void write( byte b[], int off, int len ){
                ota.append( new String( b, off, len ) );
            }
            public void write( int b ){
                ota.append( new String( new byte[]{ (byte)b } ) );
            }
        });
                    
        JFrame of = new JFrame();
        ota = new JTextArea(40,40);
        ota.append( this.getClass().getName() + " output\n" );
        JScrollPane jsp = new JScrollPane( ota );
        of.getContentPane().add( jsp, BorderLayout.CENTER );

        JButton jb = new JButton( "Done" );
        jb.addActionListener( new ActionListener(){
            public void actionPerformed( ActionEvent ae ){
                System.exit(0);
            }
        });
        of.getContentPane().add( jb, BorderLayout.SOUTH );
        
        of.addWindowListener( new WindowAdapter(){
            public void windowClosing( WindowEvent we ){
                we.getWindow().setVisible( false );
                we.getWindow().dispose();
            }
        });
        of.pack();
        of.setVisible( true );
        
        String contactString = getInput("Enter contact string:" );
        String executable = getInput( "Enter executable" );
        
	//Check for a valid certificate at this stage...
	if ( GlobusProxy.defaultUserProxyExists() ){
            myOut.println( "Default user proxy file exists." );
	    try{
		gp = GlobusProxy.getDefaultUserProxy();
            } catch( GlobusProxyException gpe ){
		myOut.println( "Exception : " + gpe );
		gpe.printStackTrace( myOut );
		myOut.println( "Going to grid-proxy-init..." );
		gridProxyInit();
	    }
	} else {
            myOut.println( "Default user proxy file does not exist." );
	    myOut.println( "Going to grid-proxy-init..." );
	    gridProxyInit();
	}
        
        GassServer gs = getGassServer();
        if ( gs == null ) exitWithError( "Unable to create Gass Server " );
        
        StringBuffer rslsb = new StringBuffer();
        
        //Perform RSL substitution to get results back
        try{
        rslsb.append( "&(rsl_substitution=(GLOBUSRUN_GASS_URL " + gs.getURL() + "))" );
        }
        catch( java.net.UnknownHostException uhe ){
            this.exitWithError( "gs.getURL() failed" );
        }
        rslsb.append( "(executable=" + executable + ")" );
        rslsb.append( "(stderr=$(GLOBUSRUN_GASS_URL)/dev/stderr)" );
        rslsb.append( "(stdout=$(GLOBUSRUN_GASS_URL)/dev/stdout)" );
        
        //Represents a simple gram job. The job can be submitted, cancelled, sent
        //signals and callbacks can be registered to it.
	GramJob gj;
	gj = new GramJob( rslsb.toString() );
        gj.addListener( new GramJobListener(){
            public void statusChanged(GramJob job) {
                myOut.println( "Job: " + job.getRSL() + "\nStatus: " + job.getStatusAsString() );
                switch( job.getStatus() ){
                    case GramJob.STATUS_FAILED:
                        myOut.println( "Problem..." );
                        myOut.println( "Error number = " + job.getError() );
                    case GramJob.STATUS_DONE:
                        Deactivator.deactivateAll();
                        break;
                    default:
                        myOut.println( "Doing nothing..." );
                }
            }
        });
              
        if ( gj.getCredentials() == null ){
            myOut.println( "Setting gj credentials to " + gp );
            gj.setCredentials( gp );
        }
        
        myOut.println( "gj.getCredentials()=" + gj.getCredentials() );
       
	try {
            myOut.println( "First try to ping the gatekeeper" );
            //Contact string - www.globus.org/gram/client/resource_manager_contact.html
            Gram.ping( gp, contactString );
            myOut.println( "Ping successfull!" );
        }
        catch( GramException ge ){
            myOut.println( "Gram exception thrown, ping operation failed." );
        }
        catch( GlobusProxyException gpe ){
            myOut.println( gpe );
            gpe.printStackTrace( System.err );
        }
        try{
            myOut.println( "gj = " + gj );
            gj.request( contactString );
            Thread.currentThread().sleep(10000);
            myOut.println( "Returned from Gram.request..." );
	}
        catch( InterruptedException ie ){
            System.err.println("Interrupted Exception");
        }
	catch (GramException ge) {
	    //error( "Error....", e );
            System.err.println( ge );
            ge.printStackTrace();
	}
        catch( GlobusProxyException gpe ){
            System.err.println( gpe );
            gpe.printStackTrace();
        }
    }
    
    private String tempString;
    /** Read a line of input
     * @return The text entered
     * @param prompt The prompt for the input
     */    
    public String getInput( String prompt ){
        JFrame f = new JFrame( prompt );
        f.addWindowListener( new WindowAdapter(){
            public void windowClosing( WindowEvent we ){
                Window w = we.getWindow();
                w.setVisible(false);
                w.dispose();
                synchronized( mgt ){
                    mgt.notify();
                }
            }
        });
        JTextField jtf = new JTextField( 40 );
        jtf.addActionListener( new ActionListener(){
            public void actionPerformed( ActionEvent ae ){
                tempString = ( (JTextField)( ae.getSource() ) ).getText();
                synchronized( mgt ){
                    mgt.notify();
                }
            }
        });
        f.getContentPane().add( jtf, BorderLayout.CENTER );
        f.pack();
        f.setVisible(true);
        try{
            synchronized( this ){
                wait();
            }
        }
        catch( InterruptedException ie ){
            //Contine
        }
        f.setVisible(false);
        f.dispose();
        return new String( tempString );
    }
        
    /** Perform a ''gridproxyinit``
     */    
    public void gridProxyInit(){
	//Need to get passphrase now...
	JFrame f = new JFrame( "GridProxyInit..." );
	f.addWindowListener( new WindowAdapter(){
		public void windowClosing( WindowEvent we ){
		    we.getWindow().setVisible(false);
                    we.getWindow().dispose();
		}
	    });
	f.getContentPane().add( new JLabel( "Enter Globus pass phrase" ), BorderLayout.NORTH );
	JPasswordField tf = new JPasswordField( 40 );
	tf.addActionListener( new ActionListener(){
		public void actionPerformed( ActionEvent ae ){
		    String password = new String( ( (JPasswordField)ae.getSource() ).getPassword() );
		    if ( password.length() == 0 ) System.exit( 0 );
		    //Remove pass phrase window
		    JFrame jf = (JFrame)( ( (JComponent)ae.getSource() ).getParent()
					  .getParent().getParent().getParent() );
		    jf.setVisible( false );
		    jf.getContentPane().removeAll();
		    jf.setTitle( "Performing GridProxyInit..." );

		    JProgressBar jpb = new JProgressBar( 0, 5 );
		    jpb.setString( "Performing GridProxyInit..." );
		    jpb.setPreferredSize( new Dimension( 300,100 ) );
		    jpb.setStringPainted( true );
		    jpb.setBorder( new EmptyBorder( 20, 20, 20, 20 ) );
		    jf.getContentPane().add( jpb, BorderLayout.CENTER );
		    jf.doLayout();
		    jf.setSize( jpb.getPreferredSize() );
		    jf.setVisible( true );
		    Thread.currentThread().yield();

		    jpb.setString( "Reading user key file..." );
		    jpb.setValue( 1 );
		    myOut.println( "Reading user key file..." );
		    try{
			SSLeayKeyHolder key = new SSLeayKeyHolder( Config.getUserKeyFile() );
			
			jpb.setString( "Decrypting key with pass phrase..." );
			jpb.setValue( 2 );
			myOut.println( "Decrypting key with pass phrase..." );
			key.decrypt( password );
			
			jpb.setString( "Generating proxy..." );
			jpb.setValue( 3 );
			myOut.println( "Generating proxy..." );
			gp = CertUtil.createProxy( CertUtil.loadCert( Config.getUserCertFile() ),
						   key.getPrivateKey(), 512, 12, false );
			//Save Proxy File for other globus utils to use....

			jpb.setString( "Saving proxy file..." );
			jpb.setValue( 4 );
			myOut.println( "Saving proxy file..." );
			gp.save( Config.getProxyFile() );

			jpb.setString( "Complete." );
			jpb.setValue( 5 );
			myOut.println( "Complete." );
		    } catch( IOException ioe ){
			error( "IO exception", ioe );
		    } catch( GeneralSecurityException gse ){
			error( "Security exeption", gse );
		    } catch( GlobusProxyException gpe ){
			error( "Globus Proxy Exception", gpe );
		    }
		    jf.setVisible( false );
		    synchronized( mgt ){
			mgt.notify();
		    }
		}
	    });
	
	f.getContentPane().add( tf, BorderLayout.CENTER );
	f.pack();
	f.setVisible( true );
	try{
	    synchronized( this ){
		wait();
	    }
	} catch( InterruptedException ie ){}
	f.setVisible( false );
    }
    //org.globus.security.CertUtil.checkCertificate( new java.security.cert.X509Certificate() );
    
    void error( String message, Exception e ){
	JDialog jd = new JDialog();
	jd.setTitle( "Error!" );
	JLabel errorLabel = new JLabel( message, 
					UIManager.getLookAndFeelDefaults().getIcon( "OptionPane.errorIcon" ),
					JLabel.CENTER );
	errorLabel.setBorder( new EmptyBorder( 5, 5, 5, 5 ) );
	jd.getContentPane().add( errorLabel, BorderLayout.NORTH );
	JTextArea ta = new JTextArea( 20, 40 );
	PipedInputStream pis = new PipedInputStream();
	BufferedReader br = new BufferedReader( new InputStreamReader( pis ) ); 
	try{
	    e.printStackTrace( new PrintStream( new PipedOutputStream( pis ) ) );
	    char[] buffer = new char[81];
	    buffer[80] = '\n';
	    String line;
	    while ( br.ready() ){
		br.read( buffer, 0, 80 );
		ta.append( new String( buffer ) );
	    }
	}
	catch( IOException ioe ){
	    System.err.println( "IOException" );
	}
	JButton ok = new JButton( "OK" );

	ok.addActionListener( new ActionListener(){
		public void actionPerformed( ActionEvent ae ){
		    synchronized( mgt ){
			mgt.notify();
		    }
		}
	    });
	
	jd.getContentPane().add( new JScrollPane( ta ), BorderLayout.CENTER );
	jd.getContentPane().add( ok, BorderLayout.SOUTH );
	jd.pack();
	jd.show();
	try{
	    synchronized( this ){
		this.wait();
	    }
	} catch( InterruptedException ie ){}
	jd.setVisible( false );
    }
}