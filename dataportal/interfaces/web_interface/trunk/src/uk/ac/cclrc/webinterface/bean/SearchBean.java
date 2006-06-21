package uk.ac.cclrc.webinterface.bean;

public class SearchBean {

    private String keyword = null ;
    
    public SearchBean() {
    }

    public void setKeyword(String keyword){
       this.keyword=keyword ;
    }

    public String getKeyword() {
       return keyword ;
    }
    
}
