
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


public class Unit
{
   private int unitID;
   private int offset_scale;
   private int numSlots;
   private int numSFPs;
   private int firstSlot;
   private int state;
   private int type;
   private int currentPanel;
   private int numStackIntf;
   private int firstStackIntfNum;
   private Image unitFrontImage;
   private Image unitBackImage;
   private Image indexImage;
   private Image preIndexImage;
   private int unitXLoc;
   private int unitYLoc;
   private Rectangle unitRectangle;
   private clickOption[] unitClickOption;
   private int toggleToFront;
   private int toggleToBack;
   private boolean toggleSupport;
   private Rectangle getNextUnit;
   private Rectangle getPrevUnit;
   private Interface servicePort;
   private Interface serialPort;
   private Interface[] sfps;
   private Card[] cards;
   private Interface[] stackIntf;
   private boolean flagged;
   private final static boolean DEBUG = false;
   private final static int MAX_CLICK_OPTIONS = 20;
   private int i;


   Unit (int unitID, int numSlots, int firstSlot, int numSFPs, int state, int type, int currentPanel, 
         Rectangle getNextUnit, Rectangle getPrevUnit, 
         int numStackIntf, int firstStackIntfNum, 
         Image unitFrontImage, Image unitBackImage, int unitXLoc, int unitYLoc, 
         Rectangle unitRectangle, Image indexImage, Image preIndexImage)
   {
       this.unitID                 = unitID;
       this.numSlots               = numSlots;
       this.numSFPs                = numSFPs;
       this.firstSlot              = firstSlot;
       this.state                  = state;
       this.type                   = type;
       this.currentPanel           = currentPanel;
       this.numStackIntf           = numStackIntf;
       this.firstStackIntfNum      = firstStackIntfNum;
       this.unitFrontImage         = unitFrontImage;
       this.unitBackImage          = unitBackImage;
       this.unitXLoc               = unitXLoc;
       this.unitYLoc               = unitYLoc;
       this.unitRectangle          = unitRectangle;
       this.toggleToBack           = -1;
       this.toggleToFront          = -1;
       this.toggleSupport          = false;
       this.getNextUnit            = getNextUnit;
       this.getPrevUnit            = getPrevUnit;
       this.indexImage             = indexImage;
       this.preIndexImage          = preIndexImage;
       this.cards                  = new Card[numSlots];
       this.stackIntf              = new Interface[numStackIntf];
       for (i=0; i<numStackIntf; i++)
       {
         stackIntf[i] = null;
       }
       this.unitClickOption        = new clickOption[MAX_CLICK_OPTIONS];
       for (i=0; i<MAX_CLICK_OPTIONS; i++)
       {
         unitClickOption[i] = null;
       }
       this.servicePort            = null;
       this.sfps                   = new Interface[numSFPs];
       this.serialPort             = null;
       this.flagged                = true;

       // if (DEBUG) System.out.println("Unit.java: Creating unit "+unitID+" at ("+unitXLoc+","+unitYLoc+")" );
   }


   // get methods to return all the private vars
   public int getUnitID() 
   {
       return unitID;
   }

   public Card getCard(int slotNum)  
   {
     int index = slotNum - firstSlot;
     if ((cards[index] != null) && (index >=0 ) && (index < numSlots))
       return cards[index];
     else
       return null;
   }

   public boolean cardExists(int slotNum)  
   {
     for (int i=firstSlot; i<(numSlots+firstSlot); i++)
     {
       if ((cards[i] != null) && (cards[i].getState() == 1))
       {
         if (cards[i].getSlot() == slotNum)
         {
           return true;
         }
       }
     }
     return false;
   }

   public Interface getFirstSFPInterface()  
   {
       for (int i=0; i<numSFPs; i++) 
       {
         if (sfps[i] != null) 
         {
             return sfps[i];
         }
       }
       return null;
   }

   public Interface getNextSFPInterface(Interface sfpObj)  
   {
       for (int i=0; i<numSFPs; i++) 
       {
         if (sfps[i] == sfpObj) 
         {
           if (((i+1) < numSFPs) && (sfps[i+1] != null))
             return sfps[i+1];
           else
             return null;
         }
       }
       return null;
   }

   public Interface getSFPInterface(int slotNum, int portNum)  
   {
       for (int i=0; i<numSFPs; i++) 
       {
         if ((sfps[i].getSlot() == slotNum) && (sfps[i].getPort() == portNum)) 
         {
             return sfps[i];
         }
       }
       return null;
   }

   public Interface getServicePort()  
   {
     return servicePort;
   }

   public Interface getSerialPort()  
   {
     return serialPort;
   }

   public int getFirstStackIntfNum() 
   {
     return firstStackIntfNum;
   }

   public Interface getStackIntf(int stackIntfNum)  
   {
     int index = stackIntfNum - firstStackIntfNum;
     if ((index >=0 ) && (index < numStackIntf))
       return stackIntf[index];
     else
       return null;
   }

   public int getCurrentPanel() 
   {
       return currentPanel;      // Front or back
   }

   public int getNumCards() 
   {
       return numSlots;
   }

   public int getFirstCard() 
   {
       return firstSlot;
   }

   public int getNumSFPs() 
   {
       return numSFPs;
   }

   public int getState() 
   {
       return state;
   }

   public int getType() 
   {
       return type;
   }

   public int getNumStackIntf() 
   {
       return numStackIntf;
   }

   public Image getUnitFrontImage() 
   {
       return unitFrontImage;
   }

   public Image getUnitBackImage() 
   {
       return unitBackImage;
   }

   public Image getIndexImage() 
   {
       return indexImage;
   }

   public Image getPreIndexImage() 
   {
       return preIndexImage;
   }

   public int getUnitXOffset() 
   {
       return unitXLoc;
   }

   public int getUnitYOffset() 
   {
       return unitYLoc;
   }

   public Rectangle getUnitRectangle() 
   {
       return unitRectangle;
   }

   public int getUnitWidth() 
   {
       return (int)unitRectangle.getWidth();
   }

   public int getUnitHeight() 
   {
       return (int)unitRectangle.getHeight();
   }
      
   public Rectangle getUnitNextGet() 
   {
       return getNextUnit;
   }

   public Rectangle getUnitPrevGet() 
   {
       return getPrevUnit;
   }

   public boolean getFlagged() 
   {
       return flagged;
   }

   public void setState(int newState)
   {
       state = newState;
   }

   public void addCard(int slotNum, Card cardObj)  
   {
     int index = slotNum - firstSlot;
     if ((index >=0 ) && (index < numSlots))
       cards[index] = cardObj;
   }

   public void addSFPInterface(Interface sfpObj)  
   {
     for (int i=0; i<numSFPs; i++) 
     {
       if (sfps[i] == null) 
       {
           sfps[i] = sfpObj;
           break;
       }
     }
   }

   public void addServicePort(Interface servicePortObj)  
   {
     servicePort = servicePortObj;
   }

   public void addSerialPort(Interface serialPortObj)  
   {
     serialPort = serialPortObj;
   }

   public clickOption getClickObject(int index)  
   {
     return unitClickOption[index];
   }

   public int getClickObjectIndexFirst()  
   {
     int j = 0;

     while (j < MAX_CLICK_OPTIONS)
     {
       if (unitClickOption[j] != null)
         break;
       j++;
     }
     if (j == MAX_CLICK_OPTIONS)
       j = -1;

     return j;
   }

   public int getClickObjectIndexNext(int index)  
   {
     int j = index+1;

     while (j < MAX_CLICK_OPTIONS)
     {
       if (unitClickOption[j] != null)
         break;
       j++;
     }
     if (j == MAX_CLICK_OPTIONS)
       j = -1;

     return j;
   }

   public int addClickObject(clickOption objClick)  
   {
     int j = 0;

     while (j < MAX_CLICK_OPTIONS)
     {
       if (unitClickOption[j] == null)
       {
         unitClickOption[j] = objClick;
         // if (DEBUG) System.out.println("Added click object "+objClick.getDescription()+" to unit "+unitID+" at position "+j);
         break;
       }
       j++;
     }
     if (j == MAX_CLICK_OPTIONS)
     {
       j = -1;
       System.out.println("Error - failed to add click object "+objClick.getDescription()+" to unit "+unitID);
     }
     return j;
   }

   public void addToggleToBack(clickOption toggleOption)  
   {
     toggleToBack = addClickObject(toggleOption);
   }

   public void addToggleToFront(clickOption toggleOption)  
   {
     toggleToFront = addClickObject(toggleOption);
   }

   public void setToggleSupport(boolean flag)
   {
       toggleSupport = flag;
   }

   public clickOption getToggleToBack()  
   {
     if (toggleToBack >= 0)
       return unitClickOption[toggleToBack];
     else
       return null;
   }

   public clickOption getToggleToFront()  
   {
     if (toggleToFront >= 0)
       return unitClickOption[toggleToFront];
     else
       return null;
   }

   public boolean getToggleSupport()
   {
       return toggleSupport;
   }

   public void removeCard(int slotNum)  
   {
     int index = slotNum - firstSlot;
     if ((index >=0 ) && (index < numSlots))
       cards[index] = null;
   }

   public void removeSFPInterface(int slotNum, int portNum)  
   {
       for (int i=0; i<numSFPs; i++) 
       {
         if ((sfps[i].getSlot() == slotNum) && (sfps[i].getPort() == portNum)) 
         {
             sfps[i] = null;
             break;
         }
       }
   }

   public void removeSerialPort()  
   {
     serialPort = null;
   }

   public void removeServicePort() 
   {
     servicePort = null;
   }

   public void addStackIntf(int stackIntfNum, Interface newStackIntf)  
   {
     int index = stackIntfNum - firstStackIntfNum;
     if ((index >=0 ) && (index < numStackIntf))
       stackIntf[index] = newStackIntf;
   }

   public void removeStackIntf(int stackIntfNum)  // not sure we need this
   {
     int index = stackIntfNum - firstStackIntfNum;
     if ((index >=0 ) && (index < numStackIntf))
       stackIntf[index] = null;
   }

   public void setCurrentPanel(int newPanel) 
   {
       currentPanel = newPanel;     // Front or back
   }

   public void setFlagged(boolean flag)
   {
       this.flagged = flag;
   }

}
