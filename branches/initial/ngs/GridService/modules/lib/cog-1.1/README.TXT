Java CoG Kit 
============

1. Introduction

   The Java CoG Kit provides a framework for interacting with the many
   Globus services that are part of the Globus Metacomputing
   Toolkit. The current distribution contains Java bindings to the
   GRAM, GASS, GridFTP, MyProxy and GARA components. The CoG Kit also
   contains an interface to the MDS -- the Globus LDAP-based
   information service. In addition, the Java CoG Kit provides various
   client tools and programs for accessing and communicating with
   remote Globus services. See Section 3 for more details.

2. Installation

   2.1 Requirements

       2.2.1 Java

	     The Java CoG Kit requires J2SE version 1.3.1 or higher
             (we recommend using J2SE 1.4.x). It is assumed that Java
             is properly installed and that the Java bin/ directory 
             has been included in the user's path.

       2.2.2 LDAP Service Provider

             If using J2SE 1.3.1, LDAP Service Provider 1.2.4 must
             also be installed. To install LDAP Service Provider 
             please download it from: http://java.sun.com/products/jndi/.
             Unpack the ldap-1_2_4.zip file into a temporary directory 
             and copy all the jar files in the "lib" directory of the 
             provider into the "lib" directory of the Java CoG Kit
             installation.
             Because of licensing issues we cannot distribute 
             these files with our software. 

       2.2.3 COG_INSTALL_PATH variable

	     The COG_INSTALL_PATH environment variable must be set
	     before any of the scripts included with the Java CoG Kit
	     will work. This variable points to where the Java CoG Kit 
             is installed. (For example: C:\cog-1.1)

   2.3 Configuration

       2.3.1 Prerequisites
  
	     The user's security certificate and the corresponding
	     private key are required. In addition, the CA certificate
	     that signed the user certificate is also required.

	     For more information about obtaining the certificates please 
	     visit: http://www-fp.globus.org/security/v1.1/certs.html

       2.3.2 Setup

	     Run the 'setup' script in the installation bin/ directory
	     to configure the CoG Kit. It will run a configuration
	     wizard which will prompt you for the location of your
	     user certificate, private key, and CA certificates.  The
	     wizard will create a configuration file required by the
	     Java CoG Kit tools and programs. After the wizard
	     completes the configuration process, you will be ready to
	     use the Java CoG Kit.

    2.4. Compatibility

      This version of the Java CoG Kit is NOT backwards compatible 
      with the previous versions.  The 1.1 version contains a totally
      new security library (GSS-API based) which required substantial
      changes to the security-related API in many components. 
      Almost all classes in org.globus.security package are now deprecated.
      Please use the GSS-API abstractions as much as possible in your 
      code. See compatibility.html document for details.

3. Features

   What's new in version 1.1?

   o A totally new security library implementation based entirely on 
     open-source libraries.
   o GSI transport security extensions for Axis SOAP engine and Tomcat
     servlet engine
   o Full GridFTP support (merged jftp code)
   o Myproxy "info" operation support
   o Replica catalog API (LDAP-based)

   The Java CoG Kit contains the following APIs and tools:

   Java Globus API 
    - GRAM 1.5 (gram)
      + uses standard Java event notification for job abstraction
    - GASS (io/gass)
      + implements server and client functionality
    - GSI (gsi)
      + provides GSS-API interface
      + implements GSS-Extensions API as defined by GGF
      + implements the new proxy certificate format specification 
        as defined by GGF (aka GSI-3 support)
      + based entirely on open-source libraries
    - MyProxy (myproxy)
      + full client implementation of MyProxy
    - GridFTP (ftp)
      + full GridFTP support (data channel authentication and protection,
        third party transfers, restart & performance markers, EBLOCK
        mode support, parallel transfers, etc.)
      + includes common GridFTP/FTP commands as methods
    - GSI MDS (mds/gsi)
      + provides GSI/SASL extensions for JNDI library and Netscape's LDAP
        Directory SDK to access MDS
    - GSI Axis (axis)
      + GSI transport security extensions for Axis SOAP engine
    - GSI Tomcat (tomcat)
      + GSI transport security extensions for Tomcat servlet engine
    - RSL
      + parses RSL strings
    - UrlCopy (io/urlcopy)
      + copies between any of https, http, ftp, gsiftp, and file
      + initiates third-party transfer between GridFTP/FTP servers 
    - Gatekeeper/Jobmanager 
      + an implementation of globus-personal-gatekeeper (GRAM 1.0 protocol)
    - Replica catalog (replica/catalog)
      + LDAP-based on replica catalog API

   Java Globus tools
    - globusrun
    - globus-url-copy
    - grid-proxy-init
    - grid-proxy-info
    - grid-proxy-destroy
    - grid-cert-info
    - globus-gass-server
    - globus-gass-server-shutdown
    - grid-info-search
    - grid-change-pass-phrase
    - globus-personal-gatekeeper

   The following APIs were deprecated:
    - GARA (gara) [no longer maintained]
      + uses standard Java event notification
    - MDSML (mdsml) [no longer distributed]
      + converts from MDSML to various formats

4. More Information

   4.1 Java CoG Kit Web Site

       http://www.globus.org/cog/java

   4.2 Bugs, suggestions, contributions

       Please report bugs and request enhancements by using our bug
       tracking system, Bugzilla, which is available at:

	http://bugzilla.globus.org

       To contact us concerning anything else, please send mail to
       java@globus.org. [Please note, you must be subscribed to this
       list to post, otherwise your submission will simply disappear].

       If you would like to contribute to the development of the Java
       CoG Kit or any other CoG Kit please send email to
       gregor@mcs.anl.gov.

   4.3 Contacts

       Jarek Gawor          Developer              gawor@mcs.anl.gov
       Peter Lane           Developer              lane@mcs.anl.gov
       Beulah Alunkal       Developer              alunkal@mcs.anl.gov
       Mihael Hategan       Developer              hategan@mcs.anl.gov
       Gregor von Laszewski Supervisor/Developer   gregor@mcs.anl.gov
  
   4.4 Contributors

       Kaizar Amin              amin@mcs.anl.gov
       Sandeep Nijsure          nijsure@mcs.anl.gov
       Nell Rehn                rehn@mcs.anl.gov
       Darcy Quesnel	    	quesnel@mcs.anl.gov
       Michael Russell 		russell@cs.uchicago.edu
       Jason Novotny 		novotny@nlanr.net
       Benjamin Temko 		btemko@extreme.indiana.edu
       Warren Smith		wwsmith@nas.nasa.gov

   4.5 Acknowledgments

       If we have forgotten to include your name in the Contributors
       and or Developers list, please send email to
       gregor@mcs.anl.gov.

5. Other CoG Kits

   Other CoG Kits are being developed in various languages such as
   Perl, Tcl/Tk, Python, CORBA, etc. If you like to contribute or find
   out more about it please email gregor@mcs.anl.gov or checkout the
   Globus CoG web site at: http://www.globus.org/cog
