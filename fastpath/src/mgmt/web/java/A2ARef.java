/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename AppletRef.java
*
* @purpose Simple static storage of Applet pointers
*
* @component Target useage is to provide global storage providing a resource
* repository to be shared between the Map and NV applet. Please note that
* even though a hashtable is used, we only allow a single applet-reference.
*
* @comments
*
* @create 05/07/2006
*
* @author darsenault
* @end
*
**********************************************************************/

import java.applet.Applet;
import java.util.*;

class A2ARef {
	// Structure for storing pointers to the applets. 
	private static Hashtable<String,Applet> applets = new Hashtable<String,Applet>();

	/**
	 * Method for registering a single applet. If the applet/key already
         * exists, it is removed and replaced with the new one. This can
         * happen if a second MapApplet browser connects to the switch. In this
         * case, the last browser to connect wins.
	 */
	public static void register(String name, Applet applet) {
          if (applets.containsKey(name)) applets.remove(name);
          applets.put(name,applet);
	}	

	/**
	 * Method for un-registering an applet in the AppletList
	 */
	public static void remove(String name) {          
          if ((null!=name) && applets.containsKey(name)) applets.remove(name);
	}	

	/**
	 * Retrieve first object from the list
	 */
	public static Applet getApplet() {
          Enumeration e = getApplets();
          if (e.hasMoreElements()) return (Applet) e.nextElement();
          return null;
	}        
        
	/**
	 * Method for retrieving an applet reference by key
	 */
	public static Applet getApplet(String name) {
	  return (Applet) applets.get(name);
	}
       
	/**
	 * Method for getting all applet-reference from the AppletList
	 */
	public static Enumeration<Applet> getApplets() {
	  return applets.elements();
	}

	/**
	 * Method for getting all applet-keys from the AppletList
	 */
	public static Enumeration<String> getAppletKeys() {
	  return applets.keys();
	}        
        
	public static int size() {
	  return applets.size();
	}

}

