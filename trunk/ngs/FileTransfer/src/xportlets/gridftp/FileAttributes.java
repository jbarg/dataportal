package xportlets.gridftp;


public class FileAttributes {
    
    String name = "";
    String time = "";
    String size = "";
    String piclink = "";
    String isFile = "";

//      String translink= "";

    public FileAttributes() {
    }

//      public FileAttributes(FileInfo f) {
//  	name = f.getName();
//  	time = f.getTime();
//  	date = f.getDate();
//  	if(f.isFile()) {
//  	    piclink = "/jetspeed/images/file.gif";
//  	    size = String.valueOf(f.getSize());
//  	}
//  	else if(f.isDirectory()) {
//  	    piclink = "/jetspeed/images/folder.gif";
//  	}
//  	else if(f.isSoftLink()) {
//  	    piclink = "/jetspeed/images/folder.gif";
//  	}
//      }

    public void setName(String v) {
	name = v;
    }

    public String getName() {
	return name;
    }

    public void setTime(String v) {
	time = v;
    }

    public String getTime() {
	return time;
    }
 
    public void setSize(String v) {
	size = v;
    }

    public String getSize() {
	return size;
    }

    public void setPiclink(String v) {
	piclink = v;
    }

    public String getPiclink() {
	return piclink;
    }

    public void setisFile(String v) {
	isFile = v;
    }

    public String getisFile() {
	return isFile;
    }
//      public void setTranslink(String v) {
//  	translink = v;
//      }

//      public String getTranslink() {
//  	return translink;
//      }

}
