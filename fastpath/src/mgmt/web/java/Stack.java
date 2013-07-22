
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


public class Stack
{
   private int numUnits;
   private int numSupportedUnits;
   private int state;
   private boolean stackingEnabled;
   private Image stackBackground;
   private Unit[] units;
   private boolean DEBUG = false;

   Stack (int numSupportedUnits, int state, boolean stackingEnabled, Image stackBackground)
   {
       this.numUnits            = 0;
       this.numSupportedUnits   = numSupportedUnits;
       this.state               = state;
       this.stackingEnabled     = stackingEnabled;
       this.stackBackground     = stackBackground;
       this.units               = new Unit[numSupportedUnits];

       for (int i=0; i<numSupportedUnits; i++)
         units[i] = null;
   }


   // get methods to return all the private vars
   public int getNumUnits() 
   {
       return numUnits;
   }

   public int getNumSupportedUnits() 
   {
       return numSupportedUnits;
   }

   public Unit getMgmtUnit() 
   {
     for (int i=0; i<numSupportedUnits; i++)
     {
       if ((units[i] != null) && (units[i].getState() == 1))  // TRGTRG Need more def'n of states
       {
         return units[i];
       }
     }
     return null;
   }

   public Unit getUnit(int unitID)  
   {
     for (int i=0; i<numSupportedUnits; i++)
     {
       if ((units[i] != null) && (units[i].getUnitID() == unitID))
       {
         return units[i];
       }
     }
     return null;
   }

   public Unit getFirstUnit()  
   {
     for (int i=0; i<numSupportedUnits; i++)
     {
       if (units[i] != null)
       {
         // if (DEBUG) System.out.println("getFirstUnit() unitID of " + units[i].getUnitID() + " at index " + i);
         return units[i];
       }
     }
     return null;
   }

   public Unit getNextUnit(int unitID)  
   {
     for (int i=0; i<numSupportedUnits; i++)
     {
       if ((units[i] != null) && (units[i].getUnitID() == unitID))
       {
         if ((i+1) < numSupportedUnits)
         {
           for (int j=i+1; j<numSupportedUnits; j++)
           {
             if (units[j] != null)
             {
               // if (DEBUG) System.out.println("getNextUnit("+unitID+") unitID of " + units[j].getUnitID() + " at index " + j);
               return units[j];
             }
           }
         }
       }
     }
     return null;
   }

   public Unit getPrevUnit(int unitID)  
   {
     for (int i=0; i<numSupportedUnits; i++)
     {
       if ((units[i] != null) && (units[i].getUnitID() == unitID))
       {
         if (i > 0)
         {
           for (int j=i-1; j>=0; j--)
           {
             if (units[j] != null)
             {
               // if (DEBUG) System.out.println("getPrevUnit("+unitID+") unitID of " + units[j].getUnitID() + " at index " + j);
               return units[j];
             }
           }
         }
       }
     }
     return null;
   }

   public boolean unitExists(int unitID)  
   {
     for (int i=0; i<numSupportedUnits; i++)
     {
       if ((units[i] != null) && (units[i].getUnitID() == unitID))
       {
         return true;
       }
     }
     return false;
   }

   public int getState() 
   {
       return state;
   }

   public boolean getStackingEnabled() 
   {
       return stackingEnabled;
   }

   public Image getSystemBackground() 
   {
       return stackBackground;
   }

   public void setState(int newState)
   {
       state = newState;
   }

   public boolean addUnit(int unitID, Unit unitObj)  
   {
     for (int i=0; i<numSupportedUnits; i++)
     {
         if (units[i] == null)
         {
           units[i] = unitObj;
           numUnits++;
           // if (DEBUG) System.out.println("Added Unit " + unitID + " at index " + i);
           return true;
         }
     }
     return false;
   }

   public boolean removeUnit(int unitID)  
   {
     Interface portobj;
     for (int i=0; i<numSupportedUnits; i++)
     {
       if ((units[i] != null) && (units[i].getUnitID() == unitID))
       {
         units[i] = null;
         numUnits--;
         // if (DEBUG) System.out.println("Removing Unit " + unitID + " at index " + i);
         return true;
       }
     }
     return false;
   }

}
