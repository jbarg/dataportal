package xportlets.proxymanager; 

import org.ietf.jgss.GSSCredential;
import org.ietf.jgss.GSSException;

// Java
import java.util.Hashtable;
import java.util.Enumeration;
 
import javax.servlet.http.HttpSession;

public class ProxyManager {

    private ProxyManager() {
    }

    public static GSSCredential getDefaultProxy(HttpSession session) 
    {
	if(session == null)
	    return null;

	ProxyStorage storage = (ProxyStorage)session.getAttribute(Constants.PROXYSTORAGE);
	if(storage == null)
	    return null;

	try {
	    storage.load(session);
	}
	catch(ProxyStorageException pse){
	    pse.printStackTrace();
	    return null;
	}
	

	return storage.getDefault();
    }

    public static GSSCredential getProxy(HttpSession session, String hash) 
	throws ProxyStorageException
    {
	if(session == null)
	    throw new ProxyStorageException("session null!");

	ProxyStorage storage = (ProxyStorage)session.getAttribute(Constants.PROXYSTORAGE);
	if(storage == null)
	    throw new ProxyStorageException("no proxy available!");

	try {
	    storage.load(session);
	}
	catch(ProxyStorageException pse){
	    pse.printStackTrace();
	    throw pse;
	}
	
	return storage.get(hash);
    }

    public static Hashtable getAllProxies(HttpSession session) 
	throws ProxyStorageException
    {
	if(session == null)
	    throw new ProxyStorageException("session null!");
	
	ProxyStorage storage = (ProxyStorage)session.getAttribute(Constants.PROXYSTORAGE);
	
	if(storage == null)
	    throw new ProxyStorageException("storage null!");

	try {
	    storage.load(session);
	}
	catch(ProxyStorageException pse){
		pse.printStackTrace();
		throw pse;
	}
	
	return storage.getAll();
    }
    
    public static String addProxy(HttpSession session, GSSCredential proxy) 
	throws ProxyStorageException
    {
	if(session == null)
	    throw new ProxyStorageException("session null!");

	if(proxy == null)
	    return null;

	ProxyStorage storage = (ProxyStorage)session.getAttribute(Constants.PROXYSTORAGE);
	if(storage == null) {
	    storage = new ProxyStorage();
	}

	String hash = null;
	try {
	    hash = storage.put(proxy);
	    
	    if(storage.getDefault() == null)
		storage.setDefault(hash);
	}
	catch(ProxyStorageException pse){
	    pse.printStackTrace();
	    throw pse;
	}
		
	session.setAttribute(Constants.PROXYSTORAGE, storage);

	return hash;
    }


    public static void addAllProxies(HttpSession session, Hashtable proxies, String def) 
	throws ProxyStorageException
    {
	if(session == null)
	    throw new ProxyStorageException("session null!");

	if(proxies == null)
	    return;

        ProxyStorage storage = (ProxyStorage)session.getAttribute(Constants.PROXYSTORAGE);
        if(storage == null)
	    storage = new ProxyStorage();  

	try {
	    String hash = null;
	    Enumeration keys = proxies.keys(); 
	    while ( keys.hasMoreElements() ) { 
		hash = (String)keys.nextElement();
		GSSCredential proxy = (GSSCredential)proxies.get( hash );
		storage.put(proxy);
	    }
	    
	    // if default not set and no previous setting, set the last one as the default
	    if(def == null && storage.getDefault() == null) {
		def = hash;
		storage.setDefault(hash);
	    }
	    else if(def != null)
		storage.setDefault(hash);
	}
	catch(ProxyStorageException pse){
	    pse.printStackTrace();
	    return;
	}
		
	session.setAttribute(Constants.PROXYSTORAGE, storage);

    }


    public static void setProxies(HttpSession session, ProxyStorage storage) 
	throws ProxyStorageException
    {
	if(session == null)
	    throw new ProxyStorageException("session null!");

	if(storage == null)
	    return;

	session.setAttribute(Constants.PROXYSTORAGE, storage);
    }

    public static void setDefaultProxy(HttpSession session, GSSCredential proxy) 
	throws ProxyStorageException
    {
	if(proxy == null)
	    throw new ProxyStorageException("proxy null!");

	String hash = String.valueOf(proxy.hashCode());
	setDefaultProxy(session, hash);
    }

    public static void setDefaultProxy(HttpSession session, String hash) 
    throws ProxyStorageException
    {
	if(session == null)
	    throw new ProxyStorageException("session null!");

	if(hash == null)
	    return;

	if(hash.trim().equals(""))
	    return;

	ProxyStorage storage = (ProxyStorage)session.getAttribute(Constants.PROXYSTORAGE);
	storage.setDefault(hash);
	session.setAttribute(Constants.PROXYSTORAGE, storage);
    }

    public static GSSCredential removeProxy(HttpSession session, GSSCredential proxy) 
	throws ProxyStorageException
    {
	if(proxy == null)
	    return null;
	
	String hash = null;
	
	try {
	    hash = String.valueOf(proxy.getName().hashCode());
	}
	catch(GSSException e) {
	}
	
	return removeProxy(session, hash);
    }

    public static GSSCredential removeProxy(HttpSession session, String hash) 
	throws ProxyStorageException
    {
	if(session == null)
	    throw new ProxyStorageException("session null!");

	if(hash == null)
	    return null;

	if(hash.trim().equals(""))
	    return null;

        ProxyStorage storage = (ProxyStorage)session.getAttribute(Constants.PROXYSTORAGE);
        if(storage == null)
            throw new ProxyStorageException("no proxy available!");

	GSSCredential proxy = null;

	try{	
	    proxy = storage.remove(hash);
	}
	catch(ProxyStorageException pse){
	    pse.printStackTrace();
	    throw pse;
	}
	
	session.setAttribute(Constants.PROXYSTORAGE, storage);

	return proxy;
    }


    public static void clear(HttpSession session) 
	throws ProxyStorageException
    {
	if(session == null)
	    throw new ProxyStorageException("session null!");

	session.removeAttribute(Constants.PROXYSTORAGE);
    }
    

    public static Hashtable removeExpiredProxies(HttpSession session) 
	throws ProxyStorageException
    {
	if(session == null)
	    throw new ProxyStorageException("session null!");

        ProxyStorage storage = (ProxyStorage)session.getAttribute(Constants.PROXYSTORAGE);
        if(storage == null)
            throw new ProxyStorageException("no proxy available!");

	Hashtable tb = null;
	try{
	    tb = storage.removeExpired();
	}
	catch(ProxyStorageException pse){
	    pse.printStackTrace();
	    throw pse;
	}
	catch(GSSException ge) {
	    ge.printStackTrace();
	    return null;
	}
	
	session.setAttribute(Constants.PROXYSTORAGE, storage);

	return tb;
    }

  //
  // Xiaobo Yang, CCLRC e-Science Centre, 14 December 2004
  // Returns (String) default_proxy of ProxyStorage class
  //
  public static String getDefaultProxyString(HttpSession session) {
    if (session==null) {
      return null;
    }

    ProxyStorage storage = (ProxyStorage)session.getAttribute(Constants.PROXYSTORAGE);
    if (storage==null) {
      return null;
    }

/*    try {
      storage.load(session);
    }
    catch (ProxyStorageException pse) {
      pse.printStackTrace();
      return null;
    }

    return storage.getDefault(); */
    return storage.getDefaultProxyString();

  }
    
}
