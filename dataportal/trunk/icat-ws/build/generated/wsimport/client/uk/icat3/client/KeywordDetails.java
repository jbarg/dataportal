
package uk.icat3.client;

import java.util.ArrayList;
import java.util.List;
import javax.xml.bind.annotation.XmlAccessType;
import javax.xml.bind.annotation.XmlAccessorType;
import javax.xml.bind.annotation.XmlElement;
import javax.xml.bind.annotation.XmlType;


/**
 * <p>Java class for keywordDetails complex type.
 * 
 * <p>The following schema fragment specifies the expected content contained within this class.
 * 
 * <pre>
 * &lt;complexType name="keywordDetails">
 *   &lt;complexContent>
 *     &lt;restriction base="{http://www.w3.org/2001/XMLSchema}anyType">
 *       &lt;sequence>
 *         &lt;element name="caseSensitve" type="{http://www.w3.org/2001/XMLSchema}boolean"/>
 *         &lt;element name="investigationIncludes" type="{client.icat3.uk}investigationInclude" minOccurs="0"/>
 *         &lt;element name="keywords" type="{http://www.w3.org/2001/XMLSchema}string" maxOccurs="unbounded" minOccurs="0"/>
 *         &lt;element name="operator" type="{client.icat3.uk}logicalOperator" minOccurs="0"/>
 *       &lt;/sequence>
 *     &lt;/restriction>
 *   &lt;/complexContent>
 * &lt;/complexType>
 * </pre>
 * 
 * 
 */
@XmlAccessorType(XmlAccessType.FIELD)
@XmlType(name = "keywordDetails", propOrder = {
    "caseSensitve",
    "investigationIncludes",
    "keywords",
    "operator"
})
public class KeywordDetails {

    protected boolean caseSensitve;
    protected InvestigationInclude investigationIncludes;
    @XmlElement(nillable = true)
    protected List<String> keywords;
    protected LogicalOperator operator;

    /**
     * Gets the value of the caseSensitve property.
     * 
     */
    public boolean isCaseSensitve() {
        return caseSensitve;
    }

    /**
     * Sets the value of the caseSensitve property.
     * 
     */
    public void setCaseSensitve(boolean value) {
        this.caseSensitve = value;
    }

    /**
     * Gets the value of the investigationIncludes property.
     * 
     * @return
     *     possible object is
     *     {@link InvestigationInclude }
     *     
     */
    public InvestigationInclude getInvestigationIncludes() {
        return investigationIncludes;
    }

    /**
     * Sets the value of the investigationIncludes property.
     * 
     * @param value
     *     allowed object is
     *     {@link InvestigationInclude }
     *     
     */
    public void setInvestigationIncludes(InvestigationInclude value) {
        this.investigationIncludes = value;
    }

    /**
     * Gets the value of the keywords property.
     * 
     * <p>
     * This accessor method returns a reference to the live list,
     * not a snapshot. Therefore any modification you make to the
     * returned list will be present inside the JAXB object.
     * This is why there is not a <CODE>set</CODE> method for the keywords property.
     * 
     * <p>
     * For example, to add a new item, do as follows:
     * <pre>
     *    getKeywords().add(newItem);
     * </pre>
     * 
     * 
     * <p>
     * Objects of the following type(s) are allowed in the list
     * {@link String }
     * 
     * 
     */
    public List<String> getKeywords() {
        if (keywords == null) {
            keywords = new ArrayList<String>();
        }
        return this.keywords;
    }

    /**
     * Gets the value of the operator property.
     * 
     * @return
     *     possible object is
     *     {@link LogicalOperator }
     *     
     */
    public LogicalOperator getOperator() {
        return operator;
    }

    /**
     * Sets the value of the operator property.
     * 
     * @param value
     *     allowed object is
     *     {@link LogicalOperator }
     *     
     */
    public void setOperator(LogicalOperator value) {
        this.operator = value;
    }

}
