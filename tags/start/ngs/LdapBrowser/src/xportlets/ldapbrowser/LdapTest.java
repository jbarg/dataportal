
import java.util.Hashtable;
import java.util.Enumeration;

import javax.naming.*;
import javax.naming.directory.*;


public class LdapTest {
    public static String INITCTX = "com.sun.jndi.ldap.LdapCtxFactory";
    public static String MY_HOST = "ldap://ngsinfo.grid-support.ac.uk:2135";
    public static String MY_SEARCHBASE = "Mds-Vo-name=ngsinfo, o=grid";
    public static String MY_FILTER = "(objectclass=*)";


    public static void main(String args[]) {

    try
    {
        Hashtable env = new Hashtable();

        env.put(Context.INITIAL_CONTEXT_FACTORY, INITCTX);

        env.put(Context.PROVIDER_URL, MY_HOST);

        DirContext ctx = new InitialDirContext(env);

        SearchControls constraints = new SearchControls();

        constraints.setSearchScope(SearchControls.SUBTREE_SCOPE);

        NamingEnumeration results = ctx.search(MY_SEARCHBASE,MY_FILTER,constraints);

        while(results != null && results.hasMore()) {

            SearchResult sr = (SearchResult) results.next();

            String dn = sr.getName() + " ," + MY_SEARCHBASE;

            System.out.println("Distinguished name is " + dn);

            Attributes attrs = sr.getAttributes();

            for(NamingEnumeration ne = attrs.getAll();ne.hasMoreElements();)
            {
                Attribute attr = (Attribute)ne.next();
                String attrID = attr.getID();

                System.out.println(attrID+":");

                for(Enumeration vals = attr.getAll();vals.hasMoreElements();)
                {
                    System.out.println("\t" + vals.nextElement());
                }
            }
            System.out.println("\n");
        }
    }
    catch(Exception e)
    {
        e.printStackTrace();
        System.exit(1);
    }
    }
}
