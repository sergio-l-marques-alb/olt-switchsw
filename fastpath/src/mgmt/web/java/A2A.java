/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename A2A.java
*
* @purpose Simple static storage of Applet pointers
*
* @component Target useage is to provide a common applet used for applet to
* applet communication. This class simply registers (and retrieves) a single
* applet reference.
*
* @comments
*
* @create 05/07/2006
*
* @author darsenault
* @end
*
**********************************************************************/

import java.applet.*;
import java.net.URL;
import java.util.*;
import netscape.javascript.*;

public class A2A extends javax.swing.JApplet implements Runnable {
   
    private Thread mainThread = null; 
    private String host = null; 
    boolean fServer = false;
    
    public void init() {
        try {
            java.awt.EventQueue.invokeAndWait(new Runnable() {
                public void run() {                   
                    initComponents();            
                    host = new String(getHostName());                          
                }
            });
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }
    
    public void start(){
      // Only the server gets registered
      fServer = (null!= stringParameterGet("SERVER_ID"))?true:false;     
      if (fServer) A2ARef.register(host,this);
      
      // Display registered applet
      //Applet applet = null;
      //System.out.println("A2A::start:"+host+": - Retrieve existing applets...");      
      //Enumeration e = A2ARef.getAppletKeys();
      //int idx = 0;
      //while (e.hasMoreElements()) {
      //  String key = (String) e.nextElement();
      //  System.out.println("A2A::"+host+": - applet key["+idx+"] = " + key);
      //  idx++;        
      //}
      
      if (mainThread == null) {
    	mainThread = new Thread(this);
	mainThread.start();
      }      
    }    

    public void stop(){
      if (fServer) A2ARef.remove(host);
      if (mainThread != null) mainThread = null;
    }
    
    public void run(){                 
    }    

    /**
     ***************************************************************************
     * Override this method for this faceless applet. It is used to obtain a
     * reference to the A2A applet running within the MapApplet browser. It then
     * calls sendLink.
     ***************************************************************************
     */             
    public void showStatus(String status) {
      A2A mapApplet = (A2A) A2ARef.getApplet();
      if (null!=mapApplet) mapApplet.sendLink(status);             
    }

    /**
     ***************************************************************************
     * Override this method and return the server key (if it exists).
     ***************************************************************************
     */                 
    public String getAppletInfo() {
      String key = null;
      Enumeration e = A2ARef.getAppletKeys();
      if (e.hasMoreElements()) key = (String) e.nextElement();
      return key;
    }    
    
    /**
     ***************************************************************************
     * The method is called when this A2A applet instantiation is running within
     * the MapApplet browser. We can therefore obtain a reference to it, which
     * in turn, we finally send the URL over to its base form.
     ***************************************************************************
     */         
    private void sendLink(String link) {
      JSObject win = null;    
      URL url = null;
      Applet mapApplet = getAppletContext().getApplet("MapAppletNavigation");      
      if (null!=mapApplet) {
        mapApplet.getAppletContext().showStatus(link);
        try { url = new URL("http://" + host + link); }
        catch (Exception e) {
          System.out.println(e.getMessage());
          return;
        }
        try {
          mapApplet.getAppletContext().showDocument(url,"basefrm");
          win = JSObject.getWindow(mapApplet);
          win.eval("window.focus();");        
        }
        catch (Exception e) {
          System.out.println(e.getMessage());
          return;
        }
      }
    }
    
    /**
     ***************************************************************************
     ***************************************************************************
     */    
    private String stringParameterGet(String parmStr) {
      String strVal = null;
      try { strVal = new String(getParameter(parmStr)); }
      catch (Throwable T) { strVal = null; }
      return strVal;
    }        
    
    /**
     ***************************************************************************
     ***************************************************************************
     */
    private String getHostName() {
        int slashIndex;
        
        if (null == host) {
            host = new String();
            host = getCodeBase().toString();
            slashIndex = host.indexOf("/");
            while (slashIndex < 7) {
                host = host.substring(slashIndex+1);
                slashIndex= host.indexOf("/");
            }
            host = host.substring(0, slashIndex);
        }
        return host;
    }
    
    /** This method is called from within the init() method to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc=" Generated Code ">//GEN-BEGIN:initComponents
    private void initComponents() {
        
    }
    // </editor-fold>//GEN-END:initComponents
    
    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    // End of variables declaration//GEN-END:variables
    
}
