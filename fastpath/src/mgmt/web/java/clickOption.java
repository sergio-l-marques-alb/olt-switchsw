
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

public class clickOption extends Component
{
   private int type;
   private MapApplet  parentApplet;
   private int panelLocation;
   private Unit parentUnit;
   private Image baseImage;
   private Image hilightImage;
   private Point baseLocation;
   private Rectangle baseRectangle;
   private PopupMenu optionPopupMenu;
   private String descr;
   private String clickAction;
   private boolean entered = false;
   private boolean optionClicked = false;
   private final static boolean DEBUG = false;
   private final static int FRONT_PANEL  = 1;
   private final static int BACK_PANEL   = 2;
   private JSObject win;             // object that allows javascript calls and evals

   private final static int OPTION_TYPE_TOGGLE = 101;

   ActionListener actionListener;     // Post action events to listeners
   // MouseListener mouseListener;        Post action events to listeners

   clickOption (int type,  
                MapApplet  parentApplet, 
                int panelLocation, 
                Unit parentUnit,
                Image baseImage, 
                Image hilightImage, 
                Point baseLocation, 
                Rectangle baseRectangle, 
                PopupMenu optionPopupMenu, 
                String descr, 
                String clickAction, 
                JSObject win)
   {
       this.type                = type;
       this.parentApplet        = parentApplet;
       this.panelLocation       = panelLocation;
       this.parentUnit          = parentUnit;
       this.baseImage           = baseImage;
       this.hilightImage        = hilightImage;
       this.baseLocation        = baseLocation;
       this.baseRectangle       = baseRectangle;
       this.optionPopupMenu     = optionPopupMenu;
       this.descr               = descr;
       this.clickAction         = clickAction;
       this.win                 = win;

       // if (DEBUG) System.out.println("Creating click object "+descr+" for unit "+parentUnit.getUnitID());
   }


   public void activateOption(ActionListener listener) 
   {
     // if (DEBUG) System.out.println("activating click object "+descr+" for unit "+parentUnit.getUnitID()+" at ("+baseLocation.getX()+","+baseLocation.getY()+")" );
     enableEvents(AWTEvent.MOUSE_EVENT_MASK);
     actionListener = AWTEventMulticaster.add(actionListener, listener);
   }

   public void deactivateOption(ActionListener listener) 
   {
     // if (DEBUG) System.out.println("deactivating click object "+descr+" for unit "+parentUnit.getUnitID()+" at ("+baseLocation.getX()+","+baseLocation.getY()+")" );
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
       // g.drawImage(hilightImage, 0, 0, this);
       // else
     // if (DEBUG) System.out.println("Painting click object "+descr+" for unit "+parentUnit.getUnitID()+" at ("+baseLocation.getX()+","+baseLocation.getY()+")" );

     if (parentUnit.getCurrentPanel() == panelLocation)   // EG, if this interface is on the front panel but we are looking at the back panel
     {
       try
       {
         if (entered) 
           g.drawImage(hilightImage, 0, 0, this);
         else
           g.drawImage(baseImage, 0, 0, this);
       }
       catch (Throwable bi)
       {
         System.out.println("clickOption paint() : Error drawing baseImage for clickOption");
       }

       /* bounds and location must be set here inside paint() 
          height and width are hard coded for now   */
       // if (DEBUG) System.out.println("Setting bounds for "+descr+" for unit "+parentUnit.getUnitID()+" at ("+baseLocation.x+","+baseLocation.y+") to ("+baseRectangle.width+","+baseRectangle.height+")" );
       setBounds(baseLocation.x, baseLocation.y, baseRectangle.width, baseRectangle.height);
       setLocation(baseLocation);
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
            // render myself inverted....
            entered = true;
            // if (DEBUG) System.out.println("mouse entered for "+descr);
	    setCursor(new Cursor(Cursor.HAND_CURSOR));
            // this.showStatus(descr);
	    // Repaint might flicker a bit. To avoid this, you can use
	    // double buffering (see the Gauge example).
	    repaint();
            break;
          
          case MouseEvent.MOUSE_RELEASED:
            switch (type)
            {
            case OPTION_TYPE_TOGGLE:
              // if (DEBUG) System.out.println("mouse released for "+descr);
              parentApplet.toggleUnitPanel(parentUnit);
              // this.optionClicked = true;
              // repaint(type);
              break;

            default: 
              System.out.println("Clicked on unknown option " );
              break;
            }

            break;

          case MouseEvent.MOUSE_PRESSED:
            break;
          
          case MouseEvent.MOUSE_EXITED:
            if(entered == true) 
            {
               entered = false;

	        // Repaint might flicker a bit. To avoid this, you can use
	        // double buffering (see the Gauge example).
	       repaint();
            }
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
       return parentUnit.getUnitID();
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

   public Image getHilightImage() 
   {
       return hilightImage;
   }

   public Point getBaseLocation() 
   {
       return baseLocation;
   }

   public Rectangle getBaseRectangle() 
   {
       return baseRectangle;
   }

   public boolean getOptionClicked() 
   {
       return optionClicked;
   }

   public void clearOptionClicked() 
   {
       optionClicked = false;
   }

   public String getDescription() 
   {
       return descr;
   }

}
