package uk.ac.cclrc.xmlwrapper ;

public class MPIMCeraMapper implements CeraMapper
{
   public String getKeys(String query)
   {
      return query ;
   }

   public void buildMetadataRecord(String key, StringBuffer sbr)
   {
      sbr.append(key) ;
      return ;
   }

}
