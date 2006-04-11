
package uk.ac.dl.myproxy;

import java.awt.Color;
import java.awt.GridBagConstraints;
import java.awt.Dimension;
import java.io.File;
import java.util.Vector;
import java.util.StringTokenizer;
import java.awt.event.*;
import java.awt.event.ActionEvent;
import java.security.GeneralSecurityException;
import java.security.Principal;
import java.security.cert.X509Certificate;
import javax.swing.JOptionPane;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.AbstractListModel;
import javax.swing.JScrollPane;
import javax.swing.JFileChooser;
import java.io.*;
import java.net.*;

import org.globus.gsi.CertUtil;
import org.globus.gsi.TrustedCertificates;
import org.globus.common.CoGProperties;

public class ConfigModule2 extends BaseModule implements ActionListener {
    
    private JList certList;
    private CertificateListModel certModel;
    private JButton removeBT, addBT ,download;
    
    public ConfigModule2(CoGProperties props) {
        super(props);
        
        JLabel label = new JLabel("Configuring Certificate Authorities (CA)");
        label.setFont(getFont(font, 1));
        label.setForeground(Color.black);
        
        add(label,
        1, 1, 1, 1);
        
        label = new JLabel(" ");
        add(label,
        1, 2, 1, 1);
        
        certModel = new CertificateListModel();
        certModel.setCertList( props.getCaCertLocations() );
        
        certList = new JList(certModel);
        JScrollPane sp = new JScrollPane(certList);
        sp.setPreferredSize(new Dimension(200, 200));
        
        final JLabel help = new JLabel( "        Help      " );
        help.setForeground(Color.BLUE);
        help.setToolTipText("Both User and MyProxy Root CA certificates need to be added.");
       // help.setBounds(0,0,0,20);
        
        help.addMouseListener(new java.awt.event.MouseAdapter() {
            public void mouseClicked(java.awt.event.MouseEvent evt) {
                
                help(evt);
            }
             public void mouseEntered(java.awt.event.MouseEvent evt) {
                
                help.setForeground(Color.RED);
            }
             public void mouseExited(java.awt.event.MouseEvent evt) {
                
                help.setForeground(Color.BLUE);
            }
             
        });
        //help.addActionListener( this );
        
        download = new JButton( "Download Uk e-Science certificate" );
        download.addActionListener( this );
        //buttonPanel.add( downloadButton );
        
        removeBT = new JButton("Remove");
        removeBT.addActionListener(this);
        addBT    = new JButton("Add");
        addBT.addActionListener(this);
        
        //add(new JLabel("CA (Trusted) Certificates:"),
       // 1, 3, 1, 1);
        
        setFill(GridBagConstraints.HORIZONTAL);
        setAnchor(GridBagConstraints.NORTH);
        
        add(addBT,
        2, 4, 1, 1);
        add(download,1,3,1,1);
        add(help,2, 3, 1, 1);
        add(removeBT,
        2, 5, 1, 1);
        
        setFill(GridBagConstraints.BOTH);
        
        add(sp,
        1, 4, 1, 3);
       
    }
    
    public void help(MouseEvent e){
          JOptionPane.showMessageDialog(this,
                "Both User and MyProxy Root CA certificates need to be added.",                 
                "Help",
                JOptionPane.INFORMATION_MESSAGE);
    }
    
    public void actionPerformed(ActionEvent e) {
        String cmd = e.getActionCommand();
        if (cmd.equals("Add")) {
            addCert();
        } else if (cmd.equals("Remove")) {
            int i = certList.getSelectedIndex();
            if (i != -1) certModel.removeCertAt(i);
        }
        else if (cmd.equals("Download Uk e-Science certificate")) {
            downloadCert();
        }
       
    }
    
    private void addCert() {
        JFileChooser filechooser = new JFileChooser();
        filechooser.setCurrentDirectory(new File(System.getProperty("user.home")) );
        
        filechooser.setApproveButtonText("Select");
        filechooser.setDialogTitle("Add CA Certificate");
        
        if(filechooser.showOpenDialog(this) ==
        JFileChooser.APPROVE_OPTION) {
            File f = filechooser.getSelectedFile();
            try {
                String path = f.getAbsolutePath();
                X509Certificate cert =
                Configure.loadAndVerifyCertificate(path);
                certModel.addCertInfo(new CertInfo(cert, path));
            } catch(Exception ex) {
                JOptionPane.showMessageDialog(this,
                ex.getMessage(),
                "CA Certificate Error",
                JOptionPane.ERROR_MESSAGE);
            }
        }
    }
     private void addCert(String path1) {
        JFileChooser filechooser = new JFileChooser();
        filechooser.setCurrentDirectory(new File(path1));
        
        filechooser.setApproveButtonText("Select");
        filechooser.setDialogTitle("Add CA Certificate");
        
        if(filechooser.showOpenDialog(this) ==
        JFileChooser.APPROVE_OPTION) {
            File f = filechooser.getSelectedFile();
            try {
                String path = f.getAbsolutePath();
                X509Certificate cert =
                Configure.loadAndVerifyCertificate(path);
                certModel.addCertInfo(new CertInfo(cert, path));
            } catch(Exception ex) {
                JOptionPane.showMessageDialog(this,
                ex.getMessage(),
                "CA Certificate Error",
                JOptionPane.ERROR_MESSAGE);
            }
        }
    }
    
    public void saveSettings() {
        props.setCaCertLocations( certModel.getCertFileList() );
    }
    
    public boolean verifySettings() {
        boolean rs;
        rs = verifyCertificate();
        if (!rs) return false;
        
        return true;
    }
    
     public void downloadCert(){
        Object[] options = { "Ok", "Cancel" };
        int result = JOptionPane.showOptionDialog(this, "Do you wish to download and install the Uk e-Sceince certificate","User Certificate Verification", JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE,null, options, options[0]);
        //System.out.println("result is "+result);
        if(result == 0){
            BufferedReader in = null;
            FileWriter out = null;
            try{
                File cert = new File(System.getProperty("user.home")+File.separator+".globus");
                File cert2 = new File(System.getProperty("user.home")+File.separator+".globus"+File.separator+"certificates");
                
                if(!cert.exists()) cert.mkdir();
                if(!cert2.exists()) cert2.mkdir();
                
               // URL url = new URL("http://esc.dl.ac.uk:9000/dataportal/html/proxy/01621954.0");
                URL url  = this.getClass().getClassLoader().getResource("01621954.0");
                System.out.println(url);
                URLConnection con = url.openConnection();
                InputStream in2 = con.getInputStream();
                in = new BufferedReader(new InputStreamReader(in2));
                out= new FileWriter(System.getProperty("user.home")+File.separator+".globus"+File.separator+"certificates"+File.separator+"01621954.0");
                String inputLine;
                
                while ((inputLine = in.readLine()) != null){
                    System.out.println(inputLine);
                    out.write(inputLine);
                    out.write("\n");
                }
                in.close();
                out.close();
                JOptionPane.showMessageDialog(this, "Successfully saved to "+System.getProperty("user.home")+File.separator+".globus"+File.separator+"certificates"+File.separator+"01621954.0","User Certificate Verification",  JOptionPane.INFORMATION_MESSAGE);
    
                addCert(System.getProperty("user.home")+File.separator+".globus"+File.separator+"certificates");
                //JOptionPane.showMessageDialog(this, "Successfully saved "+System.getProperty("user.home")+File.separator+".globus"+File.separator+"certificates"+File.separator+"01621954.0","User Certificate Verification",  JOptionPane.INFORMATION_MESSAGE);
    
                //return false;
                
            }
            catch(Exception ey){
                JOptionPane.showMessageDialog(this,
                "Could not download the certificate \n"+ey.getMessage(),
                "User Certificate Verification Error",
                JOptionPane.ERROR_MESSAGE);
                //return false;
                
            }
        }else{}
            
        
        
    }
     
   
    
    public boolean verifyCertificate() {
        try {
            X509Certificate cert =
            (X509Certificate)props.get("internal.usercert");
            certModel.verifyUserCertificate(cert);
            return true;
        } catch(Exception e) {
           /* if(e.getMessage().equals("At least one CA certificate is required.")){
                //JOptionPane.showOptionDialog(this,"You need a CA Certificate.\n Download the Uk eSceince certificate","User Certificate Verification", null,null, null, null, null,null, JOptionPane.OK_OPTION);
                Object[] options = { "Ok", "Cancel" };
                int result = JOptionPane.showOptionDialog(null, "You need a CA Certificate.\n Download the Uk eSceince certificate","User Certificate Verification", JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE,null, options, options[0]);
                System.out.println("result is "+result);
                if(result == 0){
                    BufferedReader in = null;
                    FileWriter out = null;
                    try{
                        File cert = new File(System.getProperty("user.home")+File.separator+".globus");
                        File cert2 = new File(System.getProperty("user.home")+File.separator+".globus"+File.separator+"certificates");
                        
                        if(!cert.exists()) cert.mkdir();
                        else if(!cert2.exists()) cert2.mkdir();
                        
                        URL url = new URL("http://esc.dl.ac.uk:9000/dataportal/html/proxy/01621954.0");
                        URLConnection con = url.openConnection();
                        InputStream in2 = con.getInputStream();
                        in = new BufferedReader(new InputStreamReader(in2));
                        out= new FileWriter(System.getProperty("user.home")+File.separator+".globus"+File.separator+"certificates"+File.separator+"01621954.0");
                        String inputLine;
                        
                        while ((inputLine = in.readLine()) != null){
                            System.out.println(inputLine);
                            out.write(inputLine);
                            out.write("\n");
                        }
                        in.close();
                        out.close();
                        addCert(System.getProperty("user.home")+File.separator+".globus"+File.separator+"certificates");
                        return false;
                        
                    }
                    catch(Exception ey){
                        JOptionPane.showMessageDialog(this,
                    "Could not download the certificate",
                    "User Certificate Verification Error",
                    JOptionPane.ERROR_MESSAGE);
                    return false;
               
                    }
                    
                }
                else{
                    JOptionPane.showMessageDialog(this,
                    e.getMessage(),
                    "User Certificate Verification Error",
                    JOptionPane.ERROR_MESSAGE);
                    return false;
                }
                
                
            }
            else{*/
        
                JOptionPane.showMessageDialog(this,
                e.getMessage(),
                "User Certificate Verification Error",
                JOptionPane.ERROR_MESSAGE);
                return false;
            //}
        }
    }
    
}

class CertInfo {
    public X509Certificate cert;
    public String file;
    
    public CertInfo(X509Certificate cert, String file) {
        this.cert = cert;
        this.file = file;
    }
}

class CertificateListModel extends AbstractListModel {
    
    private Vector certificates;
    
    public void setCertList(String certList) {
        certificates = new Vector();
        if (certList == null) return;
        StringTokenizer tokens = new StringTokenizer(certList, ",");
        File certFile;
        while(tokens.hasMoreTokens()) {
            certFile = new File(tokens.nextToken().trim());
            
            if (certFile.isDirectory()) {
                File[] caCertFiles =
                certFile.listFiles(TrustedCertificates.getCertFilter());
                for (int i=0;i<caCertFiles.length;i++) {
                    loadCert(caCertFiles[i].getAbsolutePath());
                }
            } else {
                loadCert(certFile.getAbsolutePath());
            }
        }
    }
    
    public void loadCert(String certFile) {
        try {
            X509Certificate cert = CertUtil.loadCertificate(certFile);
            certificates.addElement(new CertInfo(cert, certFile));
        } catch(Exception e) {
        }
    }
    
    public String getCertFileList() {
        int size = getSize();
        CertInfo info;
        StringBuffer buf = new StringBuffer();
        for (int i=0;i<size;i++) {
            info = getCertInfoAt(i);
            buf.append(info.file);
            if (i+1 < size) buf.append(", ");
        }
        return buf.toString();
    }
    
    public void addCertInfo(CertInfo newCert)
    throws Exception {
        int size = getSize();
        CertInfo info;
        for (int i=0;i<size;i++) {
            info = getCertInfoAt(i);
            if (info.cert.equals(newCert.cert)) {
                throw new Exception("CA already in the list.");
            }
        }
        certificates.addElement(newCert);
        fireIntervalAdded(this, size, size);
    }
    
    public void verifyUserCertificate(X509Certificate usercert)
    throws Exception {
        int size = getSize();
        if (size == 0) {
            throw new Exception("At least one CA certificate is required.");
        }
        
        Principal issuerDN = usercert.getIssuerDN();
        CertInfo info   = null;
        for (int i=0;i<size;i++) {
            info = getCertInfoAt(i);
            
            if (info.cert.getSubjectDN().equals(issuerDN)) {
                try {
                    // verify if this is the right ca cert...
                    usercert.verify(info.cert.getPublicKey());
                } catch(GeneralSecurityException e) {
                    throw new Exception("User certificate probably not" +
                    " signed by this" +
                    " CA : " + e.getMessage());
                }
                return;
            }
        }
        
        throw new Exception("No CA certificate for the user " +
        "certificate found.");
    }
    
    public int getSize() {
        return certificates.size();
    }
    
    public CertInfo getCertInfoAt(int i) {
        return (CertInfo)certificates.elementAt(i);
    }
    
    public Object getElementAt(int i) {
        return getCertInfoAt(i).cert.getSubjectDN();
    }
    
    public void removeCertAt(int i) {
        certificates.removeElementAt(i);
        fireIntervalRemoved(this, i, i);
    }
    
}

