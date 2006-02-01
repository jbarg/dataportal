<%@page contentType="text/html" import="java.io.*,java.net.*"%>
<html>
<head><title>Test Download</title></head>
<body>

<%
    String dir = request.getParameter("dir");
    
    URL url = new URL("http://dmgdev1.esc.rl.ac.uk:8080/srbcom/srb/sgetrservlet?dir="+dir+"&name=dp()u5er");
    URLConnection yc = url.openConnection();
    InputStream p = yc.getInputStream();

    
    //FileInputStream myFileInputStream = new FileInputStream(myFileIn);
    BufferedInputStream myBufferedInputStream = new BufferedInputStream(p);
    ServletOutputStream out1 = response.getOutputStream();
    BufferedOutputStream myBufferedOutputStream = new BufferedOutputStream(out1);
    response.setContentType("application/download");
    response.setBufferSize(65536);
    
    String filename = dir.substring(dir.lastIndexOf("/") + 1);
     filename = filename.replace('.','_');
    response.setHeader("Content-disposition","attachment; filename=" + filename + ".tar");
    byte[] buffer = new byte[65536];
    int c=0;

    while ((c=myBufferedInputStream.read(buffer)) > -1) {
        myBufferedOutputStream.write(buffer,0,c);
    }

    myBufferedOutputStream.flush();
    myBufferedOutputStream.close();
    myBufferedInputStream.close();

    out.close();
%>


</body>
</html>
