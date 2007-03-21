package uk.ac.dl.dp.coreutil.entity;
import java.io.Serializable;
import javax.persistence.Column;
import javax.persistence.Embeddable;
import javax.persistence.Id;

@Embeddable
public class DataRefAuthorisationPK implements Serializable {

    @Id
    @Column(name = "SOURCE_USER_ID", nullable = false)
    private Long sourceUserId;

    @Id
    @Column(name = "AUTHORISED_USER_ID", nullable = false)
    private Long authorisedUserId;

    public DataRefAuthorisationPK() {
    }

    public DataRefAuthorisationPK(Long authorisedUserId, Long sourceUserId) {
        this.authorisedUserId = authorisedUserId;
        this.sourceUserId = sourceUserId;
    }

    public Long getSourceUserId() {
        return this.sourceUserId;
    }

    public void setSourceUserId(Long sourceUserId) {
        this.sourceUserId = sourceUserId;
    }

    public Long getAuthorisedUserId() {
        return this.authorisedUserId;
    }

    public void setAuthorisedUserId(Long authorisedUserId) {
        this.authorisedUserId = authorisedUserId;
    }

    public int hashCode() {
        int hash = 0;
        hash += (this.authorisedUserId != null ? this.authorisedUserId.hashCode() : 0);
        hash += (this.sourceUserId != null ? this.sourceUserId.hashCode() : 0);
        return hash;
    }

    public boolean equals(Object object) {
        if (object == null || !this.getClass().equals(object.getClass())) {
            return false;
        }
        DataRefAuthorisationPK other = (DataRefAuthorisationPK)object;
        if (this.authorisedUserId != other.authorisedUserId && (this.authorisedUserId == null || !this.authorisedUserId.equals(other.authorisedUserId))) return false;
        if (this.sourceUserId != other.sourceUserId && (this.sourceUserId == null || !this.sourceUserId.equals(other.sourceUserId))) return false;
        return true;
    }
}
