
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename Interface.java
*
* @purpose Code in support of the MapApplet.html page
*
* @component unitmgr
*
* @create 09/29/2000
*
* @author Jan Jernigan, Jason Shaw
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/


import netscape.javascript.*;
import java.awt.*;
import java.awt.event.*;
import java.applet.*;
import java.net.*;
import java.util.*;
import java.io.*;
import java.lang.*;
import javax.swing.*;

public class Interface extends Component
{
   private int unit;
   private int slot;
   private int port;
   private int state;
   private boolean fpsMode;
   private int sfpSupported;
   private int stackIntfFlags;
   private int type;
   private int panelLocation;
   private Unit parentUnit;
   private Image baseImage;
   private Image linkUpImage;
   private Image linkDownImage;
   private Image linkDisabledImage;
   private Image poeUpImage;
   private Image poeDownImage;
   private Image poeDisabledImage;
   private Image poeDetachedImage;
   private boolean poeMode;
   private Image intfUpImage;
   private Image intfDownImage;
   private Image intfDisabledImage;
   private Image fpsUpImage;
   private Image fpsDownImage;
   private Image fpsDisabledImage;
   private Point baseLocation;
   private Point linkOffset;
   private Rectangle baseRectangle;
   private PopupMenu portPopupMenu;
   private String clickAction;
   private boolean entered = false;
   private final static boolean DEBUG = false;
   private JSObject win;             // object that allows javascript calls and evals

   private final static int INTF_TYPE_DATA_RJ45    = 101;         // TRGTRG ... should I dump these into the html so they are defined once??
   private final static int INTF_TYPE_DATA_GBIC    = 102;
   private final static int INTF_TYPE_DATA_SFP     = 103;
   private final static int INTF_TYPE_DATA_SCDUP   = 104;
   private final static int INTF_TYPE_DATA_XAUI    = 105;
   private final static int INTF_TYPE_SERIAL_PORT  = 201;
   private final static int INTF_TYPE_SERVICE_PORT = 301;
   private final static int INTF_TYPE_STACKING     = 401;

   private final static int APPLET_IFLAG_FPS_SUPPORT       = 0x00000100;    
   private final static int APPLET_IFLAG_FPS_MODE_SUPPORT  = 0x00000200;
   private final static int APPLET_IFLAG_FPS_MODE          = 0x00000400;
   private final static int APPLET_IFLAG_FPS_STACK_MODE    = 0x00000400;
   private final static int APPLET_IFLAG_STACK_MODE_ONLY   = 0x00000800;

   ActionListener actionListener;     // Post action events to listeners
   // MouseListener mouseListener;        Post action events to listeners

   Interface (int unit, int slot, int port, int state, int sfpSupported, int stackIntfFlags, int type,  
              int panelLocation, Unit parentUnit, Image baseImage, boolean poeMode,boolean fpsMode, 
              Image poeUpImage, Image poeDownImage, Image poeDisabledImage, Image poeDetachedImage,
              Image intfUpImage, Image intfDownImage, Image intfDisabledImage, 
              Image fpsUpImage, Image fpsDownImage, Image fpsDisabledImage, 
              Point baseLocation, Point linkOffset, 
              Rectangle baseRectangle, PopupMenu portPopupMenu, String clickAction, JSObject win)
   {
       this.unit                = unit;
       this.slot                = slot;
       this.port                = port;
       this.state               = state;
       this.sfpSupported        = sfpSupported;
       this.stackIntfFlags      = stackIntfFlags;
       this.type                = type;
       this.parentUnit          = parentUnit;
       this.panelLocation       = panelLocation;
       this.baseImage           = baseImage;
       this.fpsMode             = fpsMode;
       this.poeMode             = poeMode;

       this.poeUpImage          = poeUpImage;
       this.poeDownImage        = poeDownImage;
       this.poeDisabledImage    = poeDisabledImage;
       this.poeDetachedImage    = poeDetachedImage;

       if (fpsMode == true)
       {
         this.linkUpImage         = fpsUpImage;
         this.linkDownImage       = fpsDownImage;
         this.linkDisabledImage   = fpsDisabledImage;
       }
       else
       {
         this.linkUpImage         = intfUpImage;
         this.linkDownImage       = intfDownImage;
         this.linkDisabledImage   = intfDisabledImage;
       }
       this.baseLocation        = baseLocation;
       this.linkOffset          = linkOffset;
       this.baseRectangle       = baseRectangle;
       this.portPopupMenu       = portPopupMenu;
       this.clickAction         = clickAction;
       this.win                 = win;

       // if (DEBUG) System.out.println("adding interface " + unit + "." + slot + "." + port );
   }


   public void activateInterface(ActionListener listener) 
   {
     // if (DEBUG) System.out.println("activating interface " + unit + "." + slot + "." + port +" at ("+baseLocation.getX()+","+baseLocation.getY()+")" );
     if ((type != INTF_TYPE_SERIAL_PORT) && (type != INTF_TYPE_SERVICE_PORT))
       add(portPopupMenu);
     enableEvents(AWTEvent.MOUSE_EVENT_MASK);
     actionListener = AWTEventMulticaster.add(actionListener, listener);
   }

   public void deactivateInterface(ActionListener listener) 
   {
     // if (DEBUG) System.out.println("de-activating interface " + unit + "." + slot + "." + port+" at ("+baseLocation.getX()+","+baseLocation.getY()+")" );
     actionListener = AWTEventMulticaster.remove(actionListener, listener);
     disableEvents(AWTEvent.MOUSE_EVENT_MASK);
     // remove(portPopupMenu);
   }


     /**
   * Adds the specified action listener to receive action events
   * from this button.
   * @param listener the action listener
   */
   public void addActionListener(ActionListener listener) 
   {
       actionListener = AWTEventMulticaster.add(actionListener, listener);
       enableEvents(AWTEvent.MOUSE_EVENT_MASK);
       // if (type == INTF_TYPE_STACKING)
       //  System.out.println("addActionListener for " + unit + "." + slot + "." + port );
   }

//   public void addMouseListener(MouseListener mlistener)
//   {
//     mouseListener = AWTEventMulticaster.add(mouseListener, mlistener);
//     enableEvents(AWTEvent.MOUSE_EVENT_MASK);
     // if (DEBUG) System.out.println("addMouseListener performed");
//   }

   public void paint(Graphics g)
   {   
       // if (entered) 
       // g.drawImage(highlightImage, 0, 0, this);
       // else
     // if (DEBUG) System.out.println("Paint() for " + unit + "." + slot + "." + port );

     if (parentUnit.getCurrentPanel() == panelLocation)   // EG, if this interface is on the front panel but we are looking at the back panel
     {
       try
       {
           g.drawImage(baseImage, 0, 0, this);
       }
       catch (Throwable bi)
       {
         System.out.println("Interface paint() : Error drawing baseImage for interface " + unit + "." + slot + "." + port);
       }

       switch (state)
       {
       case 0:
         // if (DEBUG) System.out.println("link down");
         g.drawImage(linkDownImage, linkOffset.x, linkOffset.y, this);
         break;
       case 1:
         // if (DEBUG) System.out.println("link up");
         g.drawImage(linkUpImage, linkOffset.x, linkOffset.y, this);
         break;
       case 2:
         // if (DEBUG) System.out.println("link disabled");
         g.drawImage(linkDisabledImage, linkOffset.x, linkOffset.y, this);
         break;
       case 4:
         // if (DEBUG) System.out.println("poe link enabled");
         g.drawImage(poeUpImage, linkOffset.x, linkOffset.y, this);
         break;
       default:
         // if (DEBUG) System.out.println("default : link down");
         g.drawImage(linkDownImage, linkOffset.x, linkOffset.y, this);
         break;
       }

         /* bounds and location must be set here inside paint() 
            height and width are hard coded for now   */

         setBounds(baseLocation.x, baseLocation.y, baseRectangle.width, baseRectangle.height);
         setLocation(baseLocation);
         // if (type == INTF_TYPE_STACKING)
         //   System.out.println("Paint() for " + unit + "." + slot + "." + port + " at location ("+baseLocation.x+","+baseLocation.y+")" );
     }
   }


      /**
    * Paints the interface and sends an action event to all listeners.
    */
   public void processMouseEvent(MouseEvent e) 
   {
       Graphics g;

       switch(e.getID()) {
       
       case MouseEvent.MOUSE_ENTERED:
            // if (type == INTF_TYPE_STACKING)
            //  System.out.println("Mouse entered for stacking interface " + unit + "." + slot + "." + port );
            // render myself inverted....
            entered = true;
	    //setCursor(new Cursor(Cursor.HAND_CURSOR));
	    // Repaint might flicker a bit. To avoid this, you can use
	    // double buffering (see the Gauge example).
	    // repaint();
            break;
          
          case MouseEvent.MOUSE_RELEASED:
            // if (DEBUG) System.out.println("Mouse released for " + unit + "." + slot + "." + port );
            // if (DEBUG) System.out.println("mouse released for intf type "+type);
            switch (type)
            {
            case INTF_TYPE_SERVICE_PORT:
            case INTF_TYPE_SERIAL_PORT:
              win.eval("document.forms[0].selUnit.value = 0; document.forms[0].selSlot.value = 0; document.forms[0].selPort.value = 0;");
              win.eval("document.forms[0].selPageToLoad.value = \"" + clickAction +"\";");
              win.eval("document.forms[0].submit();");
              break;

            case INTF_TYPE_DATA_RJ45:
            case INTF_TYPE_DATA_GBIC:
            case INTF_TYPE_DATA_SFP:
            case INTF_TYPE_DATA_SCDUP:
            case INTF_TYPE_DATA_XAUI:
            case INTF_TYPE_STACKING:
               //portPopupMenu.show(this, e.getX(),e.getY());
               //Added to support Java 1.6
               this.add(portPopupMenu);
               portPopupMenu.show(getComponentAt(e.getX(),e.getY()), e.getX(),e.getY());
  
              // if (DEBUG) System.out.println("Clicked on interface " + unit + "." + slot + "." + port );
              win.eval("document.forms[0].selUnit.value = "+unit+"; document.forms[0].selSlot.value = "+slot+"; document.forms[0].selPort.value = "+port+";");

              if ((stackIntfFlags & APPLET_IFLAG_STACK_MODE_ONLY) == APPLET_IFLAG_STACK_MODE_ONLY)
              {
                win.eval("document.forms[0].stackIntfValid.value = "+1+"; document.forms[0].intfValid.value = "+0+";");
              }
              else if ((stackIntfFlags & APPLET_IFLAG_FPS_SUPPORT) == APPLET_IFLAG_FPS_SUPPORT)
              {
                if ((stackIntfFlags & APPLET_IFLAG_FPS_MODE_SUPPORT) == APPLET_IFLAG_FPS_MODE_SUPPORT)
                {
                  win.eval("document.forms[0].intfValid.value = "+1+"; document.forms[0].stackIntfValid.value = "+0+";");
                }
                else
                {
                  win.eval("document.forms[0].stackIntfValid.value = "+1+"; document.forms[0].intfValid.value = "+0+";");
                }
              }
              else
              {
                win.eval("document.forms[0].intfValid.value = "+1+"; document.forms[0].stackIntfValid.value = "+0+";");
              }
              break;

            default: 
              System.out.println("Clicked on unknown interface " + unit + "." + slot + "." + port );
              break;
            }

            break;

          case MouseEvent.MOUSE_PRESSED:
            break;
          
          case MouseEvent.MOUSE_EXITED:
            // if(entered == true) {
            //    entered = false;

	        // Repaint might flicker a bit. To avoid this, you can use
	        // double buffering (see the Gauge example).
	    //	repaint();

                // Note: for a more complete button implementation,
                // you wouldn't want to cancel at this point, but
                // rather detect when the mouse re-entered, and
                // re-highlight the button. There are a few state
                // issues that that you need to handle, which we leave
                // this an an excercise for the reader (I always
                // wanted to say that!)
            // }
            break;
       }
      // super.processMouseEvent(e);
   }


   // get methods to return all the private vars
   public int getUnit() 
   {
       return unit;
   }

   public int getSlot() 
   {
       return slot;
   }

   public int getPort() 
   {
       return port;
   }

   public int getState() 
   {
       return state;
   }

   public int getStackIntfFlags() 
   {
       return stackIntfFlags;
   }

   public int getSFPSupport() 
   {
       return sfpSupported;
   }

   public int getType() 
   {
       return type;
   }

   public String getClickAction() 
   {
       return clickAction;
   }

   public int getPanelLocation() 
   {
       return panelLocation;
   }

   public Image getBaseImage() 
   {
       return baseImage;
   }

   public Image getLinkUpImage() 
   {
       return linkUpImage;
   }

   public Image getLinkDownImage() 
   {
       return linkDownImage;
   }

   public Image getLinkDisabledImage() 
   {
       return linkDisabledImage;
   }

   public Point getBaseLocation() 
   {
       return baseLocation;
   }

   public Point getLinkOffset() 
   {
       return linkOffset;
   }

   public Rectangle getBaseRectangle() 
   {
       return baseRectangle;
   }

   public boolean getPoeMode()
   {
     return poeMode;
   }

   public void setPoeMode(boolean newMode)
   {
     if (poeMode != newMode)
     {
       poeMode = newMode;
       if (parentUnit.getCurrentPanel() == panelLocation)
           repaint();
     }
   }

   public boolean getFPSMode() 
   {
     return fpsMode;
   }

   public void setFPSMode(boolean newMode) 
   {
     if (fpsMode != newMode)
     {
       if (fpsMode == true)
       {
         this.linkUpImage         = fpsUpImage;
         this.linkDownImage       = fpsDownImage;
         this.linkDisabledImage   = fpsDisabledImage;
       }
       else
       {
         this.linkUpImage         = intfUpImage;
         this.linkDownImage       = intfDownImage;
         this.linkDisabledImage   = intfDisabledImage;
       }
     }
   }

   public void setState(int newState)
   {
     // if (DEBUG) System.out.println("State change for " + unit + "." + slot + "." + port );

     if (state != newState)
     {
       state = newState;
       // invalidate();
       if (parentUnit.getCurrentPanel() == panelLocation)  
           repaint();
           //validate();
     }
   }

}
