/*
 * ParameterBean.java
 *
 * Created on 24 June 2004, 01:01
 */

package uk.ac.clrc.dataportal.credentials;
import javax.xml.namespace.QName;
/**
 *
 * @author  glen
 */
public class ParameterBean {
    
    private QName type;
    private Object value;
    
    /** Creates a new instance of ParameterBean */
    public ParameterBean() {
        
    }
    
    public void setValue(Object _value){
        this.value = _value;
    
    }
    
     public Object getValue(){
        return this.value;
    }
     
     public void setType(QName _type){
        this.type = _type;
    
    }
    
     public QName getType(){
        return this.type;
    }
    
}
