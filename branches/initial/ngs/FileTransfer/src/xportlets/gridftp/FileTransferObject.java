package xportlets.gridftp;

import xportlets.gridftp.MarkerListenerImpl; 

public class FileTransferObject implements Task
{
        private String srcHost;
        private String srcPort;
        private String srcPath;
        private String destHost;
        private String destPort;
        private String destPath;
	private long size;
        private MarkerListenerImpl markerListener = null;

        private int status;
        
        public FileTransferObject(String srcHost, String srcPort, String srcPath, 
                                String destHost, String destPort, String destPath,
                                long size, MarkerListenerImpl markerListener)
        {
            this.srcHost = srcHost;
            this.srcPort = srcPort;
            this.srcPath = srcPath;
            this.destHost = destHost;
            this.destPort = destPort;
            this.destPath = destPath;
	    this.size = size;
            this.markerListener = markerListener;
            status = WAITING;
        }

        public String getSrcHost()
        {
            return srcHost;
        }
        
        public String getSrcPort()
        {
            return srcPort;
        }
        
        public String getSrcPath()
        {
            return srcPath;
        }
        
        public String getDestHost()
        {
            return destHost;
        }
        
        public String getDestPort()
        {
            return destPort;
        }
        
        public String getDestPath()
        {
            return destPath;
        }
       
	public long getSize()
	{ 
	    return size;
	}

	public void setSize(long size)
	{
	    this.size = size;	
	}
 
        public MarkerListenerImpl getMarkerListener()
        {
            return markerListener;
        }
        
        public long bytesTransferred()
        {
            return markerListener.bytesTransferred();
        }

        public int getStatus()
        {
            return status;
        }

	public String getStatusAsString()
	{
	   switch(status)
		{
			case Task.WAITING: return new String("Waiting");
			case Task.ONGOING: return new String("Active");
			case Task.DONE: return new String("Done");
			case Task.FAILED: return new String("Failed");
			case Task.RESTARTABLE: return new String("Restartable");
			default: return new String();
		}	
	}
        
        public void setStatus(int status) throws IllegalArgumentException
        {
            if((status != Task.WAITING) && (status != Task.ONGOING) 
                && (status != Task.DONE) && (status != Task.FAILED)
                && (status != Task.RESTARTABLE))
                throw new IllegalArgumentException ("Invalid status: " + status);
            
            this.status = status;
        }
}
