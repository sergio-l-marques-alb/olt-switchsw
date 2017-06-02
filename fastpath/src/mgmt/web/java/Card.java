
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename Data.java
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

import java.awt.*;


public class Card
{
   private int unit;
   private int slot;
   private int numPorts;
   private int firstPort;
   private int state;
   private int pluggable;
   private int type;
   private int panelLocation;
   private Image slotImage;
   private int slotXOffset;
   private int slotYOffset;
   private Point slotLocation;
   private Rectangle slotRectangle;
   private Interface[] ports;


   Card (int unit, int slot, int numPorts, int firstPort, int state, int pluggable, int type,  
         int panelLocation, Image slotImage, int slotXOffset, int slotYOffset, 
         Point slotLocation, Rectangle slotRectangle)
   {
       this.unit                = unit;
       this.slot                = slot;
       this.numPorts            = numPorts;
       this.firstPort           = firstPort;
       this.state               = state;
       this.pluggable           = pluggable;
       this.type                = type;
       this.panelLocation       = panelLocation;
       if (pluggable == 1)   /* Pluggable */
       {
         this.slotImage         = slotImage;
         this.slotLocation      = slotLocation;
         this.slotRectangle     = slotRectangle;
         this.slotXOffset       = slotXOffset;
         this.slotYOffset       = slotYOffset;
       }                     
       this.ports               = new Interface[numPorts];
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

   public Interface getPort(int portNum)  
   {
     int index = portNum - firstPort;
     if ((index >=0 ) && (index < numPorts))
       return ports[index];
     else
       return null;
   }

   public int getNumPorts() 
   {
       return numPorts;
   }

   public int getFirstPort() 
   {
       return firstPort;
   }

   public int getState() 
   {
       return state;
   }

   public int getPluggable() 
   {
       return pluggable;
   }

   public int getType() 
   {
       return type;
   }

   public int getPanelLocation() 
   {
       return panelLocation;
   }

   public Image getSlotImage() 
   {
       return slotImage;
   }

   public int getXOffset() 
   {
       return slotXOffset;
   }

   public int getYOffset() 
   {
       return slotYOffset;
   }

   public Point getSlotLocation() 
   {
       return slotLocation;
   }

   public Rectangle getSlotRectangle() 
   {
       return slotRectangle;
   }

   public void setState(int newState)
   {
       state = newState;
   }

   public void addPort(int portNum, Interface portObj)  
   {
     int index = portNum - firstPort;
     if ((index >=0 ) && (index < numPorts))
       ports[index] = portObj;
   }

   public void removePort(int portNum)  // not sure we need this
   {
     int index = portNum - firstPort;
     if ((index >=0 ) && (index < numPorts))
       ports[index] = null;
   }


}
