package uk.ac.cclrc.xmlwrapper ;
import java.util.* ;




public class InterfaceItems 
{
   // friendly interface class
   // no need for get and set functions at the moment

   public String field ;
   public String value ;
   public String dtd_path ;
   public String table_name ;
   public String column_name ;

   //a bit experimental
   public String conversion_function ;
 
   // for referential stuff ;
   public String key_table ;
   public String key_column ;
   public String required_column ;

   //
   // Methods
   //

   InterfaceItems()
   {
      field = "" ;
      value = "";
      dtd_path = "" ;
      table_name = ""  ;
      column_name = ""  ;

      conversion_function = "" ;
 
      key_table = ""  ;
      key_column = ""  ;
      required_column = ""  ;
    }

    InterfaceItems(String field, String dtd_path,
                   String table_name, String column_name)
    {
       this.field = field ;
       this.dtd_path = dtd_path ;
       this.table_name = table_name ;
       this.column_name = column_name ;

       value = "" ;
       conversion_function = "" ;
       key_table = "" ;
       key_column = "" ;
       required_column = "" ;
    }

    // not needed due to access modifiers but maybe necessary

    public void setValue(String value)
    {
       this.value = value ;
    }

    String getValue()
    {
       return this.value ;
    }

    // not needed but maybe nice

    public void setConvFunc(String conv_func)
    {
       conversion_function = conv_func ;
    }
 
    String getConvFunc()
    {
       return conversion_function ;
    }
   
}








       
      
