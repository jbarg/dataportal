package xportlets.gridftp;

import org.globus.ftp.*;
import org.globus.ftp.exception.*;

public class MarkerListenerImpl implements MarkerListener
    {

	private ByteRangeList list;     
	private long bytesTransferred; // keeps track of total bytes transferred so far,
                                      // i.e. at the time the last PerfMarker came in. 
	
	public MarkerListenerImpl() {
	    list = new ByteRangeList();
	}

        public long bytesTransferred()
        {
            return bytesTransferred;
        }
        
	public void markerArrived(Marker m) {
	    if (m instanceof GridFTPRestartMarker) {
		restartMarkerArrived((GridFTPRestartMarker) m);
	    } else if (m instanceof PerfMarker) {
		perfMarkerArrived((PerfMarker) m);
	    } else {
	    }
	};
	
	private void restartMarkerArrived(GridFTPRestartMarker marker) {
	    System.out.println("Restart marker arrived");
	    list.merge(marker.toVector());
	}
	
	private void perfMarkerArrived(PerfMarker marker) {
	    // time stamp

	    // stripe index
	    if (marker.hasStripeIndex()) {
		try {
		    System.out.println("Stripe index =" + marker.getStripeIndex());
		} catch (PerfMarkerException e) {
		}
	    }else {
		System.out.println("Stripe index: not present");
	    }
	    
	    // stripe bytes transferred
	    if (marker.hasStripeBytesTransferred()) {
		try {
                    bytesTransferred = marker.getStripeBytesTransferred();
		    System.out.println("Stripe bytes transferred = "
				 + bytesTransferred);
		} catch (PerfMarkerException e) {
		}
	    }else {
		System.out.println("Stripe Bytes Transferred: not present");
	    }
	    
	    // total stripe count
	    if (marker.hasTotalStripeCount()) {
		try {
		    System.out.println("Total stripe count = " 
				 + marker.getTotalStripeCount());
		} catch (PerfMarkerException e) {
		}
	    }else {
		System.out.println("Total stripe count: not present");
	    }
	}//PerfMarkerArrived   
}
