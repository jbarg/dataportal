package uk.ac.cclrc.xmlwrapper ;

import java.util.* ;

import org.apache.log4j.* ;


public class ParseQuery
{

   //variables
   //holds decomposed sql 'like' where clause (e.g. from query to wrapper)
   
   List where_list = new LinkedList() ;

   //
   //
   
   public ParseQuery()
   {
      // for the advanced query
      where_list.add( new InterfaceItems("Discipline","/TOPIC/Discipline","TOPIC","TOPIC_NAME") ) ;
      where_list.add( new InterfaceItems("Subject","/TOPIC/Subject","LOCATION","LOCATION_NAME") ) ;
      where_list.add( new InterfaceItems("StudyName","/Experiment/StudyName","PROJECT", "PROJECT_NAME") ) ;
      where_list.add( new InterfaceItems("StudyName","/Experiment/StudyName","ENTRY", "ENTRY_NAME") ) ;
      where_list.add( new InterfaceItems("StudyID", "/Experiment/StudyID/@Studyid", "PROJECT", "PROJECT_ACRONYM") ) ;
      where_list.add( new InterfaceItems("StudyID", "/Experiment/StudyID/@Studyid", "ENTRY", "ENTRY_ACRONYM") ) ;
      where_list.add( new InterfaceItems("DMInstitutionName","/Study/Investigation/DataManager/Institution","INSTITUTE","INSTITUTE_NAME") ) ;
      // need some form of conversion function here
      where_list.add( new InterfaceItems("StartDate","/Study/StudyInformation/TimePeriod/StartDate","TEMPORAL_COVERAGE","START_DAY") ) ;
      where_list.add( new InterfaceItems("StartDate","/Study/StudyInformation/TimePeriod/StartDate","TEMPORAL_COVERAGE","START_MONTH") ) ;
      where_list.add( new InterfaceItems("StartDate","/Study/StudyInformation/TimePeriod/StartDate","TEMPORAL_COVERAGE","START_YEAR") ) ;
      // need some form of conversion function
      where_list.add( new InterfaceItems("EndDate","/Study/StudyInformation/TimePeriod/StartDate","TEMPORAL_COVERAGE","END_DAY") ) ;
      where_list.add( new InterfaceItems("EndDate","/Study/StudyInformation/TimePeriod/StartDate","TEMPORAL_COVERAGE","END_MONTH") ) ;
      where_list.add( new InterfaceItems("EndDate","/Study/StudyInformation/TimePeriod/StartDate","TEMPORAL_COVERAGE","END_YEAR") ) ; 
  
      return ;
   }

   //
   //
   
   void populateConditionalCriteria(String where_clause)
   {
      //list of Criteria to find in where clause:
      String discipline = null ;
      String subject = null ;
      String studyname = null ;
      String studyid = null ;
      String dminstitution = null ; //DMInstitution
      String startdate = null ; // Date format YYYYMMDD
      String enddate = null ;

      //TOPIC 
      discipline = findIndexValue(where_clause, "Discipline") ;
      //
      //As topics will be in a full xpath style directory location
      //we need to pull out the final one
      //need to handle things in lower case - e.g. Temperature and temperature in two
      //different earth sciences databases should be dealt with as the same thing
      //
      //Need to cope with the case where there is no discipline e.g. StudyName only
      if(discipline.compareTo("") != 0)
      {
	 
         StringTokenizer st = new StringTokenizer(discipline,"/") ;

         String temp_str = null ;

         while(st.hasMoreTokens())
         {
            temp_str = st.nextToken() ;
         }

         //the final token will be the topic we are searching on later
         discipline = temp_str ;
      }

      //SUBJECT
      subject = findIndexValue(where_clause, "Subject") ;

      //STUDYNAME
      studyname = findIndexValue(where_clause, "StudyName") ;

      //STUDYID
      studyid = findIndexValue(where_clause, "StudyID") ;

      //DMINSTITUTION
      dminstitution = findIndexValue(where_clause, "DMInstitutionName") ;

      //STARTDATE
      startdate = findIndexValue(where_clause, "StartDate") ;

      //ENDDATE
      enddate = findIndexValue(where_clause, "EndDate") ;

      //
      // add the item to the search list as this will be added to in time
      //

      addToSearchList("Discipline", discipline) ;
      addToSearchList("Subject", subject) ;
      addToSearchList("StudyName", studyname) ;
      addToSearchList("StudyID", studyid) ;
      addToSearchList("DMInstitutionName", dminstitution) ;
      addToSearchList("StartDate", startdate) ;
      addToSearchList("EndDate", enddate) ;

      // criteria for now populated we can go and contruct our query
      // to retrieve the required entries from which to obtain metadata entries

      return ;
   }

   //
   //
   //

   String getFromSearchList(String index)
   {
      boolean dis_found = false ;

      Iterator e = where_list.iterator() ;

      InterfaceItems ii = null ;

      while(e.hasNext() && dis_found == false)
      { 
         ii=(InterfaceItems)e.next() ;

         if(ii.field.toLowerCase().compareTo(index.toLowerCase()) == 0 )
         {
            dis_found = true ;
         }
      }

      if(dis_found == true)
      {
         return ii.value ;
      }
      else
      {
         return "" ;
      }

   }

   //
   //
   //

   String findIndexValue(String clause_ts, String index_tf)
   {
      // unfortunately this sets up a circular dependency
      // perhaps error checking needed elsewhere
      SessionSingleton ss = SessionSingleton.getInstance() ;

      Logger log = ss.getLogger() ;

      //clause_ts - clause to search
      //index_tf - index to find - and its value is extracted if it exists

      String temp_str = null ;

      // the reason white space is not in the token list is that
      // the index value may contain white space e.g. topic hierarchy
      // 
      StringTokenizer st = new StringTokenizer(clause_ts, ",'=%") ;

      boolean dis_found = false ;

      while(st.hasMoreTokens() && dis_found == false )
      {
         temp_str = st.nextToken() ;

	 //System.err.println("Clause: " + clause_ts + "\n Token: " + temp_str) ;
	 
	 //ok - following is a little corny but otherwise I would have to start righting my
	 //own string parser - so the following takes into account not just the first index
	 //name but subsequent index names also

         if( (temp_str.toLowerCase().compareTo(" " + index_tf.toLowerCase()) == 0)
            ||
             (temp_str.toLowerCase().compareTo(index_tf.toLowerCase()) == 0)
            ||
             (temp_str.toLowerCase().compareTo(" " + index_tf.toLowerCase() + " like ") == 0)
            ||
             (temp_str.toLowerCase().compareTo(index_tf.toLowerCase() + " like ") == 0) 
	    ||
	     (temp_str.toLowerCase().compareTo(" and " + index_tf.toLowerCase()) == 0) //note this would need no space 
	    ||                                                                         // after the = sign i.e. after %3D
	     (temp_str.toLowerCase().compareTo(" and " + index_tf.toLowerCase() + " like ") == 0)) 
         { 
            dis_found = true ;
	 }
      }

      if(dis_found == true && st.hasMoreTokens())
      {
	 String found_item = st.nextToken() ;
	 log.debug("Value of index is: " + found_item) ;
         //return st.nextToken() ; //as this is the index value we are looking for
         return found_item ; //as this is the index value we are looking for
      }
      else
      {
         return "" ;
      }
   }

   //
   //
   //
   
   void addToSearchList(String index, String value)
   {
      // unfortunately this sets up a circular dependency
      // perhaps error checking needed elsewhere
      SessionSingleton ss = SessionSingleton.getInstance() ;

      Logger log = ss.getLogger() ;
      
      log.debug("String index: " + index + ", String value: " + value) ;

      Iterator e = where_list.iterator() ;

      InterfaceItems ii = null ;

      boolean dis_found = false ;

      while(e.hasNext() && dis_found == false )
      {
         ii = (InterfaceItems)e.next() ;

         if(ii.field.toLowerCase().compareTo(index.toLowerCase()) == 0 )
         {
            ii.value = value.toLowerCase() ;
            dis_found = true ;
         }
      }

      return ;
   }

   

}
